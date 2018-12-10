//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
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
//	System:					Linux Operating System
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
			
	//wxImage::AddHandler(wxICOHandler);
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
	
	//m_staticText1->SetFont (wxFont (8, 70, 90, 90, false, wxEmptyString));
	/*
	#if defined multispec_wxmac
		int fontSize = 12;
	#else
		int fontSize = 9;
	#endif
	*/
	m_staticText1->SetFont (wxFont (gFontSize,
												wxFONTFAMILY_DEFAULT,
												wxFONTSTYLE_NORMAL,
												wxFONTWEIGHT_NORMAL,
												false,
												wxEmptyString));
	
	//bSizer->Add (m_staticText1, 0, wxALL, 5);
	bSizer->Add (m_staticText1, wxSizerFlags(0).Border(wxALL, 5));
	
	m_staticline1 = new wxStaticLine (
						this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	//bSizer->Add (m_staticline1, 0, wxEXPAND | wxALL, 5);
	bSizer->Add (m_staticline1, wxSizerFlags(0).Expand().Border(wxALL, 5));
	
	m_staticText3 = new wxStaticText (
						this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_staticText3->Wrap (-1);
	//bSizer->Add (m_staticText3, 0, wxALL, 5);
	bSizer->Add (m_staticText3, wxSizerFlags(0).Border(wxALL, 5));
	/*
	m_button = new wxButton (
								this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer->Add (m_button, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
	*/
	wxSizer* standardButtonSizer = CreateButtonSizer (wxOK);
	bSizer->Add (standardButtonSizer, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
	
	this->SetSizer (bSizer);
	this->Layout ();
	bSizer->Fit (this);
	
	this->Centre (wxBOTH);
	
}	// end "LAbout"



LAbout::~LAbout ()

{
}

