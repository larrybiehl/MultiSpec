//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2019)
//						(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SSelectionUtility.cpp
//
//	Authors:					Larry L. Biehl, Ravi Budruk
//
//	Revision date:			01/16/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//								
//	Brief description:	The purpose of the routines in this file are utilies
//								to support selection area. These routines are shared
//								between the Mac and Windows versions.
//
//	Functions in file:	void						ClearNewFieldList
//								void 						ClearSelectionArea
//								void 						ClosePolygonSelection
//								void 						ComputeSelectionCoordinates
//								void 						ComputeSelectionOffscreenRectangle
//								void 						DrawSelectionArea
//								void 						DrawSelectionPolygon
//								void 						DrawSelectionRectangle
//								Boolean 					EditSelectionDialog
//								Boolean 					EditCoordinatesDialog
//								Boolean 					EditCoordinatesDialogOK
//								void 						GetBoundingSelectionRectangles
//								SInt64 					GetNumberPixelsInSelection
//								Boolean 					GetSelectedAreaInfo
//								Boolean 					GetSelectedOffscreenRectangle
//								Boolean 					GetSelectionCoordinates
//								Boolean 					GetSelectionOffscreenRectangle
//								Boolean 					GetSelectionRectangle
//								SInt16 	 				GetSelectionRectangleLimits
//								SInt16 					GetSelectionTypeCode
//								Boolean 					InitializePolygonSelection	
//								void						OutlineSelectionArea	
//								void 						SetSelectionForAllWindows
//								void 						SetSelectionInformation
//								void 						ShowGraphSelection
//                     
/*
	int numberChars = sprintf ((char*)gTextString3,
			" SSelectionUtility.cpp: (): %s",
			gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"      

#if defined multispec_lin
	#include "SMultiSpec.h"

	#include "LDrawObjects.h"
	#include "LEditSelectionDialog.h"
	#include "LGraphView.h"
	#include "LImageView.h"
#endif	// defined multispec_lin

#if defined multispec_mac || defined multispec_mac_swift
	#include "MGraphView.h"
	#define		IDC_NewLineStart			3 
	#define		IDC_NewLineEnd				4 
	#define		IDC_NewColumnStart		5 
	#define		IDC_NewColumnEnd			6 
	#define		IDC_ApplyToAllCheckbox	7
	#define		IDC_Preview					8
	#define		IDC_NewLineDash			13
	#define		IDC_NewColumnDash			14

	#define		IDC_CurrentLineStart		15 
	#define		IDC_CurrentLineEnd		16 
	#define		IDC_CurrentColumnStart	17 
	#define		IDC_CurrentColumnEnd		18
	#define		IDC_CurrentLineDash		19
	#define		IDC_CurrentColumnDash	20  

	#define		IDC_StartLCCheckBox		23   

	#define		IDS_Alert130				130
	#define		IDS_Alert131				131
	#define		IDS_Alert132				132
	#define		IDS_Alert133				133  
#endif	// defined multispec_mac || defined multispec_mac_swift   

#if defined multispec_win
	#include "CProcessor.h"
	#include	"CImageWindow.h"

	#include "WDrawObjects.h"
	#include "WEditSelectionDialog.h"
	#include "WGraphView.h"
	#include "WImageView.h"
	#include "WImageDoc.h"	
#endif	// defined multispec_win

//#include	"SExtGlob.h"				



extern SInt16 EditLineColumnDialogCheckCoordinates (
				DialogPtr							dialogPtr,
				Handle								inputWindowInfoHandle,
				SInt16								selectionDisplayUnits,
				LongRect*							selectionRectanglePtr,
				LongRect*							minMaxSelectionRectanglePtr,
				DoubleRect*							coordinateRectanglePtr,
				DoubleRect*							minMaxCoordinateRectanglePtr,
				double								newColumnStart,
				double								newColumnEnd,
				double								newLineStart,
				double								newLineEnd);

void ComputeMapCoordinates (
				Handle								windowInfoHandle,
				double								factor,
				SInt16								viewUnits,
				LongRect*							lineColumnRectPtr,
				DoubleRect*							coordinateRectPtr);

void ComputeSelectionCoordinates (
				Handle								windowInfoHandle,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double								factor,
				SInt16								gridCode,
				SInt16								viewUnits,
				UInt32								displayedColumnInterval,
				UInt32								displayedLineInterval,
				LongRect*							lineColumnRectPtr,
				DoubleRect*							coordinateRectPtr);

Boolean ConvertCoordinateRectToLCRect (
				Handle								windowInfoHandle,
				DoubleRect*							inputCoordinateRectanglePtr,
				LongRect*							selectionRectanglePtr,
				SInt16								selectionUnitsCode,
				double								factor);

               
			// Prototypes for routines in this file that are only called by		
			// other routines in this file.	
				
void		ConvertMapRectByGivenFactor (
				double								factor,
				DoubleRect*							inputCoordinateRectanglePtr);

void		DrawSelectionPolygon (
				SelectionInfoPtr					selectionInfoPtr,
				LongRect*							selectionRectPtr,
				LCToWindowUnitsVariablesPtr	lcToWindowUnitsVariablesPtr,
				LongRect*							viewRectPtr);

void		DrawSelectionRectangle (
				SelectionInfoPtr					selectionInfoPtr,
				LongRect*							selectionRectPtr,
				LongRect*							viewRectPtr);

PascalVoid DrawSelectionUnitsPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

void		EditLineColumnDialogOK (
				Handle								windowInfoHandle,
				LongRect*							inputSelectionRectanglePtr,
				LongRect*							selectionRectanglePtr,
				DoubleRect*							coordinateRectanglePtr,
				SInt16								unitsDisplayCode,
				Boolean*								changedFlagPtr);

SInt64	GetNumberPixelsInSelection (
				SelectionInfoPtr					selectionInfoPtr);

void		SetPolygonSelection (
				WindowInfoPtr						windowInfoPtr,
				SelectionInfoPtr					selectionInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				SInt32								lineAdjust,
				SInt32								columnAdjust);

void		SetRectangleSelection (
				WindowInfoPtr						windowInfoPtr,
				SelectionInfoPtr					selectionInfoPtr,
				LongRect*							lineColumnRectPtr,
				DoubleRect*							inputCoordinateRectPtr,
				SInt16								unitsToUseCode,
				SInt32								lineAdjust,
				SInt32								columnAdjust);


SInt16		gSelectionDisplayUnits = 0;



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ClearNewFieldList
//
//	Software purpose:	The purpose of this routine is to remove the list
//							in the "NewField" stats window if it is open.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ClearSelectionArea in selectionArea.c
//							CloseImageWindow in window.c
//
//	Coded By:			Larry L. Biehl			Date: 08/31/1989
//	Revised By:			Larry L. Biehl			Date: 08/31/1989	

void ClearNewFieldList (void)

{
			// Declare local variables and structures										

	GrafPtr								savedPort;


	if (gStatisticsMode == kStatNewFieldRectMode || 
														gStatisticsMode == kStatNewFieldPolyMode) 
		{
				// Save currently active window and make the Statistics window		
				// the active window.															

		GetPort (&savedPort);
		SetPortWindowPort (gProjectWindow);

				// Declare local variables and structures	

		MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 255);

				// Remove data in the list so the next field can be selected.		
				// Invalidate the list rectangle so that it will be updated.		

		RemoveListCells ();

		SetPort (savedPort);

		gUpdateEditMenuItemsFlag = TRUE;

		}	// end "if (gStatisticsMode == kStatNewFieldRectMode ..." 

}	// end "ClearNewFieldList" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		ClearSelectionArea
//
//	Software purpose:	This routine clears the selection area.  The
//							selection area type flag is cleared and the
//							outlined selected area is invalidated in the
//							image window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DisplayMultispectralImage in displayMultiSpec.c
//							DisplayThematicImage displayThematic.c
//							DoEdit in menus.c
//							Menus in menus.c
//							ModalFileSpecification in openImages.c
//							RectangleSelection in selectionArea.c
//							PolygonSelection in selectionArea.c
//							GetSelectionRectangle in selectionArea.c
//							AddFieldToProject in statistics.c
//							StatisticsWControlEvent in statistics.c
//
//	Coded By:			Larry L. Biehl			Date: 08/11/1989
//	Revised By:			Larry L. Biehl			Date: 01/16/2019

void ClearSelectionArea (
				WindowPtr							windowPtr)
				
{
	LCToWindowUnitsVariables		lcToWindowUnitsVariables;

	GrafPtr								savedPort;
	LongRect								selectionRect;

	SelectionInfoPtr					selectionInfoPtr;

	Handle								selectionInfoHandle,
											windowInfoHandle;

	SInt32								channel;

	SignedByte							handleStatus;


	windowInfoHandle = GetWindowInfoHandle (windowPtr);

	if (OffscreenImageMapExists (windowInfoHandle)) 
		{
				// Get handle to the selection information for the window.

		selectionInfoHandle = GetSelectionInfoHandle (windowInfoHandle);

				// Determine if this handle is presently locked.  Save status	
				// for use at end of routine to determine whether to unlock		
				// the handle or not.  We do not want to unlock it if it was	
				// locked at this point.  The calling routine will handle		
				// unlocking it.						

		selectionInfoPtr = (SelectionInfoPtr)GetHandleStatusAndPointer (
																					 selectionInfoHandle,
																					 &handleStatus);

		if (selectionInfoPtr != NULL) 
			{
			if (selectionInfoPtr->typeFlag == kRectangleType ||
														selectionInfoPtr->typeFlag == kPolygonType) 
				{
				GetPort (&savedPort);
				SetPortWindowPort (windowPtr);

				GetWindowClipRectangle (windowPtr, kImageFrameArea, &gViewRect);

						// Set the global variables needed to convert from 			
						// line-column units to window units.								

				SetChannelWindowVariables (kToImageWindow, 
													windowInfoHandle, 
													kNotCoreGraphics);

				SetLCToWindowUnitVariables (windowInfoHandle,
														kToImageWindow,
														FALSE,
														&lcToWindowUnitsVariables);

						// Get selection rectangle in current window coordinates.	

				ConvertLCRectToWinRect (&selectionInfoPtr->lineColumnRectangle,
												&selectionRect,
												&lcToWindowUnitsVariables);
				#if defined multispec_mac				
					Rect tempRect;

							// Now copy the long rect structure to a short rect 			
							// structure.  If everything is okay, the long rect			
							// structure is not needed now.										
							// Add one to the right and bottom to make certain that		
							// the selection gets cleared - important for polygonal		
							// selections.																

					tempRect.top = (SInt16)selectionRect.top - 1;
					tempRect.left = (SInt16)selectionRect.left - 1;
					tempRect.bottom = (SInt16)selectionRect.bottom + 1;
					tempRect.right = (SInt16)selectionRect.right + 1;
				#endif	// defined multispec_mac    

				#if defined multispec_win   				
					CRect tempRect;
					LongPoint scrollOffset;

					GetScrollOffset (windowInfoHandle, &scrollOffset);

					tempRect.top = (int)(selectionRect.top - 1 - scrollOffset.v);
					tempRect.left = (int)(selectionRect.left - 1 - scrollOffset.h);
					tempRect.bottom = (int)(selectionRect.bottom + 1 - scrollOffset.v);
					tempRect.right = (int)(selectionRect.right + 1 - scrollOffset.h);
				#endif	// defined multispec_win  	     

				#if defined multispec_lin   			
					wxRect tempRect;

					tempRect.x = (int)selectionRect.left - 3;
					tempRect.y = (int)selectionRect.top - 3;
					tempRect.width = (int)(selectionRect.right - selectionRect.left + 3);
					tempRect.height = (int)(selectionRect.bottom - selectionRect.top + 3);
				#endif	// defined multispec_lin     

				#if defined multispec_mac || defined multispec_win
					if (tempRect.top <= gViewRect.bottom && 
																	tempRect.bottom >= gViewRect.top)
				#endif	// defined multispec_mac || defined multispec_win    
				#if defined multispec_lin  
							// Modify tempRect.x to tempRect.y in the first condition 
							// 01.06.2016 Wei
					if (tempRect.y <= gViewRect.bottom &&
													tempRect.y + tempRect.height >= gViewRect.top)
				#endif	// defined multispec_lin 
						{
						for (channel=gStartChannel; channel<gSideBySideChannels; channel++) 
							{
							#if defined multispec_mac
								if (tempRect.right >= gViewRect.left)
									InvalWindowRect (windowPtr, &tempRect);

								tempRect.left += (SInt16)gChannelWindowInterval;
								tempRect.right += (SInt16)gChannelWindowInterval;
								if (tempRect.left > gViewRect.right)
									break;
							#endif	// defined multispec_mac    

							#if defined multispec_win    
										// Note that the units for the rectangle for 
										// InvalidateRect is in client units.                  

								if (tempRect.right >= gViewRect.left)
									windowPtr->InvalidateRect (&tempRect, FALSE);

								tempRect.left += (int)gChannelWindowInterval;
								tempRect.right += (int)gChannelWindowInterval;
								if (tempRect.left > gViewRect.right)
									break;
							#endif	// defined multispec_win      

							#if defined multispec_lin    
								if (tempRect.x + tempRect.width >= gViewRect.left)
									(windowPtr->m_Canvas)->RefreshRect (tempRect, FALSE);

								tempRect.x += (int)gChannelWindowInterval;

								if (tempRect.x > gViewRect.right)
									break;
							#endif	// defined multispec_lin     

							}	// end "for (channel=gStartChannel; channel<..."

						}	// end "if (tempRect.top <= gViewRect.bottom && ..."

				SetPort (savedPort);

				selectionInfoPtr->typeFlag = 0;
				selectionInfoPtr->numberPoints = 0;
				selectionInfoPtr->offScreenRectangle.top = 0;
				selectionInfoPtr->offScreenRectangle.left = 0;

				if (windowPtr == gActiveImageWindow)
					{
							// Make certain the selection coordinates window and
							// graph selection window is correct if the input
							// window is the active window.

							// Make certain that the coordinates in the Selection
							// Dialog Window are cleared.

					ShowSelection (windowInfoHandle);

							// Make certain that the graph is cleared.

					ShowGraphSelection ();

							// Force update of edit menu

					gUpdateEditMenuItemsFlag = TRUE;

					}	// end "if (windowPtr == gActiveImageWindow)"

				InvalidateWindow (windowPtr, kCoordinateSelectionArea, FALSE);

				UpdateCoordinateViewControls (windowPtr);

							// Remove the coordinates from the statistics window 		
							// list if they exist.												

				if (windowPtr == gProjectSelectionWindow)
					ClearNewFieldList ();

				selectionInfoPtr->polygonCoordinatesHandle = UnlockAndDispose (
													selectionInfoPtr->polygonCoordinatesHandle);

				}	// end "if (selectionInfoPtr->typeFlag == kRectangleType || ...)" 

					// Unlock the selection information handle if needed.				

			MHSetState (selectionInfoHandle, handleStatus);

			}	// end "if (selectionInfoPtr != NULL)" 

		}	// end "if (OffscreenImageMapExists (windowInfoHandle))" 

}	// end "ClearSelectionArea"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ClosePolygonSelection
//
//	Software purpose:	
//
//	Parameters in:		None		
//
//	Parameters out:	None			
//
//	Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 09/28/1998
//	Revised By:			Larry L. Biehl			Date: 06/24/1999			

void ClosePolygonSelection (
				SelectionInfoPtr					selectionInfoPtr)
				
{
	UInt32								bytesNeeded,
											memoryLimitNumber;


	memoryLimitNumber = selectionInfoPtr->numberPoints;

			// Unlock handle to selection polygon points.								

	CheckAndUnlockHandle (selectionInfoPtr->polygonCoordinatesHandle);

			// Adjust the handle size to only include the points actually entered.																				

	bytesNeeded = memoryLimitNumber * sizeof (ProjectFieldPoints);
	MSetHandleSize (&selectionInfoPtr->polygonCoordinatesHandle, (SInt64)bytesNeeded);

	if (selectionInfoPtr->numberPoints >= 3)
		selectionInfoPtr->numberPixels = GetNumberPixelsInSelection (selectionInfoPtr);

	if (selectionInfoPtr->numberPoints < 3 || selectionInfoPtr->numberPixels == 0)
		ClearSelectionArea (gActiveImageWindow);

	gProcessorCode = 0;

}	// end "ClosePolygonSelection"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ComputeMapCoordinates
//
//	Software purpose:	This routine updates the selection coordinates base on the user
//							selection in the Edit->Selection Rectangle... dialog box. The
//							map units will be displayed in the native units for the image.
//							One though needs to provide a factor to change them to 
//							meters when converting the selection to a lat-long parameter 
//							since this is always done in meters.
//
//	Parameters in:		None		
//
//	Parameters out:	None			
//
//	Value Returned:	None
// 
// Called By:			EditSelectionDialogSetCoordinates in SSelectionUtility.cpp
//							EditLineColumnDialog in SSelectionUtility.cpp
//							EditLineColumnDialogInitialize in SSelectionUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/24/2004
//	Revised By:			Larry L. Biehl			Date: 04/28/2012			

void ComputeMapCoordinates (
				Handle								windowInfoHandle,
				double								factor,
				SInt16								viewUnits,
				LongRect*							lineColumnRectPtr,
				DoubleRect*							coordinateRectPtr)
				
{
	MapProjectionInfoPtr				mapProjectionInfoPtr;

	Handle								mapProjectionHandle;

	SInt16								projectionCode;


	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);

	mapProjectionInfoPtr =
							(MapProjectionInfoPtr)GetHandlePointer (mapProjectionHandle);

	projectionCode = GetFileProjectionCode (windowInfoHandle);

	ComputeSelectionCoordinates (windowInfoHandle,
											mapProjectionInfoPtr,
											factor,
											projectionCode,
											viewUnits,
											1,
											1,
											lineColumnRectPtr,
											coordinateRectPtr);

}	// end "ComputeMapCoordinates"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ComputeSelectionCoordinates
//
//	Software purpose:	
//
//	Parameters in:		None		
//
//	Parameters out:	None			
//
//	Value Returned:	None
// 
// Called By:			PolygonSelection in selectionArea.c
//							SetSelectionForAllWindows in SSelectionUtility.cpp
//							UpdateSelectionCoordinates in SSelectionUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/24/2004
//	Revised By:			Larry L. Biehl			Date: 02/23/2007			

void ComputeSelectionCoordinates (
				Handle								windowInfoHandle,
				LongRect*							lineColumnRectPtr,
				DoubleRect*							coordinateRectPtr)
				
{
	MapProjectionInfoPtr				mapProjectionInfoPtr;

	Handle								mapProjectionHandle;

	SignedByte							handleStatus;


	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);

	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandleStatusAndPointer (
																					mapProjectionHandle,
																					&handleStatus);

	if (mapProjectionInfoPtr != NULL)
		ComputeSelectionCoordinates (windowInfoHandle,
												mapProjectionInfoPtr,
												lineColumnRectPtr,
												coordinateRectPtr);

	MHSetState (mapProjectionHandle, handleStatus);

}	// end "ComputeSelectionCoordinates"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ComputeSelectionCoordinates
//
//	Software purpose:	This routine determines the current selection
//							rectangle coordinates in map unit values.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			PolygonSelection in selectionArea.c
//							ComputeSelectionLineColumns in SSelectionUtility.cpp
//							SetSelectionForAllWindows in SSelectionUtility.cpp
//							UpdateSelectionCoordinates in SSelectionUtility.cpp
//
//	Revised By:			Larry L. Biehl			Date: 11/08/2000	
//	Revised By:			Larry L. Biehl			Date: 03/13/2012		

void ComputeSelectionCoordinates (
				Handle								windowInfoHandle,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				LongRect*							lineColumnRectPtr,
				DoubleRect*							coordinateRectPtr)
				
{
	double								factor;

	DisplaySpecsPtr					displaySpecsPtr;

	Handle								displaySpecsHandle;

	UInt32								displayedColumnInterval,
											displayedLineInterval;

	SInt16								projectionCode,
											viewUnits;


	viewUnits = GetCoordinateViewUnits (windowInfoHandle);

	if (viewUnits != kLineColumnUnitsMenuItem) 
		{
				// Now also get the selected area in coordinate units.

		projectionCode = GetFileProjectionCode (windowInfoHandle);

		factor = GetCoordinateViewFactor (windowInfoHandle);

		displaySpecsHandle = GetDisplaySpecsHandle (windowInfoHandle);

		displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);

		displayedColumnInterval = displaySpecsPtr->displayedColumnInterval;
		displayedLineInterval = displaySpecsPtr->displayedLineInterval;

		ComputeSelectionCoordinates (windowInfoHandle,
												 mapProjectionInfoPtr,
												 factor,
												 projectionCode,
												 viewUnits,
												 displayedColumnInterval,
												 displayedLineInterval,
												 lineColumnRectPtr,
												 coordinateRectPtr);

		}	// end "if (viewUnits != kLineColumnUnitsMenuItem)"

}	// end "ComputeSelectionCoordinates" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ComputeSelectionCoordinates
//
//	Software purpose:	This routine determines the current selection
//							rectangle coordinates in map unit values.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			PolygonSelection in selectionArea.c
//							ComputeSelectionLineColumns in SSelectionUtility.cpp
//							SetSelectionForAllWindows in SSelectionUtility.cpp
//							UpdateSelectionCoordinates in SSelectionUtility.cpp
//
//	Revised By:			Larry L. Biehl			Date: 11/19/2004	
//	Revised By:			Larry L. Biehl			Date: 02/13/2007		

void ComputeSelectionCoordinates (
				Handle								windowInfoHandle,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double								factor,
				SInt16								projectionCode,
				SInt16								viewUnits,
				UInt32								displayedColumnInterval,
				UInt32								displayedLineInterval,
				LongRect*							lineColumnRectPtr,
				DoubleRect*							coordinateRectPtr)
				
{
	double								doubleCoordinateValueBottom,
											doubleCoordinateValueLeft,
											doubleCoordinateValueRight,
											doubleCoordinateValueTop,
											xMapCoordinate11,
											yMapCoordinate11;


	if (mapProjectionInfoPtr == NULL)
																									return;

			// Now also get the selected area in coordinate units.
			// Convert the start point to coordinate units.	

	if (mapProjectionInfoPtr->planarCoordinate.polynomialOrder <= 0) 
		{
		//doubleCoordinateValueLeft = (double)(lineColumnRectPtr->left - 0.5 *
		//																	displayedColumnInterval - 1);
		doubleCoordinateValueLeft = (double)(lineColumnRectPtr->left - 0.5 - 1);
		doubleCoordinateValueLeft *= 
									mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;

		//doubleCoordinateValueTop = (double)(lineColumnRectPtr->top - 0.5 *
		//																	displayedLineInterval - 1);
		doubleCoordinateValueTop = (double)(lineColumnRectPtr->top - 0.5 - 1);
		doubleCoordinateValueTop *= 
									-mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;

				// Convert the end point to coordinate units.								

		//doubleCoordinateValueRight = (double)(lineColumnRectPtr->right + 0.5 *
		//																	displayedColumnInterval - 1);
		doubleCoordinateValueRight = (double)(lineColumnRectPtr->right + 0.5 - 1);
		doubleCoordinateValueRight *= 
									mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;

		//doubleCoordinateValueBottom = (double)(lineColumnRectPtr->bottom + 0.5 *
		//																	displayedLineInterval - 1); 
		doubleCoordinateValueBottom = (double)(lineColumnRectPtr->bottom + 0.5 - 1);
		doubleCoordinateValueBottom *= 
									-mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;

		if (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle == 0) 
			{
			xMapCoordinate11 = mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
			yMapCoordinate11 = mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;

					// Top left point

			coordinateRectPtr->left = xMapCoordinate11 + doubleCoordinateValueLeft;
			coordinateRectPtr->left *= factor;

			coordinateRectPtr->top = yMapCoordinate11 + doubleCoordinateValueTop;
			coordinateRectPtr->top *= factor;

					// Bottom right point

			coordinateRectPtr->right = xMapCoordinate11 + doubleCoordinateValueRight;
			coordinateRectPtr->right *= factor;

			coordinateRectPtr->bottom = yMapCoordinate11 + doubleCoordinateValueBottom;
			coordinateRectPtr->bottom *= factor;

			}	// end "if (planarCoordinateSystemInfoPtr->mapOrientationAngle == 0)"

		else	// planarCoordinateSystemInfoPtr->mapOrientationAngle != 0
			{
			double		cosOrientAngle,
							sinOrientAngle,
							xMapOrientationOrigin,
							yMapOrientationOrigin;

			xMapOrientationOrigin = 
								mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin;
			yMapOrientationOrigin = 
								mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin;

			cosOrientAngle = 
							cos (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle);
			sinOrientAngle = 
							sin (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle);

			coordinateRectPtr->left = xMapOrientationOrigin +
										doubleCoordinateValueLeft * cosOrientAngle +
															doubleCoordinateValueTop*sinOrientAngle;
			coordinateRectPtr->left *= factor;

			coordinateRectPtr->top = yMapOrientationOrigin -
										doubleCoordinateValueLeft * sinOrientAngle +
															doubleCoordinateValueTop*cosOrientAngle;
			coordinateRectPtr->top *= factor;

			coordinateRectPtr->right = xMapOrientationOrigin +
										doubleCoordinateValueRight * cosOrientAngle +
														doubleCoordinateValueBottom*sinOrientAngle;
			coordinateRectPtr->right *= factor;

			coordinateRectPtr->bottom = yMapOrientationOrigin -
										doubleCoordinateValueRight * sinOrientAngle +
														doubleCoordinateValueBottom*cosOrientAngle;
			coordinateRectPtr->bottom *= factor;

			}	// end "if (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle ..."

		}	// end "mapProjectionInfoPtr->planarCoordinate.polynomialOrder <= 0"

	else	// mapProjectionInfoPtr->planarCoordinate.polynomialOrder > 0
		{
		doubleCoordinateValueLeft = (double)(lineColumnRectPtr->left - 0.5);
		doubleCoordinateValueTop = (double)(lineColumnRectPtr->top - 0.5);

		GetCoefficientsVectorPointers (mapProjectionInfoPtr);
		TransformCoordinatePoint (
							 doubleCoordinateValueLeft,
							 doubleCoordinateValueTop,
							 &coordinateRectPtr->left,
							 &coordinateRectPtr->top,
							 mapProjectionInfoPtr->planarCoordinate.easting1CoefficientsPtr,
							 mapProjectionInfoPtr->planarCoordinate.northing1CoefficientsPtr,
							 mapProjectionInfoPtr->planarCoordinate.polynomialOrder);

		doubleCoordinateValueRight = (double)(lineColumnRectPtr->right + 0.5);
		doubleCoordinateValueBottom = (double)(lineColumnRectPtr->bottom + 0.5);
		TransformCoordinatePoint (
							 doubleCoordinateValueRight,
							 doubleCoordinateValueBottom,
							 &coordinateRectPtr->right,
							 &coordinateRectPtr->bottom,
							 mapProjectionInfoPtr->planarCoordinate.easting1CoefficientsPtr,
							 mapProjectionInfoPtr->planarCoordinate.northing1CoefficientsPtr,
							 mapProjectionInfoPtr->planarCoordinate.polynomialOrder);
		CloseCoefficientsVectorPointers (mapProjectionInfoPtr);

		}	// end "else mapProjectionInfoPtr->gridCoordinate.polynomialOrder > 0"

	if (viewUnits == kDecimalLatLongUnitsMenuItem || 
															viewUnits == kDMSLatLongUnitsMenuItem)
		ConvertMapRectToLatLongRect (mapProjectionInfoPtr, 
												projectionCode, 
												coordinateRectPtr);

}	// end "ComputeSelectionCoordinates" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ComputeSelectionLineColumns
//
//	Software purpose:	This routine determines the current selection
//							rectangle coordinates in line and column values.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			RectangleSelection in SSelectionArea.cpp
//
//	Revised By:			Larry L. Biehl			Date: 06/03/1993	
//	Revised By:			Larry L. Biehl			Date: 01/03/2019

void ComputeSelectionLineColumns (
				DisplaySpecsPtr					displaySpecsPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				LongRect*							lineColumnRectPtr,
				Rect*									displayRectPtr,
				LongPoint*							startPointPtr,
				DoubleRect*							coordinateRectPtr)
				
{
	double								magnification;

	SInt32								columnEnd,
											columnOrigin,
											columnStart,
											displayedColumnInterval,
											displayedLineInterval,
											lastColumn,
											lastLine,
											lineEnd,
											lineOrigin,
											lineStart;


			// Convert the start point to line and column numbers.						

	columnOrigin = displaySpecsPtr->displayedColumnStart;
	lineOrigin = displaySpecsPtr->displayedLineStart;

	lastColumn = displaySpecsPtr->displayedColumnEnd;
	lastLine = displaySpecsPtr->displayedLineEnd;

	magnification = displaySpecsPtr->magnification;
	displayedColumnInterval = displaySpecsPtr->displayedColumnInterval;
	displayedLineInterval = displaySpecsPtr->displayedLineInterval;

	columnStart = columnOrigin + (SInt32)(((double)(displayRectPtr->left - startPointPtr->h) /
													magnification) * displayedColumnInterval);
	columnStart = MIN (columnStart, lastColumn);
	lineColumnRectPtr->left = MAX (columnStart, 1);

	lineStart = lineOrigin + (SInt32)(((double)(displayRectPtr->top - startPointPtr->v) /
													magnification) * displayedLineInterval);
	lineStart = MIN (lineStart, lastLine);
	lineColumnRectPtr->top = MAX (lineStart, 1);

			// Convert the end point to line and column numbers.						
			// The selection has to be at least one pixel in size.					

	columnEnd = columnOrigin + (SInt32)(((double)(displayRectPtr->right - startPointPtr->h) /
													magnification) * displayedColumnInterval);
	columnEnd = MIN (columnEnd, lastColumn);
	columnEnd = MAX (columnEnd, 1);
	lineColumnRectPtr->right = MAX (lineColumnRectPtr->left, columnEnd);

	lineEnd = lineOrigin + (SInt32)(((double)(displayRectPtr->bottom - startPointPtr->v) /
													magnification) * displayedLineInterval);
	lineEnd = MIN (lineEnd, lastLine);
	lineEnd = MAX (lineEnd, 1);
	lineColumnRectPtr->bottom = MAX (lineColumnRectPtr->top, lineEnd);

			// Now also get the selected area in coordinate units.

	ComputeSelectionCoordinates (gActiveImageWindowInfoH,
											mapProjectionInfoPtr,
											lineColumnRectPtr,
											coordinateRectPtr);

}	// end "ComputeSelectionLineColumns" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ComputeSelectionOffscreenRectangle
//
//	Software purpose:	This routine computes the selection offscreen rectangle based on
//							the input line and column coordinates and the channel where
//							the selection was made in. This rectangle is used when zooming so
//							that the zoom takes place in the channel that the selection was
//							made in. This is usually channel 1 (the only one) unless it
//							is a side-by-side channel display.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			GetBoundingSelectionRectangles in SSelUtil.cpp
//							SetSelectionInformation in SSelUtil.cpp
//
//	Revised By:			Larry L. Biehl			Date: 10/01/1998	
//	Revised By:			Larry L. Biehl			Date: 03/22/1999				

void ComputeSelectionOffscreenRectangle (
				DisplaySpecsPtr					displaySpecsPtr,
				LongRect*							lineColumnRectanglePtr,
				Rect*									offScreenRectanglePtr,
				SInt16								startChannel)
				
{
	LongPoint							offscreenPoint;


			// A 'displayedLineInterval' value of 0 indicates that no offscreen
			// image map exists.

	if (displaySpecsPtr != NULL && displaySpecsPtr->displayedLineInterval > 0) 
		{
				// Get the selection rectangle location relative to the offscreen		
				// bit/pix map																																						
		#ifndef multispec_lin	
			ConvertLCToOffscreenPoint (displaySpecsPtr,
												(LongPoint*)lineColumnRectanglePtr,
												&offscreenPoint);
		#endif

		#if defined multispec_lin
			LongPoint lineColumntopleft;
			LongPoint lineColumnbottomright;
			lineColumntopleft.h = lineColumnRectanglePtr->left;
			lineColumntopleft.v = lineColumnRectanglePtr->top;
			lineColumnbottomright.h = lineColumnRectanglePtr->right;
			lineColumnbottomright.v = lineColumnRectanglePtr->bottom;
			ConvertLCToOffscreenPoint (displaySpecsPtr,
												(LongPoint*)&lineColumntopleft,
												&offscreenPoint);
		#endif

		#if defined multispec_mac 										
			offScreenRectanglePtr->top = (SInt16)offscreenPoint.v;
			offScreenRectanglePtr->left = (SInt16)offscreenPoint.h;
		#endif	// defined multispec_mac

		#if defined multispec_win || defined multispec_lin
			offScreenRectanglePtr->top = (int)offscreenPoint.v;
			offScreenRectanglePtr->left = (int)offscreenPoint.h;
		#endif	// defined multispec_win || defined multispec_lin 

		#ifndef multispec_lin										
			ConvertLCToOffscreenPoint (displaySpecsPtr,
												(LongPoint*)&lineColumnRectanglePtr->bottom,
												&offscreenPoint);
		#endif
		
		#if defined multispec_lin
			ConvertLCToOffscreenPoint (displaySpecsPtr,
												(LongPoint*)&lineColumnbottomright,
												&offscreenPoint);
		#endif

		#if defined multispec_mac 										
			offScreenRectanglePtr->bottom = (SInt16)offscreenPoint.v;
			offScreenRectanglePtr->right = (SInt16)offscreenPoint.h;
		#endif	// defined multispec_mac

		#if defined multispec_win || defined multispec_lin 
			offScreenRectanglePtr->bottom = (int)offscreenPoint.v;
			offScreenRectanglePtr->right = (int)offscreenPoint.h;
		#endif	// defined multispec_win || defined multispec_lin 


		#if defined multispec_mac 										
					// Now adjust the lower right point to represent the bottom right of
					// the pixel.

			offScreenRectanglePtr->bottom = 
									(SInt16)MIN (displaySpecsPtr->imageDimensions[kVertical],
														(UInt32)(offScreenRectanglePtr->bottom + 1));

			offScreenRectanglePtr->right = 
									(SInt16)MIN (displaySpecsPtr->imageDimensions[kHorizontal],
									(UInt32)(offScreenRectanglePtr->right + 1));

					// Adjust the offscreen units to reflect the channel that the			
					// selection was made within.														

			offScreenRectanglePtr->left +=
							(SInt16)(startChannel * displaySpecsPtr->offscreenChannelWidth);
			offScreenRectanglePtr->right +=
							(SInt16)(startChannel * displaySpecsPtr->offscreenChannelWidth);
		#endif	// defined multispec_mac

		#if defined multispec_win || defined multispec_lin					
					// Now adjust the lower right point to represent the bottom right of
					// the pixel.

			offScreenRectanglePtr->bottom = 
									(int)MIN (displaySpecsPtr->imageDimensions[kVertical],
													(UInt32)(offScreenRectanglePtr->bottom + 1));

			offScreenRectanglePtr->right = 
									(int)MIN (displaySpecsPtr->imageDimensions[kHorizontal],
													(UInt32)(offScreenRectanglePtr->right + 1));

					// Adjust the offscreen units to reflect the channel that the			
					// selection was made within.														

			offScreenRectanglePtr->left +=
								(int)(startChannel * displaySpecsPtr->offscreenChannelWidth);
			offScreenRectanglePtr->right +=
								(int)(startChannel * displaySpecsPtr->offscreenChannelWidth);
		#endif	// defined multispec_win || defined multispec_lin 

		}	// end "if (displaySpecsPtr != NULL)"

	else	// displaySpecsPtr == NULL && ...
		{
		offScreenRectanglePtr->top = 0;
		offScreenRectanglePtr->bottom = 0;
		offScreenRectanglePtr->left = 0;
		offScreenRectanglePtr->right = 0;

		}	// end "else displaySpecsPtr == NULL" 

}	// end "ComputeSelectionOffscreenRectangle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ConvertCoordinateRectToLCRect
//
//	Software purpose:	This routine converts the input coordinate rectangle in the units
//							defined by selectionUnitsCode to the line and columns for for
//							image window defined by the input windowInfoHandle. Note that no
//							line and columns will be computed if the selection units code is
//							already in line/columns. 
// 
//							Also allow for case when requested lat-long does not map to any 
//							map units for specified projection. For example this could be 
//							the case for the Orthographic projection.
//
//	Parameters in:		Handle to the window information structure
//							Pointer to the input coordinate rectangle
//							Code representing the units that the coordinates are in	
//
//	Parameters out:	Pointer to the output line and column rectangle.			
//
// Value Returned:	None
// 
// Called By:			EditSelectionDialogShowSelection
//							EditLineColumnDialogOK
//							SetRectangleSelection
//
//	Coded By:			Larry L. Biehl			Date: 03/17/2005
//	Revised By:			Larry L. Biehl			Date: 04/29/2012			

Boolean ConvertCoordinateRectToLCRect (
				Handle								windowInfoHandle,
				DoubleRect*							inputCoordinateRectanglePtr,
				LongRect*							selectionRectanglePtr,
				SInt16								selectionUnitsCode,
				double								factor)
				
{
	DoubleRect							coordinateRect;

	Boolean								continueFlag = TRUE;


	coordinateRect = *inputCoordinateRectanglePtr;

	if (selectionUnitsCode == kLatLongUnits)
			// Force lat-long units to be converted to map units
		continueFlag = ConvertLatLongRectToMapRect (windowInfoHandle, &coordinateRect);

	if (continueFlag) 
		{
		coordinateRect.top *= factor;
		coordinateRect.left *= factor;
		coordinateRect.bottom *= factor;
		coordinateRect.right *= factor;

		if (selectionUnitsCode != kLineColumnUnits)
					// Force map units to be converted to line-column units
			ConvertMapRectToLCRect (windowInfoHandle,
											 &coordinateRect,
											 selectionRectanglePtr,
											 factor);

		}	// end "if (continueFlag)"

	return (continueFlag);

}	// end "ConvertCoordinateRectToLCRect" 


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ConvertLatLongRectToMapRectinNativeImageUnits
//
//	Software purpose:	This routine converts the input lat-long rectanle to a rectangle
//							in the native map units for the image file.
//
//	Parameters in:			
//
//	Parameters out:				
//
// Value Returned:	True if conversion completed successfully.
//							False if  not. Only reason would be that there is no conversion
//									for the projection being used.
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/28/2012
//	Revised By:			Larry L. Biehl			Date: 04/28/2012			

Boolean ConvertLatLongRectToMapRectinNativeImageUnits (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				DoubleRect*							coordinateRectanglePtr)
				
{
	double								factor;

	SInt16								fileMapUnitsIndex;

	Boolean								continueFlag;
	
																					
			// Force lat-long units to be converted to map units

	continueFlag = ConvertLatLongRectToMapRect (mapProjectionInfoPtr, 
																coordinateRectanglePtr);

			// The map rectangle is now in meters. Convert if needed to the native units
			// for the image file.

	fileMapUnitsIndex = 
					mapProjectionInfoPtr->planarCoordinate.mapUnitsCode - kKilometersCode;
	factor = gDistanceFileConversionFactors[1] / 
											gDistanceFileConversionFactors[fileMapUnitsIndex];
					
	if (continueFlag)
		ConvertMapRectByGivenFactor (factor, coordinateRectanglePtr);

	return (continueFlag);
			
}	// end "ConvertLatLongRectToMapRectinNativeImageUnits" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ConvertLatLongRectToMapRectinNativeImageUnits
//
//	Software purpose:	This routine converts the input lat-long rectanle to a rectangle
//							in the native map units for the image file.
//
//	Parameters in:			
//
//	Parameters out:				
//
// Value Returned:	True if conversion completed successfully.
//							False if  not. Only reason would be that there is no conversion
//									for the projection being used.
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/28/2012
//	Revised By:			Larry L. Biehl			Date: 04/28/2012			

Boolean ConvertLatLongRectToMapRectinNativeImageUnits (
				Handle								windowInfoHandle,
				DoubleRect*							coordinateRectPtr)
				
{
	MapProjectionInfoPtr				mapProjectionInfoPtr;

	Handle								mapProjectionHandle;

																						
			// Get pointer to the map projection structure.									

	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);											
	mapProjectionInfoPtr = 
							(MapProjectionInfoPtr)GetHandlePointer (mapProjectionHandle);		

	return (ConvertLatLongRectToMapRectinNativeImageUnits (mapProjectionInfoPtr,
																			 coordinateRectPtr));
			
}	// end "ConvertLatLongRectToMapRectinNativeImageUnits" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DrawSelectionArea
//
//	Software purpose:	This routine outlines the selection area for
//							the input image window.  It assumes that the port
//							has already been set for the input window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 08/11/1989
//	Revised By:			Larry L. Biehl			Date: 06/20/2016			

void DrawSelectionArea (
				WindowPtr							windowPtr)
				
{
	LCToWindowUnitsVariables		lcToWindowUnitsVariables;

	LongRect								selectionRect,
											viewRect;

	SelectionInfoPtr					selectionInfoPtr;

	Handle								displaySpecsH,
											selectionInfoH,
											windowInfoH;

	SignedByte							handleStatus;


			// Get the window information structure handle for the input
			// window.

	windowInfoH = GetWindowInfoHandle (windowPtr);

			// Get the pointer to the selection rectangle information.		 		

	selectionInfoH = GetSelectionInfoHandle (windowInfoH);
	displaySpecsH = GetDisplaySpecsHandle (windowInfoH);

	if (selectionInfoH == NULL || displaySpecsH == NULL)
																									return;

			// Determine if this handle is presently locked.  Save status for use at 
			// end of precedure to determine whether to unlock the handle or not.  We 
			// do not want to unlock it if it was locked at this point.  The calling 
			// routine will handle unlocking it.																		
			// Lock handle to file information and get pointer to it					

	selectionInfoPtr = (SelectionInfoPtr)GetHandleStatusAndPointer (selectionInfoH,
																							&handleStatus);

	if (selectionInfoPtr->typeFlag != 0)
		{
				// Set the global variables needed to convert from line-column		
				// units to window units.														

		SetChannelWindowVariables (kToImageWindow, windowInfoH, kNotCoreGraphics);

		SetLCToWindowUnitVariables (windowInfoH,
											 kToImageWindow,
											 FALSE,
											 &lcToWindowUnitsVariables);

				// Get the window units for the selection rectangle.

		ConvertLCRectToWinRect (&selectionInfoPtr->lineColumnRectangle,
										 &selectionRect,
										 &lcToWindowUnitsVariables);

				// Get the view rectangle in which the image can be drawn.

		GetWindowClipRectangle (windowPtr, kImageFrameArea, &gViewRect);

		#if defined multispec_win || defined multispec_lin
					// Windows requires the view rectangle to be offset by the scroll 
					// offset.

			LongPoint scrollOffset;
			GetScrollOffset (windowInfoH, &scrollOffset);

			viewRect.top = (SInt32)gViewRect.top + scrollOffset.v;
			viewRect.bottom = (SInt32)gViewRect.bottom + scrollOffset.v;
			viewRect.left = (SInt32)gViewRect.left + scrollOffset.h;
			viewRect.right = (SInt32)gViewRect.right + scrollOffset.h; 

			#if defined multispec_lin
				wxPoint deviceOrigin = gCDCPointer->GetDeviceOrigin ();
				if (deviceOrigin.x > 0 || deviceOrigin.y > 0)
					{ 
							// the device origin should always > 0
							
					deviceOrigin.x = 0;          
					deviceOrigin.x -=  scrollOffset.h;
					deviceOrigin.y = 0;
					deviceOrigin.y -=  scrollOffset.v;
					}
					 
				//printf ("deviceOrigin (drawSelctionArea) = %d, %d \n", 
				//				deviceOrigin.x, 
				//				deviceOrigin.y); 
				gCDCPointer->SetDeviceOrigin (deviceOrigin.x, deviceOrigin.y);   
			#endif	// defined multispec_lin
		#endif	// defined multispec_win || defined multispec_lin

		#if defined multispec_mac
			viewRect.top = gViewRect.top;
			viewRect.left = gViewRect.left;
			viewRect.right = gViewRect.right;
			viewRect.bottom = gViewRect.bottom;

					// Save the current clip area and then set the clip area of the 		
					// image to just the image area.													

			GetClip (gSelectionClipRgn);

					// Clip the window to just the view rectangle.

			ClipRect (&gViewRect);
		#endif	// defined multispec_mac

			// Outline the selection rectangle if it has been set up.
						
		if (selectionInfoPtr->typeFlag == kRectangleType)
			{
			#ifndef multispec_lin
				DrawSelectionRectangle (selectionInfoPtr, &selectionRect, &viewRect);
			#else                
				DrawSelectionRectangle (selectionInfoPtr, &selectionRect, &selectionRect);
			#endif
			
			}	// end "if (selectionInfoPtr->typeFlag == kRectangleType)"

				// Outline the selection polygon if it has been set up.				

		else if (selectionInfoPtr->typeFlag == kPolygonType)
			{
			DrawSelectionPolygon (selectionInfoPtr,
											&selectionRect,
											&lcToWindowUnitsVariables,
											#ifndef multispec_lin
												&viewRect);
											#else
												&selectionRect);   
											#endif
			}

		#if defined multispec_mac	
					// Reset the clip area back to that upon routine entry.				

			SetClip (gSelectionClipRgn);
			SetEmptyRgn (gSelectionClipRgn);
		#endif	// defined multispec_mac

		}	// end "if (selectionInfoPtr->typeFlag)" 

			// Unlock the selection information handle if needed.						

	MHSetState (selectionInfoH, handleStatus);

}	// end "DrawSelectionArea"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DrawSelectionPolygon
//
//	Software purpose:	This routine draws the selection polygon.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Revised By:			Larry L. Biehl			Date: 08/11/1989	
//	Revised By:			Larry L. Biehl			Date: 12/23/2002	

void DrawSelectionPolygon (
				SelectionInfoPtr					selectionInfoPtr,
				LongRect*							selectionRectPtr,
				LCToWindowUnitsVariablesPtr	lcToWindowUnitsVariablesPtr,
				LongRect*							viewRectPtr)
				
{
	LongPoint							windowPoint;
	HPFieldPointsPtr					selectionPointsPtr;

	SInt32								savedChannelWindowOffset;

	UInt32								pointCount,
											pointCountLimit,
											pointIndex;

	SignedByte							selectionPointsStatus;
	
	 
			// Save the channel window offset so that it can be reset at the end.

	//savedChannelWindowOffset = gChannelWindowOffset;	
	savedChannelWindowOffset = lcToWindowUnitsVariablesPtr->channelWindowOffset;

	selectionPointsPtr = (HPFieldPointsPtr)GetHandleStatusAndPointer (
														selectionInfoPtr->polygonCoordinatesHandle,
														&selectionPointsStatus);

	pointCountLimit = 0;
	if (gProcessorCode == kPolygonSelectionProcessor)
		pointCountLimit = 1;

	if (selectionRectPtr->top <= viewRectPtr->bottom &&
													selectionRectPtr->bottom >= viewRectPtr->top)
		{
		#ifndef multispec_lin
			for (SInt32 channel=gStartChannel; channel<gSideBySideChannels; channel++)
				{
				if (selectionRectPtr->left <= viewRectPtr->right &&
													selectionRectPtr->right >= viewRectPtr->left)
					{
					pointCount = selectionInfoPtr->numberPoints;
					//pointIndex = 1;   
					pointIndex = 0;
					ConvertLCToWinPoint ((LongPoint*)&selectionPointsPtr[pointIndex],
													&windowPoint,
													lcToWindowUnitsVariablesPtr);

					#if defined multispec_mac
						MoveTo ((SInt16)windowPoint.h, (SInt16)windowPoint.v);
					#endif	// defined multispec_mac	

					#if defined multispec_win           
						gCDCPointer->MoveTo ((int)windowPoint.h, (int)windowPoint.v);
					#endif	// defined multispec_win	

					pointIndex++;

					while (pointCount > pointCountLimit)
						{
						if (pointCount == 1)
							pointIndex = 0;

						ConvertLCToWinPoint ((LongPoint*)&selectionPointsPtr[pointIndex],
													&windowPoint,
													lcToWindowUnitsVariablesPtr);

						#if defined multispec_mac
							LineTo ((SInt16)windowPoint.h, (SInt16)windowPoint.v);
						#endif	// defined multispec_mac	

						#if defined multispec_win               
							gCDCPointer->LineTo ((int)windowPoint.h, (int)windowPoint.v);
						#endif	// defined multispec_win	

						pointIndex++;
						pointCount--;

						}	//end "while (pointCount > pointCountLimit)"

					}	// end "if (selectionRectPtr->left <= viewRectPtr->right && ..."  
						
				if (gSideBySideChannels > 1)
					{
					selectionRectPtr->left += gChannelWindowInterval;
					selectionRectPtr->right += gChannelWindowInterval;
					lcToWindowUnitsVariablesPtr->channelWindowOffset += 
																					gChannelWindowInterval;              
					
					}	// end "if (gSideBySideChannels > 1)"

				if (selectionRectPtr->left > viewRectPtr->right)
					break;

				}	// end "for (channel=gStartChannel; channel<..."
		#endif	// ifndef multispec_lin

		#if defined multispec_lin
			Handle windowInfoHandle = GetWindowInfoHandle (gProjectSelectionWindow);
			SetChannelWindowVariables (
									kToImageWindow, gProjectSelectionWindow, kNotCoreGraphics);    
			SetLCToWindowUnitVariables (windowInfoHandle,
													 kWindowsUseOrigin,
													 FALSE,
													 lcToWindowUnitsVariablesPtr);
			            
					// Method 1 
					       
			if (gProjectSelectionWindow != NULL)
				{
				pointCount = selectionInfoPtr->numberPoints;
				int startchannel = gProjectSelectionWindow->m_startchannel_sbs;                
				wxPoint* pointlist = new wxPoint[pointCount+1];   
				int sideBysideImage_offset = startchannel * gChannelWindowInterval - gChannelWindowOffset;
				for (pointIndex = 0; pointIndex<pointCount; pointIndex++)
					{
					ConvertLCToWinPoint ((LongPoint*)&selectionPointsPtr[pointIndex],
												&windowPoint,
												lcToWindowUnitsVariablesPtr);
					pointlist[pointIndex].x = windowPoint.h;
					pointlist[pointIndex].y = windowPoint.v;

					}	// end "for (pointIndex = 0; pointIndex<pointCount; pointIndex++)"
						 
				gCDCPointer->SetBrush (*wxTRANSPARENT_BRUSH);
				gCDCPointer->SetPen (wxPen (*wxWHITE, 1, wxPENSTYLE_SHORT_DASH));            
				wxPoint scrollOffset = 
									gProjectSelectionWindow->m_Canvas->GetScrollPosition ();
						 
				if (gProcessorCode == kPolygonSelectionProcessor)
					{ 
							// During polygon selection mode
							
					wxPoint currloc_Win = wxGetMousePosition ();                  
					wxPoint canvasOffset;
					LongPoint currloc_WinCanvas;
					canvasOffset.y = 
									gProjectSelectionWindow->m_Canvas->GetScreenPosition ().y;
					canvasOffset.x = 
									gProjectSelectionWindow->m_Canvas->GetScreenPosition ().x;                  
					currloc_WinCanvas.h = currloc_Win.x -canvasOffset.x;
					currloc_WinCanvas.v = currloc_Win.y -canvasOffset.y;
					pointlist[pointIndex].x = currloc_WinCanvas.h - sideBysideImage_offset;
					pointlist[pointIndex].y = currloc_WinCanvas.v;     
					gCDCPointer->DrawPolygon (pointCount+1, 
														pointlist, 
														sideBysideImage_offset+scrollOffset.x,
														scrollOffset.y,
														wxODDEVEN_RULE);   
														
					gCDCPointer->SetPen (wxPen (*wxBLACK, 1, wxPENSTYLE_SHORT_DASH)); 
					  										  
					gCDCPointer->DrawPolygon (pointCount+1, 
														pointlist, 
														sideBysideImage_offset+scrollOffset.x+1,
														scrollOffset.y+1,
														wxODDEVEN_RULE);
														   
					}	// end "if (gProcessorCode == kPolygonSelectionProcessor)"
					
				else 
					{ 
							// After double clicking, draw on all channels
						
					for (SInt32 channel=gStartChannel; 
								channel<gSideBySideChannels; 
								channel++)
						{
						if (selectionRectPtr->left <= viewRectPtr->right &&
													selectionRectPtr->right >= viewRectPtr->left)
							{                      
							//printf ("Offset = %d\n", 
							//				lcToWindowUnitsVariablesPtr->channelWindowOffset + 
							//																scrollOffset.x);                     							
							gCDCPointer->DrawPolygon (
											pointCount, 
											pointlist, 
											lcToWindowUnitsVariablesPtr->channelWindowOffset + 
											scrollOffset.x,
											scrollOffset.y,
											wxODDEVEN_RULE);
											
							gCDCPointer->SetPen (wxPen (*wxBLACK, 1, wxPENSTYLE_SHORT_DASH));
							   										  
							gCDCPointer->DrawPolygon (
											pointCount, 
											pointlist, 
											lcToWindowUnitsVariablesPtr->channelWindowOffset + 
											scrollOffset.x+1,
											scrollOffset.y+1,
											wxODDEVEN_RULE); 
											  
							if (gSideBySideChannels > 1)
								{
								selectionRectPtr->left += gChannelWindowInterval;
								selectionRectPtr->right += gChannelWindowInterval;
								lcToWindowUnitsVariablesPtr->channelWindowOffset += 
																					gChannelWindowInterval; 
															
								}	// end "if (gSideBySideChannels > 1)"
									
							if (selectionRectPtr->left > viewRectPtr->right)
								break;

							}	// end "if (selectionRectPtr->left <= viewRectPtr->right ..." 
						
						}  // end "for (SInt32 channel=gStartChannel; channel<..."
				
					}	// end else "gProcessorCode != kPolygonSelectionProcessor

				delete pointlist;
				
				}
		#endif	// end "defined multispec_lin"

		}	// end "if (selectionRectPtr->top <= viewRectPtr->bottom && ..." 

	MHSetState (selectionInfoPtr->polygonCoordinatesHandle, selectionPointsStatus);

			// Reset the channel window offset.

	//gChannelWindowOffset = savedChannelWindowOffset;
	lcToWindowUnitsVariablesPtr->channelWindowOffset = savedChannelWindowOffset;
    
}	// end "DrawSelectionPolygon" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DrawSelectionRectangle
//
//	Software purpose:	This routine draws the selection rectangle.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Revised By:			Larry L. Biehl			Date: 08/11/1989	
//	Revised By:			Larry L. Biehl			Date: 12/23/2002	

void DrawSelectionRectangle (
				SelectionInfoPtr					selectionInfoPtr,
				LongRect*							selectionRectPtr,
				LongRect*							viewRectPtr)
				
{
	SInt32								channel;


   if (selectionRectPtr->top <= viewRectPtr->bottom &&
													selectionRectPtr->bottom >= viewRectPtr->top) 
		{
		for (channel=gStartChannel; channel<gSideBySideChannels; channel++) 
			{
			if (selectionRectPtr->left <= viewRectPtr->right &&
													selectionRectPtr->right >= viewRectPtr->left) 
				{
				#if defined multispec_mac     
							 // FrameRect would cause the system to crash when 			
							 // running heap scramble under macbugs. 						
							 // Larry Biehl 2-7-91.	
							 // Note that selectionRect would now have to be change from
							 // LongRect to Rect if FrameRect is to now be used.											

					 //FrameRect (selectionRectPtr);        

					 MoveTo ((SInt16)selectionRectPtr->left, 
									(SInt16)selectionRectPtr->top);
					 LineTo ((SInt16)selectionRectPtr->right, 
									(SInt16)selectionRectPtr->top);
					 LineTo ((SInt16)selectionRectPtr->right, 
									(SInt16)selectionRectPtr->bottom);
					 LineTo ((SInt16)selectionRectPtr->left, 
									(SInt16)selectionRectPtr->bottom);
					 LineTo ((SInt16)selectionRectPtr->left, 
									(SInt16)selectionRectPtr->top);
				#endif	// defined multispec_mac 

				#if defined multispec_win                 
					gCDCPointer->Rectangle ((int)selectionRectPtr->left,
													(int)selectionRectPtr->top,
													(int)selectionRectPtr->right,
													(int)selectionRectPtr->bottom);
				#endif	// defined multispec_win

				#if defined multispec_lin      
							// Draw rectangle such that the right side is one pixel outside of
							// the right and bottom border.
				
					int width = 
								(int)selectionRectPtr->right - (int)selectionRectPtr->left + 2;
					int height = 
								(int)selectionRectPtr->bottom - (int)selectionRectPtr->top + 2;
				
					gCDCPointer->SetBrush (*wxTRANSPARENT_BRUSH);
					//gCDCPointer->SetLogicalFunction (wxXOR);
					//gCDCPointer->SetPen (wxPen (*wxWHITE, 1, wxPENSTYLE_DOT));
						 
					//wxPen pen (*wxWHITE, 1, wxPENSTYLE_SHORT_DASH);
					//wxDash dashX[] = {0x00, 0xFF};
					//pen.SetDashes (1, dashX);
					//gCDCPointer->SetPen (pen);
				 
					gCDCPointer->SetPen (wxPen (*wxWHITE, 1, wxPENSTYLE_SHORT_DASH));
					gCDCPointer->DrawRectangle ((int)selectionRectPtr->left, 
															(int)selectionRectPtr->top, 
															width, 
															height);
					gCDCPointer->SetPen (wxPen (*wxBLACK, 1, wxPENSTYLE_SHORT_DASH));
					gCDCPointer->DrawRectangle ((int)selectionRectPtr->left+1, 
															(int)selectionRectPtr->top+1, 
															width-2, 
															height-2);                
				#endif

				}	// end "if (selectionRectPtr->left <= viewRectPtr->right && ..." 

			selectionRectPtr->left += gChannelWindowInterval;
			selectionRectPtr->right += gChannelWindowInterval;

			if (selectionRectPtr->left > viewRectPtr->right)
				break;

			}	// end "for (channel=gStartChannel; channel<..." 

		}	// end "if ((selectionRect.top >= viewRectPtr->top && ..."  

}	// end "DrawSelectionRectangle"



#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawSelectionUnitsPopUp
//
//	Software purpose:	The purpose of this routine is to draw the 
//							selection units selection for the
//							selection units pop up menu.  This routine 
//							will also draw a drop shadow box around the 
//							pop up selection.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/23/2004
//	Revised By:			Larry L. Biehl			Date: 11/23/2004	

pascal void DrawSelectionUnitsPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)
				
{
			// Use the generic pop up drawing routine.									

	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpSelectionDisplayUnitsMenu,
								gSelectionDisplayUnits, 
								TRUE);

}	// end "DrawSelectionUnitsPopUp"
#endif	// defined multispec_mac 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean EditSelectionDialog
//
//	Software purpose:	The purpose of this routine is to present a dialog
//							to allow the user to edit the coordinates that 
//							describe a field or polygon. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			Menus in menus.c
//							StatisticsWControlEvent in SStatist.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/06/1989
//	Revised By:			Larry L. Biehl			Date: 03/17/2005

Boolean EditSelectionDialog (
				SInt16								pointType,
				Boolean								fromStatisticsWindowFlag)
				
{
	DoubleRect							coordinateRectangle;
	LongPoint							lineColPoint;
	LongRect								selectionRectangle;

	WindowInfoPtr						windowInfoPtr;
	WindowPtr							windowPtr;

	Handle								windowInfoHandle = NULL;

	SInt32								lowerRightColumn,
											lowerRightLine,
											upperLeftColumn,
											upperLeftLine;

	SInt16								stringID,
											unitsDisplayCode;

	Boolean								applyToAllWindowsFlag = FALSE,
											changedFlag = FALSE,
											OKFlag = FALSE,
											selectionExistsFlag = FALSE,
											useStartLineColumnFlag = TRUE;


			// Get a pointer to the window information structure to be used.

	if (fromStatisticsWindowFlag) 
		{
		windowPtr = gProjectSelectionWindow;
		windowInfoHandle = gProjectInfoPtr->windowInfoHandle;

		}	// end "if (fromStatisticsWindowFlag)"

	else	// !fromStatisticsWindowFlag
		{
		windowPtr = gActiveImageWindow;
		windowInfoHandle = GetWindowInfoHandle (gActiveImageWindow);

		}	// end "else !fromStatisticsWindowFlag"

	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle, kLock);

	if (windowInfoPtr == NULL)
																							return (FALSE);

	unitsDisplayCode = kLineColumnUnits;

			// Now get the initialize line and column parameters.
			// Get the line/column coordinates for the current selection.

	if (fromStatisticsWindowFlag) 
		{
		SInt16 numberPoints = GetNumberListLines (gStatisticsListHandle);

		if (numberPoints == 2) 
			{
			GetLineColFromList (0, &lineColPoint);
			upperLeftLine = lineColPoint.v;
			upperLeftColumn = lineColPoint.h;

			GetLineColFromList (1, &lineColPoint);
			lowerRightLine = lineColPoint.v;
			lowerRightColumn = lineColPoint.h;

			selectionExistsFlag = TRUE;

			}	// end "if (numberPoints == 2)"

		}	// end "if (fromStatisticsWindowFlag)"

    else	// !fromStatisticsWindowFlag
		{
		selectionExistsFlag = GetSelectedAreaInfo (gActiveImageWindow,
																 windowInfoPtr,
																 &upperLeftLine,
																 &lowerRightLine,
																 &upperLeftColumn,
																 &lowerRightColumn,
																 FALSE,
																 FALSE,
																 FALSE);

		if (selectionExistsFlag)
			unitsDisplayCode = GetSelectionInfoDisplayUnits (windowInfoPtr);

		}	// end "else !fromStatisticsWindowFlag"

	if (!selectionExistsFlag) 
		{
		upperLeftLine = 1;
		upperLeftColumn = 1;

		lowerRightLine = 1;
		lowerRightColumn = 1;

		}	// end "if (!selectionExistFlag)"

    selectionRectangle.top = upperLeftLine;
    selectionRectangle.bottom = lowerRightLine;
    selectionRectangle.left = upperLeftColumn;
    selectionRectangle.right = lowerRightColumn;

    stringID = IDS_Dialog26;

    if (EditLineColumnDialog (windowPtr,
										windowInfoPtr,
										&selectionRectangle,
										&coordinateRectangle,
										pointType,
										&unitsDisplayCode,
										&changedFlag,
										&applyToAllWindowsFlag,
										&useStartLineColumnFlag,
										stringID)) 
		{
		SetSelectionInfoDisplayUnits (windowInfoPtr, unitsDisplayCode);

		if (!selectionExistsFlag)
			changedFlag = TRUE;

		if (changedFlag || applyToAllWindowsFlag) 
			{
					// A selected area was created or changed. Erase the old selection area
					// and set the new area in the selection information structure.

			if (windowPtr != NULL) 
				{
				EditSelectionDialogShowSelection (windowPtr,
																windowInfoHandle,
																&selectionRectangle,
																&coordinateRectangle,
																applyToAllWindowsFlag,
																useStartLineColumnFlag,
																unitsDisplayCode,
																1);

				ShowGraphSelection ();

				}	// end "if (windowPtr != NULL)"

			if (gProjectInfoPtr != NULL &&
					gProjectInfoPtr->statsWindowMode == kSelectFieldMode &&
						(fromStatisticsWindowFlag ||
								(gActiveImageWindow == gProjectSelectionWindow &&
										gProjectInfoPtr->selectionType == kRectangleType))) 
				{
				LoadRectangleInStatList (&selectionRectangle);

				gProjectInfoPtr->lastPointType = kRectangleType;

						// Hilite the "Add to List" control since a field is selected		

				if (gProjectWindow != NULL)
					MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 0);

				}	// end "if (fromStatisticsWindowFlag || ..."

			gUpdateFileMenuItemsFlag = TRUE;
			gUpdateEditMenuItemsFlag = TRUE;

			}	// end "if (changedFlag || applyToAllWindowsFlag)" 

		}	// end "if (EditLineColumnDialog (..."

	CheckAndUnlockHandle (windowInfoHandle);

	return (OKFlag);

}	// end "EditSelectionDialog" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void EditSelectionDialogShowSelection
//
//	Software purpose:	The purpose of this routine is to show the selection in all of
//							the requested windows. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			EditSelectionDialog in SSelectionUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/24/2004
//	Revised By:			Larry L. Biehl			Date: 05/29/2015

void EditSelectionDialogShowSelection (
				WindowPtr							windowPtr,
				Handle								windowInfoHandle,
				LongRect*							inputSelectionRectanglePtr,
				DoubleRect*							inputCoordinateRectanglePtr,
				Boolean								applyToAllWindowsFlag,
				Boolean								useStartLineColumnFlag,
				SInt16								selectionUnitsCode,
				double								inputFactor)

{
	DoubleRect							coordinateRectangle;

	DisplaySpecsPtr					displaySpecsPtr;
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	//PlanarCoordinateSystemInfoPtr	planarCoordinateSystemInfoPtr;
	SelectionInfoPtr					selectionInfoPtr;

	Handle								displaySpecsH,
											imageWindowInfoHandle,
											mapProjectionHandle,
											selectionInfoHandle,
											selectionWindowInfoHandle;

	SignedByte							displayHandleStatus,
											selectionHandleStatus;


			// Image window available for the selection.

	ClearSelectionArea (windowPtr);

	imageWindowInfoHandle = GetWindowInfoHandle (windowPtr);

	selectionInfoHandle = GetSelectionInfoHandle (imageWindowInfoHandle);
	selectionInfoPtr = (SelectionInfoPtr)GetHandleStatusAndPointer (
																				selectionInfoHandle,
																				&selectionHandleStatus);

	displaySpecsH = GetDisplaySpecsHandle (imageWindowInfoHandle);
	displaySpecsPtr = (DisplaySpecsPtr)GetHandleStatusAndPointer (
																				displaySpecsH,
																				&displayHandleStatus);

			// Now make sure that we have the selection in line-column units.

	ConvertCoordinateRectToLCRect (imageWindowInfoHandle,
												inputCoordinateRectanglePtr,
												inputSelectionRectanglePtr,
												selectionUnitsCode,
												inputFactor);

			// Get the selected area coordinate units based on the line column
			// selection an in the correct units for displaying in the coordinate
			// view.

			// Need to allow for windowInfoHandle being different from that
			// for windowPtr. The windowPtr could be that for the selection project
			// window.  WindowInfoHandle will be for that representing the base 
			// image for the project so that areas selected for training and test 
			// will refer to the base image and not the selection window (in case 
			// they are different)

	//planarCoordinateSystemInfoPtr = NULL;
	mapProjectionHandle = GetFileMapProjectionHandle2 (imageWindowInfoHandle);

	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																					mapProjectionHandle);

	//if (mapProjectionInfoPtr != NULL)											
	//	planarCoordinateSystemInfoPtr = &mapProjectionInfoPtr->planarCoordinate;

	selectionWindowInfoHandle = GetWindowInfoHandle (windowPtr);
	ComputeSelectionCoordinates (selectionWindowInfoHandle,
											mapProjectionInfoPtr,
											inputSelectionRectanglePtr,
											&coordinateRectangle);

	SetSelectionInformation (windowPtr,
										displaySpecsPtr,
										selectionInfoPtr,
										kRectangleType,
										0,
										inputSelectionRectanglePtr,
										&coordinateRectangle);

	MHSetState (displaySpecsH, displayHandleStatus);

			// Outline the selection.				

	OutlineSelectionArea (windowPtr);

	#if defined multispec_mac  
		InvalidateWindow (gActiveImageWindow, kCoordinateSelectionArea, FALSE);
	#endif	// defined multispec_mac 


	#if defined multispec_win || defined multispec_lin
		ShowSelection (windowPtr);
	#endif	// defined multispec_win || defined multispec_lin 

			// Set this selection for all image windows if requested. If the shift key
			// was down when the selection was made, then do not use the start line
			// and column in calculating the selection coordinates in the other windows.
	/*	
	useStartLineColumnFlag = TRUE;

	#if defined multispec_mac
		if (gEventRecord.modifiers & shiftKey)
			useStartLineColumnFlag = FALSE;
	#endif	// defined multispec_mac 

	#if defined multispec_win
		if (GetKeyState (VK_SHIFT) & 0x8000)
			useStartLineColumnFlag = FALSE;
	#endif	// defined multispec_win 
	*/
	if (applyToAllWindowsFlag)
	  SetSelectionForAllWindows (windowInfoHandle,
											selectionInfoPtr,
											inputCoordinateRectanglePtr,
											useStartLineColumnFlag,
											selectionUnitsCode,
											inputFactor);

	MHSetState (selectionInfoHandle, selectionHandleStatus);

}	// end "EditSelectionDialogShowSelection" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void EditSelectionDialogSetCoordinates
//
//	Software purpose:	The purpose of this routine is to set the coordinates in the
//							selection dialog box to the units requested by the user. The
//							map units will be displayed in the native units for the image.
//							So far this can be meters, feet, survey feet or decimal degrees.
//							One needs to take into account though that the coversion of
//							map units to decimal degrees is always done with meters.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			EditLineColumnDialog in SSelectionUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/23/2004
//	Revised By:			Larry L. Biehl			Date: 04/28/2012

void EditSelectionDialogSetCoordinates (
				DialogPtr							dialogPtr,
				Handle								windowInfoHandle,
				LongRect*							inputSelectionRectanglePtr,
				LongRect*							selectionRectanglePtr,
				DoubleRect*							inputCoordinateRectanglePtr,
				DoubleRect*							coordinateRectanglePtr,
				LongRect*							minMaxSelectionRectanglePtr,
				DoubleRect*							minMaxCoordinateRectanglePtr,
				SInt16								requestedSelectionUnitsCode,
				SInt16								currentSelectionUnitsCode)
				
{
	double								factor,
											mapToMetersFactor;

	SInt16								fileMapUnitsIndex,
											numberDecimals,
											projectionCode,
											referenceSystemCode,
											updateViewUnits = 0,
											viewUnits;


	referenceSystemCode = GetFileReferenceSystemCode (windowInfoHandle);
	projectionCode = GetFileProjectionCode (windowInfoHandle);

	fileMapUnitsIndex = GetFilePlanarMapUnitsCode (windowInfoHandle) - kKilometersCode;
	mapToMetersFactor = 1;
	if (fileMapUnitsIndex > 1)
		mapToMetersFactor = gDistanceFileConversionFactors[fileMapUnitsIndex] /
													gDistanceFileConversionFactors[1]; // kMeters

	factor = 1;
	if (currentSelectionUnitsCode == kLineColumnUnits) 
		{
		if (requestedSelectionUnitsCode == kMapUnits ||
													requestedSelectionUnitsCode == kLatLongUnits) 
			{
			viewUnits = kMapUnits;
			if (requestedSelectionUnitsCode == kLatLongUnits) 
				{
				viewUnits = kDecimalLatLongUnitsMenuItem;
				factor = mapToMetersFactor;

				}	// end "if (requestedSelectionUnitsCode == kLatLongUnits)"

			ComputeMapCoordinates (windowInfoHandle,
										  factor,
										  viewUnits,
										  selectionRectanglePtr,
										  coordinateRectanglePtr);

			updateViewUnits = viewUnits;

			}	// end "if (requestedSelectionUnitsCode == kMapUnits)"

		}	// end "if (currentSelectionUnitsCode == kLineColumnUnits)"

	else if (currentSelectionUnitsCode == kMapUnits) 
		{
		if (requestedSelectionUnitsCode == kLineColumnUnits) 
			{
			if (referenceSystemCode == kGeographicRSCode)
				ConvertMapRectToLatLongRect (windowInfoHandle,
														  projectionCode,
														  coordinateRectanglePtr);

			ConvertMapRectToLCRect (windowInfoHandle,
											  coordinateRectanglePtr,
											  selectionRectanglePtr,
											  1);

			}	// end "if (requestedSelectionUnitsCode == kLineColumnUnits)"

		else if (requestedSelectionUnitsCode == kLatLongUnits) 
			{
			ConvertMapRectByGivenFactor (mapToMetersFactor, coordinateRectanglePtr);

			ConvertMapRectToLatLongRect (windowInfoHandle,
													projectionCode,
													coordinateRectanglePtr);

			updateViewUnits = kDecimalLatLongUnitsMenuItem;

			}	// end "else if (requestedSelectionUnitsCode == kLatLongUnits)"

		}	// end "else if (currentSelectionUnitsCode == kMapUnits)"

	else if (currentSelectionUnitsCode == kLatLongUnits) 
		{
		if (requestedSelectionUnitsCode == kLineColumnUnits) 
			{
			if (referenceSystemCode != kGeographicRSCode)
				 ConvertLatLongRectToMapRect (windowInfoHandle, coordinateRectanglePtr);

			ConvertMapRectToLCRect (windowInfoHandle,
											  coordinateRectanglePtr,
											  selectionRectanglePtr,
											  1);

			}	// end "if (requestedSelectionUnitsCode == kLineColumnUnits)"

		else if (requestedSelectionUnitsCode == kMapUnits) 
			{
			ConvertLatLongRectToMapRect (windowInfoHandle, coordinateRectanglePtr);

			updateViewUnits = kMapUnits;

			}	// end "else if (requestedSelectionUnitsCode == kMapUnits)"

		}	// end "else if (currentSelectionUnitsCode == kMapUnits)"

	factor = 1;
	if (updateViewUnits == kDecimalLatLongUnitsMenuItem)
	  factor = mapToMetersFactor;

			// Make sure that the min and max map coordinates are in the proper units.

	if (updateViewUnits != 0)
	  ComputeMapCoordinates (windowInfoHandle,
										factor,
										updateViewUnits,
										minMaxSelectionRectanglePtr,
										minMaxCoordinateRectanglePtr);

			// Make sure the input coordinates are in the proper units.

	if (updateViewUnits != 0)
	  ComputeMapCoordinates (windowInfoHandle,
										factor,
										updateViewUnits,
										inputSelectionRectanglePtr,
										inputCoordinateRectanglePtr);

	if (requestedSelectionUnitsCode == kLineColumnUnits) 
		{
				// Now present the line-column coordinates to the user.				

		LoadDItemValue (dialogPtr, IDC_NewLineStart, selectionRectanglePtr->top);
		LoadDItemValue (dialogPtr, IDC_NewLineEnd, selectionRectanglePtr->bottom);
		LoadDItemValue (dialogPtr, IDC_NewColumnStart, selectionRectanglePtr->left);
		LoadDItemValue (dialogPtr, IDC_NewColumnEnd, selectionRectanglePtr->right);

				// Now present the current line-column coordinates to the user.				

		LoadDItemValue (
					dialogPtr, IDC_CurrentLineStart, inputSelectionRectanglePtr->top);
		LoadDItemValue (
					dialogPtr, IDC_CurrentLineEnd, inputSelectionRectanglePtr->bottom);
		LoadDItemValue (
					dialogPtr, IDC_CurrentColumnStart, inputSelectionRectanglePtr->left);
		LoadDItemValue (
					dialogPtr, IDC_CurrentColumnEnd, inputSelectionRectanglePtr->right);

		if (gDialogItemDescriptorPtr != NULL) 
			{
			gDialogItemDescriptorPtr[3] = 0;
			gDialogItemDescriptorPtr[4] = 0;
			gDialogItemDescriptorPtr[5] = 0;
			gDialogItemDescriptorPtr[6] = 0;

			}	// end "if (gDialogItemDescriptorPtr != NULL)"

		}	// end "if (requestedSelectionUnitsCode == kLineColumnUnits)"

	else	// requestedSelectionUnitsCode != kLineColumnUnits
		{
		numberDecimals = 4;
		if (requestedSelectionUnitsCode == kLatLongUnits)
			numberDecimals = 8;

				// Now present the map coordinates to the user.				

		LoadDItemRealValue (dialogPtr,
									IDC_NewLineStart,
									coordinateRectanglePtr->bottom, 
									numberDecimals);
		LoadDItemRealValue (dialogPtr,
									IDC_NewLineEnd,
									coordinateRectanglePtr->top, 
									numberDecimals);
		LoadDItemRealValue (dialogPtr,
									IDC_NewColumnStart,
									coordinateRectanglePtr->left, 
									numberDecimals);
		LoadDItemRealValue (dialogPtr,
									IDC_NewColumnEnd,
									coordinateRectanglePtr->right, 
									numberDecimals);

				// Load current map coordinate values.								

		LoadDItemRealValue (dialogPtr,
									IDC_CurrentLineStart,
									inputCoordinateRectanglePtr->bottom, 
									numberDecimals);
		LoadDItemRealValue (dialogPtr,
									IDC_CurrentLineEnd,
									inputCoordinateRectanglePtr->top, 
									numberDecimals);
		LoadDItemRealValue (dialogPtr,
									IDC_CurrentColumnStart,
									inputCoordinateRectanglePtr->left, 
									numberDecimals);
		LoadDItemRealValue (dialogPtr,
									IDC_CurrentColumnEnd,
									inputCoordinateRectanglePtr->right, 
									numberDecimals);

		if (gDialogItemDescriptorPtr != NULL) 
			{
			gDialogItemDescriptorPtr[3] = kDItemReal + kDItemMinus;
			gDialogItemDescriptorPtr[4] = kDItemReal + kDItemMinus;
			gDialogItemDescriptorPtr[5] = kDItemReal + kDItemMinus;
			gDialogItemDescriptorPtr[6] = kDItemReal + kDItemMinus;

			}	// end "if (gDialogItemDescriptorPtr != NULL)" 

		}	// end "else requestedSelectionUnitsCode != kLineColumnUnits"

	SelectDialogItemText (dialogPtr, IDC_NewLineStart, 0, SInt16_MAX);

}	// end "EditSelectionDialogSetCoordinates" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean EditLineColumnDialog
//
//	Software purpose:	The purpose of this routine is to present a dialog
//							to allow the user to edit the coordinates that 
//							describe a field or polygon. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			StatisticsWControlEvent   in statistics.c
//
//	Coded By:			Larry L. Biehl			Date: 02/16/1998
//	Revised By:			Larry L. Biehl			Date: 12/16/2016

Boolean EditLineColumnDialog (
				WindowPtr							windowPtr,
				WindowInfoPtr						windowInfoPtr,
				LongRect*							inputSelectionRectanglePtr,
				DoubleRect*							coordinateRectanglePtr,
				SInt16								pointType,
				SInt16*								unitsDisplayCodePtr,
				Boolean*								changedFlagPtr,
				Boolean*								applyToAllWindowsFlagPtr,
				Boolean*								useStartLineColumnFlagPtr,
				SInt16								stringID)

{
	LongRect								selectionRectangle;

	double								factor;

	Boolean								OKFlag = FALSE,
											previewWasUsedFlag = FALSE;


	selectionRectangle = *inputSelectionRectanglePtr;

	#if defined multispec_mac   
		DoubleRect							inputCoordinateRectangle,
												minMaxCoordinateRectangle;

		LongRect								minMaxSelectionRectangle;
		Rect									theBox;

		double								theRealNum;

		DialogPtr							dialogPtr;

		UserItemUPP							drawSelectionUnitsPtr;

		Handle								okHandle,
												theHandle;

		SInt32								theNum;

		SInt16								itemHit,
												popupItemHit,
												theType,
												valueItemHit;

		Boolean								errorAlertFlag,
												modalDone,
												valueChangedFlag;


		*changedFlagPtr = FALSE;

				// Get the modal dialog for the edit coordinates specification.	

		dialogPtr = LoadRequestedDialog (kEditCoordinatesID, kCopyScrap, 1, 2);
		if (dialogPtr == NULL)
																							return (FALSE);

				// Intialize local user item proc pointers.	

		drawSelectionUnitsPtr = NewUserItemUPP (DrawSelectionUnitsPopUp);

				// Set Procedure pointers for those dialog items that need them.

		SetDialogItemDrawRoutine (dialogPtr, 22, drawSelectionUnitsPtr);

		EditLineColumnDialogInitialize (dialogPtr,
													windowPtr,
													windowInfoPtr,
													pointType,
													*unitsDisplayCodePtr,
													inputSelectionRectanglePtr,
													&selectionRectangle,
													coordinateRectanglePtr,
													&minMaxSelectionRectangle,
													&minMaxCoordinateRectangle,
													&inputCoordinateRectangle,
													applyToAllWindowsFlagPtr,
													useStartLineColumnFlagPtr,
													&gSelectionDisplayUnits,
													stringID);

				// Save handle for the OK button for use later.								

		GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);

		SelectDialogItemText (dialogPtr, 3, 0, INT16_MAX);

				// Center the dialog and then show it.											

		ShowDialogWindow (dialogPtr, kEditCoordinatesID, kSetUpDFilterTable);

		if (*unitsDisplayCodePtr != kLineColumnUnits) 
			{
			gDialogItemDescriptorPtr[3] = kDItemReal + kDItemMinus;
			gDialogItemDescriptorPtr[4] = kDItemReal + kDItemMinus;
			gDialogItemDescriptorPtr[5] = kDItemReal + kDItemMinus;
			gDialogItemDescriptorPtr[6] = kDItemReal + kDItemMinus;

			}	// end "if (*unitsDisplayCodePtr != kLineColumnUnits)"

		valueChangedFlag = FALSE;
		modalDone = FALSE;
		itemHit = 0;
		do 
			{
			ModalDialog (gProcessorDialogFilterPtr, &itemHit);
			if ((itemHit < 2 || itemHit > 6) && itemHit != 7 && itemHit != 23) 
				{
				if (valueChangedFlag) 
					{
					valueItemHit = EditLineColumnDialogCheckCoordinates (
																	dialogPtr,
																	windowInfoPtr->windowInfoHandle,
																	gSelectionDisplayUnits,
																	&selectionRectangle,
																	&minMaxSelectionRectangle,
																	coordinateRectanglePtr,
																	&minMaxCoordinateRectangle,
																	coordinateRectanglePtr->left,
																	coordinateRectanglePtr->right,
																	coordinateRectanglePtr->bottom,
																	coordinateRectanglePtr->top);

					if (valueItemHit != 0) 
						{
						itemHit = valueItemHit;
						SelectDialogItemText (dialogPtr, itemHit, 0, INT16_MAX);

						}	// end "if (valueItemHit != 0)"

					else	// valueItemHit == 0
						valueChangedFlag = FALSE;

					}	// end "if (valueChangedFlag)"

				}	// end "if ((itemHit < 2 || itemHit > 6) && ..."

			if (itemHit > 2) 
				{
						// If itemHit was a number item, check for bad values.			
						// Get the handle to the itemHit.										
						// For number value items, get the string and convert it to		
						// a number.																	

				GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
				if (theType == 16) 
					{
					GetDialogItemText (theHandle, gTextString);
					StringToNum (gTextString, &theNum);

					}	// end "if (theType == 16)"  

				if (itemHit >= 3 && itemHit <= 6) 
					{
					errorAlertFlag = FALSE;

					if (gSelectionDisplayUnits == kLineColumnUnits) 
						{
						valueChangedFlag = TRUE;

						switch (itemHit) 
							{
							case 3: // New upper-left line or new line 
							case 4: // New lower-right line 
								if (theNum < minMaxSelectionRectangle.top) 
									{
									errorAlertFlag = TRUE;
									theNum = minMaxSelectionRectangle.top;

									}	// end "if (theNum < minMaxSelectionRectangle.top)"

								else if (theNum > minMaxSelectionRectangle.bottom) 
									{
									errorAlertFlag = TRUE;
									theNum = minMaxSelectionRectangle.bottom;

									}	// end "else if (theNum > ..."

								if (itemHit == 3)
									selectionRectangle.top = theNum;
								else	// itemHit == 4
									selectionRectangle.bottom = theNum;
								break;

							case 5: // New upper-left column or new column 
							case 6: // New lower-right column 
								if (theNum < minMaxSelectionRectangle.left) 
									{
									errorAlertFlag = TRUE;
									theNum = minMaxSelectionRectangle.left;

									}	// end "if (theNum < minMaxSelectionRectangle.top)"

								else if (theNum > minMaxSelectionRectangle.right) 
									{
									errorAlertFlag = TRUE;
									theNum = minMaxSelectionRectangle.right;

									}	// end "else if (theNum > minMaxSelectionRectangle.right)"

								if (itemHit == 5)
									selectionRectangle.left = theNum;
								else	// itemHit == 6
									selectionRectangle.right = theNum;
								break;

							}	// end "switch (itemHit)"

						if (errorAlertFlag)
							NumberErrorAlert (theNum, dialogPtr, itemHit);

						}	// end "if (gSelectionDisplayUnits == kLineColumnUnits)"

					else	// if (gSelectionDisplayUnits != kLineColumnUnits)
						{
						valueChangedFlag = TRUE;
						theRealNum = GetDItemRealValue (dialogPtr, itemHit);

						switch (itemHit) 
							{
							case 3: // New upper coordinate value
								 coordinateRectanglePtr->bottom = theRealNum;
								 break;

							case 4: // New lower coordinate value 
								 coordinateRectanglePtr->top = theRealNum;
								 break;

							case 5: // New left coordinate value 
								 coordinateRectanglePtr->left = theRealNum;
								 break;

							case 6: // New right coordinate value 
								 coordinateRectanglePtr->right = theRealNum;
								 break;

							}	// end "switch (itemHit)"

						}	// end "else gSelectionDisplayUnits != kLineColumnUnits"

					}	// end "if (itemHit >= 3 && itemHit <= 6)"

				else	// itemHit > 6
					{
					switch (itemHit) 
						{
						case 7: // Apply to all image windows
							ChangeDLogCheckBox ((ControlHandle) theHandle);
							*applyToAllWindowsFlagPtr = !*applyToAllWindowsFlagPtr;

							EditLineColumnDialogSetStartLC (dialogPtr,
																		gSelectionDisplayUnits,
																		*applyToAllWindowsFlagPtr);
							break;

						case 8: // Apply to windows now
							EditSelectionDialogShowSelection (
																	windowPtr,
																	windowInfoPtr->windowInfoHandle,
																	&selectionRectangle,
																	coordinateRectanglePtr,
																	*applyToAllWindowsFlagPtr,
																	*useStartLineColumnFlagPtr,
																	gSelectionDisplayUnits,
																	1);

							previewWasUsedFlag = TRUE;
							break;

						case 22: // Set units to use to enter the selection
							popupItemHit = StandardPopUpMenu (
																	dialogPtr,
																	21,
																	22,
																	gPopUpSelectionDisplayUnitsMenu,
																	gSelectionDisplayUnits,
																	kPopUpSelectionDisplayUnitsMenuID);

							if (popupItemHit > 0 &&
															gSelectionDisplayUnits != popupItemHit) 
								{
								 EditLineColumnDialogSetStartLC (dialogPtr,
																			popupItemHit,
																			*applyToAllWindowsFlagPtr);

								 EditSelectionDialogSetCoordinates (
																		dialogPtr,
																		windowInfoPtr->windowInfoHandle,
																		inputSelectionRectanglePtr,
																		&selectionRectangle,
																		&inputCoordinateRectangle,
																		coordinateRectanglePtr,
																		&minMaxSelectionRectangle,
																		&minMaxCoordinateRectangle,
																		popupItemHit,
																		gSelectionDisplayUnits);

								gSelectionDisplayUnits = popupItemHit;

								}	// end "if (popupItemHit > 0 && ..."
							break;

						case 23: // use start line and column values for upper left pixel
							ChangeDLogCheckBox ((ControlHandle) theHandle);
							*useStartLineColumnFlagPtr = !*useStartLineColumnFlagPtr;
							break;

						}	// end "switch (itemHit)" 

					}	// end "else itemHit > 6"
				/*
				if (itemHit <= 6)
					{	
					if (gSelectionDisplayUnits == kLineColumnUnits)
						{
								// Make certain the line-column values make sense.	
								// Turn the okay button off is they do not.				

						if ((selectionRectangle.top > selectionRectangle.bottom) || 
											(selectionRectangle.left > selectionRectangle.right))
							hiliteSetting = 255;

						else	// (newUpperLeftLine <= newLowerRightLine) &&
								//						(newUpperLeftColumn <= newLowerRightColumn) 
							hiliteSetting = 0;

						}	// end "if (gSelectionDisplayUnits == kLineColumnUnits)"

					else	// if (gSelectionDisplayUnits != kLineColumnUnits)
						{
								// Make certain the map values make sense.					

						if ((coordinateRectanglePtr->top < coordinateRectanglePtr->bottom) || 
								(coordinateRectanglePtr->left > coordinateRectanglePtr->right))
							hiliteSetting = 255;

						else	// (coordinateRectanglePtr->top >= ... .bottom) && ...
							hiliteSetting = 0;

						}	// end "else gSelectionDisplayUnits != kLineColumnUnits"

					HiliteControl ((ControlHandle)okHandle, hiliteSetting);
					SetDLogControlHilite (dialogPtr, 8, hiliteSetting);

					}		// end "if (itemHit <= 6)"
				*/
				}	// end "if (itemHit > 2)" 

			else if (itemHit == 1) // User selected OK for information 
				{
				OKFlag = TRUE;
				modalDone = TRUE;

				*unitsDisplayCodePtr = gSelectionDisplayUnits;

				}	// end "if (itemHit == 1)" 

			if (itemHit == 2) // User selected Cancel for information 
				{
				modalDone = TRUE;
				OKFlag = FALSE;

				}	// end "if	(itemHit == 2)" 

			}	while (!modalDone);

		DisposeUserItemUPP (drawSelectionUnitsPtr);

		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	#endif	// defined multispec_mac 

	#if defined multispec_win 			
		CMEditCoordinatesDlg* dialogPtr = NULL;


		TRY
			{
			dialogPtr = new CMEditCoordinatesDlg ();

			OKFlag = dialogPtr->DoDialog (windowPtr,
													windowInfoPtr,
													inputSelectionRectanglePtr,
													&selectionRectangle,
													coordinateRectanglePtr,
													pointType,
													unitsDisplayCodePtr,
													changedFlagPtr,
													applyToAllWindowsFlagPtr,
													useStartLineColumnFlagPtr,
													&previewWasUsedFlag,
													stringID);

			delete dialogPtr;
			
			}

		CATCH_ALL (e) 
			{
			MemoryMessage (0, kObjectMessage);
			
			}
		END_CATCH_ALL
	#endif	// defined multispec_win  

	#if defined multispec_lin			
		CMEditCoordinatesDlg* dialogPtr = NULL;

		try
			{
			dialogPtr = new CMEditCoordinatesDlg ();

			OKFlag = dialogPtr->DoDialog (windowPtr,
													windowInfoPtr,
													inputSelectionRectanglePtr,
													&selectionRectangle,
													coordinateRectanglePtr,
													pointType,
													unitsDisplayCodePtr,
													changedFlagPtr,
													applyToAllWindowsFlagPtr,
													useStartLineColumnFlagPtr,
													&previewWasUsedFlag,
													stringID);

			delete dialogPtr;
			
			}

		catch (int e) 
			{
			MemoryMessage (0, kObjectMessage);
			
			}    
	#endif	// defined multispec_win  

	if (OKFlag)
			// Check if the selected area changed.
		EditLineColumnDialogOK (windowInfoPtr->windowInfoHandle,
										inputSelectionRectanglePtr,
										&selectionRectangle,
										coordinateRectanglePtr,
										*unitsDisplayCodePtr,
										changedFlagPtr);

	else	// !OKFlag
		{
		if (previewWasUsedFlag) 
			{
					// If a new selection was previewed then make sure that we go back
					// to the original input selection.

					// Make sure the input coordinates are in the proper units.

			if (*unitsDisplayCodePtr != kLineColumnUnits)
				ComputeMapCoordinates (windowInfoPtr->windowInfoHandle,
												1,
												*unitsDisplayCodePtr,
												inputSelectionRectanglePtr,
												coordinateRectanglePtr);

					// Get the factor that is being used to convert the original units to
					// the requested display units.

			factor = GetCoordinateViewFactor (windowInfoPtr->windowInfoHandle);

			EditSelectionDialogShowSelection (windowPtr,
															windowInfoPtr->windowInfoHandle,
															inputSelectionRectanglePtr,
															coordinateRectanglePtr,
															*applyToAllWindowsFlagPtr,
															*useStartLineColumnFlagPtr,
															*unitsDisplayCodePtr,
															factor);

			}	// end "if (previewWasUsedFlag)"

		}	// end "else !OKFlag"

	return (OKFlag);

}	// end "EditLineColumnDialog"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void EditLineColumnDialogInitialize
//
//	Software purpose:	The purpose of this routine is to check to see if the selected
//							area has changed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None 		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/14/2005
//	Revised By:			Larry L. Biehl			Date: 03/02/2017	

void EditLineColumnDialogInitialize (
				DialogPtr							dialogPtr,
				WindowPtr							windowPtr,
				WindowInfoPtr						windowInfoPtr,
				SInt16								pointType,
				SInt16								unitsDisplayCode,
				LongRect*							inputSelectionRectanglePtr,
				LongRect*							selectionRectanglePtr,
				DoubleRect*							coordinateRectanglePtr,
				LongRect*							minMaxSelectionRectanglePtr,
				DoubleRect*							minMaxCoordinateRectanglePtr,
				DoubleRect*							inputCoordinateRectanglePtr,
				Boolean*								applyToAllWindowsFlagPtr,
				Boolean*								useStartLineColumnFlagPtr,
				SInt16*								selectionDisplayUnitsPtr,
				SInt16								stringID)
 
{
	Str255								mapString;

	SInt16								endIndex,
											gridCodeMenuItem,
											planarMapUnitsCode,
											projectionCode,
											startIndex;

	Boolean								inverseLatLongAvailableFlag;

	#if defined multispec_win  
		CComboBox*							comboBoxPtr;


		USES_CONVERSION;
	#endif

   planarMapUnitsCode = GetFilePlanarMapUnitsCode (windowInfoPtr->windowInfoHandle);
   projectionCode = GetFileProjectionCode (windowInfoPtr->windowInfoHandle);

			// Determine if inverse lat-long specification will be available.

   inverseLatLongAvailableFlag =
					DetermineIfInverseLatLongPossible (windowInfoPtr->windowInfoHandle);

			// Get popup menu (list menu for windows) strings

   if (planarMapUnitsCode < kKilometersCode)
       CtoPstring ((UCharPtr)"Map", mapString);

   else	// planarMapUnitsCode >= kKilometersCode
		{
				// This allows for MultiSpec Geographic code which is -1

       gridCodeMenuItem = projectionCode;
       if (gridCodeMenuItem > 0)
			gridCodeMenuItem++;
       else if (gridCodeMenuItem < 0)
			gridCodeMenuItem = 1;

       if (gridCodeMenuItem <= 0)
			mapString[0] = sprintf ((char*) &mapString[1], "Map");

       else	// gridCodeMenuItem > 0
			{
			#if defined multispec_mac 
				GetMenuItemText (gPopUpProjectionMenu,
										gridCodeMenuItem,
										mapString);
			#endif	// defined multispec_mac  

			#if defined multispec_win  
				MGetString (mapString,
								0,
								IDS_ProjectionType01 + gridCodeMenuItem - 1);
			#endif	// defined multispec_win 

			}	// end "else gridCode > 0"

        endIndex = mapString[0];
        startIndex = endIndex + 1;

			#if defined multispec_mac 
				GetMenuItemText (gPopUpMapUnitsMenu,
										planarMapUnitsCode + 1,
										&mapString[startIndex]);
			#endif	// defined multispec_mac  

			#if defined multispec_win  
				MGetString (&mapString[startIndex],
								 0,
								 IDS_MapUnits01 + planarMapUnitsCode);
			#endif	// defined multispec_win 

			endIndex += mapString[startIndex] + 1;

			mapString[0] = (UInt8)endIndex;
			mapString[endIndex + 1] = 0;

			mapString[startIndex] = '-';

			}	// end "else planarMapUnitsCode >= kKilometersCode"

				//	Now set menu names

		#if defined multispec_mac 
			if (planarMapUnitsCode < kKilometersCode)
				DisableMenuItem (gPopUpSelectionDisplayUnitsMenu, kMapUnits);

			else	// planarMapUnitsCode >= kKilometersCode
				EnableMenuItem (gPopUpSelectionDisplayUnitsMenu, kMapUnits);

			SetMenuItemText (gPopUpSelectionDisplayUnitsMenu,
									kMapUnits,
									mapString);

			DisableMenuItem (gPopUpSelectionDisplayUnitsMenu, kLatLongUnits);
			if (inverseLatLongAvailableFlag)
				EnableMenuItem (gPopUpSelectionDisplayUnitsMenu, kLatLongUnits);
		#endif	// defined multispec_mac  

	#if defined multispec_win  
		SInt16				index = 0,
								numberComboItems;


		comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem (IDC_CoordinateUnits);

				// Remove all but the first item in the list.

		numberComboItems = comboBoxPtr->GetCount ();
		while (numberComboItems > 1) 
			{
			comboBoxPtr->DeleteString (numberComboItems - 1);
			numberComboItems--;

			}	// end "while (numberComboItems > 1)"

		comboBoxPtr->SetItemData (index, kLineColumnUnits);
		index++;

		if (planarMapUnitsCode >= kKilometersCode) 
			{
			comboBoxPtr->InsertString (index, (LPCWSTR)A2T((char*)&mapString[1]));
			comboBoxPtr->SetItemData (index, kMapUnits);
			index++;

			}	// end "if (planarMapUnitsCode >= kKilometersCode)"

		if (inverseLatLongAvailableFlag) 
			{
			comboBoxPtr->InsertString (index, (LPCWSTR)_T("Latitude-Longitude"));
			comboBoxPtr->SetItemData (index, kLatLongUnits);
			index++;

			}	// end "if (inverseLatLongAvailableFlag)"
	#endif	// defined multispec_win

			// Change the window title	

    MGetString ((UCharPtr)&gTextString, kDialogStrID, stringID);
	 
	#if defined multispec_lin
		SetWTitle (GetDialogWindow (dialogPtr), ((UCharPtr)gTextString)+1);
	#else 
		SetWTitle (GetDialogWindow (dialogPtr), (UCharPtr)&gTextString);
	#endif
	
			// Control to allow selection to be applied to all open windows

	*applyToAllWindowsFlagPtr = FALSE;
	HideDialogItem (dialogPtr, IDC_ApplyToAllCheckbox);
	SetDLogControl (dialogPtr, IDC_ApplyToAllCheckbox, 0);

			// Control to allow selection to be previewed

	ShowDialogItem (dialogPtr, IDC_Preview);
	if (windowPtr == NULL)
		HideDialogItem (dialogPtr, IDC_Preview);

			// Control to allow start line and column of other image windows to
			// be used when applying this selection to them.
			// Always hide to start with since this only applies when applying
			// to other windows.

	*useStartLineColumnFlagPtr = TRUE;
	HideDialogItem (dialogPtr, IDC_StartLCCheckBox);
	SetDLogControl (dialogPtr, IDC_StartLCCheckBox, 1);

			// Get the min and max line-column values

	minMaxSelectionRectanglePtr->left = 1;
	minMaxSelectionRectanglePtr->top = 1;
	minMaxSelectionRectanglePtr->right = windowInfoPtr->maxNumberColumns;
	minMaxSelectionRectanglePtr->bottom = windowInfoPtr->maxNumberLines;

			// Present dialog for editing the rectangular type field.

	if (pointType == kRectangleType) 
		{
				// Force map coordinate units to be computed.

		ComputeMapCoordinates (windowInfoPtr->windowInfoHandle,
										 1,
										 kMetersUnitsMenuItem,
										 inputSelectionRectanglePtr,
										 coordinateRectanglePtr);

				// Save the input coordinate rectangle.

		*inputCoordinateRectanglePtr = *coordinateRectanglePtr;	

		EditSelectionDialogSetCoordinates (dialogPtr,
														 windowInfoPtr->windowInfoHandle,
														 inputSelectionRectanglePtr,
														 selectionRectanglePtr,
														 coordinateRectanglePtr,
														 coordinateRectanglePtr,
														 minMaxSelectionRectanglePtr,
														 minMaxCoordinateRectanglePtr,
														 unitsDisplayCode,
														 kLineColumnUnits);

		if (gNumberOfIWindows > 1 && stringID == IDS_Dialog26)
			ShowDialogItem (dialogPtr, IDC_ApplyToAllCheckbox);

		}	// end "if (pointType == kRectangleType)" 

			// Present dialog for editing the field name and changing test/train	
			// field choice.																		

	if (pointType == kPolygonType) 
		{
				// Hide items dealing with rectangular type fields.					
				// item = 4: Edit Text with "lower line value"							
				// item = 6: Edit Text with "right col value"							
				// item = 8: Static Text with "Rectangular Field Coordinate"		
				// item = 13: Static Text with "-"											
				// item = 14: Static Text with "-"											
				// item = 16: Static Text with "lower line value"						
				// item = 18: Static Text with "right col value"						
				// item = 19: Static Text with "-"											
				// item = 20: Static Text with "-"											

		HideDialogItem (dialogPtr, IDC_NewLineDash);
		HideDialogItem (dialogPtr, IDC_NewLineEnd);
		HideDialogItem (dialogPtr, IDC_NewColumnDash);
		HideDialogItem (dialogPtr, IDC_NewColumnEnd);
		HideDialogItem (dialogPtr, IDC_CurrentLineDash);
		HideDialogItem (dialogPtr, IDC_CurrentLineEnd);
		HideDialogItem (dialogPtr, IDC_CurrentColumnDash);
		HideDialogItem (dialogPtr, IDC_CurrentColumnEnd);

				// Load default line-column.												

		LoadDItemValue (dialogPtr, IDC_NewLineStart, selectionRectanglePtr->top);
		LoadDItemValue (dialogPtr, IDC_NewColumnStart, selectionRectanglePtr->left);

				// Load current line-column.												

		LoadDItemValue (dialogPtr, IDC_CurrentLineStart, selectionRectanglePtr->top);
		LoadDItemValue (dialogPtr, IDC_CurrentColumnStart, selectionRectanglePtr->left);

		}	// end "if (pointType == kPolygonType)" 

    *selectionDisplayUnitsPtr = unitsDisplayCode;

}	// end "EditLineColumnDialogInitialize"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void EditLineColumnDialogOK
//
//	Software purpose:	The purpose of this routine is to check to see if the selected
//							area has changed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None 		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/24/2004
//	Revised By:			Larry L. Biehl			Date: 12/06/2006	

void EditLineColumnDialogOK (
				Handle								windowInfoHandle,
				LongRect*							inputSelectionRectanglePtr,
				LongRect*							selectionRectanglePtr,
				DoubleRect*							coordinateRectanglePtr,
				SInt16								unitsDisplayCode,
				Boolean*								changedFlagPtr)

{
	ConvertCoordinateRectToLCRect (windowInfoHandle,
												coordinateRectanglePtr,
												selectionRectanglePtr,
												unitsDisplayCode,
												1.);

			// Now determine if the project information was changed.

	if (selectionRectanglePtr->top != inputSelectionRectanglePtr->top)
		*changedFlagPtr = TRUE;

	if (selectionRectanglePtr->left != inputSelectionRectanglePtr->left)
		*changedFlagPtr = TRUE;

	if (selectionRectanglePtr->bottom != inputSelectionRectanglePtr->bottom)
		*changedFlagPtr = TRUE;

	if (selectionRectanglePtr->right != inputSelectionRectanglePtr->right)
		*changedFlagPtr = TRUE;

	if (*changedFlagPtr) 
		{
				// Copy the new coordinates to the output parameters.

		*inputSelectionRectanglePtr = *selectionRectanglePtr;

		}	// end "if (*changedFlagPtr)"

}	// end "EditLineColumnDialogOK"      



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void EditLineColumnDialogSetStartLC
//
//	Software purpose:	The purpose of this routine is to show and hide the start line
//							and column checkbox as appropriate
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None 		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/18/2005
//	Revised By:			Larry L. Biehl			Date: 03/18/2005	

void EditLineColumnDialogSetStartLC (
				DialogPtr							dialogPtr,
				SInt16								unitsDisplayCode,
				Boolean								applyToAllWindowsFlag)

{
	if (applyToAllWindowsFlag && unitsDisplayCode == kLineColumnUnits)
		ShowDialogItem (dialogPtr, IDC_StartLCCheckBox);

	else	// !applyToAllWindowsFlag || ...
		HideDialogItem (dialogPtr, IDC_StartLCCheckBox);

}	// end "EditLineColumnDialogSetStartLC"        



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 EditLineColumnDialogCheckCoordinates
//
//	Software purpose:	The purpose of this routine is to check whether the coordinate
//							values make sense after being changed.  They need to fit within
//							the boundaries of the image.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None 		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/11/2005
//	Revised By:			Larry L. Biehl			Date: 05/16/2012	

SInt16 EditLineColumnDialogCheckCoordinates (
				DialogPtr							dialogPtr,
				Handle								windowInfoHandle,
				SInt16								selectionDisplayUnits,
				LongRect*							selectionRectanglePtr,
				LongRect*							minMaxSelectionRectanglePtr,
				DoubleRect*							coordinateRectanglePtr,
				DoubleRect*							minMaxCoordinateRectanglePtr,
				double								newColumnStart,
				double								newColumnEnd,
				double								newLineStart,
				double								newLineEnd)
 
{
	Str255								alertTextString;

	DoubleRect							localCoordinateRectangle;
	DoubleRect							localMinMaxCoordinateRectangle;
	LongRect								localSelectionRectangle;

	double								limitValue,
											mapToMetersFactor,
											theRealNum;

	SInt16								alertStringID,
											fileMapUnitsIndex,
											item,
											itemCode,
											itemError,
											projectionCode,
											referenceSystemCode,
											stringLength;

	itemError = 0;

	if (selectionDisplayUnits != kLineColumnUnits) 
		{
		referenceSystemCode = GetFileReferenceSystemCode (windowInfoHandle);
		localMinMaxCoordinateRectangle = *minMaxCoordinateRectanglePtr;

		if (selectionDisplayUnits == kLatLongUnits &&
														referenceSystemCode != kGeographicRSCode) 
			{
					// For lat-long units we need to determine the min and max values
					// for the actual location the selection is in the image. The Latitude-
					// Longitude does not have to be constant with line or column number.

			localCoordinateRectangle.top = newLineEnd;
			localCoordinateRectangle.bottom = newLineStart;
			localCoordinateRectangle.left = newColumnStart;
			localCoordinateRectangle.right = newColumnEnd;

			projectionCode = GetFileProjectionCode (windowInfoHandle);

			ConvertLatLongRectToMapRect (windowInfoHandle, &localCoordinateRectangle);

			ConvertMapRectToLCRect (windowInfoHandle,
											  &localCoordinateRectangle,
											  &localSelectionRectangle,
											  1);

					// Continue with check if the line columns are within the possible 
					// range for the image.

			if (localSelectionRectangle.top >= minMaxSelectionRectanglePtr->top &&
					  localSelectionRectangle.bottom <= minMaxSelectionRectanglePtr->bottom &&
					  localSelectionRectangle.left >= minMaxSelectionRectanglePtr->left &&
					  localSelectionRectangle.right <= minMaxSelectionRectanglePtr->right) 
				{
						// Add a pixel around the edge of the new local area to allow for
						// the edge of a pixel instead of the center of the pixel

				localSelectionRectangle.top = MAX (localSelectionRectangle.top - 1, 
																minMaxSelectionRectanglePtr->top);

				localSelectionRectangle.left = MAX (localSelectionRectangle.left - 1, 
																minMaxSelectionRectanglePtr->left);

				localSelectionRectangle.bottom = MIN (localSelectionRectangle.bottom + 1, 
																	minMaxSelectionRectanglePtr->bottom);

				localSelectionRectangle.right = MIN (localSelectionRectangle.right + 1, 
																	minMaxSelectionRectanglePtr->right);

				ConvertLCRectToMapRect (windowInfoHandle,
												&localSelectionRectangle,
												&localMinMaxCoordinateRectangle);

						// Note that the top and bottom are now in reverse order of what is 
						// expected for the rest of the comparisons.

				theRealNum = localMinMaxCoordinateRectangle.top;
				localMinMaxCoordinateRectangle.top = localMinMaxCoordinateRectangle.bottom;
				localMinMaxCoordinateRectangle.bottom = theRealNum;

				fileMapUnitsIndex = 
							GetFilePlanarMapUnitsCode (windowInfoHandle) - kKilometersCode;
				mapToMetersFactor = 1;
				if (fileMapUnitsIndex > 1)
					mapToMetersFactor = gDistanceFileConversionFactors[fileMapUnitsIndex] /
														gDistanceFileConversionFactors[1]; // kMeters

				ConvertMapRectByGivenFactor (mapToMetersFactor, 
														&localMinMaxCoordinateRectangle);

				ConvertMapRectToLatLongRect (
						windowInfoHandle, projectionCode, &localMinMaxCoordinateRectangle);

						// Now be sure that the min max limitation are not more stringent 
						// than what the original was.

				localMinMaxCoordinateRectangle.left = 
														MIN (localMinMaxCoordinateRectangle.left,
																minMaxCoordinateRectanglePtr->left);
				localMinMaxCoordinateRectangle.right = 
														MAX (localMinMaxCoordinateRectangle.right,
																minMaxCoordinateRectanglePtr->right);
				localMinMaxCoordinateRectangle.bottom = 
														MIN (localMinMaxCoordinateRectangle.bottom,
																minMaxCoordinateRectanglePtr->left);
				localMinMaxCoordinateRectangle.top = 
														MAX (localMinMaxCoordinateRectangle.top,
																minMaxCoordinateRectanglePtr->top);

				}	// end "if (localSelectionRectangle.top >= ..."

			}	// end "if (selectionDisplayUnits == kLatLongUnits && ..."

		for (item = 3; item <= 4; item++) 
			{
					// Item = 3: New upper coordinate value
					// Item = 4 New lower coordinate value

			if (item == 3) 
				{
				theRealNum = newLineStart;
				itemCode = IDC_NewLineStart;

				}	// end "if (item == 3)"

			else	// item == 4
				{
				theRealNum = newLineEnd;
				itemCode = IDC_NewLineEnd;

				}	// end "else item == 4"

			if (theRealNum < localMinMaxCoordinateRectangle.bottom - 0.00000001) 
				{
				itemError = itemCode;
				alertStringID = IDS_Alert132;
				limitValue = localMinMaxCoordinateRectangle.bottom;

				}	// end "if (theNum < localMinMaxCoordinateRectangle.bottom)"

			else if (theRealNum > localMinMaxCoordinateRectangle.top + 0.00000001) 
				{
				itemError = itemCode;
				alertStringID = IDS_Alert133;
				limitValue = localMinMaxCoordinateRectangle.top;

				}	// end "else if (theNum > localMinMaxCoordinateRectangle.top)"

			if (item == 3)
				coordinateRectanglePtr->bottom = theRealNum;
			else	// item == 4
				coordinateRectanglePtr->top = theRealNum;

			if (itemError != 0)
				break;

			}	// end "for (item=3; item<=4; item++)"

		for (item = 5; item <= 6; item++) 
			{
			if (itemError == 0) 
				{
						// Item = 5: New left coordinate value
						// Item = 6: New right coordinate value

				if (item == 5) 
					{
					theRealNum = newColumnStart;
					itemCode = IDC_NewColumnStart;

					}	// end "if (item == 5)"

				else	// item == 6
					{
					theRealNum = newColumnEnd;
					itemCode = IDC_NewColumnEnd;

					}	// end "else item == 6"

				if (theRealNum < localMinMaxCoordinateRectangle.left - 0.00000001) 
					{
					itemError = itemCode;
					alertStringID = IDS_Alert130;
					limitValue = localMinMaxCoordinateRectangle.left;

					}	// end "if (theNum < localMinMaxCoordinateRectangle.left)"

				else if (theRealNum > localMinMaxCoordinateRectangle.right + 0.00000001) 
					{
					itemError = itemCode;
					alertStringID = IDS_Alert131;
					limitValue = localMinMaxCoordinateRectangle.right;

					}	// end "else if (theNum > localMinMaxCoordinateRectangle.right)"

				if (item == 5)
					coordinateRectanglePtr->left = theRealNum;
				else	// item == 6
					coordinateRectanglePtr->right = theRealNum;

				}	// end "if (itemError == 0)"

			}	// end "for (item=5; item<=6; item++)"

		if (itemError != 0) 
			{
			if (MGetString (gTextString, kAlertStrID, alertStringID)) 
				{
				stringLength = sprintf ((char*)&alertTextString[1],
												(char*)&gTextString[1],
												limitValue);
				alertTextString[0] = (UInt8)stringLength;

				DisplayAlert (kErrorAlert2ID,
									kStopAlert,
									0,
									0,
									0,
									alertTextString);

				}	// end "if (MGetString (..."

			}	// end "if (itemError != 0)"

		}	// end "if (selectionDisplayUnits != kLineColumnUnits)"

	if (itemError == 0) 
		{
		alertStringID = 0;
		if (selectionDisplayUnits == kLineColumnUnits) 
			{
					// Make certain the line-column values make sense.	
					// Turn the okay button off is they do not.				

			if (selectionRectanglePtr->top > selectionRectanglePtr->bottom)
				alertStringID = IDS_Alert68;

			else if (selectionRectanglePtr->left > selectionRectanglePtr->right)
				alertStringID = IDS_Alert69;

			}	// end "if (gSelectionDisplayUnits == kLineColumnUnits)"

		else	// if (gSelectionDisplayUnits != kLineColumnUnits)
			{
					// Make certain the map values make sense.					

			if (coordinateRectanglePtr->top < coordinateRectanglePtr->bottom)
				alertStringID = IDS_Alert68;

			else if (coordinateRectanglePtr->left > coordinateRectanglePtr->right)
				alertStringID = IDS_Alert69;

			}	// end "else selectionDisplayUnits != kLineColumnUnits"

		if (alertStringID != 0) 
			{
			DisplayAlert (kErrorAlertID,
								kStopAlert,
								kAlertStrID,
								alertStringID,
								0,
								NULL);

			if (alertStringID == IDS_Alert68)
				itemError = IDC_NewLineStart;

			else	// alertStringID == IDS_Alert69
				itemError = IDC_NewColumnStart;

			}	// end "if (alertStringID != 0)"

		}	// end "if (itemError == 0)"
	/*
	if (errorAlertFlag)
		{
		numberDecimals = 4;
		if (selectionDisplayUnits == kLatLongUnits)
			numberDecimals = 7;

		RealNumberErrorAlert (theRealNum, dialogPtr, valueItemHit, numberDecimals);

		}		// end "if (errorAlertFlag)"
	*/
	return (itemError);

}	// end "EditLineColumnDialogCheckCoordinates"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetBoundingSelectionRectangles
//
//	Software purpose:	
//
//	Parameters in:		None		
//
//	Parameters out:	None			
//
//	Value Returned:	None
// 
// Called By:			PolygonSelection in seletionArea.c
//							SetPolygonSelection in SSelUtil.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/28/1998
//	Revised By:			Larry L. Biehl			Date: 09/28/1998			

void GetBoundingSelectionRectangles (
				DisplaySpecsPtr					displaySpecsPtr,
				SelectionInfoPtr					selectionInfoPtr,
				HPFieldPointsPtr					selectionPointsPtr,
				SInt16								startChannel)

{
	SInt32								index;


	//selectionPointsPtr++;														
	selectionInfoPtr->lineColumnRectangle.top =
						selectionInfoPtr->lineColumnRectangle.bottom =
																selectionPointsPtr->line;
	selectionInfoPtr->lineColumnRectangle.left =
						selectionInfoPtr->lineColumnRectangle.right =
																selectionPointsPtr->col;

	for (index = 1; index < selectionInfoPtr->numberPoints; index++) 
		{
		//selectionPointsPtr += 2;
		selectionPointsPtr++;

				// Update the bounding line column rectangle.							

		selectionInfoPtr->lineColumnRectangle.top =
											MIN (selectionPointsPtr->line, 
													selectionInfoPtr->lineColumnRectangle.top);
		selectionInfoPtr->lineColumnRectangle.bottom =
											MAX (selectionPointsPtr->line, 
													selectionInfoPtr->lineColumnRectangle.bottom);

		selectionInfoPtr->lineColumnRectangle.left =
											MIN (selectionPointsPtr->col, 
													selectionInfoPtr->lineColumnRectangle.left);
		selectionInfoPtr->lineColumnRectangle.right =
											MAX (selectionPointsPtr->col, 
													selectionInfoPtr->lineColumnRectangle.right);

		}	// end "for (index=1; index<..." 

			// Get the selection offscreen rectangles.

	ComputeSelectionOffscreenRectangle (displaySpecsPtr,
													&selectionInfoPtr->lineColumnRectangle,
													&selectionInfoPtr->offScreenRectangle,
													startChannel);

}	// end "GetBoundingSelectionRectangles"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt64 GetNumberPixelsInSelection
//
//	Software purpose:	The purpose of this routine is to obtain the number of pixels 
//							in the selected area.
//
//	Parameters in:		fieldNumber - the project field number.
//							areaDescriptionPtr - points to structure to use to
//											store the field coordinates.
//										
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ClosePolygonSelection in SSelUtil.cpp
//							SetSelectionInformation in SSelUtil.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/18/1998
//	Revised By:			Larry L. Biehl			Date: 08/23/2010	

SInt64 GetNumberPixelsInSelection (
				SelectionInfoPtr					selectionInfoPtr)
 
{
	SInt64								numberPixels,
											numberPixelsChecked,
											totalNumberPixelsToCheck;

	Point									point;

	HPFieldPointsPtr					selectionPointsPtr;
	LongRect*							lineColumnRectPtr;

	RgnHandle							rgnHandle;

	UInt32								count;

	SInt32								column,
											columnEnd,
											columnStart,
											line,
											lineEnd,
											numberColumns;


			// Initialize local variables.

	numberPixels = 0;
	lineColumnRectPtr = &selectionInfoPtr->lineColumnRectangle;

	totalNumberPixelsToCheck =
				(SInt64)(lineColumnRectPtr->right - lineColumnRectPtr->left + 1) *
									(lineColumnRectPtr->bottom - lineColumnRectPtr->top + 1);

			// Check if rectangular field or polygonal field.							

	if (selectionInfoPtr->typeFlag == kRectangleType) 
		{
		numberPixels = totalNumberPixelsToCheck;

		}	// end "if (selectionInfoPtr->typeFlag == kRectangleType)"

	else if (selectionInfoPtr->typeFlag == kPolygonType) 
		{
		selectionPointsPtr = (HPFieldPointsPtr)GetHandlePointer (
														selectionInfoPtr->polygonCoordinatesHandle,
														kLock);
														
		#if defined multispec_mac
			rgnHandle = NewRgn ();

			if (rgnHandle != NULL) 
				{
				OpenRgn ();
				MoveTo (selectionPointsPtr[selectionInfoPtr->numberPoints - 1].col,
						  selectionPointsPtr[selectionInfoPtr->numberPoints - 1].line);

				for (count = 0;
						count < (UInt32)selectionInfoPtr->numberPoints;
							count++) 
					{
					if (gMemoryError == noErr)
						LineTo (selectionPointsPtr->col, selectionPointsPtr->line);

					selectionPointsPtr++;

					}	// end "for (count=0; count<..."

				CloseRgn (rgnHandle);

				}	// end "if (rgnHandle != NULL)"
		#endif	// defined multispec_mac 

		#if defined multispec_win 
			POINT* pointsPtr = NULL;

			pointsPtr = (POINT*)MNewPointer (
												selectionInfoPtr->numberPoints * sizeof (POINT));
			if (pointsPtr != NULL)
				rgnHandle = new CRgn;

			if (rgnHandle != NULL) 
				{
				for (count = 0;
						count < (UInt32)selectionInfoPtr->numberPoints;
							count++) 
					{
					pointsPtr[count].x = (int)selectionPointsPtr->col;
					pointsPtr[count].y = (int)selectionPointsPtr->line;

					selectionPointsPtr++;

					}	// end "for (count=0; count<..." 

				if (!(rgnHandle)->CreatePolygonRgn ((tagPOINT*)pointsPtr,
																  selectionInfoPtr->numberPoints,
																  WINDING)) 
					{
					delete rgnHandle;
					rgnHandle = NULL;

					}	// end "if (!(*rgnHandlePtr)->CreatePolygonRgn (..."

				}	// end "if (rgnHandle != NULL)"

			CheckAndDisposePtr ((Ptr)pointsPtr);
		#endif	// defined multispec_win

		#if defined multispec_lin 
			wxPoint* pointsPtr = NULL;

			pointsPtr = (wxPoint*)MNewPointer (
											selectionInfoPtr->numberPoints * sizeof (wxPoint));
			//if (pointsPtr != NULL)
			//	rgnHandle = new wxRegion ();

			if (pointsPtr != NULL) 
				{
				//if (rgnHandle != NULL) {
				for (count=0; count<(UInt32)selectionInfoPtr->numberPoints; count++) 
					{
					pointsPtr[count].x = (int)selectionPointsPtr->col;
					pointsPtr[count].y = (int)selectionPointsPtr->line;
					selectionPointsPtr++;
				
					}	// end "for (count=0; count<..." 
					
				}	// end "if (pointsPtr != NULL)"
				
			rgnHandle = new wxRegion ((int)(selectionInfoPtr->numberPoints), 
												pointsPtr,
												wxWINDING_RULE);
			/*
			if (!(rgnHandle)->CreatePolygonRgn ((tagPOINT*)pointsPtr,
															  selectionInfoPtr->numberPoints,
															  WINDING)) 
				{
			*/
			if (rgnHandle->IsEmpty ()) 
				{
				delete rgnHandle;
				rgnHandle = NULL;
				
				}	// end "if (rgnHandle->IsEmpty ())"

			// end "if (rgnHandle != NULL)"

			CheckAndDisposePtr ((Ptr)pointsPtr);
		#endif	// defined multispec_lin
	  
		if (rgnHandle != NULL && gMemoryError == noErr) 
			{
					// Intialize the nextTime variables to indicate when the next check	
					// should occur for a command-. and status information.					

			gNextTime = SInt32_MAX;
			gNextStatusTime = TickCount () + gNextStatusTimeOffset;

			lineEnd = lineColumnRectPtr->bottom;
			columnStart = lineColumnRectPtr->left;
			columnEnd = lineColumnRectPtr->right;

			numberColumns = columnEnd - columnStart + 1;

			point.v = (SInt16)lineColumnRectPtr->top;
			for (line = lineColumnRectPtr->top; line <= lineEnd; line++) 
				{
				point.h = (SInt16)columnStart;
				for (column=columnStart; column<=columnEnd; column++) 
					{
					if (PtInRgn (point, rgnHandle))
						numberPixels++;

					point.h++;

					}	// end "for (column=columnStart; column<=..." 

				point.v++;

				if (TickCount () >= gNextStatusTime) 
					{
					numberPixelsChecked = column - columnStart +
							 (SInt64)(line - lineColumnRectPtr->top) * numberColumns;

					if (gStatusDialogPtr == NULL) 
						{
								// Determine if the status dialog box needs to be displayed.

						if (totalNumberPixelsToCheck / numberPixelsChecked > 3) 
							{
									// Get status information dialog box.									

							gStatusDialogPtr = GetStatusDialog (kGraphicStatusDialogID, TRUE);

							LoadDItemStringNumber (kProjectStrID,
															IDS_Project77,
															gStatusDialogPtr,
															IDC_Status2,
															(Str255*)gTextString);

							gNextTime = TickCount ();

									// Turn spin cursor on

							gPresentCursor = kSpin;

							}	// end "if (totalNumberPixelsToCheck/..."

						}	// end "if (gStatusDialogPtr == NULL)"	

					if (gStatusProgressControlHandle != NULL)
						MSetControlValue (
							(WindowPtr)gStatusDialogPtr,
							gStatusProgressControlHandle,
							(SInt16)(100 * numberPixelsChecked / totalNumberPixelsToCheck));

					gNextStatusTime = TickCount () + gNextStatusTimeOffset;

					}	// end "if (TickCount () >= gNextStatusTime)" 

						// Exit routine if user has "command period" down.					

				if (TickCount () >= gNextTime) 
					{
					if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask)) 
						{
						numberPixels = 0;
						break;

						}	// end "if (!CheckSomeEvents (osMask + ..." 

					}	// end "if (TickCount () >= nextTime)" 

				}	// end "for (line=lineColumnRectPtr->top; ..." 

					// Dispose of status box.													

			CloseStatusDialog (TRUE);

					// Turn spin cursor off

			MInitCursor ();

			}	// end "if (rgnHandle != NULL && ..."

			// Dispose of the region.						

		if (rgnHandle != NULL)
			DisposeRgn (rgnHandle);

		CheckAndUnlockHandle (selectionInfoPtr->polygonCoordinatesHandle);

		}	// end "else if (selectionInfoPtr->typeFlag == kPolygonType)" 

	return (numberPixels);

}	// end "GetNumberPixelsInSelection" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle GetNewSelectionInfoHandle
//
//	Software purpose:	The purpose of this routine is to get the memory for
//							the selection information structure and initialize the
//							structure.
//
//	Parameters in:		None.
//
//	Parameters out:	None
//
// Value Returned:  	Handle to the selection information structure.  The handle
//							returned will be NULL if there is not enough
//							memory.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/14/2005
//	Revised By:			Larry L. Biehl			Date: 03/14/2005

Handle GetNewSelectionInfoHandle (void)

{
	SelectionInfoPtr					selectionInfoPtr;

	Handle								selectionInfoHandle;


	selectionInfoHandle = MNewHandle (sizeof (SelectionInfo));

	selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (selectionInfoHandle);

	if (selectionInfoPtr != NULL) 
		{
		selectionInfoPtr->coordinateRectangle.top = 0;
		selectionInfoPtr->coordinateRectangle.left = 0;
		selectionInfoPtr->coordinateRectangle.bottom = 0;
		selectionInfoPtr->coordinateRectangle.right = 0;

		selectionInfoPtr->lineColumnRectangle.top = 0;
		selectionInfoPtr->lineColumnRectangle.left = 0;
		selectionInfoPtr->lineColumnRectangle.bottom = 0;
		selectionInfoPtr->lineColumnRectangle.right = 0;

		selectionInfoPtr->offScreenRectangle.top = 0;
		selectionInfoPtr->offScreenRectangle.left = 0;
		selectionInfoPtr->offScreenRectangle.bottom = 0;
		selectionInfoPtr->offScreenRectangle.right = 0;

		selectionInfoPtr->polygonCoordinatesHandle = NULL;
		selectionInfoPtr->numberPixels = 0;
		selectionInfoPtr->numberPoints = 0;
		selectionInfoPtr->typeFlag = 0;
		selectionInfoPtr->unitsDisplayCode = kLineColumnUnits;

		}	// end "if (selectionInfoPtr != NULL)"

	return (selectionInfoHandle);

}	// end "GetNewSelectionInfoHandle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetSelectedAreaInfo
//
//	Software purpose:	The purpose of this routine is to set the line and
//							column values for the selected area and determine
//							whether the entire area is to be used or a subset.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None 		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/13/1994
//	Revised By:			Larry L. Biehl			Date: 12/13/1994	

Boolean GetSelectedAreaInfo (
				WindowPtr							imageWindowPtr,
				WindowInfoPtr						imageWindowInfoPtr,
				SInt32*								lineStartPtr,
				SInt32*								lineEndPtr,
				SInt32*								columnStartPtr,
				SInt32*								columnEndPtr,
				Boolean								clearSelectionFlag,
				Boolean								useThresholdFlag,
				Boolean								adjustToBaseImageFlag)
 
{
	Boolean								returnFlag = FALSE;
	LongRect								longRect;


			// Load user selected rectangle information into specification information 
			// if the selection rectangle has been set up. 

	gSelectionRectangleFlag = FALSE;

	returnFlag = GetSelectionRectangle (imageWindowPtr,
													&longRect,
													clearSelectionFlag,
													useThresholdFlag,
													adjustToBaseImageFlag);

	if (returnFlag) 
		{
		*lineStartPtr = longRect.top;
		*lineEndPtr = longRect.bottom;
		*columnStartPtr = longRect.left;
		*columnEndPtr = longRect.right;

		gSelectionRectangleFlag = !CheckIfEntireImage (imageWindowInfoPtr,
																		 *lineStartPtr,
																		 *lineEndPtr,
																		 *columnStartPtr,
																		 *columnEndPtr);

		}	// end "if (returnFlag)" 

	return (returnFlag);

}	// end "GetSelectedAreaInfo" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetSelectedOffscreenRectangle
//
//	Software purpose:	This routine returns the selected offscreen
//							rectangle.  If the 'use threshold' flag is set then 
//							use the selected area only if it is larger than the 
//							threshold size.  If the 'use all' flag is set then
//							return the entire area if there is no valid selected 
//							area.  If no selected area has been set up then the 
//							routine returns FALSE.
//
//	Parameters in:		Image window information structure pointer
//							Flag indicating whether to use threshold size
//							Flag indicating whether to return the entire
//								area if there is no valid selected area.
//
//	Parameters out:	Selected offscreen area
//
// Value Returned:	TRUE if selected offscreen area was set up
//							FALSE if no selected offscreen area was set up
// 
// Called By:			UpdateFileMenuImageItems in menus.c
//							PrintImageWindow in print.c
//							SetUpPrintedImageInfo in print.c
//							SaveImageWindowAs in SSaveWrt.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/21/1994
//	Revised By:			Larry L. Biehl			Date: 06/24/1999			

Boolean GetSelectedOffscreenRectangle (
				WindowInfoPtr						imageWindowInfoPtr,
				Rect*									offScreenRectanglePtr,
				Boolean								useThresholdFlag,
				Boolean								returnAllFlag)
 
{
	DisplaySpecsPtr					displaySpecsPtr;
	Boolean								setUpFlag = FALSE;
	SInt16								typeCode = 0;


	if (imageWindowInfoPtr != NULL) 
		{
				// load selection area information into output rectangle if the selection 
				// rectangle has been set up.								

		SelectionInfoPtr selectionInfoPtr;
		selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (
															imageWindowInfoPtr->selectionInfoHandle);

		if (selectionInfoPtr != NULL) 
			{
			typeCode = selectionInfoPtr->typeFlag;

			if (typeCode == kRectangleType)
				*offScreenRectanglePtr = selectionInfoPtr->offScreenRectangle;

			if (typeCode == kRectangleType) 
				{
				setUpFlag = TRUE;

				if (useThresholdFlag) 
					{
							// Check if selection is larger than the minimum threshold 	
							// size.																		

					if ((offScreenRectanglePtr->bottom - offScreenRectanglePtr->top <
																						gThresholdSize) &&
							(offScreenRectanglePtr->right - offScreenRectanglePtr->left <
																							gThresholdSize))
						setUpFlag = FALSE;

					}	// end "if (useThresholdFlag)" 

				}	// end "if (typeCode == kRectangleType)" 

			if (!setUpFlag && returnAllFlag) 
				{
				Handle displaySpecsHandle = GetDisplaySpecsHandle (imageWindowInfoPtr);
				displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);

				offScreenRectanglePtr->top = 0;
				offScreenRectanglePtr->left = 0;
				offScreenRectanglePtr->bottom = 
										(SInt16)displaySpecsPtr->imageDimensions[kVertical];
				offScreenRectanglePtr->right = 
										(SInt16)displaySpecsPtr->imageDimensions[kHorizontal];

				setUpFlag = TRUE;

				}	// end "if (!setUpFlag && returnAllFlag)" 

			}	// end "if (selectionInfoPtr != NULL)"

		}	// end "if (imageWindowInfoPtr != NULL)" 

	return (setUpFlag);

}	// end "GetSelectedOffscreenRectangle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetSelectionBoundary
//
//	Software purpose:	The purpose of this routine is to get the line and
//							column coordinates for the selected area.  If
//							the selected area is of polygon type then the region is 
//							setup for the polygon.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/05/1998
//	Revised By:			Larry L. Biehl			Date: 05/01/2007

void GetSelectionBoundary (
				WindowPtr							windowPtr,
				AreaDescriptionPtr				areaDescriptionPtr)

{
	HPFieldPointsPtr					selectionPointsPtr;
	SelectionInfoPtr					selectionInfoPtr;

	Handle								selectionInfoHandle;

	SignedByte							selectionInfoStatus,
											selectionPointsStatus;


			// Initialize local variables.

	selectionInfoHandle = GetSelectionInfoHandle (windowPtr);
	selectionInfoPtr = (SelectionInfoPtr)GetHandleStatusAndPointer (
																					selectionInfoHandle,
																					&selectionInfoStatus);

			// Determine if field is described by a polygon or a rectangle.  If	
			// the field is described by a polygon then create a region for the	
			// field.																				

	if (selectionInfoPtr->typeFlag == kPolygonType) 
		{
		RgnHandle rgnHandle;

				// Allow for the first two points in the polygon point list to
				// represent the bounding rectangle.

		areaDescriptionPtr->pointType = kPolygonType;
		areaDescriptionPtr->polygonFieldFlag = TRUE;

		selectionPointsPtr = (HPFieldPointsPtr)GetHandleStatusAndPointer (
														 selectionInfoPtr->polygonCoordinatesHandle,
														 &selectionPointsStatus);

		CreateFieldRgn (selectionInfoPtr->numberPoints,
							 selectionPointsPtr,
							 &rgnHandle,
							 0,
							 0);

		if (rgnHandle != NULL) 
			{
			if (gMemoryError != noErr) 
				{
				DisposeRgn (rgnHandle);
				rgnHandle = NULL;

				}	// end "if (gMemoryError != noErr)"

			areaDescriptionPtr->rgnHandle = rgnHandle;

			}	// end "if (rgnHandle != NULL)"

		MHSetState (selectionInfoPtr->polygonCoordinatesHandle, selectionPointsStatus);

		}	// end "if (selectionInfoPtr->typeFlag == kPolygonType)" 

	else	// The field is not polygonal type 
		{
		areaDescriptionPtr->pointType = kRectangleType;
		areaDescriptionPtr->polygonFieldFlag = FALSE;
		areaDescriptionPtr->rgnHandle = NULL;

		}	// end "else the field is not polygonal type" 

	areaDescriptionPtr->lineStart = selectionInfoPtr->lineColumnRectangle.top;
	areaDescriptionPtr->lineEnd = selectionInfoPtr->lineColumnRectangle.bottom;

	areaDescriptionPtr->columnStart = selectionInfoPtr->lineColumnRectangle.left;
	areaDescriptionPtr->columnEnd = selectionInfoPtr->lineColumnRectangle.right;

	if (areaDescriptionPtr->lineStart > 0) 
		{
		areaDescriptionPtr->numberLines =
							areaDescriptionPtr->lineEnd - areaDescriptionPtr->lineStart + 1;

		if (areaDescriptionPtr->columnInterval > 0)
			areaDescriptionPtr->numSamplesPerChan =
					(areaDescriptionPtr->columnEnd - areaDescriptionPtr->columnStart +
											areaDescriptionPtr->columnInterval) /
																	areaDescriptionPtr->columnInterval;

		}	// end "if (areaDescriptionPtr->lineStart > 0)" 

	MHSetState (selectionInfoHandle, selectionInfoStatus);

}	// end "GetSelectionBoundary" 	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetSelectionCoordinates
//
//	Software purpose:	This routine returns the selection line column rectangle.  
//							If no selection rectangle has been
//							set up then the routine returns FALSE.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ShowGraphWindowSelection in SSelectionGraph.cpp
//							CMOutlineArea::Invalidate in WDrawObj.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/02/1998
//	Revised By:			Larry L. Biehl			Date: 08/23/2010			

SInt16 GetSelectionCoordinates (
				WindowPtr							windowPtr,
				LongRect*							lineColumnRectanglePtr,
				DoubleRect*							coordinateRectangePtr,
				SInt64*								numberPixelsPtr)

{
	SelectionInfoPtr					selectionInfoPtr;

	Handle								selectionInfoHandle;

	SInt16								typeCode = 0;


	selectionInfoHandle = GetSelectionInfoHandle (windowPtr);

	selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (selectionInfoHandle);

	if (selectionInfoPtr != NULL) 
		{
		if (selectionInfoPtr->typeFlag != 0) 
			{
					// load selection area information into output rectangle if 		
					// the selection rectangle has been set up.

			if (lineColumnRectanglePtr != NULL)
				*lineColumnRectanglePtr = selectionInfoPtr->lineColumnRectangle;

			if (coordinateRectangePtr != NULL)
				*coordinateRectangePtr = selectionInfoPtr->coordinateRectangle;

			if (numberPixelsPtr != NULL)
				*numberPixelsPtr = selectionInfoPtr->numberPixels;

			typeCode = selectionInfoPtr->typeFlag;

			}	// end "if (selectionInfoPtr->typeFlag != 0)" 

		}	// end "if (selectionInfoPtr != NULL)" 

	return (typeCode);

}	// end "GetSelectionCoordinates" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt64 GetSelectionNumberPixels
//
//	Software purpose:	This routine returns the number of pixels represented by the
//							of the selection for the input image window info stucture.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	typeFlag
// 
// Called By:			
//
//	Revised By:			Larry L. Biehl			Date: 12/18/1998	
//	Revised By:			Larry L. Biehl			Date: 08/23/2010			

SInt64 GetSelectionNumberPixels (
				WindowInfoPtr						windowInfoPtr)
 
{
	if (windowInfoPtr != NULL) 
		{
		SelectionInfoPtr selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (
																windowInfoPtr->selectionInfoHandle);

		return (selectionInfoPtr->numberPixels);

		}	// end "if (windowInfoPtr != NULL)"

return (0);

}	// end "GetSelectionNumberPixels"

 

SInt64 GetSelectionNumberPixels (
				WindowPtr							windowPtr)

{
	WindowInfoPtr						windowInfoPtr;


	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (GetWindowInfoHandle (windowPtr));

	return (GetSelectionNumberPixels (windowInfoPtr));

}	// end "GetSelectionNumberPixels" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetSelectionOffscreenRectangle
//
//	Software purpose:	This routine returns the selection offscreen rectangle.  
//							If no selection rectangle has been
//							set up then the routine returns FALSE. 
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/02/1998
//	Revised By:			Larry L. Biehl			Date: 10/02/1998			

Boolean GetSelectionOffscreenRectangle (
				WindowPtr							windowPtr,
				Rect*									offScreenRectanglePtr)

{
	SelectionInfoPtr selectionInfoPtr;

	Handle selectionInfoHandle;

	Boolean setUpFlag = FALSE;


	selectionInfoHandle = GetSelectionInfoHandle (windowPtr);

	selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (selectionInfoHandle);

	if (selectionInfoPtr != NULL) 
		{
		if (selectionInfoPtr->typeFlag != 0) 
			{
					// load selection area information into output rectangle if 		
					// the selection rectangle has been set up.

			*offScreenRectanglePtr = selectionInfoPtr->offScreenRectangle;

			setUpFlag = TRUE;

			}	// end "if (selectionInfoPtr->typeFlag != 0)"

		}	// end "if (selectionInfoPtr != NULL)" 

	return (setUpFlag);

}	// end "GetSelectionOffscreenRectangle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetSelectionRectangle
//
//	Software purpose:	This routine returns the selected line-column
//							rectangle.  If the 'use threshold' flag is set then 
//							use the selected area only if it is larger than the 
//							threshold size.  If the 'use all' flag is set then
//							return the entire area if there is no valid selected 
//							area.  If no rectangular selected area has been set up then the 
//							routine returns FALSE.
//
//	Parameters in:		Image window information structure pointer
//							Flag indicating whether to use threshold size
//							Flag indicating whether to return the entire
//								area if there is no valid selected area.
//
//	Parameters out:	Selected offscreen area
//
// Value Returned:	TRUE if selected offscreen area was set up
//							FALSE if no selected offscreen area was set up
// 
// Called By:			UpdateFileMenuImageItems in menus.c
//							PrintImageWindow in print.c
//							SetUpPrintedImageInfo in print.c
//							SaveImageWindowAs in SSaveWrt.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/08/1988
//	Revised By:			Larry L. Biehl			Date: 02/23/2018

Boolean GetSelectionRectangle (
				WindowPtr							windowPtr,
				LongRect*							selectionRectanglePtr,
				Boolean								clearSelectionFlag,
				Boolean								useThresholdFlag,
				Boolean								adjustToBaseImageFlag)
 
{
	FileInfoPtr							fileInfoPtr,
											projectFileInfoPtr;
	
	SelectionInfoPtr					selectionInfoPtr;

	Handle								fileInfoHandle,
											projectFileInfoHandle,
											selectionInfoH,
											windowInfoH;

	SInt32								columnOffset,
											lineOffset;

	UInt32								maxNumberColumns,
											maxNumberLines;

	Boolean								setUpFlag;


	setUpFlag = FALSE;

			// Get the window information structure handle for the input window.

	windowInfoH = GetWindowInfoHandle (windowPtr);

	if (windowInfoH != NULL) 
		{
				// Load selection area information into output rectangle if 		
				// the selection rectangle has been set up.								

		selectionInfoH = GetSelectionInfoHandle (windowInfoH);
		selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (selectionInfoH);

		if (selectionInfoPtr->typeFlag == kRectangleType) 
			{
			*selectionRectanglePtr = selectionInfoPtr->lineColumnRectangle;

					// Adjust for any difference between start line and column in		
					// base image file and start line and column in associated image	
					// file if needed.																

			if (gProjectInfoPtr != NULL && adjustToBaseImageFlag)
				{
				if (!GetProjectBaseImageFlag (windowInfoH) && 
																GetProjectWindowFlag (windowInfoH))
					{
							// Get pointer to file specification.  We do not need 	
							// to lock it here since no other routines are called.
					
							// Note that a check is being make here to determine if this is
							// a case where the associated file is tiff and the base image
							// for the project is ERDAS .lan. If this is the case, then if,
							// the base ERDAS .lan file has start line and columns which are
							// not 1, we will ignore the start line and column as long as
							// the number of lines and columns in the associated image and
							// base image are the same. Tiff files do not have tags to
							// store a start line and column. We will assume that no
							// adjust will need to be made.
					
					fileInfoHandle = GetFileInfoHandle (windowInfoH);
					fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
					
					projectFileInfoHandle = GetFileInfoHandle (
																	gProjectInfoPtr->windowInfoHandle);
					projectFileInfoPtr =
											(FileInfoPtr)GetHandlePointer (projectFileInfoHandle);
					
					if ((fileInfoPtr->format == kTIFFType ||
											fileInfoPtr->format == kGeoTIFFType) &&
							projectFileInfoPtr->format == kErdas74Type &&
							fileInfoPtr->numberLines == projectFileInfoPtr->numberLines &&
							fileInfoPtr->numberColumns == projectFileInfoPtr->numberColumns)
						{
						lineOffset = 0;
						columnOffset = 0;

						}	// end "fileInfoPtr->format == kTIFFType && ...
					
					else	// take into account different start lines and columns
						{

						lineOffset = fileInfoPtr->startColumn - gProjectInfoPtr->startColumn;
						columnOffset = fileInfoPtr->startLine - gProjectInfoPtr->startLine;
						
						}	// end "take into account different start lines and columns"
					
					selectionRectanglePtr->left += columnOffset;
					selectionRectanglePtr->right += columnOffset;

					selectionRectanglePtr->top += lineOffset;
					selectionRectanglePtr->bottom += lineOffset;

							// Now verify that the selection is still within the
							// project base image.

					maxNumberColumns =
									GetMaxNumberColumns (gProjectInfoPtr->windowInfoHandle);
					selectionRectanglePtr->left = MAX (1, selectionRectanglePtr->left);
					selectionRectanglePtr->right = MIN ((SInt32)maxNumberColumns,
																		selectionRectanglePtr->right);

					maxNumberLines =
									GetMaxNumberLines (gProjectInfoPtr->windowInfoHandle);
					selectionRectanglePtr->top = MAX (1, selectionRectanglePtr->top);
					selectionRectanglePtr->bottom = MIN ((SInt32)maxNumberLines,
																		selectionRectanglePtr->bottom);

					}	// end "if (!GetProjectBaseImageFlag (..." 

				}	// end "if (gProjectInfoPtr != NULL && adjustToBaseImageFlag)" 

					// Reset the selection rectangle flag to indicate that it has	
					// been used, if requested.  If the selection is to be cleared	
					// then invalidate the rectangle.										

			if (clearSelectionFlag)
				ClearSelectionArea (windowPtr);

			if (useThresholdFlag) 
				{
						// Check if selection is larger than the minimum threshold size.																		

				if ((selectionRectanglePtr->bottom - selectionRectanglePtr->top >=
																					gThresholdSize) ||
						(selectionRectanglePtr->right - selectionRectanglePtr->left >=
																							gThresholdSize))
					setUpFlag = TRUE;

				}	// end "if (useThresholdFlag)" 

			else	// !useThresholdFlag 
				setUpFlag = TRUE;

			}	// end "if (selectionInfoPtr->typeFlag == kRectangleType)" 

		}	// end "if (windowInfoH)" 

	return (setUpFlag);

}	// end "GetSelectionRectangle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetSelectionRectangleLimits
//
//	Software purpose:	This routine sets the limits of the selection 
//							rectangle overwhich the cursor can be and still
//							be over the image.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			PolygonSelection in selectionArea.c
//							RectangleSelection in selectionArea.c
//
//	Revised By:			Larry L. Biehl			Date: 06/03/1993	
//	Revised By:			Larry L. Biehl			Date: 04/19/2000			

SInt16 GetSelectionRectangleLimits (
				Boolean								firstTimeFlag,
				SInt16								startChannel,
				LongPoint*							inputStartPointPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				Rect*									viewRectPtr, // gViewRect
				Rect*									limitRectanglePtr, // gTempRect
				LongPoint*							outputStartPointPtr) // gTempLongPoint

{
	SInt32								displayHOrigin,
											imageSize,
											imageTopOffset,
											leftEdge,
											legendWidth,
											rightEdge,
											topEdge;


			// Get the legend width. 
			// Note that the legend in the Windows version is in its on
			// view. It is not the same as that used for the image. Therefore
			// the legend width is always 0 within the image view.

	#if defined multispec_mac
		legendWidth = gActiveLegendWidth;
	#endif	// defined multispec_mac          

	#if defined multispec_win || defined multispec_lin
		legendWidth = 0;
	#endif	// defined multispec_win

			//	Find the rectangle which defines the limits that the cursor can	
			// be and still be over the displayed portion of the image.				
			// Get the height limit first.													

	limitRectanglePtr->top = viewRectPtr->top;

	imageTopOffset = GetImageTopOffset (gActiveImageWindowInfoH);

	topEdge = (SInt32)(imageTopOffset -
					displaySpecsPtr->origin[kVertical] * displaySpecsPtr->magnification);
	imageSize = (SInt32)(topEdge +
			displaySpecsPtr->magnification * displaySpecsPtr->imageDimensions[kVertical]);
	limitRectanglePtr->bottom = (SInt16)MIN (viewRectPtr->bottom, imageSize) + 1;

			// Now define the width limit taking into account side by side images.																				

			// First get the horizontal origin in window units.						

	displayHOrigin = (SInt32)(
					displaySpecsPtr->origin[kHorizontal] * displaySpecsPtr->magnification);

			// Now get the left edge in window units that defines the start of the 
			//	channel within which the selection is taking place.	

	imageSize = (SInt32)(displaySpecsPtr->offscreenChannelWidth *
																		displaySpecsPtr->magnification);

	if (firstTimeFlag) 
		{
		startChannel = 0;
		if (imageSize > 0 && displaySpecsPtr->displayType == 7) 
			{
			startChannel = (SInt16)(
					  (displayHOrigin + inputStartPointPtr->h - legendWidth) / imageSize);

			startChannel = MIN (startChannel,
					  (SInt16)displaySpecsPtr->displayedNumberChannels - 1);

			}	// end "if (imageSize > 0 && ..." 

		}	// end "if (firstTimeFlag)" 

	leftEdge = startChannel * imageSize;
	leftEdge += legendWidth - displayHOrigin;

			// Now get the right edge in window units that defines the end of the 
			// channel within which the selection is taking place.				

	rightEdge = leftEdge + imageSize;
	if (displaySpecsPtr->displayType == 7)
		rightEdge -= 2;

	limitRectanglePtr->left = (SInt16)MAX (leftEdge, 0);
	limitRectanglePtr->right = (SInt16)MIN (viewRectPtr->right, rightEdge) + 1;

	outputStartPointPtr->h = leftEdge;
	outputStartPointPtr->v = topEdge;

	return (startChannel);

}	// end "GetSelectionRectangleLimits" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetSelectionTypeCode
//
//	Software purpose:	This routine returns the current setting of the selection type
//							for the input image window info stucture.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	typeFlag
// 
// Called By:			
//
//	Revised By:			Larry L. Biehl			Date: 10/02/1998	
//	Revised By:			Larry L. Biehl			Date: 03/28/2014			

SInt16 GetSelectionTypeCode (
				Handle								windowInfoHandle)
 
{
	Handle selectionInfoHandle = GetSelectionInfoHandle (windowInfoHandle);

	SelectionInfoPtr selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (
																					selectionInfoHandle);

	if (selectionInfoPtr != NULL)
		return (selectionInfoPtr->typeFlag);

	return (0);

}	// end "GetSelectionTypeCode" 

		

SInt16 GetSelectionTypeCode (
				WindowInfoPtr						windowInfoPtr)
 
{
	if (windowInfoPtr != NULL) 
		{
		SelectionInfoPtr selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (
																windowInfoPtr->selectionInfoHandle);

		if (selectionInfoPtr != NULL)
			return (selectionInfoPtr->typeFlag);

		}	// end "if (windowInfoPtr != NULL)"

	return (0);

}	// end "GetSelectionTypeCode" 



SInt16 GetSelectionTypeCode (
				WindowPtr							windowPtr)
 
{
	WindowInfoPtr						windowInfoPtr;


	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (GetWindowInfoHandle (windowPtr));

	return (GetSelectionTypeCode (windowInfoPtr));

}	// end "GetSelectionTypeCode" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void InitializePolygonSelection
//
//	Software purpose:	
//
//	Parameters in:		None		
//
//	Parameters out:	None			
//
//	Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 09/28/1998
//	Revised By:			Larry L. Biehl			Date: 09/28/1998			

Boolean InitializePolygonSelection (
				SelectionInfoPtr					selectionInfoPtr,
				HPFieldPointsPtr*					selectionPointsPtrPtr,
				UInt32*								memoryLimitNumberPtr,
				UInt32*								bytesNeededPtr,
				UInt32*								bytesNeededIncrementPtr)

{
			// Clear the selection area.														

	ClearSelectionArea (gActiveImageWindow);
	CheckSomeEvents (updateMask);

			// Get memory for polygon coordinates.  Allow for 100 points to 		
			// begin with.																			

	*memoryLimitNumberPtr = 100;
	//*bytesNeededPtr = *memoryLimitNumberPtr * 2 * sizeof (ProjectFieldPoints);
	*bytesNeededPtr = *memoryLimitNumberPtr * sizeof (ProjectFieldPoints);
	selectionInfoPtr->polygonCoordinatesHandle = MNewHandle (*bytesNeededPtr);
	if (selectionInfoPtr->polygonCoordinatesHandle == NULL)
																						return (FALSE);

	*bytesNeededIncrementPtr = *bytesNeededPtr;
	*selectionPointsPtrPtr = (HPFieldPointsPtr)GetHandlePointer (
														selectionInfoPtr->polygonCoordinatesHandle,
														kLock);

			// Set selection area type flag.     

	selectionInfoPtr->typeFlag = kPolygonType;

	gProcessorCode = kPolygonSelectionProcessor;

	return (TRUE);

}	// end "InitializePolygonSelection" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		OutlineSelectionArea
//
//	Software purpose:	This routine outlines the selection area for
//							the input image window.  It assumes that the port
//							has already been set for the input window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DoEdit in menus.c
//							CopyOffScreenImage in multiSpec.c
//							RectangleSelection in selectionArea.c
//
//	Coded By:			Larry L. Biehl			Date: 08/11/1989
//	Revised By:			Larry L. Biehl			Date: 03/23/1999			

void OutlineSelectionArea (
				WindowPtr							windowPtr)
 
{
	#if defined multispec_mac
		PenState								penState;
		GrafPtr								savedPort;
	#endif	// defined multispec_mac

	SelectionInfoPtr					selectionInfoPtr;

	Handle								selectionInfoH,
											windowInfoH;


			// Get the window information structure handle for the input window.

	windowInfoH = GetWindowInfoHandle (windowPtr);

			// Get the pointer to the selection rectangle information.		 		

	selectionInfoH = GetSelectionInfoHandle (windowInfoH);
	selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (selectionInfoH);

	if (!OffscreenImageMapExists (windowInfoH) || selectionInfoPtr == NULL)
																										return;

	if (selectionInfoPtr->typeFlag != 0) 
		{
		#if defined multispec_mac
			GetPort (&savedPort);
			SetPortWindowPort (windowPtr);

					// Set the pen characteristics.												

			GetPenState (&penState);
			PenPat ((ConstPatternParam)&gOutlinePenPattern);
			PenMode (patCopy);
			PenSize (1, 1);

			DrawSelectionArea (windowPtr);

			SetPenState (&penState);

			SetPort (savedPort);
		#endif	// defined multispec_mac

		#if defined multispec_win  
					// Force the new selection to be drawn

			CMOutlineArea* selectionAreaCPtr = windowPtr->GetSelectionAreaCPtr ();
			selectionAreaCPtr->Invalidate (windowPtr);
		#endif	// defined multispec_win

		#if defined multispec_lin
			(windowPtr->m_Canvas)->Refresh ();
		#endif
		
		}	// end "if (selectionInfoPtr->typeFlag != 0)" 

}	// end "OutlineSelectionArea" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetPolygonSelection
//
//	Software purpose:	This routine sets the input polygon line and column values
//							to the input selectionInfo structure. Checks are made to verify
//							that the values still make sense after the adjustment relative
//							to the image file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			SelectArea
//
//	Revised By:			Larry L. Biehl			Date: 10/02/1998	
//	Revised By:			Larry L. Biehl			Date: 10/02/1998			

void SetPolygonSelection (
				WindowInfoPtr						windowInfoPtr,
				SelectionInfoPtr					selectionInfoPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				SInt32								lineAdjust,
				SInt32								columnAdjust)

{
	HPFieldPointsPtr					selectionPointsPtr;

	UInt32								count;


	selectionPointsPtr = (HPFieldPointsPtr)GetHandlePointer (
														selectionInfoPtr->polygonCoordinatesHandle,
														kLock);

	//selectionPointsPtr++;
	for (count=0; count<(UInt32)selectionInfoPtr->numberPoints; count++) 
		{
				// Update the bounding line column polygon points.							

		selectionPointsPtr->line += lineAdjust;
		selectionPointsPtr->col += columnAdjust;

				// Check if any point is outside of the image file then do not use
				// this polgon selection.							

		if (selectionPointsPtr->line < 1 ||
						selectionPointsPtr->line > (SInt32)windowInfoPtr->maxNumberLines)
			selectionInfoPtr->typeFlag = 0;

		if (selectionPointsPtr->col < 1 ||
						selectionPointsPtr->col > (SInt32)windowInfoPtr->maxNumberColumns)
			selectionInfoPtr->typeFlag = 0;

		if (selectionInfoPtr->typeFlag == 0)
			break;

		//selectionPointsPtr += 2;
		selectionPointsPtr++;

		}	// end "for (count=0; count<..."

	if (selectionInfoPtr->typeFlag == kPolygonType) 
		{
		selectionPointsPtr = (HPFieldPointsPtr)GetHandlePointer (
													selectionInfoPtr->polygonCoordinatesHandle);

		GetBoundingSelectionRectangles (displaySpecsPtr,
													 selectionInfoPtr,
													 selectionPointsPtr,
													 0);

		}	// end "if (selectionInfoPtr->typeFlag == kPolygonType)"

	CheckAndUnlockHandle (selectionInfoPtr->polygonCoordinatesHandle);

}	// end "SetPolygonSelection" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetRectangleSelection
//
//	Software purpose:	This routine sets the rectangle selection information in
//							the inpout selection info structure to the input line and colums. 
//							Checks are made to verify that the values still make sense after 
//							the adjustment relative to the image file.  Also the selected
//							rectange area in map coordinates is computed based on the
//							line and columns.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			SetSelectionForAllWindows
//
//	Revised By:			Larry L. Biehl			Date: 10/02/1998	
//	Revised By:			Larry L. Biehl			Date: 02/22/2007			

void SetRectangleSelection (
				WindowInfoPtr						windowInfoPtr,
				SelectionInfoPtr					selectionInfoPtr,
				LongRect*							inputLineColumnRectPtr,
				DoubleRect*							inputCoordinateRectPtr,
				SInt16								unitsToUseCode,
				SInt32								lineAdjust,
				SInt32								columnAdjust)
 
{
	DoubleRect*							selectionCoordinateRectPtr;
	LongRect*							selectionLineColumnRectPtr;

	SInt32								tempValue;

	Boolean								coordinatesExistFlag;


	selectionLineColumnRectPtr = &selectionInfoPtr->lineColumnRectangle;
	selectionCoordinateRectPtr = &selectionInfoPtr->coordinateRectangle;

			// Get the factor that is being used to convert the original units to
			// the requested display units.

	//factor = GetCoordinateViewFactor (windowInfoPtr->windowInfoHandle);
	coordinatesExistFlag = ConvertCoordinateRectToLCRect (
																	windowInfoPtr->windowInfoHandle,
																	inputCoordinateRectPtr,
																	selectionLineColumnRectPtr,
																	unitsToUseCode,
																	1.0);

	if (coordinatesExistFlag) 
		{
		if (unitsToUseCode == kLineColumnUnits) 
			{
			selectionLineColumnRectPtr->top = inputLineColumnRectPtr->top + lineAdjust;
			selectionLineColumnRectPtr->bottom = inputLineColumnRectPtr->bottom + lineAdjust;

			selectionLineColumnRectPtr->left = inputLineColumnRectPtr->left + columnAdjust;
			selectionLineColumnRectPtr->right = inputLineColumnRectPtr->right + columnAdjust;

			}	// end "if (unitsToUseCode == kLineColumnUnits)"

				// Make certain that the new line and column values are 		
				// within the ranges for this window.								

				// This makes certain that the values are not less than 0.	

		selectionLineColumnRectPtr->top = MAX (selectionLineColumnRectPtr->top, 1);
		selectionLineColumnRectPtr->left = MAX (selectionLineColumnRectPtr->left, 1);

		selectionLineColumnRectPtr->bottom = MAX (selectionLineColumnRectPtr->bottom, 1);
		selectionLineColumnRectPtr->right = MAX (selectionLineColumnRectPtr->right, 1);

				// If the units to use are lat-long, it is possible for some projections 
				// for the line-column values to be in the wrong order.  Correct if they 
				// are in the wrong order.

		if (unitsToUseCode == kLatLongUnits) 
			{
			if (selectionLineColumnRectPtr->top > selectionLineColumnRectPtr->bottom) 
				{
				tempValue = selectionLineColumnRectPtr->bottom;
				selectionLineColumnRectPtr->bottom = selectionLineColumnRectPtr->top;
				selectionLineColumnRectPtr->top = tempValue;

				}	// end "if (selectionLineColumnRectPtr->top > ...->bottom)"

			if (selectionLineColumnRectPtr->left > selectionLineColumnRectPtr->right) 
				{
				tempValue = selectionLineColumnRectPtr->right;
				selectionLineColumnRectPtr->right = selectionLineColumnRectPtr->left;
				selectionLineColumnRectPtr->left = tempValue;

				}	// end "if (selectionLineColumnRectPtr->left > ...->right)"

			}	// end "if (unitsToUseCode == kLatLongUnits)"

				// This makes certain that the values are greater than the the maximums.															

		selectionLineColumnRectPtr->bottom = MIN (selectionLineColumnRectPtr->bottom,
				 (SInt32)windowInfoPtr->maxNumberLines);
				 
		selectionLineColumnRectPtr->right = MIN (selectionLineColumnRectPtr->right,
				 (SInt32)windowInfoPtr->maxNumberColumns);

		if (selectionLineColumnRectPtr->top > selectionLineColumnRectPtr->bottom)
			selectionInfoPtr->typeFlag = 0;

		if (selectionLineColumnRectPtr->left > selectionLineColumnRectPtr->right)
			selectionInfoPtr->typeFlag = 0;

		}	// end "if (coordinatesExistFlag)"

	else	// !coordinatesExistFlag
		selectionInfoPtr->typeFlag = 0;

}	// end "SetRectangleSelection" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetSelectionForAllWindows
//
//	Software purpose:	This routine sets the input line and column values
//							for all open image windows.  This routine is called
//							if a selection is made while the command-key is held
//							down.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			SelectArea in selectionArea.c
//							EditSelectionDialogShowSelection
//
//	Revised By:			Larry L. Biehl			Date: 06/03/1993	
//	Revised By:			Larry L. Biehl			Date: 05/29/2015			

void SetSelectionForAllWindows (
				Handle								inputWindowInfoHandle,
				SelectionInfoPtr					inputSelectionInfoPtr,
				DoubleRect*							inputCoordinateRectPtr,
				Boolean								useStartLineColumnFlag,
				SInt16								unitsToUseCode,
				double								inputFactor)

{
	DoubleRect							coordinateRectangle;

	DisplaySpecsPtr					displaySpecsPtr;
	FileInfoPtr							fileInfoPtr;
	GrafPtr								savedPort;
	LongRect*							inputLineColumnRectPtr;
	SelectionInfoPtr					selectionInfoPtr;
	WindowInfoPtr						windowInfoPtr;

	Handle								displaySpecsHandle,
											fileInfoHandle,
											inputPolygonCoordinatesHandle,
											polygonCoordinatesHandle,
											windowInfoHandle;

	SInt32								activeStartColumn,
											activeStartLine,
											columnAdjust,
											inputNumberPoints,
											lineAdjust;

	SInt16								inputType,
											planarMapUnitsCode,
											window,
											windowIndex;

	Boolean								continueFlag,
											stopFlag;

	SignedByte							displayHandleStatus,
											fileHandleStatus,
											selectHandleStatus,
											windowHandleStatus;


	if (gNumberOfIWindows <= 1)
																									return;

	GetPort (&savedPort);

			// Get the input parameters.

	inputType = inputSelectionInfoPtr->typeFlag;
	inputNumberPoints = inputSelectionInfoPtr->numberPoints;
	inputLineColumnRectPtr = &inputSelectionInfoPtr->lineColumnRectangle;
	inputPolygonCoordinatesHandle = inputSelectionInfoPtr->polygonCoordinatesHandle;

			// Get the line and column start for the active image window.			

	fileInfoHandle = GetFileInfoHandle (inputWindowInfoHandle);
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);

	if (fileInfoPtr != NULL) 
		{
		activeStartLine = fileInfoPtr->startLine;
		activeStartColumn = fileInfoPtr->startColumn;

		}	// end "if (fileInfoPtr != NULL)" 

	columnAdjust = 0;
	lineAdjust = 0;

	coordinateRectangle = *inputCoordinateRectPtr;

			// Convert the cooordinates to the map units expected to the original 
			// units for the base window.

	if (unitsToUseCode == kMapUnits) 
		{
		coordinateRectangle.top /= inputFactor;
		coordinateRectangle.left /= inputFactor;
		coordinateRectangle.bottom /= inputFactor;
		coordinateRectangle.right /= inputFactor;

		}	// end "if (unitsToUseCode == kMapUnits)"

			// Loop through all the available windows and set the selection	
			// coordinates.																	

	stopFlag = FALSE;
	window = 0;
	windowIndex = kImageWindowStart;
	while (window < gNumberOfIWindows) 
		{
		if (gWindowList[windowIndex] != gActiveImageWindow) 
			{
			continueFlag = TRUE;

			selectionInfoPtr = NULL;
			displaySpecsPtr = NULL;
			fileInfoPtr = NULL;

			windowInfoHandle = GetWindowInfoHandle (gWindowList[windowIndex]);
			windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

			if (windowInfoPtr == NULL)
				 continueFlag = FALSE;

			if (continueFlag) 
				{
						// Verify that an image exists in this window.					

				if (windowInfoPtr->imageBaseAddressH == NULL &&
															windowInfoPtr->offscreenGWorld == NULL)
					continueFlag = FALSE;

				}	// end "if (continueFlag)"		

			if (continueFlag) 
				{
						// Determine if the information handles are available.

				displaySpecsHandle = GetDisplaySpecsHandle (windowInfoPtr);
				fileInfoHandle = GetFileInfoHandle (windowInfoPtr);

				if (displaySpecsHandle == NULL ||
						windowInfoPtr->selectionInfoHandle == NULL ||
							fileInfoHandle == NULL)
					continueFlag = FALSE;

				}	// end "if (continueFlag)" 

			if (continueFlag) 
				{
						// There is an image to draw into.
						// Check whether one can determine the selected area within
						// the image with the user requested units.

				if (unitsToUseCode == kMapUnits) 
					{
					planarMapUnitsCode = GetFilePlanarMapUnitsCode (windowInfoHandle);
					if (planarMapUnitsCode < kKilometersCode)
						continueFlag = FALSE;

					}	// end "if (unitsToUseCode == kMapUnits)"

				else if (unitsToUseCode == kLatLongUnits)
					continueFlag = DetermineIfInverseLatLongPossible (windowInfoHandle);

				}	// end "if (continueFlag)"

			if (continueFlag) 
				{
						// Lock the handles down for the pointers being used.			
						// Be sure no code added before this will move memory.

				windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
																					windowInfoHandle,
																					&windowHandleStatus);

				fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
																					fileInfoHandle,
																					&fileHandleStatus);

				selectionInfoPtr = (SelectionInfoPtr)GetHandleStatusAndPointer (
																	windowInfoPtr->selectionInfoHandle,
																	&selectHandleStatus);

				displaySpecsPtr = (DisplaySpecsPtr)GetHandleStatusAndPointer (
																					displaySpecsHandle,
																					&displayHandleStatus);

				polygonCoordinatesHandle = NULL;
				if (inputType == kPolygonType) 
					{
							// Get the new polygon handle for this selection.

					polygonCoordinatesHandle = inputPolygonCoordinatesHandle;

					if (HandToHand (&polygonCoordinatesHandle) != noErr)
						stopFlag = TRUE;

					}	// end "if (inputType == kPolygonType)"

				if (!stopFlag) 
					{
							// Invalidate the current selection rectangle if it exists.	

					ClearSelectionArea (gWindowList[windowIndex]);

							// For the selection flag to be the same as the input one.

					selectionInfoPtr->typeFlag = inputType;
					selectionInfoPtr->numberPoints = (SInt16)inputNumberPoints;
					selectionInfoPtr->polygonCoordinatesHandle = polygonCoordinatesHandle;

					if (useStartLineColumnFlag && unitsToUseCode == kLineColumnUnits) 
						{
								// Adjust the line and column values for any differences		
								// in the start line and column.										

						lineAdjust = activeStartLine - fileInfoPtr->startLine;
						columnAdjust = activeStartColumn - fileInfoPtr->startColumn;

						}	// end "if (useStartLineColumnFlag && ...)"

					if (selectionInfoPtr->typeFlag == kRectangleType)
						SetRectangleSelection (windowInfoPtr,
														 selectionInfoPtr,
														 inputLineColumnRectPtr,
														 &coordinateRectangle,
														 unitsToUseCode,
														 lineAdjust,
														 columnAdjust);

					else	// selectionInfoPtr->typeFlag == kPolygonType
						SetPolygonSelection (windowInfoPtr,
													 selectionInfoPtr,
													 displaySpecsPtr,
													 lineAdjust,
													 columnAdjust);

							// Now also get the selected area in coordinate units.

					ComputeSelectionCoordinates (windowInfoHandle,
															 &selectionInfoPtr->lineColumnRectangle,
															 &selectionInfoPtr->coordinateRectangle);

							// Get the selection rectangle location relative to the 		
							// offscreen bit/pix map												

					if (selectionInfoPtr->typeFlag > 0) 
						{
						SetSelectionInformation (gWindowList[windowIndex],
														  displaySpecsPtr,
														  selectionInfoPtr,
														  selectionInfoPtr->typeFlag,
														  0,
														  &selectionInfoPtr->lineColumnRectangle,
														  &selectionInfoPtr->coordinateRectangle);

						SetPortWindowPort (gWindowList[windowIndex]);
						OutlineSelectionArea (gWindowList[windowIndex]);

						#if defined multispec_win || defined multispec_lin
							gWindowList[windowIndex]->UpdateSelectionCoordinates ();
						#endif	// defined multispec_win || defined multispec_lin

						}	// end "if (selectionInfoPtr->typeFlag > 0)" 

					}	// end "if (!stopFlag)"

						// Reset the handles to their input lock status.				

				MHSetState (windowInfoPtr->selectionInfoHandle, selectHandleStatus);
				MHSetState (displaySpecsHandle, displayHandleStatus);
				MHSetState (fileInfoHandle, fileHandleStatus);
				MHSetState (windowInfoHandle, windowHandleStatus);

				}	// end "if (continueFlag)" 

			}	// end "if (gWindowList[windowIndex] != gActiveImageWindow)" 

		window++;
		windowIndex++;

		if (stopFlag)
			break;

		}	// end "while (window<gNumberOfIWindows)" 

	SetPort (savedPort);

}	// end "SetSelectionForAllWindows"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetSelectionInformation
//
//	Software purpose:	
//
//	Parameters in:		None		
//
//	Parameters out:	None			
//
//	Value Returned:	None
// 
// Called By:			RectangleSelection in selectionArea.c
//							EditSelectionDialog in SSelUtil.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/13/1998
//	Revised By:			Larry L. Biehl			Date: 11/08/2000			

void SetSelectionInformation (
				WindowPtr							windowPtr,
				DisplaySpecsPtr					displaySpecsPtr,
				SelectionInfoPtr					selectionInfoPtr,
				SInt16								typeFlag,
				SInt16								startChannel,
				LongRect*							lineColumnRectPtr,
				DoubleRect*							coordinateRectPtr)
 
{
	Rect offScreenRectangle;


	ComputeSelectionOffscreenRectangle (displaySpecsPtr,
													lineColumnRectPtr,
													&offScreenRectangle,
													startChannel);

	selectionInfoPtr->typeFlag = typeFlag;
	selectionInfoPtr->coordinateRectangle = *coordinateRectPtr;
	selectionInfoPtr->lineColumnRectangle = *lineColumnRectPtr;
	selectionInfoPtr->offScreenRectangle = offScreenRectangle;
	selectionInfoPtr->numberPixels = GetNumberPixelsInSelection (selectionInfoPtr);

}	// end "SetSelectionInformation" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ShowGraphSelection
//
//	Software purpose:	
//
//	Parameters in:		None		
//
//	Parameters out:	None			
//
//	Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/09/1991
//	Revised By:			Larry L. Biehl			Date: 10/15/1995			

void ShowGraphSelection (void)

{
	if (gSelectionGraphViewCPtr != NULL &&
									gSelectionGraphViewCPtr->GetSelectionIOHandle () != NULL)
		ShowGraphWindowSelection (NULL);

}	// end "ShowGraphSelection" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateSelectionCoordinates
//
//	Software purpose:	
//
//	Parameters in:		None		
//
//	Parameters out:	None			
//
//	Value Returned:	None
// 
// Called By:			CoordinateUnitsControlEvent in controls.c
//
//	Coded By:			Larry L. Biehl			Date: 11/08/2000
//	Revised By:			Larry L. Biehl			Date: 12/28/2000			

void UpdateSelectionCoordinates (
							Handle					windowInfoHandle)
							
{
	SelectionInfoPtr					selectionInfoPtr;

	Handle								selectionInfoH;


	selectionInfoH = GetSelectionInfoHandle (windowInfoHandle);
	selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (selectionInfoH, kLock);

	ComputeSelectionCoordinates (windowInfoHandle,
											&selectionInfoPtr->lineColumnRectangle,
											&selectionInfoPtr->coordinateRectangle);

	CheckAndUnlockHandle (selectionInfoH);

}	// end "UpdateSelectionCoordinates" 
