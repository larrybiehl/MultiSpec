// ========================================================================
//		(C) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//		AllocateV:	Allocates space for a vector.  The returned value
//						is a structure containing the size of the vector
//						and a pointer to the data.
//
//		AllocateV( v0, size, error )
//
//		where:	v0		Pointer to returned vector
//				size	Number of elements to allocate space for
//				error	Returned error code
//
//		Errors returned:
//
//			NO_ERROR	Successful completion
//
//						Any errors returned by NewPtrClear or DeallocateV
//			
//			
//		revised by Larry L. Biehl  08/05/1992
//		revised by Larry L. Biehl  02/23/2016
//
// ========================================================================

#include "SMulSpec.h"
	
#if defined multispec_mac
	#include	"SGrafVew.h" 
	 #endif	// defined multispec_mac    
                            
#if defined multispec_win
	#include	"SGrafVew.h" 
	#include "CImagVew.h"
#endif	// defined multispec_win   
                            
#if defined multispec_lin
	#include "LGraphView.h"
#endif	// defined multispec_lin

#include	"SExtGlob.h"

extern void		FindMaxBinV(
						double*								maxPtr,
						vector*								xVector,
						vector*								yVector,
						HSInt32Ptr							vectorLengthsPtr,
						HSInt32Ptr							vectorDataPtr,
						char*									vectorDisplayPtr,
						double*								histogramBinWidthPtr,
						UInt32								numberVectors,
						UInt32								set,
						GraphPtr								graph);
						
						
static void	LabelLinAxis_TrimString( 
						char									*str);
	
	

void AllocateV ( 
				vector						*v0, 
				SInt32						size, 
				SInt32						*error )
	
{
	*error = 0;

	if ( v0->size >= size )
																						return;
	
	v0->numberPoints = 0;
	if (v0->baseHandle != NULL)
		DeallocateV( v0, error );
		
	v0->basePtr = NULL;
	v0->size = size;
	v0->baseHandle = MNewHandle ((SInt32)size*sizeof(GRAPHDATA));
	if ( !v0->baseHandle ) 
		{
		v0->size = 0;
		*error = NU_ALLOC_ERR;
		
		}		// end "if ( !v0->base )"
		
}		// End AllocateV



//------------------------------------------------------------------------------------
//								 Copyright (1988-2006)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SetGraphVectorSize
//
//	Software purpose:	The purpose of this routine is to check the size of the input
//							graph vector handle against the size that is need. More 
//							memory is obtained for the handle if needed.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/28/1999
//	Revised By:			Larry L. Biehl			Date: 01/28/1999	

void SetGraphVectorSize( 
				vector*								v0, 
				SInt32								size, 
				SInt32*								error)
				
{	
	SInt32								numberBytesNeeded;
	
	Boolean								changedFlag,
											continueFlag;
											
	
	*error = 0;

	if ( v0->size >= size )
																						return;
	
	numberBytesNeeded = (SInt32)size * sizeof(GRAPHDATA);
	
	v0->basePtr = (GRAPHDATA*)CheckHandleSize ( &v0->baseHandle, 
																&continueFlag, 
																&changedFlag, 
																numberBytesNeeded);
												
	if (continueFlag && changedFlag)
		v0->size = size;
		
	else if (!continueFlag)
		*error = NU_ALLOC_ERR;
		
}		// End "SetGraphVectorSize"



// ========================================================================
//		(C) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//		DeallocateV:	Vector space deallocation.  This fuction
//						deallocates space for storage of the vector
//						elements.
//
//		DeallocateV(v1, error)
//
//		where:	v1		Vector to deallocate
//				error	Returned error code
//
//
//		Error codes:
//
//			NO_ERROR	No error - successful completion
//
//						Any errors returned by 'DisposePtr'
//						
//		Revised by Larry L. Biehl		08/05/1992
//		Revised by Larry L. Biehl		08/04/1995
//
// ========================================================================

void DeallocateV(
				vector		*vectorPtr, 
				SInt32		*errorPtr)
					
{		// DeallocateV

	*errorPtr	= NU_NO_ERROR;
	if ( vectorPtr->baseHandle != NULL)
		vectorPtr->baseHandle = UnlockAndDispose( vectorPtr->baseHandle );
		
	InitV( vectorPtr, errorPtr );
	
}		// End DeallocateV



//------------------------------------------------------------------------------------
//								 Copyright (1988-2006)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DensityFunctionPlotV
//
//	Software purpose:	The purpose of this routine is to plot the requested density
//							functions.
//
//	Parameters in:		graph		Pointer to graph record ( not graph port )
//							x			Pointer to x vector
//							y			Pointer to y vector
//
//	Parameters out:	error		Returned error code ( see below )
//											NU_NO_ERROR, NU_NOTSAMESIZEG, or NU_MUSTBEPOS
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/03/1999
//	Revised By:			Larry L. Biehl			Date: 10/17/2015

void	DensityFunctionPlotV (                             
				GraphPtr								graphPtr,
				SInt32*								errorPtr,
				SInt16								drawGraphCode)
					
{								
	double								constant1,
											constant2,
											exponent,
											mean,
											standardDeviation,
											x_increment,
											x_max,
											x_offset,
											x_scale, 
											x_val, 
											y_offset,
											y_scale, 
											y_val;
											
	char*									vectorDisplayPtr;
	HDoublePtr							classStatisticsPtr;
	HSInt32Ptr							vectorPaletteColorPtr;
	Rect*									clientRectPtr;
								
	UInt32								classStatsIndex,
											classStatsIndexIncrement,
											line,
											numberVectors,
											set;
	
	SInt16								screen_width, 
											screen_height,
											x_point, 
											y_point;
	
	#ifndef multispec_lin	
		CDC* pDC = graphPtr->pDC;
	#endif
	
	#ifdef multispec_lin
		wxDC* pDC = graphPtr->pDC;
	#endif   
	
	SetGraphClip( graphPtr, errorPtr );  
	
	vectorPaletteColorPtr = (SInt32*)GetHandlePointer (
											graphPtr->vectorPaletteColorHandle,
											kLock,
											kNoMoveHi);
	
	vectorDisplayPtr = (char*)GetHandlePointer (
											graphPtr->vectorDisplayHandle,
											kLock,
											kNoMoveHi);
	
	classStatisticsPtr = (HDoublePtr)GetHandlePointer (
											graphPtr->classStatisticsHandle,
											kLock,
											kNoMoveHi);
						
	numberVectors = graphPtr->numberVectors;
	set = graphPtr->set;                       
	
	clientRectPtr = &graphPtr->clientRect;
	
	screen_width = (clientRectPtr->right - clientRectPtr->left) -
													graphPtr->rightInset - graphPtr->leftInset;
	screen_height = (clientRectPtr->bottom - clientRectPtr->top) -
													graphPtr->bottomInset - graphPtr->topInset;
													
	x_scale = screen_width / (graphPtr->xScaleMax - graphPtr->xScaleMin);
	x_offset = graphPtr->xScaleMin * x_scale - 
												graphPtr->leftInset - clientRectPtr->left;
	
	classStatsIndexIncrement = graphPtr->classStatsIncrement;
	
			// Find maximum y value. It will occur with the minimum standard
			// deviation.
			
	y_val = DBL_MAX;
	classStatsIndex = (set-1) * classStatsIndexIncrement * numberVectors + 1;
	for (line=0; line<numberVectors; line++)
		{
		if (vectorDisplayPtr [line] > 0)
			{
			y_val = MIN (y_val, classStatisticsPtr[classStatsIndex]);
		   
		   }		// end "if (vectorDisplayPtr [line] > 0)"
		   
		classStatsIndex += classStatsIndexIncrement;
		   
		}		// end "for (line=0; line<numberVectors; line++)"
		
	y_val = 1./(y_val * kSQRT2PI);
		
	y_scale = screen_height / y_val;
	y_offset = screen_height + graphPtr->topInset + clientRectPtr->top;
	
	classStatsIndex = (set-1) * classStatsIndexIncrement * numberVectors;
  
	for (line=0; line<numberVectors; line++)
		{
		if (vectorDisplayPtr [line] > 0)
			{ 
			mean = classStatisticsPtr[classStatsIndex];
			standardDeviation = classStatisticsPtr[classStatsIndex+1];
	                 
		   constant1 = 1./(standardDeviation * kSQRT2PI);
		   constant2 = 1./(2. * standardDeviation * standardDeviation);
		                
			MForeColor( pDC, vectorPaletteColorPtr[line] );
			
					// Get initial point to start at.
					
//			x_increment = (double)((SInt32)(4*standardDeviation + .5));
			x_increment = 4*standardDeviation;
//			x_increment = MAX(x_increment, 1);

			if (x_increment == 0)
				x_increment = 4 * graphPtr->histogramBinWidth;
					
			x_val = mean - x_increment;
			x_max = mean + x_increment;
					
			x_val = MAX(graphPtr->xScaleMin, x_val);
			x_max = MIN(graphPtr->xScaleMax, x_max);
			
					// Set the increment to allow about 40 points to define the curve.
					
			x_increment = (x_max - x_val)/40;
			
			if (x_increment == 0)
				x_increment = graphPtr->histogramBinWidth;
			
			exponent = (x_val-mean);
			exponent *= exponent;
			exponent *= constant2;
			y_val = constant1 * exp(-exponent);
				
			x_point = (SInt16)(floor(x_val * x_scale) - x_offset);
			y_point = (SInt16)(y_offset - floor(y_val * y_scale));
					
			#if defined multispec_mac
				MoveTo( x_point, y_point );
			#endif	// defined multispec_mac 
			 
			#if defined multispec_win
				pDC->MoveTo( x_point, y_point );
			#endif	// defined multispec_win
			
			x_val += x_increment;
     
         #if defined multispec_lin
         int b_size;
         b_size = (int)((x_max- x_val)/x_increment)+2;
         wxPoint splines[b_size];
         int i = 0;
         splines[i].x = x_point;
         splines[i].y = y_point;
         #endif	// defined multispec_lin
			while (x_val <= x_max) 
				{
				exponent = (x_val-mean);
				exponent *= exponent;
				exponent *= constant2;
				y_val = constant1 * exp(-exponent);
				
				x_point = (SInt16)(floor(x_val * x_scale) - x_offset);
				y_point = (SInt16)(y_offset - floor(y_val * y_scale));
				
				#if defined multispec_mac
					LineTo (x_point, y_point);
				#endif	// defined multispec_mac 
				 
				#if defined multispec_win
					pDC->LineTo (x_point, y_point);
				#endif	// defined multispec_win
			
				x_val += x_increment;
            #if defined multispec_lin
				i++;
            splines[i].x = x_point;
            splines[i].y = y_point;
            #endif	// defined multispec_lin
				}		// end "while (x_val<=x_max)"
         
         
         #if defined multispec_lin
            pDC->DrawSpline(i, splines);
            i = 0;
         #endif // defined multispec_lin
         
			}		// end "if (vectorDisplayPtr [lines] > 0)"
		   
		classStatsIndex += classStatsIndexIncrement;
			
		}		// end "for (line=0; line<numberVectors; line++)"
	
	MForeColor (pDC, blackColor);
	
	CheckAndUnlockHandle (graphPtr->vectorPaletteColorHandle);
	CheckAndUnlockHandle (graphPtr->vectorDisplayHandle);
	CheckAndUnlockHandle (graphPtr->classStatisticsHandle);
		
	SetWindowClip (graphPtr, errorPtr);
	
}		// End "DensityFunctionPlotV"



// ========================================================================
//		(C) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//		DrawAxis:	This routine draws axis line for the specified graph.
//
//		DrawAxis( graph, error )
//
//		where:	graph		Pointer to graph record ( not graph port )
//					error		Returned error code ( see below )
//
//		Errors returned:	NU_NO_ERROR
//		revised by Larry L. Biehl  12/31/2005
//
// ========================================================================

void	DrawAxis(                           
				GraphPtr			graphPtr, 
				SInt32*			error )
								
{		// DrawAxis 

	CDC* 					pDC;
	
	Rect					*clientRectPtr,
							tempRect;
	
	double				x_offset,
							x_scale,
							y_offset, 
							y_scale;
	
	SInt32				screen_height,
							screen_width;
	
	SInt16				bevelInset,
							x_point, 
							y_point;
				

	*error = NU_NO_ERROR;
	pDC = graphPtr->pDC;
			
			// Force the window to be gray.
			
	#if defined multispec_mac 
		RGBColor				grayColor,
								savedForeGroundColor;
		
		GetForeColor (&savedForeGroundColor);
		
		grayColor.red = 0xEEEE;
		grayColor.green = 0xEEEE;
		grayColor.blue = 0xEEEE;
		RGBForeColor ( &grayColor );
		PaintRect (&graphPtr->clientRect);
	#endif	// defined multispec_mac
	
	#if defined multispec_win
		graphPtr->pDC->FillRect( (tagRECT*)&graphPtr->clientRect, 
											(CBrush*)graphPtr->backgroundPatternPtr);
	#endif	// defined multispec_win

   #if defined multispec_lin
      wxColour grayColor;
      //grayColor.Set(wxT("#d4d4d4"));
      grayColor.Set(wxT("#ffffff"));
      graphPtr->pDC->SetBrush(wxBrush(grayColor));
		bevelInset = (SInt16)(3 * graphPtr->textScaling);
		wxCoord left = graphPtr->clientRect.left + graphPtr->leftInset - bevelInset;
		wxCoord top = graphPtr->clientRect.top + graphPtr->topInset - bevelInset;
		wxCoord width = graphPtr->clientRect.right - left - graphPtr->rightInset + 2*bevelInset;
		wxCoord height = graphPtr->clientRect.bottom - top - graphPtr->bottomInset + 2*bevelInset;
      graphPtr->pDC->DrawRectangle (left, top, width, height);
   if (graphPtr->drawGraphCode <= 0)  return;
   #endif
	
	clientRectPtr = &graphPtr->clientRect;
	
	screen_width = ( clientRectPtr->right - clientRectPtr->left ) -
										graphPtr->rightInset - graphPtr->leftInset;
	screen_height = ( clientRectPtr->bottom - clientRectPtr->top ) -
										graphPtr->bottomInset - graphPtr->topInset;

	bevelInset = (SInt16)(3 * graphPtr->textScaling);									
	tempRect.left = clientRectPtr->left + graphPtr->leftInset - bevelInset; 
	tempRect.top = clientRectPtr->top + graphPtr->topInset - bevelInset;
	
	bevelInset = (SInt16)(4 * graphPtr->textScaling);
	tempRect.right = clientRectPtr->right - graphPtr->rightInset + bevelInset; 
	tempRect.bottom = clientRectPtr->bottom - graphPtr->bottomInset + bevelInset;   
		
	#if defined multispec_mac
		grayColor.red = 0xFFFF;
		grayColor.green = 0xFFFF;
		grayColor.blue = 0xFFFF;
		
		RGBForeColor ( &grayColor );
		PaintRect (&tempRect);
		
		ForeColor ( blackColor );
		::FrameRect (&tempRect);
	
				// Draw the shadow affects around the window box.
		
		InsetRect( &tempRect, -1, -1);
		tempRect.right--;
		tempRect.bottom--;
		
		grayColor.red = 0xAAAA;
		grayColor.green = 0xAAAA;
		grayColor.blue = 0xAAAA;
		
		RGBForeColor ( &grayColor );	
		MoveTo( tempRect.left, tempRect.bottom );
		LineTo( tempRect.left, tempRect.top ); 
		LineTo( tempRect.right, tempRect.top );  
		                        
		grayColor.red = 0xFFFF;
		grayColor.green = 0xFFFF;
		grayColor.blue = 0xFFFF;
		
		RGBForeColor ( &grayColor );	
		MoveTo( tempRect.right, tempRect.top+1 );
		LineTo( tempRect.right, tempRect.bottom ); 
		LineTo( tempRect.left+1, tempRect.bottom ); 
		
		RGBForeColor ( &savedForeGroundColor );
	#endif	// defined multispec_mac 
	 
	#if defined multispec_win
		pDC->Rectangle( tempRect.left,  
						tempRect.top, 
						tempRect.right, 
						tempRect.bottom);
	
				// Draw the shadow affects around the window box.
		
		InsetRect( (tagRECT*)&tempRect, -1, -1);
		
		CPen* oldPenPtr = pDC->SelectObject( &CMGraphView::s_grayPen );	
		pDC->MoveTo( tempRect.left, tempRect.bottom );
		pDC->LineTo( tempRect.left, tempRect.top ); 
		pDC->LineTo( tempRect.right, tempRect.top );  
		                             
		pDC->SelectObject( &CMGraphView::s_whitePen );	
		pDC->MoveTo( tempRect.right, tempRect.top+1 );
		pDC->LineTo( tempRect.right, tempRect.bottom );  
		pDC->LineTo( tempRect.left+1, tempRect.bottom ); 
		
		pDC->SelectObject( oldPenPtr );
	#endif	// defined multispec_win

   #if defined multispec_lin
    
   #endif
   
	if (graphPtr->drawGraphCode <= 0)
																								return;
	
	SetGraphClip( graphPtr, error );
	
			// Check if axis needs to be drawn for case to handle + and - data

	x_scale = screen_width / ( graphPtr->xScaleMax - graphPtr->xScaleMin );
	y_scale = screen_height / ( graphPtr->yScaleMax - graphPtr->yScaleMin );
	
	x_offset = graphPtr->xScaleMin * x_scale - 
														graphPtr->leftInset - clientRectPtr->left;
	y_offset = graphPtr->yScaleMin * y_scale + screen_height + 
														graphPtr->topInset + clientRectPtr->top;
	
	if ( (graphPtr->yScaleMin < 0) && (graphPtr->yScaleMax > 0) && ( !(graphPtr->attrb & NU_YLOG) ) )
		{
		x_point = (SInt16)( floor(graphPtr->xScaleMin * x_scale) - x_offset );
		y_point = (SInt16)y_offset;   
		
		#if defined multispec_mac
			MoveTo( x_point, y_point );
			x_point = (SInt16)(graphPtr->xScaleMax * x_scale) - (SInt16)x_offset;
			LineTo( x_point, y_point );
		#endif	// defined multispec_mac 
		 
		#if defined multispec_win
			pDC->MoveTo( x_point, y_point );
			x_point = (SInt16)(graphPtr->xScaleMax * x_scale) - (SInt16)x_offset;
			pDC->LineTo( x_point, y_point );
		#endif	// defined multispec_win
		
      #if defined multispec_lin
         SInt16 m_x_point;
         m_x_point = (SInt16)(graphPtr->xScaleMax * x_scale) - (SInt16)x_offset;
         //pDC->DrawLine(x_point, y_point, m_x_point, y_point);
      #endif
		
		}
		
	if ( (graphPtr->xScaleMin < 0) && (graphPtr->xScaleMax > 0) && ( !(graphPtr->attrb & NU_XLOG) ) ) 
		{
		x_point = (SInt16)-x_offset;
		y_point = (SInt16)( y_offset - floor(graphPtr->yScaleMin * y_scale) );
		
		#if defined multispec_mac
			MoveTo( x_point, y_point );
			y_point = (SInt16)( y_offset - floor(graphPtr->yScaleMax * y_scale) );
			LineTo( x_point, y_point );
		#endif	// defined multispec_mac 
		 
		#if defined multispec_win
			pDC->MoveTo( x_point, y_point );
			y_point = (SInt16)( y_offset - floor(graphPtr->yScaleMax * y_scale) );
			pDC->LineTo( x_point, y_point );
		#endif	// defined multispec_win
		
      #if defined multispec_lin
        SInt16 m_y_point;
        m_y_point = (SInt16)( y_offset - floor(graphPtr->yScaleMax * y_scale) );
        //pDC->DrawLine(x_point, y_point, x_point, m_y_point); 
      #endif
		
		}
		
	SetWindowClip( graphPtr, error );
	
}		// End DrawAxis



// ========================================================================
//		(C) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//		DrawLinTicks:	This routine draws linearly scaled tick marks for
//						the specified graph.
//
//		DrawLinTicks( graph, xint, yint, size, error )
//
//		where:	graph		Pointer to graph record ( not graph port )
//				xint		Interval between major tick marks on the x axis.
//				yint		Interval between major tick marks on the y axis.
//				size		Size of tick marks in pixels
//				error		Returned error code ( see below )
//
//		Errors returned:	NU_NO_ERROR
//		
//		revised by Larry L. Biehl  12/31/2005
//
// ========================================================================

void	DrawLinTicks(                         
				GraphPtr				graph, 
				double				xint, 
				double				yint, 
				SInt16				size, 
				SInt32*				error )
								
{		// DrawLinTicks         
	Rect*						clientRectPtr;
	
	double					interval,
								x_offset,
								x_pos, 
								x_scale,
								y, 
								y_offset,
								y_pos,
								y_scale;
				
	SInt16					position,
								pos1, 
								pos2,
								pos3, 
								pos4,
								screen_width, 
								screen_height;
	

	*error = NU_NO_ERROR;
	#if defined multispec_win
		CDC* pDC = graph->pDC;
	#endif	// defined multispec_win
   #if defined multispec_lin
      wxDC* pDC = graph->pDC;
   #endif
	
	SetGraphClip( graph, error );                               
	
	clientRectPtr = &graph->clientRect;
	
	screen_width = ( clientRectPtr->right - clientRectPtr->left ) -
										graph->rightInset - graph->leftInset;
	screen_height = ( clientRectPtr->bottom - clientRectPtr->top ) -
										graph->bottomInset - graph->topInset;
										
	x_scale = screen_width / ( graph->xScaleMax - graph->xScaleMin );
	y_scale = screen_height / ( graph->yScaleMax - graph->yScaleMin );
	
	x_offset = graph->xScaleMin * x_scale - graph->leftInset - clientRectPtr->left;
	y_offset = graph->yScaleMin * y_scale + screen_height + 
																graph->topInset + clientRectPtr->top;
	
	interval = yint;  
   

	if ( ( !( graph->attrb & NU_YLOG ) ) && ( ( y_scale*interval ) > 3 ) ) 
		{
     
		if ( ( graph->xScaleMin < 0 ) && ( graph->xScaleMax > 0 ) )
			{
         
			pos1 = (SInt16)(-size - x_offset);
			pos2 =  (SInt16)(size - x_offset);
         
         #if defined multispec_lin
         if(pos1 < 0){
            pos1 = clientRectPtr->left + graph->leftInset;
            pos2 = clientRectPtr->right - graph->rightInset;
         }
         #endif
          
			//printf("yscale:%d, %lf, %d\n", size, x_offset, (int)(-size - x_offset));
			for ( y_pos = ceil( graph->yScaleMin / interval ) * interval;
					y_pos <= graph->yScaleMax; 
					y_pos += interval ) 
				{
				position = (SInt16)( y_offset - floor(y_pos * y_scale) );   
           
				#if defined multispec_mac
					MoveTo( pos1, position );
					LineTo( pos2, position );
				#endif	// defined multispec_mac 
				 
				#if defined multispec_win
					pDC->MoveTo( pos1, position );
					pDC->LineTo( pos2, position );
				#endif	// defined multispec_win
				
            #if defined multispec_lin
               // Draw the line in feature selection
               if(graph->setCode != 1 && graph->numberSets > 0 && interval != 1){
                  
                  pDC->DrawLine((int)pos1, (int)position, (int)pos2, (int)position);
               }
               //printf("pos1:%d, position:%d, pos2:%d\n", pos1, position, pos2);
            #endif
				
				}
				
			} 
			
		else		//  ( graph->xScaleMin >= 0 ) || ( graph->xScaleMax <= 0 )
			{
			pos1 = clientRectPtr->left + graph->leftInset;
			pos2 = pos1 + size;
			pos3 = clientRectPtr->right - graph->rightInset;
			pos4 = pos3 - size;
			                 
			pos2 = MIN(pos2,pos3);
				
			
			for ( y_pos = ceil( graph->yScaleMin / interval ) * interval;
					y_pos <= graph->yScaleMax; 
					y_pos += interval ) 
				{
				position = (SInt16)( y_offset - floor(y_pos * y_scale) );  
            
				#if defined multispec_mac
					MoveTo( pos1, position );
					LineTo( pos2, position );
				#endif	// defined multispec_mac 
				 
				#if defined multispec_win
					pDC->MoveTo( pos1, position );
					pDC->LineTo( pos2, position );
				#endif	// defined multispec_win
				
				#if defined multispec_lin
               //if(graph->setCode == 1 && graph->numberSets > 0 ||
               //   graph->xScaleMin == 1){
                  pDC->DrawLine((int)pos1, (int)position, (int)pos2, (int)position);
               //}
               //printf("pos1:%d, position:%d, interval:%d\n", pos1, position, graph->yScaleMax/interval);
            #endif
				
				if (pos2 < pos3)
					{  
					#if defined multispec_mac
						MoveTo( pos3, position );
						LineTo( pos4, position );
					#endif	// defined multispec_mac 
					 
					#if defined multispec_win
						pDC->MoveTo( pos3, position );
						pDC->LineTo( pos4, position );
					#endif	// defined multispec_win
					
					#if defined multispec_lin
                  //if(graph->setCode == 1 && graph->numberSets > 0 ||
                  //   graph->xScaleMin == 1){
                     pDC->DrawLine((int)pos3, (int)position, (int)pos4, (int)position);
                  //}
               #endif

					}		// end "if (pos2 < pos3)"
				
				}		// end "for ( y_pos = ..."
				
			}		// end "else"
			
		}
	
	interval = xint;

	if ( ( !( graph->attrb & NU_XLOG ) ) && ( ( x_scale*interval ) > 3 ) ) 
		{
      //printf("yScaleMin:%lf, yScaleMax:%lf\n", graph->yScaleMin, graph->yScaleMax);
      
		if ( ( graph->yScaleMin < 0 ) && ( graph->yScaleMax > 0 ) ) 
			{
			y = y_offset - floor(graph->yScaleMin * y_scale);
			pos1 = (SInt16)MAX(y_offset - size, y - screen_height);
			pos2 = (SInt16)MIN(y_offset + size, y);
			for ( 	x_pos = ceil( graph->xScaleMin / interval ) * interval;
					x_pos <= graph->xScaleMax; 
					x_pos += interval ) 
				{
				position = (SInt16)( floor(x_pos * x_scale) - x_offset );
				
				#if defined multispec_mac
					MoveTo( position, pos1 );
					LineTo( position, pos2 );
				#endif	// defined multispec_mac 
				 
				#if defined multispec_win
					pDC->MoveTo( position, pos1 );
					pDC->LineTo( position, pos2 );
				#endif	// defined multispec_win
				
				#if defined multispec_lin
               //pDC->DrawLine((int)pos1, (int)position, (int)pos2, (int)position);
               
            #endif
				}
				
			} 
			
		else 
			{
			pos1 = clientRectPtr->top + graph->topInset;
			pos2 = pos1 + size;
			pos3 = clientRectPtr->bottom - graph->bottomInset;
			pos4 = pos3 - size;  
			                 
			pos2 = MIN(pos2,pos3);
			 
			for ( x_pos = ceil( graph->xScaleMin / interval ) * interval;
					x_pos <= graph->xScaleMax; 
					x_pos += interval ) 
				{
				position = (SInt16)( floor(x_pos * x_scale) - x_offset );
				
				#if defined multispec_mac
					MoveTo( position, pos1 );
					LineTo( position, pos2 );
				#endif	// defined multispec_mac 
				 
				#if defined multispec_win
					pDC->MoveTo( position, pos1 );
					pDC->LineTo( position, pos2 );
				#endif	// defined multispec_win
				
				#if defined multispec_lin
               pDC->DrawLine((int)position, (int)pos1, (int)position, (int)pos2);
               //printf("pos1:%d, position:%d, pos2:%d\n", pos1, position, pos2);
            #endif
				
				if (pos2 < pos3)
					{
					#if defined multispec_mac
						MoveTo( position, pos3 );
						LineTo( position, pos4 );
					#endif	// defined multispec_mac 
					 
					#if defined multispec_win
						pDC->MoveTo( position, pos3 );
						pDC->LineTo( position, pos4 );
					#endif	// defined multispec_win
					
					#if defined multispec_lin
                  pDC->DrawLine((int)position, (int)pos3, (int)position, (int)pos4);
                  
               #endif
					}		// end "if (pos2 < pos3)"
				
				}
				
			}
			
		}
		
	SetWindowClip( graph, error );
	
}		// End DrawLinTicks

//------------------------------------------------------------------------------------
//		(C) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//		FindMaxMinV:	This routine scans a vector and returns the value
//						of the minimum and maximum elements.
//
//		FindMaxMinV (min, max, vect, error)
//
//		where:	min		Pointer to returned minimum value
//					max		Pointer to returned minimum value
//					vect		Pointer to vector to scan
//					error		Returned error code ( see below )
//
//		Errors returned:	NU_NO_ERROR
//			
//			
//		revised by Larry L. Biehl  12/09/1991
//		revised by Larry L. Biehl  01/03/2006
//
//------------------------------------------------------------------------------------

void	FindMaxMinV( 
				double*								max, 
				double*								min, 
				vector*								vect, 
				SInt32*								error)

{
	GRAPHDATA*			basePtr;
	
	double				temp;
	
	SInt32				loop,
							size;
	

	*error = NU_NO_ERROR;
	size = vect->numberPoints;              
	basePtr = (GRAPHDATA*)GetHandlePointer (
											vect->baseHandle, kNoLock, kNoMoveHi);
	
	*min = *max = *basePtr;
	
	for (loop=1; loop<size; loop++) 
		{
		basePtr++;
		temp = *basePtr;
		
		if (temp > *max)
			*max = temp;
		
		else if (temp < *min)
			*min = temp;
			
		}		// end "for (loop=1; loop<size; loop++)"
		
}		// End FindMaxMinV

// 	Find the min and max when it is known that they are at the beginning and end of
//		the vector of data.

void	FindMaxMinV(
				double*								maxPtr,
				double*								minPtr,
				vector*								vect,
				HSInt32Ptr							vectorLengthsPtr,
				HSInt32Ptr							vectorDataPtr,
				char*									vectorDisplayPtr,
				UInt32								numberVectors,
				UInt32								set)

{
	GRAPHDATA							*baseDataPtr;
	
	UInt32								dataIndex,
											size,
											vector,
											vectorIndex;
	
            
	baseDataPtr = (GRAPHDATA*)GetHandlePointer (
											vect->baseHandle, kNoLock, kNoMoveHi);
											
	*minPtr = DBL_MAX;
	*maxPtr = -DBL_MAX;
	
	for (vector=0; vector<numberVectors; vector++)
		{							
		if (vectorDisplayPtr[vector] > 0)
			{			
					// Get the index into the data array (basePtr) to start searching
					// for the maximum.
				
			vectorIndex = (set - 1) * numberVectors + vector;
			size = vectorLengthsPtr[vectorIndex];
			
			if (size > 0)
				{
				dataIndex = vectorDataPtr[vectorIndex];
					
				*minPtr = MIN(*minPtr, baseDataPtr[dataIndex]);
			
				*maxPtr = MAX(*maxPtr, baseDataPtr[dataIndex+size-1]);
				
				}		// end "if (size > 0)"
				
			}		// end "if (vectorDisplayPtr[vector] > 0)"
		
		}		// end "for (vector=0; vector<numberVector; vector++)"
		
}		// End FindMaxMinV



//------------------------------------------------------------------------------------
//								 Copyright (1988-2006)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FindMaxV
//
//	Software purpose:	The purpose of this routine is to create a new graph
//							within the input window.
//
//	Parameters in:		vect		Pointer to vector to scan
//
//	Parameters out:	max		Pointer to returned maximum value
//							error		Returned error code ( see below )
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/21/1999
//	Revised By:			Larry L. Biehl			Date: 02/09/1999

void	FindMaxV( 
				double*								maxPtr,
				vector*								vect,
				HSInt32Ptr							vectorLengthsPtr,
				HSInt32Ptr							vectorDataPtr,
				char*									vectorDisplayPtr,
				UInt32								numberVectors,
				UInt32								set)

{
	double								temp;
	
	GRAPHDATA							*baseDataPtr,
											*dataPtr;
	
	UInt32								dataIndex,
											loop,
											size,
											vector,
											vectorIndex;
											
	Boolean								initialValueFoundFlag = FALSE;
	
	           
	baseDataPtr = (GRAPHDATA*)GetHandlePointer (
											vect->baseHandle, kNoLock, kNoMoveHi);
	
	for (vector=0; vector<numberVectors; vector++)
		{				
		if (vectorDisplayPtr[vector] > 0)
			{			
					// Get the index into the support vectors for the specified set.
				
			vectorIndex = (set - 1) * numberVectors + vector;
					
					// Get the index into the data array (basePtr) to start searching
					// for the maximum.
				
			dataIndex = vectorDataPtr[vectorIndex];
			dataPtr = &baseDataPtr[dataIndex];
			
			size = vectorLengthsPtr[vectorIndex];
			
			if (!initialValueFoundFlag)
				{
				*maxPtr = *dataPtr;
				initialValueFoundFlag = TRUE;
				
				}		// end "if (!initialValueFoundFlag)"
		
			for (loop=0; loop<size; loop++) 
				{
				temp = *dataPtr;
				
				if (temp > *maxPtr)
					*maxPtr = temp;
					
				dataPtr++;
					
				}		// end "for (loop=0; loop<size; loop++)"
				
			}		// end "if (vectorDisplayPtr[vector] > 0)"
			
		}		// end "for (vector=0; vector<numberVector; vector++)"
		
}		// End FindMaxV



//------------------------------------------------------------------------------------
//								 Copyright (1988-2006)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FindMaxBinV
//
//	Software purpose:	This routine searches for the maximum y value for 'binned' 
//							histograms.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None
// 
// Called By:			SetGraphMinMax
//
//	Coded By:			Larry L. Biehl			Date: 12/11/2002
//	Revised By:			Larry L. Biehl			Date: 02/10/2006

void	FindMaxBinV(
				double*								maxPtr,
				vector*								xVector,
				vector*								yVector,
				HSInt32Ptr							vectorLengthsPtr,
				HSInt32Ptr							vectorDataPtr,
				char*									vectorDisplayPtr,
				double*								histogramBinWidthPtr,
				UInt32								numberVectors,
				UInt32								set,
				GraphPtr								graph)
					
{								
	double								binWidth,
											halfBinWidth,
											lastXGraphValue,
											xGraphValue,
											yScaleMax,
											y_val;
	
	GRAPHDATA							*xBaseDataPtr,
											*xValuePtr,
											*yBaseDataPtr,
											*yValuePtr;
								
	Rect*									clientRectPtr;
								
	UInt32								line,
											numberPoints,
											point,
											dataIndex,
											vectorIndex;
											
	Boolean								binEndFlag;
	
	
			// Make set 0 based.
			
	set--;
	
	clientRectPtr = &graph->clientRect;
	
//	screen_width = ( clientRectPtr->right - clientRectPtr->left ) -
//													graph->rightInset - graph->leftInset;
													
//	x_scale = screen_width / (graph->xScaleMax - graph->xScaleMin);
													
//	xScaleMin = graph->xScaleMin;
	yScaleMax = graph->yScaleMax;
	
			// Allow for width of data value bin.
			
	binWidth = graph->histogramBinWidth;
	halfBinWidth = binWidth/2;
						
	yScaleMax = 0;
	
	xBaseDataPtr = (GRAPHDATA*)GetHandlePointer (
											xVector->baseHandle, kNoLock, kNoMoveHi);
	yBaseDataPtr = (GRAPHDATA*)GetHandlePointer (
											yVector->baseHandle, kNoLock, kNoMoveHi);
		
	for (line=0; line<numberVectors; line++)
		{
		if (vectorDisplayPtr[line] > 0)
			{                           
					// Get the index into the support vectors for the specified set.
				
			vectorIndex = set * numberVectors + line;
					
					// Get the index into the data array (basePtr) to start searching
					// for the maximum.
				
			dataIndex = vectorDataPtr[vectorIndex];
			
			xValuePtr = &xBaseDataPtr[dataIndex];
			yValuePtr = &yBaseDataPtr[dataIndex];
			
			numberPoints = vectorLengthsPtr[vectorIndex];
			
			lastXGraphValue = floor ((*xValuePtr+halfBinWidth)/binWidth);
			y_val = 0;
			binEndFlag = FALSE;
			
			for (point=0; point<=numberPoints; point++) 
				{
				if (point < numberPoints)
					{ 
//					xGraphValue = (SInt32)(*xValuePtr/binWidth);
					xGraphValue = floor ((*xValuePtr+halfBinWidth)/binWidth);
				
					if (xGraphValue == lastXGraphValue)
						y_val += *yValuePtr;
						
					else		// xGraphValue != lastXGraphValue
						binEndFlag = TRUE;
						
					}		// end "if (point < numberPoints)"
					
				else		// point == numberPoints
					binEndFlag = TRUE;
					
				if (binEndFlag)
					{
					yScaleMax = MAX(yScaleMax, y_val);
					
					lastXGraphValue = xGraphValue; 
					y_val = *yValuePtr;
					
					}		// end "if (binEndFlag)"
			
				xValuePtr++;
				yValuePtr++;
				
				}		// end "for (point=0; point<numberPoints; point++)"
			
			}		// end "if (vectorDisplayPtr [lines] > 0)"
			
		}		// end "for (line=0; line<numberVectors; line++)"
		
	*maxPtr = yScaleMax;
	
}		// End "FindMaxBinV"



// ========================================================================
//		( C ) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//		FlaggedPlotV:	This routine draws the requested plot depending on
//						the value fo the flag.
//
//		FlaggedPlotV( graph, x, y, flag, symb, width, error )
//
//		where:	graph	Pointer to graph record ( not graph port )
//				x		Pointer to x vector
//				y		Pointer to y vector
//				flag	Flag specifying plots to draw
//				width	Width in pixels of bars
//				symb	Symbol to plot for scatter plot
//				error	Returned error code ( see below )
//
//		Errors returned:	NU_NO_ERROR
//		
//		revised 02/15/1993 by Larry L. Biehl
//		revised 01/21/1998 by Larry L. Biehl
//
//========================================================================

void	FlaggedPlotV( 
				GraphPtr								graphRecPtr, 
				vector*								x, 
				vector*								y, 
				SInt16								flag, 
				SInt16								width, 
				SInt32*								errorPtr )			
	
{
	*errorPtr = NU_NO_ERROR;
	LockAndGetVectorPointer ( x, errorPtr );
	
	if (*errorPtr == NU_NO_ERROR)
		LockAndGetVectorPointer ( y, errorPtr );

	if (*errorPtr == NU_NO_ERROR)
		{
		if (flag & NU_LINE_PLOT) 
			LinePlotV( graphRecPtr, x, y, errorPtr );
			
		else if (flag & NU_SCATTER_PLOT) 
			ScatterPlotV( graphRecPtr, x, y, errorPtr );
		
		else if (flag & NU_HISTOGRAM_PLOT)
			HistogramPlotV( graphRecPtr, x, y, errorPtr);
/*		
		else if ( flag & NU_BAR_PLOT ) 
			{
			BarPlotV( graph, x, y, width, error );
			if ( *error )
				ErrorMessage( *error, "In Bar Plot", ABORT_ITEM, stopIcon );
				
			} 
*/
		}		// end "if (error == NU_NO_ERROR)"
		
	UnlockVectorPointer ( x );
	UnlockVectorPointer ( y );
		
}		// End FlaggedPlotV



// ======================================================================
//		(C) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//
//		FormatR:	This routine formats a double number and places it
//						into a string
//
//		FormatR(buffer, number, sigfigs, error)
//
//		where	buffer			Pointer to returned string.
//				number			Number to format.
//				sigfigs			Desired number of significant figures.
//				noEFormatFlag	Flag indicting that EFormat is not to be used.
//				error				Pointer to returned error code.
//
//		errors returned:		NU_NO_ERROR or NU_ALLOC_ERR
//		
//		Revised by Larry L. Biehl 	02/16/2006
// 
// =======================================================================

void	FormatR (
				char*							str, 
				double*						number, 
				SInt16						esigfigs, 
				SInt16						fsigfigs, 
				Boolean						noEFormatFlag,
				SInt32*						error)
								
{		// FormatR
//	char		temp[ NU_MAXREALTERM_SIZE ];
	char							format[15],
									dec[5];
									
	double						absValue;
									
	SInt16						decimals,
									numberChars,
									digitsWithoutComma,
									sigfigs;
									
	Boolean						insertCommasFlag;
	

	*error = NU_NO_ERROR;
	absValue = fabs(*number);
	if (absValue == 0 || 
			noEFormatFlag ||
				(absValue >= 0.0000001 && absValue <= 1000000))
		{
		if (absValue == 0)
			fsigfigs = 0;
		
		sigfigs = MAX (0, fsigfigs);
		sigfigs = MIN (fsigfigs, NU_MAX_DIGITS);
		
		sprintf (format, "%d", fsigfigs);
		strcpy (dec, "%.");
		strcat (dec, format);
		strcat (dec, "f");
			
		insertCommasFlag = TRUE;
		
		sigfigs = fsigfigs;
			
		}		// end "if (absValue >= 0.0000001 && absValue <= 1000000)"
		
	else		// absValue != 0 && (absValue < .0000001 || absValue > 1000000) 
		{
		esigfigs = MAX (0, esigfigs);
		esigfigs = MIN (esigfigs, 5);
		
		sprintf (format, "%d", esigfigs);
		strcpy (dec, "%.");
		strcat (dec, format);
		strcat (dec, "E");
			
		insertCommasFlag = FALSE;
		
		sigfigs = esigfigs;
			
		}		// end "else absValue < .0000001 || absValue > 1000000"
		
	if (*number == -0) 
		*number = 0;
		
	numberChars = sprintf (str, dec, *number);
	
	if (insertCommasFlag)
		{
		digitsWithoutComma = 3;
		if (*number < 0)
			digitsWithoutComma = 4;
			
		if (numberChars-sigfigs > digitsWithoutComma)
			{
			decimals = sigfigs;
			if (decimals == 0)
				decimals = -1;
				
			InsertCommasInNumberString (str, 
													numberChars, 
													decimals,
													numberChars);
													
			}		// end "if (numberChars-sigfigs > 4)"
			
		}		// end "if (insertCommasFlag)"
									
//	sprintf(temp, dec, *number );
//	*str = CheckAndDisposePtr (*str);
		
//	*str = MNewPointer( (SInt32)( strlen( temp ) + 1 ) );
//	if ( *str )
//		strcpy( *str, temp );
//	else
//		*error = NU_ALLOC_ERR;
		
}		// End FormatR



//------------------------------------------------------------------------------------
//								 Copyright (1988-2006)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void HistogramPlotV
//
//	Software purpose:	This routine accepts two vectors containing the x and
//							y coordinats of each data point.  Scaling of the data
//							to fit the screen is performed automatically.
//
//	Parameters in:		graph		Pointer to graph record ( not graph port )
//							x			Pointer to x vector
//							y			Pointer to y vector
//
//	Parameters out:	error		Returned error code ( see below )
//											NU_NO_ERROR, NU_NOTSAMESIZEG, or NU_MUSTBEPOS
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/21/1999
//	Revised By:			Larry L. Biehl			Date: 02/14/2006

void	HistogramPlotV(                             
				GraphPtr								graph, 
				vector*								xPtr, 
				vector*								yPtr, 
				SInt32*								errorPtr)
					
{								
	double								binWidth,
											doublePenWidth,
											halfBinWidth,
											lastXGraphValue,
											xGraphValue,
											xScaleMin,
											x_offset,
											x_scale,
											y_offset, 
											y_scale,
											y_val;
	
	GRAPHDATA							*xValuePtr,
											*yValuePtr;
								
	Rect*									clientRectPtr;
	
	char*									vectorDisplayPtr;
	
	HSInt32Ptr							vectorLengthsPtr,
											vectorPaletteColorPtr,
											vectorDataPtr;
								
	UInt32								line,
											numberPoints,
											numberVectors,
											point,
											set,
											dataIndex,
											vectorIndex;
	
	SInt16								penWidth,
											screen_width, 
											screen_height,
											x_point, 
											y_point;
											
	Boolean								plotFlag;
	
	#if defined multispec_win
		CPen									*currentPenPtr = NULL,
												*histogramPenPtr = NULL;
		LOGPEN								currentLogPen;
		LOGBRUSH								logBrush;
//		Boolean								continueFlag;
	#endif	// defined multispec_mac 
	
	#ifndef multispec_lin	
		CDC* pDC = graph->pDC;
	#endif
	
	#ifdef multispec_lin
		wxDC* pDC = graph->pDC;
	#endif 

	*errorPtr = NU_NO_ERROR;
	if (xPtr->numberPoints != yPtr->numberPoints) 
		{
		*errorPtr = NU_NOTSAMESIZEG;
		return;
		
		}		// end "if (xPtr->numberPoints != yPtr->numberPoints)"
		
	SetGraphClip (graph, errorPtr);                         
	
	clientRectPtr = &graph->clientRect;
	
	screen_width = (clientRectPtr->right - clientRectPtr->left) -
													graph->rightInset - graph->leftInset;
	screen_height = (clientRectPtr->bottom - clientRectPtr->top) -
													graph->bottomInset - graph->topInset;
													
	xScaleMin = graph->xScaleMin;
													
	x_scale = screen_width / (graph->xScaleMax - graph->xScaleMin);
	x_offset = graph->xScaleMin*x_scale - graph->leftInset - clientRectPtr->left;
//	x_offset = (SInt32)( -graph->leftInset - clientRectPtr->left);
		
	y_scale = screen_height / (graph->yScaleMax - graph->yScaleMin);
	y_offset = graph->yScaleMin * y_scale + screen_height + 
																graph->topInset + clientRectPtr->top;
	
			// Allow for width of data value bin.
			
	binWidth = graph->histogramBinWidth;
   
   		
	halfBinWidth = binWidth/2;
//	x_offset -= (binWidth-1)/2. * x_scale;
//	x_offset -= binWidth/2. * x_scale;
																
//	penWidth = (SInt16)(x_scale * binWidth * .75);
	doublePenWidth = x_scale * binWidth;
	penWidth = (SInt16)MAX(doublePenWidth * .75, doublePenWidth - 6);						
	penWidth = MAX (1, penWidth);
	
	#if defined multispec_mac
		x_offset += (double)penWidth/2;
	#endif	// defined multispec_mac
	
	vectorPaletteColorPtr = (SInt32*)GetHandlePointer (
											graph->vectorPaletteColorHandle,
											kLock,
											kNoMoveHi);
	
	vectorLengthsPtr = (SInt32*)GetHandlePointer (
											graph->vectorLengthsHandle,
											kLock,
											kNoMoveHi);
	
	vectorDataPtr = (SInt32*)GetHandlePointer (
											graph->xVectorDataHandle,
											kLock,
											kNoMoveHi);
	
	vectorDisplayPtr = (char*)GetHandlePointer (
											graph->vectorDisplayHandle,
											kLock,
											kNoMoveHi);
						
	numberVectors = graph->numberVectors;
	set = graph->set;
	
	#if defined multispec_mac
		PenSize (penWidth,1);
	#endif	// defined multispec_mac 
		
	for (line=0; line<numberVectors; line++)
		{
		if (vectorDisplayPtr [line] > 0)
			{                           
					// Get the index into the support vectors for the specified set.
				
			vectorIndex = (set - 1) * numberVectors + line;
					
					// Get the index into the data array (basePtr) to start searching
					// for the maximum.
				
			dataIndex = vectorDataPtr[vectorIndex];
		                  
			MForeColor (pDC, vectorPaletteColorPtr[line]);
         
         #if defined multispec_lin
            wxBrush    *newPenPtr = NULL;
            wxBrush    *oldPenPtr = NULL;
            wxBrush    brush;
            wxPen      pen;
            switch (vectorPaletteColorPtr[line])
            {
               case blackColor:
                  brush.SetColour(*wxBLACK);
                  newPenPtr = &brush;
                  break; 
               case blueColor:
                  brush.SetColour(*wxBLUE);
                  newPenPtr = &brush; 
                  break;   		
               case cyanColor:
                  brush.SetColour(*wxCYAN);
                  newPenPtr = &brush;
                  break;  			 
               case greenColor:
                  brush.SetColour(*wxGREEN);
                  newPenPtr = &brush;
                  break;    	
               case magentaColor:
                  brush.SetColour(*wxRED);
                  newPenPtr = &brush;
                  break; 
               case redColor:
                  brush.SetColour(*wxRED);
                  newPenPtr = &brush;
                  break;     		
               case whiteColor:
                  brush.SetColour(*wxWHITE);
                  newPenPtr = &brush;
                  break;   
               case yellowColor:
                  brush.SetColour(*wxYELLOW);
                  newPenPtr = &brush;
                  break; 		
               default:
                  newPenPtr = NULL;
			
            }		// end "switch (color)" 
            if (newPenPtr != NULL){
               oldPenPtr = newPenPtr;
               pen.SetColour(*wxBLACK);
               pen.SetWidth(1);
               pen.SetStyle(wxPENSTYLE_TRANSPARENT);
            }
            
            pDC->SetBrush(*oldPenPtr);
            pDC->SetPen(pen);
         #endif	// defined multispec_lin
	
			#if defined multispec_win
						// Set Pen width.

				if (penWidth != 1)
					{
					currentPenPtr = pDC->GetCurrentPen ();
					currentPenPtr->GetLogPen(&currentLogPen);

					logBrush.lbStyle = BS_SOLID;
					logBrush.lbColor = currentLogPen.lopnColor;

					TRY
						{ 
						histogramPenPtr = new CPen(PS_SOLID |
									PS_GEOMETRIC | PS_ENDCAP_FLAT | PS_JOIN_BEVEL,
									penWidth,
									&logBrush,
									0,
									NULL);
						
						pDC->SelectObject(histogramPenPtr);

						}
			
					CATCH_ALL(e)
						{
						MemoryMessage(0, kObjectMessage);
						}
					END_CATCH_ALL 

					}		// end "if (penWidth != 1)"
			#endif	// defined multispec_win
         
         #if defined multispec_lin
            
         #endif	// defined multispec_lin
			
			xValuePtr = &xPtr->basePtr[dataIndex];
			yValuePtr = &yPtr->basePtr[dataIndex];
			
			numberPoints = vectorLengthsPtr[vectorIndex];
			
//			lastXGraphValue = (SInt32)(*xValuePtr/(SInt32)binWidth);
			lastXGraphValue = floor ((*xValuePtr+halfBinWidth)/binWidth);
			y_val = 0;
			plotFlag = FALSE;
         
         #if defined multispec_lin
            SInt16 xx_point, yy_point;
            SInt16 x_prev = 0, x_next;
            x_prev = (SInt16)(
							floor(lastXGraphValue * binWidth * x_scale) - x_offset);
            x_next = (SInt16)(
							floor(floor ((*(xValuePtr+1)+halfBinWidth)/binWidth) * binWidth * x_scale) 
               - x_offset);
         #endif	// defined multispec_lin

			  
			for (point=0; point<=numberPoints; point++) 
				{
//				x_val = *xValuePtr;
//				y_val = *yValuePtr;

				if (point < numberPoints)
					{ 
//					xGraphValue = (SInt32)(*xValuePtr/(SInt32)binWidth);
					xGraphValue = floor ((*xValuePtr+halfBinWidth)/binWidth);
				
					if (xGraphValue == lastXGraphValue)
						y_val += *yValuePtr;
						
					else		// xGraphValue != lastXGraphValue
						plotFlag = TRUE;
						
					}		// end "if (point < numberPoints)"
					
				else		// point == numberPoints
					plotFlag = TRUE;
					
				if (plotFlag)
					{
							// Plot the bar
				
					x_point = (SInt16)(
							floor(lastXGraphValue * binWidth * x_scale) - x_offset);
					
					y_point = (SInt16)y_offset;
					
					#if defined multispec_mac
						MoveTo (x_point, y_point);
					#endif	// defined multispec_mac 
					 
					#if defined multispec_win
						pDC->MoveTo (x_point, y_point);
					#endif	// defined multispec_win

               #if defined multispec_lin
                  xx_point = x_point;
                  yy_point = y_point;
               #endif	// defined multispec_lin
					
					y_point = (SInt16)(y_offset - floor(y_val * y_scale));
					
					#if defined multispec_mac
						LineTo (x_point, y_point);
					#endif	// defined multispec_mac 
					 
					#if defined multispec_win
						pDC->LineTo (x_point, y_point);
					#endif	// defined multispec_win
               #if defined multispec_lin
                  pDC->DrawRectangle(x_point-(penWidth/2), y_point, penWidth , yy_point-y_point);
                  
               #endif
					
					lastXGraphValue = xGraphValue; 
					y_val = *yValuePtr;
					
					}		// end "if (plotFlag)"
			
				xValuePtr++;
				yValuePtr++;
				
				}		// end "for (point=0; point<numberPoints; point++)"
	
			#if defined multispec_win
				if (histogramPenPtr != NULL)
					delete histogramPenPtr;

				histogramPenPtr = NULL;
			#endif	// defined multispec_mac 
			
			}		// end "if (vectorDisplayPtr [lines] > 0)"
			
		}		// end "for (line=0; line<numberVectors; line++)"
		
	#if defined multispec_mac
		PenSize (1,1);
	#endif	// defined multispec_mac 
		
	MForeColor (pDC, blackColor);
	
	CheckAndUnlockHandle (graph->vectorPaletteColorHandle);
	CheckAndUnlockHandle (graph->vectorLengthsHandle);
	CheckAndUnlockHandle (graph->xVectorDataHandle);
	CheckAndUnlockHandle (graph->vectorDisplayHandle);
		
	SetWindowClip (graph, errorPtr);
	
}		// End "HistogramPlotV"


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2006)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void HistogramPlotV2
//
//	Software purpose:	This routine accepts two vectors containing the x and
//							y coordinats of each data point.  Scaling of the data
//							to fit the screen is performed automatically.
//
//	Parameters in:		graph		Pointer to graph record ( not graph port )
//							x			Pointer to x vector
//							y			Pointer to y vector
//
//	Parameters out:	error		Returned error code ( see below )
//											NU_NO_ERROR, NU_NOTSAMESIZEG, or NU_MUSTBEPOS
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/21/1999
//	Revised By:			Larry L. Biehl			Date: 12/06/2002

void	HistogramPlotV2(                             
				GraphPtr								graph, 
				vector*								xPtr, 
				vector*								yPtr, 
				SInt32*								errorPtr)
					
{								
	double								xScaleMin,
											x_scale, 
											y_scale, 
//											x_val, 
											y_val;
	
	GRAPHDATA							*xValuePtr,
											*yValuePtr;
								
	Rect*									clientRectPtr;
	
	char*									vectorDisplayPtr;
	
	HSInt32Ptr							vectorLengthsPtr,
											vectorPaletteColorPtr,
											vectorDataPtr;
											
	SInt32								lastXGraphValue,
											xGraphValue,
											x_offset,
											y_offset;
								
	UInt32								binWidth,
											line,
											numberPoints,
											numberVectors,
											point,
											set,
											dataIndex,
											vectorIndex;
	
	SInt16								penWidth,
											screen_width, 
											screen_height,
											x_point, 
											y_point;
											
	Boolean								plotFlag;
	
	
	CDC* pDC = graph->pDC;

	*errorPtr = NU_NO_ERROR;
	if (xPtr->numberPoints != yPtr->numberPoints) 
		{
		*errorPtr = NU_NOTSAMESIZEG;
		return;
		
		}		// end "if (xPtr->numberPoints != yPtr->numberPoints)"
		
	SetGraphClip (graph, errorPtr);                         
	
	clientRectPtr = &graph->clientRect;
	
	screen_width = (clientRectPtr->right - clientRectPtr->left) -
													graph->rightInset - graph->leftInset;
	screen_height = (clientRectPtr->bottom - clientRectPtr->top) -
													graph->bottomInset - graph->topInset;
													
	xScaleMin = graph->xScaleMin;
													
	x_scale = screen_width / (graph->xScaleMax - graph->xScaleMin);
//	x_offset = (SInt32)(graph->xScaleMin * x_scale - graph->leftInset - clientRectPtr->left);
	x_offset = (SInt32)( -graph->leftInset - clientRectPtr->left);
		
	y_scale = screen_height / (graph->yScaleMax - graph->yScaleMin);
	y_offset = (SInt32)(graph->yScaleMin * y_scale + screen_height + 
																graph->topInset + clientRectPtr->top);
	
			// Allow for width of data value bin.
			
	binWidth = graph->histogramBinWidth;
	x_offset -= ((double)binWidth-1)/2. * x_scale;
																
	penWidth = (SInt16)(x_scale * binWidth / 2);						
	penWidth = MAX (1, penWidth);
	
	x_offset += penWidth/2;
	
	vectorPaletteColorPtr = (SInt32*)GetHandlePointer (
											graph->vectorPaletteColorHandle,
											kLock,
											kNoMoveHi);
	
	vectorLengthsPtr = (SInt32*)GetHandlePointer (
											graph->vectorLengthsHandle,
											kLock,
											kNoMoveHi);
	
	vectorDataPtr = (SInt32*)GetHandlePointer (
											graph->xVectorDataHandle,
											kLock,
											kNoMoveHi);
	
	vectorDisplayPtr = (char*)GetHandlePointer (
											graph->vectorDisplayHandle,
											kLock,
											kNoMoveHi);
						
	numberVectors = graph->numberVectors;
	set = graph->set;
	
	#if defined multispec_mac
		PenSize (penWidth,1);
	#endif	// defined multispec_mac 
	
	for (dataValue=0; dataIndex<=xDataMax; dataIndex++)
		{
		for (line=0; line<numberVectors; line++)
			{
			if (vectorDisplayPtr [line] > 0)
				{                           
						// Get the index into the support vectors for the specified set.
					
				vectorIndex = (set - 1) * numberVectors + line;
						
						// Get the index into the data array (basePtr) to start searching
						// for the maximum.
					
				dataIndex = vectorDataPtr[vectorIndex];
			                  
				MForeColor( pDC, vectorPaletteColorPtr[line] );
				
				xValuePtr = &xPtr->basePtr[dataIndex];
				yValuePtr = &yPtr->basePtr[dataIndex];
				
				numberPoints = vectorLengthsPtr[vectorIndex];
				
				lastXGraphValue = (SInt32)((*xValuePtr - xScaleMin)/binWidth);
				y_val = 0;
				plotFlag = FALSE;
				
				for (point=0; point<=numberPoints; point++) 
					{
	//				x_val = *xValuePtr;
	//				y_val = *yValuePtr;

					if (point < numberPoints)
						{ 
						xGraphValue = (SInt32)((*xValuePtr - xScaleMin)/binWidth);
					
						if (xGraphValue == lastXGraphValue)
							y_val += *yValuePtr;
							
						else		// xGraphValue != lastXGraphValue
							plotFlag = TRUE;
							
						}		// end "if (point < numberPoints)"
						
					else		// point == numberPoints
						plotFlag = TRUE;
						
					if (plotFlag)
						{
								// Plot the bar
					
						x_point = (SInt16)(floor(lastXGraphValue * binWidth * x_scale) - x_offset);
						y_point = (SInt16)y_offset;
						
						#if defined multispec_mac
							MoveTo (x_point, y_point);
						#endif	// defined multispec_mac 
						 
						#if defined multispec_win
							pDC->MoveTo (x_point, y_point);
						#endif	// defined multispec_win
						
						y_point = (SInt16)(y_offset - floor(y_val * y_scale));
						
						#if defined multispec_mac
							LineTo (x_point, y_point);
						#endif	// defined multispec_mac 
						 
						#if defined multispec_win
							pDC->LineTo (x_point, y_point);
						#endif	// defined multispec_win
						
						lastXGraphValue = xGraphValue; 
						y_val = *yValuePtr;
						
						}		// end "if (plotFlag)"
				
					xValuePtr++;
					yValuePtr++;
					
					}		// end "for (point=0; point<numberPoints; point++)"
				
				}		// end "if (vectorDisplayPtr [lines] > 0)"
				
			}		// end "for (line=0; line<numberVectors; line++)"
			
		}		// end "for (dataValue=xDataMin; dataValue<=xDataMax; dataValue++)"
		
	#if defined multispec_mac
		PenSize (1,1);
	#endif	// defined multispec_mac 
		
	MForeColor( pDC, blackColor );
	
	CheckAndUnlockHandle (graph->vectorPaletteColorHandle);
	CheckAndUnlockHandle (graph->vectorLengthsHandle);
	CheckAndUnlockHandle (graph->xVectorDataHandle);
	CheckAndUnlockHandle (graph->vectorDisplayHandle);
		
	SetWindowClip (graph, errorPtr);
	
}		// End "HistogramPlotV2"
*/


// ========================================================================
//		(C) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//		InitV:	Vector initialization.  This routine initializes a
//					vector to a null value.  It should be called to
//					initialize a vector to a known state before using
//					the vector.  Note that the vector should only
//					be initialized once.
//
//		InitV( errror )
//	
//
//		where:	error	Returned error code
//
//			NO_ERROR	No error - successful completion
//			
//			
//		revised by Larry L. Biehl  12/09/1991
//		revised by Larry L. Biehl  02/15/1993
//
// ========================================================================

void InitV(
				vector				*vectorPtr,
				SInt32				*errorPtr )
	
{
	*errorPtr = NU_NO_ERROR;
	
	vectorPtr->numberPoints = 0;
	vectorPtr->size = 0;
	vectorPtr->baseHandle = NULL;
	vectorPtr->basePtr = NULL;
	
}		// End InitV



// ========================================================================
//		(C) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//		InsetGraph:	This routine insets the graph in the window.
//
//		InsetGraph( graph, error )
//
//		where:	graph	Pointer to graph record ( not graph port )
//				left	Distance to inset from right side
//				top		Distance to inset from top
//				right	Distance to inset from right side
//				bottom	Distance to inset from bottom
//				error	Returned error code ( see below )
//
//		Errors returned:	NU_NO_ERROR
//		
//	Revised By:			Larry L. Biehl			Date: 10/09/1991	
//
// ========================================================================

void	InsetGraph( 
				GraphPtr				graph, 
				SInt16				left, 
				SInt16				top,
				SInt16				right,
				SInt16				bottom, 
				SInt32				*error )
								
{		// InsetGraph

	*error = NU_NO_ERROR;
	
	graph->leftInset = left;
	graph->topInset = top;
	graph->bottomInset = bottom;
	graph->rightInset = right;
	
}		// End InsetGraph



// ========================================================================
//		(C) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//		LabelLinAxis:	This routine draws linearly scaled tick marks for
//						the specified graph.
//
//		LabelLinAxis( graph, xint, yint, size, sigfigs, error )
//
//		where:	graph		Pointer to graph record ( not graph port )
//				xint		Interval between major tick marks on the x axis.
//				yint		Interval between major tick marks on the y axis.
//				size		Size of tick marks in pixels
//				sigfigs		Number of decimal places to write
//				error		Returned error code ( see below )
//
//		Errors returned:	NU_NO_ERROR
//		
//		Revised by Larry L. Biehl	02/21/2017
//
// ========================================================================

static	void	LabelLinAxis_TrimString(
				 char				*str)
				 				 
{		// LabelLinAxis_TrimString
	SInt16	not_sp;
	SInt16	len;
	SInt16	loop;
	

	not_sp = strspn (str, " ");
	len = strlen (str);
	for (loop = 0; (loop < len) || (str[loop] != '\0'); loop++) 
		{
		str[loop] = str[loop + not_sp];
		
		}
		
}		// End LabelLinAxis_TrimString



void	LabelLinAxis(                          
				GraphPtr				graph, 
				double				xint, 
				double				yint, 
				SInt16				size, 
				SInt16				xESigfigs,
				SInt16				xFSigfigs, 
				SInt16				yESigfigs,  
				SInt16				yFSigfigs, 
				SInt32				*error)
								
{		// LabelLinAxis         
	Rect*						clientRectPtr;
	
	char						number[64];
	
	double					x_offset,
								x_pos,
								x_scale,
								y_offset,
								y_pos, 
								y_scale; 
	
	SInt16					bevelInset,
								height,
								line,
								offset, 
								screen_height,
								screen_width,
								width;
								
	Boolean					noEFormatFlag;
	

	*error = NU_NO_ERROR;
	
	#if defined multispec_win
		CDC* pDC = graph->pDC;
	#endif	// defined multispec_win
	
	#if defined multispec_lin
      wxDC* pDC = graph->pDC;
      wxFont ffont(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL); 
      pDC->SetFont(ffont);
   #endif
	
#	if defined multispec_win
		USES_CONVERSION;
#	endif

	SetWindowClip( graph, error );                                
	
	clientRectPtr = &graph->clientRect;
	
	screen_width = (clientRectPtr->right - clientRectPtr->left) -
														graph->rightInset - graph->leftInset;
	screen_height = (clientRectPtr->bottom - clientRectPtr->top) -
														graph->bottomInset - graph->topInset;
   
		
	x_scale = screen_width / (graph->xScaleMax - graph->xScaleMin);
	y_scale = screen_height / (graph->yScaleMax - graph->yScaleMin);
	
	x_offset = graph->xScaleMin * x_scale - graph->leftInset - clientRectPtr->left;
	y_offset = graph->yScaleMin * y_scale + screen_height + 
																graph->topInset + clientRectPtr->top;
	                            
	height = graph->textSize;
	if (height == 0)
		height = 6;
	else
		height = (height+1)/2;
	
	
			//	If y axis is not log scaled, label it
   
		
	if (!(graph->attrb & NU_YLOG) && ((y_scale*yint) > 8 )) 
		{
				// Get the bevel inset.
      
		bevelInset = (SInt16)(3 * graph->textScaling);

		offset = clientRectPtr->left - size + graph->leftInset - bevelInset;
						
		noEFormatFlag = FALSE;
//		if (yint > .0000001 && yint < 1000000)
//			noEFormatFlag = TRUE;
			
		for (y_pos = ceil (graph->yScaleMin / yint - 0.1) * yint;
				y_pos < graph->yScaleMax + yint/10; 
				y_pos += yint) 
			{
			FormatR (number, &y_pos, yESigfigs, yFSigfigs, noEFormatFlag, error);
			LabelLinAxis_TrimString (number);
         //printf("y_pos:%lf, %lf\n", (graph->yScaleMax + yint/10), y_pos);
			
			width = TextWidth ((UCharPtr)number, 0, strlen (number)); 
         #if defined multispec_lin
				line = (SInt16)(y_offset - floor(y_pos * y_scale) - height);
         #endif
			
         #ifndef multispec_lin
				line = (SInt16)(y_offset - floor(y_pos * y_scale) + height);
         #endif

			#if defined multispec_mac
				MoveTo (offset - width, line);
				DrawText (number, 0, strlen(number));
			#endif	// defined multispec_mac 
			 
			#if defined multispec_win
				pDC->TextOut (offset - width, line, (LPCTSTR)A2T(number), strlen(number));
			#endif	// defined multispec_win
			
         #if defined multispec_lin
            pDC->DrawText((wxString)(number), offset - width , line);
            //printf("offset:%d, %d, %s\n", line, offset - width, number);
         #endif
			
			}		// end "for (y_pos = ceil(..."
			
		}		// end "if (!( graph->attrb & ..."
		
			// Then if x axis is not log scaled, label it
   
	if ((!(graph->attrb & NU_XLOG)) && ((x_scale*xint) > 10)) 
		{
				// Get the bevel inset.
      
		#if defined multispec_mac
			bevelInset = (SInt16)(2 * graph->textScaling);
		#endif	// defined multispec_mac 
			 
		#if defined multispec_win
			bevelInset = (SInt16)(5 * graph->textScaling);
		#endif	// defined multispec_win 
		
      #if defined multispec_lin
         bevelInset = (SInt16)(2 * graph->textScaling);
      #endif // defined multispec_lin
		
		offset = clientRectPtr->bottom + size + 
						(SInt16)(height*2.2) - graph->bottomInset + bevelInset;
						
		noEFormatFlag = FALSE;
		if (xint > .0000001 && xint < 1000000)
			noEFormatFlag = TRUE;
						
		for (x_pos = ceil( graph->xScaleMin / xint - 0.1) * xint;
				x_pos < graph->xScaleMax + xint/10; 
				x_pos += xint ) 
			{
			FormatR (number, &x_pos, xESigfigs, xFSigfigs, noEFormatFlag, error);
			LabelLinAxis_TrimString (number);
			
			width = TextWidth ((UCharPtr)number, 0, strlen (number)); 
			line = (SInt16)(floor(x_pos * x_scale) - x_offset - width/2);
			
			#if defined multispec_mac
				MoveTo (line, offset);
				DrawText (number, 0, strlen (number));
			#endif	// defined multispec_mac 
			 
			#if defined multispec_win
				pDC->TextOut (line, offset, (LPCTSTR)A2T(number), strlen(number));
			#endif	// defined multispec_win 
			
			#if defined multispec_lin
            pDC->DrawText((wxString)(number), line, offset - 10);
            //printf("%d, %d, %s\n", line, offset, number);
			#endif
			}		// end "for (x_pos = ceil(..."
			
		}		// end "if ( ( !( graph->attrb & ..."

}		// End LabelLinAxis



// ========================================================================
//		(C) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//		LinePlotV:	This routine accepts two vectors containing the x and
//					y coordinats of each data point.  Scaling of the data
//					to fit the screen is performed automatically.
//
//
//		LinePlotV( graph, x, y, size, error )
//
//		where:	graph		Pointer to graph record ( not graph port )
//				x			Pointer to x vector
//				y			Pointer to y vector
//				error		Returned error code ( see below )
//
//		Errors returned:	NU_NO_ERROR, NU_NOTSAMESIZEG, or NU_MUSTBEPOS
//			
//			
//		revised by Larry L. Biehl  12/09/1991
//		revised by Larry L. Biehl  12/31/2005
//
// ========================================================================

void	LinePlotV(                             
				GraphPtr				graph, 
				vector				*x, 
				vector				*y, 
				SInt32				*error )
					
{								
	GRAPHDATA				*xValuePtr,
								*yValuePtr;
								
	Rect*						clientRectPtr;
	
	double					x_offset,
								x_scale, 
								x_val,
								y_offset, 
								y_scale, 
								y_val;
	
	char						*vectorDisplayPtr;
	
	SInt32					*vectorLengthsPtr,
								*vectorPaletteColorPtr,
								*xVectorDataPtr;
								
	UInt32					loop,
								points;
	
	SInt16					lines,
								numberVectors,
								screen_width, 
								screen_height,
								x_point, 
								y_point;
	#ifdef multispec_lin
		SInt16               x_point1, y_point1;
	#endif
	
	
	numberVectors = graph->numberVectors;
	
	#ifndef multispec_lin	
		CDC* pDC = graph->pDC;
	#endif
	
	#ifdef multispec_lin
		wxDC* pDC = graph->pDC;
	#endif 

	*error = NU_NO_ERROR;
	if ( x->numberPoints * numberVectors != y->numberPoints ) 
		{
		*error = NU_NOTSAMESIZEG;
		return;
		
		}		// end "if ( x->numberPoints != y->numberPoints )"
		
	SetGraphClip( graph, error );                         
	
	clientRectPtr = &graph->clientRect;
	
	screen_width = ( clientRectPtr->right - clientRectPtr->left ) -
													graph->rightInset - graph->leftInset;
	screen_height = ( clientRectPtr->bottom - clientRectPtr->top ) -
													graph->bottomInset - graph->topInset;
													
	x_scale = screen_width / ( graph->xScaleMax - graph->xScaleMin );
	x_offset = graph->xScaleMin * x_scale - graph->leftInset - clientRectPtr->left;
		
	y_scale = screen_height / ( graph->yScaleMax - graph->yScaleMin );
	y_offset = graph->yScaleMin * y_scale + screen_height + 
																graph->topInset + clientRectPtr->top;
	
	yValuePtr = y->basePtr;
	
	vectorPaletteColorPtr = (SInt32*)GetHandlePointer (
											graph->vectorPaletteColorHandle,
											kLock,
											kNoMoveHi);
	
	vectorLengthsPtr = (SInt32*)GetHandlePointer (
											graph->vectorLengthsHandle,
											kLock,
											kNoMoveHi);
	
	xVectorDataPtr = (SInt32*)GetHandlePointer (
											graph->xVectorDataHandle,
											kLock,
											kNoMoveHi);
	
	vectorDisplayPtr = (char*)GetHandlePointer (
											graph->vectorDisplayHandle,
											kLock,
											kNoMoveHi);
		
	for (lines=0; lines<numberVectors; lines++)
		{
		if (vectorDisplayPtr [lines] > 0)
			{                                             
			MForeColor (pDC, vectorPaletteColorPtr[lines]);
			
			xValuePtr = &x->basePtr[xVectorDataPtr[lines]];
			
			x_val = *xValuePtr;
			y_val = *yValuePtr;
			
        
			x_point = (SInt16)(floor(x_val * x_scale) - x_offset);
			y_point = (SInt16)(y_offset - floor(y_val * y_scale));
         
			#if defined multispec_mac
				MoveTo (x_point, y_point);
			#endif	// defined multispec_mac 
			 
			#if defined multispec_win
				pDC->MoveTo (x_point, y_point);
			#endif	// defined multispec_win

			points = vectorLengthsPtr[lines];
			
			for (loop=1; loop<points; loop++) 
				{
				xValuePtr++;
				yValuePtr++;
				
				x_val = *xValuePtr;
				y_val = *yValuePtr;
            
           
				
				#ifndef multispec_lin	
					x_point = (SInt16)(floor(x_val * x_scale) - x_offset);
					y_point = (SInt16)(y_offset - floor(y_val * y_scale));
            #endif
				
            #ifdef multispec_lin
					x_point1 = (SInt16)( floor(x_val * x_scale) - x_offset );
					y_point1 = (SInt16)( y_offset - floor(y_val * y_scale) );
            #endif
            
				#if defined multispec_mac
					LineTo( x_point, y_point );
				#endif	// defined multispec_mac 
				 
				#if defined multispec_win
					pDC->LineTo( x_point, y_point );
				#endif	// defined multispec_win
				
				#if defined multispec_lin
               pDC->DrawLine(x_point, y_point, x_point1, y_point1);
               x_point = x_point1;
               y_point = y_point1;
            #endif	// defined multispec_lin
				
				}		// end "for ( loop=1; loop<points; loop++)"
				
			yValuePtr++;
			
			}		// end "if (vectorDisplayPtr [lines] > 0)"
			
		else		// vectorDisplayPtr [lines] <= 0
			yValuePtr += vectorLengthsPtr[lines];
			
		}		// end "for (lines=0; lines<numberVectors; lines++)"
		
	MForeColor( pDC, blackColor );
	
	CheckAndUnlockHandle (graph->vectorPaletteColorHandle);
	CheckAndUnlockHandle (graph->vectorLengthsHandle);
	CheckAndUnlockHandle (graph->xVectorDataHandle);
	CheckAndUnlockHandle (graph->vectorDisplayHandle);
		
	SetWindowClip( graph, error );
	
}		// End LinePlotV



void	LockAndGetVectorPointer ( 
				vector*						x, 
				SInt32*						errorPtr )
		
{		// LockAndGetVectorPointer

	*errorPtr = NU_NO_ERROR;
	if (x->baseHandle != NULL)
		x->basePtr = (GRAPHDATA*)GetHandlePointer (
								x->baseHandle,
								kLock,
								kNoMoveHi);
		
	else		// x->baseHandle == NULL
		*errorPtr = 1001;
			
}		// End LockAndGetVectorPointer



// ========================================================================
//		(C) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//		NewGraph:	This routine loads values into a graph record and
//					returns a pointer to the record.
//
//		NewGraph( graph, window, title, xMin, xMax, yMin, yMax, attrb, 
//			bars, error )
//
//		where:	graph	Pointer to graph record ( not graph port )
//				window	Window pointer for graph window
//				title	Graph title
//				xMin	Minimum x value in graph
//				xMax	Maximum x value in graph
//				yMin	Minimum y value in graph
//				yMax	Maximum y value in graph
//				attrb	Attribute of the graph
//				bars	Flag to leave space for scroll bars
//				error	Returned error code ( see below )
//
//		Errors returned:	NU_NO_ERROR, NU_MUSTBEPOS, or NU_DIVBY0
//		
//	Revised By:			Larry L. Biehl			Date: 11/27/1991	
//
// ========================================================================

GraphPtr	NewGraph( 
				GraphRecord*		graph, 
				WindowPtr			window, 
				double				xMin, 
				double				xMax, 
				double				yMin, 
				double				yMax, 
				SInt16				attrb, 
				char					bars, 
				SInt32				*error)
								
{		// NewGraph

	
	*error = NU_NO_ERROR;
	if ( ( attrb & NU_XLOG ) && ( ( xMin < 0 ) || ( xMax < 0 ) ) ) 
		{
		*error = NU_MUSTBEPOS;
																							return (graph);
		}
		
	if ( ( attrb & NU_YLOG ) && ( ( yMin < 0 ) || ( yMax < 0 ) ) )
		{
		*error = NU_MUSTBEPOS;
																							return (graph);
		}
		
	if ((xMin == xMax) || (yMin == yMax)) 
		{
		*error = NU_DIVBY0;
																							return (graph);
		
		}
		
	graph->window = window;
	graph->xScaleMin = xMin;
	graph->xScaleMax = xMax;
	graph->yScaleMin = yMin;
	graph->yScaleMax = yMax;
	graph->attrb = attrb;
	graph->leftInset = 0;
	graph->topInset = 0;
	if ( bars )
		{
		graph->bottomInset = 16;
		#ifndef multispec_lin
			graph->rightInset = 16;
		#endif
		
		#ifdef multispec_lin
			graph->rightInset = 6;
		#endif
		
		} 
	else 
		{
		graph->bottomInset = 0;
		graph->rightInset = 0;
		
		}
		
//	SelectWindow( window );
//	SetPort( window );
	
	return (graph);
	
}		// End NewGraph



// ========================================================================
//		(C) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//		ScatterPlotV:	This routine accepts two vectors containing the x
//						and y coordinats of each data point.  At each point
//						the specified symbol is drawn.  Scaling of the data
//						to fit the screen is performed automatically.
//
//
//
//		ScatterPlotV( graph, x, y, symbol, error )
//
//		where:	graph		Pointer to graph record ( not graph port )
//				x			Pointer to x vector
//				y			Pointer to y vector
//				symbol		Symbol to plot at each point
//				error		Returned error code ( see below )
//
//		Errors returned:	NU_NO_ERROR, NU_NOTSAMESIZEG, or NU_MUSTBEPOS
//			
//			
//		revised by Larry L. Biehl  12/09/1991
//		revised by Larry L. Biehl  10/17/2016
//
// ========================================================================

void	ScatterPlotV(                             
				GraphPtr					graph, 
				vector					*x, 
				vector					*y, 
				SInt32					*error)

{
	GRAPHDATA					*xValuePtr,
									*yValuePtr;
	
	Rect*							clientRectPtr;
	
	double						x_offset,
									y_offset,
									x_scale, 
									y_scale, 
									x_val, 
									y_val;
	
	char							sym, 
									*vectorDisplayPtr,
									*vectorSymbolPtr;
	
	SInt32						*vectorLengthsPtr,
									*vectorPaletteColorPtr,
									*xVectorDataPtr;
								
	UInt32						loop,
									points;
	
	SInt16						height,
									lines,
									numberVectors,
									screen_width, 
									screen_height,
									width,
									x_point, 
									y_point;
	
	#if defined multispec_lin
		char							symbolString[2];
		wxString						wx_sym;
		symbolString[0] = 0;
		symbolString[1] = 0;
	#endif

	numberVectors = graph->numberVectors;

	#ifndef multispec_lin	
		CDC* pDC = graph->pDC;
	#endif
	
	#ifdef multispec_lin
		wxDC* pDC = graph->pDC;
	#endif 
		
	SetGraphClip( graph, error );                    
	
	clientRectPtr = &graph->clientRect;
	
	screen_width = (clientRectPtr->right - clientRectPtr->left) -
														graph->rightInset - graph->leftInset;
	screen_height = (clientRectPtr->bottom - clientRectPtr->top) -
														graph->bottomInset - graph->topInset;
											
	height = graph->textSize;
	if (height == 0)
		height = 6;
	else
		height = (height+1)/2;
		
	#if defined multispec_lin
		height = -height;
	#endif
		
	x_scale = screen_width / (graph->xScaleMax - graph->xScaleMin);
	
	y_scale = screen_height / (graph->yScaleMax - graph->yScaleMin);
	y_offset = graph->yScaleMin * y_scale + screen_height + height + 
															graph->topInset + clientRectPtr->top;
		
	points = x->numberPoints;
	yValuePtr = y->basePtr;
	
	vectorPaletteColorPtr = (SInt32*)GetHandlePointer (
										graph->vectorPaletteColorHandle,
										kLock,
										kNoMoveHi);
	
	vectorLengthsPtr = (SInt32*)GetHandlePointer (
										graph->vectorLengthsHandle,
										kLock,
										kNoMoveHi);
	
	xVectorDataPtr = (SInt32*)GetHandlePointer (
										graph->xVectorDataHandle,
										kLock,
										kNoMoveHi);
	
	vectorDisplayPtr = (char*)GetHandlePointer (
										graph->vectorDisplayHandle,
										kLock,
										kNoMoveHi);
	
	vectorSymbolPtr = (char*)GetHandlePointer (
										graph->vectorSymbolHandle,
										kLock,
										kNoMoveHi);
		
	for (lines=0; lines<numberVectors; lines++)
		{
		if (vectorDisplayPtr[lines] > 0)
			{
			MForeColor (pDC, vectorPaletteColorPtr[lines]);
				
			#if defined multispec_lin
				wxColour color;
				color.Set(vectorPaletteColorPtr[lines]);
				pDC->SetTextForeground(color);
			#endif

			xValuePtr = &x->basePtr[xVectorDataPtr[lines]];
			
			points = vectorLengthsPtr[lines];
			sym = vectorSymbolPtr[lines];	
			#if defined multispec_lin
				symbolString[0] = sym;
				wx_sym = wxString::FromAscii(symbolString);
			#endif
			
			width = CharWidth (sym) / 2;            
			x_offset = graph->xScaleMin * x_scale + width - 
																graph->leftInset - clientRectPtr->left;
			
			for (loop=0; loop<points; loop++ ) 
				{
				x_val = *xValuePtr;
				y_val = *yValuePtr;
				
				xValuePtr++;
				yValuePtr++;
					
				x_point = (SInt16)(floor(x_val * x_scale) - x_offset);
				y_point = (SInt16)(y_offset - floor(y_val * y_scale));
				
				#if defined multispec_mac
					MoveTo (x_point, y_point);
					DrawChar (sym);
				#endif	// defined multispec_mac 
				 
				#if defined multispec_win
					pDC->TextOut (x_point, y_point, (LPCTSTR)&sym, 1);
				#endif	// defined multispec_win
			
				#if defined multispec_lin
					pDC->DrawText(wx_sym, x_point, y_point);
				#endif
				
				}		// end "for (loop=0; loop<points; loop++ )"
			
			}		// end "if (vectorDisplayPtr [lines] > 0)"
			
		else		// vectorDisplayPtr [lines] <= 0
			yValuePtr += vectorLengthsPtr[lines];
			
		}		// end "for (lines=0; lines<numberVectors; lines++)"
		
	MForeColor (pDC, blackColor);
	
	CheckAndUnlockHandle (graph->vectorPaletteColorHandle);
	CheckAndUnlockHandle (graph->vectorLengthsHandle);
	CheckAndUnlockHandle (graph->xVectorDataHandle);
	CheckAndUnlockHandle (graph->vectorDisplayHandle);
	CheckAndUnlockHandle (graph->vectorSymbolHandle);
		
	SetWindowClip (graph, error);
	
}		// End ScatterPlotV
		
		
		
//-----------------------------------------------------------------------------
//								 Copyright (1988-2006)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetBackgroundColor
//
//	Software purpose:	This routine sets the background gray level to be used.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 09/01/1995
//	Revised By:			Larry L. Biehl			Date: 10/17/2015			

void SetBackgroundColor (
				CDC*							pDC,
				UInt16						level)
				
{
	#if defined multispec_mac

	#endif	// defined multispec_mac

	#if defined multispec_win
		pDC->SetBkColor( RGB(level, level, level) );
	#endif	// defined multispec_win
	
	#if defined multispec_lin
		//wxColour color;
		//color.Set(RGB(level, level, level));
		//pDC->SetBackgroundColour(color);
	#endif   

}		// end "SetBackgroundColor"



// ========================================================================
//		(C) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//		SetGraphClip:	This routine sets the clip region to the graph
//						limits.
//
//		SetGraphClip( min, error )
//
//		where:	graph	Pointer to graph record
//				error	Returned error code ( see below )
//
//		Errors returned:	NU_NO_ERROR
//
// ========================================================================

void	SetGraphClip(                          
				GraphPtr				graph, 
				SInt32				*error )
								
{		// SetGraphClip
	Rect				*clientRectPtr,
						tempRect;
	
	SInt16			left, 
						top, 
						right, 
						bottom;
						

	*error = NU_NO_ERROR;                                        
	
	clientRectPtr = &graph->clientRect;
	
	left	= clientRectPtr->left + graph->leftInset;
	top		= clientRectPtr->top + graph->topInset;
	right	= clientRectPtr->right - graph->rightInset;
	bottom	= clientRectPtr->bottom - graph->bottomInset;
	
   #ifndef multispec_lin
		SetRect( (tagRECT*)&tempRect, left, top, right, bottom );
   #endif
	
   #if defined multispec_lin
		tempRect.bottom = bottom;
		tempRect.right = right;
		tempRect.top = top;
		tempRect.left = left;
   #endif

	#if defined multispec_mac
		InsetRect( &tempRect, -1, -1 );
		SetPortWindowPort( graph->window );
		ClipRect( &tempRect );
	#endif	// defined multispec_mac 
	 
	#if defined multispec_win
		InsetRect( (tagRECT*)&tempRect, 1, 1 );
	#endif	// defined multispec_win
	
}		// End SetGraphClip



// ========================================================================
//		(C) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//		SetV:	Sets the value of an element in a vector.
//
//		SetV( v1, element, value, error )
//
//		where:	v1		Vector to get element from
//				element	Element to return from vector
//				error	Returned error code
//
//		Errors returned:
//
//			NO_ERROR	Successful completion
//
//		  OUTOFRANGEV	Requested element is not in vector
//			
//			
//		revised by Larry L. Biehl  12/09/1991
//
// ========================================================================

void SetV( 
				vector			*v1, 
				SInt32			element, 
				GRAPHDATA		value, 
				SInt32			*errorPtr )
				
{
	*errorPtr = NU_NO_ERROR;
	if (element < 0 || element >= v1->size)
		*errorPtr = NU_OUTOFRANGEV;
		
	else		// element >= 0 && element < v1->size
		v1->basePtr[element] = value;
		
}		// End SetV  



// ========================================================================
//		( C ) Copyright 1989. Nicus, Inc.  All rights reserved.
//
//		SetWindowClip:	This routine sets the clip region to the window
//						limits.
//
//		SetWindowClip( min, error )
//
//		where:	graph	Pointer to graph record
//				error	Returned error code ( see below )
//
//		Errors returned:	NU_NO_ERROR
//
// ========================================================================

void	SetWindowClip( 
				GraphPtr			graph, 
				SInt32			*error)
								
{		// SetGraphClip

	*error = NU_NO_ERROR;
				
	#if defined multispec_mac
		SetPortWindowPort( graph->window );
		ClipRect (GetWindowPortBounds(graph->window, &gTempRect));
	#endif	// defined multispec_mac 
	 
	#if defined multispec_win
/*		RECT				windowRect;

		if (graph->graphViewCPtr != NULL)
			{
			graph->graphViewCPtr->GetClientRect( &windowRect );

			if (gMFC_Rgn.CreateRectRgn(windowRect.left,
											windowRect.top,
											windowRect.right,
											windowRect.bottom) )
				ClipRect ( (Rect*)&windowRect ); 
					
			gMFC_Rgn.DeleteObject();        

			}		// end "if (graph->graphViewCPtr != NULL)"
*/
	#endif	// defined multispec_win
	
}		// End SetGraphClip



void	UnlockVectorPointer ( 
				vector			*x )
		
{		// UnlockVectorPointer

	CheckAndUnlockHandle (x->baseHandle);
		
	x->basePtr = NULL;
			
}		// End UnlockVectorPointer
