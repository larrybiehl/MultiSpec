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
//	File:						xAbout.h : CMAbout Definition
//	Implementation:		xAbout.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMAbout class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2013
//	Revised By:				Larry L. Biehl			Date: 12/04/2019
//	
//------------------------------------------------------------------------------------

#pragma once
	
#include "xResource.h"

#include "wx/bitmap.h"
#include "wx/image.h"
#include "wx/icon.h"
#include "wx/statbmp.h"
#include "wx/gdicmn.h"
#include "wx/font.h"
#include "wx/colour.h"
#include "wx/settings.h"
#include "wx/string.h"
#include "wx/stattext.h"
#include "wx/statline.h"
#include "wx/button.h"
#include "wx/sizer.h"
#include "wx/dialog.h"

		// In Application OnInit() function, must call wxInitAllImageHandlers ()
		// Otherwise image loading fails.

class CMAbout : public wxDialog
{
	public:
		CMAbout (
				wxWindow*							parent,
				wxWindowID							id = IDD_ABOUTBOX,
				const wxString&					title = wxT("About MultiSpec"),
				const wxPoint&						pos = wxDefaultPosition,
				const wxSize&						size = wxSize (-1, -1),
				long									style =
															wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);
										
		CMAbout ();

	protected:
		wxButton*							m_button;
		wxStaticBitmap*					m_bitmap1;
		wxStaticLine*						m_staticline1;
	
		wxStaticText						*m_staticText1,
												*m_staticText3;
	
};	// end "class CMAbout"
