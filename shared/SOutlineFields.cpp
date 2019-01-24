//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/05/2019
//
//	File:						SOutlineFields.cpp
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains routines that outline training
//								and/or test fields in the project image window.
//
//	Functions in file: 	void 					ForceFieldOutlineUpdate
//								void 					OutlineClassFields
//								void				 	OutlineField
//								void 					OutlineFieldsControl
//								void					OutlineFieldsInProjectBaseWindows
//								void 					OutlineFieldsInProjectWindows
//								void 					OutlineProjectFields
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 
  
#if defined multispec_win 
	#include "WImageView.h"
#endif	// defined multispec_win 

#if defined multispec_lin   
	#include "LImageView.h"
#endif	// defined multispec_lin  

//#include "SExtGlob.h" 



			// Prototypes for routines in this file that are only called by		
			// other routines in this file.	
			
double GetAngle (
				HPFieldPointsPtr					originPointPtr,
				HPFieldPointsPtr					endPointPtr);
			
double GetHalfAngle (
				HPFieldPointsPtr					lastPointPtr,
				HPFieldPointsPtr					currentPointPtr,
				HPFieldPointsPtr					nextPointPtr);											
			
void OutlineClassFields (
				SInt16								outlineFieldType, 
				SInt16								classNumber, 
				Handle								windowInfoHandle,
				SInt16								windowCode,
				LCToWindowUnitsVariablesPtr	lcToWindowUnitsVariablesPtr);

void OutlineField (
				SInt16								classNumber, 
				SInt16								fieldNumber, 
				Handle								windowInfoHandle,
				SInt16								windowCode,
				LCToWindowUnitsVariablesPtr	lcToWindowUnitsVariablesPtr);

void OutlineProjectFields (
				SInt16								outlineFieldType, 
				Handle								windowInfoHandle,
				SInt16								windowCode,
				LCToWindowUnitsVariablesPtr	lcToWindowUnitsVariablesPtr);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ForceFieldOutlineUpdate
//
//	Software purpose:	The purpose of this routine is to force an update
//							event for the project image windows to draw the
//							outlines of the training and/or test fields.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
//	Called By:			AddField 					(in SEditStatistics.cpp)
//							CutClass 					(in SEditStatistics.cpp)
//							CutField 					(in SEditStatistics.cpp)
//							EditCoordinatesDialog 	(in SEditStatistics.cpp)
//							FieldListStatMode 		(in SStatistics.cpp)
// 						PolygonListStatMode 		(in SStatistics.cpp)
//
//	Coded By:			Larry L. Biehl			Date: 01/12/1989
//	Revised By:			Larry L. Biehl			Date: 09/24/2015

void ForceFieldOutlineUpdate (
				Boolean								forceFlag)

{
	WindowInfoPtr						windowInfoPtr;
	WindowPtr							windowPtr;
	
	Handle								windowInfoHandle;
	
	SInt16								windowCount,
											windowListIndex;
		
			
			// If fields are to be outlined in the image windows, go through 		
			// window list to find the project image windows and force an 			
			// update event so that only the input field is outlined.				
			
	if ((forceFlag || gProjectInfoPtr->outlineFieldType != 0) && gNumberOfIWindows > 0)
		{                    
		windowCount = 0;
		windowListIndex = kImageWindowStart;
		do
			{
			windowPtr = gWindowList[windowListIndex];         
			windowInfoHandle = GetWindowInfoHandle (windowPtr);
			windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
			
			if (windowInfoPtr->projectWindowFlag)  
				InvalidateWindow (windowPtr, kImageFrameArea, FALSE);
			
			windowListIndex++;
			windowCount++;
														
			}	while (windowCount < gNumberOfIWindows); 
		
		}	// end "if ((forceFlag || gProjectInfoPtr->...)" 
		
}	// end "ForceFieldOutlineUpdate" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double GetAngle
//
//	Software purpose:	The purpose of this routine is to get the angle represented by
//							the input two points.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/27/1998
//	Revised By:			Larry L. Biehl			Date: 03/27/1998	

double GetAngle (
				HPFieldPointsPtr					originPointPtr,
				HPFieldPointsPtr					endPointPtr)

{	
	double								angle;
											
	SInt32								colDif,
											lineDif;
											
											
	lineDif = endPointPtr->line - originPointPtr->line;
	colDif = endPointPtr->col - originPointPtr->col;
	angle = atan2 ((double)lineDif, (double)colDif);
	
	return (angle);
		
}	// end "GetAngle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double GetHalfAngle
//
//	Software purpose:	The purpose of this routine is to find the line column point
//							to use as the starting point for the label for a polygon field.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/27/1998
//	Revised By:			Larry L. Biehl			Date: 03/27/1998	

double GetHalfAngle (
				HPFieldPointsPtr					lastPointPtr,
				HPFieldPointsPtr					currentPointPtr,
				HPFieldPointsPtr					nextPointPtr)

{	
	double								halfAngle,
											lastAngle,
											nextAngle;
											
											
	nextAngle = GetAngle (currentPointPtr, nextPointPtr);
	
	lastAngle = GetAngle (currentPointPtr, lastPointPtr);
	
	halfAngle = .5 * (nextAngle + lastAngle);
	
	return (halfAngle);
		
}	// end "GetHalfAngle" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetPolygonLabelPoint
//
//	Software purpose:	The purpose of this routine is to find the line column point
//							to use as the starting point for the label for a polygon field.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/27/1998
//	Revised By:			Larry L. Biehl			Date: 03/19/1999	

void GetPolygonLabelPoint (
				HPFieldIdentifiersPtr			fieldIdentPtr,
				HPFieldPointsPtr					fieldPointsPtr,
				LongPoint*							labelPointPtr)

{	
	LongRect								regionBoxRectangle;
		
	double								halfAngle,
											labelOffset,
											lastAngle,
											nextAngle,
											tangent;
											
	Point									checkPoint;
											
	RgnHandle							rgnHandle = NULL;
											
	SInt32								colDif,
											lastColDif,
											lastLineDif,
											lineDif;
	
	SInt16								endIndex,  
											lastIndex,
											nextIndex,
											pLabelPointIndex = -1,
											pointIndex,
											sLabelPointIndex = -1;
											
	
			// Allow for the first two points which represent the bounding rectangle.
						
	pointIndex = fieldIdentPtr->firstPolygonPoint + 2;
	
	endIndex = pointIndex + fieldIdentPtr->numberOfPolygonPoints - 1;
	nextIndex = pointIndex + 1;
	lastIndex = endIndex;
	
	CreateFieldRgn (fieldIdentPtr->numberOfPolygonPoints,
							&fieldPointsPtr[pointIndex], 
							&rgnHandle,
							0,
							0);
	
			// Load the bounding rectangle for later use.
			
	if (rgnHandle != NULL)
		{
		GetBoundingRegionRectangle (rgnHandle, 
												&regionBoxRectangle,
												(UInt32)fieldIdentPtr->numberOfPolygonPoints,
												&fieldPointsPtr[pointIndex],
												0,
												0);
		
		}	// end "if (rgnHandle != NULL)"
		
	else	// rgnHandle == NULL
		{
		regionBoxRectangle.top = 1;
		regionBoxRectangle.left = 1;
		regionBoxRectangle.bottom = 1;
		regionBoxRectangle.right = 1;
		
		}	// end "else rgnHandle == NULL"
		
	pointIndex = fieldIdentPtr->firstPolygonPoint;
	fieldPointsPtr[pointIndex].line = regionBoxRectangle.top;
	fieldPointsPtr[pointIndex].col = regionBoxRectangle.left;
	
	pointIndex++;
	fieldPointsPtr[pointIndex].line = regionBoxRectangle.bottom;
	fieldPointsPtr[pointIndex].col = regionBoxRectangle.right;
		
	pointIndex = fieldIdentPtr->firstPolygonPoint + 2;
	
	while (pointIndex <= endIndex)
		{
		halfAngle = GetHalfAngle (&fieldPointsPtr[lastIndex],
											&fieldPointsPtr[pointIndex],
											&fieldPointsPtr[nextIndex]);
				
				// Get label point
				
		if (halfAngle > (double)(-kHALF_PI) && halfAngle < (double)kHALF_PI)
			{
			if (rgnHandle != NULL)
				{
						// Check if point up and to the right of this vertex is actually
						// within the area.
				
				tangent = fabs (tan (halfAngle));
				if (tangent >= 1)
					{
					colDif = 1;
					lineDif = (SInt32)(tangent +.5);
					
					}	// end "if (tangent >= 1)"
					
				else if (tangent > .2)
					{
					colDif = (SInt32)(1./tangent + .5);
					lineDif = 1;
					
					}	// end "else if (tangent > .2)"
					
				else	// tangent <= .2   
					{
					colDif = 5;
					lineDif = 1;
					
					}	// end "else tangent < 1"
				
				}	// end "if (rgnHandle != NULL)"
				
			if (halfAngle >= 0 && halfAngle < kHALF_PI)
				{
				if (rgnHandle != NULL)
					{
							// Check if point down and to the right of this vertex is 
							// actually within the area.    
							
					#if defined multispec_mac 
						//checkPoint.h = (SInt16)(fieldPointsPtr[pointIndex].col + colDif);
						//checkPoint.v = (SInt16)(fieldPointsPtr[pointIndex].line + lineDif);
					
						checkPoint.h = (SInt16)fieldPointsPtr[pointIndex].col;
						checkPoint.v = (SInt16)fieldPointsPtr[pointIndex].line;
					#endif	// defined multispec_mac    
					  
					#if defined multispec_win    
						checkPoint.h = (SInt16)fieldPointsPtr[pointIndex].col + 1;
						checkPoint.v = (SInt16)fieldPointsPtr[pointIndex].line + 1; 
					#endif	// defined multispec_win    
					
					}	// end "if (rgnHandle != NULL)"
						
				if (rgnHandle == NULL || PtInRgn (checkPoint, rgnHandle))
					{
							// Get primary label point
							
					if (pLabelPointIndex < 0)
						pLabelPointIndex = pointIndex;
						
					else	// labelPointIndex >= 0
						{
						if (fieldPointsPtr[pointIndex].line < 
																fieldPointsPtr[pLabelPointIndex].line)
							pLabelPointIndex = pointIndex;
							
						else if (fieldPointsPtr[pointIndex].line == 
															fieldPointsPtr[pLabelPointIndex].line &&
									fieldPointsPtr[pointIndex].col < 
																fieldPointsPtr[pLabelPointIndex].col)
							pLabelPointIndex = pointIndex;
						
						}	// end "else labelPointIndex >= 0"
						
					}	// end "if (rgnHandle == NULL || PtInRgn (checkPoint, rgnHandle)"
					
				}	// end "if (halfAngle >= 0 && halfAngle < kHALF_PI)"
					
					// Get secondary label point
				
			else if (halfAngle > (double)(-kHALF_PI) && (double)halfAngle < 0)
				{
				if (rgnHandle != NULL)
					{
							// Check if point up and to the right of this vertex is actually
							// within the area.
							
					checkPoint.h = (SInt16)(fieldPointsPtr[pointIndex].col + colDif);
					checkPoint.v = (SInt16)(fieldPointsPtr[pointIndex].line - lineDif);
					
					}	// end "if (rgnHandle != NULL)"
							
				if (rgnHandle == NULL || PtInRgn (checkPoint, rgnHandle))
					{
					if (sLabelPointIndex < 0)
						sLabelPointIndex = pointIndex;
						
					else	// labelPointIndex >= 0
						{
						if (fieldPointsPtr[pointIndex].line < fieldPointsPtr[sLabelPointIndex].line)
							sLabelPointIndex = pointIndex;
						
						}	// end "else labelPointIndex >= 0"
						
					}	// end "if (rgnHandle == NULL || PtInRgn (checkPoint, rgnHandle)"
				
				}	// end "else projectionPoint_v - ..."
				
			}	// end "if (halfAngle > -kHALF_PI && halfAngle < kHALF_PI)"
			
		nextIndex++;
		if (nextIndex > endIndex)
			nextIndex = fieldIdentPtr->firstPolygonPoint + 2;
			
		lastIndex++;
		if (lastIndex > endIndex)
			lastIndex = fieldIdentPtr->firstPolygonPoint + 2;
		
		pointIndex++;
		
		}	// end "while (pointIndex <= endIndex)" 
		
			// We now have an upper left type of a vertex. Now adjust the label point down
			// if the next point to the right is lower.
	
	if (pLabelPointIndex < 0)
		pLabelPointIndex = sLabelPointIndex;
	
	if (pLabelPointIndex >= 0)
		{	
		nextIndex = pLabelPointIndex + 1;
		if (nextIndex > endIndex)
			nextIndex = fieldIdentPtr->firstPolygonPoint + 2;
			
		lastIndex = pLabelPointIndex - 1;
		if (lastIndex < fieldIdentPtr->firstPolygonPoint + 2)
			lastIndex = endIndex;
			
		nextAngle = GetAngle (&fieldPointsPtr[pLabelPointIndex],
										&fieldPointsPtr[nextIndex]);
			
		lastAngle = GetAngle (&fieldPointsPtr[pLabelPointIndex],
										&fieldPointsPtr[lastIndex]);
		
				// Make sure that the points are going clockwise.
					
		if (lastAngle < nextAngle)
			{
			pointIndex = nextIndex;
			nextIndex = lastIndex;
			lastIndex = pointIndex;
			
			}	// end "if (lastAngle < nextAngle)"
			
				// Get the label line. Adjust the label line down if need to try to stay
				// away from a field border that may be drawn across it.
			
		labelPointPtr->v = MAX (fieldPointsPtr[pLabelPointIndex].line, 
										fieldPointsPtr[nextIndex].line);
										
				// However make sure that the line is not lower that the line for the last 
				// point.
			
		labelPointPtr->v = MIN (labelPointPtr->v, 
										fieldPointsPtr[lastIndex].line);
			
				// Get the label column. Adjust the label column to the right if need to 
				// try to stay away from a field border that may be drawn across it.
				
		labelPointPtr->h = fieldPointsPtr[pLabelPointIndex].col;
		lastColDif = 
					fieldPointsPtr[lastIndex].col - fieldPointsPtr[pLabelPointIndex].col;
		lastLineDif = 
					fieldPointsPtr[lastIndex].line - fieldPointsPtr[pLabelPointIndex].line;
		
		if (lastLineDif > 0)
			{
			labelOffset = 0;
			if (lastColDif > 0)
				labelOffset = gProjectInfoPtr->labelSize + 1;
				
			labelPointPtr->h += (SInt16)(lastColDif *
				 (labelPointPtr->v - fieldPointsPtr[pLabelPointIndex].line + 
																			labelOffset) / lastLineDif);
				 			
			if (lastColDif > 0)																		
				labelPointPtr->h = MIN (labelPointPtr->h, fieldPointsPtr[lastIndex].col);
			
			}	// end "if (lastLineDif > 0)"
			
		}	// end "if (pLabelPointIndex >= 0)"
		
	else	// pLabelPointIndex < 0
		{
		labelPointPtr->v =  regionBoxRectangle.top;	
		labelPointPtr->h =  regionBoxRectangle.left;
		
		}	// end "else pLabelPointIndex < 0"
		
	if (rgnHandle != NULL)
		DisposeRgn (rgnHandle);
		
}	// end "GetPolygonLabelPoint" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void OutlineFieldsInProjectWindows
//
//	Software purpose:	The purpose of this routine is to outline the selected
//							fields in all the project image windows (Base images
//							and associated images).
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			UndoCutClass in editStatistics.c
//							OpenProjectFile in project.c
//							FieldListStatMode in statistics.c
//							AddFieldToProject in statistics.c
//							NewFieldStatMode in statistics.c
//							ClassListStatMode in statistics.c
//							StatisticsDialog in statistics.c
//
//	Coded By:			Larry L. Biehl			Date: 05/10/1990
//	Revised By:			Larry L. Biehl			Date: 09/24/2015	

void OutlineFieldsInProjectWindows (
				SInt16								statsWindowMode)

{
	WindowInfoPtr						windowInfoPtr;
	WindowPtr							windowPtr;
	
	Handle								windowInfoHandle;
	
	SInt16								windowCount,
											windowListIndex;
		
			
			// If fields are to be outlined in the image windows, go through 		
			// window list to find the project image windows and call the			
			// control routine to draw the field boundaries in that window.		
			
	if (gProjectInfoPtr->outlineFieldType != 0 && gNumberOfIWindows > 0)
		{
		windowCount = 0;
		windowListIndex = kImageWindowStart;
		do
			{
			windowPtr = gWindowList[windowListIndex];
			windowInfoHandle = GetWindowInfoHandle (windowPtr);
			windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
			
			if (windowInfoPtr->projectWindowFlag) 
				{  
				#if defined multispec_mac                                             
					OutlineFieldsControl (statsWindowMode, 
													windowPtr, 
													windowInfoHandle, 
													1);
				#endif	// defined multispec_mac 
						             
				#if defined multispec_win
					InvalidateWindow (windowPtr, kImageArea, FALSE);
				#endif	// defined multispec_win                        
				
				#if defined multispec_lin
					InvalidateWindow (windowPtr, kImageArea, FALSE);
               (windowPtr->m_Canvas)->Update ();
				#endif	// defined multispec_lin
				      
				}	// end "if (windowInfoPtr->projectWindowFlag)"
				                  
			windowListIndex++;
			windowCount++;
														
			}	while (windowCount<gNumberOfIWindows);
		
		}	// end "if (gProjectInfoPtr->outlineFieldType != 0 && ...)" 

}	// end "OutlineFieldsInProjectWindows" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void OutlineFieldsInProjectBaseWindows
//
//	Software purpose:	The purpose of this routine is to outline the selected
//							fields in all the project base image windows.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			ChangeProjectBaseImage in project.c
//
//	Coded By:			Larry L. Biehl			Date: 06/26/1990
//	Revised By:			Larry L. Biehl			Date: 10/09/2015	

void OutlineFieldsInProjectBaseWindows (
				SInt16								statsWindowMode)

{
	WindowInfoPtr						windowInfoPtr;
	WindowPtr							windowPtr;
	
	Handle								windowInfoHandle;
	
	SInt16								windowCount,
											windowListIndex;
		
			
			// If fields are to be outlined in the image windows, go through 		
			// window list to find the project base image windows and call the	
			// control routine to draw the field boundaries in that window.		
			
	if (gProjectInfoPtr->outlineFieldType != 0 && gNumberOfIWindows > 0)
		{
		windowCount = 0;
		windowListIndex = kImageWindowStart;
		do
			{
			windowPtr = gWindowList[windowListIndex];
			windowInfoHandle = (Handle)GetWindowInfoHandle (windowPtr);
			windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
			
			if (windowInfoPtr->projectBaseImageFlag)     
				#if defined multispec_mac                                             
					OutlineFieldsControl (statsWindowMode, 
													windowPtr, 
													windowInfoHandle, 
													1);
				#endif	// defined multispec_mac 
						             
				#if defined multispec_win || defined multispec_lin
					InvalidateWindow (windowPtr, kImageArea, FALSE);
				#endif	// defined multispec_win || defined multispec_lin                           
				
			windowListIndex++;
			windowCount++;
														
			}	while (windowCount<gNumberOfIWindows);
		
		}	// end "if (gProjectInfoPtr->outlineFieldType != 0 && ...)" 

}	// end "OutlineFieldsInProjectBaseWindows" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void OutlineFieldsControl
//
//	Software purpose:	The purpose of this routine is to control the
//							process of outlining the training and/or test fields
//							in a project image window or the printer window.  The
//							window to be written to is an input parameter.
//
//	Parameters in:		windowCode;		=1, kToImageWindow;
//												=2, kToPrintWindow
//												=3, kToClipboardWindow
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			CopyOffScreenImage in multiSpec.c
//							PrintImageWindow in print.c
//							DisplayMultispectralImage in SDisMult.cpp
//							OutlineFieldsInProjectWindows in SOutFlds.cpp
//							OutlineFieldsInProjectBaseWindows in SOutFlds.cpp
//							ChangeProjectAssociatedImageItem in SProjUtl.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/10/1989
//	Revised By:			Larry L. Biehl			Date: 11/02/2018

void OutlineFieldsControl (
				SInt16								statsWindowMode,
				WindowPtr							windowPtr, 
				Handle								windowInfoHandle,
				SInt16								windowCode)

{	
	LCToWindowUnitsVariables		lcToWindowUnitsVariables;
						
	GrafPtr								savedPort;
	WindowInfoPtr						windowInfoPtr;
	
	SInt16								classNumber,
											fieldNumber,
											outlineFieldType;
											
	Boolean								continueFlag;
	
	SignedByte							windowHandleStatus;
			
	#if defined multispec_mac
		Pattern								pattern;
		PenState								penState; 
	#endif	// defined multispec_mac       
	
	#if defined multispec_lin
		wxPen									overlayPen;
		wxPen*								overlayPenPtr = NULL;
	#endif	// defined multispec_win          
			                       

			// Check input variables.															
			
	if (gProjectInfoPtr == NULL)
																								return;
							
	#if defined multispec_win																
		if (gCDCPointer == NULL || windowPtr == NULL)            
																								return; 
	#endif	// defined multispec_win  
	
	#if defined multispec_mac															
		if (windowCode == kToImageWindow && windowPtr == NULL)
																								return; 
	#endif	// defined multispec_mac                                   

	#if defined multispec_lin		
		/*
		wxMemoryDC displaydc;
		bool bitok = (windowPtr->m_ScaledBitmap).IsOk ();
		displaydc.SelectObject (windowPtr->m_ScaledBitmap);
		wxDC* oldgCDCpt = gCDCPointer; // Save the global DC pointer 
		gCDCPointer = &displaydc;
      
      wxClientDC dc (windowPtr->m_Canvas);
      gCDCPointer = &dc;
		*/
		if (gCDCPointer == NULL)
																								return;
	#endif	// defined multispec_lin
	
	windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
								windowInfoHandle, &windowHandleStatus, kNoMoveHi);
			
	continueFlag = (windowInfoPtr != NULL); 
	
	if (continueFlag)
		continueFlag = (GetDisplayedLineStart (windowInfoPtr) > 0);
	
	if (continueFlag)
		{    
		#if defined multispec_win  	
			LOGBRUSH 			logbrush;
			LOGFONT				logfont;
			
			CBrush 				brush;
			CFont 				font;
			
			CBrush* 				pOldBrush; 
			CPen* 				pOldPen;
			CPen* 				newPenPtr;
			CFont* 				pOldFont;
		#endif	// defined multispec_win 
				
		outlineFieldType = gProjectInfoPtr->outlineFieldType;
		if (outlineFieldType >= kTrainingType && outlineFieldType <= kBothFieldTypes &&
												windowInfoPtr->projectWindowFlag)
			{
			classNumber = gProjectInfoPtr->currentClass;
			fieldNumber = gProjectInfoPtr->currentField;
			
					// Save the current port and make active port the project image	
					//	window.																			
					
			GetPort (&savedPort);
			
			if (windowPtr != NULL)
				SetPortWindowPort (windowPtr);
			
			#if defined multispec_mac
						// Save the current pen state.												
						
				GetPenState (&penState);
				
						// Set the pen state for outlining the fields.
						
				if (gProjectInfoPtr->outlineColorCode == 1)
					PenPat (GetQDGlobalsBlack (&pattern));	
					
				else	// gProjectInfoPtr->outlineColorCode != 1
					PenPat (GetQDGlobalsWhite (&pattern));	

				PenMode (patCopy);
				PenSize (1,1);
				
				if (gProjectInfoPtr->labelFieldCode != 0)
					{
							// Set the text size and the text transfer mode for the window.	
					
					SInt16	textFont;	
					if (gFontMgrPresentFlag)		
						textFont = FMGetFontFamilyFromName ("\pgeneva");
						
					else	// !gCarbonFlag
						GetFNum ("\pgeneva", &textFont);
		
					if (textFont == 0)		
						textFont = 3;
					TextFont (textFont);
			
					TextSize (gProjectInfoPtr->labelSize);
					
					if (gProjectInfoPtr->outlineColorCode == 1)
						TextMode (srcOr);
						
					else	// gProjectInfoPtr->outlineColorCode != 1
						TextMode (srcBic);
						
					}	// end "if (gProjectInfoPtr->labelFieldCode != 0)"
			#endif	// defined multispec_mac  
			
			#if defined multispec_win  				
				GetWindowClipRectangle (windowPtr, kImageFrameArea, &gViewRect);
				if (!gMFC_Rgn.CreateRectRgn (gViewRect.left,
														gViewRect.top,
														gViewRect.right,
														gViewRect.bottom))
					continueFlag = FALSE;
				
				if (continueFlag)
					{	
					if (gProjectInfoPtr->outlineColorCode == 1)
						newPenPtr = &gBlackPen;
					
					else	// gProjectInfoPtr->outlineColorCode != 1
						newPenPtr = &gWhitePen;
				                   
					logbrush.lbStyle = BS_HOLLOW;
					
					if (gProjectInfoPtr->outlineColorCode == 1)
						logbrush.lbColor = RGB (0, 0, 0);
					
					else	// gProjectInfoPtr->outlineColorCode != 1
						logbrush.lbColor = RGB (255, 255, 255);
						
					logbrush.lbHatch = HS_HORIZONTAL; 
				   
					::GetObject (GetStockObject (SYSTEM_FIXED_FONT), 
										sizeof (LOGFONT), 
										&logfont);
					
							// Change to Courier font. 
				                           
					//logfont.lfHeight = 0xfffa;
					logfont.lfHeight = -10;
					if (windowCode == kToPrintWindow)
						logfont.lfHeight = 
								(SInt16)(windowPtr->m_printerTextScaling * logfont.lfHeight);
						   
					logfont.lfWidth = 0x0000;    
					logfont.lfQuality = 0x01;
					logfont.lfFaceName[0] = 'C';
					logfont.lfFaceName[1] = 'o'; 
					logfont.lfFaceName[2] = 'u';
					logfont.lfFaceName[3] = 'r';
					logfont.lfFaceName[4] = 'i';
					logfont.lfFaceName[5] = 'e';
					logfont.lfFaceName[6] = 'r';
					logfont.lfFaceName[7] = 0x00;                              
	            
	            		// Set brush to be used.
	            		
					if (!brush.CreateBrushIndirect (&logbrush))
						continueFlag = FALSE;
						
					}	// end "if (continueFlag)"                               
				/*
            if (continueFlag)
            	{
            		// Set pen to be used.
				 
					if (!pen.CreatePenIndirect (&logpen))
						continueFlag = FALSE;
				 
					}	// end " if (continueFlag)"
				*/
            if (continueFlag)
            	{    
	            		// Set font to be used.
				                                                              
					if (!font.CreateFontIndirect (&logfont)) 
						continueFlag = FALSE; 
						
					}	// end " if (continueFlag)"                               
            
            if (continueFlag)
            	{    
					pOldBrush = gCDCPointer->SelectObject (&brush);
					pOldPen = gCDCPointer->SelectObject (newPenPtr);      
					pOldFont = gCDCPointer->SelectObject (&font); 
					
					gCDCPointer->SetBkMode (TRANSPARENT);
					 
					if (gProjectInfoPtr->outlineColorCode == 1)
						gCDCPointer->SetTextColor (RGB (0, 0, 0)); 
						
					else	// gProjectInfoPtr->outlineColorCode == 2 
						gCDCPointer->SetTextColor (RGB (255, 255, 255)); 
						
					}	// end " if (continueFlag)"                                
			#endif	// defined multispec_win
      
			#if defined multispec_lin
				GetWindowClipRectangle (windowPtr, kImageFrameArea, &gViewRect);
				if (gProjectInfoPtr->outlineColorCode == 1)
					{
					overlayPenPtr = new wxPen (*wxBLACK);
					gCDCPointer->SetTextForeground (*wxBLACK);
					
					}	// end "if (gProjectInfoPtr->outlineColorCode == 1)"
					
				else	// gProjectInfoPtr->outlineColorCode != 1
					{
					overlayPenPtr = new wxPen (*wxWHITE);
					gCDCPointer->SetTextForeground (*wxWHITE);
					
					}	// end "else gProjectInfoPtr->outlineColorCode != 1"
							
				gCDCPointer->SetPen (*overlayPenPtr);  
				gCDCPointer->SetBrush (*wxTRANSPARENT_BRUSH);
				/*
						// Set text description						
            int width, height; // Get the size of the DC in pixels
            gCDCPointer->GetSize (&width, &height);
            float scaleX = (float)(width/gChannelWindowInterval);
            float scaleY = (float)(height/gChannelWindowInterval);
            float actualScale = wxMin (scaleX, scaleY);
            printf ("measured scale = %.1f\n", actualScale);
				int pointSize = 9/actualScale;	
				*/		
				wxFont font (gFontSize, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL); 
            gCDCPointer->SetFont (font);
            gCDCPointer->SetUserScale (1, 1);
			#endif	// defined multispec_lin
		
	      if (continueFlag)
	      	{ 
						// Set the global variables needed to convert from 					
						// line-column units to window units.										
									
				SetChannelWindowVariables (windowCode, windowInfoHandle, kNotCoreGraphics);
				
				SetLCToWindowUnitVariables (windowInfoHandle,
														windowCode, 
														TRUE,
														&lcToWindowUnitsVariables);		
														
				#ifdef multispec_lin
							// Magnification is not need for linux. It is already take into
							// account.
					//lcToWindowUnitsVariables.magnification = 1;
				#endif	// defined multispec_lin

						// Set parameters to properly copy offscreen to destination window.
						
				SetDestinationWindowParameters (windowCode, windowPtr);

						// Get the bounding clipping rectangle. It will be used in the
						// 'OutlineField' routine.	 
					    
				#if defined multispec_mac    							
					if (windowCode == kToImageWindow || windowCode == kToPrintWindow)
						GetWindowClipRectangle (windowPtr, kImageFrameArea, &gViewRect);
				#endif	// defined multispec_mac   

				#if defined multispec_win  
					if (windowCode == kToPrintWindow || windowCode == kToClipboardWindow)
						{                     
						if (gCDCPointer != NULL)
							{
							RECT clipBox;
									  
							gCDCPointer->GetClipBox (&clipBox);
							
							clipBox.left = 0;
							clipBox.top = 0;
							gViewRect.left = 
									clipBox.left + GetLegendWidthForWindow (windowInfoHandle);
							gViewRect.right = clipBox.right;              
							gViewRect.top = lcToWindowUnitsVariables.imageTopOffset;       
							gViewRect.bottom = clipBox.bottom; 
							
							ClipRect (&gViewRect);
									
							}	// end "if (gCDCPointer != NULL)" 
								
						}	// end "if (windowCode == kToPrintWindow || ...)"
				#endif	// defined multispec_win  
		
				switch (statsWindowMode)
					{
					case 0:
					case 1:
					case 2:
						OutlineProjectFields (outlineFieldType, 
														windowInfoHandle,
														windowCode, 
														&lcToWindowUnitsVariables);
						break;
						
					case 3:
						OutlineClassFields (outlineFieldType, 
													classNumber, 
													windowInfoHandle,
													windowCode,
													&lcToWindowUnitsVariables);
						break;
						
					case 4:
						OutlineField (classNumber, 
											fieldNumber,
											windowInfoHandle,
											windowCode, 
											&lcToWindowUnitsVariables);
						break;
						
					}	// end "switch (outlineFieldMode)" 
					
				ResetDestinationWindowParameters (windowCode,
																windowPtr);
																				
				#if defined multispec_win  
					gCDCPointer->SelectObject (pOldBrush);
					gCDCPointer->SelectObject (pOldPen);  
					gCDCPointer->SelectObject (pOldFont); 
					
					gCDCPointer->SetBkMode (OPAQUE);  
					
					gMFC_Rgn.DeleteObject ();              
				#endif	// defined multispec_win 
					
				}	// end "if (continueFlag)"
				
			#if defined multispec_mac
						// Reset the pen state back to the original settings.					
				
				SetPenState (&penState);
				
						// Reset text mode back to srcOr mode.										
						
				TextMode (srcOr);
			#endif	// defined multispec_mac  
				
			SetPort (savedPort);
				
			}	// end "if (outlineFieldMode >= kTrainingType && ...)"
		
		#if defined multispec_lin
			//displaydc.SelectObject (wxNullBitmap);
			//gCDCPointer = oldgCDCpt;
         gCDCPointer->DestroyClippingRegion ();
			if (overlayPenPtr != NULL)
				delete overlayPenPtr;      
		#endif	// multispec_lin

		}	// end "if (continueFlag)"
		
	MHSetState (windowInfoHandle, windowHandleStatus);

}	// end "OutlineFieldsControl" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void OutlineProjectFields
//
//	Software purpose:	The purpose of this routine is to all the training
//							and/or test fields in the project image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/10/89
//	Revised By:			Larry L. Biehl			Date: 06/22/1999	

void OutlineProjectFields (
				SInt16								outlineFieldType, 
				Handle								windowInfoHandle,
				SInt16								windowCode,
				LCToWindowUnitsVariablesPtr	lcToWindowUnitsVariablesPtr)

{
	UInt16								classIndex;
	
	
			// Continue only if input parameters are within proper ranges 			
			// and statistics are up to date.												
			
	if (gProjectInfoPtr->numberStatisticsClasses > 0)
		{
		for (classIndex=0;
				classIndex<gProjectInfoPtr->numberStatisticsClasses; 
					classIndex++)
			OutlineClassFields (outlineFieldType, 
										classIndex, 
										windowInfoHandle,
										windowCode, 
										lcToWindowUnitsVariablesPtr);
			
		}	// end "if (gProjectInfoPtr->numberStatisticsClasses > 0)" 
		
}	// end "OutlineProjectFields" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void OutlineClassFields
//
//	Software purpose:	The purpose of this routine is to outline the
//							training and/or test fields in the project image
//							window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/10/1989
//	Revised By:			Larry L. Biehl			Date: 06/22/1999

void OutlineClassFields (
				SInt16								outlineFieldType, 
				SInt16								classNumber, 
				Handle								windowInfoHandle,
				SInt16								windowCode,
				LCToWindowUnitsVariablesPtr	lcToWindowUnitsVariablesPtr)

{
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	SInt16								classStorage,
											fieldNumber;
	
	
			// Continue only if input parameters are within proper ranges.			
																						
	if (classNumber>=0 && classNumber<(SInt16)gProjectInfoPtr->numberStatisticsClasses)
		{
				// Initialize local variables.												
						
		classStorage = gProjectInfoPtr->storageClass[classNumber];
		classNamesPtr = gProjectInfoPtr->classNamesPtr;
		fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
				
				// Loop through the fields for the class									
	
		fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
	
		while (fieldNumber != -1)
			{
					// Outline the field if it is of the proper type (train/test).	
			
			if (outlineFieldType == kBothFieldTypes ||
									outlineFieldType == fieldIdentPtr[fieldNumber].fieldType)
				OutlineField (classNumber,
									fieldNumber,
									windowInfoHandle, 
									windowCode,
									lcToWindowUnitsVariablesPtr);
			
			fieldNumber = fieldIdentPtr[fieldNumber].nextField;
		
			}	// end "while (fieldNumber != -1)" 
			
		}	// end "if (classNumber>=0 && ...)" 

}	// end "OutlineClassFields" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void OutlineField
//
//	Software purpose:	The purpose of this routine is to outline the
//							specified field whether defined by rectangular
//							or polygonal coordinates.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			OutlineFieldControl.c 	in outlineFields.c
//							OutlineClassFields.c		in outlineFields.c
//
//	Coded By:			Larry L. Biehl			Date: 01/10/1989
//	Revised By:			Larry L. Biehl			Date: 01/05/2019

void OutlineField (
				SInt16								classNumber, 
				SInt16								fieldNumber, 
				Handle								windowInfoHandle,
				SInt16								windowCode,
				LCToWindowUnitsVariablesPtr	lcToWindowUnitsVariablesPtr)

{
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	HPFieldPointsPtr					fieldPointsPtr;
	
	LongRect*							LCRectPtr; 
												
	LongPoint							drawPoint,
											nextPoint,
											startPoint; 
	#if defined multispec_lin
		//LongPoint							lastPoint;
      LongPoint							windowPoint;
      wxPoint scrollOffset;
	#endif // defined multispec_lin
												
	LongRect								fieldRect,
											tFieldRect; 
											
	Rect									clipRect;
	
	SInt32								channel,
											displayImageLeft,
											displayImageRight,
											drawHPoint,
											drawVPoint,
											pointCount,
											pointIndex,
											savedChannelWindowOffset;
	
	SInt16								classStorage,
											textSize;
				
	Boolean								changeClipFlag,
											continueFlag;


	#if defined multispec_win
		USES_CONVERSION;
	#endif
	
			// Check input values.  Continue only if input parameters are within	
			// proper ranges, the field statistics are to be printed, and the		
			// field statistics are up to date.												
			
	continueFlag = TRUE;
	if (fieldNumber<0 || fieldNumber>=gProjectInfoPtr->numberStorageFields)
		continueFlag = FALSE;
		
	else if (classNumber<0 ||
								classNumber>=(SInt16)gProjectInfoPtr->numberStatisticsClasses)
		continueFlag = FALSE;
		
	else if (windowInfoHandle == NULL)
		continueFlag = FALSE;
		
	else if (gProjectInfoPtr->fieldIdentPtr[fieldNumber].pointType >= kClusterType)
		continueFlag = FALSE;
																						
	if (continueFlag)
		{
				// Initialize local variables.	
				
		fieldPointsPtr = gProjectInfoPtr->fieldPointsPtr;
		fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
		displayImageLeft = displayImageRight = gStartChannelWindow;
		
				// Get the right side of the displayed image. 'gChannelWindowWidth' allows 
				// for the 2 pixel spacing between side by side images.
				
		displayImageRight += gChannelWindowWidth;
		
		changeClipFlag = (gSideBySideChannels > 1);	
		
				// Determine if field rectangle is within clip portion of the window.
		
		pointIndex = fieldIdentPtr[fieldNumber].firstPolygonPoint;
		LCRectPtr = (LongRect*)&fieldPointsPtr[pointIndex]; 
		/*
			}	// end "if (...pointType == kRectangleType)"
			
		else	// fieldIdentPtr[fieldNumber].pointType != kRectangleType 	
			{
					// Polygon type window.
					
			GetFieldBoundary (gProjectInfoPtr, &areaDescription, fieldNumber); 
				
			if (areaDescription.rgnHandle != NULL)
				{
				DisposeRgn (areaDescription.rgnHandle);
				areaDescription.rgnHandle = NULL;
				LCRectPtr = (LongRect*)&areaDescription.lineStart;
				
				}	// end "if (areaDescription.rgnHandle != NULL)"
				
			else	// areaDescription.rgnHandle = NULL
				continueFlag = FALSE;          
			
			}	// end "else ...pointType != kRectangleType"	  
		*/		
		if (continueFlag)
			{	
			#if defined multispec_lin
						// Need to reset it again before drawing rectangles in linux, 
						// since the setting will be changed after drawing polygons.
						
				SetLCToWindowUnitVariables (windowInfoHandle,
													 kToImageWindow,
													 FALSE,
													 lcToWindowUnitsVariablesPtr);   
			#endif	// defined multispec_lin
			
			ConvertLCRectToWinRect (LCRectPtr, 
												&fieldRect, 
												lcToWindowUnitsVariablesPtr);
			tFieldRect = fieldRect;
			
					// Now get the clip portion of the image (1 channel wide).
					
			clipRect = gViewRect;

			#if defined multispec_mac
				clipRect.left = (SInt16)MAX (0, displayImageLeft);
				clipRect.right = (SInt16)MIN (gViewRect.right, displayImageRight); 
			#endif	// define multispec_mac 
							
			#if defined multispec_win
				clipRect.left = (int)MAX (0, displayImageLeft);
				clipRect.right = (int)MIN (gViewRect.right, displayImageRight);
				clipRect.top += (int)lcToWindowUnitsVariablesPtr->lineScrollOffset;
				clipRect.bottom += (int)lcToWindowUnitsVariablesPtr->lineScrollOffset;
				
				clipRect.left += (int)lcToWindowUnitsVariablesPtr->columnScrollOffset; 
				clipRect.right += (int)lcToWindowUnitsVariablesPtr->columnScrollOffset;
			#endif	// defined multispec_win 
			
			#if multispec_lin	              
				clipRect.left = (int)MAX (0, displayImageLeft);
				clipRect.right = (int)MIN (gViewRect.right, displayImageRight);

				clipRect.top += (int)lcToWindowUnitsVariablesPtr->lineScrollOffset;
				clipRect.bottom += (int)lcToWindowUnitsVariablesPtr->lineScrollOffset;
				clipRect.left += (int)lcToWindowUnitsVariablesPtr->columnScrollOffset; 
				clipRect.right += (int)lcToWindowUnitsVariablesPtr->columnScrollOffset;
				
			#endif	// multispec_lin 
			
					// If field is to the left of the displayed image, then move the
					// field and clip window to the next channel to the right if a 
					// multichannel display.
					
			if (tFieldRect.right < clipRect.left && changeClipFlag)
				{
				tFieldRect.left += gChannelWindowInterval;
				tFieldRect.right += gChannelWindowInterval;

				#if defined multispec_mac
					clipRect.left = (SInt16)MAX (0, 
													displayImageLeft+gChannelWindowInterval);
					clipRect.right = (SInt16)MIN (gViewRect.right, 
													displayImageRight+gChannelWindowInterval);
				#endif	// define multispec_mac 
							
				#if defined multispec_win || defined multispec_lin
					clipRect.left = (int)MAX (0, 
													displayImageLeft+gChannelWindowInterval);
					clipRect.right = (int)MIN (gViewRect.right, 
													displayImageRight+gChannelWindowInterval);

					clipRect.left += (int)lcToWindowUnitsVariablesPtr->columnScrollOffset; 
					clipRect.right += (int)lcToWindowUnitsVariablesPtr->columnScrollOffset;
				#endif	// defined multispec_win || defined multispec_lin
				
				}	// end "if (tFieldRect.right < clipRect.left && changeClipFlag)"					
			
			if (tFieldRect.top > clipRect.bottom ||
					tFieldRect.bottom < clipRect.top ||
						tFieldRect.left > clipRect.right ||
							tFieldRect.right < clipRect.left)
				continueFlag = FALSE;
				
			}	// end "if (continueFlag)"
			
		}	// end "if (continueFlag)"	
																	
	if (continueFlag)
		{
		classNamesPtr = gProjectInfoPtr->classNamesPtr;
		clipRect = gViewRect;
			
		if (fieldIdentPtr[fieldNumber].pointType == kRectangleType)
			{
					// Draw rectangle representing the field.
					// Set point to start drawing class name.			
         
			drawPoint.h = fieldRect.left;
			drawPoint.v = fieldRect.top;
			
			for (channel=gStartChannel; channel<gSideBySideChannels; channel++)
				{                                     
				if (displayImageRight >= gViewRect.left)
					{
					if (changeClipFlag)
						{  
						#if defined multispec_mac
							clipRect.left = (SInt16)MAX (0, displayImageLeft);
							clipRect.right = (SInt16)MIN (gViewRect.right, displayImageRight);
							ClipRect (&clipRect); 
						#endif	// defined multispec_mac 

  						#if defined multispec_win
							clipRect.left = (int)MAX (0, displayImageLeft);
							clipRect.right = (int)MIN (gViewRect.right, displayImageRight);                      
							if (windowCode != kToPrintWindow)
								ClipRect (&clipRect);
						#endif	// defined multispec_win || defined multispec_lin

						#if defined multispec_lin
							clipRect.left = (int)MAX (0, displayImageLeft);
							clipRect.right = (int)MIN (gViewRect.right, displayImageRight);
							gCDCPointer->DestroyClippingRegion ();
							if (windowCode != kToPrintWindow)
								ClipRect (&clipRect);
						#endif	// defined multispec_win || defined multispec_lin
						
						}	// end "if (changeClipFlag)" 
						
					#if defined multispec_mac
								// FrameRect would cause the system to crash when 			
								// running heap scramble under macbugs. 						
								// Larry Biehl 2-7-1991.
							
						//FrameRect (&fieldRect);
	
						MoveTo ((SInt16)fieldRect.left, (SInt16)fieldRect.top);
						LineTo ((SInt16)fieldRect.right, (SInt16)fieldRect.top);
						LineTo ((SInt16)fieldRect.right, (SInt16)fieldRect.bottom);
						LineTo ((SInt16)fieldRect.left, (SInt16)fieldRect.bottom);
						LineTo ((SInt16)fieldRect.left, (SInt16)fieldRect.top);
					#endif	// defined multispec_mac 
						
					#if defined multispec_win
						gCDCPointer->Rectangle ((int)fieldRect.left,
														(int)fieldRect.top,
														(int)fieldRect.right,
														(int)fieldRect.bottom);
					#endif	// defined multispec_win 
						
					#if defined multispec_lin  
						gCDCPointer->DrawRectangle (
											wxPoint	((int)fieldRect.left, (int)fieldRect.top),
											wxSize ((int)(fieldRect.right-fieldRect.left),
											(int)(fieldRect.bottom-fieldRect.top)));
					#endif	// defined multispec_lin 
					
					}	// end "if (displayImageRight >= gViewRect.left)" 
				                                        
				if (gSideBySideChannels > 1)
					{
					fieldRect.left += gChannelWindowInterval;
					fieldRect.right += gChannelWindowInterval;
					displayImageLeft += gChannelWindowInterval;
					displayImageRight += gChannelWindowInterval;
					
					}	// end "if (gSideBySideChannels > 1)" 
				                                     
				if (displayImageLeft > gViewRect.right)
					break;
				
				}	// end "for (channel=0; channel<..." 
			
			}	// end "if (gProjectInfoPtr.pointType == kRectangleType)" 
			
		else	// fieldIdentPtr[fieldNumber]->pointType != kRectangleType
			{
					// Draw border around polygon that describes the field.
					// Set point to start drawing class name.			
					
					// This code is here for testing purposes. The label point has already
					// been calculated and saved.
			/*
			LongPoint labelPoint;
			GetPolygonLabelPoint (&fieldIdentPtr[fieldNumber],
											fieldPointsPtr,
											&labelPoint);
			fieldIdentPtr[fieldNumber].labelPoint = labelPoint;
			*/
			ConvertLCToWinPoint (&fieldIdentPtr[fieldNumber].labelPoint,
											&drawPoint, 
											lcToWindowUnitsVariablesPtr);
											
					// Now draw the field outline
					// Note that the first two paired values represent the bounding box for
					// the polygon.
											
			pointIndex = fieldIdentPtr[fieldNumber].firstPolygonPoint + 2;
			ConvertLCToWinPoint ((LongPoint*)&fieldPointsPtr[pointIndex],
										&startPoint, 
										lcToWindowUnitsVariablesPtr);
											
			savedChannelWindowOffset = lcToWindowUnitsVariablesPtr->channelWindowOffset;
			
			#ifndef multispec_lin	
				for (channel=gStartChannel; channel<gSideBySideChannels; channel++)
					{				
					if (changeClipFlag)
						{						
						#if defined multispec_mac
							clipRect.left = (SInt16)MAX (0, displayImageLeft);
							clipRect.right = (SInt16)MIN (gViewRect.right, displayImageRight);
							ClipRect (&clipRect);
						#endif	// defined multispec_mac 
							
						#if defined multispec_win
							clipRect.left = (int)MAX (0, displayImageLeft);
							clipRect.right = (int)MIN (gViewRect.right, displayImageRight);
							if (windowCode != kToPrintWindow)
								ClipRect (&clipRect);
						#endif	// defined multispec_win 
						
						}	// end "if (changeClipFlag)" 
						
					pointCount = fieldIdentPtr[fieldNumber].numberOfPolygonPoints;
					pointIndex = fieldIdentPtr[fieldNumber].firstPolygonPoint + 3;
					
					#if defined multispec_mac
						MoveTo ((SInt16)startPoint.h, (SInt16)startPoint.v);
					#endif	// defined multispec_mac	
					
					#if defined multispec_win              
						gCDCPointer->MoveTo ((int)startPoint.h, (int)startPoint.v);
					#endif	// defined multispec_win

					#if defined multispec_lin              
								// wxWidgets does not have MoveTo; only DrawLine.
						lastPoint = startPoint;
					#endif	// defined multispec_lin	
					
					while (pointCount > 1)
						{
						ConvertLCToWinPoint ((LongPoint*)&fieldPointsPtr[pointIndex++], 
														&nextPoint, 
														lcToWindowUnitsVariablesPtr);
						/*	
						int numberChars = sprintf ((char*)gTextString3,
																	" column, line, nextPoint.h, nextPoint.v: %d, %d, %d, %d%s", 
																	fieldPointsPtr[pointIndex-1].col,
																	fieldPointsPtr[pointIndex-1].line,
																	nextPoint.h,
																	nextPoint.v,
																	gEndOfLine);
						ListString ((char*)gTextString3, numberChars, gOutputTextH);
						*/								
						#if defined multispec_mac
							LineTo ((SInt16)nextPoint.h, (SInt16)nextPoint.v);
						#endif	// defined multispec_mac
														
						#if defined multispec_win                   
							gCDCPointer->LineTo ((int)nextPoint.h, (int)nextPoint.v); 
						#endif	// defined multispec_win
						
						#if defined multispec_lin
							gCDCPointer->DrawLine ((int)lastPoint.h, (int)lastPoint.v, (int)nextPoint.h, (int)nextPoint.v);
							lastPoint = nextPoint;
						#endif	// defined multispec_lin
									
						pointCount--;
						
						}	// end "while (pointCount > 1)" 
								
					#if defined multispec_mac
						LineTo ((SInt16)startPoint.h, (SInt16)startPoint.v);
					#endif	// defined multispec_mac
								
					#if defined multispec_win  
						gCDCPointer->LineTo ((int)startPoint.h, (int)startPoint.v);
					#endif	// defined multispec_win
						
					#if defined multispec_lin
						gCDCPointer->DrawLine ((int)lastPoint.h,
														(int)lastPoint.v,
														(int)startPoint.h,
														(int)startPoint.v);
					#endif	// defined multispec_lin

					if (gSideBySideChannels > 1)
						{
						startPoint.h += gChannelWindowInterval;
						displayImageLeft += gChannelWindowInterval;
						displayImageRight += gChannelWindowInterval;
						//gChannelWindowOffset += gChannelWindowInterval;
						lcToWindowUnitsVariablesPtr->channelWindowOffset +=
																				gChannelWindowInterval;
						
						}	// end "if (gSideBySideChannels > 1)" 
					
					if (displayImageLeft > gViewRect.right)
						break;
					
					}	// end "for (channel=gStartChannel; channel<..." 
			#endif	// ifndef multispec_lin
			
			#if defined multispec_lin
						// Draw polygon
			
				CMImageView* imageViewCPtr = GetWindowPtr (windowInfoHandle);
			
				SetChannelWindowVariables (kToImageWindow,
													//gProjectSelectionWindow,
													imageViewCPtr,
													kNotCoreGraphics);
			
				SetLCToWindowUnitVariables (windowInfoHandle,
														 kWindowsUseOrigin,
														 FALSE,
														 lcToWindowUnitsVariablesPtr);
				
				pointCount = fieldIdentPtr[fieldNumber].numberOfPolygonPoints;  
				wxPoint* pointlist = new wxPoint[pointCount];
				//scrollOffset = gProjectSelectionWindow->m_Canvas->GetScrollPosition ();
				scrollOffset = imageViewCPtr->m_Canvas->GetScrollPosition ();
				for (int index = 0; index < pointCount; index++)
					{
					ConvertLCToWinPoint ((LongPoint*) & fieldPointsPtr[pointIndex++],
												&windowPoint,
												lcToWindowUnitsVariablesPtr);
					
					pointlist[index].x = windowPoint.h;
					pointlist[index].y = windowPoint.v;
					
					}	// end "for (int index = 0; ..."
			
				for (SInt32 channel=gStartChannel; channel<gSideBySideChannels; channel++)
					{
					if (displayImageLeft <= gViewRect.right &&
																displayImageRight >= gViewRect.left)
						{
						gCDCPointer->DrawPolygon (
							pointCount,
							pointlist,
							lcToWindowUnitsVariablesPtr->channelWindowOffset + scrollOffset.x,
							scrollOffset.y,
							wxODDEVEN_RULE);
						
						if (gSideBySideChannels > 1)
							{
							displayImageLeft += gChannelWindowInterval;
							displayImageRight += gChannelWindowInterval;                        
							lcToWindowUnitsVariablesPtr->channelWindowOffset +=
																					gChannelWindowInterval;
							
							} // end "if (gSideBySideChannels > 1)"
										
						if (displayImageLeft > gViewRect.right)
							break;
						
						}	// end "if (displayImageLeft <= gViewRect.right && ..."
					
					}  // end "for (SInt32 channel=gStartChannel; ..."
			
				delete pointlist;
			#endif	// defined multispec_lin
		
			//gChannelWindowOffset = savedChannelWindowOffset;
			lcToWindowUnitsVariablesPtr->channelWindowOffset = savedChannelWindowOffset;
			
			}	// end "else gProjectInfoPtr->pointType != kRectangleType" 
			
				// If requested, print class name in the field and indicate 		
				// whether it is a training or test field.								
			
		if (gProjectInfoPtr->labelFieldCode != 0)
			{
			displayImageLeft = displayImageRight = gStartChannelWindow;
			displayImageRight += gChannelWindowWidth;
				
					// Get the class storage number.											
							
			classStorage = gProjectInfoPtr->storageClass[classNumber];
			textSize = gProjectInfoPtr->labelSize;
			
			for (channel=gStartChannel; channel<gSideBySideChannels; channel++)
				{	
				if (displayImageRight >= gViewRect.left)
					{
					if (changeClipFlag)
						{					
						#if defined multispec_mac
							clipRect.left = (SInt16)MAX (0, displayImageLeft);
							clipRect.right = (SInt16)MIN (gViewRect.right, displayImageRight);
							ClipRect (&clipRect);
						#endif	// defined multispec_mac 

                  #if defined multispec_win
							clipRect.left = (int)MAX (0, displayImageLeft);
							clipRect.right = (int)MIN (gViewRect.right, displayImageRight);
							if (windowCode != kToPrintWindow)
								ClipRect (&clipRect);
						#endif	// defined multispec_win || defined multispec_lin

						#if defined multispec_lin
							clipRect.left = (int)MAX (0, displayImageLeft);
							clipRect.right = (int)MIN (gViewRect.right, displayImageRight);
                     gCDCPointer->DestroyClippingRegion ();
							if (windowCode != kToPrintWindow)
								ClipRect (&clipRect);
						#endif	// defined multispec_win || defined multispec_lin

						}	// end "if (changeClipFlag)"
						
					#if defined multispec_mac
						drawHPoint = drawPoint.h + 1;
						drawVPoint = drawPoint.v + textSize;
						
						if (gProjectInfoPtr->labelFieldCode & 0x0001)
							{
							MoveTo ((SInt16)drawHPoint, (SInt16)drawVPoint);
							DrawString (classNamesPtr[classStorage].name);
							drawVPoint += textSize;
							
							}	// end "if (gProjectInfoPtr->labelFieldCode & 0x0001)"
							
						MoveTo ((SInt16)drawHPoint, (SInt16)drawVPoint);
						
						if (gProjectInfoPtr->labelFieldCode & 0x0004)
							{
							if (fieldIdentPtr[fieldNumber].fieldType == kTrainingType)
								DrawString ("\pn-");
																						
							else	// fieldIdentPtr[fieldNumber]... != kTrainingType)	
								DrawString ("\pt-");
							
							}	// end "if (gProjectInfoPtr->labelFieldCode & 0x0004)"
						
						if (gProjectInfoPtr->labelFieldCode & 0x0002)
							DrawString (fieldIdentPtr[fieldNumber].name);
					#endif	// defined multispec_mac 
						
					#if defined multispec_win 
						gCDCPointer->SetTextAlign (TA_UPDATECP);
													
						TEXTMETRIC	textMetric;
						gCDCPointer->GetTextMetrics (&textMetric);
						
						drawHPoint = drawPoint.h + 1;
						drawVPoint = drawPoint.v + 1;
						
						if (gProjectInfoPtr->labelFieldCode & 0x0001)
							{
							gCDCPointer->MoveTo ((int)drawHPoint, (int)drawVPoint); 
						
							gCDCPointer->TextOut (
										0,
										0,
										(LPCTSTR)A2T((char*)&classNamesPtr[classStorage].name[1]), 
										(int)classNamesPtr[classStorage].name[0]);
							
							drawVPoint += textMetric.tmHeight;
							
							}	// end "if (gProjectInfoPtr->labelFieldCode & 0x0001)"
							
						gCDCPointer->MoveTo ((int)drawHPoint, (int)drawVPoint);
						
						if (gProjectInfoPtr->labelFieldCode & 0x0004)
							{ 
							if (fieldIdentPtr[fieldNumber].fieldType == kTrainingType)
								gCDCPointer->TextOut (0, 0, (LPCTSTR)_T("n-"), 2);
																					
							else	// fieldIdentPtr[fieldNumber]... != kTrainingType)	
								gCDCPointer->TextOut (0, 0, (LPCTSTR)_T("t-"), 2);
								
							}	// end "if (gProjectInfoPtr->labelFieldCode & 0x0004)"
						
						if (gProjectInfoPtr->labelFieldCode & 0x0002)
							gCDCPointer->TextOut (
										0,
										0,
										(LPCTSTR)A2T((char*)&fieldIdentPtr[fieldNumber].name[1]), 
										(int)fieldIdentPtr[fieldNumber].name[0]);
					#endif	// defined multispec_win
						
					#if defined multispec_lin 
						drawHPoint = drawPoint.h + 1 ;//+ scrollOffset.x;
						drawVPoint = drawPoint.v + 1 ;//+ scrollOffset.y;
						
						if (gProjectInfoPtr->labelFieldCode & 0x0001)
							{
							/*
							gCDCPointer->DrawLine ((int)drawHPoint,
																(int)drawVPoint,
																(int)drawHPoint,
																(int)drawVPoint);
							*/
							gCDCPointer->DrawText ((char*)&classNamesPtr[classStorage].name[1], 
															wxPoint (drawHPoint, drawVPoint));
													
							drawVPoint += gCDCPointer->GetCharHeight ();
							
							}	// end "if (gProjectInfoPtr->labelFieldCode & 0x0001)"
						/*
						gCDCPointer->DrawLine ((int)drawHPoint,
														(int)drawVPoint, 
														(int)drawHPoint, 
														(int)drawVPoint);
						*/
						if (gProjectInfoPtr->labelFieldCode & 0x0004)
							{ 
							if (fieldIdentPtr[fieldNumber].fieldType == kTrainingType)
								gCDCPointer->DrawText (wxT("n-"), 
																wxPoint (drawHPoint, drawVPoint));
																					
							else	// fieldIdentPtr[fieldNumber]... != kTrainingType)	
								gCDCPointer->DrawText (wxT("t-"), 
																wxPoint (drawHPoint, drawVPoint));
								
							}	// end "if (gProjectInfoPtr->labelFieldCode & 0x0004)"
						
						if (gProjectInfoPtr->labelFieldCode & 0x0002)
							gCDCPointer->DrawText ((char*)&fieldIdentPtr[fieldNumber].name[1], 
															wxPoint (drawHPoint, drawVPoint));
					#endif	// defined multispec_lin
					
					}	// end "if (displayImageRight >= gViewRect.left)" 
				
				if (gSideBySideChannels > 1)
					{
					drawPoint.h += gChannelWindowInterval;
					displayImageLeft += gChannelWindowInterval;
					displayImageRight += gChannelWindowInterval;
					
					}	// end "if (gSideBySideChannels > 1)" 
				
				if (displayImageLeft > gViewRect.right)
					break;
					
				}	// end "for (channel=0; channel<..." 
				
			}	// end "if (gProjectInfoPtr->printLabelFlag)" 
			
		#if defined multispec_lin
			gCDCPointer->DestroyClippingRegion ();
		#endif
		
		if (changeClipFlag)
			ClipRect (&gViewRect);
		
		}	// end "if (continueFlag)" 
		
}	// end "OutlineField" 

