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
//	File:						xDrawObjects.cpp : class implementation file
//	Class Definition:		xDrawObjects.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			10/16/2019
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMDrawObject and CMOutlineArea classes.
//
// Following is template for debugging
/*
	int numberChars = sprintf ((char*)gTextString3,
								 " xDrawObjects:: (): %s",
								 gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xDrawObjects.h"
#include "xImageView.h"
#include "SImageWindow_class.h"
#include "SDisplay_class.h"
#include "xTools.h"

Rect CMDrawObject::s_viewRect;

CMDrawObject::CMDrawObject ()

{
	 m_document = NULL;
	 m_drawFlag = FALSE;

	 m_logpen.lopnStyle = wxDOT;
	 m_logpen.lopnWidth.x = 1;
	 m_logpen.lopnWidth.y = 1;
	 m_logpen.lopnColor = RGB (0, 0, 0);

	 m_logbrush.lbStyle = wxTRANSPARENT;
	 m_logbrush.lbColor = RGB (255, 255, 255);
	 m_logbrush.lbHatch = 0; // Not needed

}	// end "CMDrawObject"



CMDrawObject::~CMDrawObject ()

{

}	// end "~CMDrawObject"



CMDrawObject::CMDrawObject (
				const wxRect& 						position)

{
    m_position = position;
    m_document = NULL;
    m_drawFlag = FALSE;

    m_logpen.lopnStyle = wxDOT;
    m_logpen.lopnWidth.x = 1;
    m_logpen.lopnWidth.y = 1;
    m_logpen.lopnColor = RGB (0, 0, 0);
    m_logbrush.lbStyle = wxTRANSPARENT;
    m_logbrush.lbColor = RGB (255, 255, 255);
    m_logbrush.lbHatch = 0;	// Not needed

}	// end "CMDrawObject"



void CMDrawObject::Remove ()

{
    delete this;

}	// end "Remove"



void CMDrawObject::Draw (
				CDC* 									pDC,
        		CMImageView* 						imageViewCPtr)

{

}	// end "Draw"



void CMDrawObject::Invalidate ()

{
    //ASSERT_VALID(this);

    //	m_document->UpdateAllViews(NULL, HINT_UPDATE_DRAWOBJ, this);

}	// end "Invalidate"



void CMDrawObject::OnOpen (
				CMImageView*						imageViewCPtr)

{
    //ASSERT_VALID(this);

}	// end "CMDrawObject::OnOpen"



//------------------------------------------------------------------------------------
// CMOutlineArea Class

CMOutlineArea::CMOutlineArea ()

{
    //m_style = selection;
    //m_type = none;
    //m_drawFlag = FALSE;

    //m_selectInfoHandle = NULL;

}	// end "CMOutlineArea"



CMOutlineArea::CMOutlineArea (
				const wxRect& 						position)
		: CMDrawObject (position)

{
	//m_selectInfoHandle = NULL;
	/*
	m_style = selection;

	if (gStatisticsMode==kStatNewFieldPolyMode)
		m_type = polygon;
	else
		m_type = rectangle;

	m_drawFlag = FALSE;
	*/
}	// end "CMOutlineArea"



CMOutlineArea::~CMOutlineArea ()

{

}	// end "~CMOutlineArea"



void CMOutlineArea::Clear (
				CMImageView* 						imageViewCPtr)

{
    ClearSelectionArea (imageViewCPtr);
	
}	// end "CMOutlineArea::Clear"



void CMOutlineArea::Draw (
				CDC* 									pDC,
				CMImageView* 						imageViewCPtr)

{
	if (GetSelectionTypeCode (imageViewCPtr) == 0)
																									return;

	wxBrush pOldBrush;
	wxPen pOldPen;
	pOldBrush = pDC->GetBrush ();
	pOldPen = pDC->GetPen ();
	gCDCPointer = pDC;

	DrawSelectionArea (imageViewCPtr);

	gCDCPointer = NULL;

	pDC->SetBrush (pOldBrush);
	pDC->SetPen (pOldPen);

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
//	Coded By:			Abdur Rachman Maud	Date: ??/??/2012
//	Revised By:			Larry L. Biehl			Date: 03/29/2019

void CMOutlineArea::Invalidate (
        CMImageView* 							imageViewCPtr)

 {
	LCToWindowUnitsVariables 		lcToWindowUnitsVariables;
	DoubleRect 							coordinateRectangle;

	LongRect 							lineColumnRectangle,
            							selectionRect;

	Rect 									tempRect;

	SInt32 								channel;

	SInt64 								numberPixels;
 
 
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

		tempRect.top = (int)(selectionRect.top - scrollOffset.v - 2);
		tempRect.left = (int)(selectionRect.left - scrollOffset.h - 2);
		tempRect.bottom = (int)(selectionRect.bottom + 2 - scrollOffset.v);
		tempRect.right = (int)(selectionRect.right + 2 - scrollOffset.h);
		
		for (channel=gStartChannel; channel<gSideBySideChannels; channel++)
        	{
					// Note that the units for the rectangle for InvalidateRect
					// is in client units.

			if (tempRect.right >= s_viewRect.left)
				imageViewCPtr->InvalidateRect ((Rect*)&tempRect, FALSE);

			tempRect.left += (int)gChannelWindowInterval;
			tempRect.right += (int)gChannelWindowInterval;

			if (tempRect.left > s_viewRect.right)
				break;

        	}	// end "for (channel=gStartChannel; channel<..."

		}	// end "if (GetSelectionTypeCode (imageViewCPtr) != 0)"

}	// end "Invalidate"
