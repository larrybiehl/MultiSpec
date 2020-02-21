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
//	File:						WDrawObjects.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/04/2018
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMDrawObject and CMOutlineArea classes.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "SDisplay_class.h"
#include "SImageWindow_class.h"

#include "WDrawObjects.h"
#include "WImageView.h" 
#include "WTools.h"

Rect									CMDrawObject::s_viewRect;



CMDrawObject::CMDrawObject ()

{                     
	m_document = NULL; 
	m_drawFlag = FALSE; 

	m_logpen.lopnStyle = PS_DOT;  
	m_logpen.lopnWidth.x = 1;  
	m_logpen.lopnWidth.y = 1; 
	m_logpen.lopnColor = RGB (0, 0, 0); 

	m_logbrush.lbStyle = BS_HOLLOW; 
	m_logbrush.lbColor = RGB (255, 255, 255); 
	m_logbrush.lbHatch = HS_HORIZONTAL; 

}	// end "CMDrawObject"



CMDrawObject::CMDrawObject (
				const CRect& 						position)

{         
	m_position = position;  
	m_document = NULL; 
	m_drawFlag = FALSE; 

	m_logpen.lopnStyle = PS_DOT;
	m_logpen.lopnWidth.x = 1;  
	m_logpen.lopnWidth.y = 1; 
	m_logpen.lopnColor = RGB (0, 0, 0); 

	m_logbrush.lbStyle = BS_HOLLOW; 
	m_logbrush.lbColor = RGB (255, 255, 255);
	m_logbrush.lbHatch = HS_HORIZONTAL; 

}	// end "CMDrawObject"  



CMDrawObject::~CMDrawObject ()

{

}	// end "~CMDrawObject"


#ifdef _DEBUG   
void CMDrawObject::AssertValid ()

{   
	ASSERT (m_position.left <= m_position.right);
	ASSERT (m_position.bottom <= m_position.top);

}	// end "AssertValid"
#endif


void CMDrawObject::Draw (
				CDC* 									pDC,
				CMImageView*						imageViewCPtr)

{

}	// end "CMDrawObject::Draw"



void CMDrawObject::Invalidate ()

{             
	ASSERT_VALID (this);

}	// end "CMDrawObject::Invalidate"

    

void CMDrawObject::OnOpen (
				CMImageView* 						pView)

{ 
	ASSERT_VALID (this);

}	// end "OnOpen"


void CMDrawObject::Remove ()

{       
	delete this; 

}	// end "Remove"



//------------------------------------------------------------------------------------
//

CMOutlineArea::CMOutlineArea ()

{

}	// end "CMOutlineArea" 



CMOutlineArea::CMOutlineArea (
				const CRect& 						position)
		: CMDrawObject (position)

{

	ASSERT_VALID (this);

}	// end "CMOutlineArea"



CMOutlineArea::~CMOutlineArea ()

{                      

}	// end "~CMOutlineArea"  

  

void CMOutlineArea::Clear (
				CMImageView*						imageViewCPtr)
				
{
	ClearSelectionArea (imageViewCPtr);
	
}	// end "CMOutlineArea::Clear" 



void CMOutlineArea::Draw (
				CDC* 									pDC,
				CMImageView*						imageViewCPtr)

{   
	ASSERT_VALID (this);

	if (GetSelectionTypeCode (imageViewCPtr) == 0)
																								return;

	CBrush brush; 
	if (!brush.CreateBrushIndirect (&m_logbrush))   
																								return;

	CPen pen;  
	if (!pen.CreatePenIndirect (&m_logpen))   
																								return;

	CBrush* 		pOldBrush;
	CPen* 		pOldPen;

	pOldBrush = pDC->SelectObject (&brush);
	pOldPen = pDC->SelectObject (&pen);
	
	gCDCPointer = pDC;
	
	DrawSelectionArea (imageViewCPtr);
	
	gCDCPointer = NULL;

	pDC->SelectObject (pOldBrush);
	pDC->SelectObject (pOldPen);

}	// end "CMOutlineArea::Draw" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//  
//	Function name:		void CMOutlineArea::Invalidate  
//     
//	Software purpose:	This routine invalidates the current selection area. 
//  
//	Parameters in:
//         
//	Parameters out:
//
// Value Returned:	None  
//       
// Called By:
//  
//	Coded By:			Larry L. Biehl			Date: 07/13/1995
//	Revised By:			Larry L. Biehl			Date: 06/20/2002	

void  CMOutlineArea::Invalidate (
				CMImageView*						imageViewCPtr) 

{  
	LCToWindowUnitsVariables		lcToWindowUnitsVariables;
	DoubleRect							coordinateRectangle;
	    
	LongRect								lineColumnRectangle,
											selectionRect; 
								 
	Rect									tempRect; 
	
	SInt64								numberPixels;   

	SInt32								channel; 


	if (this == NULL)
																									return;                      

   if (GetSelectionTypeCode (imageViewCPtr) != 0)
   	{        
		::GetWindowClipRectangle (imageViewCPtr, kImageArea, &s_viewRect);

				// Set the global variables needed to convert from 
				// line-column units to window units.		
				
		SetChannelWindowVariables (kToImageWindow, imageViewCPtr, kNotCoreGraphics);

		Handle windowInfoH = GetWindowInfoHandle (imageViewCPtr);
				
		SetLCToWindowUnitVariables (windowInfoH,
												kToImageWindow, 
												FALSE,
												&lcToWindowUnitsVariables);

				// Get selection rectangle in current window coordinates.
		
		GetSelectionCoordinates (imageViewCPtr, 
											&lineColumnRectangle, 
											&coordinateRectangle,
											&numberPixels);
		
		ConvertLCRectToWinRect (&lineColumnRectangle, 
											&selectionRect,  
											&lcToWindowUnitsVariables); 

				// Now copy the long rect structure to a short rect 	
				// structure.  If everything is okay, the long rect
				// structure is not needed now.				
				// Add one to the right and bottom to make certain that	
				// the selection gets cleared - important for polygonal
				// selections.                       

				// Also adjust the units from logical window units to client 
				// units.		
                         
		LongPoint scrollOffset;   
      
      GetScrollOffset (windowInfoH, &scrollOffset);  

		tempRect.top = (int)(selectionRect.top - scrollOffset.v);  
		tempRect.left = (int)(selectionRect.left - scrollOffset.h); 
		tempRect.bottom = (int)(selectionRect.bottom + 1 - scrollOffset.v);
		tempRect.right = (int)(selectionRect.right + 1 - scrollOffset.h); 

		for (channel=gStartChannel; channel<gSideBySideChannels; channel++) 
			{                                                        
					// Note that the units for the rectangle for InvalidateRect  
					// is in client units.                  

			if (tempRect.right >= s_viewRect.left)    
				imageViewCPtr->InvalidateRect ((tagRECT*)&tempRect, FALSE);   

			tempRect.left += (int)gChannelWindowInterval;
			tempRect.right += (int)gChannelWindowInterval; 

			if (tempRect.left > s_viewRect.right)  
				break;                     

			}	// end "for (channel=gStartChannel; channel<..." 

		}	// end "if (GetSelectionTypeCode (imageViewCPtr) != 0)"

}	// end "CMOutlineArea::Invalidate"
