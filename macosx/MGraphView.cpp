//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SGraphView.cpp : implementation file 
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			04/06/2018
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	This file contains functions that relate to the graph view 
//								class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"                

#include "MGraphView.h"


#define	kXAxis			1
#define	kYAxis			2
#define	kBothXYAxes		3

void 			GraphWControlEvent (
					ControlHandle						theControl, 
					WindowPtr							windowPtr, 
					Point									location, 
					SInt16								partCode);


// === Static Member Variable ===

//Handle			CMGraphView::s_selectionIOInfoHandle = NULL;
		
//UInt16			CMGraphView::s_graphicsWindowCount = 0;

//UInt16			CMGraphView::s_numberOfGWindows = 0;


CMGraphView::CMGraphView ()

{
	Boolean					continueFlag = TRUE;
	SelectionIOInfoPtr 	selectionIOPtr;
	
	
	m_graphRecordHandle = NULL;
	m_window = NULL;
	
   m_graphRecordHandle = (Handle)MNewHandle (sizeof (GraphRecord));
   continueFlag = (m_graphRecordHandle != NULL); 
		
	IntializeGraphRecord ();
   
	if (continueFlag &&
			s_selectionIOInfoHandle == NULL &&
			gProcessorCode == kSelectionGraphProcessor)
		{                               		
		s_selectionIOInfoHandle = MNewHandle (sizeof (SelectionIOInfo));
					
		if (s_selectionIOInfoHandle != NULL)
			{ 																			
			selectionIOPtr = (SelectionIOInfoPtr)GetHandlePointer (s_selectionIOInfoHandle);
								                                                          
			selectionIOPtr->channelStatsHandle = NULL;
			//selectionIOPtr->graphRecordHandle = NULL;
			selectionIOPtr->sumSquaresHandle = NULL;
			selectionIOPtr->ioBufferHandle = NULL;
			selectionIOPtr->bufferSize = 0;
			selectionIOPtr->outputBufferOffset = 0;
			selectionIOPtr->checkIOMemoryFlag = TRUE;
			selectionIOPtr->memoryWarningFlag = FALSE;
							
			}	// end "if (s_selectionIOInfoHandle != NULL)" 
			
		}	// end "if (continueFlag && ..."
	
	if (continueFlag)
		{                      
		s_graphicsWindowCount++;	
		m_graphicsWindowNumber = s_graphicsWindowCount;
		
		s_numberOfGWindows++;
		
		}	// end "if (continueFlag)"
	
}	// end "CMGraphView"



CMGraphView::~CMGraphView ()
{           
	if (m_graphRecordHandle != NULL)
		{                    							                               
		GraphPtr graphRecordPtr = (GraphPtr)GetHandlePointer (m_graphRecordHandle, kLock);
		
		DisposeOfGraphRecordMemory (graphRecordPtr); 
		
		m_graphRecordHandle = UnlockAndDispose (m_graphRecordHandle);
			
		}	// end "m_graphRecordHandle != NULL"
			
	if (gSelectionGraphViewCPtr == this && s_selectionIOInfoHandle != NULL)
		{                    							
		SelectionIOInfoPtr selectionIOPtr = (SelectionIOInfoPtr)GetHandlePointer (
																			s_selectionIOInfoHandle, kLock);
							                                                          
		UnlockAndDispose (selectionIOPtr->channelStatsHandle); 
		UnlockAndDispose (selectionIOPtr->sumSquaresHandle);
		UnlockAndDispose (selectionIOPtr->ioBufferHandle); 
		
		s_selectionIOInfoHandle = UnlockAndDispose (s_selectionIOInfoHandle);
		
		}	// end "if (gSelectionGraphViewCPtr == this && ...)"
		
	if (gSelectionGraphViewCPtr == this)
		gSelectionGraphViewCPtr = NULL;
	
	if (s_numberOfGWindows > 0)	
		s_numberOfGWindows--; 
	
}	// end "~CMGraphView"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoGraphWActivateEvent
//
//	Software purpose:	This routine handles the activate events for graph windows.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/23/1993
//	Revised By:			Larry L. Biehl			Date: 03/16/2018

void DoGraphWActivateEvent (
				WindowPtr							windowPtr, 
				Handle								windowInfoHandle,
				Boolean								becomingActiveFlag)
		
{
			//	Local Declarations 	
			
	GraphPtr								graphRecordPtr;
	
	ControlHandle						controlHandle,
											rootControlHandle;
											
	Handle								graphRecordHandle;
	
	UInt16								index,
											numberControls;
	
	SignedByte							handleStatus;
	
	
	if (becomingActiveFlag)
		{	
				// Hilite the controls as needed.
	
		graphRecordHandle = GetGraphRecordHandle (windowPtr);
		graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (graphRecordHandle,
																					&handleStatus);
				
		UpdateGraphControls (windowPtr);
              	
		MHSetState (graphRecordHandle, handleStatus);
	     	
		}	// end "if (becomingActiveFlag)"
		
	else	// !becomingActiveFlag
		{
				// This is a deactivate event. Unhilite the graph controls			
							
		GetRootControl (windowPtr, &rootControlHandle);
		
		if (rootControlHandle != NULL)
			{
			CountSubControls (rootControlHandle, &numberControls);
			for (index=numberControls; index>=1; index--)
				{
				GetIndexedSubControl (rootControlHandle, index, &controlHandle);
				HiliteControl (controlHandle, 255);
					
				}	// end "for (index=numberControls; index>=1; index--)"
				
			}	// end "if (rootControlHandle != NULL)"
		
		}	// end "else !becomingActiveFlag"

}	// end "DoGraphWActivateEvent"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoGraphicsWindowEdit
//
//	Software purpose:	This routine handle the editing function for a
//							Graph window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/09/1993
//	Revised By:			Larry L. Biehl			Date: 06/04/1996			

Boolean DoGraphWindowEdit (
				SInt16								menuItem)
													
{
	GraphPtr								graphRecordPtr;
	
	PicHandle							thePic;
	
	SInt32								errCode,
											handleSize;
											
	Boolean								continueFlag;
	SignedByte							handleStatus;
		
	
	continueFlag = TRUE;
	
	switch (menuItem)
		{
		case kUndo:
					//	Cannot Undo 
			break;
	
		case kCut:
					//	Cannot Cut 
			break;
			
		case kCopy:		
			SetPortWindowPort (gTheActiveWindow);
			thePic = (PicHandle)OpenPicture (
									GetWindowPortBounds (gTheActiveWindow, &gTempRect));
				
			if (thePic != NULL)
				{
				WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																				gActiveWindowInfoH);
				CMGraphView* graphViewCPtr = windowInfoPtr->graphViewCPtr;
				
				graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (
												graphViewCPtr->GetGraphRecordHandle (),
												&handleStatus);
			
				continueFlag = DrawGraph (graphRecordPtr, TRUE);
			
				ClosePicture ();

				ScrapRef 	scrapRef;
				ClearCurrentScrap ();
				errCode = GetCurrentScrap (&scrapRef);
				
            handleSize = GetHandleSize ((Handle)thePic);
            MHLock ((Handle)thePic);

				errCode = PutScrapFlavor (scrapRef,
													'PICT', 
													kScrapFlavorMaskNone, 
													handleSize,
													(Ptr)*thePic);
            if (errCode != noErr)
					errCode = DisplayAlert (1153, kStopAlert, 0, 0, 0, NULL);
					
				else	// errCode == noErr 
					{
							// Make certain that any text edit scrap does not		
							// write over the desk scrap.		
														
					TESetScrapLength (0);
					gLastScrapCount = 0;
								
					}	// end "else errCode == noErr" 
					
            CheckAndUnlockHandle ((Handle)thePic);
            KillPicture (thePic);
              	
				MHSetState (graphViewCPtr->GetGraphRecordHandle (), handleStatus);
              	
				}	// end "if (graphPic !=NULL)" 
				
		case kPaste:
			//	Cannot Paste 
			break;
		
		case kClear:
			//	Cannot Clear 
			break;
			
		case kSelectAll:
			//	Cannot Select All 
			break;
			
		}	// end "switch (menuItem)" 
		
	return (continueFlag);

}	// end "DoGraphWindowEdit" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoGraphWindowGrow
//
//	Software purpose:	This routine handles mouse down events in the
//							grow box for graph windows.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 10/04/1991
//	Revised By:			Larry L. Biehl			Date: 04/30/2003		

void DoGraphWindowGrow (
				WindowPtr							window,
				Handle								windowInfoHandle)

{
	Rect									growArea;
	//GraphPtr								graphRecordPtr;
	Handle								graphRecordHandle;
	SInt32								newSize;
										
	
			// Get the current maximum rectangular display area.
													
	GetDisplayRegionRectangle (&growArea);	
			
	growArea.right -= growArea.left;
	growArea.bottom -= growArea.top;
	growArea.left = growArea.top = gGrowAreaMinimum;

	newSize = GrowWindow (window, gEventRecord.where, &growArea);
	SizeWindow (window, LoWord (newSize), HiWord (newSize), TRUE);
	InvalWindowRect (window, GetWindowPortBounds (window, &gTempRect));
	graphRecordHandle = GetGraphRecordHandle (windowInfoHandle);
	
	UpdateGraphScales (graphRecordHandle);
	/*
	graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle, kLock);
	SetXGraphScale (window, graphRecordPtr);
	
	if (graphRecordPtr->graphCodesAvailable & 0x0001 &&
										graphRecordPtr->flag & NU_HISTOGRAM_PLOT)
		SetGraphMinMax (graphRecordPtr, kYAxis);
		
	else	// !...->graphCodesAvailable & 0x0001 && ...
		SetYGraphScale (window, graphRecordPtr);
		
	CheckAndUnlockHandle (graphRecordHandle);
	*/	
	//EraseRect (&gTempRect);
	DrawGraphControls (window);
	
}	// end "DoGraphWindowGrow" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		DoGraphWMouseDnContent
//
//	Software purpose:	This routine handle mouse down events in the
//							content part of a window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 01/23/1999
//	Revised By:			Larry L. Biehl			Date: 01/23/1999			

void DoGraphWMouseDnContent (
				WindowPtr							theWindow,
				Point									mousePt)

{
			//	Local Declarations 																
	
	ControlHandle						whichControl = NULL;
	
	SInt16								partCode;

		
	partCode = FindControl (mousePt, theWindow, &whichControl);
	if (partCode > 0)
     	{
     			// Set clip region to include the scroll bars.							
     			
		ClipRect (GetWindowPortBounds (theWindow, &gTempRect));
			
				// Exit if this was a mouse down in a control area of an		
				// inactive image window.  We will assume that the user was	
				// just wanting to select the window.								
				
		if (gNewSelectedWindowFlag)
																					return;
																					
		GraphWControlEvent (whichControl, theWindow, mousePt, partCode);
   		
		}	// end "if (partCode > 0)" 
		 
	else	// partCode == 0.  Mouse not within controls 
		{
		
		}	// end "else partCode == 0" 

}	// end "DoGraphWMouseDnContent"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoGraphWUpdateEvent
//
//	Software purpose:	This routine handles the update events for the graph
//							window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 09/27/1991
//	Revised By:			Larry L. Biehl			Date: 06/04/1996		

void DoGraphWUpdateEvent (
				WindowPtr							window, 
				CMGraphView*						graphViewCPtr)
				
{
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle;
	
	SignedByte							handleStatus;
	
	
	if (graphViewCPtr == NULL)
																								return;
	
	graphRecordHandle = graphViewCPtr->GetGraphRecordHandle ();
	graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (graphRecordHandle,
																				&handleStatus);
	
	BeginUpdate (window);
	GetWindowPortBounds (window, &graphRecordPtr->clientRect);
	
	//EraseRect (&window->portRect);
	
	DrawGraph (graphRecordPtr, FALSE);
	
	DrawControls (window);
	
	graphViewCPtr->DrawGraphGrowIcon (graphRecordPtr);
	
	EndUpdate (window);
	
	ClipRect (&graphRecordPtr->clientRect);
	
	MHSetState (graphRecordHandle, handleStatus);
	
}	// end "DoGraphWUpdateEvent"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void GraphChangeSetControlEvent
//
//	Software purpose:	This routine handles the next/previous channel action in 
//							TrackControl for next and previous channel control actions.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/26/1999
//	Revised By:			Larry L. Biehl			Date: 11/15/2002			

pascal void GraphChangeSetControlEvent (
				ControlHandle						theControl, 
				SInt16								partCode)
				
{	
	Point									mousePoint;
	Rect									controlRect;
	
	GraphPtr								graphRecordPtr;
	
	Handle								graphRecordHandle; 
	
	SInt16								refCon;
	
	Boolean								nextGraphFlag;
	
	
			// Return if mouse location has moved outside of control area.			
			
	if (partCode == 0)
																							return;
																							
	GetControlBounds (theControl, &controlRect);
																							
	while (StillDown ()) 
		{				
				// Only zoom if the mouse is still in the control.
				
		::GetMouse (&mousePoint);
		if (!PtInRect (mousePoint, &controlRect))
																							return;
	
				// If the command key is down, then do not need to wait
				// 'gControlOffset' ticks - zoom away.
		
		nextGraphFlag = TRUE;
		if (!(gEventRecord.modifiers & cmdKey))
			{				
					// Make certain that at least 'gControlOffset' ticks 
					// have passed since the last zoom operation.	This is 
					// need for very fast systems where one click of the 
					// mouse may cause two loops through this routine.																		
																									
			if (TickCount () < gNextTime + gControlOffset)
				nextGraphFlag = FALSE;
																								
			}	// end "if (!(gEventRecord.modifiers & cmdKey))"
			
		if (nextGraphFlag)
			{
			gNextTime = TickCount ();
																									
					// Initialize variables.
			
			refCon = (SInt16)GetControlReference (theControl);
			
			graphRecordHandle = GetGraphRecordHandle (gActiveWindowInfoH);
			graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);
			
			DoNextOrPreviousChannel	(graphRecordPtr->graphViewCPtr, refCon);
			
					// Do an event available to load into the gEventRecord the status	
					// of the command key.
					
			gEventRecord.modifiers = GetCurrentKeyModifiers ();
			
			}	// end "if (nextGraphFlag)"
			
		}	// end "while (StillDown ())"

}	// end "GraphChangeSetControlEvent" 	 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GraphWControlEvent
//
//	Software purpose:	This routine handles control events in graph windows. 
//							Graph window control events include advance to next set, go
//							back to previous set.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoMouseDnContent
//
//	Coded By:			Larry L. Biehl			Date: 01/23/1999
//	Revised By:			Larry L. Biehl			Date: 03/17/2018

void GraphWControlEvent (
				ControlHandle						theControl, 
				WindowPtr							windowPtr, 
				Point									location, 
				SInt16								partCode)
				
{
			// Local Structures and Variables
	
	CMGraphView*						graphViewCPtr;		
	GraphPtr								graphRecordPtr;
			
	Handle								graphRecordHandle;
	MenuHandle							popUpMenuHandle;
	
	//#ifdef AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER
	#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 1050
		MenuItemIndex						selection;
	#else
		SInt16								selection;
	#endif
	
	OSErr									errCode;	
			
	SInt16								currentSelection,
											index,
											numberMenuItems,
											refCon;
	
	
			// Get the refCon of the control. This will contain a code for which 
			// control it is.																		
			// refCon = 0: Vertical scroll bar												
			// refCon = 1: Horizontal scroll bar									
	
	refCon = (SInt16)GetControlReference (theControl);
	
	if (refCon >= kNextGraphSetControl && refCon <= kGraphXAxisLabelControl)
		{
		gNextTime = 0;
		
		switch (refCon)
			{
			case kNextGraphSetControl:				// Advance to next graph set
			case kPreviousGraphSetControl:		// Go back to previous graph set 
			
						// If there was a mouseup in this control, advance to the
						// next graph set or go back to the previous one.						
					
				partCode = TrackControl (theControl, location, gGraphChangeSetControlActionPtr);
				UpdateGraphControls (windowPtr);
				break;
				
			case kGraphVectorControl:
				errCode = noErr;
				selection =  0;
				
				graphRecordHandle = GetGraphRecordHandle (windowPtr);
				graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);
				graphViewCPtr = graphRecordPtr->graphViewCPtr;
				
				popUpMenuHandle = GetPopUpMenuHandle (theControl);
				if (popUpMenuHandle != NULL)
					{
					SetUpVectorPopUpMenu (graphViewCPtr, popUpMenuHandle);
												
					selection = TrackControl (theControl, location, NIL);		
					
					if (selection > 0)
						errCode = GetBevelButtonMenuValue (theControl, &selection);					
									
							// Now delete the current menu items.
														
					numberMenuItems = CountMenuItems (gPopUpTemporaryMenu);
					for (index=0; index<numberMenuItems; index++)			
						DeleteMenuItem (gPopUpTemporaryMenu, 1);
						
					}	// end "if (popUpMenuHandle != NULL)"

				if (errCode == noErr && selection > 0)
					{
					SetVectorDisplayList (graphViewCPtr, selection-1);
					
					SetGraphMinMax (gActiveWindowInfoH, kBothXYAxes);
							
					InvalidateGraphWindow (windowPtr, kGraphArea);
						
					}	// end "if (errCode == noErr && selection > 0)"
					
				InvalWindowRect (windowPtr, GetControlBounds (theControl, &gTempRect));
				break;
				
			case kGraphOverlayControl:
				errCode = noErr;
				selection =  0;
				
				graphRecordHandle = GetGraphRecordHandle (windowPtr);
				graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);
				graphViewCPtr = graphRecordPtr->graphViewCPtr;
												
				popUpMenuHandle = GetPopUpMenuHandle (theControl);
				if (popUpMenuHandle != NULL)
					{
					SetUpOverlayPopUpMenu (graphViewCPtr, popUpMenuHandle);
												
					selection = TrackControl (theControl, location, NIL);		
					
					if (selection > 0)
						errCode = GetBevelButtonMenuValue (theControl, &selection);				
									
							// Now delete the current menu items.
														
					numberMenuItems = CountMenuItems (gPopUpTemporaryMenu);
					for (index=0; index<numberMenuItems; index++)			
						DeleteMenuItem (gPopUpTemporaryMenu, 1);
						
					}	// end "if (popUpMenuHandle != NULL)"

				if (errCode == noErr && selection > 0)
					{
					SetOverlayDisplayList (graphViewCPtr, selection);
							
					InvalidateGraphWindow (windowPtr, kGraphArea);
						
					}	// end "if (errCode == noErr && selection > 0)"
					
				InvalWindowRect (windowPtr, GetControlBounds (theControl, &gTempRect));
				break;
				
			case kGraphBinWidthControl:
				errCode = noErr;
				selection =  0;
				
				graphRecordHandle = GetGraphRecordHandle (windowPtr);
				graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);
				graphViewCPtr = graphRecordPtr->graphViewCPtr;
												
				popUpMenuHandle = GetPopUpMenuHandle (theControl);
				if (popUpMenuHandle != NULL)
					{
					SetUpBinWidthPopUpMenu (graphViewCPtr, popUpMenuHandle);
												
					selection = TrackControl (theControl, location, NIL);		
					
					if (selection > 0)
						errCode = GetBevelButtonMenuValue (theControl, &selection);
					
							// Now delete the current menu items.
														
					numberMenuItems = CountMenuItems (gPopUpTemporaryMenu);
					for (index=0; index<numberMenuItems; index++)			
						DeleteMenuItem (gPopUpTemporaryMenu, 1);	
						
					}	// end "if (popUpMenuHandle != NULL)"

				if (errCode == noErr && selection > 0)
					{
					if (SetHistogramBinWidth (graphViewCPtr, selection))
						{
						SetGraphMinMax (graphRecordPtr, kBothXYAxes);
						
						InvalidateGraphWindow (windowPtr, kGraphArea);
						InvalidateGraphWindow (windowPtr, kGraphBinWidth);
						
						}	// end "if (SetHistogramBinWidth (graphViewCPtr, selection))"
						
					}	// end "if (errCode == noErr && selection > 0)"
					
				InvalWindowRect (windowPtr, GetControlBounds (theControl, &gTempRect));
				break;
				
			case kGraphXAxisLabelControl:
				errCode = noErr;
				selection =  0;
				
				graphRecordHandle = GetGraphRecordHandle (windowPtr);
				graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);
				
				currentSelection = graphRecordPtr->xAxisCode;
				
				popUpMenuHandle = GetPopUpMenuHandle (theControl);
				if (popUpMenuHandle != NULL)
					{
					SetUpXAxisPopUpMenu (graphRecordPtr, popUpMenuHandle);
												
					TrackControl (theControl, location, (ControlActionUPP)-1);	
					selection = GetControlValue (theControl);
					
					if (errCode == noErr &&
								selection > 0 &&
										currentSelection != selection)
						{
						graphRecordPtr->xAxisCode = selection;
						
						if (selection == kChannels)
							graphRecordPtr->xValueTypeCode = kIntegerType;
							
						else	// selection != kChannels
							graphRecordPtr->xValueTypeCode = kRealType;

						ReloadXAxis (graphRecordPtr, selection);
						
						if (graphRecordPtr->processorCode == kListDataProcessor)
							InvalidateGraphWindow (graphRecordPtr->window, kGraphArea);
						
						}	// end "if (errCode == noErr && ..."
					
					}	// end "if (popUpMenuHandle != NULL)"
				break;
				
			default:
				break;
	    	
			}	// end "switch (refCon)" 

		}	// end of "if (refCon >= kNextGraphSetControl && ..." 
	  
}	// end "GraphWControlEvent"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateGraphControls
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
// Called By:			DoGraphWActivateEvent in SGraUtil.cpp
//							GraphWControlEvent in SGraUtil.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/25/1999
//	Revised By:			Larry L. Biehl			Date: 03/16/2018

void UpdateGraphControls (
				WindowPtr							windowPtr)

{	     
	GraphPtr								graphRecordPtr;
	             
	ControlHandle						controlHandle,
											rootControlHandle = NULL;
												
	Handle								graphRecordHandle;
	
	SInt16								numberSets,
											refCon,
											set;		
	
	UInt16								index,
											numberControls;
	
	
			// Update graph window controls
				
	GetRootControl (windowPtr, &rootControlHandle);

	if (rootControlHandle == NULL)
																						return;
	
	graphRecordHandle = GetGraphRecordHandle (windowPtr);
	graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle);
											
	numberSets = graphRecordPtr->numberSets;
	set = graphRecordPtr->set;
											
	CountSubControls (rootControlHandle, &numberControls);
	for (index=numberControls; index>=1; index--)
		{
		GetIndexedSubControl (rootControlHandle, index, &controlHandle);
		refCon = (SInt16)GetControlReference (controlHandle);
		
		switch (refCon)
			{
			case kNextGraphSetControl:
				if (numberSets <= 1)
					HideControl (controlHandle);
					
				else	// numberSets > 1
					{
					if (set < numberSets)
						HiliteControl (controlHandle, 0);
						
					else	// set >= numberSets
						HiliteControl (controlHandle, 255);
						
					}	// end "else numberSets > 1"
				break;
				
			case kPreviousGraphSetControl:
				if (numberSets <= 1)
					HideControl (controlHandle);
					
				else	// >numberSets > 1
					{
					if (set > 1)
						HiliteControl (controlHandle, 0);
						
					else	// >set <= 1
						HiliteControl (controlHandle, 255);
						
					}	// end "else numberSets <= 1"
				break;
				
			case kGraphVectorControl:
				HiliteControl (controlHandle, 0);
				break;
				
			case kGraphOverlayControl:
				HiliteControl (controlHandle, 0);
				break;
				
			case kGraphBinWidthControl:
				HiliteControl (controlHandle, 0);
				break;
				
			case kGraphXAxisLabelControl:
				HiliteControl (controlHandle, 0);
				break;
			
			}	// end "switch (refCon)" 
				
		}	// end "while (controlHandle != NULL)" 
	
}	// end "UpdateGraphControls"

