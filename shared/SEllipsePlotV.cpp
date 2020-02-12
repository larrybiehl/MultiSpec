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
//	File:						SEllipsePlotV.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/21/2017
//	
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains routine(s) that handle drawing 
//								ellipses.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_wx
#endif

//#include "SGraphic.h"


// ========================================================================
//		(C) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//		EllipsePlotV:	This routine accepts two vectors containing the x and
//					y coordinats of each data point.  Scaling of the data
//					to fit the screen is performed automatically.
//
//
//		LinePlotV (graph, x, y, size, error)
//
//		where:	graph		Pointer to graph record (not graph port)
//				x			Pointer to x vector
//				y			Pointer to y vector
//				error		Returned error code (see below)
//
//		Errors returned:	NU_NO_ERROR, NU_NOTSAMESIZEG, or NU_MUSTBEPOS
//			
//			
//		revised by Larry L. Biehl  12/09/1991
//		revised by Larry L. Biehl  05/27/1994
//		revised by Larry L. Biehl  04/29/2003
//
//========================================================================

void PlotSpecifiedEllipse (
				GraphPtr					graph,
				double					*classStatisticsPtr,
				SInt32					featureA,
				SInt32					featureB,
				double					x_scale,
				SInt32					x_offset,
				double					y_scale,
				SInt32					y_offset);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void EllipsePlotV ()
//
//	Software purpose:	This routine plots the specified plane(s) of
//							an ellipsoid.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	True, if memory is okay
//							False, if there is not enough memory.
// 
// Called By:			ShowGraphWindowSelection in SSelectionGraph.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/12/1994
//	Revised By:			Larry L. Biehl			Date: 06/02/2016			

void	EllipsePlotV (
				GraphPtr								graph, 
				SInt32*								error)
					
{
	char									*vectorDisplayPtr,
											*vectorSymbolPtr;
									
	double*								classStatisticsPtr;
	Rect*									clientRectPtr;
	SInt32*								vectorPaletteColorPtr;
	
	double								angle,
											angleIncrement,
											dim,
											distance,
											endAngle,
											fac,
											xMean, 
											xVariance,
											xyVariance,
											x_scale, 
											x_val,
											yMean,
											yVariance,
											y_scale, 
											y_val;
	
	SInt32								classStatsIncrement,
											screen_height,
											screen_width, 
											x_offset,
											y_offset;
	
	SInt16								endFeature,
											index1,
											index2,
											lines,
											numberVectors,
											startFeature,
											x_point, 
											y_point;
   
	#ifndef multispec_wx	
		CDC*									pDC = graph->pDC;
	#endif

	#ifdef multispec_wx
		SInt16                        x_point1, y_point1;
		char									symbolString[2];
		wxString								wx_sym;
		wxDC*									pDC = graph->pDC;
		
		symbolString[0] = 0;
		symbolString[1] = 0;
	#endif
	
	numberVectors = graph->numberVectors;

	*error = NU_NO_ERROR;
		
	SetGraphClip (graph, error);
	
	clientRectPtr = &graph->clientRect;

	screen_width = (clientRectPtr->right - clientRectPtr->left) -
													graph->rightInset - graph->leftInset;
	screen_height = (clientRectPtr->bottom - clientRectPtr->top) -
													graph->bottomInset - graph->topInset;
													
	x_scale = screen_width / (graph->xScaleMax - graph->xScaleMin);
	x_offset =
			(SInt32)(graph->xScaleMin * x_scale - graph->leftInset - clientRectPtr->left);
		
	y_scale = screen_height / (graph->yScaleMax - graph->yScaleMin);
	y_offset = (SInt32)(graph->yScaleMin * y_scale + screen_height + 
													graph->topInset + clientRectPtr->top);
	
	dim = graph->classThresholdValue;
	
	classStatisticsPtr = (double*)GetHandlePointer (
														graph->classStatisticsHandle,
														kLock);
	classStatsIncrement = 5 + 3 * graph->numberStatisticsChannels;
	
	vectorPaletteColorPtr = (SInt32*)GetHandlePointer (
														graph->vectorPaletteColorHandle,
														kLock);
	
	vectorDisplayPtr = (char*)GetHandlePointer (
														graph->vectorDisplayHandle,
														kLock);
	
	vectorSymbolPtr = (char*)GetHandlePointer (
														graph->vectorSymbolHandle,
														kLock);
	
	startFeature = 0;
	endFeature = 0;
	endFeature = MIN (endFeature, graph->numberStatisticsChannels-1);
	
	angleIncrement = 2 * kDegreesToRadians;
	endAngle = (double)kTWO_PI - 3 * angleIncrement;
		
	for (lines=1; lines<numberVectors; lines++)
		{
		if (vectorDisplayPtr[lines] > 0)
			{
			MForeColor (pDC, vectorPaletteColorPtr[lines]);
			
					// Draw ellipse in x-y plane of ellipsoid.
			
			xMean = classStatisticsPtr[0];
			yMean = classStatisticsPtr[1];
			
			xVariance = classStatisticsPtr[2];
			xyVariance = classStatisticsPtr[3];
			yVariance = classStatisticsPtr[4];
			
			angle = 0.;
			
			while (angle < endAngle)
				{
				x_val = cos (angle);
				y_val = sin (angle);
				
				distance = x_val*x_val*xVariance + 
									x_val*y_val*xyVariance + y_val*y_val*yVariance;
																		
				distance = sqrt (fabs (distance));
				
				fac = dim/distance;
				
				x_val *= fac;
				y_val *= fac;
				
				x_val += xMean;
				y_val += yMean;
					
				x_point = (SInt16)(floor (x_val * x_scale) - x_offset);
				y_point = (SInt16)(y_offset - floor (y_val * y_scale));
			

				if (angle == 0)
					{
					#if defined multispec_mac
						MoveTo (x_point, y_point);
					#endif 
					
					#if defined multispec_win
						pDC->MoveTo (x_point, y_point);
					#endif
					
					#if defined multispec_wx
						x_point1 = x_point;
						y_point1 = y_point;   
					#endif
					}
				else	// angle != 0 
					{
					#if defined multispec_mac
						LineTo (x_point, y_point);
					#endif

					#if defined multispec_win
						pDC->LineTo (x_point, y_point);
					#endif

					#if defined multispec_wx
						pDC->DrawLine (x_point1, y_point1, x_point, y_point);
					#endif            
					}
					
				angle += angleIncrement;
            
				#if defined multispec_wx      
					x_point1 = x_point;
					y_point1 = y_point;
				#endif				
				}	// end "while (angle <= endAngle)" 
				
			#if defined multispec_mac				
				DrawChar (vectorSymbolPtr[lines]);
			#endif
			
			#if defined multispec_win
				#ifdef UNICODE
					WCHAR tc[1] = { 0 };
					tc[0] = vectorSymbolPtr[lines];
					pDC->TextOut (x_point, y_point, tc, 1);
				#else
					pDC->TextOut (x_point, y_point, (LPCTSTR)&vectorSymbolPtr[lines], 1);
				#endif
			#endif	// defined multispec_win
		
			#if defined multispec_wx
				wxColour color;
				color.Set (vectorPaletteColorPtr[lines]);
				pDC->SetTextForeground (color);
				
				symbolString[0] = vectorSymbolPtr[lines];
				wx_sym = wxString::FromAscii (symbolString);
				pDC->DrawText (wx_sym, x_point, y_point);
				#endif
			
					// Draw ellipses in principal planes of ellipsoid				
					// and transform back to x-y plane.									
			
			for (index1=startFeature; index1<endFeature; index1++)
				{
				for (index2=index1+1; index2<=endFeature; index2++)		
					PlotSpecifiedEllipse (graph, classStatisticsPtr, 
						index1, index2, x_scale, x_offset, y_scale, y_offset);
											
				}	// end "for (index1=startFeature; index1<..."
			
			}	// end "if (vectorDisplayPtr [lines] > 0)" 
			
		classStatisticsPtr += classStatsIncrement;
			
		}	// end "for (lines=1; lines<numberVectors; lines++)" 
		
	MForeColor (pDC, blackColor);
	
	CheckAndUnlockHandle (graph->classStatisticsHandle);
	CheckAndUnlockHandle (graph->vectorPaletteColorHandle);
	CheckAndUnlockHandle (graph->vectorDisplayHandle);
	CheckAndUnlockHandle (graph->vectorSymbolHandle);
		
	SetWindowClip (graph, error);
	
}	// end "EllipsePlotV" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void PlotSpecifiedEllipse ()
//
//	Software purpose:	This routine plots the specified principal plane
//							of an ellipsoid.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None.
//												
// Called By:			EllipsePlotV in SEllipsePlotV.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/12/1994
//	Revised By:			Larry L. Biehl			Date: 05/22/2017			

void	PlotSpecifiedEllipse (
				GraphPtr								graph,
				double								*classStatisticsPtr,
				SInt32								featureA,
				SInt32								featureB,
				double								x_scale,
				SInt32								x_offset,
				double								y_scale,
				SInt32								y_offset)
					
{
	double								angle,
											x_val, 
											xT_val, 
											y_val,
											yT_val;
	
	double								angleIncrement,
											dim,
											distance,
											endAngle,
											fac,
											xMean,
											xVariance,
											x11Transform,
											x12Transform,
											x21Transform,
											x22Transform,
											yMean,
											yVariance;
	
	SInt16								x_point, 
											y_point;

	#if defined multispec_win
		CDC*								pDC = graph->pDC;
	#endif
	
	#ifdef multispec_wx
		SInt16								x_point1, 
												y_point1;
		wxDC* pDC = graph->pDC;
	#endif
		
			// Draw ellipse in x-y plane of ellipsoid.								
			
	dim = graph->classThresholdValue;
			
	xMean = classStatisticsPtr[0];
	yMean = classStatisticsPtr[1];
			
	xVariance = classStatisticsPtr[5 + featureA];
	yVariance = classStatisticsPtr[5 + featureB];
	
	x11Transform = classStatisticsPtr[
									5+featureA+graph->numberStatisticsChannels];
	x12Transform = classStatisticsPtr[
									5+featureB+graph->numberStatisticsChannels];
	x21Transform = classStatisticsPtr[
									5+featureA+2*graph->numberStatisticsChannels];
	x22Transform = classStatisticsPtr[
									5+featureB+2*graph->numberStatisticsChannels];
			
	angleIncrement = 2 * kDegreesToRadians;
	endAngle = kTWO_PI + angleIncrement;
	angle = 0.;
			
	while (angle < endAngle)
		{
		xT_val = cos (angle);
		yT_val = sin (angle);
		
		distance = xT_val*xT_val*xVariance + yT_val*yT_val*yVariance;
		distance = sqrt (fabs (distance));
		
		fac = dim/distance;
		
		xT_val *= fac;
		yT_val *= fac;
		
		x_val =  x11Transform * xT_val + x12Transform * yT_val;
		y_val =  x21Transform * xT_val + x22Transform * yT_val;
		
		x_val += xMean;
		y_val += yMean;
			
		x_point = (SInt16)(floor (x_val * x_scale) - x_offset);
		y_point = (SInt16)(y_offset - floor (y_val * y_scale));
	
		if (angle == 0)
			{
			#if defined multispec_mac
				MoveTo (x_point, y_point);
			#endif

			#if defined multispec_win
				pDC->MoveTo (x_point, y_point);
			#endif

			#if defined multispec_wx
            x_point1 = x_point;
            y_point1 = y_point;   
			#endif
			}
			
		else	// angle != 0
			{		 
			#if defined multispec_mac
				LineTo (x_point, y_point);
			#endif

			#if defined multispec_win
				pDC->LineTo (x_point, y_point);
			#endif

			#if defined multispec_wx
				pDC->DrawLine (x_point1, y_point1, x_point, y_point);
			#endif
			}	// end "else angle != 0"
      
		angle += angleIncrement;
      
		#if defined multispec_wx
			x_point1 = x_point;
			y_point1 = y_point;   
		#endif
		
		}	// end "while (angle <= endAngle)" 
	
}	// end "PlotSpecifiedEllipse" 
