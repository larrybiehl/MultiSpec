//	 										MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SWindow.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			03/19/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Functions in file:	void 			RemoveWindowFromList
//								void			UpdateWindowList
//
//	Brief description:	This file contains functions that operate on image and
//								output windows.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 

#if defined multispec_mac
	#include	"MGraphView.h"
#endif	// defined multispec_mac 
  
#if defined multispec_win
	#include	"WGraphView.h"
	extern SInt16 StringWidth (UCharPtr     	stringPtr);
#endif	// defined multispec_win    

//#include "SExtGlob.h"


		// Prototype descriptions for routines in this file that are only		
		// called by routines in this file.




//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SaveIfWindowChanged
//
//	Software purpose:	The purpose of this routine is to check if the input
//							window information has been changed and needs to be
//							saved. The user is given the chance to save the changed
//							information.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	TRUE if operation is cancelled
//							FALSE if operation was not cancelled			
// 
// Called By:			ExitMultiSpec in multispec.c
//							CloseImageWindow in window.c
//
//	Coded By:			Larry L. Biehl			Date: 11/13/1996
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

Boolean SaveIfWindowChanged (
				Handle								windowInfoHandle,
				Boolean								forceCloseFlag)

{
	SInt16								alertID,
											windowType;
	
	
			// Check input variable; exit if it is NULL	
	
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
			
	if (windowInfoPtr == NULL)												
																							return (FALSE); 
																						
	windowType = windowInfoPtr->windowType;
	
	alertID = kSaveAlertID;
	if (forceCloseFlag)
		alertID = kSaveDontSaveAlertID;
	
	if (windowType == kThematicWindowType)
		{
				// Check if class and/or group tables have not been saved. If not
				// allow the user a chance to save them.
				
				// Indicate that this will be same as if the user selected save
				// class or group info from the File menu.
				
		gProcessorCode = kSaveProcessor;
				
		Handle fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
		FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (
																				fileInfoHandle, kLock);
													
 		GetCopyOfPFileNameFromFileInfo (fileInfoPtr, (FileStringPtr)gTextString3);
		#if defined multispec_win
			USES_CONVERSION;
				// Convert unicode to char string
			T2A((LPCWSTR)gTextString3);
		#endif
		
		SInt16 itemHit = 0;
			
		if (fileInfoPtr->groupChangedFlag && fileInfoPtr->bandInterleave <= kBIS)
			{
			if (LoadSpecifiedStringNumberStringP (kAlertStrID,
																IDS_Alert51, 
																(char*)gTextString,
																(char*)gTextString2,
																TRUE,
																(char*)&gTextString3[1]))
				itemHit = DisplayAlert (kSaveAlertID, kStopAlert, 0, 0, 0, gTextString);
			
			if (itemHit == 1)
				WriteThematicInfo (windowInfoHandle, kGroupDisplay);
				
			if (itemHit != 2)
				fileInfoPtr->groupChangedFlag = FALSE;
			
			}	// end "if (fileInfoPtr->groupChangedFlag && ..."
													
		if (itemHit != 2 && fileInfoPtr->classChangedFlag)
			{
			if (LoadSpecifiedStringNumberStringP (kAlertStrID,
																IDS_Alert50,
																(char*)gTextString,
																(char*)gTextString2,
																TRUE,
																(char*)&gTextString3[1]))
				itemHit = DisplayAlert (kSaveAlertID, kStopAlert, 0, 0, 0, gTextString);
			
			if (itemHit == 1)
				WriteThematicClassesAs (windowInfoHandle, fileInfoPtr);
				
			if (itemHit != 2)
				fileInfoPtr->classChangedFlag = FALSE;
			
			}	// end "if (fileInfoPtr->classChangedFlag)"
			
		gProcessorCode = 0;
			
		if (itemHit == 2 && !forceCloseFlag)
																							return (TRUE);
																				
		}	// end "if (windowType == kThematicWindowType)"
																				
	return (FALSE);
			
}	// end "SaveIfWindowChanged" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void RemoveWindowFromList
//
//	Software purpose:	The purpose of this routine is to remove the input
//							image or graphics window from the window menu list.
//
//	Parameters in:		Window Pointer
//							Window Menu Item Number
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			CloseImageWindow in window.c
//							CloseGraphicsWindow in window.c
//
//	Coded By:			Larry L. Biehl			Date: 07/20/92
//	Revised By:			Larry L. Biehl			Date: 06/29/2001	

void RemoveWindowFromList (
				WindowPtr							window, 
				SInt16								windowMenuItem)

{
	SInt16								count,
											loop,
											numberOfWindows,
											windowIndex; 
								
	            
	#if defined multispec_mac                         
		if (windowMenuItem <= 0)  
																										return;
	#endif	// defined multispec_mac
	
	if (window != NULL)
		{
				// Move the rest of the window menu items up.							
				
		numberOfWindows = gNumberOfIWindows + gNumberOfGWindows;
		
		#if defined multispec_mac
					// Delete the window name from the window menu							
				
			DeleteMenuItem (gMultiSpecMenus[kWindowM], windowMenuItem);
		#endif	// defined multispec_mac
		
				// Update application window list and window menu item number for	
				// the open windows																
		
		windowIndex = 0;		
		for (loop=1; loop<=numberOfWindows; loop++)
			{
			if (window == gWindowList[loop+2]) 
				{
				windowIndex = loop;
				break;
				
				}	// end  "if (window == gWindowList[loop+2]) "
				
			}	// end "for (loop=1; loop<=numberOfWindows; loop++)"
			
				// Update application global window count
				
		if (windowIndex > gNumberOfIWindows)
			gNumberOfGWindows--;
		
		else	// windowIndex <= gNumberOfIWindows
			gNumberOfIWindows--;	
			
		count = windowIndex;
		if (count < numberOfWindows)
			{
			for (loop=count; loop<numberOfWindows; loop++)
				{
				gWindowList[loop+2] = gWindowList[loop+3];
				#if defined multispec_mac
					Handle windowInfoHandle = (Handle)GetWRefCon (gWindowList[loop+2]);
					((WindowInfoPtr)*windowInfoHandle)->windowMenuItem  = 
																				loop + kStatisticsWItem;
				#endif	// defined multispec_mac
																
				}	// end "for (loop=count; ..." 
				
			}	// end "if (count < numberOfWindows)" 
			
		gWindowList[numberOfWindows+2] = NULL;
		
		}	// end "if (window != NULL)" 
			
}	// end "RemoveWindowFromList" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateLegendParameters
//
//	Software purpose:	The purpose of this routine is to update the legend
//							parameters after a change in the legend width.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ChangeLegendWidth in window.c
//							DisplayThematicDialogOK in SDisThem.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/27/1996
//	Revised By:			Larry L. Biehl			Date: 04/27/2016	

void UpdateLegendParameters (
				Handle								windowInfoHandle,
				SInt16								legendWidth)

{			      
	#if defined multispec_mac || defined multispec_mac_swift
		unsigned char*					legendTitle1Ptr = (unsigned char*)"\pGroups/Classes"; 
	#endif	// defined multispec_mac || defined multispec_mac_swift   

	#if defined multispec_win  
		unsigned char*					legendTitle1Ptr = NULL; 
	#endif		// defined multispec_win   
		
	#if defined multispec_lin  
		 unsigned char*				legendTitle1Ptr = NULL;
	#endif		
	
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
			
	windowInfoPtr->legendTitleWidth = MAX (103, StringWidth (legendTitle1Ptr));
	SInt16 legendStart = MAX (4, legendWidth - windowInfoPtr->legendTitleWidth - 13);
	//windowInfoPtr->legendTopStart = 1;
	windowInfoPtr->legendTopStart = (SInt16)windowInfoPtr->coordinateHeight + 1;
	windowInfoPtr->legendLeftStart = 0 + legendStart/2 - 4;
	windowInfoPtr->legendWidth = legendWidth;	

}	// end "UpdateLegendParameters"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateWindowList
//
//	Software purpose:	The purpose of this routine is to update the global window
//							list. For the Macintosh version this will be the list
//							of window pointers. For the Windows and Linux versions 
//							this will be the list of view class pointers.
//		
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 01/23/1996
//	Revised By:			Larry L. Biehl			Date: 09/08/2006

void UpdateWindowList (
				WindowPtr							theWindowPtr,
				SInt16								windowType)
                     
{
	SInt16			index;
	
	#if defined multispec_mac
		Handle								tempHandle;
		SInt16								windowMenuItem;	
	#endif	// defined multispec_mac
				                 
	
	if (windowType != kGraphicsWindowType)
		{
				// Add new image window to window list.  Insert the new image		
				// window before the graphics windows.										
		
		gNumberOfIWindows++;
		
				// Make space for the image window in the list.  All image 			
				// windows are kept together.													
				
		if (gNumberOfGWindows > 0)
			{
			for (index=gNumberOfIWindows+gNumberOfGWindows+2; 
					index>gNumberOfIWindows+2; 
					index--)
				{
				gWindowList[index] = gWindowList[index-1];
				
				#if defined multispec_mac
					tempHandle = (Handle)GetWRefCon (gWindowList[index]);
					((WindowInfoPtr)*tempHandle)->windowMenuItem  = 
																		index + kStatisticsWItem - 2;
				#endif	// defined multispec_mac
																
				}	// end "for (index=gNumberOfIWindows ..." 
				
			}	// end "if (gNumberOfGWindows > 0)" 
			
		gWindowList[gNumberOfIWindows+2] = theWindowPtr;
				
		#if defined multispec_mac
			windowMenuItem = gNumberOfIWindows + kStatisticsWItem;
		#endif	// defined multispec_mac
		
		}	// end "if (windowType != kGraphicsWindowType)"
		
	else	// windowType == kGraphicsWindowType
		{
		gNumberOfGWindows++;
		gWindowList[gNumberOfIWindows+gNumberOfGWindows+2] = theWindowPtr;
		
		#if defined multispec_mac
			windowMenuItem = gNumberOfIWindows + gNumberOfGWindows + kStatisticsWItem;
		#endif	// defined multispec_mac
		
		}	// end "else windowType == kGraphicsWindowType"
		
	#if defined multispec_mac
		tempHandle = (Handle)GetWRefCon (theWindowPtr);
		((WindowInfoPtr)*tempHandle)->windowMenuItem  = windowMenuItem;
	#endif	// defined multispec_mac
		
}	// end "UpdateWindowList" 
