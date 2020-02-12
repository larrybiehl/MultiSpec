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
//	File:						xTitleBar.cpp : class implementation file
//	Class Definition:		xTitleBar.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			03/20/2019
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMCoordinateBar class.
/* Template for debugging
		int numberChars = sprintf (
			(char*)&gTextString3,
			" xTitleBar: (): %s",
			gEndOfLine);
		ListString ((char*)&gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xImageView.h"
#include "xTitleBar.h"

IMPLEMENT_DYNAMIC_CLASS (CMTitleBar, wxPanel)

BEGIN_EVENT_TABLE (CMTitleBar, wxPanel)
	EVT_PAINT (CMTitleBar::OnPaint)
END_EVENT_TABLE ()



CMTitleBar::CMTitleBar ()

{
	
}	// end "CMTitleBar"



CMTitleBar::CMTitleBar (
				wxWindow*							parent)
		: wxPanel (parent)

{	
	#if defined multispec_wxmac
		int fontSize = 12;
	#else
		int fontSize = 9;
	#endif
	SetFont (wxFont (fontSize,
							wxFONTFAMILY_TELETYPE,
							wxFONTSTYLE_NORMAL,
							wxFONTWEIGHT_NORMAL,
							false,
							wxEmptyString));
	
}	// end "CMTitleBar"



CMTitleBar::~CMTitleBar ()

{

}	// end "~CMTitleBar ()"


	
void CMTitleBar::InitialUpdate ()

{
	
}	// end "InitialUpdate"



void CMTitleBar::OnPaint (
				wxPaintEvent&						event)

{
	Rect									destinationRect;
	wxRect 								updateRect;
	

			// Shifts the device origin so we don't have to worry about the current
			// scroll position ourselves

	wxPaintDC dc (this);
	
			// Fills the dc with the current bg brush
			// Find Out where the window is scrolled to

	PrepareDC (dc);
	dc.Clear ();
	
			//	Sets the scale to be applied when converting from logical units to device
			// units

   dc.SetUserScale (1, 1);
	
			// Sets the bg brush used in Clear(). Default:wxTRANSPARENT_BRUSH

	dc.SetBackground (wxBrush (*wxWHITE));
	
	updateRect = GetClientRect ();
	
	destinationRect.top = updateRect.GetTop ();
	destinationRect.left = updateRect.GetLeft ();
	destinationRect.bottom = updateRect.GetBottom ();
	destinationRect.right = updateRect.GetRight ();

	wxPoint scrollOffset;
	m_view->m_Canvas->CalcUnscrolledPosition (0, 0, &scrollOffset.x, &scrollOffset.y);
	
	if (scrollOffset.x < 0)
		scrollOffset.x = 0;
	dc.SetDeviceOrigin (2-scrollOffset.x, 0);
	
	Handle windowInfoHandle = GetWindowInfoHandle (m_view);
	DrawSideBySideTitles (&dc,
									windowInfoHandle,
									m_view,
									&destinationRect,
									1);
	
}	// end "OnPaint"
