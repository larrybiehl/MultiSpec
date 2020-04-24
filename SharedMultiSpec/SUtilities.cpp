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
//	File:						SUtilities.cpp
//
//	Authors:					Larry L. Biehl, Ravi Budruk
//
//	Revision date:			04/16/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to
//								provide utility type functions in MultiSpec that are 
//								shared in the Linux, Mac and Windows versions.
//
/* Template for debugging
		int numberChars = sprintf ((char*)gTextString3,
												" SUtility.cpp::xxx (entered routine. %s", 
												gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);	
*/
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include	"SImageWindow_class.h"
#include	<ctype.h>  

#if defined multispec_wx
	#include "wx/dcmemory.h"
	#include "wx/gdicmn.h"
	#include "wx/wx.h"
	#include "SFileStream_class.h"
	#include "xDialog.h"
	#include "xImageDoc.h"
	#include "xImageFrame.h"
	#include "xImageView.h" 
	#include "xLegendList.h" 
	#include "xLegendView.h"
	#include "xMainFrame.h"
	#include "xTitleBar.h"
#endif

#if defined multispec_mac
	#include	"MGraphView.h"
#endif	// defined multispec_mac    
                            
#if defined multispec_win
	#include	"SDisplay_class.h"
	#include "SHistogram_class.h"
	#include "WMultiSpec.h"
	//#include "CProcessor.h"

	#include "WDialog.h"
	#include	"WGraphView.h"
	#include "WImageDoc.h"
	#include	"WImageView.h"
	#include "WTextDoc.h"
	#include "WTextView.h"
	#include "WLegendView.h"	

	extern Boolean GetSelectionRectangle (
					WindowPtr							windowPtr,
					LongRect*							selectionRectanglePtr, 
					Boolean								clearSelectionFlag, 
					Boolean								useThresholdFlag, 
					Boolean								adjustToBaseImageFlag);
#endif	// defined multispec_win



			// Prototypes for routines in this file that are only called by		
			// other routines in this file.											

Boolean 	CreateResultsDiskFile (
				SInt32								numberLines,
				SInt32								numberColumns,
				SInt32								numberBytes, 
				SInt16								lOutputStorageType, 
				SInt16								numberClasses,
				SInt16								paletteSelection, 
				DiskFileListPtr					diskFileListPtr, 
				Boolean								promptFlag, 
				SInt16								vRefNum,
				SInt32								parID,
				Boolean								fSSpecFlag);

Boolean 	GetNumberNonZeroLeadingDigits (
				double								dataValue,
				double								floorLogDataValue,
				UInt16*								nonZeroIntevalDigitsPtr);


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckIfOffscreenImageExists
//
//	Software purpose:	The purpose of this routine is to determine if a offscreen
//							image exists.
//
//	Parameters in:		None			
//
//	Parameters out:	None			
//
//	Value Returned:	TRUE if offscreen image exists
//							FALSE if offscreen image does not exist
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 08/01/1995
//	Revised By:			Larry L. Biehl			Date: 11/30/1995	

Boolean CheckIfOffscreenImageExists (
				WindowInfoPtr						windowInfoPtr)

{  
	Boolean								returnFlag = FALSE;
	
	
	if (windowInfoPtr != NULL)
		{                               
		#if defined multispec_mac 
			returnFlag = (windowInfoPtr->imageBaseAddressH != NULL ||
														windowInfoPtr->offscreenGWorld != NULL);
		#endif	// defined multispec_mac
		            
		#if defined multispec_win || defined multispec_wx
			returnFlag = (windowInfoPtr->imageBaseAddressH != NULL);
		#endif	// defined multispec_win || defined multispec_wx
		
		}	// end "if (windowInfoPtr != NULL)"

	return (returnFlag);
	
}	// end "CheckIfOffscreenImageExists"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CheckIfValueInList
//
//	Software purpose:	This routine determines if the input value is included in the 
//							input list of values. 
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/10/2013
//	Revised By:			Larry L. Biehl			Date: 04/10/2013		

SInt16 CheckIfValueInList (
				SInt16*								listPtr,
				SInt16								numberValuesInList,
				SInt16								value)

{
	UInt32								index;
	
	SInt16								locationInList;
	

			// Determine if adjust selected channel is included in the selected
			// channels to be adjusted.
	
	locationInList = -1;
	for (index=0; index<(UInt32)numberValuesInList; index++)
		{
		if (listPtr[index] == value)
			{
			locationInList = (SInt16)index;
			break;
			
			}	// end "if (listPtr[index] == value)"
		
		}	// end "for (index=0; index<(UInt32)numberValuesInList; index++)"
		
	return (locationInList);
	
}	// end "CheckIfValueInList"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ClearAreaDescriptionOffsetVariables
//
//	Software purpose:	This routine clears variables within the area description
//							structure that relate to the application of offsets in the
//							field boundary information. 
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/19/1999
//	Revised By:			Larry L. Biehl			Date: 03/19/1999		

void ClearAreaDescriptionOffsetVariables (
				AreaDescriptionPtr				areaDescriptionPtr)

{
	areaDescriptionPtr->applyOffsetFlag = FALSE;
		
	areaDescriptionPtr->columnOffset = 0;
	areaDescriptionPtr->lineOffset = 0;
		
	areaDescriptionPtr->maxNumberColumns = 0;
	areaDescriptionPtr->maxNumberLines = 0;
		
}	// end "ClearAreaDescriptionOffsetVariables" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void CloseResultsFiles
//
//	Software purpose:			The purpose of this routine is to close all
//									results files.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/19/1991
//	Revised By:			Larry L. Biehl			Date: 01/16/1996	

void CloseResultsFiles (void)

{
	DiskFileListPtr					diskFileListPtr;
	
	SInt16								index,
											numberFiles;
	
	
			// Initialize local variables.												
	
	numberFiles = gResultsFileSummary.numberFiles;
	
	if (numberFiles > 0)
		{
		diskFileListPtr = gResultsFileSummary.diskFileListPtr;
		
		if (diskFileListPtr != NULL)
			{
			for (index=0; index<numberFiles; index++)
				{      
				DisposeFileInfoHandle (diskFileListPtr->fileInfoH);
					
				diskFileListPtr++;
				
				}	// end "for (index=0; index<numberFiles; index++)" 
				
			}	// end "if (diskFileListPtr != NULL)" 
		
		}	// end "if (numberFiles > 0)" 
	
	UnlockAndDispose (gResultsFileSummary.diskFileListH);
	
	gResultsFileSummary.numberFiles = 0;
	gResultsFileSummary.diskFileListPtr = NULL;
	gResultsFileSummary.diskFileListH = NULL;

}	// end "CloseResultsFiles"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CloseTheProject
//
//	Software purpose:	The purpose of this routine is to close the project
//							and the statistics window and all structures connected
//							with them.  A check is made to give the user a chance
//							to save the project file if it has been changed since
//							the last save.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	True: continue closing the projectd
//							False: User cancelled closing the project (and session if this
//										is from a application exit event.
// 
// Called By:			Menus in MMenus.c
//							ExitMultiSpec in MMultiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 01/05/1989
//	Revised By:			Larry L. Biehl			Date: 11/27/2017	

Boolean CloseTheProject (void)

{
	SInt16								returnCode;

				
	returnCode = 0;
	if (gProjectInfoPtr != NULL)
		{	
		if (gProjectInfoPtr->changedFlag)
			returnCode = SaveProjectFile (2);
			
		if (returnCode == 0)
			{		
			CloseProjectStructure (gProjectInfoPtr);
			       
			CloseProjectWindow ();
			                   
			}	// end "if (returnCode == 0)" 
			
		}	// end "if (gProjectInfoPtr != NULL)" 
		
	if (returnCode == 0)  
		return (TRUE);
	
	else	// returnCode != 0, User cancelled the close project.  
		return (FALSE);

}	// end "CloseTheProject"     



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CloseUpAreaDescription
//
//	Software purpose:	The purpose of this routine is to release any handles or
//							pointers that have been set up in the input area description
//							structure.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned: 		
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/23/1998
//	Revised By:			Larry L. Biehl			Date: 01/05/1999

void CloseUpAreaDescription (
				AreaDescriptionPtr				areaDescriptionPtr)

{
	if (areaDescriptionPtr != NULL)
		{
		areaDescriptionPtr->maskInfoPtr = NULL;
		
		if (areaDescriptionPtr->rgnHandle != NULL)
			DisposeRgn (areaDescriptionPtr->rgnHandle);
			
		areaDescriptionPtr->rgnHandle = NULL;
		areaDescriptionPtr->polygonFieldFlag = FALSE;
		
		areaDescriptionPtr->pointType = kRectangleType; 
		
		}	// end "if (areaDescriptionPtr != NULL)"
	
}	// end "CloseUpAreaDescription"    



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertLCToOffscreenPoint
//
//	Software purpose:	This routine converts the input image line and
//							column to the coordinates in the offscreen map
//							for the window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ComputeSelectionOffscreenRectangle in SSelectionUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/30/1989
//	Revised By:			Larry L. Biehl			Date: 10/09/1998			

void ConvertLCToOffscreenPoint (
				DisplaySpecsPtr					displaySpecsPtr, 
				LongPoint*							selectedlineColPtr, 
				LongPoint*							offscreenPointPtr)

{
			// Get the selected point location relative to the offscreen			
			// bit/pix map																			
	
	offscreenPointPtr->v = (SInt32)
		((selectedlineColPtr->v - displaySpecsPtr->displayedLineStart)/
														displaySpecsPtr->displayedLineInterval);
											
	offscreenPointPtr->h = (SInt32)
		((selectedlineColPtr->h - displaySpecsPtr->displayedColumnStart)/
														displaySpecsPtr->displayedColumnInterval);

}	// end "ConvertLCToOffscreenPoint"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertLCToWinPoint
//
//	Software purpose:	This routine converts the input image line and
//							column to window point in local pixel coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			OutlineField in SOutlineFields.cpp	
//							OutlineSelectionArea in SSelectionUtility.cpp
//							PolygonSelection in SSelectionUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/04/1988
//	Revised By:			Larry L. Biehl			Date: 09/22/2015		

void ConvertLCToWinPoint (
				LongPoint*							lineColumnPointPtr, 
				LongPoint*							windowPointPtr, 
				LCToWindowUnitsVariablesPtr	lcToWindowUnitsVariablesPtr)

{
	windowPointPtr->v = (SInt32)((lineColumnPointPtr->v + 
										lcToWindowUnitsVariablesPtr->lineOffset)/
													lcToWindowUnitsVariablesPtr->lineInterval);
											                                 
	windowPointPtr->h = (SInt32)((lineColumnPointPtr->h + 
										lcToWindowUnitsVariablesPtr->columnOffset)/
													lcToWindowUnitsVariablesPtr->columnInterval);
	
			// Now convert the offscreen coordinates to window coordinates.

	windowPointPtr->v -= lcToWindowUnitsVariablesPtr->yOrigin;
	windowPointPtr->h -= lcToWindowUnitsVariablesPtr->xOrigin;
	
	windowPointPtr->v = (SInt32)(windowPointPtr->v *
													lcToWindowUnitsVariablesPtr->magnification);
	windowPointPtr->h = (SInt32)(windowPointPtr->h *
													lcToWindowUnitsVariablesPtr->magnification);

	windowPointPtr->v += lcToWindowUnitsVariablesPtr->imageTopOffset;
	windowPointPtr->h += lcToWindowUnitsVariablesPtr->imageLeftOffset;
	
			// This takes into account the possibility of a side by side			
			// channel display.																	
			
	windowPointPtr->h += lcToWindowUnitsVariablesPtr->channelWindowOffset;
	
}	// end "ConvertLCToWinPoint"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertLCRectToWinRect
//
//	Software purpose:	This routine converts the input line/column
//							rectangle coordinates to the current window 
//							rectangle coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			OutlineField in SOutlineFields.cpp
//							ClearSelectionArea in SSelectionUtility.cpp
//							OutlineSelectionArea in SSelectionUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/20/1990
//	Revised By:			Larry L. Biehl			Date: 10/09/2015		

void ConvertLCRectToWinRect (
				LongRect*							lineColumnRect, 
				LongRect*							windowRectPtr, 
				LCToWindowUnitsVariablesPtr	lcToWindowUnitsVariablesPtr)

{
			// Get the line/column rectangle relative to the offscreen		
			// bit/pix map																	
	
	windowRectPtr->top = (SInt32)((lineColumnRect->top + 
										lcToWindowUnitsVariablesPtr->lineOffset)/
													lcToWindowUnitsVariablesPtr->lineInterval);
											
	windowRectPtr->left = (SInt32)((lineColumnRect->left + 
										lcToWindowUnitsVariablesPtr->columnOffset)/
													lcToWindowUnitsVariablesPtr->columnInterval);
	
	windowRectPtr->bottom = (SInt32)((lineColumnRect->bottom  + 
										lcToWindowUnitsVariablesPtr->lineOffset)/
													lcToWindowUnitsVariablesPtr->lineInterval);
											
	windowRectPtr->right = (SInt32)((lineColumnRect->right + 
										lcToWindowUnitsVariablesPtr->columnOffset)/
													lcToWindowUnitsVariablesPtr->columnInterval);
		
			// Now convert the offscreen coordinates to window coordinates.

	windowRectPtr->top -= lcToWindowUnitsVariablesPtr->yOrigin;
	windowRectPtr->left -= lcToWindowUnitsVariablesPtr->xOrigin;
	windowRectPtr->bottom -= lcToWindowUnitsVariablesPtr->yOrigin;
	windowRectPtr->right -= lcToWindowUnitsVariablesPtr->xOrigin;
	
	windowRectPtr->bottom++;
	windowRectPtr->right++;
					
	windowRectPtr->top = (SInt32)(windowRectPtr->top *
													lcToWindowUnitsVariablesPtr->magnification);
	windowRectPtr->left = (SInt32)(windowRectPtr->left *
													lcToWindowUnitsVariablesPtr->magnification);
	windowRectPtr->bottom = (SInt32)(windowRectPtr->bottom *
													lcToWindowUnitsVariablesPtr->magnification);
	windowRectPtr->right = (SInt32)(windowRectPtr->right *
													lcToWindowUnitsVariablesPtr->magnification);
	
	windowRectPtr->top += lcToWindowUnitsVariablesPtr->imageTopOffset;
	windowRectPtr->bottom += lcToWindowUnitsVariablesPtr->imageTopOffset;
	windowRectPtr->left += lcToWindowUnitsVariablesPtr->imageLeftOffset;
	windowRectPtr->right += lcToWindowUnitsVariablesPtr->imageLeftOffset;
	
			// This takes into account the possibility of a side by side	
			// channel display.															
			
	windowRectPtr->left += lcToWindowUnitsVariablesPtr->channelWindowOffset;
	windowRectPtr->right += lcToWindowUnitsVariablesPtr->channelWindowOffset;

}	// end "ConvertLCRectToWinRect" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertOffScreenPointToWinPoint
//
//	Software purpose:	This routine converts the input image offscreen
//							point coordinates to the current window 
//							point coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:			DoImageZoom in MControls.c
//							DoWindowZoomInOutEvent in MMultiSpec.c	
//
//	Coded By:			Larry L. Biehl			Date: 08/30/1989
//	Revised By:			Larry L. Biehl			Date: 01/03/2019

void ConvertOffScreenPointToWinPoint (
				Handle								windowInfoH, 
				LongPoint*							offscreenPointPtr, 
				LongPoint*							windowPointPtr)

{	
	double								magnification;
	DisplaySpecsPtr					displaySpecsPtr;
	Handle								displaySpecsH;
	
	
			// Initialize output window point.												
			
	windowPointPtr->h = offscreenPointPtr->h;
	windowPointPtr->v = offscreenPointPtr->v;
	
	if (windowInfoH != NULL)
		{
				// Get pointer to display specification.  We do not need to lock 	
				//	it here since no other routines are called							
		
		displaySpecsH = GetDisplaySpecsHandle (windowInfoH);
		if (displaySpecsH != NULL)
			{
			displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsH);
			magnification = displaySpecsPtr->magnification;
	
			//windowPointPtr->v -= displaySpecsPtr->origin[kVertical];
			//windowPointPtr->h -= displaySpecsPtr->origin[kHorizontal];
							
			//windowPointPtr->v = (SInt32)(windowPointPtr->v * magnification);
			//windowPointPtr->h = (SInt32)(windowPointPtr->h * magnification);
			
			windowPointPtr->v = (SInt32)(magnification *
								(windowPointPtr->v - displaySpecsPtr->origin[kVertical]));
			windowPointPtr->h = (SInt32)(magnification *
								(windowPointPtr->h - displaySpecsPtr->origin[kHorizontal]));
			
			WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																							windowInfoH);
													
			windowPointPtr->v += windowInfoPtr->imageTopOffset;
			windowPointPtr->h += windowInfoPtr->legendWidth;
		
			}	// end "if (displaySpecsH != NULL)" 
			
		}	// end "if (windowInfoH != NULL)" 

}	// end "ConvertOffScreenPointToWinPoint" 	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertOffScreenRectToWinRect
//
//	Software purpose:	This routine converts the input image offscreen
//							rect coordinates to the current window rect coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:				
//
//	Coded By:			Larry L. Biehl			Date: 01/20/2003
//	Revised By:			Larry L. Biehl			Date: 01/20/2003			

void ConvertOffScreenRectToWinRect (
				LongRect*							offscreenRectPtr, 
				Rect*									windowRectPtr,
				LCToWindowUnitsVariables*		lcToWindowUnitsVariablesPtr)

{						
	windowRectPtr->top = (SInt16)(lcToWindowUnitsVariablesPtr->imageTopOffset + 
				lcToWindowUnitsVariablesPtr->magnification *
						(offscreenRectPtr->top - lcToWindowUnitsVariablesPtr->yOrigin));
												
	windowRectPtr->bottom = (SInt16)(lcToWindowUnitsVariablesPtr->imageTopOffset + 
				lcToWindowUnitsVariablesPtr->magnification *
						(offscreenRectPtr->bottom - lcToWindowUnitsVariablesPtr->yOrigin));
												
	windowRectPtr->left = (SInt16)(lcToWindowUnitsVariablesPtr->imageLeftOffset + 
				lcToWindowUnitsVariablesPtr->magnification *
						(offscreenRectPtr->left - lcToWindowUnitsVariablesPtr->xOrigin));
													
	windowRectPtr->right = (SInt16)(lcToWindowUnitsVariablesPtr->imageLeftOffset + 
				lcToWindowUnitsVariablesPtr->magnification *
						(offscreenRectPtr->right - lcToWindowUnitsVariablesPtr->xOrigin));

}	// end "ConvertOffScreenRectToWinRect" 	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertWinPointToLC
//
//	Software purpose:	This routine returns the input image window point 
//							in local coordinates to image line-column value.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			UpdateCursorCoordinates in MMultiSpec.c
//							PolygonSelection in SSelectionArea.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/04/1988
//	Revised By:			Larry L. Biehl			Date: 02/29/2020

void ConvertWinPointToLC (
				LongPoint*							selectedPointPtr, 
				LongPoint*							lineColPointPtr)

{	
	double								magnificationFactor;
	
	DisplaySpecsPtr					displaySpecsPtr;
	
	Handle								displaySpecsHandle;
	
	SInt32								channelWindowInterval,
											columnOrigin,
											lineOrigin,
											numberChannelWidths,
											//originPixelXOffset,
											//originPixelYOffset,
											startChannelWindow;

	SInt16								imageTopOffset,
											legendWidth;
	
	
			// Get pointer to display specification.  We do not need to lock it	
			// here since no other routines are called.									
			
	displaySpecsHandle = GetActiveDisplaySpecsHandle ();
				
	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);
	
			// Get the magnification factors to use to convert from screen			
			// elements to lines and columns. Vertical and horizontal are 			
			// currently the same number of pixels per image pixel.					
			
	magnificationFactor = displaySpecsPtr->magnification;
																
			// Get the title height and legend width for the window.
	
	#if defined multispec_wx
		imageTopOffset = 0;
		legendWidth = 0;
	#endif	// defined multispec_wx
			
	#if defined multispec_mac
		imageTopOffset = ((WindowInfoPtr)*gActiveImageWindowInfoH)->imageTopOffset;
		legendWidth = ((WindowInfoPtr)*gActiveImageWindowInfoH)->legendWidth;
		#endif	// defined multispec_mac
	
	#if defined multispec_win                 
		imageTopOffset = gActiveImageViewCPtr->GetTitleHeight ();                
		legendWidth = 0;
	#endif	// defined multispec_win

			// Get the coordinates of the origin of the window 						
			
	columnOrigin = (SInt32)(displaySpecsPtr->displayedColumnStart +
							displaySpecsPtr->origin[kHorizontal] *
										displaySpecsPtr->displayedColumnInterval);
					
	lineOrigin = (SInt32)(displaySpecsPtr->displayedLineStart +
								displaySpecsPtr->origin[kVertical] *
											displaySpecsPtr->displayedLineInterval);
	
	selectedPointPtr->h -= legendWidth;
	
	startChannelWindow = -(SInt32)(displaySpecsPtr->origin[kHorizontal] * 
															displaySpecsPtr->magnification);
	selectedPointPtr->h -= startChannelWindow;
	
	channelWindowInterval = (SInt32)(displaySpecsPtr->offscreenChannelWidth * 
																displaySpecsPtr->magnification);
																
	numberChannelWidths = 0;
	if (displaySpecsPtr->displayType == kSideSideChannelDisplayType &&
								channelWindowInterval > 0)
		numberChannelWidths = selectedPointPtr->h/channelWindowInterval;
	
	selectedPointPtr->h -= numberChannelWidths * channelWindowInterval;
	if (numberChannelWidths > 0)
		columnOrigin = 1;
		
	else
		selectedPointPtr->h += startChannelWindow;
	
	lineColPointPtr->h = (SInt32)(columnOrigin +
				selectedPointPtr->h/magnificationFactor *
													displaySpecsPtr->displayedColumnInterval);
					
	lineColPointPtr->v = (SInt32)(lineOrigin +
				(selectedPointPtr->v-imageTopOffset)/magnificationFactor *
													displaySpecsPtr->displayedLineInterval);
	
	if (displaySpecsPtr->displayedColumnEnd > 0)
		{
				// Image is being displayed in the window. Check against the area
				// being displayed.
		
		lineColPointPtr->h =
				MIN (lineColPointPtr->h, (SInt32)displaySpecsPtr->displayedColumnEnd);
		lineColPointPtr->v =
				MIN (lineColPointPtr->v, (SInt32)displaySpecsPtr->displayedLineEnd);
		
		}	// end "if (displaySpecsPtr->displayedColumnEnd > 0)"
	
	else	// displaySpecsPtr->displayedColumnEnd == 0
		{
				// Image is not being displayed. Check against the maximum size of the
				// image file being referenced.
				
		lineColPointPtr->h =
					(SInt32)MIN (lineColPointPtr->h,
									(double)GetMaxNumberColumns (gActiveImageWindowInfoH));
		lineColPointPtr->v =
					(SInt32)MIN (lineColPointPtr->v,
									(double)GetMaxNumberLines (gActiveImageWindowInfoH));
		
		}	// end "else displaySpecsPtr->displayedColumnEnd == 0"
	

}	// end "ConvertWinPointToLC"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertWinPointToDoubleLC
//
//	Software purpose:	This routine returns the input image window point 
//							in local coordinates to image decimal line-column values.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			UpdateCursorCoordinates in MMultiSpec.c
//							PolygonSelection in SSelectionUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/08/2000
//	Revised By:			Larry L. Biehl			Date: 01/11/2020

void ConvertWinPointToDoubleLC (
				LongPoint*							selectedPointPtr, 
				DoublePoint*						lineColPointPtr)

{	
	double								columnOrigin,
											lineOrigin,
											magnificationFactor;
	
	DisplaySpecsPtr					displaySpecsPtr;
	
	Handle								displaySpecsHandle;
	
	SInt32								channelWindowInterval,
											numberChannelWidths,
											startChannelWindow;

	SInt16								imageTopOffset,
											legendWidth;
	
	
			// Get pointer to display specification.  We do not need to lock it	
			// here since no other routines are called.									
			
	displaySpecsHandle = GetActiveDisplaySpecsHandle ();
				
	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);
	
			// Get the magnification factors to use to convert from screen			
			// elements to lines and columns. Vertical and horizontal are 			
			// currently the same number of pixels per image pixel.					
			
	magnificationFactor = displaySpecsPtr->magnification;
																
			// Get the title height and legend width for the window.					
			
	#if defined multispec_mac
		imageTopOffset = ((WindowInfoPtr)*gActiveImageWindowInfoH)->imageTopOffset;
		legendWidth = ((WindowInfoPtr)*gActiveImageWindowInfoH)->legendWidth;
	#endif	// defined multispec_mac
	
	#if defined multispec_win                 
		imageTopOffset = gActiveImageViewCPtr->GetTitleHeight ();                
		legendWidth = 0;
	#endif	// defined multispec_win
	
	#if defined multispec_wx
		imageTopOffset = 0;
		legendWidth = 0;
	#endif	// defined multispec_wx

			// Get the coordinates of the origin of the window 						
			
	columnOrigin = displaySpecsPtr->displayedColumnStart +
					((double)displaySpecsPtr->origin[kHorizontal]*
										displaySpecsPtr->displayedColumnInterval) - 1;
					
	lineOrigin = displaySpecsPtr->displayedLineStart +
					((double)displaySpecsPtr->origin[kVertical]*
											displaySpecsPtr->displayedLineInterval) - 1;
	
	selectedPointPtr->h -= legendWidth;
	
	startChannelWindow = -(SInt32)(displaySpecsPtr->origin[kHorizontal] * 
																	displaySpecsPtr->magnification);
	selectedPointPtr->h -= startChannelWindow;
	
	channelWindowInterval = (SInt32)(displaySpecsPtr->offscreenChannelWidth * 
																	displaySpecsPtr->magnification);
																
	numberChannelWidths = 0;
	if (displaySpecsPtr->displayType == kSideSideChannelDisplayType &&
																	channelWindowInterval > 0)
		numberChannelWidths = selectedPointPtr->h/channelWindowInterval;
	
	selectedPointPtr->h -= numberChannelWidths * channelWindowInterval;
	if (numberChannelWidths > 0)
		columnOrigin = 0;
		
	else	// numberChannelWidths <= 0
		selectedPointPtr->h += startChannelWindow;
	
	lineColPointPtr->h = (double)(columnOrigin + (selectedPointPtr->h *
				displaySpecsPtr->displayedColumnInterval/magnificationFactor));
					
	lineColPointPtr->v = (double)(lineOrigin + ((selectedPointPtr->v-imageTopOffset) * 
					displaySpecsPtr->displayedLineInterval/magnificationFactor));
	
	if (displaySpecsPtr->displayedColumnEnd > 0)
		{
				// Image is being displayed in the window. Check against the area
				// being displayed.
		
		lineColPointPtr->h =
				MIN (lineColPointPtr->h, (double)displaySpecsPtr->displayedColumnEnd);
		lineColPointPtr->v =
				MIN (lineColPointPtr->v, (double)displaySpecsPtr->displayedLineEnd);
		
		}	// end "if (displaySpecsPtr->displayedColumnEnd > 0)"
	
	else	// displaySpecsPtr->displayedColumnEnd == 0
		{
				// Image is not being displayed. Check against the maximum size of the
				// image file being referenced.
				
		lineColPointPtr->h =
							MIN (lineColPointPtr->h,
									(double)GetMaxNumberColumns (gActiveImageWindowInfoH));
		lineColPointPtr->v =
							MIN (lineColPointPtr->v,
									(double)GetMaxNumberLines (gActiveImageWindowInfoH));
		
		}	// end "else displaySpecsPtr->displayedColumnEnd == 0"

}	// end "ConvertWinPointToDoubleLC"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CreateBackgroundImageFile
//
//	Software purpose:	The purpose of this routine is to create a Thematic
//							image file for the specified size with all pixels
//							being equal to the background value.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			CreateClusterMaskFile in SCluster.cpp
//							AreasToThematicFileControl in SFieldsToThematicFile.cpp
//							ShapeToThematicFileControl in SShapeToThematic.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/29/1990
//	Revised By:			Larry L. Biehl			Date: 12/08/2014

Boolean CreateBackgroundImageFile (
				FileInfoPtr							fileInfoPtr, 
				HUCharPtr							ioOutBufferPtr,
				SInt16								tiffSourceCode,
				UInt32								numberLinesToFill,
				unsigned char						backgroundValue,
				//SInt16								headerFormat,
				Boolean								writeBackgroundDataToFileFlag)
			
{
	HUCharPtr					 		endioOutBufferPtr,
											localIOOutBufferPtr;
											
	UInt16								*endioOut2ByteBufferPtr,
											*localIOOut2ByteBufferPtr;
											
	UInt32								count,
											line,
											numberLines;
											
	SInt16								errCode,
											paletteSelection;
	
	Boolean								continueFlag;


			// Check input parameters.															
			
	if (fileInfoPtr == NULL || ioOutBufferPtr == NULL)
																						return (FALSE);
																				
			// Initialize local parameters.
			
	continueFlag = TRUE;
																				
			// Write image header if requested.	
			
	paletteSelection = kNoPaletteColorsDefined;
	if (tiffSourceCode == kFromClassification)
		paletteSelection = kDefaultColors;					
	
	if (fileInfoPtr->format != kNoneType)		
		continueFlag = WriteNewImageHeader (NULL,
														fileInfoPtr, 
														(HPtr)ioOutBufferPtr,
														NULL,
														tiffSourceCode,	// kFromReformat
														kNewFileMenuItem, 
														1,
														1,
														kClassDisplay,
														paletteSelection);	
	
			//	Title for creating background image.										

	LoadDItemStringNumber (kClusterStrID,
									IDS_Cluster46,	// "\pCreating Cluster Mask Background"
									gStatusDialogPtr, 
									IDC_Status11,
									(Str255*)gTextString);
									
	CheckSomeEvents (updateMask);

	if (continueFlag)
		{
		numberLines = fileInfoPtr->numberLines;
		
				// Fill buffer with background values for one line of image data.
					
		if (fileInfoPtr->numberBytes == 1)
			{
			localIOOutBufferPtr = ioOutBufferPtr;
			endioOutBufferPtr = &ioOutBufferPtr[
											numberLinesToFill * fileInfoPtr->numberColumns];
			for (; localIOOutBufferPtr<endioOutBufferPtr; localIOOutBufferPtr++)
				*localIOOutBufferPtr = backgroundValue;
				
			}	// end "if (fileInfoPtr->numberBytes == 1)"
			
		else	// fileInfoPtr->numberBytes == 2
			{
			localIOOut2ByteBufferPtr = (UInt16*)ioOutBufferPtr;
			endioOut2ByteBufferPtr = &localIOOut2ByteBufferPtr[
											numberLinesToFill * fileInfoPtr->numberColumns];
			for (; localIOOut2ByteBufferPtr<endioOut2ByteBufferPtr;
							localIOOut2ByteBufferPtr++)
				*localIOOut2ByteBufferPtr = backgroundValue;
				
			}	// end "else fileInfoPtr->numberBytes == 2"
		
		if (writeBackgroundDataToFileFlag)
			{
					// Write buffer to disk file for the number of lines in the			
					// output file.																	
					
			count = fileInfoPtr->numberColumns * fileInfoPtr->numberBytes;
			CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			for (line=0; line<numberLines; line++)
				{
				errCode = MWriteData (fileStreamPtr, 
												&count, 
												ioOutBufferPtr,
												kErrorMessages);
				
				if (errCode != noErr)
					break;
				
				}	// end "for (line=0; line<numberLines; line++)" 
				
			if (errCode != noErr)
				continueFlag = FALSE;
				
			}	// end "if (writeBackgroundDataToFileFlag)"
		
		}	// end "if (continueFlag)" 
					
	return (continueFlag);
		
}	// end "CreateBackgroundImageFile" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CreateResultsDiskFile
//
//	Software purpose:	The purpose of this routine is to set up a disk
//							file to write the results to. First
//							the file name is determined, amount of space left
//							on the volume is checked against the estimated size
//							of the file and then the file is opened if there is
//							enough space.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			CreateResultsDiskFiles in SUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/18/1989
//	Revised By:			Larry L. Biehl			Date: 04/12/2020

Boolean CreateResultsDiskFile (
				SInt32								numberLines, 
				SInt32								numberColumns,
				SInt32								numberBytes, 
				SInt16								lOutputStorageType, 
				SInt16								numberClasses,
				SInt16								paletteSelection, 
				DiskFileListPtr					diskFileListPtr, 
				Boolean								promptFlag, 
				SInt16								vRefNum,
				SInt32								parID,
				Boolean								fSSpecFlag)

{
	SInt64								numberHeaderBytes;
	
	FileStringPtr 						filePathPointer,
											resultsFilePathPtr;
	
	CMFileStream						*fileStreamPtr,
											*resultsFileStreamPtr;
	
	FileInfoPtr							fileZeroInfoPtr,
											resultsFileInfoPtr;
											 
	SInt32								creator;
                                           
	SInt16				 				errCode,
											formatVersion,
											promptStringIndex,
											stringIndex;  
	
	Boolean								abortedFlag,
											checkIfDirectoryIsWritableFlag,
											writeHeaderFlag;

	
			// Either the 2, 4, 8, 16, 32 or 64 bit should be set to indicate 			
			// the format of the results file.												
			
	if (!(lOutputStorageType & 0x007E))
																						return (FALSE);
																					
			// Initialize local variables.													
			
	errCode = noErr;
	abortedFlag = FALSE;
	writeHeaderFlag = TRUE;
	promptStringIndex = IDS_SaveClassificationAs;	// kFileIOStr13;
	checkIfDirectoryIsWritableFlag = FALSE;
	
			// Get a handle to a block of memory to be used for file information	
			// for the classificaton output file. Lock the memory until the 		
			// information has been loaded in. Then get the pointer to the file 	
			// information block.							
		
			// Initialize the variables and handles in the structure.				
			
	diskFileListPtr->fileInfoH = InitializeFileInfoStructure (
																			diskFileListPtr->fileInfoH,
																			kNotPointer);
	if (diskFileListPtr->fileInfoH == NULL)
																						return (FALSE);

	resultsFileInfoPtr = diskFileListPtr->fileInfoPtr =
			(FileInfoPtr)GetHandlePointer (diskFileListPtr->fileInfoH, kLock, kMoveHi);

	resultsFileStreamPtr = GetFileStreamPointer (resultsFileInfoPtr);

	resultsFilePathPtr =
				(FileStringPtr)GetFilePathPPointerFromFileStream (resultsFileStreamPtr);
		
	diskFileListPtr->refCon = lOutputStorageType;
			
			// Fill some file information parameters.									
							
	resultsFileInfoPtr->numberColumns = numberColumns;
	resultsFileInfoPtr->numberLines = numberLines;
	resultsFileInfoPtr->thematicType = TRUE;
	creator = gCreator;
	numberHeaderBytes = 0;

			// Set up default name.																
			// Get the output file name and volume to write file to.	Make  		
			// the default name the same as the output window name minus 			
			// ".Project" if it is there and add code name for processor at the 	
			// end for ascii version, .gis for ERDAS version, .tif for TIFF, etc.

	if (lOutputStorageType & (kProbFormatCode+kEchoFieldsCode+kEchoClassesCode))
		{
		fileZeroInfoPtr = GetResultsFilePtr (0);
		if (fileZeroInfoPtr != NULL)
			filePathPointer =
					(FileStringPtr)GetFilePathPPointerFromFileInfo (fileZeroInfoPtr);
		
		else	// fileZeroInfoPtr == NULL
			{
					// Try using the file info from the project
			filePathPointer =
					(FileStringPtr)GetFilePathPPointerFromProjectInfo (gProjectInfoPtr);
			
			checkIfDirectoryIsWritableFlag = TRUE;
			
			}	// end "else fileZeroInfoPtr == NULL"
		
		if (filePathPointer != NULL)
			{
			CopyFileStringToFileString (filePathPointer, resultsFilePathPtr, _MAX_PATH);
			
			RemoveCharsAddVersion ((UCharPtr)"\0.project\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.prj\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.gaia\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.gis\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.tif\0", resultsFilePathPtr);
			
			stringIndex = IDS_Project14;
	      if (gProcessorCode == kClassifyProcessor && 
	      												gClassifySpecsPtr->mode == kEchoMode)
				stringIndex = IDS_Project24;
			
			if (MGetString (gTextString, kProjectStrID, stringIndex))
				RemoveCharsAddVersion (gTextString, resultsFilePathPtr);
				
			}	// end "if (inputFileInfoPtr != NULL)"
		
		}	// lOutputStorageType & (kProbFormatCode+... 
		
	if (lOutputStorageType & kClusterMaskCode)
		{
		if (gProjectInfoPtr != NULL)
			{
			resultsFilePathPtr[0] = 0;
			fileZeroInfoPtr = GetResultsFilePtr (0);
			filePathPointer =
					(FileStringPtr)GetFilePathPPointerFromFileInfo (fileZeroInfoPtr);
			
			if (filePathPointer == NULL || filePathPointer[0] == 0)
						// No cluster disk file is being written. Use the project
						// image file name as the base.
				/*
				ConcatPStrings (resultsFilePathPtr,
										gProjectInfoPtr->imageFileName, 
										255);
				*/
				filePathPointer =
					(FileStringPtr)GetFilePathPPointerFromProjectInfo (gProjectInfoPtr);
			
			CopyFileStringToFileString (filePathPointer, resultsFilePathPtr, _MAX_PATH);
													
			RemoveCharsAddVersion ((UCharPtr)"\0_cluster.txt\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.cluster\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.tiff\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.bil\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.bip\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.bsq\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.clu\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.dat\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.ecw\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.fst\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.hdf\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.img\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.lan\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.raw\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.tif\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.jp2\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.png\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.nc\0", resultsFilePathPtr);
				
			}	// end "if (gProjectInfoPtr != NULL)"
		
		}	// end "if (lOutputStorageType & kClusterMaskCode)"
		
	if (resultsFilePathPtr[0] == 0)
		{
		if (gProjectInfoPtr != NULL)
			{
			filePathPointer =
					(FileStringPtr)GetFilePathPPointerFromProjectInfo (gProjectInfoPtr);
			CopyFileStringToFileString (filePathPointer, resultsFilePathPtr, _MAX_PATH);
			
			RemoveCharsAddVersion ((UCharPtr)"\0.Project\0", resultsFilePathPtr);
			RemoveCharsAddVersion ((UCharPtr)"\0.Prj\0", resultsFilePathPtr);
			
			checkIfDirectoryIsWritableFlag = TRUE;
				
			}	// end "if (gProjectInfoPtr != NULL)"
		
		}	// end "if (resultsFilePathPtr[0] == 0)"
		
	if (resultsFilePathPtr[0] == 0)
		{
		filePathPointer =
					(FileStringPtr)GetFilePathPPointerFromFileInfo (gImageFileInfoPtr);
		CopyFileStringToFileString (filePathPointer, resultsFilePathPtr, _MAX_PATH);
		RemoveSuffix (resultsFilePathPtr);
		
		}	// end "if (resultsFilePathPtr[0] == 0)"
	
	formatVersion = 0;
	if (gAreaDescription.diskFileFormat == kErdas74Type)
		{
		formatVersion = kErdas74Type;
		
		if (lOutputStorageType & kClassifyFileCode)           
			ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_cl.gis\0");
		
		else if (lOutputStorageType & kProbFormatCode)
			{                                                                
			ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0Prob.gis\0");
			
			promptStringIndex = IDS_SaveProbabilityAs;	// kFileIOStr34;
			
			}	// end "else if (lOutputStorageType & kProbFormatCode)" 
			
		else if (lOutputStorageType & kEchoFieldsCode)
			{                                               
			ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_Fields.gis\0");
			
			}	// end "else if (lOutputStorageType & kEchoFieldsCode)"
			
		else if (lOutputStorageType & kEchoClassesCode)
			{                                               
			ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_Classes.gis\0");
			
			}	// end "else if (lOutputStorageType & kEchoClassesCode)"
			
		else if (lOutputStorageType & kClusterMaskCode)
			{
			ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_clMask.gis\0");
			
			promptStringIndex = IDS_FileIO97;	// Save Cluster Mask As
			
			}	// end "else if (lOutputStorageType & kClusterMaskCode)"
			
		}	// end "if (gAreaDescription.diskFileFormat == kErdas74Type)" 
		
	else if (gAreaDescription.diskFileFormat == kGAIAType)
		{
		formatVersion = kGAIAType;
		
		if (lOutputStorageType & kClassifyFileCode)                             
			#if defined multispec_mac             
				ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0.gaia\0");
			#endif	// defined multispec_mac   
			                                              
			#if defined multispec_win || defined multispec_wx
				ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0.gai\0");
			#endif	// defined multispec_win || defined multispec_wx
		
		fileStreamPtr = GetFileStreamPointer (resultsFileInfoPtr);
		SetType (fileStreamPtr, kGAIAFileType);
		creator = kGAIACreator;
			
		}	// end "else if (gAreaDescription.diskFileFormat == kGAIAType)"
	
	else if (gAreaDescription.diskFileFormat == kTIFFType)
		{
		formatVersion = kTIFFType;
		
		if (lOutputStorageType & kClassifyFileCode)
			{
			switch (gClassifySpecsPtr->mode)
				{
				case kMaxLikeMode:
					ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_mxcl.tif\0");
					break;
					
				case kMahalanobisMode:
					ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_macl.tif\0");
					break;

				case kFisherMode:
					ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_ficl.tif\0");
					break;

				case kEchoMode:
					ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_echocl.tif\0");
					break;
					
				case kSupportVectorMachineMode:
					ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_svmcl.tif\0");
               break;
					
				case kKNearestNeighborMode:
					ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_knncl.tif\0");
					break;

					
				case kEuclideanMode:
					ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_eucl.tif\0");
					break;
					
				case kCorrelationMode:
					ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_cocl.tif\0");
					break;
					
				case kCEMMode:
					ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_cemcl.tif\0");
					break;
					
				case kParallelPipedMode:
					ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_ppcl.tif\0");
					break;
					
				default:
					ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_cl.tif\0");
				
				}	// end "switch (gClassifySpecsPtr->mode)"
			
			}	// end "if (lOutputStorageType & kClassifyFileCode)"
		
		else if (lOutputStorageType & kProbFormatCode)
			{                                                                
			ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0Prob.tif\0");
			
			promptStringIndex = IDS_SaveProbabilityAs;	// kFileIOStr34;
			
			}	// end "else if (lOutputStorageType & kProbFormatCode)" 
		
		else if (lOutputStorageType & kEchoFieldsCode)
			{                                               
			ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_Fields.tif\0");

			}	// end "else if (lOutputStorageType & kEchoFieldsCode)"
		
		else if (lOutputStorageType & kEchoClassesCode)
			{                                               
			ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_Classes.tif\0");
			
			}	// end "else if (lOutputStorageType & kEchoClassesCode)"
		
		else if (lOutputStorageType & kClusterMaskCode)
			{
			ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)"\0_clMask.tif\0");
               
			promptStringIndex = IDS_FileIO97;	// Save Cluster Mask As
			
			}	// end "else if (lOutputStorageType & kClusterMaskCode)"
	
		}	// end "else if (gAreaDescription.diskFileFormat == kTIFFType)" 
		
	if (formatVersion == 0)
		{
		if (lOutputStorageType & kAsciiFormatCode)
			{
			stringIndex = IDS_Project15;
			
			switch (gProcessorCode)
				{
				case kHistogramProcessor:
					stringIndex = IDS_Project26;
					promptStringIndex = IDS_FileIO63;
					break;
					
				case kClusterProcessor:
					stringIndex = IDS_Project12;
					promptStringIndex = IDS_SaveReportMapAs;
					break;
								
				case kSeparabilityProcessor:
					stringIndex = IDS_Project13;
					promptStringIndex = IDS_SaveSepReportAs;
					break;
								
				case kClassifyProcessor:
					stringIndex = IDS_Project14;
					if (gClassifySpecsPtr->mode == kFisherMode)
						stringIndex = IDS_Project22;
					else if (gClassifySpecsPtr->mode == kEuclideanMode)
						stringIndex = IDS_Project23;
					else if (gClassifySpecsPtr->mode == kEchoMode)
						stringIndex = IDS_Project24;
					else if (gClassifySpecsPtr->mode == kCorrelationMode)
						stringIndex = IDS_Project85;
					else if (gClassifySpecsPtr->mode == kCEMMode)
						stringIndex = IDS_Project86;
					else if (gClassifySpecsPtr->mode == kParallelPipedMode)
						stringIndex = IDS_Project87;
					else if (gClassifySpecsPtr->mode == kMahalanobisMode)
						stringIndex = IDS_Project88;
					promptStringIndex = IDS_SaveClassificationAs;	// kFileIOStr13;
					break;
								
				case kListResultsProcessor:
					stringIndex = IDS_Project15;
					promptStringIndex = IDS_FileIO41;
					break;
								
				case kListDataProcessor:
					stringIndex = IDS_Project16;
					promptStringIndex = IDS_FileIO39;
					break;
								
				case kPrincipalComponentsProcessor:
					stringIndex = IDS_Project17;
					promptStringIndex = IDS_FileIO40;
					break;
					
				}	// end "switch (gProcessorCode)" 
				
			if (MGetString (gTextString, kProjectStrID, stringIndex))
				ConcatFilenameSuffix (resultsFilePathPtr, (StringPtr)gTextString);
								
			}	// end "if (lOutputStorageType & kAsciiFormatCode)" 
			
		}	// end "if (formatVersion == 0)"
	
	if (checkIfDirectoryIsWritableFlag)
					// Verify that the project file is in a writeable directory. If not,
					// change to the working directory.
			CheckIfDirectoryIsWriteable (resultsFileStreamPtr);

	VerifyFileStreamForOpen (resultsFileStreamPtr, NULL);

	if (formatVersion == kErdas74Type || formatVersion == kTIFFType)
		SetType (resultsFileStreamPtr, 'BINA');
	
	if (promptFlag)
		{
		errCode = PutFile (
						resultsFileStreamPtr, numberBytes, promptStringIndex, creator);
				
		if (lOutputStorageType & kClusterMaskCode)
			writeHeaderFlag = FALSE;
		
		}	// end "if (promptFlag)"
				
	else	// !promptFlag 
		{
		SetFSSpecFlag (resultsFileStreamPtr, fSSpecFlag);
		SetParID (resultsFileStreamPtr, parID);

		#if defined multispec_mac
			resultsFileStreamPtr->vRefNum = vRefNum;
		
					// Set parent FSRef
				
			CMFileStream*						fileStreamPtr;
			
			fileStreamPtr = GetFileStreamPointer (gImageFileInfoPtr);
			resultsFileStreamPtr->parentFSRef = fileStreamPtr->parentFSRef;
		#endif	// defined multispec_mac
		
		#if defined multispec_wx
			if (errCode == noErr)
				{
		#endif

				if (!(lOutputStorageType & kEchoFieldsCode))
					errCode = CreateNewFile (resultsFileStreamPtr,
														vRefNum,
														creator, 
														kErrorMessages, 
														kReplaceFlag);
		
				else	// Echo homogeneous fields file not created here
					writeHeaderFlag = FALSE;
      
		#if defined multispec_wx
				}	// if (errCode == noErr)
		#endif
											
		}	// end "else !promptFlag"
	
			// Make sure the output directory variable is set to empty.

	ResetOutputDirectory ();
	
			// Now check if a file name was entered.

	resultsFilePathPtr =
				(FileStringPtr)GetFilePathPPointerFromFileStream (resultsFileStreamPtr);
		
	abortedFlag = (errCode != noErr) ||
										(GetFileStringLength (resultsFilePathPtr) == 0);
	
	if (!abortedFlag && (formatVersion != 0))
		{		
		resultsFileInfoPtr->numberBits = 8;
		resultsFileInfoPtr->numberBytes = 1;
		resultsFileInfoPtr->numberChannels = 1;
		resultsFileInfoPtr->startColumn =  
						gAreaDescription.columnStart + gAreaDescription.startColumn - 1;
		resultsFileInfoPtr->startLine =  
						gAreaDescription.lineStart + gAreaDescription.startLine - 1;
		
				// Count the null class as a class.  The data values will be		
				// '0'.																				
				
		resultsFileInfoPtr->numberClasses = numberClasses + 1;
		if (lOutputStorageType & kEchoFieldsCode)
					// Allow for 255 classes for Echo Fields Output. It will be reset
					// later.
			resultsFileInfoPtr->numberClasses = 255;
			
		resultsFileInfoPtr->format = formatVersion;
		resultsFileInfoPtr->ancillaryInfoformat = formatVersion;
		
		IntermediateFileUpdate (resultsFileInfoPtr);		
			
				// Allow the map projection information handle for the input	
				// file information structure to be available for the output 	
				// file information structure.	
			
		if (formatVersion == kErdas74Type || formatVersion == kTIFFType)										
			UpdateMapProjectionStructure (gImageFileInfoPtr->mapProjectionHandle,
													resultsFileInfoPtr,
													gAreaDescription.startLine, 
													gAreaDescription.startColumn,
													gAreaDescription.lineInterval,
													gAreaDescription.columnInterval,
													FALSE,
													FALSE);
		
		if (writeHeaderFlag)
			{
			if (formatVersion == kErdas74Type)
				{
						// Fill the rest of the header and write it to the file.	
														
				WriteNewErdasHeader (
									resultsFileInfoPtr, (SInt16*)gTextString3, formatVersion);
				
				}	// end "if (formatVersion == kErdas74Type)" 
													
			else if (formatVersion == kGAIAType)
				{
						// Fill the rest of the header and write it to the file.	
								
				gSwapBytesFlag = gBigEndianFlag;
				
				SInt16* channelsPtr = (SInt16*)GetHandlePointer (
																gClassifySpecsPtr->channelsHandle);
						
				abortedFlag = !WriteGAIAHeader (resultsFileInfoPtr,
															NULL, 
															gClassifySpecsPtr->numberChannels, 
															channelsPtr,
															gClassifySpecsPtr->numberClasses,
															(SInt16*)gClassifySpecsPtr->classPtr);
				
				gSwapBytesFlag = FALSE;
				
				}	// end "else if (formatVersion == kGAIAType)" 
				
			else if (formatVersion == kTIFFType)
				{
				SInt32 writePosOff32 = 0;
				
						// Fill the rest of the header and write it to the file.	
					
				abortedFlag = !WriteTIFFImageFile (resultsFileInfoPtr,
															  resultsFileStreamPtr,
															  NULL,
															  NULL,
															  kFromClassification,
															  kClassDisplay,
															  paletteSelection,
															  &writePosOff32,
															  1,
															  1);
					
				numberHeaderBytes = writePosOff32;
					
				}	// end "else if (formatVersion == kTIFFType)" 
			
			if (numberHeaderBytes == 0)
				errCode = MGetMarker (resultsFileStreamPtr,
											 &numberHeaderBytes, 
											 kNoErrorMessages);
										
			if (errCode == noErr)
				resultsFileInfoPtr->numberHeaderBytes = (UInt32)numberHeaderBytes;
			
			}	// end "if (writeHeaderFlag)"
			
		}	// end "if (!abortedFlag && (formatVersion != 0))" 
		
	return (!abortedFlag);
	
}	// end "CreateResultsDiskFile" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CreateResultsDiskFiles
//
//	Software purpose:	The purpose of this routine is to set up the disk
//							file(s) to write the results to. First the number of
//							disk files to be created is determined.  The recommended
//							minimum space needed for the file is calculated.  Then
//							the specified files are opened.
//
//	Parameters in:		lOutputStorageType: Code indicating which type of files should 
//								be set up.
//							numberClasses: Number of classes
//							numberProbabilityClasses: number of probability classes
//
//	Parameters out:				
//
// Value Returned:	True: if result disk files were setup and opened okay.
//							False: if the result disk files could not be set up.
//
// Called By:			ClassifyControl in SClassify.cpp
//							ClusterControl in SCluster.cpp
//							SeparabilityControl in SFeatureSelection.cpp
//							HistogramControl in SHistogram.cpp
//							ListDataControl in SListData.cpp
//							ListResultsControl in SListResults.cpp
//							PrincipalComponentControl in SPrincipalComponents.cpp
//							HistogramStatsControl in SProjectHistogramStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/19/1991
//	Revised By:			Larry L. Biehl			Date: 03/27/2017	

Boolean CreateResultsDiskFiles (
				SInt16								lOutputStorageType, 
				SInt16								numberClasses,
				SInt16								numberProbabilityClasses)

{
	DiskFileListPtr					diskFileListPtr;
	
	SInt32								count,
											parID,
											numberBytes,
											numberColumns,
											numberLines;
											
	SInt16								echoVRefNum,
											numberResultsFiles,
											savedDiskFileFormat;
											
	Boolean								continueFlag,
											directoryLocationSetFlag = FALSE,
											fSSpecFlag = FALSE;
	
	
			// Either the 2, 4 or 8 bit should be set to indicate the format of 	
			// the results file.																	
			
	if (!(lOutputStorageType & (kAsciiFormatCode+kClassifyFileCode+ 
					kProbFormatCode+kEchoFieldsCode+kEchoClassesCode+
								kClusterMaskCode)))
																							return (TRUE);
																					
	if (gResultsFileSummary.numberFiles != 0)
																							return (FALSE);
	
			// Verify that we have some parameters that will be required.
																							
	if (lOutputStorageType & 
					(kClassifyFileCode+kProbFormatCode+kEchoFieldsCode))
		{
	 	if (gProjectInfoPtr == NULL || numberClasses == 0)
																							return (FALSE);
																					
		}	// end "if ((... & (kClassifyFileCode + kProbFormatCode + ..." 
		
			// The probability results file needs to always be at the second		
			// location.																			
			// The intermediate ECHO image files need to be at locations 3 and	
			// 4.																						
	
	continueFlag = TRUE;																					
	numberResultsFiles = 1;
	if (lOutputStorageType & kProbFormatCode)											
		numberResultsFiles++;
		
	if (lOutputStorageType & (kEchoFieldsCode+kEchoClassesCode))
		numberResultsFiles = 4;
		
	if (lOutputStorageType & kClusterMaskCode)										
		numberResultsFiles = 5;	
		
			// Get the memory for the disk file information list.						
			
	gResultsFileSummary.diskFileListH = MNewHandle (
								(SInt32)numberResultsFiles * sizeof (DiskFileList));
	if (gResultsFileSummary.diskFileListH == NULL)
																							return (FALSE);
																				
	diskFileListPtr = gResultsFileSummary.diskFileListPtr =
				(DiskFileListPtr)GetHandlePointer (
											gResultsFileSummary.diskFileListH, kLock, kMoveHi);

	gResultsFileSummary.numberFiles = 0;
	
			// Initialize list information.													
			
	for (count=0; count<numberResultsFiles; count++)
		{
		diskFileListPtr->fileInfoH = NULL;
		diskFileListPtr->fileInfoPtr = NULL;
		diskFileListPtr++;
		
		}	// end "for (count=0; count<numberResultsFiles; count++)" 
		
			// Now determine the number of lines and columns in the file.			
			
	numberColumns = (gAreaDescription.columnEnd - gAreaDescription.columnStart + 
						gAreaDescription.columnInterval)/gAreaDescription.columnInterval;
	
	numberLines = (gAreaDescription.lineEnd - gAreaDescription.lineStart + 
						gAreaDescription.lineInterval)/gAreaDescription.lineInterval;
		
			// Now determine the space required for the file.							
							
	numberBytes = (SInt32)numberLines * numberColumns;
	
			// Allow for GAIA format.															
	
	if (gAreaDescription.diskFileFormat == kGAIAType)
		numberBytes *= 2;
	
			// Allow for header.																	
	
	if (gAreaDescription.diskFileFormat == kGAIAType)
		{
		if (gClassifySpecsPtr != NULL)
			{
			count = gClassifySpecsPtr->numberChannels;
			numberBytes += 1709 + 102*count;
			numberBytes += 198 * numberClasses;
			numberBytes += 90 * ((numberClasses+6)/7);
			numberBytes += 90 * numberClasses*((count+4)/5);
			numberBytes += 90 * numberClasses*((count*(count+1)/2 + 4)/5);
			numberBytes += 4 * numberClasses*(count*(count+1)/2);
			numberBytes += 200 * numberClasses;
			
			}	// end "if (gClassifySpecsPtr != NULL)" 
		
		}	// end "if (gAreaDescription.diskFileFormat == kGAIAType)" 
		
	else	// gAreaDescription.diskFileFormat != kGAIAType 
		numberBytes += 200;
				
			// Add in space for ERDAS trailer file if there will be one.	 		
											
	if (lOutputStorageType & (kClassifyFileCode+kProbFormatCode+
											kEchoFieldsCode+kEchoClassesCode+kClusterMaskCode))
		{
		numberBytes += 7*128;
		numberBytes += 9*128;
		
		if (lOutputStorageType & kClassifyFileCode)
			numberBytes += ((numberClasses+4)/4) * 128;
		
		if (lOutputStorageType & kProbFormatCode)
			numberBytes += ((numberProbabilityClasses+4)/4) * 128;
			
		numberBytes += 100;
		
		}	// end "if (... & (kClassifyFileCode+kProbFormatCode+..." 
		
	numberBytes *= numberResultsFiles;
	
			// Allow for possibility of 2 byte output data for ECHO 					
			// homogeneous field output file.												
			
	if (lOutputStorageType & (kEchoFieldsCode+kEchoClassesCode))
		{
		numberBytes += (SInt32)numberLines * numberColumns + 100;
		numberBytes += ((512+4)/4) * 128;
		
		}	// end "if (lOutputStorageType & kEchoFieldsCode)" 
		
			// Now create the results file for ascii or gis thematic image classification
			// case.									
	
	echoVRefNum = GetVolumeReferenceNumber (gImageFileInfoPtr);	
	parID = GetParID (gImageFileInfoPtr);
	fSSpecFlag = GetFSSpecFlag (gImageFileInfoPtr);	
	
	diskFileListPtr = gResultsFileSummary.diskFileListPtr;
	if (lOutputStorageType & (kAsciiFormatCode+kClassifyFileCode))
		{		
		SInt16 classifyColors = kDefaultColors;
		if (gProjectInfoPtr != NULL)
			classifyColors = gProjectInfoPtr->imagePalettePopupMenuSelection;
			
				// The classification map generated by the cluster processor must be in
				// ascii format.
		
		savedDiskFileFormat = gAreaDescription.diskFileFormat;
		if (gProcessorCode == kClusterProcessor)
			gAreaDescription.diskFileFormat = kMultiSpecClassificationType;
		
		continueFlag = CreateResultsDiskFile (numberLines,
															numberColumns,
															numberBytes,
															(SInt16)(lOutputStorageType & 0x0006), 
															numberClasses,
															classifyColors,
															diskFileListPtr,
															TRUE,
															0,
															0,
															FALSE);
			
		gAreaDescription.diskFileFormat = savedDiskFileFormat;
					
		if (continueFlag && diskFileListPtr->fileInfoH != NULL)
			{
			gResultsFileSummary.numberFiles = 1;
			
					// Save the directory location so that any echo specific files will
					// be saved to the same location. Volume reference number is used for
					// older Mac OS's; ParID is used for newer Mac OS's
			
			directoryLocationSetFlag = TRUE;		
			echoVRefNum = GetVolumeReferenceNumber (diskFileListPtr->fileInfoH);
			parID = GetParID (diskFileListPtr->fileInfoH);
			fSSpecFlag = GetFSSpecFlag (diskFileListPtr->fileInfoH);
			
			}	// end "if (continueFlag && diskFileListPtr->fileInfoH != NULL)"
		
		}	// end "if (lOutputStorageType & 0x0006)" 
		
			// Now create the results file type kProbFormatCode.						
	
	diskFileListPtr++;	
	if (continueFlag && (lOutputStorageType & kProbFormatCode))
		{
		if (gAreaDescription.diskFileFormat != kErdas74Type && 
													gAreaDescription.diskFileFormat != kTIFFType)
			gAreaDescription.diskFileFormat = kTIFFType;
		
		SInt16 probabilityColors = kProbablilityColors;
		if (gClassifySpecsPtr->mode == kCEMMode)
			probabilityColors = kProbablilityColors2;
		
		continueFlag = CreateResultsDiskFile (numberLines,
															numberColumns, 
															numberBytes,  
															kProbFormatCode, 
															numberProbabilityClasses,
															probabilityColors,
															diskFileListPtr,
															TRUE,
															0,
															0,
															FALSE);
		
		if (diskFileListPtr->fileInfoH != NULL)
			{
			gResultsFileSummary.numberFiles = 2;
			
			if (!directoryLocationSetFlag)
				{		
						// Save the directory location so that any echo specific files will
						// be saved to the same location. Volume reference number is used
						// for older Mac OS's; ParID is used for newer Mac OS's
					
				directoryLocationSetFlag = TRUE;	
				echoVRefNum = GetVolumeReferenceNumber (diskFileListPtr->fileInfoH);
				parID = GetParID (diskFileListPtr->fileInfoH);
				fSSpecFlag = GetFSSpecFlag (diskFileListPtr->fileInfoH);
				
				}	// end "if (!directoryLocationSetFlag)"
			
			}	// end "if (diskFileListPtr->fileInfoH != NULL)"
		
		}	// end "if (... & kProbFormatCode))"
	
	if (continueFlag)
		{
				// Now create the results file for the ECHO homogeous field file.
		
		diskFileListPtr++;
		if (lOutputStorageType & kEchoFieldsCode)
			{
					// For now this must be ERDAS gis type to be able to handle possibility
					// for more than 255 classes. This is not true any more?? (03/27/2017)
			
			savedDiskFileFormat = gAreaDescription.diskFileFormat;
			if (gAreaDescription.diskFileFormat != kErdas74Type &&
														gAreaDescription.diskFileFormat != kTIFFType)
				gAreaDescription.diskFileFormat = kTIFFType;
			
			continueFlag = CreateResultsDiskFile (numberLines,
																numberColumns,
																numberBytes,
																kEchoFieldsCode,
																255,
																kDefaultColors,
																diskFileListPtr,
																FALSE,
																echoVRefNum,
																parID,
																fSSpecFlag);
			
			if (diskFileListPtr->fileInfoH)
				gResultsFileSummary.numberFiles = 3;
			
			gAreaDescription.diskFileFormat = savedDiskFileFormat;
			
			}	// end "if (lOutputStorageType & kEchoFieldsCode)"
		
		}	// end "if (continueFlag)"
	
	if (continueFlag)
		{
				// Now create the results file for the ECHO classified homogeous
				// field file.
		
		diskFileListPtr++;
		if (lOutputStorageType & kEchoClassesCode)
			{
			if (gAreaDescription.diskFileFormat != kErdas74Type &&
														gAreaDescription.diskFileFormat != kTIFFType)
				gAreaDescription.diskFileFormat = kTIFFType;
			
			continueFlag = CreateResultsDiskFile (
													numberLines,
													numberColumns,
													numberBytes,
													kEchoClassesCode,
													numberClasses,
													gProjectInfoPtr->imagePalettePopupMenuSelection,
													diskFileListPtr,
													FALSE,
													echoVRefNum,
													parID,
													fSSpecFlag);
			
			if (diskFileListPtr->fileInfoH != NULL)
				gResultsFileSummary.numberFiles = 4;
			
			}	// end "if (lOutputStorageType & kEchoClassesCode)"
		
		}	// end "if (continueFlag)"
	
	if (continueFlag)
		{
				// Now create the results file for the cluster mask image file.
		
		diskFileListPtr++;
		if (lOutputStorageType & kClusterMaskCode)
			{
			continueFlag = CreateResultsDiskFile (numberLines,
																numberColumns,
																numberBytes,
																kClusterMaskCode,
																numberClasses,
																kDefaultColors,
																diskFileListPtr,
																TRUE,
																0,
																0,
																FALSE);
			
			if (diskFileListPtr->fileInfoH != NULL)
				gResultsFileSummary.numberFiles = 5;
			
			}	// end "if (lOutputStorageType & kClusterMaskCode)"
		
		}	// end "if (continueFlag)"
	                                          
	return (continueFlag);
																					
}	// end "CreateResultsDiskFiles"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean DetermineIfChannelsInOrder
//
//	Software purpose:	The purpose of this routine is to determine if the
//							channels in the input list are in order from lowest to highest.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/17/1999
//	Revised By:			Larry L. Biehl			Date: 02/17/1999			

Boolean DetermineIfChannelsInOrder (
				UInt16*								channelListPtr, 
				UInt32								numberChannels)

													
{
	UInt32								index;
	
	Boolean								returnFlag;


	returnFlag = FALSE;
	
	if (channelListPtr != NULL && numberChannels > 0)
		{
		returnFlag = TRUE;
	
		for (index=1; index<numberChannels; index++)
			{
			if (channelListPtr[index] < channelListPtr[index-1])
				{
				returnFlag = FALSE;
				break;
				
				}	// end "if (channelListPtr[index] < channelListPtr[index-1])" 
			
			}	// end "for (index=1; index<numberChannels; index++)" 
				
		}	// end "if (channelListPtr != NULL && numberChannels > 0)" 
		
	return (returnFlag);	
		
}	// end "DetermineIfChannelsInOrder"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DetermineIfContinuousChannels
//
//	Software purpose:	The purpose of this routine is to determine if the
//							channels in the input list are in continuous order.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 11/02/1993
//	Revised By:			Larry L. Biehl			Date: 05/03/1995			

Boolean DetermineIfContinuousChannels (
				UInt16*								channelListPtr, 
				UInt32								numberChannels)

													
{
	UInt32								index;
	
	Boolean								returnFlag;


	returnFlag = FALSE;
	
	if (channelListPtr != NULL && numberChannels > 0)
		{
		returnFlag = TRUE;
	
		for (index=1; index<numberChannels; index++)
			{
			if (channelListPtr[index] != channelListPtr[index-1] + 1)
				{
				returnFlag = FALSE;
				break;
				
				}	// end "if (channel != *channelListPtr)" 
			
			}	// end "for (index=1; index<numberChannels; index++)" 
				
		}	// end "if (channelListPtr != NULL && numberChannels > 0)" 
		
	return (returnFlag);	
		
}	// end "DetermineIfContinuousChannels"

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DrawSideBySideTitles
//
//	Software purpose:	The purpose of this routine is to draw the titles
//							for the side-by-side channel display.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			CopyOffScreenImage in MMultiSpec.c
//							DoImageWUpdateEvent in MMultiSpec.c
//							CopyPrintOffscreenImage in MPrint.c
//
//	Coded By:			Larry L. Biehl			Date: ??/??/1991
//	Revised By:			Larry L. Biehl			Date: 03/07/2020

void	DrawSideBySideTitles (
				wxDC*									titleBarDCPtr,
				Handle								windowInfoHandle, 
				WindowPtr							windowPtr, 
				Rect*									updateRectPtr,
				SInt16								windowCode)

{                     
			// Declare local variables and structures	
	
	double								characterWidth,
											magnification;					
	
	DisplaySpecsPtr					displaySpecsPtr;
	ChannelDescriptionPtr			channelDescriptionPtr;
	
	FileInfoPtr							fileInfoPtr,
											localFileInfoPtr;
	
	LayerInfoPtr						layerInfoPtr; 
	SInt16*								channels;
	WindowInfoPtr						windowInfoPtr;
	
	Handle								channelsHandle,
											displaySpecsHandle,
											fileInfoHandle,
											layerInfoHandle;
	
	SInt32								columnOrigin,
											descriptionStart,
											drawCenter,
											drawInterval,
											//drawStart,
											imageRight,
											index;
	
	SInt16								channelNum,
											fileChannelNumber,
											fileInfoIndex,
											numberChannels,
											numberDescriptionCharacters,
											numberImageFiles,
											textWidth,
											channelNumberWidth,
											numChars,
											width;
															
	#if defined multispec_mac 			
		Pattern								black;			
		SInt16								titleYDrawStart;
	#endif	// defined multispec_mac
											
	Boolean								includeDescriptionsFlag;
	
	SignedByte							handleStatus,
											handleStatus2,
											handleStatus3; 
											
	#if defined multispec_wx
		if (updateRectPtr->top > 0)
																									return;
	#endif
	
	#if defined multispec_win
		CSize 								size;

		USES_CONVERSION;

		if (windowPtr == NULL)
																									return;
	#endif // defined multispec_win 
	
			// Set the global variables needed to convert to window units.										
						
	SetChannelWindowVariables (windowCode, windowInfoHandle, kNotCoreGraphics);
	
			// Initialize local variables.
			
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
			
	layerInfoHandle = GetLayerInfoHandle (windowInfoPtr);
	displaySpecsHandle = GetDisplaySpecsHandle (windowInfoPtr);
	fileInfoHandle = GetFileInfoHandle (windowInfoPtr);
	
	#if defined multispec_mac  
		titleYDrawStart = 12;
		if (windowCode == kToImageWindow)
			titleYDrawStart += windowInfoPtr->coordinateHeight;
	#endif // defined multispec_mac 
	                                                  
	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);
							
	channelsHandle = displaySpecsPtr->channelsHandle;
	numberChannels = displaySpecsPtr->numberChannels;
	magnification = displaySpecsPtr->magnification;
	
	drawInterval = (SInt32)(magnification * 
							displaySpecsPtr->imageDimensions[kHorizontal] / numberChannels);
	if (windowCode == kToClipboardWindow || windowCode == kToPrintWindow)
		drawInterval = (SInt32)((updateRectPtr->right - updateRectPtr->left) / 
															(gSideBySideChannels-gStartChannel));
								
	//columnOrigin = (SInt32)(magnification * displaySpecsPtr->origin[kHorizontal]);
	columnOrigin = gStartChannelWindow;
	//columnOrigin = 0;
	
	channelDescriptionPtr = NULL;
	fileInfoIndex = -1;	
	includeDescriptionsFlag = FALSE;
	numberDescriptionCharacters = windowInfoPtr->maxNumberDescriptionCharacters;
	numberImageFiles = windowInfoPtr->numberImageFiles; 
	
	characterWidth = 7.0;
	
			// Get a pointer to the channel list.											
			                                     
	channels = (SInt16*)GetHandleStatusAndPointer (channelsHandle, &handleStatus);
	
			// Get pointer to channel layer information.									
			                                    
	layerInfoPtr = (LayerInfoPtr)GetHandleStatusAndPointer (
																layerInfoHandle, &handleStatus2);
	
			// Get pointer to file information and determine if any channel		
			// descriptions exist.																
			
	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
																fileInfoHandle, &handleStatus3);
	localFileInfoPtr = fileInfoPtr;
	for (index=0; index<numberImageFiles; index++)
		{
		if (fileInfoPtr[index].channelDescriptionH != NULL)
			includeDescriptionsFlag = TRUE;
			
		}	// end "for (index=0; index<numberImageFiles; index++)" 
		       
	#if defined multispec_mac                
		GrafPtr						savedPort;
		PenState						savedPenState;
		  
				// Save the current port and make active port the project image		
				//	window.																				
				
		GetPort (&savedPort);
		
		if (windowPtr != NULL)
			SetPortWindowPort (windowPtr);
			
				// Set the pen state for outlining the fields.								
			
		GetPenState (&savedPenState);
		PenPat (GetQDGlobalsBlack (&black));
		PenMode (patCopy);
		PenSize (1,1);
		
				// Set the text size and the text transfer mode for the window.		
				
		TextSize (9);
		TextMode (srcCopy);
	#endif // defined multispec_mac 
			
	#if defined multispec_win            
		LOGFONT				logfont; 
			   
		::GetObject (GetStockObject (SYSTEM_FIXED_FONT), 
							sizeof (LOGFONT), 
							&logfont); 
							
		//logfont.lfHeight = 10;
		logfont.lfHeight = ((CMultiSpecApp*)AfxGetApp())->getFontHeightForDefaultDC (10.0);
		if (windowPtr->m_pDC->IsPrinting ())          
			logfont.lfHeight = (SInt16)
							(logfont.lfHeight * gActiveImageViewCPtr->m_printerTextScaling);   

		logfont.lfWidth = 0;    

		logfont.lfQuality = DEFAULT_QUALITY; 

		logfont.lfFaceName[0] = 'C';
		logfont.lfFaceName[1] = 'o'; 
		logfont.lfFaceName[2] = 'u';
		logfont.lfFaceName[3] = 'r';
		logfont.lfFaceName[4] = 'i';
		logfont.lfFaceName[5] = 'e';
		logfont.lfFaceName[6] = 'r';
		logfont.lfFaceName[7] = 0x00;                                
            
      		// Set font to be used.
			   
	   CFont font;                                                            
		if (!font.CreateFontIndirect (&logfont)) 
																									return;
			                                                   
		CFont* pOldFont = windowPtr->m_pDC->SelectObject (&font); 
				
		windowPtr->m_pDC->SetBkMode (OPAQUE);
		windowPtr->m_pDC->SetTextColor (RGB (0, 0, 0));
	
		if (windowPtr->m_printCopyModeFlag)
			{
					// Get the scaling for printing. The pixels
					// per inch may be different between the screen display and
					// the paper. Also get the right edge to be printed on the
					// paper.
				                                                         
			characterWidth *= windowPtr->m_printerTextScaling;
			columnOrigin = 0;
			updateRectPtr->right = (SInt16)(drawInterval * 
															(gSideBySideChannels-gStartChannel));

			}	// end "if (windowPtr->m_pDC->IsPrinting ())" 
	#endif	// defined multispec_win 

	#if defined multispec_wx            
		if (windowPtr->m_printCopyModeFlag)
			{
					// Get the scaling for printing. The pixels
					// per inch may be different between the screen display and
					// the paper. Also get the right edge to be printed on the
					// paper.
				                                                         
			characterWidth *= windowPtr->m_printerTextScaling;
			columnOrigin = 0;
			updateRectPtr->right = (int)(drawInterval *
															(gSideBySideChannels-gStartChannel));

			}	// end "if (windowPtr->m_pDC->IsPrinting ())" 
	#endif	// defined multispec_wx
	
			// Estimate width of longest string.											
			//  Get the width of the largest channel number.

	NumToString ((UInt32)(channels[gSideBySideChannels-1]+1), (UCharPtr)gTextString); 
	channelNumberWidth =  gTextString[0];
	
	numChars = channelNumberWidth;
	descriptionStart = channelNumberWidth + 2;
	
	if (includeDescriptionsFlag)
		numChars += numberDescriptionCharacters + 1;
		
			//  Now take into account the approximate pixel width of the			
			//  characters.																		
			
	textWidth = (SInt16)(numChars * characterWidth + .5);
	
			// Check if the drawinterval between the channel centers is less than the 
			// character width.  If so try just the descriptions in the titles.																		
			// If that doesn't work, then just do the channel numbers.				
			
	if (textWidth > drawInterval && includeDescriptionsFlag)
		{
		numChars = numberDescriptionCharacters;
		descriptionStart = 1;
			                       
		textWidth = (SInt16)(numChars * characterWidth + .5); 
		
		}	// end "if (textWidth > drawInterval && ...)" 
			
	if (textWidth > drawInterval)
		{
		numChars = gTextString[0];
		includeDescriptionsFlag = FALSE;
		descriptionStart = channelNumberWidth + 2; 
		
		textWidth = (SInt16)(numChars * characterWidth + .5);
		
		}	// end "if (textWidth > drawInterval)" 
		
	gTextString[0] = (UInt8)numChars;
	
			// Get the draw start location and the draw interval.						
			
	drawCenter = drawInterval/2 + columnOrigin;
	#if defined multispec_win || defined multispec_wx
		drawCenter += gStartChannel*drawInterval;
	#endif	// defined multispec_win || defined multispec_wx
		
	imageRight = columnOrigin + drawInterval;
	textWidth /= 2;
	
			// Write channel numbers.															
	#if defined multispec_wx
		titleBarDCPtr->SetMapMode (wxMM_TEXT);
		titleBarDCPtr->SetUserScale (1, 1);
	
		#if defined multispec_wxlin
			updateRectPtr->top += 2;
		#endif
	#endif
	
	for (index=gStartChannel; index<gSideBySideChannels; index++)
		{
				// Get pointer to channel discriptions if they exist.					
				
		if (includeDescriptionsFlag && 
							fileInfoIndex != (SInt16)layerInfoPtr[index+1].fileInfoIndex)
			{
			if (fileInfoIndex != -1)	
				CheckAndUnlockHandle (localFileInfoPtr->channelDescriptionH);
				
			fileInfoIndex = layerInfoPtr[index+1].fileInfoIndex;
			localFileInfoPtr = &fileInfoPtr[fileInfoIndex];

			channelDescriptionPtr = NULL;
			if (localFileInfoPtr->channelDescriptionH != NULL)
				{                                      
				channelDescriptionPtr = (ChannelDescriptionPtr)GetHandlePointer (
												localFileInfoPtr->channelDescriptionH, kLock);
				gTextString[0] = (UInt8)numChars;
				
				}	// end "if (localFileInfoPtr->channelDescriptionH != NULL" 
				
			else	// localFileInfoPtr->channelDescriptionH == NULL 
				gTextString[0] = channelNumberWidth + 1;
				
			}	// end "if (includeDescriptionsFlag && ..." 
	
		//if (drawStart + textWidth > 0)
		if (imageRight >= 0)
			{
			channelNum = channels[index] + 1;
			
			if (descriptionStart > 1)
				sprintf ((char*)&gTextString[1], "%*hd", channelNumberWidth, channelNum);
				
			if (channelDescriptionPtr != NULL)
				{
				fileChannelNumber = layerInfoPtr[channelNum].fileChannelNumber;
				gTextString[channelNumberWidth+1] = ':';
				BlockMoveData (
							&channelDescriptionPtr[fileChannelNumber-1],
							&gTextString[descriptionStart], numberDescriptionCharacters);
            gTextString[numberDescriptionCharacters+descriptionStart] = 0;
														
				}	// end "if (channelDescriptionPtr != NULL)"
				
			width = TextWidth ((UCharPtr)gTextString, 1, gTextString[0])/2; 
		
			#if defined multispec_mac
				MoveTo (drawCenter-width, titleYDrawStart);
				DrawString (gTextString);
			#endif // defined multispec_mac 
		
			#if defined multispec_win  
				windowPtr->m_pDC->SetTextAlign (TA_NOUPDATECP); 
				/*
				CSize size = windowPtr->m_pDC->GetTextExtent ((char*)&gTextString[1],
																				gTextString[0]);
				size.cx /= 2;
				*/
				windowPtr->m_pDC->TextOut ((int)(drawCenter-width-columnOrigin), 
													(int)updateRectPtr->top, 
													(LPCTSTR)A2T((char*)&gTextString[1]), 
													(SInt16)gTextString[0]);
			#endif // defined multispec_win
         
         #if defined multispec_wx
            wxString image_label ((char*)&gTextString[1], wxConvUTF8);
            wxSize label_size;
            label_size = titleBarDCPtr->GetTextExtent (image_label);
			
            width = (label_size.GetWidth ())/2;
            titleBarDCPtr->DrawText (image_label,
												(int)(drawCenter-width-columnOrigin),
												updateRectPtr->top);
         #endif
			
			}	// end "if (imageRight >= 0)" 
		
		#ifndef multispec_wx
			if (imageRight > updateRectPtr->right)
				break;
		#endif
			
		drawCenter += drawInterval;
		imageRight += drawInterval;
		
		}	// end "for (channel=0;..." 	
		                  
	#if defined multispec_mac
		SetPenState (&savedPenState); 
		SetPort (savedPort);
	#endif // defined multispec_mac 
			
	#if defined multispec_win                        
		windowPtr->m_pDC->SelectObject (pOldFont);
		windowPtr->m_pDC->SetBkMode (OPAQUE);       
	#endif	// defined multispec_win     
	/*
   #if defined multispec_wx
      wxRect refreshRect (0, 0, imageRight, updateRectPtr->bottom);
      windowPtr->m_Canvas->RefreshRect (refreshRect, true);
   #endif
	*/
	if (localFileInfoPtr->channelDescriptionH)
		CheckAndUnlockHandle (localFileInfoPtr->channelDescriptionH);
		
	MHSetState (channelsHandle, handleStatus);
	MHSetState (layerInfoHandle, handleStatus2);
	MHSetState (fileInfoHandle, handleStatus3);

}	// end "DrawSideBySideTitles" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void FormatHistogramSummaryString
//
//	Software purpose:	The purpose of this is to force the 'Histogram' code
//							resource to be loaded into memory.  It is called by
//							DisplayControl in the 'Display' code resource.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/03/2006
//	Revised By:			Larry L. Biehl			Date: 03/21/2006

SInt16 FormatHistogramSummaryString (
				char*									stringPtr,
				double								value,
				UInt16								numberEDecimalDigits,
				UInt16								numberFDecimalDigits,
				Boolean								forceEFormatFlag)

{
	double								absValue;
	
	SInt16								length;
	
			
	absValue = fabs (value);
	if (!forceEFormatFlag && 
				absValue >= kMinValueToListWith_f && 
							absValue <= kMaxValueToListWith_f)
		{
		length = sprintf (stringPtr, 
								"%.*f",
								numberFDecimalDigits,
								value);
			
		}	// end "if (absValue >= kMinValueToListWith_f && ..."
		
	else	// forceEFormatFlag || absValue < kMinValueToListWith_f || ...
		{
		length = sprintf (stringPtr, 
								"%.*E",
								numberEDecimalDigits,
								value);
			
		}	// end "else fabs (value) > kMaxValueToListWith_f"
		
	return (length);
	
}	// end "FormatHistogramSummaryString"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		GetActiveImageClipRectangle
//
//	Software purpose:	This routine takes an input rectangle for input
//							and returns the same rectangle with the width of
//							the scroll bars and the legend width subtracted off.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/10/1995
//	Revised By:			Larry L. Biehl			Date: 11/12/1995			

void GetActiveImageClipRectangle (
				SInt16								areaCode,
				Rect*									outRectPtr)

{ 
	#if defined multispec_mac          
  		if (gActiveImageWindow != NULL)
			GetWindowClipRectangle (gActiveImageWindow, areaCode, outRectPtr);
	#endif // defined multispec_mac

	#if defined multispec_win 
   	if (gActiveImageViewCPtr != NULL)
			GetWindowClipRectangle (gActiveImageViewCPtr, 
											areaCode,
											outRectPtr);
	#endif // defined multispec_win
	
   #if defined multispec_wx
		if (gActiveImageViewCPtr != NULL)
			GetWindowClipRectangle (gActiveImageViewCPtr,
											areaCode,
											outRectPtr);
   #endif // defined multispec_wx

}	// end "GetActiveImageClipRectangle" 


	
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DetermineHistogramBinWidth
//
//	Software purpose:	This routine determines the bin width to be used based on the
//							number of histogram bins allowed and the minimum & maximum values.
//							The bin width will be adjusted to be a "nice" value and the
//							minimum bin value will be adjust so that a bin will be centered
//							on 0 (if it would be in the range of values).
//
//	Parameters in:		Pointer to rectangle			
//
//	Parameters out:	None				
//
// Value Returned:	None	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/28/2006
//	Revised By:			Larry L. Biehl			Date: 06/14/2006

double DetermineHistogramBinWidth (
				double								inMinValue,
				double								inMaxValue,
				double*								outMinValuePtr,
				double*								outMaxValuePtr,
				UInt32								maxNumberHistogramBins)

{	
	double								binWidth,
											binWidthFactor,
											maxBinValue,
											minBinValue,
											range;
											
	SInt32								binWidthExponent;
	
	UInt32								finalNumberHistogramBins;
		
		
	range = inMaxValue - inMinValue;
	if (range <= 0)
		range = 1;
			
	binWidth = range/(double)(maxNumberHistogramBins-3);
	binWidthFactor = ConvertToScientificFormat (binWidth, &binWidthExponent);
	binWidth = floor (binWidthFactor);
	binWidth = MIN (5, binWidth);
	binWidth *= pow ((double)10, (double)binWidthExponent);
	
			// Now adjust the min/max to fit with the bin width.
			
	minBinValue = SIGN (floor ((fabs (inMinValue) + binWidth/2)/binWidth),
										inMinValue);
	minBinValue = minBinValue*binWidth - binWidth/2;
			
	maxBinValue = SIGN (floor ((fabs (inMaxValue) + binWidth/2)/binWidth),
										inMaxValue);
	maxBinValue = maxBinValue*binWidth + binWidth/2;
	
	finalNumberHistogramBins = (UInt32)((maxBinValue - minBinValue)/binWidth + 2);
	
	if (finalNumberHistogramBins > maxNumberHistogramBins)
		{
		binWidthFactor = ceil ((double)(finalNumberHistogramBins-2)/
																(maxNumberHistogramBins - 2));
		binWidth *= binWidthFactor;
		
		finalNumberHistogramBins -= 2;
		finalNumberHistogramBins = (UInt32)(finalNumberHistogramBins/binWidthFactor);
		finalNumberHistogramBins += 2;
		
		}	// end "if (finalNumberHistogramBins > maxNumberHistogramBins)"
	
	*outMinValuePtr = minBinValue;
	*outMaxValuePtr = maxBinValue;
	
	return (binWidth);
	
}	// end "DetermineHistogramBinWidth" 


	
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double GetBinIndexForDataValue
//
//	Software purpose:	This routine determines the data value for the input index into
//							the medianArray depending upon the type of array.
//
//	Parameters in:					
//
//	Parameters out:	None				
//
// Value Returned:	Data value	
//
// Called By:			GetThematicTypeMinMaxIndices in SDisplayMultispectral.cpp
//							UpdateMinMaxValueIndices in SDisplayMultispectral.cpp
//							GetClippedMinMaxValueIndices in SHistogram.cpp
//							ListHistogramValues in SHistogram.cpp
//							ListHistogramValuesInLines in SHistogram.cpp
//							ListHistogramValuesInColumns in SHistogram.cpp
//							ConvertDataValueToBinValue in SHistogram.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/22/2005
//	Revised By:			Larry L. Biehl			Date: 12/08/2011

UInt32 GetBinIndexForDataValue (
				double								dataValue,
				HistogramSummaryPtr				histogramSummaryPtr)

{
	SInt32								binIndex,
											binType;
											
	
	binType = histogramSummaryPtr->binType;
		
	if (binType == kDataValueIsBinIndex)
		binIndex = (SInt32)dataValue;
	
	else if (binType == kBinWidthOfOne)
		binIndex = (SInt32)(dataValue - histogramSummaryPtr->minNonSatValue) + 1;
		
	else	// binType != kBinWidthOfOne
		binIndex =
			(SInt32)((dataValue - histogramSummaryPtr->minNonSatValue) * 
														histogramSummaryPtr->binFactor) + 1;
			
	binIndex = MAX (0, binIndex);
	binIndex = MIN (binIndex, (SInt32)histogramSummaryPtr->numberBins-1);
		
	return ((UInt32)binIndex);
		
}	// end "GetBinIndexForDataValue" 


	
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetClipRectangle
//
//	Software purpose:	This routine gets the bounding rectangle for the clipped
//							region.
//
//	Parameters in:		Pointer to rectangle			
//
//	Parameters out:	None				
//
// Value Returned:	None	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/23/1996
//	Revised By:			Larry L. Biehl			Date: 01/23/1996

void GetClipRectangle (
				Rect*									clipRectanglePtr)

{			
	#if defined multispec_mac										
		RgnHandle clipRegion = NewRgn ();
		if (clipRegion != NULL)
			{
			GetClip (clipRegion);
			GetRegionBounds (clipRegion, clipRectanglePtr);
		
			DisposeRgn (clipRegion);
			
			}	// end "if (clipRegion != NULL)
	#endif	// defined multispec_mac
	
	#if defined multispec_win	
			
	#endif	// defined multispec_win
		
}	// end "GetClipRectangle"


	
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetCommonArea
//
//	Software purpose:	The purpose of this routine is to compare the requested image
//							area 1 to the available image area 2 and return the
//							areas of each that overlap. The image 1 and image 2 start lines
//							and columns are taken into account as are the requested image 
//							line and column intervals.
//
//	Parameters in:		Pointer to rectangle			
//
//	Parameters out:	None				
//
// Value Returned:	None	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/19/1999
//	Revised By:			Larry L. Biehl			Date: 07/19/1999

Boolean GetCommonArea (
				SInt32								image1StartLine,
				SInt32								image1StartColumn,
				UInt32								image1LineInterval, 
				UInt32								image1ColumnInterval, 
				UInt32*								image1LineStartPtr, 
				UInt32*								image1LineEndPtr, 
				UInt32*								image1ColumnStartPtr, 
				UInt32*								image1ColumnEndPtr,
				SInt32								image2StartLine,
				SInt32								image2StartColumn, 
				UInt32*								image2LineStartPtr, 
				UInt32*								image2ColumnStartPtr,
				UInt32								image2NumberLines, 
				UInt32								image2NumberColumns)

{		
	ldiv_t								offset;
	
	SInt32								image1LineStart, 
											image1LineEnd, 
											image1ColumnStart, 
											image1ColumnEnd, 
											initialImage1ColumnStart,
											initialImage1LineStart,
											image2ColumnEnd,
											image2ColumnStart,
											image2LineEnd,
											image2LineStart;
												
	
			// Adjust image area 2 to reflect the same area as that relative to
			// the image area 1 in case the two image areas start at different
			// locations.
			
	image2LineStart = 1 +  image2StartLine - image1StartLine;
	image2LineEnd = image2NumberLines + image2StartLine - image1StartLine;
			
	image2ColumnStart = 1 + image2StartColumn - image1StartColumn;
	image2ColumnEnd = image2NumberColumns + image2StartColumn - image1StartColumn;
	
			// Save the requested start line and column.
			
	initialImage1LineStart = (SInt32)*image1LineStartPtr;
	initialImage1ColumnStart = (SInt32)*image1ColumnStartPtr;
	
	image1LineStart = (SInt32)*image1LineStartPtr;
	image1LineEnd = (SInt32)*image1LineEndPtr;
	image1ColumnStart = (SInt32)*image1ColumnStartPtr;
	image1ColumnEnd = (SInt32)*image1ColumnEndPtr;
	
			// Adjust the selected image area 1 to represent the same area as
			// image area 2 that is available.

	image1LineStart = MAX (image1LineStart, image2LineStart);
	
	if (image1LineEnd > 0)
		image1LineEnd = MIN (image1LineEnd, image2LineEnd);
		
	else	// image1LineEnd = 0
		image1LineEnd = image2LineEnd;
	
	image1ColumnStart = MAX (image1ColumnStart, image2ColumnStart);
	
	if (image1ColumnEnd > 0)
		image1ColumnEnd = MIN (image1ColumnEnd, image2ColumnEnd);
		
	else	// image1ColumnEnd = 0
		image1ColumnEnd = image2ColumnEnd;
	
			// Take into account the interval requests.
			
	offset = ldiv (image1LineStart-initialImage1LineStart, image1LineInterval);
	image1LineStart += offset.rem;
			
	offset = ldiv (image1ColumnStart-initialImage1ColumnStart, image1ColumnInterval);
	image1ColumnStart += offset.rem;
			
			// Adjust the image 2 area to represent the image area to be used.

	image2LineStart = MAX (image1LineStart, image2LineStart);
	image2LineEnd = MIN (image1LineEnd, image2LineEnd);
	
	image2ColumnStart = MAX (image1ColumnStart, image2ColumnStart);
	image2ColumnEnd = MIN (image1ColumnEnd, image2ColumnEnd);
	
	if (image2LineStart > image2LineEnd || image2ColumnStart > image2ColumnEnd)
																							return (FALSE);
		
	else	// image2LineStart <= image2LineEnd && ... 
		{
		*image1LineStartPtr = (UInt32)image1LineStart;
		*image1LineEndPtr = (SInt32)image1LineEnd;
		*image1ColumnStartPtr = (SInt32)image1ColumnStart;
		*image1ColumnEndPtr = (SInt32)image1ColumnEnd;
			
				// Return the mask line and column start values relative to the
				// mask area.
				
		*image2LineStartPtr = 
				(UInt32)image2LineStart - image2StartLine + image1StartLine;
		*image2ColumnStartPtr = 
				(UInt32)image2ColumnStart - image2StartColumn + image1StartColumn;
	
																							return (TRUE);
		
		}	// end "else image2LineStart <= image2LineEnd && ..."
		
}	// end "GetCommonArea"


	
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double GetDataValueForBinIndex
//
//	Software purpose:	This routine determines the data value for the input index into
//							the medianArray depending upon the type of array.
//
//	Parameters in:					
//
//	Parameters out:	None				
//
// Value Returned:	Data value	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/20/2005
//	Revised By:			Larry L. Biehl			Date: 01/19/2006

double GetDataValueForBinIndex (
				UInt32								binIndex,
				HistogramSummaryPtr				histogramSummaryPtr,
				SInt32								signedValueOffset)

{
	double								dataValue;
	
	UInt32								binType;
	
	
	binType = histogramSummaryPtr->binType;
	
	if (binType == kDataValueIsBinIndex)
		dataValue = (SInt32)binIndex - signedValueOffset;
		
	else	// binType != kDataValueIsBinIndex
		{
		if (binIndex <= 0)
			dataValue = histogramSummaryPtr->minValue;
			
		else if (binIndex >= histogramSummaryPtr->numberBins-1)
			dataValue = histogramSummaryPtr->maxValue;
			
		else	// binIndex > 0 && <...numberBins-1
			{
			binIndex--;
			if (binType == kBinWidthOfOne)
				dataValue = binIndex + histogramSummaryPtr->minNonSatValue;
			
			else	// binType != kBinWidthOfOne
				dataValue = (double)binIndex/histogramSummaryPtr->binFactor + 
																histogramSummaryPtr->minNonSatValue;
																		
			}	// end "else binIndex > 0 && <...numberBins-1"
																		
		}	// end "else binType != kDataValueIsBinIndex"
		
	return (dataValue);
		
}	// end "GetDataValueForBinIndex" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double GetLongIntValue
//
//	Software purpose:	The purpose of this routine is to obtain the double
//							value from the input string.  The bytes will be swapped
//							if needed depending upon the global setting.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/18/1999
//	Revised By:			Larry L. Biehl			Date: 02/05/2001	

double GetDoubleValue (
				UCharPtr								doublePtr)

{
	double								doubleValue;
	
	
	doubleValue = (double)GetShortDoubleValue (doublePtr);
                      
	#if defined multispec_mac
		#ifdef _MC68881_
			double			mc68881Double;
			
			nonx96tox96 ((double*)&doubleValue, (extended96*)&mc68881Double);

			return (mc68881Double);
		#endif	// _MC68881_ 
	#endif	// defined multispec_mac
		
	return (doubleValue);

}	// end "GetDoubleValue" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double GetShortDoubleValue
//
//	Software purpose:	The purpose of this routine is to obtain the short double
//							value from the input string.  The bytes will be swapped
//							if needed depending upon the global setting.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/18/1999
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

SDouble GetShortDoubleValue (
				UCharPtr								doublePtr)

{
	char								returnValue[8];
		
	if (gSwapBytesFlag)
		{		
		SInt32							tempInt32,
											tempInt32a,
											tempInt32b;
		                      
		memcpy ((Ptr)&tempInt32a, (Ptr)doublePtr, 4);
		memcpy ((Ptr)&tempInt32b, (Ptr)&doublePtr[4], 4);
		
		tempInt32 = ((tempInt32a & 0xff000000) >> 24) | 
						((tempInt32a & 0x00ff0000) >>  8) | 
						((tempInt32a & 0x0000ff00) <<  8) | 
						((tempInt32a & 0x000000ff) << 24);
		memcpy ((Ptr)&returnValue[4], (Ptr)&tempInt32, 4);
		
		tempInt32 = ((tempInt32b & 0xff000000) >> 24) | 
						((tempInt32b & 0x00ff0000) >>  8) | 
						((tempInt32b & 0x0000ff00) <<  8) | 
						((tempInt32b & 0x000000ff) << 24);
		memcpy ((Ptr)returnValue, (Ptr)&tempInt32, 4);

		}	// end "if (gSwapBytesFlag)"
		
	else	// !gSwapBytesFlag
		memcpy ((Ptr)returnValue, (Ptr)doublePtr, 8);
		
	return (*((SDouble*)returnValue));

}	// end "GetShortDoubleValue" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetFileInformationForChannelList
//
//	Software purpose:	The purpose of this routine is to determine whether
//							the channel list represents (1) one image file or 
//							multiple image files; (2) image files with different
//							sizes of data bytes; and (3) the maximum size for the
//							data bytes.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None	
// 
// Called By:			MosaicImagesSideBySideDialog in SMosaic.cpp
//							RectifyImageDialog in SRectifyImage.cpp
//							GetIOBufferPointers in SMemoryUtilities.cpp
//							ChangeImageFormatDialog in SReformatChangeImageFileFormat.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/28/1991
//	Revised By:			Larry L. Biehl			Date: 10/26/1999	

Boolean GetFileInformationForChannelList (
				WindowInfoPtr						windowInfoPtr, 
				LayerInfoPtr						layerInfoPtr, 
				FileInfoPtr							fileInfoPtr,
				SInt16*								channelListPtr, 
				SInt16								numberChannels)
	
{
	Boolean								multipleImageFileFlag;
	
	FileInfoPtr							localFileInfoPtr;
	
	SInt16								channelIndex,
											channelNumber;
	
	UInt16              				fileInfoIndex;
	
	
			// Check some input variables. If 'windowInfoPtr' or 'layerInfoPtr'
			// pointers are NULL then assume that the number of image files
			// is one and is represented by the input 'fileInfoPtr'.
			
	if (windowInfoPtr == NULL || layerInfoPtr	== NULL)
																						return (FALSE);
			
			// Intialize variables.																
			
	multipleImageFileFlag = FALSE;
	windowInfoPtr->localBytesDifferFlag = FALSE;
	windowInfoPtr->localMaxNumberBytes = fileInfoPtr->numberBytes;
	
			// Determine if the data should be forced to two bytes and determine	
			// the maximum number of bytes for the channel to be used.				
					
	channelNumber = 0;
	if (windowInfoPtr->numberImageFiles >= 1)
		{
		if (channelListPtr)
			channelNumber = channelListPtr[0];
			
		channelNumber++;
		
		fileInfoIndex = layerInfoPtr[channelNumber].fileInfoIndex;
		localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
		
		windowInfoPtr->localMaxNumberBytes = localFileInfoPtr->numberBytes;
			
		for (channelIndex=1; channelIndex<numberChannels; channelIndex++)
			{
			if (channelListPtr)
				channelNumber = channelListPtr[channelIndex];
			
			channelNumber++;
				
			if (fileInfoIndex != layerInfoPtr[channelNumber].fileInfoIndex)
				{
				fileInfoIndex = layerInfoPtr[channelNumber].fileInfoIndex;
				multipleImageFileFlag = TRUE;
				localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
				
				if (windowInfoPtr->localMaxNumberBytes != localFileInfoPtr->numberBytes)
					{
					windowInfoPtr->localBytesDifferFlag = TRUE;
					windowInfoPtr->localMaxNumberBytes = 
														MAX (windowInfoPtr->localMaxNumberBytes,
																localFileInfoPtr->numberBytes);
					
					}	// end "if (windowInfoPtr->localMaxNumberBytes != ..." 
				
				}	// end "if (fileInfoIndex != layerInfoPtr..." 
				
			}	// end "for (channel=1; channel<..." 
			
		}	// end "if (windowInfoPtr->numberImageFiles >= 1)" 
		
	return (multipleImageFileFlag);
		
}	// end "GetFileInformationForChannelList" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetImageInformationPointers
//
//	Software purpose:	The purpose of this routine is to lock the window information, 
//							layer information and file information handles for the input 
//							window information handle and load output pointers for the 
//							information structure. Note that locking handles was for an older
//							memory management techiques that are not needed in the new OSes.
//
//	Parameters in:		windowInfoHandle: handle to the window information structure.
//
//	Parameters out:	handleStatusPtr: Code indicating whether the handles were already 
//								locked before the pointers to the structure determined
//							windowInfoPtrPtr: pointer to the memory location to store the 
//								window information pointer
//							layerInfoPtrPtr: pointer  to the memory location to store the
//								layer information pointer
//							fileInfoPtrPtr: pointer to the memory location to store the 
//								file information pointer
//
// Value Returned:	True: valid pointers are available.
//							False: the pointers were not set.
// 
// Called By:			MosaicImagesSideBySide in SMosaic.cpp
//							ClassifyControl in SClassify.cpp
//							PrincipalComponentControl in SPrincipalComponents.cpp
//							GetProjectImageFileInfo in SProject.cpp
//							StatisticsImageControl in SStatisticsImage.cpp
//							SetUpActiveImageInformationGlobals in SUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/07/1993
//	Revised By:			Larry L. Biehl			Date: 08/06/1996	

Boolean GetImageInformationPointers (
				SInt16*								handleStatusPtr, 
				Handle								windowInfoHandle,
				WindowInfoPtr*						windowInfoPtrPtr, 
				LayerInfoPtr*						layerInfoPtrPtr, 
				FileInfoPtr*						fileInfoPtrPtr)

{
	*handleStatusPtr = 0;
	                         
	#if defined multispec_win || defined multispec_wx
			// Until I can figure out how to get 'GlobalFlags' to work. 
		
		if (*windowInfoPtrPtr == NULL) 
			*handleStatusPtr |= 0x0001;
		
		if (*layerInfoPtrPtr == NULL) 
			*handleStatusPtr |= 0x0002;
		
		if (*fileInfoPtrPtr == NULL) 
			*handleStatusPtr |= 0x0004;
	#endif	// defined multispec_win || defined multispec_wx
	
	*fileInfoPtrPtr = NULL;
	*layerInfoPtrPtr = NULL;
	*windowInfoPtrPtr = NULL;
	
			// Lock the information handles for the input window handle and		
			// get pointers to the information.												
	
	if (windowInfoHandle != NULL)
		{
		#if defined multispec_mac 
			if (MHGetState (windowInfoHandle) >= 0)
				*handleStatusPtr |= 0x0001;  
		#endif	// defined multispec_mac    
																				
		*windowInfoPtrPtr = (WindowInfoPtr)GetHandlePointer (
																	windowInfoHandle, kLock, kMoveHi);
		
		}	// end "if (windowInfoHandle != NULL)" 
			
	if (*windowInfoPtrPtr != NULL)
		{
		Handle layerInfoHandle = GetLayerInfoHandle (*windowInfoPtrPtr); 

		#if defined multispec_mac                           
			if (MHGetState (layerInfoHandle) >= 0) 
				*handleStatusPtr |= 0x0002; 
		#endif	// defined multispec_mac                
																				
		*layerInfoPtrPtr = (LayerInfoPtr)GetHandlePointer (
																	layerInfoHandle, kLock, kMoveHi);
				
		}	// end "if (*windowInfoPtrPtr != NULL)" 
	
	if (*windowInfoPtrPtr != NULL) 
		{
		Handle fileInfoHandle = GetFileInfoHandle (*windowInfoPtrPtr); 

		#if defined multispec_mac                           
			if (MHGetState (fileInfoHandle) >= 0)
				*handleStatusPtr |= 0x0004;  
		#endif	// defined multispec_mac              
																				
		*fileInfoPtrPtr = (FileInfoPtr)GetHandlePointer (
																	fileInfoHandle, kLock, kMoveHi);
		
		}	// end "if (*windowInfoPtrPtr != NULL)" 
	
	return (*windowInfoPtrPtr != NULL && 
										*fileInfoPtrPtr != NULL && *layerInfoPtrPtr != NULL);
				
}	// end "GetImageInformationPointers"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetImageList
//
//	Software purpose:	The purpose of this routine is to determine the
//							number of image windows that can be mosaicked to
//							the input image. If the input flag is set,
//							a list of the possible images will be made
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			MosaicImagesSideBySideDialog in SMosaic.cpp
//							ClassifyDialogInitialize in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/06/1993
//	Revised By:			Larry L. Biehl			Date: 04/16/2020

SInt16 GetImageList (
				DialogPtr							dialogPtr, 
				Handle								windowInfoHandle, 
				UInt16								comboListItem,
				Boolean								loadListFlag, 
				SInt16*								listCountPtr)

{
	#if defined multispec_win                                
		CComboBox* 							comboBoxPtr;
	#endif	// defined multispec_win

   #if defined multispec_wx
      wxChoice*    						listCtrl;          
		//wxComboBox* 						comboBoxPtr;
   #endif
	
	FileInfoPtr							fileInfoPtr;
	
	WindowInfoPtr						compareWindowInfoPtr,
											windowInfoPtr;
											
	WindowPtr							windowPtr;
	
	Handle								compareWindowInfoHandle,
											savedCompareWindowInfoHandle;
	
	SInt16								firstEnabledMenuItem,
											imageListLength,
								 			window,
											windowIndex;
	
	Boolean								includeFlag;
	
	SignedByte							handleStatus1,
											handleStatus2,
											handleStatus3;
	
	
	#if defined multispec_win
		USES_CONVERSION;
	#endif

		// Initialize local variables.													
	
	imageListLength = 0;
	*listCountPtr = 0;
	firstEnabledMenuItem = SHRT_MAX;
	savedCompareWindowInfoHandle = NULL;
	
	if (loadListFlag && (gProcessorCode == kClassifyProcessor))
		{
				// Put project base image as the first menu item in the list.		
			
		imageListLength++;
		sprintf ((char*)gTextString, " Base-");
		gTextString[0] = 5;		
		ConcatPStrings (gTextString, 
								gProjectInfoPtr->imageFileName, 
								255);
      
		#if defined multispec_mac  
			AppendMenu (gPopUpTemporaryMenu, "\pNewFile");         
			MSetMenuItemText (
					gPopUpTemporaryMenu, imageListLength, gTextString, kUTF8CharString);
		#endif	// defined multispec_mac   

		#if defined multispec_win       
			comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem (comboListItem);                     
			comboBoxPtr->AddString ((LPCTSTR)_T("\0NewFile\0"));                                           
			dialogPtr->SetComboItemText (comboListItem, 
													imageListLength-1, 
													&gTextString[1],
													kUTF8CharString);
		#endif	// defined multispec_win

      #if defined multispec_wx
			dialogPtr->SetChoiceItemText (comboListItem,
													imageListLength-1, 
													(char*)&gTextString[1]);
		#endif	// defined multispec_wx
				
		(*listCountPtr)++;
		firstEnabledMenuItem = imageListLength;
		
				// Make certain that the input window information handle still		
				// exists.  If not then, the use the project window information	
				// handle.																			
		
		includeFlag = FALSE;
		windowIndex = kImageWindowStart;
		for (window=0; window<gNumberOfIWindows; window++)
			{
			windowPtr = gWindowList[windowIndex];
			compareWindowInfoHandle = GetWindowInfoHandle (windowPtr);
			if (windowInfoHandle == compareWindowInfoHandle)
				includeFlag = TRUE;
			windowIndex++;
														
			}	// end "for (window=0; window<gNumberOfIWindows; window++)" 
			
		if (!includeFlag)
			windowInfoHandle = gProjectInfoPtr->windowInfoHandle;
				
		}	// end "if (loadListFlag && (... == kClassifyProcessor))" 
	
	windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
																	windowInfoHandle,
																	&handleStatus1);
	
	if (gNumberOfIWindows >= 1 && 
				windowInfoPtr != NULL &&
				(windowInfoPtr->bandInterleave != kMixed))
		{
		window = 0;
		windowIndex = kImageWindowStart;
		do
			{
			includeFlag = TRUE;
			
			windowPtr = gWindowList[windowIndex];
			compareWindowInfoHandle = GetWindowInfoHandle (windowPtr);
	
			compareWindowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
																	compareWindowInfoHandle,
																	&handleStatus2);
																	
			if (compareWindowInfoPtr != NULL)
				{
				if (gProcessorCode == kRefMosaicImagesProcessor)
					{
					if (compareWindowInfoPtr->bandInterleave == kBIS ||
														windowInfoPtr->bandInterleave == kBIS)
						{
						if (compareWindowInfoPtr->bandInterleave != 
																	windowInfoPtr->bandInterleave)
							includeFlag = FALSE;
							
						}	// end "if (...->bandInterleave == kBIS || ..." 
				
					}	// end "if (gProcessorCode == kRefMosaicImagesProcessor)" 
						
				if (compareWindowInfoPtr->numberBytes != windowInfoPtr->numberBytes)
					includeFlag = FALSE;
				
				if (compareWindowInfoPtr->totalNumberChannels != 
															windowInfoPtr->totalNumberChannels)
					includeFlag = FALSE;
					
				if ((gProcessorCode == kClassifyProcessor) && 
													compareWindowInfoPtr->projectBaseImageFlag)
					includeFlag = FALSE;
		
				if (includeFlag)
					(*listCountPtr)++;
			
				if (loadListFlag)
					{	
					fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
																	compareWindowInfoPtr->fileInfoHandle,
																	&handleStatus3);
				
					FileStringPtr fileNamePtr = 
											(FileStringPtr)GetFileNamePPointerFromFileInfo (fileInfoPtr);
					
					#if defined multispec_mac  
						imageListLength++;
						AppendMenu (gPopUpTemporaryMenu, "\pNewFile");
						MSetMenuItemText (gPopUpTemporaryMenu, 
												imageListLength, 
												fileNamePtr,
												kASCIICharString);
					#endif	// defined multispec_mac   
			
					#if defined multispec_win  
						if (includeFlag) 
							{
							imageListLength++;
							comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem (comboListItem);                     
							comboBoxPtr->AddString ((LPCTSTR)_T("\0NewFile"));                            
							dialogPtr->SetComboItemText (comboListItem, 
																	imageListLength-1, 
																	&fileNamePtr[2],
																	kUTF8CharString);
							comboBoxPtr->SetItemData (imageListLength-1, windowIndex);
																	
							}	// end "if (includeFlag)"
					#endif	// defined multispec_win
						
					#if defined multispec_wx  
						if (includeFlag) 
							{
							imageListLength++;
							
                     listCtrl = (wxChoice*)wxWindow::FindWindowById (comboListItem);
                     
							dialogPtr->SetChoiceItemText (comboListItem,
																	imageListLength-1, 
																	(char*)&fileNamePtr[2]);
                    
                     SInt64 windowIndex_64 = windowIndex;
                     listCtrl->SetClientData (
													imageListLength - 1, (void*)windowIndex_64);
                     
                     listCtrl->SetString (imageListLength - 1, (char*)&fileNamePtr[2]);
                    
 							}	// end "if (includeFlag)"
					#endif	// defined multispec_wx
					
					if (includeFlag)
						{
						if (gProcessorCode == kRefMosaicImagesProcessor)
							{
							if (firstEnabledMenuItem == SHRT_MAX || 
												savedCompareWindowInfoHandle == windowInfoHandle)
								{
								firstEnabledMenuItem = imageListLength;
								savedCompareWindowInfoHandle = compareWindowInfoHandle;
								
								}	// end "if (firstEnabledMenuItem == SHRT_MAX || ..." 
								
							}	// end "if (gProcessorCode == kRefMosaicImagesProcessor)" 
							
						if (gProcessorCode == kClassifyProcessor)
							{
							if (compareWindowInfoHandle == windowInfoHandle)
								firstEnabledMenuItem = imageListLength;
								
							}	// end "if (gProcessorCode == kClassifyProcessor)" 
							
						}	// end "if (includeFlag)" 
					
					#if defined multispec_mac  
						else	// !includeFlag 
							DisableMenuItem (gPopUpTemporaryMenu, imageListLength);
					#endif	// defined multispec_mac   
				
					MHSetState (compareWindowInfoPtr->fileInfoHandle, handleStatus3);
						
					}	// end "if (loadListFlag)" 
					
				}	// end "if (compareWindowInfoPtr != NULL)"
			
			MHSetState (compareWindowInfoHandle, handleStatus2);
			
			window++;
			windowIndex++;
														
			}		while (window<gNumberOfIWindows);
			
		}	// end "if (gNumberOfIWindows >= 1 && ..." 
			
	MHSetState (windowInfoHandle, handleStatus1);
		
	if (loadListFlag)
		imageListLength = firstEnabledMenuItem;
		
	return (imageListLength);
			
}	// end "GetImageList" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetInformationPointers
//
//	Software purpose:	The purpose of this routine is to lock the window
//							information, layer information and file information
//							handles for the input window information handle and
//							load output pointers for the information structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	-1 if handles were locked upon input. Pointers were
//											successfully loaded.
//							0 if pointers were not intialized. 
//							1 if pointers were successfully loaded.
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/26/1999
//	Revised By:			Larry L. Biehl			Date: 02/26/1999	

Boolean GetInformationPointers (
				SInt16*								handleStatusPtr, 
				Handle								windowInfoHandle,
				WindowInfoPtr*						windowInfoPtrPtr, 
				LayerInfoPtr*						layerInfoPtrPtr, 
				FileInfoPtr*						fileInfoPtrPtr)

{
	*handleStatusPtr = 0;
	                         
	#if defined multispec_win
			// Until I can figure out how to get 'GlobalFlags' to work. 
		
		if (*windowInfoPtrPtr == NULL) 
			*handleStatusPtr |= 0x0001;
		
		if (*layerInfoPtrPtr == NULL) 
			*handleStatusPtr |= 0x0002;
		
		if (*fileInfoPtrPtr == NULL) 
			*handleStatusPtr |= 0x0004;
	#endif	// defined multispec_win 
	
	*fileInfoPtrPtr = NULL;
	*layerInfoPtrPtr = NULL;
	*windowInfoPtrPtr = NULL;
	
			// Lock the information handles for the input window handle and		
			// get pointers to the information.												
	
	if (windowInfoHandle != NULL)
		{
		#if defined multispec_mac 
			if (MHGetState (windowInfoHandle) >= 0)
				*handleStatusPtr |= 0x0001;  
		#endif	// defined multispec_mac    
																				
		*windowInfoPtrPtr = (WindowInfoPtr)GetHandlePointer (
																	windowInfoHandle, kLock, kMoveHi);
		
		}	// end "if (windowInfoHandle != NULL)" 
			
	if (*windowInfoPtrPtr != NULL)
		{
		Handle layerInfoHandle = (*windowInfoPtrPtr)->layerInfoHandle; 

		#if defined multispec_mac                           
			if (MHGetState (layerInfoHandle) >= 0) 
				*handleStatusPtr |= 0x0002; 
		#endif	// defined multispec_mac                
																				
		*layerInfoPtrPtr = (LayerInfoPtr)GetHandlePointer (
																	layerInfoHandle, kLock, kMoveHi);
				
		}	// end "if (*windowInfoPtrPtr != NULL)" 
	
	if (*windowInfoPtrPtr != NULL) 
		{
		Handle fileInfoHandle = (*windowInfoPtrPtr)->fileInfoHandle; 

		#if defined multispec_mac                           
			if (MHGetState (fileInfoHandle) >= 0)
				*handleStatusPtr |= 0x0004;  
		#endif	// defined multispec_mac              
																				
		*fileInfoPtrPtr = (FileInfoPtr)GetHandlePointer (
																	fileInfoHandle, kLock, kMoveHi);
		
		}	// end "if (*windowInfoPtrPtr != NULL)" 
	
	return (*windowInfoPtrPtr != NULL && 
					*fileInfoPtrPtr != NULL && 
							*layerInfoPtrPtr != NULL);
				
}	// end "GetInformationPointers" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetLegendWidthForWindow
//
//	Software purpose:	This routine returns legend width that is to be used for
//							the window being created for the input window information
//							handle. The routine is needed for the windows version
//							because the legend width to be used for the image window
//							is always 0 because the legend and image are in separate
//							(split) windows. If the legend width is needed for print
//							or copy windows then the actual legend width needs to be
//							returned.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/22/1999
//	Revised By:			Larry L. Biehl			Date: 09/06/2017

SInt16 GetLegendWidthForWindow (
				Handle								windowInfoHandle)

{           
	#if defined multispec_mac
  		return (GetLegendWidth (windowInfoHandle));
	#endif // defined multispec_mac
	
	#if defined multispec_mac_swift
  		return (0);
	#endif // defined multispec_mac_swift
	
	#if defined multispec_win
		SInt16		legendWidth = 0;
		
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																						windowInfoHandle);
										
		if (windowInfoPtr != NULL)
			{                                    
	   	if (windowInfoPtr->cImageWindowPtr != NULL)
	   		{
	   		CMImageView* imageViewCPtr = 
	   							windowInfoPtr->cImageWindowPtr->mImageViewCPtr;
	   							
	   		if (imageViewCPtr != NULL && imageViewCPtr->m_printCopyModeFlag)
	   			{ 
					legendWidth = GetLegendWidth (windowInfoHandle);
					
					legendWidth = (SInt16)
								(legendWidth * imageViewCPtr->m_printerTextScaling);
					
					}	// end "if (imageViewCPtr != NULL && ..." 
							
				}	// end "if (windowInfoPtr->cImageWindowPtr != NULL)" 
		
			}	// end "if (windowInfoPtr != NULL)"    
	     
		return (legendWidth);
	#endif // defined multispec_win
	
	#if defined multispec_wx
		return 0;
	#endif
	
}	// end "GetLegendWidthForWindow"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetListBottom
//
//	Software purpose:	The purpose of this routine is to obtain the long int
//							value from the input string.  The bytes will be swapped
//							if needed depending upon the global setting.
//
//	Parameters in:		None
//
//	Parameters out:		None
//
//	Value Returned:		None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/02/1998
//	Revised By:			Larry L. Biehl			Date: 11/06/2019

SInt16 GetListBottom (
				ListHandle							listHandle)

{
	#if defined multispec_mac
		Rect									cellRect;
		
		Cell									cell;
		
		
		cell.h = 0;
		cell.v = MIN (((ListPtr)*listHandle)->visible.bottom,
										((ListPtr)*listHandle)->dataBounds.bottom);
		cell.v--;
		LRect (&cellRect, cell, listHandle);
		
		return (cellRect.bottom);
	#endif	// defined multispec_mac

	#if defined multispec_mac_swift
		return (0);
	#endif	// defined multispec_mac_swift
										  
	#if defined multispec_win
		CRect						listRect; 
															
		SInt16					itemHeight,
									numberItems; 
		
		if (listHandle != NULL)
			{
			numberItems = ((CMLegendList*)listHandle)->GetCount ();
			itemHeight = ((CMLegendList*)listHandle)->GetItemHeight (0); 
										
			return (numberItems * itemHeight + 17); 
				
			}	// end "if (listHandle != NULL)"
			
		else	// listHandle == NULL  
			return (0);
	#endif	// defined multispec_win
	
	return (0);
	
}	// end "GetListBottom"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 GetLongIntValue
//
//	Software purpose:	The purpose of this routine is to obtain the 32-bit int
//							value from the input string.  The bytes will be swapped
//							if needed depending upon the global setting.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/17/1995
//	Revised By:			Larry L. Biehl			Date: 05/12/1995	

SInt32 GetLongIntValue (
				CharPtr								longIntPtr)

{
	if (gSwapBytesFlag)
		{
		SInt32							tempInt32;
		                      
		memcpy ((Ptr)&tempInt32, (Ptr)longIntPtr, 4);
		return (((tempInt32 & 0xff000000) >> 24) | 
					((tempInt32 & 0x00ff0000) >>  8) | 
					((tempInt32 & 0x0000ff00) <<  8) | 
					((tempInt32 & 0x000000ff) << 24));
		
		}	// end "if (gSwapBytesFlag)"
		
	else	// !gSwapBytesFlag
		return (*((SInt32*)longIntPtr));

}	// end "GetLongIntValue" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt64 GetLongInt64Value
//
//	Software purpose:	The purpose of this routine is to obtain the 64-bit int
//							value from the input string.  The bytes will be swapped
//							if needed depending upon the global setting.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/13/2009
//	Revised By:			Larry L. Biehl			Date: 07/13/2009	

SInt64 GetLongInt64Value (
				CharPtr								longInt64Ptr)

{
	if (gSwapBytesFlag)
		{
		/*
		SInt64							tempInt64;
		                      
		memcpy ((Ptr)&tempInt64, (Ptr)longInt64Ptr, 8);
		return (((tempInt64 & 0xff00000000000000) >> 56) |
					((tempInt64 & 0x00ff000000000000) >> 40) |
					((tempInt64 & 0x0000ff0000000000) >> 24) |
					((tempInt64 & 0x000000ff00000000) >>  8) |
					((tempInt64 & 0x00000000ff000000) <<  8) | 
					((tempInt64 & 0x0000000000ff0000) << 24) | 
					((tempInt64 & 0x000000000000ff00) << 40) | 
					((tempInt64 & 0x00000000000000ff) << 56));
		*/
		SInt32							tempInt32,
											tempInt32a,
											tempInt32b;
		                      
		memcpy ((Ptr)&tempInt32a, (Ptr)longInt64Ptr, 4);
		memcpy ((Ptr)&tempInt32b, (Ptr)&longInt64Ptr[4], 4);
		
		tempInt32 = ((tempInt32a & 0xff000000) >> 24) | 
						((tempInt32a & 0x00ff0000) >>  8) | 
						((tempInt32a & 0x0000ff00) <<  8) | 
						((tempInt32a & 0x000000ff) << 24);
		memcpy ((Ptr)&longInt64Ptr[4], (Ptr)&tempInt32, 4);
		
		tempInt32 = ((tempInt32b & 0xff000000) >> 24) | 
						((tempInt32b & 0x00ff0000) >>  8) | 
						((tempInt32b & 0x0000ff00) <<  8) | 
						((tempInt32b & 0x000000ff) << 24);
		memcpy ((Ptr)longInt64Ptr, (Ptr)&tempInt32, 4);	
			
		}	// end "if (gSwapBytesFlag)"
		
	return (*((SInt64*)longInt64Ptr));

}	// end "GetLongInt64Value"
     		


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetMaxSystemPixelSize
//
//	Software purpose:	This routine gets the pixel size (or depth) in
//							number of bits for the input window.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:			GetDefaultPaletteSpecs in SDisplayMultispectral.cpp
//							CreateGraphicsWindow in SGraphiUtilities.cpp
//							AdjustImageWSize in MWindow.c
//							CreateImageWindow in SImageWindow_class.cpp
//							CreatePaletteWindow in MWindow.c
//
//	Coded By:			Larry L. Biehl			Date: 09/06/1989
//	Revised By:			Larry L. Biehl			Date: 04/02/2019

SInt16 GetMaxSystemPixelSize (void)

{ 	
	SInt16								pixelSize;
	
	#if defined multispec_mac
				// Local Declarations 																
		
		GDHandle								theMaxDevice;
		PixMapHandle						maxWindowPortPixMap; 
				  
			
				// Initialize pixelSize to 1 bit, assuming that color quickdraw is	
				// not present.																		
				 
		pixelSize = 1;
			
		if (gColorQDflag)
			{
			Rect		displayRegionRect;
			
			
					// Get number of bits available for the color image from the	
					// deepest device.
					
			GetDisplayRegionRectangle (&displayRegionRect);															
				
			theMaxDevice = ::GetMaxDevice (&displayRegionRect);
			maxWindowPortPixMap = (*theMaxDevice)->gdPMap;
			pixelSize = (*maxWindowPortPixMap)->pixelSize;
															
			}	// end "else gColorQDflag"
	#endif	// defined multispec_mac
		
	#if defined multispec_win
				// Set color information about the display 
				
		CDC				pDC;
		SInt16 			bitsPerPixel = 8;                                             
		
		if (pDC.CreateIC ((LPCTSTR)_T("DISPLAY"), NULL, NULL, NULL))
			{ 
			bitsPerPixel = pDC.GetDeviceCaps (BITSPIXEL);
			
			}	// end "if (pDC.CreateIC ("Display", NULL, NULL, NULL))"
		
		pixelSize = 8;

		if (bitsPerPixel > 8)
			pixelSize = 24;
	#endif	// defined multispec_win 
			
	#if defined multispec_wx
		pixelSize = wxDisplayDepth ();
	
				// Note that for now this needs to be 24 bits for wx macos. The
				// offscreen bit map routines are set up for 24 bits. It may be
				// advantageous to change these routines to the 32 bit max to better fit
				// mac os in the future.
	
		//#if defined multispec_wxmac
		//	pixelSize = MIN (24, pixelSize);
		//#endif
	#endif

	return (pixelSize);
			
}	// end "GetMaxSystemPixelSize" 	
     		


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetMinSystemPixelSize
//
//	Software purpose:	This routine gets the minimum pixel size (or depth) in
//							number of bits for all available monitors.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/04/1997
//	Revised By:			Larry L. Biehl			Date: 12/04/1997		

SInt16 GetMinSystemPixelSize (void)

{ 	
	SInt16								pixelSize;
				  
			
			// Initialize with the maximum pixel size.	
			 
	pixelSize = GetMaxSystemPixelSize ();
	
	#if defined multispec_mac
				// Local Declarations 																
		
		GDHandle								curDevice;
		PixMapHandle						maxWindowPortPixMap; 
			
		if (gColorQDflag)
			{
					// Get number of bits available for the color image from each device.
			
			curDevice = GetDeviceList ();
		
			while (curDevice != NULL)
				{
				if (TestDeviceAttribute (curDevice, screenDevice) &&
						TestDeviceAttribute (curDevice, screenActive))
					{
					maxWindowPortPixMap = (*curDevice)->gdPMap;
					pixelSize = MIN (pixelSize, (*maxWindowPortPixMap)->pixelSize);
					
					}	// end "if (TestDeviceAttribute (curDevice, screenDevice) && ..."
					
				curDevice = GetNextDevice (curDevice);
				
				}	// end "while (curDevice != NULL)" 
				
			pixelSize = MAX (pixelSize, 8);
															
			}	// end "else gColorQDflag"
	#endif	// defined multispec_mac
		
	#if defined multispec_win
				// Do not know how to check for multiple monitors, so will just return
				// the info for the main monitor.
	#endif	// defined multispec_win 
			
	#if defined multispec_wx
			// Do not know how to check for multiple monitors, so will just return
			// the info for the main monitor.
	#endif	// defined multispec_wx

	return (pixelSize);
			
}	// end "GetMinSystemPixelSize" 	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetNumberDecimalDigits
//
//	Software purpose:	The purpose of this routine is to get the number of decimal
//							digits to uniquely define interval with respect to the minimum
//							and maximum values when printed. 
//							The number of digits will be computed for E and F formatted
//							numbers.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/15/2006
//	Revised By:			Larry L. Biehl			Date: 08/05/2016 
 
void GetNumberDecimalDigits (
				UInt16								dataTypeCode,
				double								xMin,
				double								xMax,
				double								interval,
				UInt16*								eDecimalDigitsPtr,
				UInt16*								fDecimalDigitsPtr)

{    
	double								logInterval,
											logXMax,
											logXMin; 
	 
	SInt16								eDecimalDigits,
											fDecimalDigits,
											fDecimalDigits1,
											fDecimalDigits2,
											intervalDecimalPlaces; 
											
	UInt16								nonZeroIntervalDigits,
											nonZeroMaxDigits,
											nonZeroMinDigits;
											
	Boolean								allNonZeroMaxDigitsFlag,
											allNonZeroMinDigitsFlag,
											allNonZeroIntervalDigitsFlag;
	   
	
	eDecimalDigits = 0;
	fDecimalDigits = 0;
	
			// Force to only work with positive data values
	
	xMin = fabs (xMin);
	xMax = fabs (xMax);
	interval = fabs (interval);
	
	if (interval > 0)
		{
		logXMin = 0;
		if (xMin > 0)
			logXMin = log10 (xMin);
		
		logXMax = 0;
		if (xMax > 0)
			logXMax = log10 (xMax);
		
		logInterval = log10 (interval);
		
		if (logXMin >= 0)
			logXMin = floor (logXMin);
		else	// logXMin < 0
			logXMin = floor (logXMin);
		
		if (logXMax >= 0)
			logXMax = floor (logXMax);
		else	// logXMax < 0
			logXMax = floor (logXMax);
		
		if (logInterval >= 0)
			logInterval = floor (logInterval);
		else	// logInterval < 0
			logInterval = floor (logInterval);
		
				// Find number of non-zero leading digits of minimum value
		
		allNonZeroMinDigitsFlag = GetNumberNonZeroLeadingDigits (xMin,
																					logXMin,
																					&nonZeroMinDigits);
		
				// Find number of non-zero leading digits of maximum value
		
		allNonZeroMaxDigitsFlag = GetNumberNonZeroLeadingDigits (xMax,
																					logXMax,
																					&nonZeroMaxDigits);	
		
				// Find number of non-zero leading digits of interval
		
		allNonZeroIntervalDigitsFlag = GetNumberNonZeroLeadingDigits (
																				interval,
																				logInterval,
																				&nonZeroIntervalDigits);
														
				// Now get the maximum number of digits needed for E formats.
														
				// If min, max and interval are all non-zero digits, then the number
				// of digits required to differentiate the three values is just the
				// difference in the exponents to be used.  If this is zero, then assume 
				// 2 decimal digits.
				
		if (allNonZeroMinDigitsFlag)
			nonZeroMinDigits = 2;
				
		if (allNonZeroMaxDigitsFlag)
			nonZeroMaxDigits = 2;
				
		if (allNonZeroIntervalDigitsFlag)
			nonZeroIntervalDigits = 2;
			
		if (logXMin > logInterval)
			{
			if (allNonZeroMinDigitsFlag)
				nonZeroMinDigits = 0;
				
			if (allNonZeroIntervalDigitsFlag)
				nonZeroMinDigits = MAX (nonZeroMinDigits, 
												(UInt16)(logXMin - logInterval));
			
			else	// !allNonZeroIntervalDigitsFlag
				//nonZeroMinDigits = MAX (
				//					nonZeroMinDigits, 
				//					(UInt16)(logXMin - logInterval + nonZeroIntervalDigits));
				nonZeroMinDigits = MAX (nonZeroMinDigits, 
												(UInt16)(logXMin - logInterval));
											
			}	// end "if (logXMin > logInterval)"
											
		else	// logXMin <= logInterval
			nonZeroMinDigits = MAX (nonZeroMinDigits, nonZeroIntervalDigits);
		
		if (logXMax > logInterval)
			{
			if (allNonZeroMaxDigitsFlag)
				nonZeroMaxDigits = 0;
				
			if (allNonZeroIntervalDigitsFlag)
				nonZeroMaxDigits = MAX (nonZeroMaxDigits, 
												(UInt16)(logXMax - logInterval));
			
			else	// !allNonZeroIntervalDigitsFlag
				//nonZeroMaxDigits = MAX (
				//					nonZeroMaxDigits, 
				//					(UInt16)(logXMax - logInterval + nonZeroIntervalDigits));
				nonZeroMaxDigits = MAX (nonZeroMaxDigits, 
												(UInt16)(logXMax - logInterval));
											
			}	// end "if (logXMax > logInterval)"
											
		else	// logXMax <= logInterval
			nonZeroMaxDigits = MAX (nonZeroMaxDigits, nonZeroIntervalDigits);
			
		eDecimalDigits = MAX (nonZeroMinDigits, nonZeroMaxDigits);
			
				// Now ignore the first digit since it will be to the left of the
				// decimal point in an E formatted number.
				// Limit the number of decimal digits for E formatted number to 7.
				// Need to handle case when we have 99999999...
		
		eDecimalDigits = MAX (0, eDecimalDigits);
		eDecimalDigits = MIN (7, eDecimalDigits);
		
				// Now get the decimal digits for an f-formatted number.
		
		if (dataTypeCode == kIntegerType)
			fDecimalDigits = 0;
			
		else	// dataTypeCode == kRealType
			{
			if (logInterval >= 0)
				intervalDecimalPlaces = 0;
			else	// logBinWidth < 0
				intervalDecimalPlaces = (SInt16)fabs (logInterval);
			
			if (logXMin >= 0)
				{
				fDecimalDigits1 = intervalDecimalPlaces + 1;
				/*
				fDecimalDigits1 = logXMin - intervalDecimalPlaces;
				if (fDecimalDigits1 < 0)
					{
					fDecimalDigits1 = abs (fDecimalDigits1);
					fDecimalDigits1 = MAX (intervalDecimalPlaces, fDecimalDigits1);
					
					}	// end "if (fDecimalDigits1 < 0)"
				
				else	// fDecimalDigits1 > 0
					fDecimalDigits1 = 0;
				*/				
				}	// end "if (logXMin >= 0)"
			
			else	// logXMin < 0
				fDecimalDigits1 = MAX ((SInt16)(fabs (logXMin-1)), intervalDecimalPlaces);
			
			if (logXMax >= 0)
				{
				fDecimalDigits2 = intervalDecimalPlaces + 1;
				/*
				fDecimalDigits2 = logXMax - intervalDecimalPlaces;				
				if (fDecimalDigits2 < 0)
					{
					fDecimalDigits2 = abs (fDecimalDigits2);
					fDecimalDigits2 = MAX (intervalDecimalPlaces, fDecimalDigits2);
					
					}	// end "if (fDecimalDigits2 < 0)"
				
				else	// fDecimalDigits1 > 0
					fDecimalDigits2 = 0;
				*/					
				}	// end "if (logXMax >= 0)"
			
			else	// logXMin < 0
				fDecimalDigits2 = MAX ((SInt16)(fabs (logXMax - 1)), 
												intervalDecimalPlaces);
							
			fDecimalDigits = MAX (fDecimalDigits1, fDecimalDigits2);
				
			}	// end "else dataTypeCode == kRealType"
			
		fDecimalDigits = MAX (0, fDecimalDigits);
		fDecimalDigits = MIN (11, fDecimalDigits);
			
		}	// end "if (interval > 0)"
		
	*eDecimalDigitsPtr = eDecimalDigits;
	*fDecimalDigitsPtr = fDecimalDigits;
		
}	// end "GetNumberDecimalDigits" 

	
/*
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetNumberDecimalDigitsForDataVector
//
//	Software purpose:	This routine estimates the number of decimal digits required when
//							listing the data values to present the significant information to
//							the user.
//
//
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/15/2006
//	Revised By:			Larry L. Biehl			Date: 06/15/2006

void GetNumberDecimalDigitsForDataVector (
				UInt16								dataTypeCode,
				double*								dataVectorPtr,
				UInt32								numberElements,
				UInt16*								numberEDecimalDigitsPtr,
				UInt16*								numberFDecimalDigitsPtr)

{
	UInt32								index;
	
	UInt16								numberEDecimalDigits,
											numberFDecimalDigits)

	histogramSpecsPtr->maxNumberBins = gImageWindowInfoPtr->numberBins;
	if (gImageWindowInfoPtr->numberBytes > 2)
		histogramSpecsPtr->maxNumberBins = 2048;
	
	for (index==0; index<numberElements; index++)
		{
		GetNumberDecimalDigits (dataTypeCode,
										dataVectorPtr[index],
										dataVectorPtr[index],
										1/histogramSummaryPtr[chanIndex].binFactor,
										&numberEDecimalDigits,
										&numberFDecimalDigits);
			
		}	// end "for (index==0; index<numberElements; index++)"
		
}	// end "GetNumberDecimalDigitsForDataVector" 
*/


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetNumberFileTypes
//
//	Software purpose:	This routine returns the number of file types in the list
//							to be used as filters for the open file dialog box.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/13/1995
//	Revised By:			Larry L. Biehl			Date: 12/13/1995			

SInt16 GetNumberFileTypes (void)

{              
	SInt16 numberFileTypes = gNumberFileTypes;
		
	#if defined multispec_mac
		if (gEventRecord.modifiers & optionKey)
			numberFileTypes = -1;
	#endif	// defined multispec_mac
		
	#if defined multispec_win 
		
	#endif	// defined multispec_win 
	
	return (numberFileTypes);
			
}	// end "GetNumberFileTypes" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double GetNumberNonZeroLeadingDigits
//
//	Software purpose:	The purpose of this routine is to determine the number of
//							non-zero leading digits.  The number of leading digits will be
//							limited to 8.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/03/2006
//	Revised By:			Larry L. Biehl			Date: 03/21/2006			

Boolean GetNumberNonZeroLeadingDigits (
				double								dataValue,
				double								floorLogDataValue,
				UInt16*								nonZeroIntevalDigitsPtr)

{
	char									string [128];
	
	UInt32								index,
											numberZeros;
	
	
	sprintf (string, "%10.8E", dataValue);
	
			// Find number of leading digits before 3 zeros.
	
	numberZeros =  0;
	index = 2;		
	while (index < 10)	
		{
		if (string[index] == '0')
			numberZeros++;
			
		else	// string[index] != '0'
			numberZeros = 0;
			
		if (numberZeros >= 3 || index == 9)
			break;
			
		index++;
						
		}	// end "while (index < 10)"
		
			// Assume if less than 3 zeros in a row that all are non zero.
			
	if (numberZeros < 3)
		numberZeros = 0;
	
			// Ignore the leading value before the decimal point.
			
	index--;
		
	*nonZeroIntevalDigitsPtr = (UInt16)(index - numberZeros);
	
	return (numberZeros==0);
		
}	// end "GetNumberNonZeroLeadingDigits" 
		


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt64 GetNumberPixelsInArea
//
//	Software purpose:	The purpose of this routine is to compute the total
//							number of pixels in the input rectangular area
//							given the line and column sample interval.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/11/1994
//	Revised By:			Larry L. Biehl			Date: 08/20/2010	

SInt64 GetNumberPixelsInArea (
				SInt32								lineStart,
				SInt32								lineEnd,
				SInt32								lineInterval,
				SInt32								columnStart,
				SInt32								columnEnd,
				SInt32								columnInterval,
				SInt16								algorithmCode)

{
	SInt64								numberPixels;
	
		
	if (algorithmCode == 0)
		{
		numberPixels = (lineEnd - lineStart + lineInterval)/lineInterval;
		numberPixels = MAX (0, numberPixels);
		numberPixels *= 
					(columnEnd - columnStart + columnInterval)/columnInterval;
					
		}	// end "if (algorithmCode == 0)" 
		
	else	// algorithmCode != 0 
		{
		if (algorithmCode & kSPClusterCase)
					// Adjust start line for one-pass cluster.							
			lineStart += lineInterval;
	
		numberPixels = (lineEnd - lineStart + lineInterval)/lineInterval;
		numberPixels = MAX (0, numberPixels);
		
		if (columnEnd - columnStart + 1 >= columnInterval)
			{
			numberPixels *= (columnEnd - columnStart + 1);
			numberPixels = MAX (0, numberPixels);
	
			numberPixels = (numberPixels + columnInterval - 1)/columnInterval;
			
			}	// end "if (columnEnd - columnStart + 1 >= columnInterval)"
			
		else	// (columnEnd - columnStart + 1) < columnInterval
			{
					// For this case there will always be one column from each
					// line that is used.
					
			//numberPixels *= 1; // This code is here just for documentation.
			
			}	// end "if (columnEnd - columnStart + 1 >= columnInterval)"
				
		if (algorithmCode & kSPClusterCase)
					// Now add in the number of pixels in the first line for 		
					// single pass cluster case.												
			numberPixels += (columnEnd - columnStart + 1);
		
		}	// end "else algorithmCode != 0" 
					
	numberPixels = MAX (0, numberPixels);
	
	return (numberPixels);
	
}	// end "GetNumberPixelsInArea"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		DisplaySpecsPtr GetActiveDisplaySpecsPtr
//
//	Software purpose:	The purpose of this routine is to obtain the pointer
//							to the display specifications for the current palette.
//
//							Renamed from GetPaletteDisplaySpecPtr to 
//								GetActiveDisplaySpecsPtr on 4/04/1996.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			UpdatePaletteFor16and24BImage in SDisplay16_24Bits.cpp
//							UpdateActiveImageLegend in SPalette.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/09/1989
//	Revised By:			Larry L. Biehl			Date: 06/04/1996	

DisplaySpecsPtr GetActiveDisplaySpecsPtr (
				Handle*								displaySpecsHandlePtr)
				
{
			// Declare local variables.
											
	DisplaySpecsPtr					displaySpecsPtr = NULL;		
	
	Handle								localDisplaySpecsH,
											windowInfoHandle;												
	
	SignedByte							handleStatus;
	
	                              
	windowInfoHandle = GetActiveImageWindowInfoHandle ();
		
			// Get handle to display specification	for active image					
			// window.  Lock handle to display specification & get 					
			// pointer to it.	
			
	localDisplaySpecsH = GetDisplaySpecsHandle (windowInfoHandle);
	                       	
	if (localDisplaySpecsH != NULL)
		{
				// Get status of handle.  If the handle is already locked then		
				// return displaySpecsH as NULL so that it will not be unlock 		
				// by the routine that called this routine.  The handle 				
				// will be unlocked later by another routine.
		
		displaySpecsPtr = (DisplaySpecsPtr)GetHandleStatusAndPointer (
																						localDisplaySpecsH,
																						&handleStatus);
			
		if (handleStatus < 0)
			localDisplaySpecsH = NULL;
							
		}	// end "if (localDisplaySpecsH != NULL)" 
	
	*displaySpecsHandlePtr = localDisplaySpecsH;
	return (displaySpecsPtr);
				
}	// end "GetActiveDisplaySpecsPtr" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetOneChannelThematicDisplayConversionValues
//
//	Software purpose:	The purpose of this routine is to get the conversion values
//							to use to convert data values to bin values to be used as
//							an index into the display value vector.
//
//	Parameters in:		.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/24/2013
//	Revised By:			Larry L. Biehl			Date: 10/24/2013

void GetOneChannelThematicDisplayConversionValues (
				WindowInfoPtr						imageWindowInfoPtr,
				HistogramSummaryPtr				histogramSummaryPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				double*								binFactorPtr,
				double*								minValuePtr,
				UInt32*								maxBinPtr)
				
{				
								
	*binFactorPtr = histogramSummaryPtr->binFactor;
	*minValuePtr = histogramSummaryPtr->minNonSatValue;
	*maxBinPtr = histogramSummaryPtr->numberBins - 1;
						
	if (imageWindowInfoPtr->localMaxNumberBytes <= 2)
		{
		*binFactorPtr = 1;
		*minValuePtr = 0;
		
		}	// end "if (imageWindowInfoPtr->localMaxNumberBytes <= 2)"
						
	else	// imageWindowInfoPtr->localMaxNumberBytes > 2
		{
		if (histogramSummaryPtr->binType == kBinWidthOfOne)
			{
			*binFactorPtr = displaySpecsPtr->thematicBinWidth;
			*minValuePtr = displaySpecsPtr->thematicTypeMinMaxValues[0];
			*maxBinPtr = displaySpecsPtr->numberLevels - 1;
			
			}	// end "if (histogramSummaryPtr->binType == kBinWidthOfOne)"
			
		else if (histogramSummaryPtr->binType == kBinWidthNotOne)
			{
			*binFactorPtr = displaySpecsPtr->thematicBinWidth;
			*minValuePtr = displaySpecsPtr->thematicTypeMinMaxValues[0];
			
			}	// end "else if (histogramSummaryPtr->binType == kBinWidthNotOne)"
			
		}	// end "else imageWindowInfoPtr->localMaxNumberBytes > 2"
				
}	// end "GetOneChannelThematicDisplayConversionValues" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetProbabilityThresholdCode
//
//	Software purpose:	The purpose of this routine is to get the code level for the 
//							requested probability threshold value.  This routine must 
//							determine which type of probability map it is for.  One type 
//							has 109 levels; the other type has 120 levels.  The number of 
//							levels is obtained from the input file information structure 
//							handle. If that handle is NULL, then the current number of 
//							probability levels is used.
//
//	Parameters in:		.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			BiPlotDataDialog in SBiPlotData.cpp
//							ClassifyDialog in SClassify.cpp
//							ClusterDialog in SCluster.cpp
//							ListResultsDialog in SListResults.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/04/1994
//	Revised By:			Larry L. Biehl			Date: 01/16/1998

SInt16 GetProbabilityThresholdCode (
				double								thresholdPercent,
				Handle								probabilityFileInfoHandle)

{
	SInt32								index,
											numberProbabilityClasses,
											probabilityThresholdCode;
										
			
	probabilityThresholdCode = 0;							
	if (thresholdPercent > 0 && thresholdPercent <= 100)
		{
		if (probabilityFileInfoHandle != NULL)
			{
			FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (
																			probabilityFileInfoHandle);
			numberProbabilityClasses = (SInt16)(fileInfoPtr->numberClasses - 1);
			
			}	// end "if (probabilityFileInfoHandle != NULL)" 
			
		else	// probabilityFileInfoHandle != NULL 	
			numberProbabilityClasses = gNumberProbabilityClasses;
			
		index = 0;
		if (numberProbabilityClasses == 121)
			{
			probabilityThresholdCode = 12;
				
			if (thresholdPercent <= 0.1)
				{
				probabilityThresholdCode = 13;
				
						// Adjust to the decade that the threshold value is in.			
						
				for (index=1; index<=12; index++)
					{
					if (thresholdPercent <= .01)
						{
						thresholdPercent *= 10;
						
						probabilityThresholdCode--;
						
						}	// end "if (thresholdPercent <= 1)" 
						
					else	// thresholdPercent > .1 
						break;
					
					}	// end "for (index=1; index<=13; index++)" 
					
				thresholdPercent = 0;
					
				}	// end "if (thresholdPercent <= 0.1)" 
				
			}	// end "if (numberProbabilityClasses == 121)" 
			
		if (thresholdPercent > 0)
			{	
			if (thresholdPercent <= 1)
				index = (SInt16)(10. * thresholdPercent + 0.5);
				
			else	// thresholdPercent > 1 
				{
				index = (SInt16)(thresholdPercent + 0.5);
				index += 9;
				
				}	// end "else thresholdPercent > 1"  
			
			probabilityThresholdCode += index;
				
			}	// end "if (thresholdPercent > 0)"
		
		}	// end "if (thresholdPercent > 0 && thresholdPercent <= 100)" 
															
	return ((SInt16)probabilityThresholdCode);
	
}	// end "GetProbabilityThresholdCode"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				FileInfoPtr GetResultsFilePtr
//
//	Software purpose:			The purpose of this routine is to return the
//									pointer to the file information for the specified
//									results file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/19/1991
//	Revised By:			Larry L. Biehl			Date: 04/04/1991	

FileInfoPtr GetResultsFilePtr (
				SInt16								index)

{
	DiskFileListPtr					diskFileListPtr;
	FileInfoPtr							resultsFilePtr;
	
	
			// Initialize local variables.												
			
	resultsFilePtr = NULL;
	
	if (index >= 0 && gResultsFileSummary.numberFiles > index)
		{
		diskFileListPtr = gResultsFileSummary.diskFileListPtr;
		
		if (diskFileListPtr != NULL)
			resultsFilePtr = diskFileListPtr[index].fileInfoPtr;
		
		}	// end "if (gResultsFileSummary.numberFiles >= indexNumber)" 
		
	return (resultsFilePtr);

}	// end "GetResultsFilePtr"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				CMFileStream* GetResultsFileStreamPtr
//
//	Software purpose:			The purpose of this routine is to return the
//									pointer to the file stream information for the specified
//									results file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/26/1995
//	Revised By:			Larry L. Biehl			Date: 10/26/1995	

CMFileStream* GetResultsFileStreamPtr (
				SInt16								index)

{
	CMFileStream*						resultsFileStreamPtr;
	DiskFileListPtr					diskFileListPtr;
	
	
			// Initialize local variables.												
			
	resultsFileStreamPtr = NULL;
	
	if (index >= 0 && gResultsFileSummary.numberFiles > index)
		{
		diskFileListPtr = gResultsFileSummary.diskFileListPtr;
		
		if (diskFileListPtr != NULL)
			resultsFileStreamPtr = GetFileStreamPointer (
																diskFileListPtr[index].fileInfoPtr);
		
		}	// end "if (gResultsFileSummary.numberFiles >= indexNumber)" 
		
	return (resultsFileStreamPtr);

}	// end "GetResultsFileStreamPtr"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetThresholdCode
//
//	Software purpose:	The purpose of this routine is to get the code level
//							for the requested threshold value.  This result depends
//							on the type of input threshold table being used.
//
//	Parameters in:		.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			ClassifyDialog in SClassify.cpp
//							ListResultsDialog in SListResults.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/13/1998
//	Revised By:			Larry L. Biehl			Date: 11/02/2019

SInt16 GetThresholdCode (
				double								thresholdValue,
				Handle								probabilityFileInfoHandle,
				SInt16								thresholdTypeCode)

{
	SInt32								thresholdCode;
	
	
	if (thresholdTypeCode == kKNearestNeighborMode)
		{
				// Threshold for KNearestNeighbor type table.
		
		thresholdCode = (SInt32)(thresholdValue - 1);
		
		}	// end "else gClassifySpecsPtr->mode == kKNearestNeighborMode"
											
	else if (thresholdTypeCode == kCorrelationMode)
		{
				// Threshold for Correlation type table.
				
		thresholdCode = (90 - (SInt32)thresholdValue) + 1;
			
		}	// end "else gClassifySpecsPtr->mode == kCorrelationMode"
		
	else if (thresholdTypeCode == kCEMMode)
		{
				// Threshold for CEM type table.
		
		thresholdCode = (SInt32)(50*thresholdValue) + 1;
			
		}	// end "else gClassifySpecsPtr->mode == kCorrelationMode"
		
	else	// thresholdTypeCode != kCorrelationMode && ...
		{
				// Threshold for probability type table.
				
		thresholdCode = GetProbabilityThresholdCode (
													thresholdValue, probabilityFileInfoHandle);
			
		}	// end "else ...->mode != kCorrelationMode && ..."
															
	return ((SInt16)thresholdCode);
	
}	// end "GetThresholdCode"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		time_t GetTotalTime
//
//	Software purpose:	The purpose of this routine is to compute the total
//							numer of seconds since the input time.  Checks are
//							made to make certain that the resulting time is not
//							less than 0 which happens from time to time - I
//							do not understand why.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	total time 
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/12/1992
//	Revised By:			Larry L. Biehl			Date: 03/12/1992	

UInt32 GetTotalTime (
				time_t								startTime)

{
	time_t								endTime;

	UInt32								totalTime;
	
			
	endTime = time (NULL);
	
	if (endTime >= startTime)
		{
		totalTime = (UInt32)(endTime - startTime);
		return (totalTime);

		}	// end "if (endTime >= startTime)"
	
	else	// endTime < startTime 
		return (0);
	
}	// end "GetTotalTime" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 GetTotalNumberOfPixels
//
//	Software purpose:	The purpose of this routine is to determine the total
//							number of pixels that are specified by the selected
//							area and/or class list.  A 'specialMode' code is
//							provided for special cases such as for Single Pass
//							Clustering.
//
//	Parameters in:		specialMode
//								& 0x0001  = Allow for cluster case.
//								& 0x0002  = Allow for single pass cluster case.
//
//	Parameters out:	None
//
// Value Returned:	Number of cluster pixels
// 
// Called By:			BiPlotDataControl in SBiPlotData.cpp
//							InitializeClusterMemory in SCluster.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/06/1990
//	Revised By:			Larry L. Biehl			Date: 12/31/2003

SInt64 GetTotalNumberOfPixels (
				SInt16								fieldTypeCode, 
				SInt16*								classPtr, 
				UInt32								numberClasses, 
				SInt32*								selectionAreaPtr,
				SInt16								specialMode)

{	
	AreaDescription					areaDescription;
												
	Point									point;
	RgnHandle							rgnHandle;
	
	SInt64								pixelCount,
											totalPixelCount;
	
	SInt32								areaNumber,	
											column,
											columnEnd, 
											columnInterval,
											columnStart,
											firstColumnSkip,
											line,
											lineEnd, 
											lineInterval,
											lineStart,
											localColumnInterval;
	
	SInt16								fieldNumber,
											lastClassIndex,
											lastFieldIndex;
											
	Boolean								singlePassClusterFlag;
	
	
			// Check parameters.																	
			
	if (fieldTypeCode < kTrainingType || 
								fieldTypeCode > kTrainingType+kTestingType+kAreaType)
																								return (0);
	
			// Initialize local variables.													
			
	totalPixelCount = 0;
												
	if (fieldTypeCode & kAreaType)
		{
				// Get the pixel count for the selected area.							
			
		pixelCount = GetNumberPixelsInArea (selectionAreaPtr[0],
														selectionAreaPtr[1],
														selectionAreaPtr[2],
														selectionAreaPtr[3],
														selectionAreaPtr[4],
														selectionAreaPtr[5],
														specialMode);
			
		totalPixelCount = pixelCount;
		
		}	// end "if (fieldTypeCode & kAreaType)" 
																						
	if (fieldTypeCode & (kTrainingType+kTestingType))
		{
		if (gProjectInfoPtr == NULL || numberClasses <= 0)
																								return (0);
																						
		lineInterval = selectionAreaPtr[2];								
		columnInterval = selectionAreaPtr[5];
		lastClassIndex = -1;
		lastFieldIndex = -1;
		singlePassClusterFlag = (specialMode & kSPClusterCase);
	
		InitializeAreaDescription (&areaDescription);	
			
				// Loop by number of areas.													
			
		for (areaNumber=0; areaNumber<kMaxNumberStatFields; areaNumber++)
			{	
					// Get the next field number.												
						
			fieldNumber = GetNextFieldArea (gProjectInfoPtr, 
														classPtr, 
														numberClasses, 
														&lastClassIndex, 
														lastFieldIndex, 
														fieldTypeCode,
														kDontIncludeClusterFields);
							
			if (fieldNumber < 0)
				break;
																							
			lastFieldIndex = fieldNumber;
			
					// Get the field coordinates.				
						
			GetFieldBoundary (gProjectInfoPtr, &areaDescription, fieldNumber);
					
			if (areaDescription.pointType != kPolygonType)
				{
				if (areaDescription.maskInfoPtr == NULL)
					
							// Get pixel count for rectangular field.							
				
					pixelCount = GetNumberPixelsInArea (areaDescription.lineStart,
																	areaDescription.lineEnd,
																	lineInterval,
																	areaDescription.columnStart,
																	areaDescription.columnEnd,
																	columnInterval,
																	specialMode);
											
				else	// areaDescription.maskInfoPtr != NULL
					{
							// Get pixel count for mask field.	
							
					pixelCount = (SInt32)GetNumberPixelsInMaskArea (
														areaDescription.maskInfoPtr,
														(UInt16)areaDescription.maskValueRequest, 
														(UInt32)areaDescription.maskLineStart, 
														(UInt32)areaDescription.maskColumnStart,
														areaDescription.lineStart,
														areaDescription.lineEnd,
														lineInterval,
														areaDescription.columnStart,
														areaDescription.columnEnd,
														columnInterval,
														specialMode);
					
					}	// end "else areaDescription.maskInfoPtr != NULL"
				
				}	// end "if (areaDescription.pointType != kPolygonType)" 
						
			else	// areaDescription.pointType == kPolygonType
				{
						// Get pixel count for polygonal field.							
						
				lineStart = areaDescription.lineStart;
				lineEnd = areaDescription.lineEnd;
				columnStart = areaDescription.columnStart;
				columnEnd = areaDescription.columnEnd;
				rgnHandle = areaDescription.rgnHandle;
						
				firstColumnSkip = 0;
				pixelCount = 0;		
								
						// Initialize vertical (line) point.								
								
				point.v = (SInt16)lineStart;				
			   		
		   			// Handle column interval for the one-pass cluster case.		
		   			
		   	localColumnInterval = columnInterval;
				if (singlePassClusterFlag)
		   		localColumnInterval = 1;
				
						// Loop through lines for area.										
				
				for (line=lineStart; line<=lineEnd; line+=lineInterval)
					{
				   point.h = (SInt16)(columnStart + firstColumnSkip);
					
			   			// Loop through columns for area. 								
			   	
			   	for (column = point.h; 
			   			column <= columnEnd; 
								column += localColumnInterval)
						{
					   
						if (PtInRgn (point, rgnHandle))
							pixelCount++;
		      
		   			point.h += (SInt16)localColumnInterval;
							
						}	// end "for (column = point.h; ..." 
		   			
  					point.v += (SInt16)lineInterval;
  							
  							// Make certain that the column interval is correct for	
  							// the rest of the lines.											
  							
			   	localColumnInterval = columnInterval;
  				
  					if (specialMode != 0)
  						firstColumnSkip = column - columnEnd - 1;
						
					}	// end "for (line=lineStart; ..." 
						
						// Dispose of the region.		
					
				CloseUpAreaDescription (&areaDescription);
					
				}	// end "else areaDescription.pointType == kPolygonType" 
						
			totalPixelCount += pixelCount;
			
			}	// end "for (areaNumber=0; ... 
			
		}	// end "if (fieldTypeCode & (kTrainingType+kTrainingType))" 
		
	return (totalPixelCount);

}	// end "GetTotalNumberOfPixels"	 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetWindowClipRectangle
//
//	Software purpose:	This routine returns the rectangle which represent the area
//							of the input window (view) with no scroll bars and no
//							legend and title if requested.
//
//	Parameters in:		windowPtr for the window to be evaluated
//							areaCode for portion of the window to be evaluated
//								- kFrameArea: the entire window
//								- kImageArea: the image portion of the window
//								- kImageFrameArea: the portion of the window that can contain 
//										an image
//								- kCoordinateArea: the coordinate portion of the window	
//								- kNoScrollBarArea: the entire window less the scroll area
//										Same as Image Frame Area when there is no legend
//										and not coordinate view and title. (i.e. text window).
//								- kGraphArea: the graph area of the window	
//								- kImageScrollArea: the image area including the scroll bars
//								- kCoordinateCursorArea: cursor area of the coordinate view
//								- kCoordinateSelectionArea: selection area of the 
//										coordinate view
//								- kCoordinateAreaUnitsArea: area units of the coordinate view
//								- kTitleArea: area for the title for side by side displays
//								- kBottomScrollBar area for scroll bar on the bottom
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/24/1996
//	Revised By:			Larry L. Biehl			Date: 01/10/2020

void GetWindowClipRectangle (
				WindowPtr							windowPtr, 
				SInt16								areaCode, 
				Rect*									outRectanglePtr)

{
	DisplaySpecsPtr					displaySpecsPtr;
	WindowInfoPtr						windowInfoPtr;
		
	Handle								windowInfoHandle;
	
	SInt32								imageBottom,
											imageRight;
											
										
	if (windowPtr != NULL)
		{
		#if defined multispec_mac
			SInt32								legendWidth,
													topOffset;
		
			GetWindowPortBounds (windowPtr, outRectanglePtr);
		
			windowInfoHandle = (Handle)GetWRefCon (windowPtr);
			windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
			if (windowInfoPtr == NULL)
				areaCode = kFrameArea;
				
			switch (areaCode)
				{
				case kImageArea:
					legendWidth = windowInfoPtr->legendWidth;
					topOffset = windowInfoPtr->imageTopOffset;
						
					outRectanglePtr->top += topOffset;
					outRectanglePtr->left += legendWidth;
					outRectanglePtr->bottom -= kSBarWidth;
					outRectanglePtr->right -= kSBarWidth;
					
					displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
										windowInfoPtr->displaySpecsHandle);
					
					if (displaySpecsPtr != NULL)
						{ 										
						imageRight = legendWidth + displaySpecsPtr->magnification *
													displaySpecsPtr->imageDimensions[kHorizontal];
											 
						imageBottom = topOffset + displaySpecsPtr->magnification *
													displaySpecsPtr->imageDimensions[kVertical];
						
						if (imageRight < (SInt32)outRectanglePtr->right)											
							outRectanglePtr->right = (SInt16)imageRight;
							
						if (imageBottom < (SInt32)outRectanglePtr->bottom)
							outRectanglePtr->bottom = (SInt16)imageBottom;
						
						}	// end "if (displaySpecsPtr != NULL)"
					break;
					
				case kCoordinateArea:
					outRectanglePtr->bottom = windowInfoPtr->coordinateHeight;
					break;
					
				case kCoordinateCursorArea:
					outRectanglePtr->bottom = windowInfoPtr->coordinateHeight;
					outRectanglePtr->left = windowInfoPtr->coordinateViewCursorStart;
					outRectanglePtr->right = windowInfoPtr->coordinateViewSelectionStart;
					break;
					
				case kCoordinateSelectionArea:
					outRectanglePtr->bottom = windowInfoPtr->coordinateHeight;
					outRectanglePtr->left = windowInfoPtr->coordinateViewSelectionStart;
					outRectanglePtr->right = windowInfoPtr->coordinateViewAreaPopupStart;
					break;
					
				case kCoordinateAreaUnitsArea:
					outRectanglePtr->bottom = windowInfoPtr->coordinateHeight;
					outRectanglePtr->left = windowInfoPtr->coordinateViewNumberPixelsStart;
					break;
					
				case kImageScrollArea:
					outRectanglePtr->top = windowInfoPtr->coordinateHeight;
					break;
					
				case kGraphArea:
					outRectanglePtr->bottom -= kSBarWidth;
					break;
					
				case kGraphBinWidth:
					outRectanglePtr->top = outRectanglePtr->bottom - kSBarWidth + 2;
					outRectanglePtr->left += 95 + 6*kLegendScrollWidth+25 + 60;
					break;
					
				case kImageFrameArea:
				case kNoScrollBarArea:
					outRectanglePtr->top += windowInfoPtr->imageTopOffset;
					outRectanglePtr->left += windowInfoPtr->legendWidth;
					outRectanglePtr->bottom -= kSBarWidth;
					outRectanglePtr->right -= kSBarWidth;
					break;
					
				case kTitleArea:
					outRectanglePtr->top += windowInfoPtr->coordinateHeight;
					outRectanglePtr->left += windowInfoPtr->legendWidth;
					outRectanglePtr->bottom = outRectanglePtr->top + 
																			windowInfoPtr->titleHeight;
					outRectanglePtr->right -= kSBarWidth;
					break;
					
				case kBottomScrollBar:
			  		outRectanglePtr->top = outRectanglePtr->bottom - kSBarWidth - 1;
					outRectanglePtr->right -= kSBarWidth;
					break;
				
				}	// end "switch (areaCode)"			
		#endif	// defined multispec_mac		
		
		#if defined multispec_win		
			Rect									inRect;
			SInt32								legendWidth,
													topOffset;
			
			
					// Get input view rectangle.
					
			windowPtr->GetClientRect ((RECT*)&inRect);  
			
			outRectanglePtr->top = inRect.top;
			outRectanglePtr->left = inRect.left;
			outRectanglePtr->bottom = inRect.bottom;
			outRectanglePtr->right = inRect.right;
		
			windowInfoHandle = GetWindowInfoHandle (windowPtr);
			windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
														windowInfoHandle);
			if (windowInfoPtr == NULL)
				areaCode = kImageFrameArea;  
				
			switch (areaCode)
				{
				case kImageArea:
					legendWidth = GetLegendWidthForWindow (windowInfoHandle);       
					topOffset = windowPtr->GetTitleHeight ();
						
					outRectanglePtr->top += topOffset;
					outRectanglePtr->left += legendWidth;
					outRectanglePtr->right += legendWidth;
					
					displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
															GetDisplaySpecsHandle (windowInfoPtr));
					
					if (displaySpecsPtr != NULL)
						{ 										
						imageRight = (SInt32)(legendWidth +
												displaySpecsPtr->magnification *
													displaySpecsPtr->imageDimensions[kHorizontal]);
											 
						imageBottom = (SInt32)(topOffset +
												displaySpecsPtr->magnification *
													displaySpecsPtr->imageDimensions[kVertical]);
						
						if (imageRight < (SInt32)outRectanglePtr->right)											
							outRectanglePtr->right = (SInt16)imageRight;
							
						if (imageBottom < (SInt32)outRectanglePtr->bottom)
							outRectanglePtr->bottom = (SInt16)imageBottom;
						
						}	// end "if (displaySpecsPtr != NULL)"
					break;   
					
				case kImageFrameArea:
					legendWidth = GetLegendWidthForWindow (windowInfoHandle);       
					topOffset = windowPtr->GetTitleHeight ();
					
					outRectanglePtr->top += topOffset;
					outRectanglePtr->left += legendWidth;
					outRectanglePtr->right += legendWidth;
					break;   
					
				case kBottomScrollBar:
			  		outRectanglePtr->top = outRectanglePtr->bottom - kSBarWidth - 1;
					outRectanglePtr->right -= kSBarWidth;
					break;
				
				}	// end "switch (areaCode)"         
			/*					
			if (areaCode == kImageFrameArea)
				{                           
				windowInfoHandle = GetWindowInfoHandle (windowPtr);		
				legendWidth = GetLegendWidthForWindow (windowInfoHandle);        
				titleHeight = windowPtr->GetTitleHeight ();
				
				}	// end "if (areaCode == kImageFrameArea)"
				
			outRectanglePtr->top = inRect.top + titleHeight;
			outRectanglePtr->left = inRect.left + legendWidth;
			outRectanglePtr->bottom = inRect.bottom;
			outRectanglePtr->right = inRect.right + legendWidth; 
			*/		
		#endif	// defined multispec_win
					
		#if defined multispec_wx
			CMImageFrame*						imageFramePtr;
			wxRect								frameArea;
			wxRect								tempArea;

			windowInfoHandle = GetWindowInfoHandle (windowPtr);
			windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
			if (windowInfoPtr == NULL)
				areaCode = kImageFrameArea;
			/*
					// Get input view rectangle.
					
			wxRect imagearea = (windowPtr->m_Canvas)->GetClientRect ();
			wxRect imagearea2 = (windowPtr->m_frame)->GetClientRect ();
			windowPtr->GetClientRect ((RECT*) & inRect);
			wxRect imagearea = ((windowPtr->m_frame)->m_mainWindow)->GetClientRect ();
			*/
			imageFramePtr = windowPtr->GetImageFrameCPtr ();

			switch (areaCode)
				{
				case kImageArea:
					if ((windowPtr->m_frame)->m_mainWindow->IsShown ())
						frameArea = ((windowPtr->m_frame)->m_mainWindow)->GetClientRect ();
					else
						frameArea = wxRect (0,0,0,0);
					
					outRectanglePtr->top = frameArea.GetTop ();
					outRectanglePtr->left = frameArea.GetLeft ();
					outRectanglePtr->bottom = frameArea.GetBottom ();
					outRectanglePtr->right = frameArea.GetRight ();
               
					displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
															GetDisplaySpecsHandle (windowInfoPtr));
					
					if (displaySpecsPtr != NULL)
						{ 										
						imageRight = (SInt32)(displaySpecsPtr->magnification *
													displaySpecsPtr->imageDimensions[kHorizontal]);
											 
						imageBottom = (SInt32)(displaySpecsPtr->magnification *
													displaySpecsPtr->imageDimensions[kVertical]);
						
								// If imageRight or imageBottom equal 0, then this implies
								// no image is being displayed in the window yet.
							
						if (imageRight > 0 &&
												imageRight < (SInt32)outRectanglePtr->right)
							outRectanglePtr->right = imageRight;
							
						if (imageBottom > 0 &&
												imageBottom < (SInt32)outRectanglePtr->bottom)
							outRectanglePtr->bottom = imageBottom;
						
						}	// end "if (displaySpecsPtr != NULL)"
					break;
							
				case kImageFrameArea:
					if ((windowPtr->m_frame)->m_mainWindow->IsShown ())
						frameArea = ((windowPtr->m_frame)->m_mainWindow)->GetClientRect ();
					else
						frameArea = wxRect (0,0,0,0);
					
					outRectanglePtr->top = frameArea.GetTop ();
					outRectanglePtr->left = frameArea.GetLeft ();
					outRectanglePtr->bottom = frameArea.GetBottom ();
					outRectanglePtr->right = frameArea.GetRight ();
					break;
				
				}	// end "switch (areaCode)"				
		#endif	// defined multispec_wx

		}	// end "if (windowPtr != NULL)"
	
}	// end "GetWindowClipRectangle"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetWindowImageMagnification
//
//	Software purpose:	This routine returns the image magnification for the window
//							refered to by the input window information structure pointer.
//
//	Parameters in:		Pointer to window information structure		
//
//	Parameters out:				
//
// Value Returned:	magnification
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/12/1999
//	Revised By:			Larry L. Biehl			Date: 03/12/1999		

double GetWindowImageMagnification (
				WindowInfoPtr						windowInfoPtr)

{
	double								magnification = 1.0;
	
	if (windowInfoPtr != NULL)
		{
		Handle displaySpecsH = windowInfoPtr->displaySpecsHandle;
		DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																							displaySpecsH);
															
		if (displaySpecsPtr != NULL)													
			magnification = displaySpecsPtr->magnification; 
			
		}	// end "if (windowInfoPtr != NULL)"
		
	return (magnification);
	
}	// end "GetWindowImageMagnification"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeAreaDescription
//
//	Software purpose:	The purpose of this routine is to initialize the input
//							area description structure to default values.
//		
//	Parameters in:		Pointer to the area description structure.	
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/31/2003
//	Revised By:			Larry L. Biehl			Date: 12/31/2003

void InitializeAreaDescription (
				AreaDescriptionPtr				areaDescriptionPtr)

{			
	InitializeAreaDescription (areaDescriptionPtr, 
										1, 
										1, 
										1, 
										1, 
										1, 
										1,
										1,
										1,
										0);
	
}	// end "InitializeAreaDescription"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeAreaDescription
//
//	Software purpose:	The purpose of this routine is to initialize the input
//							area description structure with the input pararmeters.
//		
//	Parameters in:		Pointer to the area description structure.
//							lineStart
//							lineEnd
//							columnStart
//							columnEnd
//							lineInterval
//							columnInterval
//							startLine
//							startColumn
//							diskFileFormat	
//
//	Parameters out:	None	
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/23/1998
//	Revised By:			Larry L. Biehl			Date: 01/12/1999

void InitializeAreaDescription (
				AreaDescriptionPtr				areaDescriptionPtr, 
				SInt32								lineStart, 
				SInt32								lineEnd, 
				SInt32								columnStart, 
				SInt32								columnEnd, 
				SInt32								lineInterval, 
				SInt32								columnInterval,
				SInt32								startLine,
				SInt32								startColumn,
				SInt16								diskFileFormat)

{
	if (areaDescriptionPtr != NULL)
		{
		areaDescriptionPtr->maskInfoPtr = NULL;
		areaDescriptionPtr->rgnHandle = NULL;
		
		areaDescriptionPtr->line = 0;
		
		areaDescriptionPtr->lineStart = lineStart;
		areaDescriptionPtr->lineEnd = lineEnd;
		areaDescriptionPtr->columnStart = columnStart;
		areaDescriptionPtr->columnEnd = columnEnd;
		areaDescriptionPtr->columnStart = columnStart;
		areaDescriptionPtr->lineInterval = lineInterval;
		areaDescriptionPtr->columnInterval = columnInterval;
		
		areaDescriptionPtr->columnOffset = 0;
		areaDescriptionPtr->lineOffset = 0;
		areaDescriptionPtr->maxNumberColumns = 0;
		areaDescriptionPtr->maxNumberLines = 0;
		
		areaDescriptionPtr->numberLines = 0;
		if (lineInterval > 0 && lineStart > 0 && lineEnd >= lineStart)
			areaDescriptionPtr->numberLines = 
											(lineEnd - lineStart + lineInterval)/lineInterval;
			
		areaDescriptionPtr->numSamplesPerChan = 0;
		if (columnInterval > 0 && columnStart > 0 && columnEnd >= columnStart)
			areaDescriptionPtr->numSamplesPerChan = 
									(columnEnd - columnStart + columnInterval)/columnInterval;
							
		areaDescriptionPtr->notPolygonPoints = 0;
		areaDescriptionPtr->startLine = startLine;
		areaDescriptionPtr->startColumn = startColumn;
		
		areaDescriptionPtr->maskColumnStart = 1;
		areaDescriptionPtr->maskLineStart = 1;
		areaDescriptionPtr->maskValueRequest = 0;
		
		areaDescriptionPtr->classNumber = 0;
		areaDescriptionPtr->fieldNumber = 0;
		areaDescriptionPtr->diskFileFormat = diskFileFormat; 
		areaDescriptionPtr->pointType = kRectangleType; 
		
		areaDescriptionPtr->applyOffsetFlag = FALSE; 
		areaDescriptionPtr->polygonFieldFlag = FALSE; 
		
		}	// end "if (areaDescriptionPtr != NULL)"
	
}	// end "InitializeAreaDescription"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeDoubleVariables ()
//
//	Software purpose:	The purpose of this procedure is to initialize
//							the double variables in MultiSpec.  
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/04/1990
//	Revised By:			Larry L. Biehl			Date: 05/17/2012	

void InitializeDoubleVariables (void)
 
{
	
			// Initialize minimum magnification that is allowed.						
			// This is done here, because putting this line of code in 				
			// 'InitializeMultiSpec' causes coprocessor calls to be made before	
			// the check is made to determine if a coprocessor is available.		
			// The calls made are those to place the coprocessor registers on		
			// the stack upon entry into 'InitializeMultiSpec'.						

	gMinMagnification = 1/(double)gMaxMagnification;
	
	gLowerDeterminantOffsetAllowed = -35.;
	
			// Initialize the conversion factors from one distance unit to
			// another. This is used in displaying the coordinates in the coordinate
			// view. Everthing is relative to 1 mm.
			
	gDistanceFileConversionFactors[0] = 1000000.;			// km to mm
	gDistanceFileConversionFactors[1] = 1000.;				// m  to mm
	gDistanceFileConversionFactors[2] = 10.;					// cm to mm
	gDistanceFileConversionFactors[3] = 1.;					// mm to mm
	gDistanceFileConversionFactors[4] = 0.001;				// um to mm
	gDistanceFileConversionFactors[5] = 1609344.;			// mi to mm
	gDistanceFileConversionFactors[6] = 914.4;				// yd to mm
	gDistanceFileConversionFactors[7] = 304.800609601219;	// survey ft to mm
	gDistanceFileConversionFactors[8] = 304.8;				// ft to mm
	gDistanceFileConversionFactors[9] = 25.4;					// in to mm
	
			// Initialize the conversion factors from one distance unit to
			// another. This is used in displaying the coordinates in the coordinate
			// view. Everthing is relative to 1 mm.
			
	gDistanceDisplayConversionFactors[0] = 1000000.;		// km to mm
	gDistanceDisplayConversionFactors[1] = 100000.;			// sq root of hectare to mm
	gDistanceDisplayConversionFactors[2] = 1000.;			// m  to mm
	gDistanceDisplayConversionFactors[3] = 10.;				// cm to mm
	gDistanceDisplayConversionFactors[4] = 1.;				// mm to mm
	gDistanceDisplayConversionFactors[5] = 0.001;			// um to mm
	gDistanceDisplayConversionFactors[6] = 1609344.;		// mi to mm
	gDistanceDisplayConversionFactors[7] = 63614.90724;	// acre to mm
	gDistanceDisplayConversionFactors[8] = 914.4;			// yd to mm
	gDistanceDisplayConversionFactors[9] = 304.800609601219;	// survey ft to mm
	gDistanceDisplayConversionFactors[10] = 304.8;			// ft to mm
	gDistanceDisplayConversionFactors[11] = 25.4;			// in to mm
	
}	// end "InitializeDoubleVariables" 


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InvalidateWindow
//
//	Software purpose:	The purpose of this routine is to invalidate the
//							requested portion of the input window. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/10/1996
//	Revised By:			Larry L. Biehl			Date: 03/28/2019

void InvalidateWindow (
				WindowPtr							windowPtr, 
				SInt16								areaCode,
				Boolean								eraseFlag)

{  
	if (windowPtr != NULL)
		{
		#if defined multispec_mac  
			Rect									rectArea;  
			GrafPtr								savedPort;
			
			
			GetWindowClipRectangle (windowPtr, areaCode, &rectArea);
				
			GetPort (&savedPort);
			SetPortWindowPort (windowPtr);
			InvalWindowRect (windowPtr, &rectArea);
			if (eraseFlag)
				::EraseRect (&rectArea);
			SetPort (savedPort);   
		#endif	// defined multispec_mac 
								 
		#if defined multispec_win
			windowPtr->Invalidate (eraseFlag);
		#endif	// defined multispec_win      
		
		#if defined multispec_wx
			if (areaCode == kCoordinateSelectionArea)
				((CMImageView*)windowPtr)->m_frame->m_coordinatesBar->Refresh (eraseFlag);
			else	//
				((CMImageView*)windowPtr)->m_Canvas->Refresh (eraseFlag);
		#endif
		
		}	// end "if (windowPtr != NULL)"

}	// end "InvalidateWindow"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadChannelsVector
//
//	Software purpose:	The purpose of this routine is to load the channels vector with
//							a list of channels that will need to be accessed from the image
//							file.  Use the transformation feature list (if available) or the
//							feature list to determine the channels to be used.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned: 		
//
// Called By:			BiPlotDataDialog in SBiPlotData.cpp
//							ClassifyDialog in SClassifyDialogs.cpp
//							ClusterDialog in SCluster.cpp
//							LoadClusterSpecs in SCluster.cpp
//							FeatureExtractionDialog in SFeatureExtraction.cpp
//							LoadListDataSpecs in SListData.cpp
//							ListDataDialog in SListData.cpp
//							EvaluateCovariancesDialog in SOther.cpp
//							LoadPrincipalComponentSpecs in SPrincipalComponents.cpp
//							PrincipalComponentDialog in SPrincipalComponents.cpp
//							SeparabilityDialog in SFeatureSelection.cpp
//							LoadStatHistogramSpecs in SProjectHistogramStatistics.cpp
//							StatHistogramDialog in SProjectHistogramStatistics.cpp
//							StatisticsImageDialog in SStatisticsImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/05/1989
//	Revised By:			Larry L. Biehl			Date: 10/13/1997

void LoadChannelsVector (
				Boolean								projectFlag, 
				Boolean								useTransformFlag, 
				SInt16								numberFeatures,
				SInt16*								featurePtr, 
				SInt16*								numberChannelsPtr, 
				SInt16*								channels)

{
	SInt16*								lFeaturePtr;

	SInt16								index,
											numberChannels;
	
	
	if (useTransformFlag && 
			gTransformationMatrix.eigenFeatureHandle &&
					gTransformationMatrix.createdByCode < 16)
		{
		lFeaturePtr = (short int*)GetHandlePointer (
														gTransformationMatrix.eigenFeatureHandle);
		numberChannels = gTransformationMatrix.numberChannels;
		
		}	// end "if (useTransformFlag && ..." 
		
	else	// !useTransformFlag || !gTransformationMatrix.eigenFeatureHandle || ...
		{
		lFeaturePtr = featurePtr;
		numberChannels = numberFeatures;
		
		}	// end "else !useTransformFlag || ..." 
	
	if (projectFlag && gProjectInfoPtr && !useTransformFlag)
		{
		for (index=0; index<numberChannels; index++)
			channels[index] = gProjectInfoPtr->channelsPtr[lFeaturePtr[index]];
			
		}	// end "if (projectFlag && gProjectInfoPtr ..." 
		
	else	// "!projectFlag || !gProjectInfoPtr || ..." 
		{
		for (index=0; index<numberChannels; index++)
			channels[index] = lFeaturePtr[index];
			
		}	// end "else !projectFlag || !gProjectInfoPtr || ..." 
		
	*numberChannelsPtr = numberChannels;
							
}	// end "LoadChannelsVector" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadClassName
//
//	Software purpose:	The purpose of this routine is to load the class name
//							into the specified location.  If the class is not part
//							of the list, then -not used. is added at the end of the
//							name.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	Length of the class name in characters
// 
// Called By:			CreateThematicSupportFile in SFileIO.cpp
//							WriteGAIAHeader in SGAIARoutines.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/30/1992
//	Revised By:			Larry L. Biehl			Date: 01/16/1998	

SInt16 LoadClassName (
				SInt16								classNumber, 
				SInt16								numberClasses, 
				SInt16*								classPtr, 
				UInt16*								classIndexPtr, 
				char*									outputStringPtr,
				SInt16								nameCode)

{	
	SInt16								classListNumber,
											classStorage,
											nameLength;
	
	
	nameLength = 0;
	
	if (gProjectInfoPtr && nameCode != kClassNumberOnly)
		{
				// Get the class storage number.											
				
		classStorage = gProjectInfoPtr->storageClass[classNumber-1];
	
		nameLength = gProjectInfoPtr->classNamesPtr[classStorage].name[0];
		BlockMoveData ((char*)&gProjectInfoPtr->classNamesPtr[classStorage].name[1],
							outputStringPtr, 
							nameLength);
			
		if (classPtr != NULL)			
			classListNumber = classPtr[*classIndexPtr];
			
		else	// classPtr == NULL
			classListNumber = classNumber;
		
		if (classNumber == classListNumber)
			{
			if (*classIndexPtr < (UInt16)(numberClasses - 1))
				(*classIndexPtr)++;
						
			}	// end "if (classNumber == classListNumber)" 
			
		else	// classNumber != classListNumber 
			{
			if (nameLength > 22)
				nameLength = 22;
			sprintf (&outputStringPtr[nameLength], "-not used");
			
			nameLength += 9;
			
			}	// end "else classNumber != classListNumber" 
			
		}	// end "if (gProjectInfoPtr && nameCode != kClassNumberOnly)" 
		
	else	// gProjectInfoPtr == NULL || nameCode == kClassNumberOnly 
		{
		sprintf (outputStringPtr, "Class %d    ", classNumber+1);
		nameLength = 10;
		
		}	// end "else gProjectInfoPtr == NULL || nameCode == ..." 
		
	return (nameLength);

}	// end "LoadClassName" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadFeatureVector
//
//	Software purpose:	The purpose of this routine is to load the feature
//							vector with a list of pointers to the channels 
//							in the project that are included in the project
//							statistics.
//		
//	Parameters in:				
//
//	Parameters out:			
//
//	Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/15/1989
//	Revised By:			Larry L. Biehl			Date: 06/07/1996

void LoadFeatureVector (
				Boolean								useTransformFlag, 
				SInt16*								outputNumberFeaturesPtr, 
				SInt16*								featurePtr, 
				SInt16								inputNumberFeatures)

{
	SInt16								index;
	
	
	if (useTransformFlag && gTransformationMatrix.eigenFeatureHandle != NULL)
		inputNumberFeatures = gTransformationMatrix.numberFeatures;
	
	for (index=0; index<inputNumberFeatures; index++)
		featurePtr[index] = index;
																	
	*outputNumberFeaturesPtr = inputNumberFeatures;
							
}	// end "LoadFeatureVector"


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void MForeColor
//
//	Software purpose:	The purpose of this routine is to set the foreground color. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/31/1995
//	Revised By:			Larry L. Biehl			Date: 09/06/2017	
// Revised By:			Tsung Tai Yeh			Date: 09/10/2015
#ifndef multispec_wx
	void* MForeColor (
					CDC*									pDC,
					SInt32								color)
#else
	void* MForeColor (
					wxDC*									pDC,
					SInt32								color)
#endif
{         
#if defined multispec_mac 
	ForeColor (color);
	
	return (NULL);
#endif	// defined multispec_mac
	
#if defined multispec_mac_swift
	return (NULL);
#endif	// defined multispec_mac_swift

#if defined multispec_win
	CPen									*newPenPtr = NULL,
											*oldPenPtr = NULL;
	                 
	switch (color)
		{
		case blackColor:
			newPenPtr = &gBlackPen;
			break; 
			
		case blueColor:
			newPenPtr = &gBluePen;
			break;   
			
		case cyanColor:
			newPenPtr = &gCyanPen;
			break;  
			 
		case greenColor:
			newPenPtr = &gGreenPen;
			break;    
			
		case magentaColor:
			newPenPtr = &gMagentaPen;
			break; 
			
		case redColor:
			newPenPtr = &gRedPen;
			break;     
			
		case whiteColor:
			newPenPtr = &gWhitePen;
			break;   
			
		case yellowColor:
			newPenPtr = &gYellowPen;
			break; 
			
		default:
			newPenPtr = NULL;
			
		}	// end "switch (color)" 
	
	if (newPenPtr != NULL)
		oldPenPtr = pDC->SelectObject (newPenPtr);
		
	return ((void*)oldPenPtr); 
#endif	// defined multispec_win 
   
#if defined multispec_wx
   wxPen									*newPenPtr = NULL,
											*oldPenPtr = NULL;
	                 
	
	switch (color)
		{
		case blackColor:
			newPenPtr = &gBlackPen;
			break; 
			
		case blueColor:
			newPenPtr = &gBluePen;
			break;   
			
		case cyanColor:
			newPenPtr = &gCyanPen;
			break;  
			 
		case greenColor:
			newPenPtr = &gGreenPen;
			break;    
			
		case magentaColor:
			newPenPtr = &gMagentaPen;
			break; 
			
		case redColor:
			newPenPtr = &gRedPen;
			break;     
			
		case whiteColor:
			newPenPtr = &gWhitePen;
			break;   
			
		case yellowColor:
			newPenPtr = &gYellowPen;
			break; 
			
		default:
			newPenPtr = NULL;
			
		}	// end "switch (color)" 
   
   if (newPenPtr != NULL)
		oldPenPtr = newPenPtr;
		
	pDC->SetPen (wxPen (*oldPenPtr));
	
	return ((void*)oldPenPtr);
#endif

}	// end "MForeColor"


                                                           
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void MHiliteControl
//
//	Software purpose:	The purpose of this routine is to set the hilite for the
//							specified control to the requested value.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			many routines
//
//	Coded By:			Larry L. Biehl			Date: 02/01/1996
//	Revised By:			Larry L. Biehl			Date: 02/01/1996	

void MHiliteControl (
				WindowPtr							windowPtr,
				ControlHandle						controlHandle,
				SInt16								hiliteValue)

{
			// Declare local variables and structures
			
	#if defined multispec_mac	
		HiliteControl (controlHandle, hiliteValue);
	#endif	// defined multispec_mac 
	
	#if defined multispec_win   
		SInt64 windowid64 = (SInt64)((int*)(controlHandle));
		SInt16 windowid = (SInt16)windowid64;
	   windowPtr->GetDlgItem (windowid)->EnableWindow (hiliteValue==0);
	#endif	// defined multispec_win
   
   #if defined multispec_wx   
		SInt64 windowid64 = (SInt64)((int*)(controlHandle));
		SInt16 windowid = (SInt16)windowid64;
		windowPtr->GetFrame()->FindWindow (windowid)->Enable (hiliteValue==0);
	#endif	// defined multispec_wx
	 
}	// end "MHiliteControl"      


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void MInitCursor
//
//	Software purpose:	The purpose of this routine is to set the cursor
//							to the pointer cursor. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/23/1995
//	Revised By:			Larry L. Biehl			Date: 08/28/2018

void MInitCursor (void)

{         
	#if defined multispec_mac 
		::InitCursor ();
	#endif	// defined multispec_mac	
				
	#if defined multispec_win
		AfxGetApp()->DoWaitCursor (0);
	#endif	// defined multispec_win 
		
	#if defined multispec_wx
		//(wxTheApp->GetTopWindow ())->SetCursor (wxCursor (wxCURSOR_ARROW));
		CMainFrame* mframeptr = (CMainFrame*)(wxTheApp->GetTopWindow ());
		CMImageView* currentview = wxDynamicCast (
						mframeptr->GetDocumentManager()->GetCurrentView (), CMImageView);
		if (currentview != NULL)
			{
			CMImageCanvas* canvasptr = currentview->m_Canvas;
			//canvasptr->SetCursor (wxCursor (wxCURSOR_ARROW));
			canvasptr->SetCursor (wxCURSOR_ARROW);
			}
	
		wxSetCursor (wxNullCursor);
	#endif
	
	gPresentCursor = kArrow;
		
}	// end "MInitCursor"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void MSetCursor
//
//	Software purpose:	The purpose of this routine is to set the specified
//							cursor.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/29/1995
//	Revised By:			Larry L. Biehl			Date: 03/09/2020
 
void MSetCursor (
				SInt16								cursorIndex)

{     
	#if defined multispec_mac 
		if (gCursorPtr[cursorIndex] != NULL)
			SetCursor (gCursorPtr[cursorIndex]);
			
		else	// gCursorPtr[cursorIndex] == NULL
			::InitCursor ();
	#endif	// defined multispec_mac  

	#if defined multispec_win
		if (gPresentCursor == kWait && cursorIndex != kWait)
			AfxGetApp()->DoWaitCursor (-1);
		
		switch (cursorIndex)
			{
			case kArrow:
				//AfxGetApp()->DoWaitCursor (-1);                   
				::SetCursor (AfxGetApp()->LoadStandardCursor (IDC_ARROW));
				break;
	  
			case kWait:
				AfxGetApp()->DoWaitCursor (1);
				break;
				
			case kCross:
				//AfxGetApp()->DoWaitCursor (-1); 
				//::SetCursor (AfxGetApp()->LoadCursor (IDC_CROSS_CURSOR));
				::SetCursor (AfxGetApp()->LoadCursor (gCrossCursorID));
				break;
			
			case kBlinkOpenCursor1:
				//AfxGetApp()->DoWaitCursor (-1); 
				::SetCursor (AfxGetApp()->LoadCursor (IDC_BLINK_OPEN));
				break;
			
			case kBlinkOpenCursor2:
				//AfxGetApp()->DoWaitCursor (-1); 
				::SetCursor (AfxGetApp()->LoadCursor (IDC_BLINK_OPEN));
				break;
				
			case kBlinkShutCursor1:
				//AfxGetApp()->DoWaitCursor (-1); 
				::SetCursor (AfxGetApp()->LoadCursor (IDC_BLINK_SHUT));
				break;
				
			case kBlinkShutCursor2:
				//AfxGetApp()->DoWaitCursor (-1); 
				::SetCursor (AfxGetApp()->LoadCursor (IDC_BLINK_SHUT));
				break;
				
			default:
				//AfxGetApp()->DoWaitCursor (-1);                   
				::SetCursor (AfxGetApp()->LoadStandardCursor (IDC_ARROW));
				cursorIndex = kArrow;
				break;
				
			}	// end "switch (cursorIndex)"
	#endif	// defined multispec_win   

	#if defined multispec_wx 
				// Based on what I understand now:
				//		canvasptr->SetCursor sets the cursor for the respective window
				//			that the cursor is over.
				//		wxSetCursor sets the cursor overall and overrides the window
				//			setting. One needs to use wxSetCursor (wxNullCursor) to set the
				//			overall cursor to the 'default' such that the window cursor
				//			setting takes over.
	
		CMainFrame* mframeptr = (CMainFrame*)(wxTheApp->GetTopWindow ());
		CMImageView* currentView = wxDynamicCast (
							mframeptr->GetDocumentManager()->GetCurrentView (), CMImageView);
	
		if (currentView == NULL && gImageWindowInfoPtr != NULL)
					// This may be in middle of an activation event for a window. Use
					// the view from the active image window if it is available.
					// The technique above will not work in this situation until the
					// end of the activation process
			currentView = gImageWindowInfoPtr->cImageWindowPtr->mImageViewCPtr;

		if (currentView != NULL)
			{
			if (cursorIndex == kBlinkOpenCursor1 || cursorIndex == kBlinkShutCursor1)
				{
				CMLegendView* legendViewCPtr =
					currentView->GetDocument()->GetImageFrameCPtr()->GetLegendViewCPtr ();
				
				CMLegendList* legendListCPtr = legendViewCPtr->GetLegendListCPtr ();
				
				if (legendListCPtr != NULL)
					{
					switch (cursorIndex)
						{
						case kBlinkOpenCursor1:
							legendListCPtr->SetCursor (
															currentView->m_frame->m_blinkOpenCursor);
							wxSetCursor (currentView->m_frame->m_blinkOpenCursor);
							break;

						case kBlinkShutCursor1:
							legendListCPtr->SetCursor (
															currentView->m_frame->m_blinkShutCursor);
							wxSetCursor (currentView->m_frame->m_blinkShutCursor);
							break;
							
						default:
							legendListCPtr->SetCursor (wxCURSOR_ARROW);
							wxSetCursor (wxNullCursor);
							cursorIndex = kArrow;
							break;
						
						}	// end "switch (cursorIndex)"
					
					}	// end "if (legendListCPtr != NULL)"
				
				}	// end "if (cursorIndex == kBlinkOpenCursor1 || cursorIndex == ..."
	
			else	// cursorIndex != kBlinkOpenCursor1 && != kBlinkShutCursor1
				{
				CMImageCanvas* canvasptr = currentView->m_Canvas;
				
				switch (cursorIndex)
					{
					case kArrow:
						canvasptr->SetCursor (wxCURSOR_ARROW);
						wxSetCursor (wxNullCursor);
						break;

					case kWait:
						canvasptr->SetCursor (wxCURSOR_WAIT);
						wxSetCursor (wxCURSOR_WAIT);
						break;

					case kCross:
						canvasptr->SetCursor (wxCURSOR_CROSS);
						wxSetCursor (wxNullCursor);
						break;

					case kBlinkOpenCursor2:
						canvasptr->SetCursor (currentView->m_frame->m_blinkOpenCursor);
						wxSetCursor (currentView->m_frame->m_blinkOpenCursor);
						break;

					case kBlinkShutCursor2:
						canvasptr->SetCursor (currentView->m_frame->m_blinkShutCursor);
						wxSetCursor (currentView->m_frame->m_blinkShutCursor);
						break;
					
					default:
						canvasptr->SetCursor (wxCURSOR_ARROW);
						wxSetCursor (wxNullCursor);
						cursorIndex = kArrow;
						break;
					
					}	// end "switch (cursorIndex)"
			
				}	// end "else cursorIndex != kBlinkOpenCursor1 && != kBlinkShutCursor1"
					
			}	// end "if (currentView != NULL)"
	
					// If wait cursor is being requested make this the overall cursor.
			
		//if (cursorIndex == kWait)
	
		//#if defined multispec_wxmac
		//else if (cursorIndex != kBlinkOpenCursor2 && cursorIndex != kBlinkShutCursor2)
					// Set overall cursor setting to default so that cursor setting for
					// window take precidence
		//#endif
	#endif	// defined multispec_wx
	
   gPresentCursor = cursorIndex;  

}	// end "MSetCursor"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void PauseIfInBackground
//
//	Software purpose:	This routine will pause execution until MultiSpec is
//							in the foreground.  This is used before the standard
//							file routines and alerts are posted.  This approach
//							is used until the Notification Manager routines can
//							be implemented.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/12/1993
//	Revised By:			Larry L. Biehl			Date: 03/12/1993			

void PauseIfInBackground (void)
													
{
	#if defined multispec_mac
		Boolean							tempFlag;
		
		
				// Wait until we are not in the background.									
				
				
		while (gInBackground)
			{
			tempFlag = CheckSomeEvents (osMask);
			
			}	// end "while (gInBackground)"
	#endif	// defined multispec_mac
	
	#if defined multispec_win  
			
	#endif	// defined multispec_win 
		
}	// end "PauseIfInBackground"  


	
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void RemoveSuffix
//
//	Software purpose:	This routine removes some standard suffixes to be ready for a 
//							new suffix such as .Project, .sta, etc.	
//
//	Parameters in:		Pointer to pascal name			
//
//	Parameters out:	None				
//
// Value Returned:	None	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/22/1999
//	Revised By:			Larry L. Biehl			Date: 04/16/2020

void RemoveSuffix (
				FileStringPtr							fileNamePtr)

{
			// Remove some standard suffixes to be ready for a new suffix.		
			
	RemoveCharsNoCase ((char*)"\0.asc\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.bag\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.grb\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.LAN\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.GIS\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.IMG\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.ige\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.DAT\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.cluster\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.CLU\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.RAW\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.bil\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.bsq\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.bip\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.ecw\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.fst\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.cal\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.drk\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.hdf\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.nc\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.I1\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.I2\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.I3\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.I4\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.I5\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.I6\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.I7\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.I8\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.I9\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.dd\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.h5\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.jp2\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.jpg\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.png\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.tiff\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.tif\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.prj\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.lbl\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.project\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.STI\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.ntf\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.nsf\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.pix\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.vrt\0", fileNamePtr);
	RemoveCharsNoCase ((char*)"\0.txt\0", fileNamePtr);
		
}	// end "RemoveSuffix"  


	
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ResetDestinationWindowParameters
//
//	Software purpose:	The purpose of this routine is to reset the parameters that
//							were set for drawing to a specified destination window.
//
//	Parameters in:		windowCode;		=1, kToImageWindow;
//												=2, kToPrintWindow
//												=3, kToClipboardWindow
//
//							Saved orgin parameters
//
//	Parameters out:	None				
//
// Value Returned:	None	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/27/2001
//	Revised By:			Larry L. Biehl			Date: 06/19/2002

void ResetDestinationWindowParameters (
				SInt16								windowCode,
				WindowPtr							windowPtr)
				//WindowInfoPtr					windowInfoPtr,
				//SInt32*							saveOriginPtr)

{				
	if (windowCode == kToImageWindow)
		{
		GetWindowClipRectangle (windowPtr, kFrameArea, &gViewRect);
		ClipRect (&gViewRect);
		
		}	// end "if (windowCode == kToImageWindow)" 
	/*	
			// If the window is a clipboard or printer window, then reset the	
			// display window offsets.														
			
	if (windowCode == kToClipboardWindow)
		{
		DisplaySpecsPtr					displaySpecsPtr;
		Handle								displaySpecsH;
		
		displaySpecsH = GetDisplaySpecsHandle (windowInfoPtr);
		displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsH);
		
		if (displaySpecsPtr != NULL)
			{														
			displaySpecsPtr->origin[kVertical] = saveOriginPtr[kVertical];
			displaySpecsPtr->origin[kHorizontal] = saveOriginPtr[kHorizontal];
			
			}	// end "if (displaySpecsPtr != NULL)"
		
		}	// end "if (windowCode == kToClipboardWindow)" 
	*/		
}	// end "ResetDestinationWindowParameters"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ResetMapToWindowUnitsVariables
//
//	Software purpose:	The purpose of this routine is to reset some parameter relative
//							to converting from map units to window units and vice versa. For
//							now this is just unlocking the handle to the map polygon 
//							transformation coefficients.
//							
//
//	Parameters in:		Handle to window information handle
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			DrawArcViewShapes in SArcView.cpp
//							OutlineFieldsControl in SOutlineFields.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/26/2007
//	Revised By:			Larry L. Biehl			Date: 02/26/2007	

void ResetMapToWindowUnitsVariables (
				Handle								windowInfoHandle,
				MapToWindowUnitsVariablesPtr	mapToWindowUnitsVariablesPtr)

{	
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	Handle								mapProjectionHandle;
											
			
	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																					mapProjectionHandle);
							
	CloseCoefficientsVectorPointers (mapProjectionInfoPtr);
	
	mapToWindowUnitsVariablesPtr->polynomialOrder = 0;
	mapToWindowUnitsVariablesPtr->easting2CoefficientsPtr = NULL;	
	mapToWindowUnitsVariablesPtr->northing2CoefficientsPtr = NULL;	
		
}	// end "ResetMapToWindowUnitsVariables" 


	
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetActiveDisplaySpecsHandle
//
//	Software purpose:	This routine gets the handle containing
//							the display specifications structure.
//
//	Parameters in:		None			
//
//	Parameters out:	None				
//
// Value Returned:	Handle for the display specifications structure.	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/18/1995
//	Revised By:			Larry L. Biehl			Date: 11/18/1995

void SetActiveDisplaySpecsHandle (
				Handle								displaySpecsHandle)

{
	#if defined multispec_mac 	
		if (gActiveImageWindowInfoH != NULL)
			((WindowInfoPtr)*gActiveImageWindowInfoH)->displaySpecsHandle =
																						displaySpecsHandle;
	#endif	// defined multispec_mac 
	
	#if defined multispec_win || defined multispec_wx
		if (gActiveImageViewCPtr != NULL)
			gActiveImageViewCPtr->m_displayMultiCPtr->mDisplaySpecsHandle =
																						displaySpecsHandle;
	#endif	// defined multispec_win || defined multispec_wx
		
}	// end "SetActiveDisplaySpecsHandle" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetAreaDescriptionOffsetVariables
//
//	Software purpose:	This routine sets up the variables within the area description
//							structure that relate to the application of offsets in the
//							field boundary information. This is needed when a image window
//							is associated to the project file but may not have the same
//							start line and column and/or be the same size as the project
//							base image file.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ListProjectData in SListData.cpp
//							ListResultsTrainTestFields in SListResults.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/19/1999
//	Revised By:			Larry L. Biehl			Date: 08/31/2011		

void SetAreaDescriptionOffsetVariables (
				AreaDescriptionPtr				areaDescriptionPtr,
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr)

{
								
			// Get offsets to use for lines and columns in case needed for project fields.
	
	if (gImageWindowInfoPtr->projectBaseImageFlag)	
		ClearAreaDescriptionOffsetVariables (areaDescriptionPtr);
		
	else	//	!gImageWindowInfoPtr->projectBaseImageFlag
		{
		areaDescriptionPtr->applyOffsetFlag = TRUE;
					
				// Note that if the image file is a geotiff/tiff file, then ignore
				// the startLine and startColumn offset if they are 1 in the image
				// file. tiff/geotiff does not have a tag to store this information so
				// 1 is just the default. If different than 1, then use the start line/
				// column since the user set the parameter.
					
		if ((gImageFileInfoPtr->format == kTIFFType || 
											gImageFileInfoPtr->format == kGeoTIFFType) &&
								gImageFileInfoPtr->startColumn == 1 && 
										gImageFileInfoPtr->startLine == 1)
			areaDescriptionPtr->applyOffsetFlag = FALSE;
		
		areaDescriptionPtr->columnOffset = 
								gProjectInfoPtr->startColumn - gImageFileInfoPtr->startColumn;
		areaDescriptionPtr->lineOffset = 
								gProjectInfoPtr->startLine - gImageFileInfoPtr->startLine;
		
		areaDescriptionPtr->maxNumberColumns = gImageWindowInfoPtr->maxNumberColumns;
		areaDescriptionPtr->maxNumberLines = gImageWindowInfoPtr->maxNumberLines;
		
		}	// end "else !gImageWindowInfoPtr->projectBaseImageFlag"
		
}	// end "SetAreaDescriptionOffsetVariables" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetChannelWindowVariables
//
//	Software purpose:	This routine sets up the global variables that are used to 
//							convert between line-column units and window units.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DrawShapeFiles in ArcView.cpp
//							PolygonSelection in SSelectionUtility.cpp
//							OutlineFieldsControl in SOutlineFields.cpp
//							ClearSelectionArea in SSelectionUtility.cpp
//							DrawSelectionArea in SSelectionUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/24/1992
//	Revised By:			Larry L. Biehl			Date: 11/13/2019

void SetChannelWindowVariables (
				SInt16								windowCode,
				WindowPtr							windowPtr,
				Boolean								coreGraphicsFlag)

{
	Handle								windowInfoHandle;
	
	
	windowInfoHandle = GetWindowInfoHandle (windowPtr);
	
	SetChannelWindowVariables (windowCode, windowInfoHandle, coreGraphicsFlag);
	
}	// end "SetChannelWindowVariables"

	

void SetChannelWindowVariables (
				SInt16								windowCode,
				Handle								windowInfoHandle,
				Boolean								coreGraphicsFlag)

{	
	double								magnification;
	Rect									offScreenRectangle;

	DisplaySpecsPtr					displaySpecsPtr;
	
	Handle								displaySpecsH;
	
	SInt32								numberChannels;
											
	
			// Check input handle.																
			
	if (windowInfoHandle != NULL)
		{
				// Get pointer to display specification.  We do not need to lock 	
				//	it here since no other routines are called							
		
		displaySpecsH = GetDisplaySpecsHandle (windowInfoHandle);
		displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsH);
	
		if (displaySpecsPtr != NULL)
			{
					// If Core Graphics being used, the magnification is already handled
					// when scaling for the grahics context.
					
			magnification = displaySpecsPtr->magnification;
			if (coreGraphicsFlag)
				magnification = 1;
			
			if (displaySpecsPtr->displayedLineStart > 0)
				{
						// Determine the number of side by side channels.
						
				gSideBySideChannels = displaySpecsPtr->numberChannels;
				if (displaySpecsPtr->displayType != kSideSideChannelDisplayType)
					gSideBySideChannels = 1;
				
						// Determine first channel that has at least part of it			
						// being displayed in the window.										
							
				gStartChannel = (SInt32)(displaySpecsPtr->origin[kHorizontal]/
															displaySpecsPtr->offscreenChannelWidth);
				
				if (displaySpecsPtr->displayType == kSideSideChannelDisplayType)
					{
					/*   
					if (windowCode == kToPrintWindow && 
													GetSelectionTypeCode (windowInfoHandle) != 0)
						gSideBySideChannels = gStartChannel + 1;
					*/	
					if (windowCode == kToClipboardWindow || windowCode == kToPrintWindow)
						{
						gStartChannel = 0;

						if (GetSelectionOffscreenRectangle (GetWindowPtr (windowInfoHandle), 
																		&offScreenRectangle))
							{
							if (offScreenRectangle.right - offScreenRectangle.left > 1 ||
										offScreenRectangle.bottom - offScreenRectangle.top > 1)
								{
										// A selected area exists and it is not the entire image.
										
								gStartChannel = offScreenRectangle.left/
														displaySpecsPtr->offscreenChannelWidth;
							
								numberChannels = 
									(offScreenRectangle.right - offScreenRectangle.left) /
															displaySpecsPtr->offscreenChannelWidth;
								numberChannels = MAX (numberChannels, 1);
								gSideBySideChannels = gStartChannel + numberChannels;

								}	// end "if (offScreenRectangle.right - ..."
								
							}	// end "if (GetSelectionTypeCode (windowInfoHandle) != 0)"

						}	// end "else if (windowCode == kToClipboardWindow || ..." 
					
					}	// end "if (displaySpecsPtr->displayType == ..."
														
						// Determine the draw interval between channels for the 			
						// current magnification.													
							
				gChannelWindowInterval =
						(SInt32)(displaySpecsPtr->offscreenChannelWidth *  magnification);
						
						// Determine the width of the image for one channel.
																	
				gChannelWindowWidth = gChannelWindowInterval;
				if (displaySpecsPtr->displayType == kSideSideChannelDisplayType)
					gChannelWindowWidth = 
							(SInt32)((displaySpecsPtr->offscreenChannelWidth - 2) *
																								magnification);
														
						// Determine the offset in window units for the first channel	
						// that has at least part of it being displayed, printed or copied.					
						
				gChannelWindowOffset = gStartChannel*gChannelWindowInterval;
				
						// Get the location of the first channel being displayed in		
						// window units.																
				
				gStartChannelWindow = 0;
				if (windowCode == kToImageWindow)		
					gStartChannelWindow = -(SInt32)(displaySpecsPtr->origin[kHorizontal] * 
																								magnification);
																		
				gStartChannelWindow += ::GetLegendWidthForWindow (windowInfoHandle);
				
				if (windowCode == kToImageWindow)
					gStartChannelWindow += gChannelWindowOffset;
				
				}	// end "if (displaySpecsPtr->displayedLineStart > 0)"
			
			else if (ImageWindowIsAvailable (windowInfoHandle))
				{
						// This implies that a vector or image overlay is drawn in the
						// image window. Set parameters assuming that the entire image
						// file connected with the window is available.
				
				gSideBySideChannels = 1;
				
				gStartChannel = 0;
				
				gChannelWindowInterval = (SInt32)(GetMaxNumberColumns (windowInfoHandle) *
																								magnification);
				
				gChannelWindowWidth = gChannelWindowInterval;
				
				gChannelWindowOffset = 0;
				
				gStartChannelWindow = ::GetLegendWidthForWindow (windowInfoHandle);
				
				}	// end "else if (ImageWindowIsAvailable (windowInfoHandle))"
			
			}	// end "if (displaySpecsPtr != NULL)"
		
		}	// end "if (windowInfoHandle != NULL)" 

}	// end "SetChannelWindowVariables" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetDestinationWindowParameters
//
//	Software purpose:	The purpose of this routine is to set some parameter relative
//							to the destination window. The destination window is defined
//							by windowCode.
//
//	Parameters in:		windowCode;		=1, kToImageWindow;
//												=2, kToPrintWindow
//												=3, kToClipboardWindow
//
//	Parameters out:	Saved orgin parameters
//
// Value Returned:	None
// 
// Called By:			DrawArcViewShapes in SArcView.cpp
//							OutlineFieldsControl in SOutlineFields.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/27/2001
//	Revised By:			Larry L. Biehl			Date: 06/19/2002	

void SetDestinationWindowParameters (
				SInt16								windowCode,
				WindowPtr							windowPtr)

{									
			// If the input window is the project image window then set			
			// clipping region project image window to the port rectangle		
			// minus scroll bars.															
	
	if (windowCode == kToImageWindow)
		{
		GetWindowClipRectangle (windowPtr, kImageArea, &gViewRect);
		ClipRect (&gViewRect);
		
		}	// end "if (windowCode == kToImageWindow)"  
		
}	// end "SetDestinationWindowParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetLCToWindowUnitVariables
//
//	Software purpose:	The purpose of this routine is to set some parameter relative
//							to converting from line/column units to window units. This is 
//							called before drawing selected areas and project fields in a 
//							window. The destination window is defined by windowCode.
//
//	Parameters in:		windowCode;		=1, kToImageWindow;
//												=2, kToPrintWindow
//												=3, kToClipboardWindow
//
//	Parameters out:	
//
// Value Returned:	None
// 
// Called By:			OutlineFieldsControl in SOutlineFields.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/19/2002
//	Revised By:			Larry L. Biehl			Date: 11/19/2019

void SetLCToWindowUnitVariables (
				Handle								windowInfoHandle,
				SInt16								windowCode, 
				Boolean								associatedImageFlag,
				LCToWindowUnitsVariablesPtr	lcToWindowUnitsVariablesPtr)

{	
	DisplaySpecsPtr					displaySpecsPtr;
	FileInfoPtr							fileInfoPtr;
	SelectionInfoPtr 					selectionInfoPtr;
	
	Handle								displaySpecsH = NULL,
											fileInfoHandle = NULL,
											selectionInfoHandle;
											
	SInt32								columnOffset,
											lineOffset;
											
	SInt32								xOrigin,
											yOrigin;
											
			// Check input handle.																
			
	if (windowInfoHandle != NULL)
		{
				// Get pointer to display specification.  We do not need to lock 	
				//	it here since no other routines are called							
		
		displaySpecsH = GetDisplaySpecsHandle (windowInfoHandle);
	
				// Get pointer to file specification.  We do not need to lock it	
				// here since no other routines are called								
				
		fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
		
		}	// end "if (windowInfoHandle != NULL)"
	
	if (windowInfoHandle != NULL && displaySpecsH != NULL && fileInfoHandle != NULL)
		{
		displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsH);
														
		lcToWindowUnitsVariablesPtr->magnification = displaySpecsPtr->magnification;
		/*
		#if defined multispec_win 		
			if (windowCode == kToPrintWindow)
				{
				WindowPtr windowPtr = GetWindowPtr (windowInfoHandle);
				lcToWindowUnitsVariablesPtr->magnification = 
																	windowPtr->m_printerTextScaling;

				}	// end "if (windowCode == kToPrintWindow)"
		#endif	// defined multispec_win 
		*/
				// Get the origin to be used.
				// If writing to the clipboard, then set the display offsets
				// correctly to allow for writing to output with origin at (0, 0).
				
		xOrigin = (SInt32)displaySpecsPtr->origin[kHorizontal];
		yOrigin = (SInt32)displaySpecsPtr->origin[kVertical];

		if (windowCode == kToClipboardWindow)
			{
		
					// Get handle to structure which contains the selection			
					// rectangle										
	
			selectionInfoHandle = GetSelectionInfoHandle (windowInfoHandle);
			selectionInfoPtr = (SelectionInfoPtr)
								GetHandlePointer (selectionInfoHandle);
							
			if (selectionInfoPtr != NULL)
				{			
				xOrigin = selectionInfoPtr->offScreenRectangle.left;
				yOrigin = selectionInfoPtr->offScreenRectangle.top;
						
				}	// end "if (selectionInfoPtr != NULL)" 

			}	// end "else if (windowCode == kToClipboardWindow)"              
	
		#if defined multispec_win || defined multispec_wx
			if (windowCode == kToImageWindow)
				{               
				xOrigin = 0;
				yOrigin = 0;
				
				}	// end "if (windowCode == kToImageWindow)"
		#endif	// defined multispec_win || defined multispec_wx
		 
		lcToWindowUnitsVariablesPtr->xOrigin = xOrigin;
		lcToWindowUnitsVariablesPtr->yOrigin = yOrigin;
		
				// Get the line and column offsets
		
		columnOffset = -(SInt32)displaySpecsPtr->displayedColumnStart;
		lineOffset = -(SInt32)displaySpecsPtr->displayedLineStart;
		
		if (associatedImageFlag)
			{
					// Adjust for any difference between start line and column 	
					// in	 base image file and start line and column in 			
					// associated image file.	
					//
					// Note that if the image file is a geotiff/tiff file, then ignore
					// the startLine and startColumn offset if they are 1 in the image
					// file. tiff/geotiff does not have a tag to store this information so
					// 1 is just the default. If different than 1, then use the start line/
					// column since the user set the parameter.
						
			fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);	
												
			if ((fileInfoPtr->format != kTIFFType && 
														fileInfoPtr->format != kGeoTIFFType) ||
						fileInfoPtr->startColumn != 1 || fileInfoPtr->startLine != 1)
				{
				columnOffset += gProjectInfoPtr->startColumn - fileInfoPtr->startColumn;
				lineOffset += gProjectInfoPtr->startLine - fileInfoPtr->startLine;
							
				}	// end "if ((fileInfoPtr->format != kTIFFType && ..."
			
			}	// end "if (associatedImageFlag)" 
		
		lcToWindowUnitsVariablesPtr->columnOffset = columnOffset;
		lcToWindowUnitsVariablesPtr->lineOffset = lineOffset;
		
				// Get the line and column intervals
		
		if (displaySpecsPtr->displayedColumnInterval > 0 &&
												displaySpecsPtr->displayedLineInterval > 0)
			{
			lcToWindowUnitsVariablesPtr->columnInterval =
														displaySpecsPtr->displayedColumnInterval;
			lcToWindowUnitsVariablesPtr->lineInterval =
														displaySpecsPtr->displayedLineInterval;
			
			}	// end "if (displaySpecsPtr->displayedColumnInterval > 0 && ..."
		
		else	// ...displayedColumnInterval == 0 || displayedLineInterval == 0
			{
					// This implies that the image is in the process of being drawn.
			
			lcToWindowUnitsVariablesPtr->columnInterval = 1;
			lcToWindowUnitsVariablesPtr->lineInterval = 1;
			
			}	// end "else	// ...displayedColumnInterval == 0 || ..."
						
				// Get the image left offset.  
				// Note that the legend in the Windows version is in its own
				// view. It is not the same as that used for the image. Therefore
				// the legend width is always 0 within the image view. However
				// if the legend width is being called for use in a edit copy
				// or print window, then the actual legend width needs to be
				// returned.
				
		lcToWindowUnitsVariablesPtr->imageLeftOffset = 
														GetLegendWidthForWindow (windowInfoHandle);

				// Get the image top offset.
						
		lcToWindowUnitsVariablesPtr->imageTopOffset = 0;

		if (windowCode == kToImageWindow)
			lcToWindowUnitsVariablesPtr->imageTopOffset = 
																GetImageTopOffset (windowInfoHandle);

      if (windowCode == kToClipboardWindow)
			lcToWindowUnitsVariablesPtr->imageTopOffset = 
																GetTitleHeight (windowInfoHandle);

      
		#if defined multispec_win 		
			if (windowCode == kToPrintWindow)
				{
				WindowPtr windowPtr = GetWindowPtr (windowInfoHandle);
				lcToWindowUnitsVariablesPtr->imageTopOffset =
								(SInt32)(windowPtr->m_printerTextScaling*
																GetTitleHeight (windowInfoHandle));
				}	// end "if (windowCode == kToPrintWindow)"
		#endif	// defined multispec_win 
		
		lcToWindowUnitsVariablesPtr->columnScrollOffset = 0;
		lcToWindowUnitsVariablesPtr->lineScrollOffset = 0;	
																
					// Get the window location for the start channel being viewed.
					
		lcToWindowUnitsVariablesPtr->channelWindowOffset = gChannelWindowOffset;
			            
		#if defined multispec_win || defined multispec_wx
			LongPoint 	scrollOffset;
			if (windowCode == kToPrintWindow || windowCode == kToClipboardWindow)
				{
				scrollOffset.v = 0;
				scrollOffset.h = 0;
				
				}	// end "if (windowCode == kToPrintWindow || ..."
			
			else	// if (windowCode != kToPrintWindow && ...
				GetScrollOffset (windowInfoHandle, &scrollOffset);
				
			lcToWindowUnitsVariablesPtr->columnScrollOffset = scrollOffset.h;
			lcToWindowUnitsVariablesPtr->lineScrollOffset = scrollOffset.v;
		#endif	// defined multispec_win || defined multispec_wx
		
		}	// end "if (windowInfoHandle != NULL && ..." 
		
	else	// windowInfoHandle == NULL || ...
		{
		lcToWindowUnitsVariablesPtr->xOrigin = 0;
		lcToWindowUnitsVariablesPtr->yOrigin = 0;
		
		lcToWindowUnitsVariablesPtr->columnInterval = 1;
		lcToWindowUnitsVariablesPtr->lineInterval = 1;
		
		lcToWindowUnitsVariablesPtr->columnOffset = 0;
		lcToWindowUnitsVariablesPtr->lineOffset = 0;
		
		lcToWindowUnitsVariablesPtr->magnification = 1;
		
		lcToWindowUnitsVariablesPtr->imageTopOffset = 0;
		lcToWindowUnitsVariablesPtr->imageLeftOffset = 0;
		
		lcToWindowUnitsVariablesPtr->channelWindowOffset = 0;	
		
		lcToWindowUnitsVariablesPtr->columnScrollOffset = 0;
		lcToWindowUnitsVariablesPtr->lineScrollOffset = 0;		
		
		}	// end "else windowInfoHandle == NULL || ..."

}	// end "SetLCToWindowUnitVariables" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetMapToWindowUnitVariables
//
//	Software purpose:	The purpose of this routine is to set some parameter relative
//							to converting from map units to window units. This is called
//							before drawing shape files in a window. The destination window 
//							is defined by windowCode.
//
//	Parameters in:		windowCode;		=1, kToImageWindow
//												=2, kToPrintWindow
//												=3, kToClipboardWindow
//
//							overlayCode;	=1, for image overlays
//												=2, for vector lines
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			DrawArcViewShapes in SArcView.cpp
//							OutlineFieldsControl in SOutlineFields.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/17/2002
//	Revised By:			Larry L. Biehl			Date: 11/02/2019	

void SetMapToWindowUnitsVariables (
				Handle								windowInfoHandle,
				SInt16								windowCode,
				SInt16								overlayCode,
				Boolean								coreGraphicsFlag,
				MapToWindowUnitsVariablesPtr	mapToWindowUnitsVariablesPtr)

{	
	double								xMapCoordinate11,
											xMapPixelSize,
											yMapCoordinate11,
											yMapPixelSize;	
											
	DisplaySpecsPtr					displaySpecsPtr;
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	SelectionInfoPtr 					selectionInfoPtr;
	
	Handle								displaySpecsH,
											mapProjectionHandle,
											selectionInfoHandle;
											
	SInt32								displayedColumnStart,
											displayedLineStart;
											
	UInt32								xOrigin,
											yOrigin;
											
			
	displaySpecsH = GetDisplaySpecsHandle (windowInfoHandle);
	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsH);						
			
	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);
	mapProjectionInfoPtr = 
							(MapProjectionInfoPtr)GetHandlePointer (mapProjectionHandle);
										
	if (displaySpecsPtr != NULL && mapProjectionInfoPtr != NULL)
		{
				// Set up polygon transformation coefficients if needed.
		
		GetCoefficientsVectorPointers (mapProjectionInfoPtr);
		mapToWindowUnitsVariablesPtr->polynomialOrder = 
							mapProjectionInfoPtr->planarCoordinate.polynomialOrder;
		mapToWindowUnitsVariablesPtr->easting2CoefficientsPtr = 
							mapProjectionInfoPtr->planarCoordinate.easting2CoefficientsPtr;	
		mapToWindowUnitsVariablesPtr->northing2CoefficientsPtr = 
							mapProjectionInfoPtr->planarCoordinate.northing2CoefficientsPtr;	
									
				// Get the magnification.
					                      
		mapToWindowUnitsVariablesPtr->magnification = displaySpecsPtr->magnification; 

				// Get the origin to be used.
				// If the writing to the clipboard, then set the display offsets	
				// correctly to allow for writing to output with origin at (0,0).
				
		xOrigin = (UInt32)displaySpecsPtr->origin[kHorizontal];
		yOrigin = (UInt32)displaySpecsPtr->origin[kVertical];

		if (windowCode == kToClipboardWindow)
			{
		
					// Get handle to structure which contains the selection			
					// rectangle										
	
			selectionInfoHandle = GetSelectionInfoHandle (windowInfoHandle);
			selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (selectionInfoHandle);
							
			if (selectionInfoPtr != NULL)
				{			
				xOrigin = selectionInfoPtr->offScreenRectangle.left;
				yOrigin = selectionInfoPtr->offScreenRectangle.top;
						
				}	// end "if (selectionInfoPtr != NULL)" 

			}	// end "else if (windowCode == kToClipboardWindow)"              
	
		#if defined multispec_win || defined multispec_wx
			if (windowCode == kToImageWindow)
				{               
				xOrigin = 0;
				yOrigin = 0;
				
				}	// end "if (windowCode == kToImageWindow)"
		#endif	// defined multispec_win || defined multispec_wx
		
		if (coreGraphicsFlag)
			{
			mapToWindowUnitsVariablesPtr->magnification = 1; 
			xOrigin = 0;
			yOrigin = 0;
			
			}	// end "if (coreGraphicsFlag)" 
			
		mapToWindowUnitsVariablesPtr->xOrigin = xOrigin;
		mapToWindowUnitsVariablesPtr->yOrigin = yOrigin;
			
		mapToWindowUnitsVariablesPtr->xMap11 = 
										mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
		mapToWindowUnitsVariablesPtr->yMap11 = 
										mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
		
				// Get info for map orientation angle.
		
		if (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle != 0)
			{
			mapToWindowUnitsVariablesPtr->useMapOrientationAngleFlag = TRUE;
			mapToWindowUnitsVariablesPtr->cosOrAngle = 
						cos (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle);
			mapToWindowUnitsVariablesPtr->sinOrAngle = 
						sin (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle);
				
			}	// end "if (...->planarCoordinate.mapOrientationAngle != 0)"
			
		else	// mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle == 0
			{
			mapToWindowUnitsVariablesPtr->useMapOrientationAngleFlag = FALSE;
			mapToWindowUnitsVariablesPtr->cosOrAngle = 0;
			mapToWindowUnitsVariablesPtr->sinOrAngle = 0;
				
			}	// end "else ...->planarCoordinate.mapOrientationAngle == 0"
		
				// Get the map coordinate for the left (x) part of image to be drawn.											
				
		displayedColumnStart = (SInt32)displaySpecsPtr->displayedColumnStart;
		xMapCoordinate11 = mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
		xMapPixelSize = mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;							
		
		displayedLineStart = (SInt32)displaySpecsPtr->displayedLineStart;
		yMapCoordinate11 = mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
		yMapPixelSize = mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
		
		mapToWindowUnitsVariablesPtr->mapXOrigin = xMapCoordinate11 + 
											((double)displayedColumnStart - 1.5) * xMapPixelSize;
																	
		xMapPixelSize *= displaySpecsPtr->displayedColumnInterval;
		mapToWindowUnitsVariablesPtr->mapXOrigin += xOrigin * xMapPixelSize;
		
		mapToWindowUnitsVariablesPtr->xMapPixelSize = xMapPixelSize;  
		
				// Get the map coordinate for the top (y) part of image to be drawn.	
		
		mapToWindowUnitsVariablesPtr->mapYOrigin = yMapCoordinate11 - 
											((double)displayedLineStart - 1.5) * yMapPixelSize;
																	
		yMapPixelSize *= displaySpecsPtr->displayedLineInterval;
		mapToWindowUnitsVariablesPtr->mapYOrigin -= yOrigin * yMapPixelSize;
		
		mapToWindowUnitsVariablesPtr->yMapPixelSize = -yMapPixelSize;
		
				// The following 4 variables are for map coordinates defined by a 
				// transformation polynomial.
		
		mapToWindowUnitsVariablesPtr->displayedColumnInterval = 
															displaySpecsPtr->displayedColumnInterval;
		mapToWindowUnitsVariablesPtr->displayedLineInterval = 
															displaySpecsPtr->displayedLineInterval;
		
		mapToWindowUnitsVariablesPtr->windowColumnOrigin = 
									displaySpecsPtr->displayedColumnStart + 
											xOrigin * displaySpecsPtr->displayedColumnInterval;
											
		mapToWindowUnitsVariablesPtr->windowLineOrigin = 
									displaySpecsPtr->displayedLineStart + 
											yOrigin * displaySpecsPtr->displayedLineInterval;
			
				// Get the image left offset.  
				// Note that the legend in the Windows version is in its on view. It is 
				// not the same as that used for the image. Therefore the legend width 
				// is always 0 within the image view. However if the legend width is 
				// being called for use in a edit copy or print window, then the actual 
				// legend width needs to be returned.
				
		mapToWindowUnitsVariablesPtr->imageLeftOffset = 
														GetLegendWidthForWindow (windowInfoHandle);

				// Get the image top offset.
						
		mapToWindowUnitsVariablesPtr->imageTopOffset = 0;
		if (!coreGraphicsFlag)
			{	
			if (windowCode == kToImageWindow)
				mapToWindowUnitsVariablesPtr->imageTopOffset = 
																GetImageTopOffset (windowInfoHandle);
			if (windowCode == kToClipboardWindow)
				mapToWindowUnitsVariablesPtr->imageTopOffset = 
																GetTitleHeight (windowInfoHandle);
																
			}	// end "if (!coreGraphicsFlag)"
																
				// Get the window location for the start channel being viewed.
					
		mapToWindowUnitsVariablesPtr->channelWindowOffset = gChannelWindowOffset;	
		
				// Set the offset to be used  
				
		mapToWindowUnitsVariablesPtr->offset = -.5;	
		if (overlayCode == kImageOverlay)	
			mapToWindowUnitsVariablesPtr->offset = 0.5;	
		
		}	// end "if (displaySpecsPtr != NULL && ..." 
		
	else	// displaySpecsPtr == NULL || ...
		{
		mapToWindowUnitsVariablesPtr->polynomialOrder = 0;
		mapToWindowUnitsVariablesPtr->easting2CoefficientsPtr = NULL;	
		mapToWindowUnitsVariablesPtr->northing2CoefficientsPtr = NULL;		
		
		mapToWindowUnitsVariablesPtr->xOrigin = 0;
		mapToWindowUnitsVariablesPtr->yOrigin = 0;
		
		mapToWindowUnitsVariablesPtr->mapXOrigin = 0;
		mapToWindowUnitsVariablesPtr->mapYOrigin = 0;
		
		mapToWindowUnitsVariablesPtr->xMapPixelSize = 1;
		mapToWindowUnitsVariablesPtr->yMapPixelSize = -1;
		
		mapToWindowUnitsVariablesPtr->magnification = 1;
		
		mapToWindowUnitsVariablesPtr->imageTopOffset = 0;
		mapToWindowUnitsVariablesPtr->imageLeftOffset = 0;
		
		mapToWindowUnitsVariablesPtr->channelWindowOffset = 0;	
		
		mapToWindowUnitsVariablesPtr->offset = 0;		
		
		mapToWindowUnitsVariablesPtr->cosOrAngle = 0;
		mapToWindowUnitsVariablesPtr->sinOrAngle = 0;	
		
		mapToWindowUnitsVariablesPtr->useMapOrientationAngleFlag = FALSE;
		
		mapToWindowUnitsVariablesPtr->displayedColumnInterval = 1;
		mapToWindowUnitsVariablesPtr->displayedLineInterval = 1;
		
		mapToWindowUnitsVariablesPtr->windowColumnOrigin = 1;
		mapToWindowUnitsVariablesPtr->windowLineOrigin = 1;
		
		}	// end "else displaySpecsPtr == NULL || ..."

}	// end "SetMapToWindowUnitVariables" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean SetUpActiveImageInformationGlobals
//
//	Software purpose:	The purpose of this routine is to lock the window
//							information, layer information and file information
//							handles for the active image window load the global
//							pointers for the information structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	-1 if handles were locked upon input. Pointers were
//											successfully loaded.
//							0 if pointers were not intialized. 
//							1 if pointers were successfully loaded.
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 08/30/1991
//	Revised By:			Larry L. Biehl			Date: 08/02/1996	

Boolean SetUpActiveImageInformationGlobals (
				Handle*								windowInfoHandlePtr,
				SInt16*								handleStatusPtr,
				Boolean								useProjectFlag)

{
	Boolean								returnFlag = FALSE;
	
	
	*windowInfoHandlePtr = GetActiveImageWindowInfoHandle ();
	if (*windowInfoHandlePtr == NULL && 
			useProjectFlag && 
				gProjectInfoPtr != NULL)
		{
				// Make certain the we have access to the file.						
				
		GetProjectImageFileInfo (kPrompt, kDoNotSetupGlobalInfoPointers);
		
		*windowInfoHandlePtr = gProjectInfoPtr->windowInfoHandle;
			
		}	// end "if (windowInfoHandle == NULL && ..."
	
	returnFlag = GetImageInformationPointers (handleStatusPtr, 
															*windowInfoHandlePtr,
															&gImageWindowInfoPtr, 
															&gImageLayerInfoPtr, 
															&gImageFileInfoPtr);
								
	return (returnFlag);
				
}	// end "SetUpActiveImageInformationGlobals"  


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetUpAreaUnitsPopUpMenu
//
//	Software purpose:	The purpose of this routine is to set up the area units
//							popup menu according to the map information available.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/22/2000
//	Revised By:			Larry L. Biehl			Date: 07/28/2017
void SetUpAreaUnitsPopUpMenu (
				MenuHandle							menuHandle,
				Handle								windowInfoHandle,
				#ifndef multispec_wx
					DialogPtr							dialogPtr)
				#else
					wxPanel*								dialogPtr)
				#endif
{ 
	SInt16							planarMapUnitsCode;
		
	planarMapUnitsCode = GetFilePlanarMapUnitsCode (windowInfoHandle);
		
	#if defined multispec_mac
		EnableMenuItem (menuHandle, kNumberPixelsUnitsMenuItem);
		
		if (planarMapUnitsCode == kUnknownCode)
			EnableMenuItem (menuHandle, kAreaUnknownUnitsMenuItem);
		
		else	// planarMapUnitsCode != kUnknownCode
			DisableMenuItem (menuHandle, kAreaUnknownUnitsMenuItem);
		
		if (planarMapUnitsCode >= kKilometersCode)
			{
			EnableMenuItem (menuHandle, kSqKilometersUnitsMenuItem);
			EnableMenuItem (menuHandle, kHectareUnitsMenuItem);
			EnableMenuItem (menuHandle, kSqMetersUnitsMenuItem);
			EnableMenuItem (menuHandle, kSqCentimetersUnitsMenuItem);
			EnableMenuItem (menuHandle, kSqMillimetersUnitsMenuItem);
			EnableMenuItem (menuHandle, kSqMicrometersUnitsMenuItem);
			EnableMenuItem (menuHandle, kSqMilesUnitsMenuItem);
			EnableMenuItem (menuHandle, kAcresUnitsMenuItem);
			EnableMenuItem (menuHandle, kSqYardsUnitsMenuItem);
			EnableMenuItem (menuHandle, kSqFeetUnitsMenuItem);
			EnableMenuItem (menuHandle, kSqInchesUnitsMenuItem);
			
			}	// end "if (planarMapUnitsCode >= kKilometersCode)"
		
		else	// planarMapUnitsCode < kKilometersCode
			{
			DisableMenuItem (menuHandle, kSqKilometersUnitsMenuItem);
			DisableMenuItem (menuHandle, kHectareUnitsMenuItem);
			DisableMenuItem (menuHandle, kSqMetersUnitsMenuItem);
			DisableMenuItem (menuHandle, kSqCentimetersUnitsMenuItem);
			DisableMenuItem (menuHandle, kSqMillimetersUnitsMenuItem);
			DisableMenuItem (menuHandle, kSqMicrometersUnitsMenuItem);
			DisableMenuItem (menuHandle, kSqMilesUnitsMenuItem);
			DisableMenuItem (menuHandle, kAcresUnitsMenuItem);
			DisableMenuItem (menuHandle, kSqYardsUnitsMenuItem);
			DisableMenuItem (menuHandle, kSqFeetUnitsMenuItem);
			DisableMenuItem (menuHandle, kSqInchesUnitsMenuItem);
			
			}	// end "else planarMapUnitsCode < kKilometersCode"
	#endif	// defined multispec_mac 
	
	#if defined multispec_win
		SInt16							index = 0,
											numberComboItems;

			
		CComboBox* comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem (IDC_AreaUnitsCombo);

				// Remove all but the first item in the list.

		numberComboItems = comboBoxPtr->GetCount ();
		while (numberComboItems > 1)
			{
			comboBoxPtr->DeleteString (numberComboItems-1);
			numberComboItems--;

			}	// end "while (numberComboItems > 1)"
		
		comboBoxPtr->SetItemData (index, kNumberPixelsUnitsMenuItem);
		index++;
		
		if (planarMapUnitsCode == kUnknownCode)
			{
			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Unknown units"));
			comboBoxPtr->SetItemData (index, kAreaUnknownUnitsMenuItem);
			index++;

			}	// end "if (planarMapUnitsCode == kUnknownCode)"
		
		if (planarMapUnitsCode >= kKilometersCode)
			{
			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Sq. kilometers"));
			comboBoxPtr->SetItemData (index, kSqKilometersUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Hectares"));
			comboBoxPtr->SetItemData (index, kHectareUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Sq. meters"));
			comboBoxPtr->SetItemData (index, kSqMetersUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Sq. centimeters"));
			comboBoxPtr->SetItemData (index, kSqCentimetersUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Sq. millimeters"));
			comboBoxPtr->SetItemData (index, kSqMillimetersUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Sq. micrometers"));
			comboBoxPtr->SetItemData (index, kSqMicrometersUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Sq. miles"));
			comboBoxPtr->SetItemData (index, kSqMilesUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Acres"));
			comboBoxPtr->SetItemData (index, kAcresUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Sq. yards"));
			comboBoxPtr->SetItemData (index, kSqYardsUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Sq. US survey feet"));
			comboBoxPtr->SetItemData (index, kSqUSSurveyFeetUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Sq. feet"));
			comboBoxPtr->SetItemData (index, kSqFeetUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Sq. inches"));
			comboBoxPtr->SetItemData (index, kSqInchesUnitsMenuItem);
			index++;
			
			}	// end "if (planarMapUnitsCode >= kKilometersCode)"
	#endif	// defined multispec_win
			
	#if defined multispec_wx
		SInt16	index = 0,
					numberComboItems;


		#if defined multispec_wxlin
			wxComboBox* areaUnitsCtrl =
							(wxComboBox*)dialogPtr->FindWindow (IDC_AreaUnitsCombo);
		#endif
		#if defined multispec_wxmac
			wxChoice* areaUnitsCtrl =
							(wxChoice*)dialogPtr->FindWindow (IDC_AreaUnitsCombo);
		#endif

				// Remove all but the first item in the list.

		numberComboItems = areaUnitsCtrl->GetCount ();
		while (numberComboItems > 1) 
			{
			areaUnitsCtrl->Delete (numberComboItems-1);
			numberComboItems--;

			}	// end "while (numberComboItems > 1)"

		areaUnitsCtrl->SetClientData (index, (void*)(SInt64)gNumberPixelsUnitsMenuItem);
		index++;

		if (planarMapUnitsCode == kUnknownCode) 
			{
			areaUnitsCtrl->Append (wxT("Unknown units"));
			areaUnitsCtrl->SetClientData (index, (void*)(SInt64)gAreaUnknownUnitsMenuItem);
			index++;

			}	// end "if (planarMapUnitsCode == kUnknownCode)"

		if (planarMapUnitsCode >= kKilometersCode) 
			{
			areaUnitsCtrl->Append (wxT("Sq. kilometers"));
			areaUnitsCtrl->SetClientData (index, (void*)(SInt64)gSqKilometersUnitsMenuItem);
			index++;

			areaUnitsCtrl->Append (wxT("Hectares"));
			areaUnitsCtrl->SetClientData (index, (void*)(SInt64)gHectareUnitsMenuItem);
			index++;

			areaUnitsCtrl->Append (wxT("Sq. meters"));
			areaUnitsCtrl->SetClientData (index, (void*)(SInt64)gSqMetersUnitsMenuItem);
			index++;

			areaUnitsCtrl->Append (wxT("Sq. centimeters"));
			areaUnitsCtrl->SetClientData (index, (void*)(SInt64)gSqCentimetersUnitsMenuItem);
			index++;

			areaUnitsCtrl->Append (wxT("Sq. millimeters"));
			areaUnitsCtrl->SetClientData (index, (void*)(SInt64)gSqMillimetersUnitsMenuItem);
			index++;

			areaUnitsCtrl->Append (wxT("Sq. micrometers"));
			areaUnitsCtrl->SetClientData (index, (void*)(SInt64)gSqMicrometersUnitsMenuItem);
			index++;

			areaUnitsCtrl->Append (wxT("Sq. miles"));
			areaUnitsCtrl->SetClientData (index, (void*)(SInt64)gSqMilesUnitsMenuItem);
			index++;

			areaUnitsCtrl->Append (wxT("Acres"));
			areaUnitsCtrl->SetClientData (index, (void*)(SInt64)gAcresUnitsMenuItem);
			index++;

			areaUnitsCtrl->Append (wxT("Sq. yards"));
			areaUnitsCtrl->SetClientData (index, (void*)(SInt64)gSqYardsUnitsMenuItem);
			index++;

			areaUnitsCtrl->Append (wxT("Sq. feet"));
			areaUnitsCtrl->SetClientData (index, (void*)(SInt64)gSqFeetUnitsMenuItem);
			index++;

			areaUnitsCtrl->Append (wxT("Sq. inches"));
			areaUnitsCtrl->SetClientData (index, (void*)(SInt64)gSqInchesUnitsMenuItem);
			index++;

			}	// end "if (planarMapUnitsCode >= kKilometersCode)"
	#endif	// defined multispec_wx

}	// end "SetUpAreaUnitsPopUpMenu"  


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetUpCoordinateUnitsPopUpMenu
//
//	Software purpose:	The purpose of this routine is to set up the coordinate units
//							popup menu according to the map information available.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/06/2000
//	Revised By:			Larry L. Biehl			Date: 07/28/2017

void SetUpCoordinateUnitsPopUpMenu (
				MenuHandle							menuHandle,
				Handle								windowInfoHandle,
				#ifndef multispec_wx
					DialogPtr							dialogPtr)
				#else
					wxPanel*								dialogPtr)
				#endif
{    
	SInt16								planarMapUnitsCode;
										
	Boolean								enableLatLongUnitsMenuItemFlag;
	
	
	planarMapUnitsCode = GetFilePlanarMapUnitsCode (windowInfoHandle);

	enableLatLongUnitsMenuItemFlag = DetermineIfLatLongPossible (windowInfoHandle);
			
	#if defined multispec_wx
		SInt16								index = 0,
												numberComboItems;


		wxChoice* displayUnitsCtrl =
								(wxChoice*)dialogPtr->FindWindow (IDC_DisplayUnitsCombo);

				// Remove all but the first item in the list.

		numberComboItems = displayUnitsCtrl->GetCount ();
		
		while (numberComboItems > 1) 
			{
			displayUnitsCtrl->Delete (numberComboItems - 1);
			numberComboItems--;

			}	// end "while (numberComboItems > 1)"

		displayUnitsCtrl->SetClientData (
												index, (void*)(SInt64)gLineColumnUnitsMenuItem);
		index++;

		if (planarMapUnitsCode == kUnknownCode) 
			{
			displayUnitsCtrl->Append (wxT("Unknown units"));
			displayUnitsCtrl->SetClientData (
												index, (void*)(SInt64)gUnknownUnitsMenuItem);
			index++;

			}	// end "if (planarMapUnitsCode == kUnknownCode)"

		if (enableLatLongUnitsMenuItemFlag) 
			{
			displayUnitsCtrl->Append (wxT("Lat-Long (Decimal)"));
			displayUnitsCtrl->SetClientData (
											index, (void*)(SInt64)gDecimalLatLongUnitsMenuItem);
			index++;

			displayUnitsCtrl->Append (wxT("Lat-Long (DMS)"));
			displayUnitsCtrl->SetClientData (
												index, (void*)(SInt64)gDMSLatLongUnitsMenuItem);
			index++;

			}	// end "if (enableLatLongUnitsMenuItemFlag)"

		if (planarMapUnitsCode >= kKilometersCode) 
			{
			displayUnitsCtrl->Append (wxT("Kilometers"));
			displayUnitsCtrl->SetClientData (
												index, (void*)(SInt64)gKilometersUnitsMenuItem);
			index++;

			displayUnitsCtrl->Append (wxT("Meters"));
			displayUnitsCtrl->SetClientData (index, (void*)(SInt64)gMetersUnitsMenuItem);
			index++;

			displayUnitsCtrl->Append (wxT("Centimeters"));
			displayUnitsCtrl->SetClientData (
												index, (void*)(SInt64)gCentimetersUnitsMenuItem);
			index++;

			displayUnitsCtrl->Append (wxT("Millimeters"));
			displayUnitsCtrl->SetClientData (
												index, (void*)(SInt64)gMillimetersUnitsMenuItem);
			index++;

			displayUnitsCtrl->Append (wxT("Micrometers"));
			displayUnitsCtrl->SetClientData (
												index, (void*)(SInt64)gMicrometersUnitsMenuItem);
			index++;

			displayUnitsCtrl->Append (wxT("Miles"));
			displayUnitsCtrl->SetClientData (index, (void*)(SInt64)gMilesUnitsMenuItem);
			index++;

			displayUnitsCtrl->Append (wxT("Yards"));
			displayUnitsCtrl->SetClientData (index, (void*)(SInt64)gYardsUnitsMenuItem);
			index++;

			displayUnitsCtrl->Append (wxT("Feet"));
			displayUnitsCtrl->SetClientData (index, (void*)(SInt64)gFeetUnitsMenuItem);
			index++;

			displayUnitsCtrl->Append (wxT("Inches"));
			displayUnitsCtrl->SetClientData (index, (void*)(SInt64)gInchesUnitsMenuItem);
			index++;

			}	// end "if (planarMapUnitsCode >= kKilometersCode)"
	#endif	// defined multispec_wx

	#if defined multispec_mac
		EnableMenuItem (menuHandle, kLineColumnUnitsMenuItem);
		
		if (planarMapUnitsCode == kUnknownCode)
			EnableMenuItem (menuHandle, kUnknownUnitsMenuItem);
		
		else	// planarMapUnitsCode != kUnknownCode
			DisableMenuItem (menuHandle, kUnknownUnitsMenuItem);
			
		if (enableLatLongUnitsMenuItemFlag)
			{
			EnableMenuItem (menuHandle, kDecimalLatLongUnitsMenuItem);
			EnableMenuItem (menuHandle, kDMSLatLongUnitsMenuItem);
			
			}	// end "if (enableLatLongUnitsMenuItemFlag)"
			
		else	// !enableLatLongUnitsMenuItemFlag
			{
			DisableMenuItem (menuHandle, kDecimalLatLongUnitsMenuItem);
			DisableMenuItem (menuHandle, kDMSLatLongUnitsMenuItem);
			
			}	// end "else !enableLatLongUnitsMenuItemFlag"
		
		if (planarMapUnitsCode >= kKilometersCode)
			{
			EnableMenuItem (menuHandle, kKilometersUnitsMenuItem);
			EnableMenuItem (menuHandle, kMetersUnitsMenuItem);
			EnableMenuItem (menuHandle, kCentimetersUnitsMenuItem);
			EnableMenuItem (menuHandle, kMillimetersUnitsMenuItem);
			EnableMenuItem (menuHandle, kMicrometersUnitsMenuItem);
			EnableMenuItem (menuHandle, kMilesUnitsMenuItem);
			EnableMenuItem (menuHandle, kYardsUnitsMenuItem);
			EnableMenuItem (menuHandle, kUSSurveyFeetUnitsMenuItem);
			EnableMenuItem (menuHandle, kFeetUnitsMenuItem);
			EnableMenuItem (menuHandle, kInchesUnitsMenuItem);
			
			}	// end "if (planarMapUnitsCode >= kMetersCode)"
		
		else	// planarMapUnitsCode < kKilometersCode
			{
			DisableMenuItem (menuHandle, kKilometersUnitsMenuItem);
			DisableMenuItem (menuHandle, kMetersUnitsMenuItem);
			DisableMenuItem (menuHandle, kCentimetersUnitsMenuItem);
			DisableMenuItem (menuHandle, kMillimetersUnitsMenuItem);
			DisableMenuItem (menuHandle, kMicrometersUnitsMenuItem);
			DisableMenuItem (menuHandle, kMilesUnitsMenuItem);
			DisableMenuItem (menuHandle, kYardsUnitsMenuItem);
			DisableMenuItem (menuHandle, kUSSurveyFeetUnitsMenuItem);
			DisableMenuItem (menuHandle, kFeetUnitsMenuItem);
			DisableMenuItem (menuHandle, kInchesUnitsMenuItem);
			
			}	// end "else planarMapUnitsCode < kMetersCode"
	#endif	// defined multispec_mac 
	
	#if defined multispec_win
		SInt16								index = 0,
												numberComboItems;

			
		CComboBox* comboBoxPtr = 
								(CComboBox*)dialogPtr->GetDlgItem (IDC_DisplayUnitsCombo);

				// Remove all but the first item in the list.

		numberComboItems = comboBoxPtr->GetCount ();
		while (numberComboItems > 1)
			{
			comboBoxPtr->DeleteString (numberComboItems-1);
			numberComboItems--;

			}	// end "while (numberComboItems > 1)"
		
		comboBoxPtr->SetItemData (index, kLineColumnUnitsMenuItem);
		index++;
		
		if (planarMapUnitsCode == kUnknownCode)
			{
			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Unknown units"));
			comboBoxPtr->SetItemData (index, kUnknownUnitsMenuItem);
			index++;

			}	// end "if (planarMapUnitsCode == kUnknownCode)"
		
		if (enableLatLongUnitsMenuItemFlag)
			{
			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Lat-Long (Decimal)"));
			comboBoxPtr->SetItemData (index, kDecimalLatLongUnitsMenuItem);
			index++;
			
			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Lat-Long (DMS)"));
			comboBoxPtr->SetItemData (index, kDMSLatLongUnitsMenuItem);
			index++;
			
			}	// end "if (enableLatLongUnitsMenuItemFlag)"
		
		if (planarMapUnitsCode >= kKilometersCode)
			{
			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Kilometers"));
			comboBoxPtr->SetItemData (index, kKilometersUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Meters"));
			comboBoxPtr->SetItemData (index, kMetersUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Centimeters"));
			comboBoxPtr->SetItemData (index, kCentimetersUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Millimeters"));
			comboBoxPtr->SetItemData (index, kMillimetersUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Micrometers"));
			comboBoxPtr->SetItemData (index, kMicrometersUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Miles"));
			comboBoxPtr->SetItemData (index, kMilesUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Yards"));
			comboBoxPtr->SetItemData (index, kYardsUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("US Survey Feet"));
			comboBoxPtr->SetItemData (index, kUSSurveyFeetUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Feet"));
			comboBoxPtr->SetItemData (index, kFeetUnitsMenuItem);
			index++;

			comboBoxPtr->InsertString (index, (LPCTSTR)_T("Inches"));
			comboBoxPtr->SetItemData (index, kInchesUnitsMenuItem);
			index++;
			
			}	// end "if (planarMapUnitsCode >= kKilometersCode)"
	#endif	// defined multispec_win 

}	// end "SetUpCoordinateUnitsPopUpMenu"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		short int Swap2Bytes
//
//	Software purpose:	This routine swaps the bytes in the input short integer value.
//
//	Parameters in:		short int value.
//
//	Parameters out:	none.
//
// Value Returned:	short int value with the bytes swapped.
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/16/1994
//	Revised By:			Larry L. Biehl			Date: 10/26/1995

SInt16 Swap2Bytes (
				SInt16 								inputValue)

{
	return (((inputValue & 0xff00) >> 8) || ((inputValue & 0x00ff) << 8));
	
}	// end "Swap2Bytes" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UnlockActiveImageInformationGlobals
//
//	Software purpose:	The purpose of this routine is to unlock the window
//							information, layer information and file information
//							handles for the active image window set the global
//							pointers for those information structures to NULL.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/30/1991
//	Revised By:			Larry L. Biehl			Date: 08/05/1996	

void UnlockActiveImageInformationGlobals (
				SInt16								handleStatus)
				
{
	Handle windowInfoHandle = GetActiveImageWindowInfoHandle ();

	UnlockImageInformationHandles (handleStatus, 
												windowInfoHandle);
	 
	if (handleStatus & 0x0001)
		gImageWindowInfoPtr = NULL;
		
	if (handleStatus & 0x0002)
		gImageLayerInfoPtr = NULL;
	
	if (handleStatus & 0x0004)
		gImageFileInfoPtr = NULL;
		
}	// end "UnlockActiveImageInformationGlobals" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UnlockImageInformationHandles
//
//	Software purpose:	The purpose of this routine is to unlock the window
//							information, layer information and file information
//							handles for input image window handle and set the 
//							input pointers for those information structures to NULL.
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
//	Revised By:			Larry L. Biehl			Date: 01/11/1996	

void UnlockImageInformationHandles (
				SInt16								handleStatus, 
				Handle								windowInfoHandle)

{
	if (windowInfoHandle != NULL)
		{
		WindowInfoPtr windowInfoPtr = 
											(WindowInfoPtr)GetHandlePointer (windowInfoHandle);
		if (handleStatus & 0x0002)
			{
			Handle layerInfoHandle = GetLayerInfoHandle (windowInfoPtr);
			CheckAndUnlockHandle (layerInfoHandle);
			
			}	// end "if (handleStatus & 0x0002)"
		
		if (handleStatus & 0x0004)
			{
			Handle fileInfoHandle = GetFileInfoHandle (windowInfoPtr);
			CheckAndUnlockHandle (fileInfoHandle);
			
			}	// end "if (handleStatus & 0x0004)"
			
		if (handleStatus & 0x0001)
			CheckAndUnlockHandle (windowInfoHandle);
			
		}	// end "if (windowInfoHandle != NULL)" 
				
}	// end "UnlockImageInformationHandles"                              



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateActiveImageWScrolls
//
//	Software purpose:	This routine will pause execution until MultiSpec is
//							in the foreground.  This is used before the standard
//							file routines and alerts are posted.  This approach
//							is used until the Notification Manager routines can
//							be implemented.
//
//	Parameters in:		magnification		
//
//	Parameters out:	None			
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/15/1995
//	Revised By:			Larry L. Biehl			Date: 03/14/2019

void UpdateActiveImageWScrolls (
				double								magnification)
													
{
	#if defined multispec_wx
		gActiveImageViewCPtr->UpdateScrolls (magnification);
		//gActiveImageViewCPtr->OnUpdate (gActiveImageViewCPtr, NULL);
	#endif	// defined multispec_wx
	
	#if defined multispec_mac
		UpdateImageWScrolls (gActiveImageWindow);
	#endif	// defined multispec_mac
	
	#if defined multispec_win
		gActiveImageViewCPtr->UpdateScrolls (magnification);
		gActiveImageViewCPtr->OnUpdate (gActiveImageViewCPtr, NULL);
	#endif	// defined multispec_win
	
}	// end "UpdateActiveImageWScrolls"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateMapProjectionStructure
//
//	Software purpose:	The purpose of this routine is to update parameters in the map 
//							projection structure based on the input arguments.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			UpdateOutputFileStructure in SReformatUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/17/2000
//	Revised By:			Larry L. Biehl			Date: 02/24/2007

void UpdateMapProjectionStructure (
				Handle								inputMapProjectionHandle,
				FileInfoPtr							outFileInfoPtr,
				SInt32								inputStartLine, 
				SInt32								inputStartColumn,
				SInt32								lineInterval,
				SInt32								columnInterval,
				Boolean								rightToLeftFlag,
				Boolean								bottomToTopFlag)

{
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	
	if (inputMapProjectionHandle != NULL && outFileInfoPtr != NULL)
		{
				// Put copy of the input map projection structure in the output
				// map projection structure.
				
		CopyMapProjectionHandle (inputMapProjectionHandle,
											&outFileInfoPtr->mapProjectionHandle);
								
		mapProjectionInfoPtr = (MapProjectionInfoPtr)
						GetHandlePointer (outFileInfoPtr->mapProjectionHandle);
     
		if (mapProjectionInfoPtr != NULL)
     		{
			mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 += 
				(outFileInfoPtr->startColumn - inputStartColumn) * 
								mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
                                                                           
			mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 -= 
				(outFileInfoPtr->startLine - inputStartLine)  * 
								mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
			
			mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize *= columnInterval;
			
			mapProjectionInfoPtr->planarCoordinate.verticalPixelSize *= lineInterval;
															
			if (rightToLeftFlag)
				{
				mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 +=
						(outFileInfoPtr->numberColumns - 1) *
									mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
				
				
				mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = 
							-mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
							
				}	// end "if (rightToLeftFlag)"
															
			if (bottomToTopFlag)
				{
				mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 -=
						(outFileInfoPtr->numberLines - 1) *
									mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
									
				mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = 
							-mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
							
				}	// end "if (bottomToTopFlag)"
                                                                            
			}	// end "if (mapProjectionInfoPtr != NULL)"
			
		}	// end "if (inputMapProjectionHandle != NULL && outFileInfoPtr != NULL)"
				 										
}	// end "UpdateMapProjectionStructure"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdatePlanarCoordinateInfo
//
//	Software purpose:	The purpose of this routine is to update parameters in the planar
//							coordinate info structure relative to the input map information 
//							and the information on the subset of the image for the input
//							map information.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/13/2003
//	Revised By:			Larry L. Biehl			Date: 03/30/2007

void UpdatePlanarCoordinateInfo (
				Handle								inputMapProjectionHandle,
				PlanarCoordinateSystemInfo*	planarCoordinatePtr,
				SInt32								newStartColumn,
				SInt32								newStartLine, 
				SInt32								numberColumns,
				SInt32								numberLines,
				SInt32								columnInterval,
				SInt32								lineInterval,
				Boolean								rightToLeftFlag,
				Boolean								bottomToTopFlag)

{
//	DoublePoint							mapPoint;
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	
	if (inputMapProjectionHandle != NULL && planarCoordinatePtr != NULL)
		{
				// Put copy of the input map projection structure in the output
				// map projection structure.
				
		mapProjectionInfoPtr = (MapProjectionInfoPtr)
						GetHandlePointer (inputMapProjectionHandle);
     
		if (mapProjectionInfoPtr != NULL)
     		{                                                                      
			planarCoordinatePtr->xMapOrientationOrigin = 
							mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin;                        
			planarCoordinatePtr->yMapOrientationOrigin = 
							mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin;
			                                
			planarCoordinatePtr->xMapCoordinate11 = 
					mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 +
							(newStartColumn - 1) * 
								mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
                                                                       
			planarCoordinatePtr->yMapCoordinate11 =                                     
					mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 - 
							(newStartLine - 1)  * 
								mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
			                               
			planarCoordinatePtr->horizontalPixelSize = columnInterval * 
								mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
			                               
			planarCoordinatePtr->verticalPixelSize = lineInterval * 
								mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
			                               
			planarCoordinatePtr->mapOrientationAngle = 
								mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle;
								
			planarCoordinatePtr->polynomialOrder = 
								mapProjectionInfoPtr->planarCoordinate.polynomialOrder;

					// The following is currently not used. May be if do overlays for
					// some of reformat processors.
															
			if (rightToLeftFlag)
				{
				planarCoordinatePtr->xMapCoordinate11 +=
					(numberColumns - 1) * planarCoordinatePtr->horizontalPixelSize;
				
				
				planarCoordinatePtr->horizontalPixelSize = 
													-planarCoordinatePtr->horizontalPixelSize;
							
				}	// end "if (rightToLeftFlag)"
															
			if (bottomToTopFlag)
				{
				planarCoordinatePtr->yMapCoordinate11 -=
						(numberLines - 1) * planarCoordinatePtr->verticalPixelSize;
									
				planarCoordinatePtr->verticalPixelSize = 
													-planarCoordinatePtr->verticalPixelSize;
							
				}	// end "if (bottomToTopFlag)"
			/*				
			if (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle != 0)
				{
						// Correct the map position of the point to take into account
						// the map orientation angle in the image.
						
				mapPoint.h = planarCoordinatePtr->xMapCoordinate11;
				mapPoint.v = planarCoordinatePtr->yMapCoordinate11;
				
				ConvertMapPointToOrientationAngleMapPoint (
									mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle, 
									mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11, 
									mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11,
									&mapPoint);
						
				planarCoordinatePtr->xMapCoordinate11 = mapPoint.h;
				planarCoordinatePtr->yMapCoordinate11 = mapPoint.v;
						
				}	// end "if (...->planarCoordinate.mapOrientationAngle != 0)"
			*/                                                                            
			}	// end "if (mapProjectionInfoPtr != NULL)"
			
		}	// end "if (inputMapProjectionHandle != NULL && planarCoordinatePtr != NULL)"
				 										
}	// end "UpdatePlanarCoordinateInfo" 
