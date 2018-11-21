//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WAbout.h : LAbout Definition
//	Implementation:		WAbout.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the LAbout class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2013
//	Revised By:				Larry L. Biehl			Date: 10/19/2018
//	
//------------------------------------------------------------------------------------

#ifndef __labout__
	#define __labout__
	
#include "LResource.h"

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

// String Defines for About Dialog Box
//#define AboutStaticText "MultiSpec Application for mygeohub.org\n August 29, 2018 \n\n (64-bit version) \n\n Copyright © 2015-2018\nPurdue Research Foundation\nAll Rights Reserved \n\n Work leading to MultiSpec was funded in part by NASA Grants \n NAGW-925, NAGW-3924 and NAGW5-3975\n\n and NSF Grant-#1261727 \n\n Supported by AmericaView (www.americaview.org)"

//#define AboutImagePath "res/mspec.xpm"

		// In Application OnInit() function, must call wxInitAllImageHandlers ()
		// Otherwise image loading fails.

class LAbout : public wxDialog
 
{
	private:
	
	protected:
		wxStaticBitmap*	m_bitmap1;
		wxStaticText*		m_staticText1;
		wxStaticLine*		m_staticline1;
		wxStaticText*		m_staticText3;
		wxButton*			m_button;
	
	public:
		LAbout (	
				wxWindow*			parent, 
				wxWindowID			id = IDD_ABOUTBOX, 
				const wxString&	title = wxT("About MultiSpec"), 
				const wxPoint&		pos = wxDefaultPosition, 
				const wxSize&		size = wxSize( -1,-1 ), 
				long					style = wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);
						
		~LAbout ();
	
};

#endif //__labout__
