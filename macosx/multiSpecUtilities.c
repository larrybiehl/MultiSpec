//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2015)
//							� Purdue Research Foundation
//									All rights reserved.
//
//	File:						multiSpecUtilities.c
//
//	Authors:					Larry L. Biehl, Ravi Budruk
//
//	Revision number:		2.8
//
//	Revision date:			01/09/2015
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	The purpose of the routines in this file is to
//								provide utility type functions in MultiSpec.
//
//	Functions in file:	Boolean 				ButtonDownEvent
//								Boolean				ListSpecifiedString
//								void 					UpdateDeskScrap

//#include 	"SMulSpec.h"
//#include 	"SExtGlob.h"

#include		"ctype.h"
						

extern void						WriteInterfacePreferenceValue (
										Boolean								fullVersionFlag);


			// Prototypes for routines in this file that are only called by		
			// other routines in this file.


		
/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		AppendPascaltoPascal
//
//	Software purpose:	This routine appends a pascal string to the end
//							of a pascal string
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/04/1988
//	Revised By:			Larry L. Biehl			Date: 11/04/1988

void AppendPascaltoPascal (
				UCharPtr								appendStringPtr, 
				char*									stringPtr)

{
	SInt16								lengthString,
											lengthAppendString;
	
	
	lengthString = *stringPtr;
	lengthAppendString = *appendStringPtr;
	
	*stringPtr += lengthAppendString;
	 
	stringPtr = &stringPtr[++lengthString];
	appendStringPtr++;
	
	BlockMoveData ((char*)appendStringPtr, stringPtr, lengthAppendString);

}		// end "AppendPascaltoPascal" 
*/
	

//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ButtonDownEvent
//
//	Software purpose:	This routine handles mouse down events in specified
// 						rectangles.  The rectangle is inverted and a check is 
//							made to determine if the mouse was still within the 
//							rectangle when the mouse up event occurs.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	TRUE if mouse still within the control rectangle
//							FALSE if not
// 
// Called By:			UpdateLineColumnValues in dialogUtilities.c
//							StatisticsWControlEvent in statistics.c
//
//	Coded By:			Larry L. Biehl			Date:11/28/1988
//	Revised By:			Larry L. Biehl			Date:06/10/1993			

Boolean ButtonDownEvent (
				Rect*									rectPtr, 
				Boolean								waitOnMouseUpFlag)

{
	Point									presentPt;
	
	Boolean								lastPointInRectFlag,
											returnFlag;
	
			
	returnFlag = TRUE;
	
			// Invert the rectangle.															
			
	InvertRect ( rectPtr );
	
	if (waitOnMouseUpFlag)
		{
				// Wait until button goes up													
		
		do
			{
			GetMouse (&presentPt);
			lastPointInRectFlag = PtInRect (presentPt, rectPtr);
			
			if ( (lastPointInRectFlag && !returnFlag) || 
													(!lastPointInRectFlag && returnFlag) )
				{
				InvertRect ( rectPtr );
				returnFlag = !returnFlag;
				
				}		// end "if ( PtInRect (presentPt, rectPtr) && ..." 
			
			} while ( StillDown() );
			
				// Clear the event of the mouse up event.									
				
		FlushEvents (mUpMask,0);
						
		}		// end "if (waitOnMouseUpFlag)" 
		
	else		// !waitOnMouseUpFlag 
		Delay (7, (UInt32*)&gNextTime);
		
			// Invert the the rectangle that encloses the button again to make	
			// it normal.																			
	
	if (returnFlag)		
		InvertRect ( rectPtr );
																				
	return (returnFlag);

}		// end "ButtonDownEvent" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListSpecifiedString
//
//	Software purpose:	This routine lists the specified string in the
//							resource file to the output text window and/or
//							disk file.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/17/1993
//	Revised By:			Larry L. Biehl			Date: 02/17/1993			

Boolean ListSpecifiedString (
				SInt16								stringID, 
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								outputCode )
													
{
	char* 								stringPtr;
	
	SInt32								stringLength;
	StringHandle						stringHandle;
	
	Boolean								continueFlag;


	stringLength = GetSpecifiedString (stringID, &stringHandle, &stringPtr);
			
	continueFlag = (stringLength > 0);
	if (stringHandle)
		{
		continueFlag = OutputString ( resultsFileStreamPtr, 
												stringPtr, 
												stringLength, 
												outputCode, 
												TRUE);
	
		HUnlock ( (Handle)stringHandle );
		ReleaseResource ( (Handle)stringHandle );
		
		}		// end "if (stringHandle)" 
		
	return (continueFlag);

}		// end "ListSpecifiedString"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MGetPixRowBytes
//
//	Software purpose:	The purpose of this routine is to get the number of pix row
//							bytes in the input pix map handle. This routine has to take
//							into account whether this is a carbon application or not.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/02/2001
//	Revised By:			Larry L. Biehl			Date: 11/02/2001

SInt32 MGetPixRowBytes (
				PixMapHandle 						pixMapHandle)

{
	SInt32								longPixRowBytes = 0;
	
	if (pixMapHandle != NULL)
		{
		#if TARGET_API_MAC_CARBON
			longPixRowBytes = GetPixRowBytes (pixMapHandle);
		#else		// !TARGET_API_MAC_CARBON
			UInt16			rowBytes = 0;
			
			if (pixMapHandle != NULL)
				{
				rowBytes = (**pixMapHandle).rowBytes;
				BitClr (&rowBytes, 0);
				BitClr (&rowBytes, 1);
				
				}		// end "if (pm != NULL)"
				
			longPixRowBytes = rowBytes;
		#endif	// TARGET_API_MAC_CARBON, else...	
		
		}		// end "if (pixMapHandle != NULL)"
		
	return (longPixRowBytes);

}		// end "MGetPixRowBytes"
	

/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SetUpMultiSpecMenu
//
//	Software purpose:	This routine sets up the MultiSpec menu based on whether the full
//							of capabilites is requested or a reduced set is desired.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	TRUE if mouse still within the control rectangle
//							FALSE if not
// 
// Called By:			UpdateLineColumnValues in dialogUtilities.c
//							StatisticsWControlEvent in statistics.c
//
//	Coded By:			Larry L. Biehl			Date: 12/05/2014
//	Revised By:			Larry L. Biehl			Date: 12/05/2014			

Boolean SetUpMultiSpecMenu (
				Boolean								fullSetFlag)

{
	Boolean							returnFlag = TRUE;
	
	
	if (fullSetFlag)
		{
				// Build the menus																	 
		
		gMultiSpecMenus[kAppleM] = GetMenu (kAppleMenuID);
		if (gMultiSpecMenus[kAppleM] == NULL)										
																						ExitToShell ();
		#if !TARGET_API_MAC_CARBON	
			AppendResMenu (gMultiSpecMenus[kAppleM], (ResType)'DRVR');
		#endif	// !TARGET_API_MAC_CARBON	
		InsertMenu (gMultiSpecMenus[kAppleM], 0);		// insert at end of list 
		
				// Build the 'File' Menu.															
				
		gMultiSpecMenus[kFileM] = GetMenu (kFileMenuID);
		if (gMultiSpecMenus[kFileM] == NULL)										
																						ExitToShell ();
		InsertMenu (gMultiSpecMenus[kFileM], 0);			// insert at end of list 
		DisableMenuItem (gMultiSpecMenus[kFileM], kNewProject);
		DisableMenuItem (gMultiSpecMenus[kFileM], kOpenProjImage);
		DisableMenuItem (gMultiSpecMenus[kFileM], kOpenThematicClassGroup);
		DisableMenuItem (gMultiSpecMenus[kFileM], kCloseImage);
		
		EnableMenuItem (gMultiSpecMenus[kFileM], kSaveTransformMenuItem);
		
		DisableMenuItem (gMultiSpecMenus[kFileM], kSave);
		DisableMenuItem (gMultiSpecMenus[kFileM], kSaveOutputAs);
		DisableMenuItem (gMultiSpecMenus[kFileM], kSaveProject);
		DisableMenuItem (gMultiSpecMenus[kFileM], kSaveProjectAs);
		DisableMenuItem (gMultiSpecMenus[kFileM], kPrintOutput);
		EnableMenuItem  (gMultiSpecMenus[kFileM], kQuit);
		
		if (gOSXFlag)
			{
					// Allow for Aqua interface.
					// Delete the quit and quit separator from the File menu.
					
			DeleteMenuItem(gMultiSpecMenus[kFileM], kQuit);
			DeleteMenuItem(gMultiSpecMenus[kFileM], kQuit-1);
			
					// Indicate that file names can be up to 254 characters.
					
			gFileNameLengthLimit = 254;
			
			}		// end "if (gOSXFlag)"
		
				// Build the 'Edit' Menu.															
		
		gMultiSpecMenus[kEditM] = GetMenu (kEditMenuID);
		if (gMultiSpecMenus[kEditM] == NULL)										
																						ExitToShell ();
		InsertMenu (gMultiSpecMenus[kEditM], 0);			// insert at end of list 
		DisableMenuItem (gMultiSpecMenus[kEditM], kUndo);
		DisableMenuItem (gMultiSpecMenus[kEditM], kCut);
		DisableMenuItem (gMultiSpecMenus[kEditM], kCopy);
		DisableMenuItem (gMultiSpecMenus[kEditM], kPaste);
		DisableMenuItem (gMultiSpecMenus[kEditM], kClear);
		DisableMenuItem (gMultiSpecMenus[kEditM], kSelectAll);
		DisableMenuItem (gMultiSpecMenus[kEditM], kEditSelectionRect);
		DisableMenuItem (gMultiSpecMenus[kEditM], kClearSelectionRect);
		DisableMenuItem (gMultiSpecMenus[kEditM], kClearTransformMenuItem);
		DisableMenuItem (gMultiSpecMenus[kEditM], kChangeSpecs);
		DisableMenuItem (gMultiSpecMenus[kEditM], kCoordinates);	
		DisableMenuItem (gMultiSpecMenus[kEditM], kClearOverlays);									
		
		gMultiSpecMenus[kOverlaysM] = GetMenu (kClearOverlaysMenuID);
		if (gMultiSpecMenus[kOverlaysM] == NULL)										
																						ExitToShell ();
		InsertMenu (gMultiSpecMenus[kOverlaysM], -1);			// insert submenu in	  
																			// edit menu		 
		
		gMultiSpecMenus[kProjectM] = GetMenu (kProjectMenuID);
		if (gMultiSpecMenus[kProjectM] == NULL)										
																						ExitToShell ();
		InsertMenu (gMultiSpecMenus[kProjectM], 0);		// insert at end of list 
		DisableMenuItem (gMultiSpecMenus[kProjectM], kUseBaseStatistics);
		DisableMenuItem (gMultiSpecMenus[kProjectM], kUseLOOStatistics);
		DisableMenuItem (gMultiSpecMenus[kProjectM], kUseEnhancedStatistics);
		DisableMenuItem (gMultiSpecMenus[kProjectM], kMixedStatisticsUsed);
		DisableMenuItem (gMultiSpecMenus[kProjectM], kClearStatistics);
		DisableMenuItem (gMultiSpecMenus[kProjectM], kChangeBaseImageFile);
		DisableMenuItem (gMultiSpecMenus[kProjectM], kAssociatedImage);
		
		gMultiSpecMenus[kProcessorM] = GetMenu (kProcessorMenuID);
		if (gMultiSpecMenus[kProcessorM] == NULL)								
																						ExitToShell ();
		InsertMenu (gMultiSpecMenus[kProcessorM], 0);	// insert at end of list 
		EnableMenuItem (gMultiSpecMenus[kProcessorM], 0);
		DisableMenuItem (gMultiSpecMenus[kProcessorM], kDisplaysMItem);
		DisableMenuItem (gMultiSpecMenus[kProcessorM], kHistogramMItem);
		DisableMenuItem (gMultiSpecMenus[kProcessorM], kListDataMenuItem);
		EnableMenuItem (gMultiSpecMenus[kProcessorM], kReformatMItem);
		DisableMenuItem (gMultiSpecMenus[kProcessorM], kClusterMItem);
		DisableMenuItem (gMultiSpecMenus[kProcessorM], kStatisticsMItem);
		DisableMenuItem (gMultiSpecMenus[kProcessorM], kStatEnhanceMItem);
		DisableMenuItem (gMultiSpecMenus[kProcessorM], kFeatureExtractionMItem);
		DisableMenuItem (gMultiSpecMenus[kProcessorM], kSeparabilityMItem);
		DisableMenuItem (gMultiSpecMenus[kProcessorM], kClassifyMItem);
		DisableMenuItem (gMultiSpecMenus[kProcessorM], kListResultsMItem);
		EnableMenuItem (gMultiSpecMenus[kProcessorM], kUtilitiesMItem);

				// Add the reformat submenu.									
		
		gMultiSpecMenus[kReformatM] = GetMenu (kReformatMenuID);
		if (gMultiSpecMenus[kReformatM] == NULL)								
																						ExitToShell ();
		InsertMenu (gMultiSpecMenus[kReformatM], -1);			// insert submenu in	  
																			// processor menu		 
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatChangeHeaderMenuItem);
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatChangeImageMenuItem);
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertMultispectralMenuItem);
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertProjectMenuItem);
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertShapeMenuItem);
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatModifyChannelMenuItem);
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatMosaicImagesMenuItem);
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatRecodeThematicMenuItem);
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatRectifyImageMenuItem);
		EnableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertENVIASCIIMenuItem);

				// Add the utilities submenu.														
				
	//	SetItemCmd  (gMultiSpecMenus[kProcessorM], kUtilitiesMItem, cmdChar);
	//	SetItemMark (gMultiSpecMenus[kProcessorM], kUtilitiesMItem, markCharUtilities);
		gMultiSpecMenus[kUtilitiesM] = GetMenu (kUtilitiesMenuID);
		if (gMultiSpecMenus[kUtilitiesM] == NULL)								
																						ExitToShell ();
		InsertMenu (gMultiSpecMenus[kUtilitiesM], -1);			// insert submenu in	  
																			// processor menu		 
		DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kPrincipalComponentsMenuItem);
		DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kStatisticsImageMenuItem);
		DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kCovarianceCheckMenuItem);
		DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kBiPlotDataMenuItem);
		DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kListDescriptionMenuItem);
		
				// Get the options menu.															
		
		gMultiSpecMenus[kOptionM] = GetMenu (kOptionMenuID);
		if (gMultiSpecMenus[kOptionM] == NULL)									
																						ExitToShell ();
		InsertMenu (gMultiSpecMenus[kOptionM], 0);		// insert at end of list 
		
				// Remove the test menu item if requested.
												
		if (!gIncludeTestMenuItemFlag)	
			DeleteMenuItem (gMultiSpecMenus[kOptionM], kDebug);
		
				// Get the palette menu.
				// Will not use this menu item from now on (3/28/2014)															
				
	//	gMultiSpecMenus[kPaletteM] = GetMenu (kPaletteMenuID);
	//	if (gMultiSpecMenus[kPaletteM] == NULL)									
	//																					ExitToShell ();
	//	InsertMenu (gMultiSpecMenus[kPaletteM], -1);		// insert submenu in options menu 
		DeleteMenuItem (gMultiSpecMenus[kOptionM], kPalette);
		
				// Get the area units menu.															
				
		gMultiSpecMenus[kAreaUnitsM] = GetMenu (kDefaultAreaUnitsMenuID);
		if (gMultiSpecMenus[kAreaUnitsM] == NULL)									
																						ExitToShell ();
		InsertMenu (gMultiSpecMenus[kAreaUnitsM], -1);		// insert submenu in options menu 
		
				// Get the windows menu
		
		gMultiSpecMenus[kWindowM] = GetMenu (kWindowMenuID);
		if (gMultiSpecMenus[kWindowM] == NULL)										
																						ExitToShell ();
		InsertMenu (gMultiSpecMenus[kWindowM], 0);		// insert at end of list 
		DisableMenuItem (gMultiSpecMenus[kWindowM], kShowHideCoordinateView);
		DisableMenuItem (gMultiSpecMenus[kWindowM], kSelectionGraphMenuItem);
		DisableMenuItem (gMultiSpecMenus[kWindowM], kPaletteWindow);
		DisableMenuItem (gMultiSpecMenus[kWindowM], kStatisticsWItem);
		
		}		// end "fullSetFlag"
		
	else		// !fullSetFlag
		{
		DeleteMenuItem(gMultiSpecMenus[kFileM], kQuit);
		DeleteMenuItem(gMultiSpecMenus[kFileM], kQuit-1);
		
		}		// end "else !fullSetFlag"

	DrawMenuBar();
																				
	return (returnFlag);

}		// end "SetUpMultiSpecMenu" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateDeskScrap
//
//	Software purpose:	The purpose of this routine is to make certain that
// 						the desk scrap is up to date.  The only item that
//							needs to be checked is the text edit scrap.  If it
//							exists then copy it to the desk scrap.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/10/1993
//	Revised By:			Larry L. Biehl			Date: 02/10/1993	

void UpdateDeskScrap (
				Boolean								clearTextEditScrapFlag)

{
	SInt32								scrapSize;
	
	SInt16								osErr,
											scrapCount;
	

			// Make certain that the desk scrap is up to date.  The				
			// text edit scrap may be the latest.										

	scrapSize = TEGetScrapLength();
	if (scrapSize > 0)
		{         	
		#if !TARGET_API_MAC_CARBON
			PScrapStuff				pScrappStuffPtr;
			pScrappStuffPtr = InfoScrap();
			ZeroScrap();
			scrapCount = pScrappStuffPtr->scrapCount;
		#else
			ScrapRef 	scrapRef;
			ClearCurrentScrap ();
			osErr = GetCurrentScrap (&scrapRef);
					// Not sure how to get scrap count. May never be needed for Carbon since
					// desk scrap may never come up.
// ??			scrapCount = scrapRef.scrapCount;
			scrapCount = 0;
		#endif	// !TARGET_API_MAC_CARBON
		
		osErr = TEToScrap();
				   
		if (osErr == noErr)
//			gLastScrapCount = pScrappStuffPtr->scrapCount;
			gLastScrapCount = scrapCount;
			
		if (clearTextEditScrapFlag)
			{
			TESetScrapLength (0);
			gLastScrapCount = 0;
			
			}		// end "if (clearTextEditScrapFlag)" 
						
		}		// end "if ( scrapSize > 0 )" 
				
}		// end "UpdateDeskScrap" 



#if !TARGET_API_MAC_CARBON	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateTextEditScrap
//
//	Software purpose:	The purpose of this routine is to make certain that
// 						the text edit scrap is up to date.  Only scrap in memory, non-PICT 
//							type scrap, and limited size scrap is copied.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			LoadRequestedDialog in dialogUtilities.c
//
//	Coded By:			Larry L. Biehl			Date: 12/19/1997
//	Revised By:			Larry L. Biehl			Date: 12/23/1997	

void UpdateTextEditScrap (void)

{
	ScrapStuffPtr						scrappStuffPtr;
	SInt32*								scrapDataPtr;
	
	SInt16								osErr;
	

			// Make certain that the text edit scrap is up to date.  The				
			// desk scrap may be the latest.										

	scrappStuffPtr = InfoScrap();
	
			// Only update if the desk scrap is new and the scrap is in memory.
					
	if (scrappStuffPtr->scrapSize > 0 && gLastScrapCount != scrappStuffPtr->scrapCount)
		{
		if (scrappStuffPtr->scrapHandle != NULL)
			{
			scrapDataPtr = (SInt32*)*scrappStuffPtr->scrapHandle;
			if ( *scrapDataPtr != 'PICT' )
				{
				osErr = TEFromScrap();
				
				if (osErr == noErr)
					gLastScrapCount = scrappStuffPtr->scrapCount;
				
				}		// end "if ( *scrapDataPtr != 'PICT' )"
				
			}		// end "if (scrappStuffPtr->scrapHandle != NULL)"
			
		}		// end "if (scrappStuffPtr->scrapSize > 0 && ..." 
		
	if (gLastScrapCount != scrappStuffPtr->scrapCount)
		TESetScrapLength (0);
				
}		// end "UpdateTextEditScrap" 
#endif	// !TARGET_API_MAC_CARBON



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void WriteInterfacePreferenceValue
//
//	Software purpose:	This routine lists the specified string in the
//							resource file to the output text window and/or
//							disk file.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/09/2015
//	Revised By:			Larry L. Biehl			Date: 01/09/2015			

void WriteInterfacePreferenceValue (
				Boolean								fullVersionFlag)
													
{
	SInt16								interfaceValue;
	
	
	interfaceValue = 2;
	if (gFullVersionFlag)
		interfaceValue = 1;
		
			// Set up the preference. 

	CFStringRef userInterfaceKey = CFSTR("DefaultUserInterface");
	CFNumberRef userInterfaceValue =	CFNumberCreate (kCFAllocatorDefault, kCFNumberSInt16Type, &interfaceValue);
	CFPreferencesSetAppValue(userInterfaceKey, userInterfaceValue, kCFPreferencesCurrentApplication);
						
			// Write out the preference data. 
		
	CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
					
			// Release CF values 
			
	CFRelease(userInterfaceKey);
	CFRelease(userInterfaceValue);

}		// end "WriteInterfacePreferenceValue" 
