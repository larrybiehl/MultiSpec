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
//	Revised By:				Larry L. Biehl			Date: 02/19/2025
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

#if defined multispec_wxwin
// String Defines for About Dialog Box 
#define AboutStaticText "MultiSpec\n Version 2025.02.19\n(64-bit Windows wx version)\n\n Laboratory for Applications of Remote Sensing\nPurdue University\nWest Lafayette, IN 47907\n\n Copyright \© 1988-2020\nPurdue Research Foundation\n\n Work leading to MultiSpec was funded in part by NASA Grants \n NAGW-925, NAGW-3924 and NAGW5-3975\n\n Supported by AmericaView (www.americaview.org)\n\n wxWidgets (3.2.2.1), GDAL (2.1.2), HDF5 (1.1.8), OpenJPEG (2.5.0) [jpeg2000], \nERDAS/Earth Resource Mapping ECW (2-3.3), and HDF4 (2r2) software libraries are used.\n\nChih-Chung Chang and Chih-Jen Lin LIBSVM: a library for support vector machines is also used.\nACM Transactions on Intelligent Systems and Technology, 2:27:1--23:27, 2011.\nSoftware and information available at www.csie.ntu.edu.tw/~cjlin/libsvm/."

//#define AboutImagePath "Resources\\mspec.xpm"
#endif


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
