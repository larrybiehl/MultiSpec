//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						menus.c
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			06/14/2016
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	This file contains routine to handle decode and manage
//								user menu selections.
//
//	Functions in file:	void 					ClearTransformationMatrix
//								Boolean 				CloseTheProject
//								void 					DoEdit
//								void					DoOpenProjectFile
//								void 					DoPrintWindow
//								void 					ForceProjectSegmentsToBeLoaded
//								void					HideMultiSpecMenuBar
//								void					LoadProjectCodeSegments
//								void 					MemorySummary
//								void 					Menus
//								void					SaveOutputMenuItem
//								void					SelectionGraphMenuItem
//								void					ShowMultiSpecMenuBar
//								void					UnloadProjectCodeSegments
//								void					UpdateEditMenuItems
//								void					UpdatePaletteMenuItems
//								void					UpdateProcessorMenuItems
//								void 					UpdateProjectMenuItems	 
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

//#include	"SMulSpec.h"
//#include "SExtGlob.h"
//#include "mwCarbonCompatibility.h"

extern SInt32				gNumberOfOpenFilesHardLimit;



extern void					DecisionTreeControl (
									FileInfoPtr							fileInfoPtr);

#if use_mlte_for_text_window	
	extern OSStatus			SetOutputWindowFont (
										TXNObject							textObject);
										
	extern OSStatus			TXNSetReadOnly ( 
										TXNObject							inTextObject, 
										Boolean								inIsReadOnly);
#endif	// use_mlte_for_text_window
						

extern void						WriteInterfacePreferenceValue (
										Boolean								fullVersionFlag);


			// Prototypes for file routines that are only called from other 		
			// routines in this file.					

void 						DoEdit (short int);

void 						DoPrintWindow (void);

void						HideMultiSpecMenuBar (void);

void						LoadProjectCodeSegments (void);

void 						MemorySummary (void);

void						SaveOutputMenuItem (void);

void 						SelectionGraphMenuItem (void);

void 						ShowHideCoordinateView (
								Handle								windowInfoHandle);

void 						UnloadProjectCodeSegments (void);

//void 						UpdateCoordinateMenuBeforeSelection (
//								WindowInfoPtr						windowInfoPtr);

void 						UpdateEditMenuItems (void);

void						UpdateFileMenuItems (void);

void 						UpdateGraphicsMenuBeforeSelection (
								WindowInfoPtr						windowInfoPtr,
								WindowInfoPtr						activeImageWindowInfoPtr);

void 						UpdateImageMenuBeforeSelection (
								WindowInfoPtr						windowInfoPtr,
								WindowInfoPtr						activeImageWindowInfoPtr);

void 						UpdateMenuBeforeSelectionForNoWindow (
								WindowInfoPtr						windowInfoPtr,
								WindowInfoPtr						activeImageWindowInfoPtr);
								
void						UpdateOptionsMenuItems (void);

void 						UpdateOutputTextMenuBeforeSelection (
								WindowInfoPtr						windowInfoPtr,
								WindowInfoPtr						activeImageWindowInfoPtr);

void 						UpdatePaletteMenuBeforeSelection (
								WindowInfoPtr						windowInfoPtr,
								WindowInfoPtr						activeImageWindowInfoPtr);

//void 					UpdatePaletteMenuItems (
//								WindowInfoPtr						activeImageWindowInfoPtr);

void						UpdateProcessorMenuItems (
								WindowInfoPtr						activeImageWindowInfoPtr);

void 						UpdateProjectMenuItems (
								WindowInfoPtr						activeImageWindowInfoPtr); 	

void 						UpdateReformatMenuItems (
								WindowInfoPtr						windowInfoPtr);

void 						UpdateStatisticsMenuBeforeSelection (
								WindowInfoPtr						windowInfoPtr,
								WindowInfoPtr						activeImageWindowInfoPtr);
								
void 						UpdateWindowMenuItems (
								WindowInfoPtr						windowInfoPtr);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoEdit
//
//	Software purpose:	The purpose of this routine is to handle the edit
//							menu items.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/12/1988
//	Revised By:			Larry L. Biehl			Date: 12/29/2008

void DoEdit (
				SInt16								menuItem)
{
			// Declare local variables and structures	
	
	double								magnification;
	
	OpenCPicParams						pictureParameters;
	
	Rect									destinationRect,
											imageRect,
											sourceRect;
	
//	CGrafPtr								savedPort;    
	WindowInfoPtr						windowInfoPtr;
	
//	GDHandle								savedDevice; 
	Handle								displaySpecsH;
	PicHandle							thePic = NULL;
	
//	WSHandle								scrapTextH,
//											textH;	
	
	#if use_mlte_for_text_window
		OSStatus								status;
		TXNObject							scrapTextH,
												textH;	
	#endif
	#if !use_mlte_for_text_window
		WEReference							scrapTextH,
												textH;	
	#endif
	
	SInt32								errCode,
											handleSize;
											
	SInt16								legendBottom,
											legendWidth,
											returnCode,
											windowType;
	
	Boolean								continueFlag,
											redisplayFlag;
											
	#if use_mlte_for_text_window
		Boolean								callSetFontFlag = FALSE;
	#endif
	

	#if !TARGET_API_MAC_CARBON
	if ( !SystemEdit ( menuItem-1))
		{
	#endif	// !TARGET_API_MAC_CARBON
		windowType = ((WindowInfoPtr)*gActiveWindowInfoH)->windowType;
			
		if (windowType == kOutputWindowType)
			{
			textH = gOutputTextH;
			redisplayFlag = FALSE;
			scrapTextH = NULL;

					// Handle Undo/Cut/Copy/Paste/Clear properly							
					// between WSScrap and the Clipboard.									
			
			switch (menuItem)
				{
				case kUndo:
					#if use_mlte_for_text_window
						TXNUndo (textH);
					#endif
					#if !use_mlte_for_text_window
						WEUndo (textH);
					#endif
					break;
					
				case kCut:
					#if use_mlte_for_text_window
//						TXNSetReadOnly (textH, FALSE);
						status = TXNCut (textH);
//						TXNSetReadOnly (textH, TRUE);
						callSetFontFlag = TRUE;
					#endif
					#if !use_mlte_for_text_window
						WECut (textH);
					#endif
					break ;
			
				case kCopy:
					#if use_mlte_for_text_window
						status = TXNCopy (textH);
					#endif
					#if !use_mlte_for_text_window
						WECopy (textH);
					#endif
					break ;
				
				case kPaste:
							// Make certain that a minimum amount of space is			
							// available for the paste.										
							// 'errCode' and 'handleSize' are used here as temp		
							// variables.															
							
//					PurgeSpace (&longOffset, &handleSize);
					MGetFreeMemory ((UInt32*)&handleSize);
					if (handleSize <= gTextMemoryMinimum)
						{
						returnCode = DisplayAlert (kTextWindowLowMemoryAlertID, 
																kCautionAlert, 
																kAlertStrID, 
																IDS_Alert58, 
																0, 
																NULL);
						break;
						
						}		// end "if ( handleSize <= gTextMemoryMinimum )" 
						
					#if use_mlte_for_text_window
//						TXNSetReadOnly (textH, FALSE);
						status = TXNPaste (textH);
//						TXNSetReadOnly (textH, TRUE);
						callSetFontFlag = TRUE;
					#endif
					#if !use_mlte_for_text_window
						WEPaste (textH);
					#endif
					break;
				
				case kClear:
					#if use_mlte_for_text_window
//						TXNSetReadOnly (textH, FALSE);
						status = TXNClear (textH);
//						TXNSetReadOnly (textH, TRUE);
						callSetFontFlag = TRUE;
					#endif
					#if !use_mlte_for_text_window
						WEDelete (textH);
					#endif
					break;
					
				case kSelectAll:
					#if use_mlte_for_text_window
						TXNSelectAll (textH);
					#endif
					#if !use_mlte_for_text_window
						WESetSelection (0, LONG_MAX, textH);
					#endif
					break;
					
				}		// end "switch (menuItem)"
				
			#if use_mlte_for_text_window
				if (callSetFontFlag)
					status = SetOutputWindowFont (gTextObject);
			#endif
				
					// The only thing that doesn't change the information in the 	
					// window is copying and selecting all.								
					
			if (menuItem != kCopy && menuItem != kSelectAll)
				((WindowInfoPtr)*gActiveWindowInfoH)->changed = 1;
			
					// If there is no text in the output window, then 
					// reset the gMaxCharsInLine.										
			
			#if use_mlte_for_text_window
				if (TXNDataSize (gOutputTextH) <= 2) 
			#endif
			#if !use_mlte_for_text_window		
				if (WEGetTextLength (gOutputTextH) <= 2)
			#endif
					{
					gMaxCharsInLine = 1;
					SetOutputWindowChangedFlag (FALSE);
				
					}		// end "if ((*textH)->tLength <= 2)"
		      
					// Determine if text is selected.  If so change the menu items	
					// for 'Save Output Text' and 'Print Text' to indicate that		
					// Selected text will be used.

			gUpdateFileMenuItemsFlag = TRUE;
			gUpdateEditMenuItemsFlag = TRUE;
/*				
					// Invalidate the output text window if it needs to be			
					// updated.																		
			
			if (redisplayFlag)
				{
//				longOffset = WSRebuild (textH, FALSE);
				WECalText (textH);
				GetWindowClipRectangle (gOutputWindow, kNoScrollBarArea, &sourceRect);
				InvalRect ( &sourceRect );
				
						// Scroll output window to the selection and adjust the 		
						// scroll bar if requested.											
					
				UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
				
				}		// end "if (redisplayFlag)" 
*/				
			}		// end "if (windowType == kOutputWindowType)" 
		
		if (windowType == kImageWindowType || windowType == kThematicWindowType)
			{
					// Handle Undo/Cut/Copy/Paste/Clear properly							
					// between WSScrap and the Clipboard.									

			switch (menuItem)
				{
				case kUndo:
							//	Cannot Undo 
					break;
			
				case kCut:
							//	Cannot Cut 
					break;
					
				case kCopy:
				
							// Change cursor to watch.											
							
					MSetCursor (kWait);
					
					HLock (gActiveImageWindowInfoH);
					windowInfoPtr = (WindowInfoPtr)*gActiveImageWindowInfoH;
					
							// Get the selection rectangle												
												
					GetSelectedOffscreenRectangle (windowInfoPtr, 
																&sourceRect, 
																TRUE,
																TRUE);
		
							// Determine the destination rectangle. 						
							// Get handle and pointer to display specification for	
							// window.																
			
					displaySpecsH = windowInfoPtr->displaySpecsHandle;
					magnification = ((DisplaySpecsPtr)*displaySpecsH)->magnification;
									
							// Get the legend width and height.											
	
					if (windowInfoPtr->showLegend)
						{
						legendWidth = windowInfoPtr->legendWidth;
						legendBottom = GetListBottom (windowInfoPtr->legendListHandle);
						
						}
						
					else		// !windowInfoPtr->showLegend
						{
						legendWidth = 0;
						legendBottom = 0;
						
						}		// end "else !windowInfoPtr->showLegend"
				
					imageRect.top = 0;
					imageRect.left = legendWidth;
					imageRect.bottom =  
						(SInt16)((sourceRect.bottom-sourceRect.top) * magnification) + 
																	windowInfoPtr->titleHeight;
					imageRect.right = 
						(SInt16)((sourceRect.right-sourceRect.left) * magnification) + 
																						legendWidth;
					
					destinationRect.top = 0;
					destinationRect.left = 0;							
					destinationRect.bottom = MAX(imageRect.bottom, legendBottom);
					destinationRect.right = imageRect.right;
							
							// Save the current clip region and set the clip region 	
							// so that the selected area will be copied to the 		
							// clipboard.															
							
					if (gTempRegion1 != NULL)
						{
						pictureParameters.srcRect = destinationRect;
						pictureParameters.hRes = 0x00480000;
						pictureParameters.vRes = 0x00480000;
						pictureParameters.version = -2;
						
//						GetGWorld (&savedPort, &savedDevice);
//						SetGWorld (windowInfoPtr->offscreenGWorld, NULL);
						
						SetPortWindowPort (gActiveImageWindow);
						
						GetClip (gTempRegion1);
					
						ClipRect (&destinationRect);
						
								// Save the destination rectangle; It may be needed to draw project
								// fields.
								
						gViewRect = destinationRect;
							
               	thePic = OpenCPicture (&pictureParameters);
               	if (thePic != NULL)
	               	{
							ClipRect (&imageRect);
	               	CopyOffScreenImage (gActiveImageWindow, 
	               								NIL, 
	               								&sourceRect, 
	               								kClipboardCopy,
	               								kNotFromImageUpdate);
				
							ClipRect (&destinationRect);
	               	if (legendWidth > 0)
								DrawThematicLegend (gActiveImageWindow, 
													NULL, 
													windowInfoPtr, 
													windowInfoPtr->legendListHandle,
													windowInfoPtr->legendLeftStart,
													windowInfoPtr->legendTopStart,
													FALSE);
									
	               	ClosePicture ();
	               	
							#if !TARGET_API_MAC_CARBON
	               		ZeroScrap();
	               	#else
	               		ScrapRef 	scrapRef;
	               		errCode = ClearCurrentScrap ();
	               		errCode = GetCurrentScrap (&scrapRef);
								MoveHHi ((Handle)thePic);
								errCode = MemError ();
							#endif	// !TARGET_API_MAC_CARBON
	
	               	handleSize = GetHandleSize ((Handle)thePic);
	               	HLock ((Handle)thePic);
							errCode = MemError ();
	               	
							#if !TARGET_API_MAC_CARBON
	               		errCode = PutScrap(handleSize, 'PICT', (Ptr)*thePic);
	               	#else
	               		errCode = PutScrapFlavor (scrapRef, 
	               											kScrapFlavorTypePicture, 
	               											kScrapFlavorMaskNone, 
	               											handleSize,
	               											(Ptr)*thePic);
							#endif	// !TARGET_API_MAC_CARBON
								
	               	HUnlock ((Handle)thePic);
	              		KillPicture (thePic);
	               	
	               	if (errCode != noErr)  
								returnCode = DisplayAlert (1153, 3, 0, 0, 0, NULL);
					
							else		// errCode == noErr 
								{
										// Make certain that any text edit scrap does 	
										// not write over the desk scrap.					
								TESetScrapLength (0);
								gLastScrapCount = 0;
								
								}		// end "else errCode == noErr" 
	              		
	              		}		// end "if (thePic != NULL)" 
	              	
	              	else		// thePic == NULL  
							returnCode = DisplayAlert (1153, 3, 0, 0, 0, NULL);
		
								// Set clip region back to that for present image 		
								// window and then dispose of the saved region.			
						
						SetClip (gTempRegion1);
						SetEmptyRgn (gTempRegion1);
						
//						SetGWorld (savedPort, savedDevice);
						
						}		// end "if (gTempRegion1 != NULL)" 
					
					HUnlock (gActiveImageWindowInfoH);
					
							// Change cursor back to pointer.								
								
					MInitCursor ();
					break;
				
				case kPaste:
					//	Cannot Paste 
					break;
				
				case kClear:
					//	Cannot Clear 
					break;
					
				case kSelectAll:
				
					DoEditSelectAllImage ( gActiveImageWindow );
					
							// Enable Edit-Copy if not already so that selection can	
							// be copied to the clipboard										
							
					gUpdateEditMenuItemsFlag = TRUE;
					
					break;
					
 	      	case kEditSelectionRect:
					EditSelectionDialog (kRectangleType, FALSE);
					
 	      		break;
					
				}		// end "switch (menuItem)" 
					
			}		// end "if (windowType == kImageWindowType || ..." 
			
		else if (windowType == kGraphicsWindowType)
			continueFlag = DoGraphWindowEdit (menuItem);
			
		else if (windowType == kStatisticsWindowType)
			DoStatisticsWEdit (menuItem);
				
	#if !TARGET_API_MAC_CARBON
		}		// end "if ( !SystemEdit ( menuItem-1))" 
	#endif	// !TARGET_API_MAC_CARBON		

}		// end "DoEdit" 

						
											
//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoOpenProjectFile
//
//	Software purpose:	The purpose of this is to force the 'Menu' code
//							resource to be loaded into memory.  It is called by
//							main.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			Menus in menus.c
//
//	Coded By:			Larry L. Biehl			Date: 06/30/1993
//	Revised By:			Larry L. Biehl			Date: 12/14/2000

void DoOpenProjectFile (
				SInt16								projectVersionCode)

{			
	ForceProjectSegmentsToBeLoaded ();

	if (gMemoryTypeNeeded == 0)
		{
	   gProcessorCode = kOpenProjectFileProcessor;
	   
		if (projectVersionCode > 0)		
			OpenProjectFile (&gLocalAppFile);
			
		else		// projectVersionCode <= 0 	
			OpenProjectFile (NULL);
		
		if (gProjectInfoPtr != NULL)
			CreateProjectWindow ();
			
	   gProcessorCode = 0;
			
		}		// end "if (gMemoryTypeNeeded == 0)" 
		
			// If the 'Project' and 'Statistics' code resources 					
			// could not be loaded without using spare memory then				
			// unload the resources so that the spare code resource				
			// memory can be reclaimed.  The user has already been				
			// warned that memory is low.													
		
	if (gProjectInfoPtr == NULL)
		UnloadProjectCodeSegments ();
		
	gMemoryTypeNeeded = 0;		

}		// end "DoOpenProjectFile" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoPrintWindow
//
//	Software purpose:	The purpose of this routine is to print the active
//							window.  Two different methods may be used 
//							depending upon whether Apple Events are being used.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:  Error code for file operations.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/10/1993
//	Revised By:			Larry L. Biehl			Date: 10/17/1996

void DoPrintWindow (void)

{
	SInt16		returnError;
	
	
   gMemoryTypeNeeded = 1;
   
	if (gAppleEventsFlag)
		returnError = SendAEOpenOrPrintDocument (NULL, kAEPrintDocuments);
	 
	 else		// !gAppleEventsFlag 
	 	{
		DoPrint (TRUE, gTheActiveWindow);
		
		}		// end " else !gAppleEventsFlag"
		
	UnloadSeg (DoPrint);
   gProcessorCode = 0;
   gMemoryTypeNeeded = 0;
	
	return;

}		// end "DoPrintWindow" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void HideMultiSpecMenuBar
//
//	Software purpose:	The purpose of this routine is to disable all MultiSpec
//							menu bar items.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			GetStatusDialog in dialogUtilities.c
//
//	Coded By:			Larry L. Biehl			Date: 03/15/1993
//	Revised By:			Larry L. Biehl			Date: 03/15/1993	

void HideMultiSpecMenuBar (void)

{
	DisableMenuItem (gMultiSpecMenus[kFileM], 0);
	DisableMenuItem (gMultiSpecMenus[kEditM], 0);
	DisableMenuItem (gMultiSpecMenus[kWindowM], 0);
	DisableMenuItem (gMultiSpecMenus[kProjectM], 0);
	DisableMenuItem (gMultiSpecMenus[kProcessorM], 0);
	DisableMenuItem (gMultiSpecMenus[kPaletteM], 0);
	DisableMenuItem (gMultiSpecMenus[kOptionM], 0);
	
	gRedrawMenuBar = TRUE;
		
}		// end "HideMultiSpecMenuBar" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MemorySummary
//
//	Software purpose:	The purpose of this routine is to give a summary
//							of the memory status of the application.  It is
//							called from the options menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/25/1988
//	Revised By:			Larry L. Biehl			Date: 03/21/2013	

void MemorySummary (void)

{
	Str63									contiguousMemoryText,
											freeBytesPossibleText,
											freeMemoryText,
											openFilesText,
											stackSpaceText;
											
	DialogPtr							dialogPtr;
	
	SInt32								freeMemory,
											lContBlock,
											freeBytesPossible,
											stackSpace;
	
	SInt16								itemHit,
											numberChars;
	
	
	dialogPtr = NULL;
	
	freeMemory = FreeMem ();
	
//	PurgeSpace (&freeBytesPossible, &lContBlock);
	freeBytesPossible = (SInt32)MGetFreeMemory ((UInt32*)&lContBlock);
	
	stackSpace = StackSpace ();
	
			// Update the alert with static text items									
				
	NumToString (freeMemory, freeMemoryText);
	numberChars = freeMemoryText[0];
	numberChars = InsertCommasInNumberString (
								(char*)&freeMemoryText[1], 
								numberChars, 
								-1,
								numberChars);
	freeMemoryText[0] = numberChars;
										
	NumToString (lContBlock, contiguousMemoryText);
	numberChars = contiguousMemoryText[0];
	numberChars = InsertCommasInNumberString (
								(char*)&contiguousMemoryText[1], 
								numberChars, 
								-1,
								numberChars);
	contiguousMemoryText[0] = numberChars;
										
	NumToString (freeBytesPossible, freeBytesPossibleText);
	numberChars = freeBytesPossibleText[0];
	numberChars = InsertCommasInNumberString (
								(char*)&freeBytesPossibleText[1], 
								numberChars, 
								-1,
								numberChars);
	freeBytesPossibleText[0] = numberChars;
										
	NumToString (stackSpace, stackSpaceText);
	numberChars = stackSpaceText[0];
	numberChars = InsertCommasInNumberString (
								(char*)&stackSpaceText[1], 
								numberChars, 
								-1,
								numberChars);
	stackSpaceText[0] = numberChars;
	
	ParamText ((ConstStr255Param)&freeMemoryText,
					(ConstStr255Param)&contiguousMemoryText,
					(ConstStr255Param)&freeBytesPossibleText,
					(ConstStr255Param)&stackSpaceText);
	
			// Get the modal dialog for the memory summary.	 Be certain to allow	
			// for ParamText not working above because of the lack of memory.		
	
	if (MemError() == noErr)
		dialogPtr = LoadRequestedDialog (kMemoryID, kDoNotCopyScrap, 1, 0);
		
		
	if (dialogPtr != NULL)
		{
				// Load estimate of the number of open files			
			
		NumToString (gNumberOfOpenFiles, openFilesText);
		openFilesText[0] = InsertCommasInNumberString (
									(char*)&openFilesText[1], 
									openFilesText[0], 
									-1,
									openFilesText[0]);
		LoadDItemString (dialogPtr, 12, (char*)openFilesText);
//		LoadDItemValue (dialogPtr, 12, gNumberOfOpenFiles);
					
				// Load soft limit for the number of open files. Add the 5 back in 
				// that was subtracted of in the initialize section	
				
		NumToString (gNumberOfOpenFilesLimit+20, openFilesText);
		openFilesText[0] = InsertCommasInNumberString (
									(char*)&openFilesText[1], 
									openFilesText[0], 
									-1,
									openFilesText[0]);
		LoadDItemString (dialogPtr, 14, (char*)openFilesText);
//		LoadDItemValue (dialogPtr, 14, gNumberOfOpenFilesLimit+20);
					
				// Load hard limit for the number of open files.	
				
		NumToString (gNumberOfOpenFilesHardLimit, openFilesText);
		openFilesText[0] = InsertCommasInNumberString (
									(char*)&openFilesText[1], 
									openFilesText[0], 
									-1,
									openFilesText[0]);
		LoadDItemString (dialogPtr, 16, (char*)openFilesText);
//		LoadDItemValue (dialogPtr, 16, gNumberOfOpenFilesHardLimit);
		
				// Center the dialog and then show it.										
				
		ShowDialogWindow (dialogPtr, kMemoryID, kDoNotSetUpDFilterTable);
			
				// Get deactivate event for the window that is now behind the 		
				// dialog and handle it.														
			
		itemHit = 0;
		
		do
			{		
			ModalDialog (gProcessorDialogFilterPtr, &itemHit);
				
			}	while (itemHit != 1);
		
		CloseRequestedDialog (dialogPtr, kDoNotSetUpDFilterTable);
		
		}		// end "if (dialogPtr != NULL)" 
	
}		// end "MemorySummary" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void Menus
//
//	Software purpose:	The purpose of this routine is to handle the menu
//							selection.
//
//	Functional description:	The steps in this function follow.
//
//			A mouse down event was detected in the menu area; process menu
//			selection
//
//  		The MENUSELECT tool box call handles the messy details of highlighting
//  		menus and menu selections, pulling menus down, and determining which
//  		item was selected. The HILITEMENU call at the end of the switch
//  		actually unhighlights the menus.
//
//	Parameters in:		Long word which contains menu and menu item selected.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl		Date: 02/01/1988
//	Revised By:			Ravi S. Budruk		Date: 08/19/1988 
//	Revised By:			Larry L. Biehl		Date: 01/09/2015

void Menus (
				SInt32							menuData)

{
			//	Declare local variables 
	
	CursPtr								tempCursorPtr;
	DialogPtr							dialogPtr;
//	DisplaySpecsPtr					displaySpecsPtr;
	WindowPtr							window;
	
	Handle					//			displaySpecsH,
											projectInfoHandle;
	
	SInt32								freeBytesPossible,
											longestContBlock;
	
	SInt16								menuID,
											menuItem;
	
	Boolean								newOrHideFlag,
											parameterChangedFlag,
											returnFlag,
											savedFullVersionFlag;
	
	
	menuItem = LoWord (menuData);
	menuID   = HiWord (menuData);
	gOperationCanceledFlag = FALSE;
	gFullVersionSettingFlag = FALSE;

	switch (menuID)
		{
		case kAppleMenuID:  		// the Apple menu was selected 
			
			switch (menuItem)
				{
				case kAboutMultiSpec:
				
	      		gProcessorCode = kAboutMultiSpecProcessor;
				
							// Get the modal dialog for about MultiSpec.					
	
					dialogPtr = LoadRequestedDialog (kMoveableAboutMultiSpecID, 
																	kDoNotCopyScrap,
																	1,
																	0);
					if (dialogPtr != NULL)
						{
						savedFullVersionFlag = gFullVersionFlag;
						
						//HideDialogItem (dialogPtr, 1);
								// If option key is down, show the option for setting simpler version of
								// MultiSpec
						if (gProjectInfoPtr == NULL && (gEventRecord.modifiers & controlKey))
							{
							gFullVersionSettingFlag = TRUE;
							if (gFullVersionFlag)
								SetDLogControl (dialogPtr, 15, 0);
							else 
								SetDLogControl (dialogPtr, 15, 1);
								
							ShowDialogItem (dialogPtr, 15);
							
							}		// end "if (gEventRecord.modifiers & controlKey)"
						else
							HideDialogItem (dialogPtr, 15);
						
						#if use_mlte_for_text_window
							//Rect									portRect;
							
							//GetWindowPortBounds (GetDialogWindow(dialogPtr), &portRect);
							//SizeWindow (GetDialogWindow(dialogPtr), 
							//					portRect.right-portRect.left, 
							//					portRect.bottom-portRect.top-10,			// 35
							//					FALSE);
							HideDialogItem (dialogPtr, 12);
						#endif
						
						#if !include_gdal_capability
							HideDialogItem (dialogPtr, 13);
						#endif
						
						ShowDialogWindow (
								dialogPtr, kMoveableAboutMultiSpecID, kDoNotSetUpDFilterTable);
								
						Boolean 	modalDone = FALSE;
						SInt16	itemHit = 0;
						do 
							{
						   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
							
							if (itemHit == 15)
								{
								SInt16	theType;
								Handle	theHandle;
								Rect		theBox;
								
								GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
								ChangeDLogCheckBox ((ControlHandle)theHandle);
								
								}		// end "if (itemHit == 15)"
								
							else if (itemHit == 1)
								{
								if (gFullVersionSettingFlag)
									{
											// simpler version setting may have changed.
											
									gFullVersionFlag = !GetDLogControl (dialogPtr, 15);
									
									}		// end "if (fullVersionSettingFlag)"
									
								modalDone = TRUE;
								
								}		// end "else if itemHit == 1"
				
							} while (!modalDone);
							
								// Remove any mouse down and up events.					
								
						FlushEvents (mDownMask+mUpMask, 0);	
		
						CloseRequestedDialog (dialogPtr, kDoNotSetUpDFilterTable);
						
								// If the version setting flag changed, write it out to the preferences file.
						
						if (savedFullVersionFlag != gFullVersionFlag)
							WriteInterfacePreferenceValue (gFullVersionFlag);
						
						gFullVersionSettingFlag = FALSE;
	      			gProcessorCode = 0;
						
						}		// end "if (dialogPtr != NULL)" 
					break;
					
				default:
	  
					#if !TARGET_API_MAC_CARBON
						GrafPtr	savedPort;
						
						GetMenuItemText (gMultiSpecMenus[kAppleM], menuItem, gMenuItemString);
						GetPort (&savedPort);
						OpenDeskAcc ( (ConstStr255Param)&gMenuItemString);
						SetPort (savedPort);
					#endif	// !TARGET_API_MAC_CARBON
					break;
					
				}		// end "switch (menuItem)" 
			break;
	
		case kFileMenuID:			// the "File" menu was selected 
		
	      switch (menuItem)
	      	{
				case kNewProject:
	      		gProcessorCode = kOpenProjectFileProcessor;
				
					ForceProjectSegmentsToBeLoaded ();
					
					if (gMemoryTypeNeeded >= 0)
 	      			StatisticsControl ();
						
						// If the 'Project' and 'Statistics' code resources 			
						// could not be loaded without using spare memory then		
						// unload the resources so that the spare code resource		
						// memory can be reclaimed.  The user has already been		
						// warned that memory is low.											
						
					if (gProjectInfoPtr == NULL)
						{
						UnloadSeg (StatisticsControl);
						UnloadSeg (ForceProjectUtilityCodeResourceLoad);
						UnloadSeg (OpenProjectFile);
						
						}		// end "if (gMemoryTypeNeeded < 0)" 
						
	      		gProcessorCode = 0;
	      		gMemoryTypeNeeded = 0;
					break;
					
	      	case kOpenImage:
	      		gProcessorCode = kOpenImageFileProcessor;
					GetImageFile ();
	      		gProcessorCode = 0;
	      		break;
				
				case kOpenProject:
					DoOpenProjectFile (0);
					break;
	      		
		      case kOpenProjImage:
					OpenProjectImageWindow ();
		      	break;
	      		
		      case kOpenThematicClassGroup:
		      	LoadThematicInfo ();
		      	break;
						      
				case kCloseImage:
				
  					switch (gActiveWindowType)
  						{
  						case kGraphicsWindowType:
      					CloseGraphicsWindow (gTheActiveWindow);
      					break;
      						
  						case kImageWindowType:
  						case kThematicWindowType:
      					CloseImageWindow (gTheActiveWindow);
      					break;
      					
//  					case kPaletteWindowType:
//							ClosePaletteWindow (gPaletteWindow);
//      					break;
      					
  						case kStatisticsWindowType:
							CloseTheProject ();
      					break;
      						
      				}		// end "switch (gActiveWindowType)" 
		      		
					break;
 	      		
 	      	case kSaveTransformMenuItem:
	   			gProcessorCode = kLoadSaveTransformationMatrix;
 	      		if (gTransformationMatrix.numberChannels > 0)
 	      			WriteTransformationFile ();
 	      		
 	      		else		// gTransformationMatrix.numberChannels <= 0 
 	      			LoadTransformationFile ();
 	      		break;
 	      		
				case kSave:
				
	   			gProcessorCode = kSaveProcessor;
					WriteThematicInfo (NULL, 0);
					break;
					
				case kSaveOutputAs:
				
	   			gProcessorCode = kSaveOutputAsProcessor;
					SaveOutputMenuItem ();
					break;
				
				case kSaveProject:
				
	   			gProcessorCode = kSaveProjectProcessor;
					SaveProjectFile (0);
					break;
				
				case kSaveProjectAs:
					
	   			gProcessorCode = kSaveProjectAsProcessor;
					SaveProjectFile (1);
					break;
					
				case kPageSetUp:
					
	      		gMemoryTypeNeeded = 1;
					DoPageSetup (gTheActiveWindow);
					UnloadSeg (DoPrint);
	      		gMemoryTypeNeeded = 0;
					break;
					
				case kPrintOutput:
				
					DoPrintWindow ();
					break;
		  
				case kQuit:
	
					gDoneFlag = TRUE;
					break;
				}
	   	
	   	gProcessorCode = 0;
			break;
	    
		case kEditMenuID:				// the "Edit" menu was selected 
			
			if (menuItem < kClearSelectionRect)
				DoEdit (menuItem);
 	      
 	      else		// menuItem >= kEditSelectionRect 
 	      	{
 	      	switch (menuItem)
 	      		{
	 	      	case kClearSelectionRect:
	 	      	
						if (gTheActiveWindow == gProjectWindow)
							{
							if (gProjectSelectionWindow == NULL)
								ClearNewFieldList ();
								
							else		// gProjectSelectionWindow != NULL
	 	      				ClearSelectionArea (gProjectSelectionWindow);
								
							}		// end "if (gTheActiveWindow == gProjectWindow)"  
	 	      		
	 	      		else		// gTheActiveWindow!= gProjectWindow
	 	      			ClearSelectionArea (gActiveImageWindow);
	 														
	 							// Update the Save and Print menu item to indicate 	 	
	 							// selection is now entire image.								
	 			
						gUpdateFileMenuItemsFlag = TRUE;
						
	 	      		break;
	 	      		
	 	      	case kClearTransformMenuItem:
	 	      		ClearTransformationMatrix (TRUE);
	 	      		break;
					
					case kChangeSpecs:
		      		gProcessorCode = kChangeImageDescriptionProcessor;
						FileSpecificationDialog (gActiveImageFileInfoH, 
															gActiveImageWindowInfoH,
															&parameterChangedFlag);
		      		gProcessorCode = 0;
		      		
		      		if (parameterChangedFlag)
		      			{
			      		gProcessorCode = kDisplayProcessor;
				  			DisplayImage ();
		  		  			UnloadSeg (DisplayImage);
		  		  			
		  		  			}		// end "if (parameterChangedFlag)"
		      			
						break;
						
	 	      	case kCoordinates:
						if ( CoordinateDialog () )
							gUpdateWindowsMenuItemsFlag = TRUE;
 	      			break;
					
					}		// end "switch (menuItem)" 
					
				}		// end "else menuItem >= kEditSelectionRect" 
					
			break;
 	      	
 	   case kClearOverlaysMenuID:
	    
			switch (menuItem)
				{
 	      	case kAllImageOverlaysMenuItem:
 	      		CloseAllImageOverlayFiles ();
 	      		gUpdateEditMenuItemsFlag = TRUE;
  		  			break;
  		  			
 	      	case kAllVectorOverlaysMenuItem:
 	      		CloseAllVectorOverlayFiles ();
 	      		gUpdateEditMenuItemsFlag = TRUE;
  		  			break;
		    
				default:
					ClearOverlay (menuItem-3);
 	      		gUpdateEditMenuItemsFlag = TRUE;
					break;
					
				}		// end "switch (menuItem)"
				
			break;
	
		case kProjectMenuID:			// the "Project" menu was selected 
		
	      switch (menuItem)
	      	{
				case kUseBaseStatistics:		// kOriginalStats
				case kUseLOOStatistics:			// kLeaveOneOutStats
				case kUseEnhancedStatistics:	// kEnhancedStats
               {
					UInt16 covarianceStatsToUse = menuItem - kUseBaseStatistics + 1;
					if (covarianceStatsToUse == 3)
						covarianceStatsToUse = 4;
						
					SetProjectCovarianceStatsToUse (covarianceStatsToUse);
               }
					break;
				
				case kClearStatistics:
					
					returnFlag = ProjectMenuClearStatistics ();
					break;
				
				case kChangeBaseImageFile:
				
					ChangeProjectBaseImage ();
					break;
				
				case kAssociatedImage:
					
					ChangeProjectAssociatedImageItem (gActiveImageWindowInfoH);
					break;
					
				}		// end "switch (menuItem)" 
				
	      gProcessorCode = 0;
			gUpdateProcessorMenuItemsFlag = TRUE;
				
			break;
	
		case kProcessorMenuID:		// the "Processor" menu was selected 
	    
	      gMemoryTypeNeeded = 1;
			switch (menuItem)
				{
	      	case kDisplaysMItem:
	      		gProcessorCode = kDisplayProcessor;
		  			DisplayImage ();
  		  			UnloadSeg (DisplayImage);
 	      		break;
 	      		
	      	case kHistogramMItem:
	      		gProcessorCode = kHistogramProcessor;
  		  			returnFlag = HistogramControl (1);
  		  			UnloadSeg (HistogramControl);
 	      		break;
 	      		
 	      	case kListDataMenuItem:
	      		
							// Make sure the graphics code is loaded.					
							
					ForceGraphCodeResourceLoad ();
					if (gMemoryTypeNeeded == 0)
						{
						gMemoryTypeNeeded = 1;
	      			gProcessorCode = kListDataProcessor;
 	      			ListDataControl ();
  		  				UnloadSeg (ListDataControl);
						
								// Unload the graphics code if it is not being used.
								
						if (gNumberOfGWindows == 0)
							UnloadSeg (ForceGraphCodeResourceLoad);
							
	      			gMemoryTypeNeeded = 0;
	      			
	      			}		// end "if (gMemoryTypeNeeded == 0)"
		      		
  		  			break;
 	      		
 	      	case kClusterMItem:
	      		gProcessorCode = kClusterProcessor;
 						
				 			// Load in project information file if needed first so 	
				 			// that the code will be at the bottom of the heap			
				 			// before the cluster code. 										
				 		
					if (gProjectInfoPtr == NULL)
						{
						ForceProjectSegmentsToBeLoaded ();
						
						if (gMemoryTypeNeeded >= 0)
							InitializeNewProject (NULL, gActiveImageWindowInfoH);
						
						}		// end "if ( gProjectInfoPtr == NULL )" 
						
					else		// gProjectInfoPtr != NULL 
						gProjectInfoPtr->newProjectFlag = FALSE;
								
					if (gMemoryTypeNeeded >= 0 && gProjectInfoPtr != NULL)
						{
						gMemoryTypeNeeded = 1;
 	      			ClusterControl ();
 	      			
 	      			}		// end "if (gMemoryTypeNeeded >= 0 && ...)" 
 	      			
  		  			UnloadSeg (ClusterControl);
  		  			
  		  			if (gProjectInfoPtr == NULL)
  		  				{
						UnloadSeg (StatisticsControl);
						UnloadSeg (ForceProjectUtilityCodeResourceLoad);
						UnloadSeg (ForceProjectCodeResourceLoad);
  		  				
  		  				}		// if (gProjectInfoPtr == NULL) 
 	      		break;
 	      		
 	      	case kStatisticsMItem:
	      		gProcessorCode = kStatisticsProcessor;
	      		
					ForceProjectSegmentsToBeLoaded ();
					
					if (gMemoryTypeNeeded >= 0)
 	      			StatisticsControl ();
						
						// If the 'Project' and 'Statistics' code resources 			
						// could not be loaded without using spare memory then		
						// unload the resources so that the spare code resource		
						// memory can be reclaimed.  The user has already been		
						// warned that memory is low.											
						
					if (gProjectInfoPtr == NULL)
						{
						UnloadSeg (StatisticsControl);
						UnloadSeg (ForceProjectUtilityCodeResourceLoad);
						UnloadSeg (OpenProjectFile);
						
						}		// end "if (gMemoryTypeNeeded < 0)" 
 	      		break;
 	      		
 	      	case kStatEnhanceMItem:
	      		gProcessorCode = kStatEnhanceProcessor;
 	      		StatisticsEnhanceControl ();
					gUpdateProjectMenuItemsFlag = TRUE;
  		  			UnloadSeg (StatisticsEnhanceControl);
 	      		break;
 	      		
 	      	case kFeatureExtractionMItem:
	      		gProcessorCode = kFeatureExtractionProcessor;
 	      		FeatureExtractionControl ();
  		  			UnloadSeg (FeatureExtractionControl);
  		  			UnloadSeg (Preprocess);
 	      		break;
 	      		
 	      	case kSeparabilityMItem:
	      		gProcessorCode = kSeparabilityProcessor;
 	      		SeparabilityControl ();
  		  			UnloadSeg (SeparabilityControl);
 	      		break;
 	      		
 	      	case kClassifyMItem:
	      		gProcessorCode = kClassifyProcessor;
				
	      				// Load in list results code segment.
	      				
	      		gMemoryTypeNeeded = 1;
					ForceListResultsCodeResourceLoad ();
			
					if (gMemoryTypeNeeded == 0)
						{				
								// The following call was put here to force the compiler
								// to work correctly.  Codewarrior 9.4.  This part
								// was being optimized out.
						ForceListResultsCodeResourceLoad ();
								
//		     			gMemoryTypeNeeded = 1;
//						ForceGAIACodeResourceLoad ();
					
						}		// end "if (gMemoryTypeNeeded == 0)" 
		
					if (gMemoryTypeNeeded == 0)
						{
	      			gMemoryTypeNeeded = 1;	
 	      			ClassifyControl ();
					
						}		// end "if (gMemoryTypeNeeded == 0)" 
						
  		  			UnloadSeg (ClassifyControl);
//  		  			UnloadSeg (ForceGAIACodeResourceLoad);
  		  			UnloadSeg (ForceListResultsCodeResourceLoad);
  		  			UnloadSeg (EchoClsfierControl);
  		  			UnloadSeg (DecisionTreeControl);
 	      		break;
 	      		
 	      	case kListResultsMItem:
	      		gProcessorCode = kListResultsProcessor;
 	      		ListResultsControl ();
  		  			UnloadSeg (ListResultsControl);
 	      		break;
 	      		
 	      	}		// end "switch (menuItem)" 
 	      	
	      gMemoryTypeNeeded = 0;
	      gProcessorCode = 0;
 	      break;
 	      	
 	   case kReformatMenuID:
	    
	      gMemoryTypeNeeded = 1;
	      
	      gProcessorCode = kReformatProcessor;
	      ReformatControl (menuItem);
  			UnloadSeg (ReformatControl);
//	  		UnloadSeg ( ForceGAIACodeResourceLoad );
	  		UnloadSeg ( AreasToThematicFileControl );
	  		UnloadSeg ( RectifyImageControl ); 
	  		UnloadSeg ( ForceDisplayCodeResourceLoad );
	  		UnloadSeg ( CovertMultispectralToThematicControl );
 	      	
	      gMemoryTypeNeeded = 0;
	      gProcessorCode = 0;
 	      break;
 	      	
 	   case kUtilitiesMenuID:
	    
	      gMemoryTypeNeeded = 1;
			switch (menuItem)
				{
 	      	case kPrincipalComponentsMenuItem:
	      		gProcessorCode = kPrincipalComponentsProcessor;
 	      		PrincipalComponentControl ();
  		  			UnloadSeg (PrincipalComponentControl);
  		  			break;
  		  			
  		  		case kStatisticsImageMenuItem:
	      		gProcessorCode = kStatisticsImageProcessor;
 	      		StatisticsImageControl ();
  		  			UnloadSeg (StatisticsImageControl);
  		  			break;
  		  			
  		  		case kListDescriptionMenuItem:
	      		gProcessorCode = kListDescriptionProcessor;
 	      		ListDescriptionInformation ();
  		  			UnloadSeg (ListDescriptionInformation);
  		  			break;
  		  			
 	      	case kCovarianceCheckMenuItem:
	      		gProcessorCode = kCovarianceCheckProcessor;
 	      		EvaluateCovariancesControl ();
  		  			UnloadSeg (EvaluateCovariancesControl);
  		  			break;
  		  			
  		  		case kCheckTransformMenuItem:
	      		gProcessorCode = kTransformCheckProcessor;
 	      		EvaluateTransformationControl ();
  		  			UnloadSeg (EvaluateTransformationControl);
  		  			break;
  		  			
  		  		case kBiPlotDataMenuItem:
	      		gProcessorCode = kBiPlotDataProcessor;
																							
							// Make sure the graphics code is loaded.					
							
					ForceGraphCodeResourceLoad ();
					if (gMemoryTypeNeeded == 0)
						{
						gMemoryTypeNeeded = 1;
 	      			BiPlotDataControl ();
  		  				UnloadSeg (BiPlotDataControl);
  		  				
  		  				}		// end "if (gMemoryTypeNeeded == 0)" 
  		  			break;
 	      		
 	      	}		// end "switch (menuItem)" 
 	      	
	      gMemoryTypeNeeded = 0;
	      gProcessorCode = 0;
 	      break;
/*				// The palette menu was disabled on 3/28/2014. It does not seem
					to be a useful feature at this time.
 	   case kPaletteMenuID:
 	   
					// Get pointer to palette display specifications.					
				
			displaySpecsPtr = GetActiveDisplaySpecsPtr (&displaySpecsH);
			
			redrawPaletteWindow = FALSE;
			paletteWindowChanged = FALSE;
			
			SInt16 classGroupCode = displaySpecsPtr->classGroupCode;
			
			if (displaySpecsPtr == NULL && menuItem != kShowClosePaletteW)
				menuItem = 100;
			
 	   	switch (menuItem)
 	   		{
 	   		case kSpectralClasses:
 	   			if ( displaySpecsPtr->classGroupCode != kClassDisplay )
 	   				{
 	   				paletteWindowChanged = TRUE;
						
						UpdateActiveImageLegend (kClassDisplay, kCallCreatePalette);
						gUpdateFileMenuItemsFlag = TRUE;
 	   				
 	   				}		// end "if ( displaySpecsPtr->classGroupCode..." 
 	   			break;
 	   			
 	   		case kInfoGroups:
 	   			if ( displaySpecsPtr->classGroupCode != kGroupDisplay )
 	   				{
 	   				paletteWindowChanged = TRUE;
						
						UpdateActiveImageLegend (kGroupDisplay, kCallCreatePalette);
						gUpdateFileMenuItemsFlag = TRUE;
 	   				
 	   				}		// end "if ( displaySpecsPtr->kGroupDisplay..." 
 	   			break;
 	   			
 	   		case kInvertPalette:
 	   			displaySpecsPtr->invertValues = !displaySpecsPtr->invertValues;
					displaySpecsPtr->paletteUpToDateFlag = FALSE;
					paletteWindowChanged = TRUE;
					
					if (gActiveImageWindow != NULL)
						InvalidateWindow (gActiveImageWindow, kImageFrameArea, FALSE);
					break;
					
				case kShowClosePaletteW:
					if (gPaletteWindow != NULL)
						ClosePaletteWindow (gPaletteWindow);
					
					else		// gPaletteWindow == NULL 
						{
						redrawPaletteWindow = TRUE;
 	   				paletteWindowChanged = TRUE;
 	   				
 	   				}		// end "else gPaletteWindow == NULL" 
 	      		break;
					
				case kBlink1MenuItem:
					if (gBlinkProcedure != kBlink1)
						{
 	   				displaySpecsPtr->paletteUpToDateFlag = FALSE;
						gBlinkProcedure = kBlink1;
						
						UpdateActiveImageLegend (classGroupCode, kCallCreatePalette);
						
						}		// end "if (gBlinkProcedure != kBlink1)" 
						
 	      		break;
					
				case kBlink2MenuItem:
					if (gBlinkProcedure != kBlink2)
						{
 	   				displaySpecsPtr->paletteUpToDateFlag = FALSE;
						gBlinkProcedure = kBlink2;
						
						UpdateActiveImageLegend (classGroupCode, kCallCreatePalette);
						
						}		// end "if (gBlinkProcedure != kBlink2)" 
						
 	      		break;
					
				case k8BitPaletteMenuItem:
 	      		
	      		gMemoryTypeNeeded = 1;
 	      		UpdatePaletteFor16and24BImage ();
	      		gMemoryTypeNeeded = 0;
						
 	      		break;
 	      		
 	   		}		// end "switch (menuItem)" 
						
					// Unlock the display specification handle if needed.				
			
			CheckAndUnlockHandle (displaySpecsH);
				
					// Update palette menu and redraw the palette if it exists.		
			
			gUpdatePaletteMenuItemsFlag = TRUE;
			UpdatePaletteWindow (paletteWindowChanged, redrawPaletteWindow);
 	      break;
*/ 	      	
	   case kOptionMenuID:		// the "Option" menu was selected 
	    
			switch (menuItem)
				{
 	      	case kMemoryStatus:
					gMultiSpecWorkflowInfo.workFlowCode = 0;		// 3
					if (gMultiSpecWorkflowInfo.workFlowCode == 0)
						MemorySummary ();
 	      		break;
 	      		
 	      	case kSwitchCrossCursor:
 	      		tempCursorPtr = gCursorPtr[kCross];
 	      		gCursorPtr[kCross] = gCursorPtr[kCross2];
 	      		gCursorPtr[kCross2] = tempCursorPtr;
 	      		break;
 	      		
 	      	case kDebug:
					gUpdateOptionsMenuItemsFlag = TRUE;
 	      		gTestFlag = !gTestFlag;
 	      		break;
 	      		
 	      	}		// end "switch (menuItem)" 
 	      break;
 	      	
 	   case kDefaultAreaUnitsMenuID:
	    
			gDefaultAreaUnits = menuItem + kSqKilometersUnitsMenuItem - 1;
			gUpdateOptionsMenuItemsFlag = TRUE;
			break;
	
		case kWindowMenuID:				// the "Windows" menu was selected 
		
			newOrHideFlag = FALSE;
			window = NULL;
	    
			switch (menuItem)
				{
				case kHideTheWindow:
		      	HideWindow (gTheActiveWindow);
					newOrHideFlag = TRUE;
					break;
					
				case kShowHideCoordinateView:
					ShowHideCoordinateView (gActiveImageWindowInfoH);
					break;
 	      		
 		      case	kSelectionGraphMenuItem:
 		      	SelectionGraphMenuItem ();
 	      		break;
					
				case kTextOutput:
					window = gOutputWindow;
					break;
					
				case kPaletteWindow:
					window = gPaletteWindow;
					break;
					
				case kStatisticsWItem:
					window = gProjectWindow;
					break;
		    
				default:
					window = gWindowList[menuItem-kTextOutput];
					break;
					
				}		// end "switch (menuItem)" 

			if (window != NULL && !newOrHideFlag)
				{
				ShowWindow (window);
				if (window != FrontWindow())  
					SelectWindow (window);
//				SetPort (window);
				SetPortWindowPort (window);
				
				}		// end "if (window != NULL && !newOrHideFlag)" 
			break;
 	      	
	  	default:		// just playing with the mouse 
	  		break;
	  		
	  	}		// end "switch (menuID)"
  		  	
  			// Move project memory if needed to keep from fragmenting the memory.
  					
  	if (gProjectInfoPtr && gProjectInfoPtr->moveMemoryFlag)
  		{
		PurgeSpace (&freeBytesPossible, &longestContBlock);
		if (freeBytesPossible > longestContBlock)
			{
  			UnlockProjectMemory (&gProjectInfoPtr, 0, &projectInfoHandle);
  			
			ForceProjectSegmentsToBeLoaded ();
  			
  			LockProjectMemory(projectInfoHandle, 0, &gProjectInfoPtr);
  		
  			gProjectInfoPtr->moveMemoryFlag = FALSE;
  			
  			}		// end "if (freeBytesPossible > longestContBlock)" 
  		  					
  		}		// end "else if (gProjectInfoPtr && ...->moveMemoryFlag)" 
  		
	gOperationCanceledFlag = FALSE;
		
			// Reset the global memory error values.										
			
	gMemoryError = noErr;
	gTextMemoryShortCode = 0;
	gMemoryAlertDisplayFlag = FALSE;
		
			// Try to bring the memory allocated to the spare memory handles		
			// up to the minimums.																
	
	if (!gDoneFlag) 
  		GetSpareMemory ();
	
	while (gInBackground && !gOSXCoreGraphicsFlag)
		returnFlag = CheckSomeEvents (osMask+activMask+updateMask);
		
	HiliteMenu (0);		// unhilite menu title 
	
}		// end "Menus" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ForceProjectSegmentsToBeLoaded
//
//	Software purpose:	The purpose of this routine is to load the project
//							code segments at one end of the application heap.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/12/1993
//	Revised By:			Larry L. Biehl			Date: 02/14/1993

void ForceProjectSegmentsToBeLoaded (void)

{
	SInt32											freeBytesPossible,
														longestContBlock;
	
	
   LoadProjectCodeSegments ();
		
	if (gMemoryTypeNeeded == 0)
		{
				// Verify that the code segments have not fragmented memory.	
				// If so then unload and reload.  This should load them at		
				// one end of the heap or the other.									
		
		PurgeSpace (&freeBytesPossible, &longestContBlock);
		if (freeBytesPossible > longestContBlock)
			{
			UnloadProjectCodeSegments ();
			
   		LoadProjectCodeSegments ();
			
			}		// end "if (freeBytesPossible > longestContBlock)" 
			
		}		// end "if (gMemoryTypeNeeded == 0)" 
						
			// If the 'Project', ProjectUtility, or 'Statistics' code 			
			// resources could not be loaded without using spare memory then	
			// unload the resources so that the spare code resource				
			// memory can be reclaimed.  The user has already been				
			// warned that memory is low.													
		
	if (gMemoryTypeNeeded < 0)
		UnloadProjectCodeSegments ();
	
}		// end "ForceProjectSegmentsToBeLoaded" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				void LoadProjectCodeSegments
//
//	Software purpose:			The purpose of this routine is to load the code segments
//									related to project capability.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/14/1993
//	Revised By:			Larry L. Biehl			Date: 02/14/1993

void LoadProjectCodeSegments (void)

{
			// Make certaint that statistics code segment is loaded.				
      				
   gMemoryTypeNeeded = 1;
	ForceStatisticsCodeResourceLoad ();

			// Make certaint that project utility code segment is loaded.		

	if (gMemoryTypeNeeded == 0)
		{
      gMemoryTypeNeeded = 1;
		ForceProjectUtilityCodeResourceLoad ();
		
		}		// end "if (gMemoryTypeNeeded == 0)" 

			// Make certaint that project code segment is loaded.					

	if (gMemoryTypeNeeded == 0)
		{
      gMemoryTypeNeeded = 1;
		ForceProjectCodeResourceLoad ();
		
		}		// end "if (gMemoryTypeNeeded == 0)" 
	
}		// end "LoadProjectCodeSegments" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SaveOutputMenuItem
//
//	Software purpose:	The purpose of this routine is to save handle the
//							'Save' menu item.  This routine will call the
//							appropriate routine depending upon which window is
//							active.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/10/1992
//	Revised By:			Larry L. Biehl			Date: 09/08/2006

void SaveOutputMenuItem (void)

{
	switch (gActiveWindowType)
		{
		case kOutputWindowType:
			SaveTextOutputAs ();
			break;
			
		case kThematicWindowType:
		case kImageWindowType:
			SaveImageWindowAs ();
			break;
			
		}		// end "switch (gActiveWindowType)" 
	
}		// end "SaveOutputMenuItem" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SelectionGraphMenuItem
//
//	Software purpose:	The purpose of this routine is to handle the 
//							'New Selection Graph' menu item.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/27/1992
//	Revised By:			Larry L. Biehl			Date: 02/15/1993	

void SelectionGraphMenuItem (void)

{
	SInt32								freeBytesPossible,
											longestContBlock;
 	
 	      
	if (gActiveImageWindow != NULL)
		{
		gProcessorCode = kSelectionGraphProcessor;
		
				// Make certain that the graphics code resourses are loaded.		
				
		if (gNumberOfGWindows == 0)
			{
			gMemoryTypeNeeded = 1;
			ForceGraphCodeResourceLoad ();
			
					// This should force the code to be at one end of the heap		
					// not the middle.															
					
			PurgeSpace (&freeBytesPossible, &longestContBlock);
			if (freeBytesPossible > longestContBlock)
				{
				UnloadSeg (ForceGraphCodeResourceLoad);
				ForceGraphCodeResourceLoad ();
				
				}		// end "if (freeBytesPossible > longestContBlock)" 
				
			}		// end "if (gNumberOfGWindows == 0)" 
		
		if (gMemoryTypeNeeded == 0)
			{
			SelectionGraphControl(NULL);
			
			}		// end "if (gMemoryTypeNeeded == 0)" 
			
		else		// gMemoryTypeNeeded != 0 
			{
			UnloadSeg (ForceGraphCodeResourceLoad);
			
			}		// else gMemoryTypeNeeded != 0 
		
		gMemoryTypeNeeded = 0;
		gProcessorCode = 0;
				
		}		// end "if (gActiveImageWindow != NULL)" 
		
}		// end "SelectionGraphMenuItem"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ShowHideCoordinateView
//
//	Software purpose:	The purpose of this routine is to change the current visibility
//							of the coordinate view in the input image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/02/2000
//	Revised By:			Larry L. Biehl			Date: 09/08/2006	

void ShowHideCoordinateView (
				Handle								windowInfoHandle)

{
	if (windowInfoHandle != NULL)
		{
		UInt32 coordinateHeight = GetCoordinateHeight (windowInfoHandle);
		
		if (coordinateHeight > 0)
			{
			SetCoordinateHeight (windowInfoHandle, 0);
			SetControlTitle (
						((WindowInfoPtr)*windowInfoHandle)->coordinateViewControl, "\p-");
			
			}
			
		else		// coordinateHeight == 0)
			{
			SetCoordinateHeight (windowInfoHandle, 25);
			SetControlTitle (
						((WindowInfoPtr)*windowInfoHandle)->coordinateViewControl, "\p");
		
					// Set the coordinate item location parameters
					
			SetCoordinateViewLocationParameters (windowInfoHandle);
		 							
		 	}		// end "else coordinateHeight == 0)"
		 	
		WindowPtr window = GetWindowPtr (windowInfoHandle);
		
		if (GetWindowType (windowInfoHandle) == kThematicWindowType)
			AdjustLegendListLength (window, TRUE);
		 							
		InvalidateWindow (window, kFrameArea, TRUE);
		
		UpdateImageWScrolls (window); 
		DrawScrollBars (window);
		
		UpdateCoordinateViewControls (window);
		
		UpdateScaleInformation (windowInfoHandle);
		
		gUpdateWindowsMenuItemsFlag = TRUE;
			
		}		// end "if (windowInfoHandle != NULL)"
		
}		// end "ShowHideCoordinateView"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ShowMultiSpecMenuBar
//
//	Software purpose:	The purpose of this routine is to enable all MultiSpec
//							menu bar items.
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
//	Revised By:			Larry L. Biehl			Date: 03/15/1993	

void ShowMultiSpecMenuBar (void)

{
	EnableMenuItem (gMultiSpecMenus[kFileM], 0);
	EnableMenuItem (gMultiSpecMenus[kEditM], 0);
	EnableMenuItem (gMultiSpecMenus[kWindowM], 0);
	EnableMenuItem (gMultiSpecMenus[kProjectM], 0);
	EnableMenuItem (gMultiSpecMenus[kProcessorM], 0);
	EnableMenuItem (gMultiSpecMenus[kPaletteM], 0);
	EnableMenuItem (gMultiSpecMenus[kOptionM], 0);
	
	gRedrawMenuBar = TRUE;
		
}		// end "ShowMultiSpecMenuBar"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				void UnloadProjectCodeSegments
//
//	Software purpose:			The purpose of this routine is to unload the project
//									code segments.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/14/1993
//	Revised By:			Larry L. Biehl			Date: 02/14/1993

void UnloadProjectCodeSegments (void)

{
	UnloadSeg ( ForceStatisticsCodeResourceLoad );
	UnloadSeg ( ForceProjectUtilityCodeResourceLoad );
	UnloadSeg ( ForceProjectCodeResourceLoad );
		
}		// end "UnloadProjectCodeSegments" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateEditMenuItems
//
//	Software purpose:	The purpose of this routine is to update the items
//							in the edit menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			DoEdit in menus.c
//							DoActivateEvent in multiSpec.c
//							DoOutputWActivateEvent in multiSpec.c
//							DoMouseDnContent in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 04/25/1991
//	Revised By:			Larry L. Biehl			Date: 03/02/1997

void UpdateEditMenuItems (void)

{																			
  			// Change names of edit menu items back to defaults in case they	
  			// have been changed.															
  			
	SetMenuItemText (gMultiSpecMenus[kEditM], kUndo,  "\pUndo");
	SetMenuItemText (gMultiSpecMenus[kEditM], kCut,   "\pCut");
	SetMenuItemText (gMultiSpecMenus[kEditM], kCopy,  "\pCopy");
	SetMenuItemText (gMultiSpecMenus[kEditM], kPaste, "\pPaste");
	SetMenuItemText (gMultiSpecMenus[kEditM], kSelectAll, "\pSelect All");
  	
			// Disable the edit menu items												
				
  	DisableMenuItem (gMultiSpecMenus[kEditM], kUndo);
  	DisableMenuItem (gMultiSpecMenus[kEditM], kCut);
  	DisableMenuItem (gMultiSpecMenus[kEditM], kCopy);
  	DisableMenuItem (gMultiSpecMenus[kEditM], kPaste);
  	DisableMenuItem (gMultiSpecMenus[kEditM], kClear);
  	DisableMenuItem (gMultiSpecMenus[kEditM], kSelectAll);	
  	
	DisableMenuItem (gMultiSpecMenus[kEditM], kEditSelectionRect);
	DisableMenuItem (gMultiSpecMenus[kEditM], kClearSelectionRect);
						
	if (gTransformationMatrix.numberChannels > 0)
		EnableMenuItem (gMultiSpecMenus[kEditM], kClearTransformMenuItem);
 		
 	else		// gTransformationMatrix.numberChannels <= 0
		DisableMenuItem (gMultiSpecMenus[kEditM], kClearTransformMenuItem);
		
	
	if (gActiveImageWindow != NULL)
		EnableMenuItem (gMultiSpecMenus[kEditM], kChangeSpecs);
		
	else		// gActiveImageWindow == NULL
		DisableMenuItem (gMultiSpecMenus[kEditM], kChangeSpecs);
			
	
	if ( UpdateEditImageMapParameters () )
		EnableMenuItem (gMultiSpecMenus[kEditM], kCoordinates);
		
	else		// !UpdateEditImageMapParameters ()
		DisableMenuItem (gMultiSpecMenus[kEditM], kCoordinates);
			
	
	if ( UpdateEditClearOverlays (NULL) )
		EnableMenuItem (gMultiSpecMenus[kEditM], kClearOverlays);
		
	else		// !UpdateEditClearOverlays ()
		DisableMenuItem (gMultiSpecMenus[kEditM], kClearOverlays);
		
}		// end "UpdateEditMenuItems" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateFileMenuItems ()
//
//	Software purpose:	The purpose of this routine is to update the items
//							in the file menu dealing with output window text
//							items.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			DoEdit in menus.c
//
//	Coded By:			Larry L. Biehl			Date: 12/18/1990
//	Revised By:			Larry L. Biehl			Date: 01/03/2015	

void UpdateFileMenuItems (void)

{
	SInt16 								classGroupCode,
											windowType;
	
			
	windowType = GetWindowType (gActiveWindowInfoH);
	classGroupCode = GetDisplayClassGroupCode (gActiveWindowInfoH);
	
	if (gProjectInfoPtr == NULL && gFullVersionFlag)
		{
		if (DetermineIfProjectPossible (gActiveWindowInfoH) )
			EnableMenuItem (gMultiSpecMenus[kFileM], kNewProject);
			
		else		// gActiveImageWindowInfoH != NULL && ...
			DisableMenuItem (gMultiSpecMenus[kFileM], kNewProject);
		
		EnableMenuItem (gMultiSpecMenus[kFileM], kOpenProject);
		
		}		// end "if (gProjectInfoPtr == NULL)"
		
	else		// gProjectInfoPtr != NULL 
		{
		DisableMenuItem (gMultiSpecMenus[kFileM], kNewProject);
		DisableMenuItem (gMultiSpecMenus[kFileM], kOpenProject);				
		
		}		// end "else gProjectInfoPtr != NULL"
	
			// Open Image Menu item.
			//  Check if at maximum number of windows image windows.				
			
	if (gNumberOfIWindows >= kMaxNumberIWindows)
		DisableMenuItem (gMultiSpecMenus[kFileM], kOpenImage);
		
	else		// gNumberOfIWindows < kMaxNumberIWindows
		EnableMenuItem (gMultiSpecMenus[kFileM], kOpenImage);
		
			// Open Project Image Menu item.
			
	if (gProjectInfoPtr == NULL || gNumberOfIWindows >= kMaxNumberIWindows)
		DisableMenuItem (gMultiSpecMenus[kFileM], kOpenProjImage);
	
	else		// gProjectInfoPtr != NULL && gNumberOfIWindows >= ...
		EnableMenuItem (gMultiSpecMenus[kFileM], kOpenProjImage);
		
			// Open Thematic Class/Group Info Menu item.
			
	if (UpdateFileOpenThematicInfo (NULL))
		EnableMenuItem (gMultiSpecMenus[kFileM], kOpenThematicClassGroup);
		
	else		// !UpdateFileOpenThematicInfo (NULL)
		DisableMenuItem (gMultiSpecMenus[kFileM], kOpenThematicClassGroup);
		
			// Load Transformation Matrix Menu item.
			
	if ( UpdateFileTransform (NULL) )
		EnableMenuItem (gMultiSpecMenus[kFileM], kSaveTransformMenuItem);
		
	else		// !UpdateFileTransform (NULL)
		DisableMenuItem (gMultiSpecMenus[kFileM], kSaveTransformMenuItem);
		
	if (gProjectInfoPtr == NULL)
		{
		DisableMenuItem (gMultiSpecMenus[kFileM], kSaveProject);
		DisableMenuItem (gMultiSpecMenus[kFileM], kSaveProjectAs);
		
		}		// end "if (gProjectInfoPtr == NULL)"
		
	else		// gProjectInfoPtr != NULL 
		{
		if (gProjectInfoPtr->changedFlag)
			EnableMenuItem (gMultiSpecMenus[kFileM], kSaveProject);
			
		else		// !gProjectInfoPtr->changedFlag 
			DisableMenuItem (gMultiSpecMenus[kFileM], kSaveProject);
			
		EnableMenuItem (gMultiSpecMenus[kFileM], kSaveProjectAs);						
		
		}		// end "else gProjectInfoPtr != NULL"
		
			// Load File Save Menu item.
	
	if ( UpdateFileSave (NULL) )
		EnableMenuItem (gMultiSpecMenus[kFileM], kSave);
		
	else		// !UpdateFileSave (NULL)
		DisableMenuItem (gMultiSpecMenus[kFileM], kSave);
				
}		// end "UpdateFileMenuItems"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateGraphicsMenuBeforeSelection
//
//	Software purpose:	The purpose of this routine is to update the items
//							in the options menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1995
//	Revised By:			Larry L. Biehl			Date: 11/10/2000	

void UpdateGraphicsMenuBeforeSelection (
				WindowInfoPtr						windowInfoPtr,
				WindowInfoPtr						activeImageWindowInfoPtr)

{
			// File Menu
			
	if (gUpdateFileMenuItemsFlag)
		{	
		UpdateFileGraphClose (NULL);
								
		UpdateFileMenuItems ();
		
		EnableMenuItem (gMultiSpecMenus[kFileM], kCloseImage);
	 	DisableMenuItem (gMultiSpecMenus[kFileM], kSaveOutputAs);
	 	EnableMenuItem (gMultiSpecMenus[kFileM], kPrintOutput);
	 	
	 	UpdateFileDefaultSaveAs (NULL);

		UpdateFileGraphPrint (NULL);
		
		gUpdateFileMenuItemsFlag = FALSE;
		
		}		// end "if (gUpdateFileMenuItemsFlag)"
		
			// Edit Menu
			
	if (gUpdateEditMenuItemsFlag)
		{
		UpdateEditMenuItems ();
		
		UpdateEditGraphicsCopy (NULL);
		
		EnableMenuItem (gMultiSpecMenus[kEditM], kCopy);
		
		gUpdateEditMenuItemsFlag = FALSE;
		
		}		// end "if (gUpdateEditMenuItemsFlag)"
	
	UpdateProjectMenuItems (activeImageWindowInfoPtr);
	
	UpdateProcessorMenuItems (activeImageWindowInfoPtr);
	
	UpdateOptionsMenuItems ();
	
//	UpdatePaletteMenuItems (activeImageWindowInfoPtr);
	
	UpdateWindowMenuItems (windowInfoPtr);
		
}		// end "UpdateGraphicsMenuBeforeSelection"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateImageMenuBeforeSelection
//
//	Software purpose:	The purpose of this routine is to update the items
//							in the options menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1995
//	Revised By:			Larry L. Biehl			Date: 12/20/2001	

void UpdateImageMenuBeforeSelection (
				WindowInfoPtr						windowInfoPtr,
				WindowInfoPtr						activeImageWindowInfoPtr)

{
			// File Menu
			
	if (gUpdateFileMenuItemsFlag)
		{
		UpdateFileImageClose (NULL);
		
		UpdateFileMenuItems ();
				
		EnableMenuItem (gMultiSpecMenus[kFileM], kCloseImage);
		
				// Save image as TIFF file.
				
		if ( UpdateFileImageSaveAs (NULL, windowInfoPtr) )
	 		EnableMenuItem (gMultiSpecMenus[kFileM], kSaveOutputAs);
	 		
	 	else		// !UpdateFileImageSaveAs (NULL, windowInfoPtr)
	 		DisableMenuItem (gMultiSpecMenus[kFileM], kSaveOutputAs);
			
				// Print image.
				
		if ( UpdateFileImagePrint (NULL, windowInfoPtr) && !gOSXPre10_1Flag)
	 		EnableMenuItem (gMultiSpecMenus[kFileM], kPrintOutput);
	 		
	 	else		// !UpdateFileImagePrint (NULL, windowInfoPtr) 
	 		DisableMenuItem (gMultiSpecMenus[kFileM], kPrintOutput);
			
		gUpdateFileMenuItemsFlag = FALSE;
		
		}		// end "if (gUpdateFileMenuItemsFlag)"
		
			// Edit Menu
			
	if (gUpdateEditMenuItemsFlag)
		{
		UpdateEditMenuItems ();
										
		if ( !gOSXFlag && UpdateEditImageCopy (NULL, windowInfoPtr) )
//		if ( UpdateEditImageCopy (NULL, windowInfoPtr) )
			EnableMenuItem (gMultiSpecMenus[kEditM], kCopy);		
			
		Boolean			rectangularSelectionFlag = FALSE;
     	if ( (gActiveImageWindow != gProjectSelectionWindow) || 
     				(gProjectInfoPtr != NULL &&
     						gActiveImageWindow == gProjectSelectionWindow &&
	 								gProjectInfoPtr->selectionType == kRectangleType) )
	 		rectangularSelectionFlag = TRUE;			
     			
	   UpdateEditImageSelectAll (NULL);
		if (gActiveOffscreenMapExists && rectangularSelectionFlag)
	 		EnableMenuItem (gMultiSpecMenus[kEditM], kSelectAll);
     	
     	if (rectangularSelectionFlag)
			EnableMenuItem (gMultiSpecMenus[kEditM], kEditSelectionRect);
     	
     	SInt16 selectionTypeCode = GetSelectionTypeCode (windowInfoPtr); 
     	UpdateEditClearSelection (NULL, selectionTypeCode);
		if ( selectionTypeCode != 0 )
			EnableMenuItem (gMultiSpecMenus[kEditM], kClearSelectionRect);
		
		gUpdateEditMenuItemsFlag = FALSE;
		
		}		// end "if (gUpdateEditMenuItemsFlag)"
		
			// Window Menu
			
	if (gUpdateWindowsMenuItemsFlag)
		{
		UpdateWindowMenuItems (windowInfoPtr);
   
	   SInt16 windowMenuItem = 0;
	   if (windowInfoPtr != NULL)
	   	windowMenuItem = windowInfoPtr->windowMenuItem;
	   		
	   		// Make the outlined window item up-to-date.
	   
	   if (gOutlinedWindowMenuItem != windowMenuItem)
	   	{
	   	if (gOutlinedWindowMenuItem != 0)
	     		SetItemStyle (gMultiSpecMenus[kWindowM], 
	     							gOutlinedWindowMenuItem, 
	     							0);
	     		
	   	if (windowMenuItem != 0)
	  			SetItemStyle (gMultiSpecMenus[kWindowM], 
	  								windowMenuItem, 
	  								outline);
	  								
		 	gOutlinedWindowMenuItem = windowMenuItem;
		 	
		 	}		// end " if (gOutlinedWindowMenuItem != windowMenuItem)"
		 	
		 }		// end "if (gUpdateWindowsMenuItemsFlag)"
	
	UpdateProjectMenuItems (activeImageWindowInfoPtr);
	
	UpdateProcessorMenuItems (activeImageWindowInfoPtr);
	
//	UpdatePaletteMenuItems (activeImageWindowInfoPtr);
	
	UpdateOptionsMenuItems ();
		
}		// end "UpdateImageMenuBeforeSelection"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateMenuBeforeSelection
//
//	Software purpose:	The purpose of this routine is to update the MultiSpec
//							menu bar to be ready for a selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1995
//	Revised By:			Larry L. Biehl			Date: 09/08/2006

void UpdateMenuBeforeSelection (void)

{
	WindowInfoPtr						activeImageWindowInfoPtr,
											windowInfoPtr;
		
	
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
														gActiveWindowInfoH,
														kLock,
														kNoMoveHi);
														
	activeImageWindowInfoPtr = (WindowInfoPtr)GetHandlePointer(
														gActiveImageWindowInfoH,
														kLock,
														kNoMoveHi);
	
	switch (gActiveWindowType)
		{															
		case kOutputWindowType: 
			UpdateOutputTextMenuBeforeSelection(windowInfoPtr, activeImageWindowInfoPtr);
			break;
		
		case kImageWindowType:
		case kThematicWindowType:
			UpdateImageMenuBeforeSelection (windowInfoPtr, activeImageWindowInfoPtr);
			break;
		
		case kStatisticsWindowType:
			UpdateStatisticsMenuBeforeSelection (windowInfoPtr, activeImageWindowInfoPtr);
			break;
		
		case kPaletteWindowType:
			UpdatePaletteMenuBeforeSelection (windowInfoPtr, activeImageWindowInfoPtr);
			break;
		
		case kGraphicsWindowType:
			UpdateGraphicsMenuBeforeSelection (windowInfoPtr, activeImageWindowInfoPtr);
			break;
			
		default:
			UpdateMenuBeforeSelectionForNoWindow (windowInfoPtr, activeImageWindowInfoPtr);
			break;
		
		}		// end "switch (gActiveWindowType)"
	
	CheckAndUnlockHandle (gActiveImageWindowInfoH);
	
	CheckAndUnlockHandle (gActiveWindowInfoH);
		
}		// end "UpdateMenuBeforeSelection"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateMenuBeforeSelectionForNoWindow
//
//	Software purpose:	The purpose of this routine is to update the menu items
//							for the case when there is no active window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1995
//	Revised By:			Larry L. Biehl			Date: 11/10/2000	

void UpdateMenuBeforeSelectionForNoWindow (
				WindowInfoPtr						windowInfoPtr,
				WindowInfoPtr						activeImageWindowInfoPtr)

{
			// File Menu
			
	if (gUpdateFileMenuItemsFlag)
		{	
		UpdateFileTextClose (NULL);
		
		UpdateFileMenuItems ();
		
		DisableMenuItem (gMultiSpecMenus[kFileM], kCloseImage);
	 	DisableMenuItem (gMultiSpecMenus[kFileM], kSaveOutputAs);
	 	DisableMenuItem (gMultiSpecMenus[kFileM], kPrintOutput);
	 	
	 	UpdateFileDefaultSaveAs (NULL);
	 	UpdateFileDefaultPrint (NULL);
			
		gUpdateFileMenuItemsFlag = FALSE;
		
		}		// end "if (gUpdateFileMenuItemsFlag)"
		
			// Edit Menu
			
	if (gUpdateEditMenuItemsFlag)
		{
		UpdateEditMenuItems ();
		
		DisableMenuItem (gMultiSpecMenus[kEditM], kChangeSpecs);
		
		gUpdateEditMenuItemsFlag = FALSE;
		
		}		// end "if (gUpdateEditMenuItemsFlag)"
	
	UpdateWindowMenuItems (windowInfoPtr);
	
	UpdateProjectMenuItems (activeImageWindowInfoPtr);
	
	UpdateProcessorMenuItems (activeImageWindowInfoPtr);
	
//	UpdatePaletteMenuItems (activeImageWindowInfoPtr);
	
	UpdateOptionsMenuItems ();
		
}		// end "UpdateMenuBeforeSelectionForNoWindow"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateOptionsMenuItems
//
//	Software purpose:	The purpose of this routine is to update the items
//							in the options menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/27/2003
//	Revised By:			Larry L. Biehl			Date: 02/27/2003

void UpdateOptionsMenuItems ()

{
	if (!gUpdateOptionsMenuItemsFlag)
																						return;
   		
   		// Make the checked area units item up-to-date.
   	
   if (gCheckedAreaUnitsMenuItem != gDefaultAreaUnits-kAreaUnknownUnitsMenuItem)
   	{
   	if (gCheckedAreaUnitsMenuItem != 0)
	 		CheckMenuItem (gMultiSpecMenus[kAreaUnitsM], 
	 								gCheckedAreaUnitsMenuItem, 
	 								FALSE);
	 		
	 	gCheckedAreaUnitsMenuItem = gDefaultAreaUnits-kAreaUnknownUnitsMenuItem;
   	if (gCheckedAreaUnitsMenuItem != 0)
	 		CheckMenuItem (gMultiSpecMenus[kAreaUnitsM], 
	 								gCheckedAreaUnitsMenuItem, 
	 								TRUE);
	 	
	 	}		// end " if (gCheckedAreaUnitsMenuItem != gDefaultAreaUnits)"
	
	if (gIncludeTestMenuItemFlag)
		{	
				// Test menu item is currently not used.
			
		if (gTestFlag)
			CheckMenuItem (gMultiSpecMenus[kOptionM], kDebug, TRUE);
			
		else		// !gTestFlag 
			CheckMenuItem (gMultiSpecMenus[kOptionM], kDebug, FALSE);
			
		}		// end "if (gIncludeTestMenuItemFlag"
	 	
	gUpdateOptionsMenuItemsFlag = FALSE; 
		
}		// end "UpdateOptionsMenuItems"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateOutputTextMenuBeforeSelection
//
//	Software purpose:	The purpose of this routine is to update the items
//							in the options menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1995
//	Revised By:			Larry L. Biehl			Date: 12/30/2008	

void UpdateOutputTextMenuBeforeSelection (
				WindowInfoPtr						windowInfoPtr,
				WindowInfoPtr						activeImageWindowInfoPtr)

{		
			// File Menu
			
	#if use_mlte_for_text_window
				// Always update the file menu and edit menu items for MLTE.
				
		gUpdateFileMenuItemsFlag = TRUE;
		gUpdateEditMenuItemsFlag = TRUE;
	#endif
			
	if (gUpdateFileMenuItemsFlag)
		{
		UpdateFileTextClose (NULL);
								
		UpdateFileMenuItems ();
			
		DisableMenuItem (gMultiSpecMenus[kFileM], kCloseImage);
			
				// Determine if text is selected.  If so change the menu items			
				// for 'Save Output Text' and 'Print Text' to indicate that				
				// Selected text will be used.
				
	   		// Save Output As
	   		   
	   if ( UpdateFileOutputTextSaveAs (NULL) )
	      EnableMenuItem (gMultiSpecMenus[kFileM], kSaveOutputAs);
	      
	   else		// !UpdateFileOutputTextSaveAs (NULL)
	      DisableMenuItem (gMultiSpecMenus[kFileM], kSaveOutputAs); 
	      
	      	// Print text output
	      
	   if ( UpdateFileOutputTextPrint (NULL) )
	      EnableMenuItem (gMultiSpecMenus[kFileM], kPrintOutput);
	      
	   else		// !UpdateFileOutputTextPrint (NULL)
	      DisableMenuItem (gMultiSpecMenus[kFileM], kPrintOutput);
			
		gUpdateFileMenuItemsFlag = FALSE;
		
		}		// end "if (gUpdateFileMenuItemsFlag)"
		
			// Edit Menu
			
	if (gUpdateEditMenuItemsFlag)
		{
		UpdateEditMenuItems ();
		
		UpdateEditTextPaste (NULL);
		
		#if use_mlte_for_text_window
			if (TXNCanUndo (gOutputTextH, NULL))
//			if (TXNCanUndoAction (gOutputTextH, &actionName)) // Only available in OSX 10.4 or later
				EnableMenuItem (gMultiSpecMenus[kEditM], kUndo); 
		#endif
		
		#if use_mlte_for_text_window
			if (TXNIsScrapPastable ())
				EnableMenuItem (gMultiSpecMenus[kEditM], kPaste); 
		#endif
		#if !use_mlte_for_text_window
			EnableMenuItem (gMultiSpecMenus[kEditM], kPaste); 
		#endif
     	
		Boolean charactersSelectedFlag = CharactersSelected ();
		#if use_mlte_for_text_window
			if (TXNDataSize (gOutputTextH) > 2 && charactersSelectedFlag) 
		#endif
		#if !use_mlte_for_text_window
			if (WEGetTextLength(gOutputTextH) > 2 && charactersSelectedFlag)
		#endif
				{
				UpdateEditTextCut (NULL);
				UpdateEditTextCopy (NULL);
				UpdateEditTextClear (NULL);
			
				EnableMenuItem (gMultiSpecMenus[kEditM], kCut);
				EnableMenuItem (gMultiSpecMenus[kEditM], kCopy);
				EnableMenuItem (gMultiSpecMenus[kEditM], kClear);
				
				}		// end "if (*gOutputTextH)->tLength > 2 && ..."
	   	 
	   UpdateEditTextSelectAll (NULL);	
		
		#if use_mlte_for_text_window
			if (TXNDataSize (gOutputTextH) > 2) 
		#endif
		#if !use_mlte_for_text_window
			if (WEGetTextLength(gOutputTextH) > 2)
		#endif
				EnableMenuItem (gMultiSpecMenus[kEditM], kSelectAll);
		
		gUpdateEditMenuItemsFlag = FALSE;
		
		}		// end "if (gUpdateEditMenuItemsFlag)"
	
	UpdateWindowMenuItems (windowInfoPtr);
	
	UpdateProjectMenuItems (activeImageWindowInfoPtr);
	
	UpdateProcessorMenuItems (activeImageWindowInfoPtr);
	
//	UpdatePaletteMenuItems (activeImageWindowInfoPtr);
	
	UpdateOptionsMenuItems ();
		
}		// end "UpdateOutputTextMenuBeforeSelection"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdatePaletteMenuBeforeSelection
//
//	Software purpose:	The purpose of this routine is to update the menu items
//							for the case when there is no active window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1995
//	Revised By:			Larry L. Biehl			Date: 11/10/2000	

void UpdatePaletteMenuBeforeSelection (
				WindowInfoPtr						windowInfoPtr,
				WindowInfoPtr						activeImageWindowInfoPtr)

{
			// File Menu
			
	if (gUpdateFileMenuItemsFlag)
		{
		UpdateFilePaletteClose (NULL);
		
		UpdateFileMenuItems ();
			
		EnableMenuItem (gMultiSpecMenus[kFileM], kCloseImage);
	 	DisableMenuItem (gMultiSpecMenus[kFileM], kSaveOutputAs);
	 	DisableMenuItem (gMultiSpecMenus[kFileM], kPrintOutput);
	 	
	 	UpdateFileDefaultSaveAs (NULL);
	 	UpdateFileDefaultPrint (NULL);
			
		gUpdateFileMenuItemsFlag = FALSE;
		
		}		// end "if (gUpdateFileMenuItemsFlag)"
		
			// Edit Menu
			
	if (gUpdateEditMenuItemsFlag)
		{
		UpdateEditMenuItems ();
		
		gUpdateEditMenuItemsFlag = FALSE;
		
		}		// end "if (gUpdateEditMenuItemsFlag)"
	
	UpdateWindowMenuItems (windowInfoPtr);
	
	UpdateProjectMenuItems (activeImageWindowInfoPtr);
	
	UpdateProcessorMenuItems (activeImageWindowInfoPtr);
	
//	UpdatePaletteMenuItems (activeImageWindowInfoPtr);
	
	UpdateOptionsMenuItems ();
		
}		// end "UpdatePaletteMenuBeforeSelection"
 

/*	Disabled feature on 3/28/2014
//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdatePaletteMenuItems
//
//	Software purpose:	The purpose of this routine is to update the items
//							in the palette menu.  This will be called after the
// 						display dialog is called, the active image window
//							changes or the palette for the active image window
//							changes.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/08/1989
//	Revised By:			Larry L. Biehl			Date: 02/05/2001	

void UpdatePaletteMenuItems (
				WindowInfoPtr						activeImageWindowInfoPtr)

{
			// Declare local variables.				
	
	DisplaySpecsPtr					displaySpecsPtr;
	GWorldPtr							offscreenGWorld;
	
	Handle								displaySpecsH,
											imageBaseAddressH;
											
	SInt16								classGroupCode,
											grafPortType,
											tolerance,
											usage,
											windowType;
										
										
	if (!gUpdatePaletteMenuItemsFlag)
																						return;
	
	imageBaseAddressH = NULL;
	offscreenGWorld = NULL;
	
	if (gActiveImageWindowInfoH != NULL && 
									gActiveImageWindowInfoH == gActiveWindowInfoH)
		{
		imageBaseAddressH = activeImageWindowInfoPtr->imageBaseAddressH;
		
		offscreenGWorld = activeImageWindowInfoPtr->offscreenGWorld;
					
		}		// end "if (gActiveImageWindowInfoH != NULL && ...)" 
	
	if (imageBaseAddressH == NULL && offscreenGWorld == NULL)
		{
				// Turn off palette menu items if no image window is displayed.	
				
		CheckMenuItem ( gMultiSpecMenus[kPaletteM], kSpectralClasses, FALSE );
		DisableMenuItem (gMultiSpecMenus[kPaletteM], kSpectralClasses);
		CheckMenuItem ( gMultiSpecMenus[kPaletteM], kInfoGroups, FALSE );
		DisableMenuItem (gMultiSpecMenus[kPaletteM], kInfoGroups);
		
		CheckMenuItem ( gMultiSpecMenus[kPaletteM], kInvertPalette, FALSE );
		DisableMenuItem (gMultiSpecMenus[kPaletteM], kInvertPalette);
		
		if (gPaletteWindow == NULL)
			DisableMenuItem (gMultiSpecMenus[kPaletteM], kShowClosePaletteW);
				
		CheckMenuItem ( gMultiSpecMenus[kPaletteM], kBlink1MenuItem, FALSE );
		DisableMenuItem (gMultiSpecMenus[kPaletteM], kBlink1MenuItem);
		
		CheckMenuItem ( gMultiSpecMenus[kPaletteM], kBlink2MenuItem, FALSE );
		DisableMenuItem (gMultiSpecMenus[kPaletteM], kBlink2MenuItem);
				
		CheckMenuItem ( gMultiSpecMenus[kPaletteM], k8BitPaletteMenuItem, FALSE );
		DisableMenuItem (gMultiSpecMenus[kPaletteM], k8BitPaletteMenuItem);
		
		}		// end "if (imageBaseAddressH == NULL && offscreenGWorld ...)" 
		
			// Check if any changes need to be made.										
			
	else		// imageBaseAddressH != NULL || offscreenGWorld != NULL
		{
				// Get pointer to palette display specifications.						
				
		displaySpecsPtr = GetActiveDisplaySpecsPtr (&displaySpecsH);
		
		if (displaySpecsPtr != NULL)
			{		
	 	      	// Get the window type.														
	 	      	
			windowType = activeImageWindowInfoPtr->windowType;
			grafPortType = activeImageWindowInfoPtr->grafPortType;
			
			if (windowType == kThematicWindowType)
				{
				classGroupCode = displaySpecsPtr->classGroupCode;
				usage = 2;
				if (displaySpecsPtr->paletteObject)
					GetEntryUsage ( displaySpecsPtr->paletteObject, 3, &usage, &tolerance);
				
				CheckMenuItem ( gMultiSpecMenus[kPaletteM], kSpectralClasses, 
													(classGroupCode == kClassDisplay) );
				CheckMenuItem ( gMultiSpecMenus[kPaletteM], kInfoGroups, 
													(classGroupCode == kGroupDisplay) );
				
				if (grafPortType == kCGrafType || classGroupCode == kClassDisplay)
					EnableMenuItem (gMultiSpecMenus[kPaletteM], kSpectralClasses);
					
				else		// graf... != kCGrafType && class... != kClassDisplay 
					DisableMenuItem (gMultiSpecMenus[kPaletteM], kSpectralClasses);
				
				if (((FileInfoPtr)*gActiveImageFileInfoH)->numberGroups > 0 && 
					(grafPortType == kCGrafType || classGroupCode == kGroupDisplay) )
					EnableMenuItem (gMultiSpecMenus[kPaletteM], kInfoGroups);
					
				else		// fileInfoPtr->numberGroups <= 0 || ...
					DisableMenuItem (gMultiSpecMenus[kPaletteM], kInfoGroups);
				
				CheckMenuItem ( gMultiSpecMenus[kPaletteM], kInvertPalette, FALSE);
				DisableMenuItem (gMultiSpecMenus[kPaletteM], kInvertPalette);
				
				CheckMenuItem ( gMultiSpecMenus[kPaletteM], kBlink1MenuItem, 
																		!(usage & 0x0004) );
				EnableMenuItem (gMultiSpecMenus[kPaletteM], kBlink1MenuItem);
				
				CheckMenuItem ( gMultiSpecMenus[kPaletteM], kBlink2MenuItem, 
																		(usage & 0x0004) );
								
				if (gQD32IsImplemented && !gOSXFlag)
					EnableMenuItem (gMultiSpecMenus[kPaletteM], kBlink2MenuItem);
					
				else		// !gQD32IsImplemented || gOSXFlag
					DisableMenuItem (gMultiSpecMenus[kPaletteM], kBlink2MenuItem);
				
				CheckMenuItem ( gMultiSpecMenus[kPaletteM], k8BitPaletteMenuItem, FALSE );
				DisableMenuItem (gMultiSpecMenus[kPaletteM], k8BitPaletteMenuItem);
	 	   	
	 	   	}		// end "if (windowType == kThematicWindowType)" 
	 	   	
	 	   else		// windowType != kThematicWindowType 
	 	   	{
				CheckMenuItem ( gMultiSpecMenus[kPaletteM], kSpectralClasses, FALSE );
				DisableMenuItem (gMultiSpecMenus[kPaletteM], kSpectralClasses);
				CheckMenuItem ( gMultiSpecMenus[kPaletteM], kInfoGroups, FALSE );
				DisableMenuItem (gMultiSpecMenus[kPaletteM], kInfoGroups);
				
				CheckMenuItem ( gMultiSpecMenus[kPaletteM], kInvertPalette,
		 	   											(displaySpecsPtr->invertValues) );
				EnableMenuItem (gMultiSpecMenus[kPaletteM], kInvertPalette);
				if (displaySpecsPtr->pixelSize > 8)
					DisableMenuItem (gMultiSpecMenus[kPaletteM], kInvertPalette);
				
				CheckMenuItem ( gMultiSpecMenus[kPaletteM], kBlink1MenuItem, FALSE );
				DisableMenuItem (gMultiSpecMenus[kPaletteM], kBlink1MenuItem);
				
				CheckMenuItem ( gMultiSpecMenus[kPaletteM], kBlink2MenuItem, FALSE );
				DisableMenuItem (gMultiSpecMenus[kPaletteM], kBlink2MenuItem);
				
				CheckMenuItem ( gMultiSpecMenus[kPaletteM], 
								k8BitPaletteMenuItem, 
								displaySpecsPtr->better8BitPaletteFlag );
					
				if (gQD32IsImplemented && 
						displaySpecsPtr->pixelSize > 8 &&
						!displaySpecsPtr->better8BitPaletteFlag &&
						!gOSXCoreGraphicsFlag)
					EnableMenuItem (gMultiSpecMenus[kPaletteM], k8BitPaletteMenuItem);
				
				else		// !gQD32IsImplemented || ...
					DisableMenuItem (gMultiSpecMenus[kPaletteM], k8BitPaletteMenuItem);
	 	   	
	 	   	}		// end "else windowType != kThematicWindowType" 
		
					// Unlock handle to display specifications if needed.				
					
			CheckAndUnlockHandle (displaySpecsH);
				
			}		// end "if (displaySpecsH != NULL)" 
 																
		EnableMenuItem (gMultiSpecMenus[kPaletteM], kShowClosePaletteW);
		
		}		// end "else imageBaseAddressH != NULL || ..." 
	 	   	
			// Update "ShowClosePaletteW".													
  	
  	if (gPaletteWindow != NULL)
 		SetMenuItemText (gMultiSpecMenus[kPaletteM], kShowClosePaletteW,
 																"\pClose Palette Window");
						
	else		// gPaletteWindow == NULL 
 		SetMenuItemText (gMultiSpecMenus[kPaletteM], kShowClosePaletteW,
 																"\pShow Palette Window");
 																
 	gUpdatePaletteMenuItemsFlag = FALSE;
					
}		// end "UpdatePaletteMenuItems" 
*/

/* Disabled feature on 3/28/2014
//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdatePaletteWindow
//
//	Software purpose:	The purpose of this routine is to update the palette
//							window if need. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/31/1991
//	Revised By:			Larry L. Biehl			Date: 01/31/1991	

void UpdatePaletteWindow (
				Boolean								paletteWindowChanged, 
				Boolean								forceDrawPaletteWindow)
				
{

			// Check if palette window needs to be created.								
		
	if (gPaletteWindow != NULL || forceDrawPaletteWindow)
		{
			// Redraw the palette window if the palette menu changed and			
			// the palette window exists or force drawing of the window if			
			// requested.																			
	
		if ( (paletteWindowChanged || forceDrawPaletteWindow) )
			{
 	      DrawPalette ();
 	      
 	      		// Update the image window associated with the palette 			
 	      		// window.																		
 	      		
 	     	gPaletteImageWindow = gActiveImageWindow;
 	      	
 	      }		// end "if (paletteWindowChanged && gPaletteWindow != ..." 
	 	      
		}		// end "if (gPaletteWindow != NULL || forceDrawPaletteWindow)" 
	 	      
}		// end "UpdatePaletteWindow" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateProcessorMenuItems
//
//	Software purpose:	The purpose of this routine is to update the items
//							in the processor menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			Menus						 	in menus.c
//
//	Coded By:			Larry L. Biehl			Date: 02/23/1989
//	Revised By:			Larry L. Biehl			Date: 06/14/2016	

void UpdateProcessorMenuItems (
				WindowInfoPtr						activeImageWindowInfoPtr)

{
	WindowInfoPtr						projectWindowInfoPtr;
	
	SInt16								windowType;
	
	Boolean								projectWindowFlag;
									
	
	if( !gUpdateProcessorMenuItemsFlag )
																									return;
	
	if (gActiveImageWindow == NULL && gProjectInfoPtr == NULL)
		{
//		DisableMenuItem (gMultiSpecMenus[kProcessorM], 0);
		DisableMenuItem (gMultiSpecMenus[kProcessorM], kDisplaysMItem);
		DisableMenuItem (gMultiSpecMenus[kProcessorM], kHistogramMItem);
		DisableMenuItem (gMultiSpecMenus[kProcessorM], kListDataMenuItem);
		
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatChangeHeaderMenuItem);
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatChangeImageMenuItem);
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertMultispectralMenuItem);
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertProjectMenuItem);
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertShapeMenuItem);
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatModifyChannelMenuItem);
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatMosaicImagesMenuItem);
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatRecodeThematicMenuItem);
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatRectifyImageMenuItem);
		
		DisableMenuItem (gMultiSpecMenus[kProcessorM], kClusterMItem);
		DisableMenuItem (gMultiSpecMenus[kProcessorM], kStatisticsMItem);
//		DisableMenuItem (gMultiSpecMenus[kProcessorM], kUtilitiesMItem);
		DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kPrincipalComponentsMenuItem);
		DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kStatisticsImageMenuItem);
		DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kBiPlotDataMenuItem);
 		DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kListDescriptionMenuItem);
 		DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kCovarianceCheckMenuItem);
 		DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kCheckTransformMenuItem);
		gRedrawMenuBar = TRUE;
		
		}		// end "if (gActiveImageWindow == NULL && ..." 
		
	else		// gActiveImageWindow != NULL || gProjectInfoPtr != NULL 
 		{
		EnableMenuItem (gMultiSpecMenus[kProcessorM], kListDataMenuItem);
		EnableMenuItem (gMultiSpecMenus[kProcessorM], kUtilitiesMItem);
		EnableMenuItem (gMultiSpecMenus[kUtilitiesM], kPrincipalComponentsMenuItem);
 		EnableMenuItem (gMultiSpecMenus[kUtilitiesM], kListDescriptionMenuItem);
			
	 	if (gActiveImageWindow != NULL)
	 		{
		 	windowType = activeImageWindowInfoPtr->windowType;
		 	projectWindowFlag = activeImageWindowInfoPtr->projectWindowFlag;
		 	
 			
	 		if (gNumberOfIWindows == 1)
	 			{
 				EnableMenuItem (gMultiSpecMenus[kProcessorM], 0);
 				gRedrawMenuBar = TRUE;
 				
 				}		// end "if (gNumberOfIWindows == 1)"
	 	
	 		EnableMenuItem (gMultiSpecMenus[kProcessorM], kDisplaysMItem);
	 		EnableMenuItem (gMultiSpecMenus[kProcessorM], kHistogramMItem);
	 		EnableMenuItem (gMultiSpecMenus[kProcessorM], kReformatMItem);
	 		
	 		if (gFullVersionFlag && windowType == kImageWindowType)
 				EnableMenuItem (gMultiSpecMenus[kUtilitiesM], kStatisticsImageMenuItem);
				
			else		// windowType != kImageWindowType
				DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kStatisticsImageMenuItem);
	 		
	 		if (windowType == kImageWindowType && 
	 									activeImageWindowInfoPtr->totalNumberChannels >= 2)
				EnableMenuItem (gMultiSpecMenus[kUtilitiesM], kBiPlotDataMenuItem);
				
			else		// windowType != kImageWindowType || ... 
				DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kBiPlotDataMenuItem);
	 		
		   if (projectWindowFlag ||
		   			DetermineIfProjectPossible (activeImageWindowInfoPtr->windowInfoHandle))
				EnableMenuItem (gMultiSpecMenus[kProcessorM], kClusterMItem);
		   	
		   else		// ... windowType != kImageWindowType && !projectWindowFlag ... 
				DisableMenuItem (gMultiSpecMenus[kProcessorM], kClusterMItem);
	 		
		   if (gFullVersionFlag && (projectWindowFlag ||
		   			DetermineIfProjectPossible (activeImageWindowInfoPtr->windowInfoHandle)))
				EnableMenuItem (gMultiSpecMenus[kProcessorM], kStatisticsMItem);
		   	
		   else		// ... windowType != kImageWindowType && !projectWindowFlag ... 
				DisableMenuItem (gMultiSpecMenus[kProcessorM], kStatisticsMItem);
		   			   	
		   if (gFullVersionFlag && windowType == kImageWindowType)
		 		EnableMenuItem (gMultiSpecMenus[kUtilitiesM], kPrincipalComponentsMenuItem);
		 		
		 	else		// windowType != kImageWindowType 
		 		DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kPrincipalComponentsMenuItem);
		 		
		   if (windowType == kThematicWindowType)
		 		EnableMenuItem (gMultiSpecMenus[kProcessorM], kListResultsMItem);
		 		
		   else		// windowType != kThematicWindowType 
		 		DisableMenuItem (gMultiSpecMenus[kProcessorM], kListResultsMItem);
	 		
	 		}		// end "if (gActiveImageWindow != NULL)" 
	 		
		if (gProjectInfoPtr == NULL)
			{
			DisableMenuItem (gMultiSpecMenus[kProcessorM], kStatEnhanceMItem);
 			DisableMenuItem (gMultiSpecMenus[kProcessorM], kFeatureExtractionMItem);
			DisableMenuItem (gMultiSpecMenus[kProcessorM], kClassifyMItem);
	 		DisableMenuItem (gMultiSpecMenus[kProcessorM], kSeparabilityMItem);
		 	
		 	DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kCovarianceCheckMenuItem);
			
			}		// end "if (gProjectInfoPtr == NULL)" 
			
		if (gProjectInfoPtr != NULL)
			{
			if (gActiveImageWindow == NULL)
				{
				EnableMenuItem (gMultiSpecMenus[kProcessorM], 0);
	 	
	 			DisableMenuItem (gMultiSpecMenus[kProcessorM], kDisplaysMItem);
	 			DisableMenuItem (gMultiSpecMenus[kProcessorM], kHistogramMItem);
	 		
			 	projectWindowInfoPtr = (WindowInfoPtr)*gProjectInfoPtr->windowInfoHandle;
		 		if (projectWindowInfoPtr->totalNumberChannels >= 2)
					EnableMenuItem (gMultiSpecMenus[kUtilitiesM], kBiPlotDataMenuItem);
					
				else		// projectWindowInfoPtr->totalNumberChannels < 2 
					DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kBiPlotDataMenuItem);
				
				gRedrawMenuBar = TRUE;
				
				}		// end "if (gActiveImageWindow == NULL)" 
				
					// Enable processor that do not require any classes.				
					
			EnableMenuItem (gMultiSpecMenus[kProcessorM], kClusterMItem);
			EnableMenuItem (gMultiSpecMenus[kProcessorM], kStatisticsMItem);
 			EnableMenuItem (gMultiSpecMenus[kUtilitiesM], kStatisticsImageMenuItem);
			
					// Enable processors that require at least one training or		
					// test field.																	
					
			if (gProjectInfoPtr->numberTotalPoints > 0)
	 			EnableMenuItem (gMultiSpecMenus[kProcessorM], kReformatMItem);
				
					// Disable processors that need one or more classes having at  
					// least one training field.												
			
			if (gProjectInfoPtr->numberStatTrainClasses <= 0)
				{
				DisableMenuItem (gMultiSpecMenus[kProcessorM], kStatEnhanceMItem);
				DisableMenuItem (gMultiSpecMenus[kProcessorM], kClassifyMItem);
		 		DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kCovarianceCheckMenuItem);
				
				}		// end "if (gProjectInfoPtr->numberStatTrainClasses <= 0)" 
			
					// Disable processors that need two or more classes having at  
					// least one training field.												
			
			if (gProjectInfoPtr->numberStatTrainClasses <= 1)
				{
 				DisableMenuItem (gMultiSpecMenus[kProcessorM], kFeatureExtractionMItem);
		 		DisableMenuItem (gMultiSpecMenus[kProcessorM], kSeparabilityMItem);
				
				}		// end "if (gProjectInfoPtr->numberStatTrainClasses <= 1)" 
				
					// Enable processors that will work with one or more classes 	
					// having at least one training field.									
			
			if (gProjectInfoPtr->numberStatTrainClasses >= 1)
				{
				EnableMenuItem (gMultiSpecMenus[kProcessorM], kStatEnhanceMItem);
				EnableMenuItem (gMultiSpecMenus[kProcessorM], kClassifyMItem);
		 		EnableMenuItem (gMultiSpecMenus[kProcessorM], kUtilitiesMItem);
		 		EnableMenuItem (gMultiSpecMenus[kUtilitiesM], kCovarianceCheckMenuItem);
				
				}		// end "gProjectInfoPtr->numberStatTrainClasses >= 1)" 
			
					// Enable processors that will work with two or more classes 	
					// having at least one training field.									
			
			if (gProjectInfoPtr->numberStatTrainClasses >= 2)
				{
 				EnableMenuItem (gMultiSpecMenus[kProcessorM], kFeatureExtractionMItem);
		 		EnableMenuItem (gMultiSpecMenus[kProcessorM], kSeparabilityMItem);
				
				}		// end "if (gProjectInfoPtr->numberStatTrainClasses >= 2)" 
				
					// Disable processors that require the second order statistics	
					// I.E. the covariance matrices.											
					
			if (gProjectInfoPtr->statisticsCode != kMeanCovariance)
				{
		 		DisableMenuItem (gMultiSpecMenus[kProcessorM], kStatEnhanceMItem);
		 		DisableMenuItem (gMultiSpecMenus[kProcessorM], kFeatureExtractionMItem);
		 		DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kCovarianceCheckMenuItem);
				
				}		// end "if (...->statisticsCode != kMeanCovariance)" 
				
			}		// end "else gProjectInfoPtr != NULL" 
			
		UpdateReformatMenuItems (activeImageWindowInfoPtr);
			
		}		// end "else gActiveImageWindow != NULL || ... " 	
	
			// Convert ENVI ASCII ROI to Thematic Image	

	if (gFullVersionFlag)
		EnableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertENVIASCIIMenuItem);
		
	else		// !gFullVersionFlag
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertENVIASCIIMenuItem);
						
	if (gTransformationMatrix.numberChannels <= 0)
 		DisableMenuItem (gMultiSpecMenus[kUtilitiesM], kCheckTransformMenuItem);
 		
 	else		// gTransformationMatrix.numberChannels > 0 
		{
		EnableMenuItem (gMultiSpecMenus[kProcessorM], kUtilitiesMItem);
 		EnableMenuItem (gMultiSpecMenus[kUtilitiesM], kCheckTransformMenuItem);
		
		}		// end "else gTransformationMatrix.numberChannels > 0"
		
	EnableMenuItem (gMultiSpecMenus[kProcessorM], kReformatMItem);
 		
	gUpdateProcessorMenuItemsFlag = FALSE;
		
}		// end "UpdateProcessorMenuItems" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateProjectMenuItems
//
//	Software purpose:	The purpose of this routine is to update the items
//							in the project menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/29/1990
//	Revised By:			Larry L. Biehl			Date: 02/05/2001

void UpdateProjectMenuItems (
				WindowInfoPtr						activeImageWindowInfoPtr)

{
	if( !gUpdateProjectMenuItemsFlag )
																									return;
		
			// Update project menu items.														
	
	if (gProjectInfoPtr == NULL)
		{
/*		
		if (gActiveImageWindowInfoH != NULL && 
				((WindowInfoPtr)*gActiveImageWindowInfoH)->windowType == kImageWindowType &&
				((WindowInfoPtr)*gActiveImageWindowInfoH)->numberImageFiles == 1 )
			EnableMenuItem (gMultiSpecMenus[kProjectM], kNewProject);
			
		else		// gActiveImageWindowInfoH != NULL && ...
			DisableMenuItem (gMultiSpecMenus[kProjectM], kNewProject);
		
		EnableMenuItem (gMultiSpecMenus[kProjectM], kOpenProject);
		DisableMenuItem (gMultiSpecMenus[kProjectM], kCloseProject);
		DisableMenuItem (gMultiSpecMenus[kProjectM], kSaveProject);
		DisableMenuItem (gMultiSpecMenus[kProjectM], kSaveProjectAs);
*/		
		CheckMenuItem ( gMultiSpecMenus[kProjectM], kUseBaseStatistics, FALSE );
		DisableMenuItem (gMultiSpecMenus[kProjectM], kUseBaseStatistics);
		
		CheckMenuItem ( gMultiSpecMenus[kProjectM], kUseLOOStatistics, FALSE );
		DisableMenuItem (gMultiSpecMenus[kProjectM], kUseLOOStatistics);
		
		CheckMenuItem ( gMultiSpecMenus[kProjectM], kUseEnhancedStatistics, FALSE );
		DisableMenuItem (gMultiSpecMenus[kProjectM], kUseEnhancedStatistics);
		
		CheckMenuItem ( gMultiSpecMenus[kProjectM], kMixedStatisticsUsed, FALSE );
		DisableMenuItem (gMultiSpecMenus[kProjectM], kMixedStatisticsUsed);
		
		DisableMenuItem (gMultiSpecMenus[kProjectM], kClearStatistics);
		DisableMenuItem (gMultiSpecMenus[kProjectM], kChangeBaseImageFile);
		
		SetMenuItemText (gMultiSpecMenus[kProjectM], kAssociatedImage, 
		 														"\pAdd As Associated Image");
		DisableMenuItem (gMultiSpecMenus[kProjectM], kAssociatedImage);
		
		}		// end "if (gProjectInfoPtr == NULL)" 
		
	else		// gProjectInfoPtr != NULL 
		{
/*
		DisableMenuItem (gMultiSpecMenus[kProjectM], kNewProject);
		DisableMenuItem (gMultiSpecMenus[kProjectM], kOpenProject);
		EnableMenuItem (gMultiSpecMenus[kProjectM], kCloseProject);
		
		if (gProjectInfoPtr->changedFlag)
			EnableMenuItem (gMultiSpecMenus[kProjectM], kSaveProject);
			
		else		// !gProjectInfoPtr->changedFlag 
			DisableMenuItem (gMultiSpecMenus[kProjectM], kSaveProject);
			
		EnableMenuItem (gMultiSpecMenus[kProjectM], kSaveProjectAs);
*/						
		EnableMenuItem (gMultiSpecMenus[kProjectM], kUseBaseStatistics);
		CheckMenuItem ( gMultiSpecMenus[kProjectM], 
						kUseBaseStatistics, 
						gProjectInfoPtr->covarianceStatsToUse == kOriginalStats );
						
		EnableMenuItem (gMultiSpecMenus[kProjectM], kUseLOOStatistics);
		CheckMenuItem ( gMultiSpecMenus[kProjectM], 
						kUseLOOStatistics, 
						gProjectInfoPtr->covarianceStatsToUse == kLeaveOneOutStats );
						
		if (gProjectInfoPtr->enhancedStatsExistFlag)
			EnableMenuItem (gMultiSpecMenus[kProjectM], kUseEnhancedStatistics);
			
		else		// !gProjectInfoPtr->enhancedStatsExistFlag 
			DisableMenuItem (gMultiSpecMenus[kProjectM], kUseEnhancedStatistics);
		
		CheckMenuItem ( gMultiSpecMenus[kProjectM], 
						kUseEnhancedStatistics, 
						gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats );
		
		CheckMenuItem ( gMultiSpecMenus[kProjectM], 
						kMixedStatisticsUsed, 
						gProjectInfoPtr->covarianceStatsToUse == kMixedStats );
	
		if (gProjectInfoPtr->fieldChanStatsHandle != NULL)
			EnableMenuItem (gMultiSpecMenus[kProjectM], kClearStatistics);
		
		else		// gProjectInfoPtr->fieldChanStatsHandle == NULL 
			DisableMenuItem (gMultiSpecMenus[kProjectM], kClearStatistics);
			
		EnableMenuItem (gMultiSpecMenus[kProjectM], kChangeBaseImageFile);
		if (gActiveImageWindow != NULL)
			{
			if (activeImageWindowInfoPtr->projectBaseImageFlag)
				{
				SetMenuItemText (gMultiSpecMenus[kProjectM], kAssociatedImage, 
															"\pBase Image");
				DisableMenuItem (gMultiSpecMenus[kProjectM], kAssociatedImage);
				
				}		// end "if ( windowInfoPtr->projectBaseImageFlag )" 
				
			else		// !...->projectBaseImageFlag 
				{
				if (activeImageWindowInfoPtr->projectWindowFlag)
	 				SetMenuItemText (gMultiSpecMenus[kProjectM], kAssociatedImage, 
	 														"\pRemove As Associated Image");
		 															
				else		// !activeImageWindowInfoPtr->projectWindowFlag 
		 			SetMenuItemText (gMultiSpecMenus[kProjectM], kAssociatedImage, 
		 													"\pAdd As Associated Image");
				
				EnableMenuItem (gMultiSpecMenus[kProjectM], kAssociatedImage);
				
				}		// end "else !windowInfoPtr->projectBaseImageFlag" 
	 			
	 		}		// end "if (gActiveImageWindow != NULL)" 
	 		
	 	}		// end "else gProjectInfoPtr != NULL"
	 	
	gUpdateProjectMenuItemsFlag = FALSE; 
		
}		// end "UpdateProjectMenuItems" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateReformatMenuItems
//
//	Software purpose:	The purpose of this routine is to update the items
//							in the processor menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			Menus						 
//
//	Coded By:			Larry L. Biehl			Date: 02/03/2001
//	Revised By:			Larry L. Biehl			Date: 06/14/2016	

void UpdateReformatMenuItems (
				WindowInfoPtr						activeImageWindowInfoPtr)

{
	FileInfoPtr							activeImageFileInfoPtr;
	
	Handle								activeImageFileInfoHandle;
	
	SignedByte							fileHandleStatus;
	
		
	activeImageFileInfoHandle = GetFileInfoHandle (activeImageWindowInfoPtr);
	activeImageFileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
															activeImageFileInfoHandle, 
															&fileHandleStatus, 
															kNoMoveHi);
	 
	if (activeImageWindowInfoPtr == NULL && 
				activeImageFileInfoPtr == NULL && 
							gProjectInfoPtr == NULL)
																									return;
	
			// Change Header
			
	if (UpdateReformatChangeHeader (activeImageWindowInfoPtr, 
													activeImageFileInfoPtr, 
													NULL) )
		EnableMenuItem (gMultiSpecMenus[kReformatM], kReformatChangeHeaderMenuItem);
		
	else		// !UpdateReformatChangeHeader (...
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatChangeHeaderMenuItem);
		
	
			// Change Image File Format
			
	if (activeImageFileInfoPtr != NULL)
		EnableMenuItem (gMultiSpecMenus[kReformatM], kReformatChangeImageMenuItem);
		
	else		// activeImageFileInfoPtr == NULL
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatChangeImageMenuItem);
		
	
			// Convert Multispectral Image to Thematic Image							
			// Allow this option for active image windows that are of				
			// multispectral type and also only if system 7 is being used.	
			// This capability is not available under OSX
			
	if (activeImageFileInfoPtr != NULL &&
			!activeImageFileInfoPtr->thematicType && 
						gQuickDrawVersion >= 0x00000230 &&
								!gOSXCoreGraphicsFlag)
		EnableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertMultispectralMenuItem);
		
	else		// activeImageFileInfoPtr == NULL || ...
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertMultispectralMenuItem);
	
	
			// Convert project train/test fields to thematic image file. Check
			// if any training and/or test fields exist that are not cluster fields.

	if (gProjectInfoPtr != NULL && gProjectInfoPtr->numberStatisticsClasses > 0)
		EnableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertProjectMenuItem);
		
	else		// gProjectInfoPtr == NULL || ...
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertProjectMenuItem);
	
	
			// Convert shape file to thematic image file. Check if shape file exists
			// for active image window and if it is of a point type.

	if (UpdateReformatConvertShape (activeImageWindowInfoPtr, 
														activeImageFileInfoPtr, 
														NULL) )
		EnableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertShapeMenuItem);
		
	else		// gProjectInfoPtr == NULL || ...
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatConvertShapeMenuItem);
	
	
			// Modify channel wavelength descriptions.									
			// Do not allow this option for thematic images.							
			// Also wavelength descriptions will not be allowed for a window image	
			// with more than one image disk file.	  

	if (UpdateReformatModifyChannel (activeImageWindowInfoPtr, 
														activeImageFileInfoPtr, 
														NULL) )
		EnableMenuItem (gMultiSpecMenus[kReformatM], kReformatModifyChannelMenuItem);
		
	else		// !UpdateReformatModifyChannel (
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatModifyChannelMenuItem);
	
	
			// Mosaic two images side by side.	

	if ( UpdateReformatMosaicImages (NULL) )
		EnableMenuItem (gMultiSpecMenus[kReformatM], kReformatMosaicImagesMenuItem);
		
	else		// !UpdateReformatMosaicImages (
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatMosaicImagesMenuItem);
	
	
			// Recode Thematic Image
			
	if (UpdateReformatRecodeThematicImage (activeImageFileInfoPtr))
/*	headerFromGDALOKFlag = FALSE;
	if (activeImageFileInfoPtr->gdalDataSetH == NULL ||
			((activeImageFileInfoPtr->format == kTIFFType ||
							activeImageFileInfoPtr->format == kGeoTIFFType) &&
									activeImageFileInfoPtr->numberHeaderBytes > 0))
		headerFromGDALOKFlag = TRUE;

	if (activeImageFileInfoPtr != NULL && 
			activeImageFileInfoPtr->thematicType &&
				activeImageFileInfoPtr->dataCompressionCode == kNoCompression && 
					activeImageFileInfoPtr->bandInterleave <= kBNonSQMenuItem &&
						activeImageFileInfoPtr->numberBytes >= 1 &&
							activeImageFileInfoPtr->numberBytes <= 2 &&
								headerFromGDALOKFlag) */
		EnableMenuItem (gMultiSpecMenus[kReformatM], kReformatRecodeThematicMenuItem);
		
	else		// activeImageFileInfoPtr == NULL || ...
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatRecodeThematicMenuItem);
	
			// Rectify Image	

	if (activeImageFileInfoPtr != NULL)
		EnableMenuItem (gMultiSpecMenus[kReformatM], kReformatRectifyImageMenuItem);
		
	else		// activeImageFileInfoPtr == NULL || ...
		DisableMenuItem (gMultiSpecMenus[kReformatM], kReformatRectifyImageMenuItem);
		
			// Set the fileInfoHandle state.		
	
	MHSetState (activeImageFileInfoHandle, fileHandleStatus);
		
}		// end "UpdateReformatMenuItems" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateStatisticsMenuBeforeSelection
//
//	Software purpose:	The purpose of this routine is to update the menu items
//							for the case when there is no active window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			UpdateMenuBeforeSelection in menus.c
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1995
//	Revised By:			Larry L. Biehl			Date: 11/10/2000	

void UpdateStatisticsMenuBeforeSelection (
				WindowInfoPtr						windowInfoPtr,
				WindowInfoPtr						activeImageWindowInfoPtr)

{
			// File Menu
			
	if (gUpdateFileMenuItemsFlag)
		{	
		UpdateFileProjectClose (NULL);
		
		UpdateFileMenuItems ();
		
		EnableMenuItem (gMultiSpecMenus[kFileM], kCloseImage);
	 	DisableMenuItem (gMultiSpecMenus[kFileM], kSaveOutputAs);
	 	DisableMenuItem (gMultiSpecMenus[kFileM], kPrintOutput);
	 	
	 	UpdateFileDefaultSaveAs (NULL);
	 	UpdateFileDefaultPrint (NULL);
			
		gUpdateFileMenuItemsFlag = FALSE;
		
		}		// end "if (gUpdateFileMenuItemsFlag)"
		
			// Edit Menu
			
	if (gUpdateEditMenuItemsFlag)
		{
		UpdateEditMenuItems ();
		UpdateStatWEditMenu ();	
			
		Boolean			rectangularSelectionFlag = FALSE;
     	if ( gProjectInfoPtr != NULL &&
	 								gProjectInfoPtr->selectionType == kRectangleType )
	 		rectangularSelectionFlag = TRUE;
     			
		if (rectangularSelectionFlag)
	 		EnableMenuItem (gMultiSpecMenus[kEditM], kSelectAll);
     	
     	if (rectangularSelectionFlag)
			EnableMenuItem (gMultiSpecMenus[kEditM], kEditSelectionRect);
     	
     	SInt16 selectionTypeCode = GetSelectionTypeCode (gProjectSelectionWindow); 
     	UpdateEditClearSelection (NULL, selectionTypeCode);
		if ( selectionTypeCode != 0 )
			EnableMenuItem (gMultiSpecMenus[kEditM], kClearSelectionRect);
		
		gUpdateEditMenuItemsFlag = FALSE;
		
		}		// end "if (gUpdateEditMenuItemsFlag)"
	
	UpdateWindowMenuItems (windowInfoPtr);
	
	UpdateProjectMenuItems (activeImageWindowInfoPtr);
	
	UpdateProcessorMenuItems (activeImageWindowInfoPtr);
	
//	UpdatePaletteMenuItems (activeImageWindowInfoPtr);
	
	UpdateOptionsMenuItems ();
		
}		// end "UpdateStatisticsMenuBeforeSelection"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateWindowMenuItems
//
//	Software purpose:	The purpose of this routine is to update the items
//							in the project menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1995
//	Revised By:			Larry L. Biehl			Date: 12/15/2000	

void UpdateWindowMenuItems (
				WindowInfoPtr						windowInfoPtr)

{
	SInt16								windowMenuItem;
	
	
	if( !gUpdateWindowsMenuItemsFlag )
																						return;	
	
	if (gTheActiveWindow != NULL)     			
		EnableMenuItem (gMultiSpecMenus[kWindowM], kHideTheWindow);
	else		// gTheActiveWindow == NULL
		DisableMenuItem (gMultiSpecMenus[kWindowM], kHideTheWindow);
		
	if ( UpdateWindowCoordinateView (NULL) )
		EnableMenuItem (gMultiSpecMenus[kWindowM], kShowHideCoordinateView);
		
	else		// !UpdateWindowCoordinateView (NULL)
		DisableMenuItem (gMultiSpecMenus[kWindowM], kShowHideCoordinateView);
/*		
	if (windowInfoPtr->windowType == kImageWindowType || 
										windowInfoPtr->windowType == kThematicWindowType)
		{
		EnableMenuItem (gMultiSpecMenus[kWindowM], kShowHideCoordinateView);
		
		if (windowInfoPtr->coordinateHeight > 0)
			SetMenuItemText (gMultiSpecMenus[kWindowM], 
									kShowHideCoordinateView, 
									"\pHide Coordinate View");
									
		else		// windowInfoPtr->coordinateHeight == 0
			SetMenuItemText (gMultiSpecMenus[kWindowM], 
									kShowHideCoordinateView, 
									"\pShow Coordinate View");
		
		}		// end "if (windowInfoPtr->windowType == kImageWindowType || ..."
		
	else		// windowType != kImageWindowType && ...
		{
		DisableMenuItem (gMultiSpecMenus[kWindowM], kShowHideCoordinateView);
		
		SetMenuItemText (gMultiSpecMenus[kWindowM], 
								kShowHideCoordinateView, 
								"\pShow Coordinate View");
		
		}		// end "else windowType != kImageWindowType && ..."
*/	
	if ( UpdateWindowSelectionGraph() )
		EnableMenuItem (gMultiSpecMenus[kWindowM], kSelectionGraphMenuItem);
			
	else		// !UpdateWindowSelectionGraph()
		DisableMenuItem (gMultiSpecMenus[kWindowM], kSelectionGraphMenuItem);
/*			
	if (gActiveImageWindow != NULL)
		{	
		if (gNumberOfGWindows >= kMaxNumberGWindows)
			DisableMenuItem (gMultiSpecMenus[kWindowM], kSelectionGraphMenuItem);
			
		else		// gNumberOfGWindows < kMaxNumberGWindows
			EnableMenuItem (gMultiSpecMenus[kWindowM], kSelectionGraphMenuItem);
		
		}		// end "if (gActiveImageWindow != NULL)"
		
	else		// gActiveImageWindow == NULL																				
		{															
 		DisableMenuItem (gMultiSpecMenus[kWindowM], kSelectionGraphMenuItem);
 		
 		}		// end "else gActiveImageWindow == NULL"	
*/ 		
	if (gPaletteWindow != NULL)		
		EnableMenuItem (gMultiSpecMenus[kWindowM], kPaletteWindow);
	else		// gPaletteWindow == NULL
		DisableMenuItem (gMultiSpecMenus[kWindowM], kPaletteWindow);
	
	if (gProjectWindow != NULL)		
		EnableMenuItem (gMultiSpecMenus[kWindowM], kStatisticsWItem);
	else		// gProjectWindow == NULL
		DisableMenuItem (gMultiSpecMenus[kWindowM], kStatisticsWItem);
   
   windowMenuItem = 0;
   if (windowInfoPtr != NULL)
   	windowMenuItem = windowInfoPtr->windowMenuItem;
   		
   		// Make the checked window item up-to-date.
   	
   if (gCheckedWindowMenuItem != windowMenuItem)
   	{
   	if (gCheckedWindowMenuItem != 0)
	 		CheckMenuItem (gMultiSpecMenus[kWindowM], gCheckedWindowMenuItem, FALSE);
	 		
   	if (windowMenuItem != 0)
	 		CheckMenuItem (gMultiSpecMenus[kWindowM], windowMenuItem, TRUE);
	 	
	 	gCheckedWindowMenuItem = windowMenuItem;
	 	
	 	}		// end " if (gCheckedWindowMenuItem != windowMenuItem)"
	 	
	gUpdateWindowsMenuItemsFlag = FALSE; 
		
}		// end "UpdateWindowMenuItems" 

