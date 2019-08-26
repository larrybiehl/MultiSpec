///	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LDrawObjects.cpp : class implementation file
//	Class Definition:		LDrawObjects.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/29/2017
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMDrawObject class.
//
// Following is template for debugging
/*
	int numberChars = sprintf ((char*)gTextString3,
								 " LDrawObjects:: (): %s",
								 gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------
//
#include "SMultiSpec.h"

#include "LDrawObjects.h"
#include "LImageView.h"
#include "CImageWindow.h"
#include "CDisplay.h"
#include "LTools.h"

Rect CMDrawObject::s_viewRect;

CMDrawObject::CMDrawObject ()

{
    m_document = NULL;
    m_drawFlag = FALSE;

    m_logpen.lopnStyle = wxDOT;
    m_logpen.lopnWidth.x = 1;
    m_logpen.lopnWidth.y = 1;
    m_logpen.lopnColor = RGB (0, 0, 0);
	
    		// The pen is defined as BS_HOLLOW
    		// Hpefull it means transparent
	
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
    m_logpen.lopnColor = RGB(0, 0, 0);
    // The pen is defined as BS_HOLLOW
    // Hopefully it means transparent
    m_logbrush.lbStyle = wxTRANSPARENT;
    m_logbrush.lbColor = RGB(255, 255, 255);
    m_logbrush.lbHatch = 0;// Not needed

}	// end "CMDrawObject"



void CMDrawObject::Remove ()

{
    delete this;

}	// end "Remove"



void CMDrawObject::Draw (
        CDC* pDC,
        CMImageView* imageViewCPtr)
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
// CMOutlineArea   

CMOutlineArea::CMOutlineArea ()

{
    //	m_style = selection;
    //	m_type = none;
    //	m_drawFlag = FALSE;

    //	m_selectInfoHandle = NULL;

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



//------------------------------------------------------------------------------------
//					 Copyright (1988-2019)
//           (c) Purdue Research Foundation
//						All rights reserved.
//
//	Function name:		void CMOutlineArea::Clear
//
//	Software purpose:	This routine clears the selection area.  The
//							selection area type flag is cleared and the
//							outlined selected area is invalidated in the
//							image window.
//
//	Parameters in
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
//							GetSelectionRectangle in selectionArea.
//							AddFieldToProject in statistics.
//							StatisticsWControlEvent in statistics.c
//
//	Coded By:			Larry L. Biehl			Date: 08/11/1989
//	Revised By:			Larry L. Biehl			Date: 09/25/1995

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
	//pDC->SetPen(pen);
	//pDC->SetBrush(brush);
	//pDC->SetBrush (*wxTRANSPARENT_BRUSH);
	//pDC->SetPen (wxPen (*wxBLACK, 1, wxDOT));
	gCDCPointer = pDC;

	DrawSelectionArea (imageViewCPtr);
	/*
			// Set the member variables needed to convert from line-column
			// units to window units.

	::GetWindowClipRectangle (imageViewCPtr, kImageArea, &s_viewRect);

	SetChannelWindowVariables (imageViewCPtr);

	switch (m_type)
		{
		case rectangle:
			DrawRectangle(pDC, imageViewCPtr);
			break;

		case polygon:
			//DrawPolygon(pDC, imageViewCPtr);
			DrawSelectionPolygon (imageViewCPtr, selectionInfoPtr);
			gCDCPointer = NULL;
			break;

		}	// end "switch (m_Style)"
	*/
	gCDCPointer = NULL;

	pDC->SetBrush (pOldBrush);
	pDC->SetPen (pOldPen);

}	// end "CMOutlineArea::Draw"



//------------------------------------------------------------------------------------   
//								 Copyright (1988-2019)
//							(c) Purdue Research Foundation
//									All rights reserved. 
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
		
		//tempRect.top -= 2;
		//tempRect.left -= 2;

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

		}	// end "if (GetSelectionTypeCode (imageViewCPtr) != 0 )"

}	// end "Invalidate"

