//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LAbout.cpp : class implementation file
//	Class Definition:		LAbout.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/07/2018
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the LAbout
//								class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "LAbout.h"
#ifdef multispec_wxmac
	#include "wxmac_Multispec.rc"
	#include "mspec.xpm"
#else
	#include "LMultispec.rc"
	#include "res/mspec.xpm"
#endif

///////////////////////////////////////////////////////////////////////////

LAbout::LAbout (
				wxWindow*							parent, 
				wxWindowID							id, 
				const wxString&					title, 
				const wxPoint&						pos, 
				const wxSize&						size, 
				long									style) : 
				wxDialog (parent, id, title, pos, size, style)

{
	this->SetSizeHints (wxDefaultSize, wxDefaultSize);
   wxBitmap mspecbmp = wxBitmap (mspec_xpm);
	
	wxBoxSizer* bSizer;
	bSizer = new wxBoxSizer (wxVERTICAL);
	
			// Temporarily adding handlers
			
	m_bitmap1 = new wxStaticBitmap (
							this, wxID_ANY, mspecbmp, wxDefaultPosition, wxDefaultSize, 0);
	bSizer->Add (
				m_bitmap1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	m_staticText1 = new wxStaticText (this, 
													wxID_ANY, 
													wxT(AboutStaticText), 
													wxDefaultPosition, 
													wxDefaultSize, 
													wxALIGN_CENTRE);
	m_staticText1->Wrap (-1);
	
	m_staticText1->SetFont (wxFont (gFontSize,
												wxFONTFAMILY_DEFAULT,
												wxFONTSTYLE_NORMAL,
												wxFONTWEIGHT_NORMAL,
												false,
												wxEmptyString));
	
	bSizer->Add (m_staticText1, wxSizerFlags(0).Border(wxALL, 5));
	
	m_staticline1 = new wxStaticLine (
						this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	bSizer->Add (m_staticline1, wxSizerFlags(0).Expand().Border(wxALL, 5));
	
	m_staticText3 = new wxStaticText (
						this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_staticText3->Wrap (-1);
	bSizer->Add (m_staticText3, wxSizerFlags(0).Border(wxALL, 5));

	wxSizer* standardButtonSizer = CreateButtonSizer (wxOK);
	bSizer->Add (standardButtonSizer, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
	
	SetSizerAndFit (bSizer);
	
	this->Centre (wxBOTH);
	
}	// end "LAbout"



LAbout::~LAbout ()

{
}

