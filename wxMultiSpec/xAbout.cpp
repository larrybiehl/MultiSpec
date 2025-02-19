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
//	File:						xAbout.cpp : class implementation file
//	Class Definition:		xAbout.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			04/26/2023
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the CMAbout
//								class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xMultiSpec.h"

#include "xAbout.h"
#ifdef multispec_wxmac
	#include "MultiSpec_wxmac.rc"
	#include "mspec.xpm"
#endif
#ifdef multispec_wxlin
	#include "MultiSpec_wxlin.rc"
	#include "res/mspec.xpm"
#endif
#ifdef multispec_wxwin
	//#include "MultiSpec_wxwin.rc"
	#include "mspec.xpm"
#endif



CMAbout::CMAbout (
				wxWindow*							parent, 
				wxWindowID							id, 
				const wxString&					title, 
				const wxPoint&						pos, 
				const wxSize&						size, 
				long									style)
		: wxDialog (parent, id, title, pos, size, style)

{
   //wxSize         minSize;
   
   
   wxBitmap mspecbmp = wxBitmap (mspec_xpm);
   
   SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxBoxSizer* bSizer;
	bSizer = new wxBoxSizer (wxVERTICAL);
   
   //minSize = bSizer->GetMinSize ();
	
			// Temporarily adding handlers
			
	m_bitmap1 = new wxStaticBitmap (
						this, wxID_ANY, mspecbmp, wxDefaultPosition, wxDefaultSize, 0);
	bSizer->Add (
				m_bitmap1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
         // Note had to fix the size on 3/20/2023 because the window width had become
         // the entire width of the screen. Could not figure out how to fix. Probably
         // related to high dpi screens. Maybe fixed in a later version of wxWidgets.
         // by Larry Biehl
   
	m_staticText1 = new wxStaticText (this,
													wxID_ANY, 
													wxT(AboutStaticText), 
													wxDefaultPosition, 
													//wxDefaultSize,
                                       wxSize (600,400),
													wxALIGN_CENTRE);
	m_staticText1->Wrap (-1);
   
   //minSize = bSizer->GetMinSize ();
   //minSize = m_staticText1->GetMinSize ();
	
	m_staticText1->SetFont (wxFont (gFontSize,
												wxFONTFAMILY_DEFAULT,
												wxFONTSTYLE_NORMAL,
												wxFONTWEIGHT_NORMAL,
												false,
												wxEmptyString));
   //minSize = bSizer->GetMinSize ();
   //minSize = m_staticText1->GetMinSize ();
	
	bSizer->Add (m_staticText1, wxSizerFlags(1).Expand().Border(wxALL, 5));
   //minSize = bSizer->GetMinSize ();
	/*
	m_staticline1 = new wxStaticLine (this,
													wxID_ANY,
													wxDefaultPosition,
													wxDefaultSize,
													wxLI_HORIZONTAL);
	bSizer->Add (m_staticline1, wxSizerFlags(0).Expand().Border(wxALL, 5));
   //bSizer->Add (m_staticline1, wxSizerFlags(0).Border(wxALL, 5));
	
	m_staticText3 = new wxStaticText (this,
													wxID_ANY,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText3->Wrap (-1);
	bSizer->Add (m_staticText3, wxSizerFlags(0).Border(wxALL, 5));
   */
	wxSizer* standardButtonSizer = CreateButtonSizer (wxOK);
   bSizer->Add (standardButtonSizer, wxSizerFlags(0).Border(wxALL, 5).CentreHorizontal());
   
   //minSize = bSizer->GetMinSize ();
	
	SetSizerAndFit (bSizer);
	Centre (wxBOTH);
   
   //SetSizer (bSizer);
   //Layout ();
   //Fit ();
	
}	// end "class CMAbout"



CMAbout::CMAbout ()

{

}	// end "~CMAbout"

