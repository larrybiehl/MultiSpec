// LDrawObjects.cpp - implementation for drawing objects 
// 
// Revised by Larry Biehl on 11/28/2017
// 

#include "SMultiSpec.h"

#include "LDrawObjects.h"
#include "LImageView.h"
#include "CImageWindow.h"
#include "CDisplay.h"
#include "LTools.h"

//#include "SExternalGlobals.h"

// IMPLEMENT_SERIAL(CDrawObj, CObject, 0)  

Rect CMDrawObject::s_viewRect;

CMDrawObject::CMDrawObject()
 {
    m_document = NULL;
    m_drawFlag = FALSE;

    m_logpen.lopnStyle = wxDOT;
    m_logpen.lopnWidth.x = 1;
    m_logpen.lopnWidth.y = 1;
    m_logpen.lopnColor = RGB(0, 0, 0);
    // The pen is defined as BS_HOLLOW
    // Hpefull it means transparent
    m_logbrush.lbStyle = wxTRANSPARENT;
    m_logbrush.lbColor = RGB(255, 255, 255);
    m_logbrush.lbHatch = 0; // Not needed

} // end "CMDrawObject"

CMDrawObject::~CMDrawObject()
 {



} // end "~CMDrawObject"

CMDrawObject::CMDrawObject(const wxRect& position)
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

} // end "CMDrawObject"

void CMDrawObject::Remove()
{
    delete this;

} // end "Remove"

void CMDrawObject::Draw(
        CDC* pDC,
        CMImageView* imageViewCPtr)
{

} // end "Draw"

void CMDrawObject::Invalidate()
 {
    //ASSERT_VALID(this);

    //	m_document->UpdateAllViews(NULL, HINT_UPDATE_DRAWOBJ, this);

} // end "CMDrawObject::Invalidate"

void
CMDrawObject::OnOpen(CMImageView*)
 {
    //ASSERT_VALID(this);

} // end "CMDrawObject::OnOpen"



#ifdef _DEBUG   

void CMDrawObject::AssertValid()
 {
    ASSERT(m_position.left <= m_position.right);
    ASSERT(m_position.bottom <= m_position.top);

}
#endif  



//----------------------------------------------------------------------------
// CMOutlineArea   
// Variables used when converting between line-column units 
// and window units.
//SInt32				s_channelWindowInterval;

//SInt32				s_channelWindowOffset;

//SInt32				s_sideBySideChannels;

//SInt32				s_startChannel;

//SInt32				s_startChannelWindow;



// Variable containing the threshold value for which any rectangle
// selection in an image must be larger than to be considered a
// selected ares.  This will allow a user to click in an image window
// to select the window or to zoom in at that point and not consider
// it a rectangle selection.											

// SInt32		CMOutlineArea::s_ThresholdSize = 2; // gThresholdSize is used instead.

CMOutlineArea::CMOutlineArea()
 {
    //	m_style = selection;
    //	m_type = none;
    //	m_drawFlag = FALSE;

    //	m_selectInfoHandle = NULL;

} // end "CMOutlineArea"

CMOutlineArea::CMOutlineArea(const wxRect& position)
: CMDrawObject(position)
 {

   // ASSERT_VALID(this);

    //	m_selectInfoHandle = NULL;
    /*
            m_style = selection;

            if (gStatisticsMode==kStatNewFieldPolyMode)
                    m_type = polygon;
            else
                    m_type = rectangle;

            m_drawFlag = FALSE;
     */
} // end "CMOutlineArea"

CMOutlineArea::~CMOutlineArea()
 {

} // end "~CMOutlineArea"



//-----------------------------------------------------------------------------
//					 Copyright (1988-1998)
//                                  c Purdue Research Foundation
//					All rights reserved.
//
//	Function name:		void CMOutlineArea::Clear
//
//	Software purpose:	This routine clears the selection area.  The
//				selection area type flag is cleared and the
//				outlined selected area is invalidated in the
//				image window.
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
//	Coded By:			Larry L. Biehl			Date: 08/11/89
//	Revised By:			Larry L. Biehl			Date: 09/25/95			

void
CMOutlineArea::Clear(
        CMImageView* imageViewCPtr)
{
    //ClearSelectionArea(imageViewCPtr->GetImageWindowCPtr());
    ClearSelectionArea(imageViewCPtr);
} // end "CMOutlineArea::Clear"

void CMOutlineArea::Draw(CDC* pDC, CMImageView* imageViewCPtr)
{
   // ASSERT_VALID(this);

    if (GetSelectionTypeCode(imageViewCPtr) == 0)
        return;

    //wxBrush brush(wxColour(GetRValue(m_logbrush.lbColor),GetGValue(m_logbrush.lbColor),GetBValue(m_logbrush.lbColor)),m_logbrush.lbStyle);
    
    //wxPen pen(wxColour(GetRValue(m_logpen.lopnColor),GetGValue(m_logpen.lopnColor),GetBValue(m_logpen.lopnColor)),m_logpen.lopnWidth.x,m_logpen.lopnStyle);
    
    wxBrush pOldBrush;
    wxPen pOldPen;
    pOldBrush = pDC->GetBrush();
    pOldPen = pDC->GetPen();
    //pDC->SetPen(pen);
    //pDC->SetBrush(brush);
    pDC->SetBrush(*wxTRANSPARENT_BRUSH);
    pDC->SetPen(wxPen(*wxBLACK, 1, wxDOT));
    gCDCPointer = pDC;

    DrawSelectionArea(imageViewCPtr);
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
    //			DrawPolygon(pDC, imageViewCPtr);
                            DrawSelectionPolygon (imageViewCPtr, selectionInfoPtr);
                            gCDCPointer = NULL;
                            break;

                    }		// end "switch (m_Style)"
     */

    gCDCPointer = NULL;

    pDC->SetBrush(pOldBrush);
    pDC->SetPen(pOldPen);

} // end "CMOutlineArea::Draw"


/*
void 
CMOutlineArea::DrawPolygon(
                                CDC* 							pDC,
                                CMImageView*				imageViewCPtr)
				
{                    
	
        LongPoint					drawLPoint;
									
        LongRect						selectionRect;

        POINT							drawPoint;
	
        #ifdef _WIN32
                SCROLLINFO					scrollInfo;
        #endif
	
        HPFieldPointsPtr			selectionPointsPtr;

        Handle						windowInfoHandle;
	
        SInt32						savedChannelWindowOffset;
	
        UInt32						pointCount,
                                                                        pointCountLimit,
                                                                        pointIndex;

        Boolean						stopDrawFlag = TRUE;
	 
        SignedByte					selectionPointsStatus;
	

        windowInfoHandle = GetWindowInfoHandle(imageViewCPtr);

        ConvertLCRectToWinRect (windowInfoHandle,
                                                                &m_lineColumnRectangle,
                                                                &selectionRect,
                                                                FALSE);

        // Convert to short int rectangle structure

        CPoint scrollOffset;

        #ifndef	_WIN32
                scrollOffset = imageViewCPtr->GetScrollPosition();
        #else		// _WIN32
                imageViewCPtr->GetScrollInfo (SB_HORZ, &scrollInfo, SIF_POS);
                scrollOffset.x = scrollInfo.nPos;
                imageViewCPtr->GetScrollInfo (SB_VERT, &scrollInfo, SIF_POS);
                scrollOffset.y = scrollInfo.nPos;
        #endif	// !_WIN32

        s_viewRect.top += scrollOffset.y;
        s_viewRect.bottom += scrollOffset.y;
        s_viewRect.left += scrollOffset.x;
        s_viewRect.right += scrollOffset.x;
	
                        // Save the channel window offset so that it can be reset at the end.
			
        savedChannelWindowOffset = gChannelWindowOffset;

        selectionPointsPtr = (HPFieldPointsPtr)GetHandleStatusAndPointer (
                                                                                m_polygonCoordinatesHandle,
                                                                                &selectionPointsStatus,
                                                                                kNoMoveHi);
										
        pointCountLimit = 0;
        if (gProcessorCode == kPolygonSelectionProcessor)
                pointCountLimit = 1;
                
        if (selectionRect.top <= s_viewRect.bottom &&
                                                                        selectionRect.bottom >= s_viewRect.top)
                {
                for (SInt32 channel=gStartChannel; channel<gSideBySideChannels; channel++)
                        {
                        if (selectionRect.left <= s_viewRect.right &&
                                                                        selectionRect.right >= s_viewRect.left)
                                {
                                pointCount = m_numberPolyPoints;
                                pointIndex = 1;
                                ConvertLCToWinPoint ( windowInfoHandle,
                                                                                        (LongPoint*)&selectionPointsPtr[pointIndex++],
                                                                                        &drawLPoint,
                                                                                        FALSE,
                                                                                        FALSE);

                                pDC->MoveTo(drawPoint.x, drawPoint.y);
                                pDC->MoveTo( (int)drawLPoint.h, (int)drawLPoint.v );
                                pointIndex++;;
				
                                while ( pointCount > pointCountLimit )
                                        {
                                        if (pointCount == 1)
                                                pointIndex = 1;
						                                         
                                        ConvertLCToWinPoint ( windowInfoHandle,
                                                                                        (LongPoint*)&selectionPointsPtr[pointIndex++],
                                                                                        &drawLPoint,
                                                                                        FALSE,
                                                                                        FALSE);

                                        pDC->LineTo( (int)drawLPoint.h, (int)drawLPoint.v );
					
                                        pointIndex++;
                                        pointCount--;
						
                                        }  //end "while ( pointCount > pointCountLimit )"

                                }		// end "if (selectionRect.left <= s_viewRect.right && ..."
		   
                   if (gSideBySideChannels > 1)
                        {
                                selectionRect.left += gChannelWindowInterval;
                                selectionRect.right += gChannelWindowInterval;
                                gChannelWindowOffset += gChannelWindowInterval;
				
                                }		// end "if (gSideBySideChannels > 1)"
		
                        if (selectionRect.left > s_viewRect.right)
                                break;
				
                        }		// end "for (channel=gStartChannel; channel<..."
			 
                }     // end "if (selectionRect.top <= s_viewRect.bottom && ..."

        MHSetState (m_polygonCoordinatesHandle, selectionPointsStatus);
	
                        // Reset the channel window offset.
			
        gChannelWindowOffset = savedChannelWindowOffset;
 
}		// end "CMOutlineArea::DrawPolygon" 
 */

/*
void   
CMOutlineArea::DrawRectangle( 
                                CDC* 					pDC,
                                CMImageView*		imageViewCPtr)

{          
        #ifdef	_WIN32
                SCROLLINFO			scrollInfo;
        #endif	// _WIN32

        Handle					windowInfoH;

        LongRect					selectionRect;

        SInt32					channel;

                        // Get selection rectangle in current window coordinates.

        windowInfoH = GetWindowInfoHandle(imageViewCPtr);

        ConvertLCRectToWinRect (windowInfoH,
                                                                &m_lineColumnRectangle,
                                                                &selectionRect,
                                                                FALSE);

        CPoint scrollOffset;

        #ifndef	_WIN32
                scrollOffset = imageViewCPtr->GetScrollPosition();
        #else		// _WIN32
                imageViewCPtr->GetScrollInfo (SB_HORZ, &scrollInfo, SIF_POS);
                scrollOffset.x = scrollInfo.nPos;
                imageViewCPtr->GetScrollInfo (SB_VERT, &scrollInfo, SIF_POS);
                scrollOffset.y = scrollInfo.nPos;
        #endif	// !_WIN32

        s_viewRect.top += scrollOffset.y;
        s_viewRect.bottom += scrollOffset.y;
        s_viewRect.left += scrollOffset.x;
        s_viewRect.right += scrollOffset.x;

        if ( (selectionRect.top >= s_viewRect.top &&
                                                                        selectionRect.top < s_viewRect.bottom) ||
                        (selectionRect.bottom > s_viewRect.top &&
                                                                        selectionRect.bottom <= s_viewRect.bottom) )
                {
                for (channel=gStartChannel; channel<gSideBySideChannels; channel++)
                        {
                        if ( (selectionRect.left >= s_viewRect.left &&
                                                                        selectionRect.left < s_viewRect.right) ||
                                (selectionRect.right > s_viewRect.left &&
                                                                        selectionRect.right <= s_viewRect.right) )
                                {
                                pDC->Rectangle( (int)selectionRect.left,
                                                                (int)selectionRect.top,
                                                                (int)selectionRect.right,
                                                                (int)selectionRect.bottom);

                                }	// end "if (selectionRect.left >= s_viewRect.left ..."

                        selectionRect.left += gChannelWindowInterval;
                        selectionRect.right += gChannelWindowInterval;

                        if (selectionRect.left > s_viewRect.right)
                                break;

                        }		// end "for (channel=gStartChannel; channel<..."

                }		// end "if ( (selectionRect.top >= s_viewRect.top && ..."

}		// end "CMOutlineArea::DrawRectangle"   
 */

/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-1995)
//								� Purdue Research Foundation 
//									All rights reserved. 
//                            
//	Function name:		Boolean CMOutlineArea::GetLineColumnSelection 
//                                             
//	Software purpose:	This routine returns the selection rectangle. 
//							If no selection rectangle has been   
//							set up then the routine returns FALSE.  If the  
//							input clear selection flag is TRUE, then clear the  
//							flag indicating that a selection exists.  Otherwise 
//							do not clear the flag.  For example, the statistics 
//							processor will request that the selection exists flag  
// 						be set to FALSE after a field selection has been 
//							made so that the user can go directly to the next  
//							field selection.  If the use threshold flag is set 
//							then only return a selection area if it is larger than  
//							the threshold size.      
//                                       
//							If the adjustToBaseImage flag is TRUE then the line  
// 						and column values that are returned are adjusted to  
//							the line and column values relative to the base image. 
//                                                                          
//	Parameters in:		
//             
//	Parameters out:
//                
// Value Returned:	None   
//              
// Called By:	 
//   
//	Coded By:			Larry L. Biehl			Date: 09/08/88  
//	Revised By:			Larry L. Biehl			Date: 04/07/98	

Boolean 
CMOutlineArea::GetLineColumnSelection ( 
                                CMImageView*			imageViewCPtr,
                                LongRect*				selectionRectanglePtr,
                                Boolean					clearSelectionFlag,
                                Boolean					useThresholdFlag,
                                Boolean					adjustToBaseImageFlag)

{                        
//	Handle					fileInfoHandle; 

//	SInt32					offset; 

//	FileInfoPtr				fileInfoPtr; 

        Boolean					setUpFlag;
	

        if (this == NULL)
                                                                                                                                                                                return (FALSE);

        setUpFlag = FALSE;

        if (m_type == kRectangleType)
                {
 *selectionRectanglePtr = m_lineColumnRectangle;

                                // Adjust for any difference between start line and column in
                                // base image file and start line and column in associated image
                                // file if needed.
		       
                if (adjustToBaseImageFlag &&
                                                                ((WindowInfoPtr)*windowInfoH)->projectWindowFlag)
                        {
                        if ( !((WindowInfoPtr)*windowInfoH)->projectBaseImageFlag &&
                                                                                                                                        gProjectInfoPtr != NULL)
                                {
                                                // Get pointer to file specification.  We do not need
                                                // to lock it here since no other routines are called.

                                fileInfoHandle = ((WindowInfoPtr)*windowInfoH)->fileInfoHandle;
                                fileInfoPtr = (FileInfoPtr)*fileInfoHandle;

                                offset = fileInfoPtr->startColumn - gProjectInfoPtr->startColumn;
                                selectionRectangle->left += offset;
                                selectionRectangle->right += offset;

                                offset = fileInfoPtr->startLine - gProjectInfoPtr->startLine;
                                selectionRectangle->top += offset;
                                selectionRectangle->bottom += offset;

                                }		// end "if ( !((WindowInfoPtr)*windowInfoH)->..."

                        }		// end "if (adjustToBaseImageFlag && ..."
		                                         
                                // Reset the selection rectangle flag to indicate that it has
                                // been used, if requested.  If the selection is to be cleared
                                // then invalidate the rectangle.

                if (clearSelectionFlag)
                        Clear (imageViewCPtr);
//			ClearSelectionArea (imageViewCPtr);

                if (useThresholdFlag)
                        {
                                        // Check if selection is larger than the minimum threshold
                                        // size.

                        if ( (selectionRectanglePtr->bottom - selectionRectanglePtr->top >=
                                                                                                        s_ThresholdSize) ||
                                        (selectionRectanglePtr->right - selectionRectanglePtr->left >=
                                                                                                        s_ThresholdSize) )
                                setUpFlag = TRUE;

                        }		// end "if (useThresholdFlag)"

                else		// !useThresholdFlag
                        setUpFlag = TRUE;

                }		// end "if (m_type == kRectangleType)"

        return (setUpFlag);

}		// end "CMOutlineArea::GetLineColumnSelection"
 */

/*
Boolean 
CMOutlineArea::GetLineColumnSelection (          
                                LongRect*				selectionRectanglePtr)

{                                       
        Boolean					setUpFlag = FALSE;
	

        if (this != NULL && m_type != 0)
                {
 *selectionRectanglePtr = m_lineColumnRectangle;
		
                setUpFlag = TRUE;

                }		// end "if (m_type != 0)"
		
        return (setUpFlag);

}		// end "CMOutlineArea::GetLineColumnSelection"   
 */

/*
//----------------------------------------------------------------------------- 
//								 Copyright (1988-1998)              
//								� Purdue Research Foundation    
//									All rights reserved.   
//                                      
//	Function name:		Boolean CMOutlineArea::GetOffscreenSelection  
//                                                       
//	Software purpose:	This routine returns the offscreen rectangle within which  
//							the selection is to be drawn. If no selection rectangle has  
//							been set up then the routine returns FALSE.  
//                  
//	Parameters in:
//     
//	Parameters out:
// 
// Value Returned:	None    
//                   
// Called By:	           
//                          
//	Coded By:			Larry L. Biehl			Date: 07/16/95 
//	Revised By:			Larry L. Biehl			Date: 07/16/95	

Boolean        
CMOutlineArea::GetOffscreenSelection ( 
                                Rect*					selectionRectanglePtr)

{  
        Boolean					setUpFlag;
	

        setUpFlag = FALSE;

        if (m_type != none)
                {
 *selectionRectanglePtr = m_offscreenRectangle;
                setUpFlag = TRUE;

                }		// end "if (m_typeFlag != none)"

        return (setUpFlag);

}		// end "CMOutlineArea::GetOffscreenSelection"  
 */

/*
//-----------------------------------------------------------------------------  
//								 Copyright (1988-1998)      
//								� Purdue Research Foundation  
//									All rights reserved. 
//                                            
//	Function name:		Boolean CMOutlineArea::GetSelectionExistsFlag 
//                            
//	Software purpose:	This routine returns FALSE if no selection area has  
//							been set up and TRUE if it has.   
//          
//	Parameters in:	
//        
//	Parameters out:
//     
// Value Returned:	None   
// 
// Called By:	
//  
//	Coded By:			Larry L. Biehl			Date: 07/16/95 
//	Revised By:			Larry L. Biehl			Date: 07/16/95	

Boolean   
CMOutlineArea::GetSelectionExistsFlag (void) 

{ 
        return (m_type != none);

}		// end "CMOutlineArea::GetSelectionExistsFlag"  
 */

/*
//----------------------------------------------------------------------------- 
//								 Copyright (1988-1998)        
//								� Purdue Research Foundation  
//									All rights reserved.  
//                               
//	Function name:		SInt16 CMOutlineArea::GetTypeCode  
//                              
//	Software purpose:	This routine returns selection type code. 
//    
//	Parameters in:	
//       
//	Parameters out:
// 
// Value Returned:	None 
//       
// Called By:	
//         
//	Coded By:			Larry L. Biehl			Date: 07/16/95  
//	Revised By:			Larry L. Biehl			Date: 07/16/95	

SInt16 
CMOutlineArea::GetTypeCode (void) 

{  
        return (m_type);
}		// end "CMOutlineArea::GetTypeCode"  
 */


//-----------------------------------------------------------------------------   
//								 Copyright (1988-1998) 
//								� Purdue Research Foundation    
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
//	Coded By:			Larry L. Biehl			Date: 07/13/95 
//	Revised By:			Larry L. Biehl			Date: 06/20/2002	

void
CMOutlineArea::Invalidate(
        CMImageView* imageViewCPtr)
 {
    LCToWindowUnitsVariables lcToWindowUnitsVariables;
    DoubleRect coordinateRectangle;

    LongRect lineColumnRectangle,
            selectionRect;

    Rect tempRect;

    SInt32 channel;

    SInt64 numberPixels;

    if (this == NULL)
        return;

    if (GetSelectionTypeCode(imageViewCPtr) != 0) {
        ::GetWindowClipRectangle(imageViewCPtr, kImageArea, &s_viewRect);

        // Set the global variables needed to convert from
        // line-column units to window units.

        SetChannelWindowVariables(kToImageWindow, imageViewCPtr, kNotCoreGraphics);

        Handle windowInfoH = GetWindowInfoHandle(imageViewCPtr);

        SetLCToWindowUnitVariables(windowInfoH,
                kToImageWindow,
                FALSE,
                &lcToWindowUnitsVariables);

        // Get selection rectangle in current window coordinates.

        GetSelectionCoordinates(imageViewCPtr,
                &lineColumnRectangle,
                &coordinateRectangle,
                &numberPixels);

        ConvertLCRectToWinRect(&lineColumnRectangle,
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

        GetScrollOffset(windowInfoH, &scrollOffset);

        tempRect.top = (int) (selectionRect.top - scrollOffset.v);
        tempRect.left = (int) (selectionRect.left - scrollOffset.h);
        tempRect.bottom = (int) (selectionRect.bottom + 1 - scrollOffset.v);
        tempRect.right = (int) (selectionRect.right + 1 - scrollOffset.h);

        for (channel = gStartChannel; channel < gSideBySideChannels; channel++) {
            // Note that the units for the rectangle for InvalidateRect
            // is in client units.

            if (tempRect.right >= s_viewRect.left)
                imageViewCPtr->InvalidateRect((tagRECT*) & tempRect, FALSE);

            tempRect.left += (int) gChannelWindowInterval;
            tempRect.right += (int) gChannelWindowInterval;

            if (tempRect.left > s_viewRect.right)
                break;

        } // end "for (channel=gStartChannel; channel<..."

    } // end "if ( GetSelectionTypeCode (imageViewCPtr) != 0 )"

} // end "CMOutlineArea::Invalidate"

