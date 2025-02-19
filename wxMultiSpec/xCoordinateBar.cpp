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
//	File:						xCoordinateBar.cpp : class implementation file
//	Class Definition:		xCoordinateBar.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			05/01/2023
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMCoordinateBar class.
/* Template for debugging
		int numberChars = snprintf (
			256,
			(char*)&gTextString3,
			" xCoordinateBar: (): %s",
			gEndOfLine);
		ListString ((char*)&gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xImageView.h"
#include "xCoordinateBar.h"

IMPLEMENT_DYNAMIC_CLASS (CMCoordinateBar, wxPanel)



BEGIN_EVENT_TABLE (CMCoordinateBar, wxPanel)
	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_AreaUnitsCombo, CMCoordinateBar::OnSelendokAreaUnits)
	#endif
 	#if defined multispec_wxmac || defined multispec_wxwin
		EVT_CHOICE (IDC_AreaUnitsCombo, CMCoordinateBar::OnSelendokAreaUnits)
	#endif

   EVT_CHOICE (IDC_DisplayUnitsCombo, CMCoordinateBar::OnSelendokDisplayUnits)

	EVT_COMBOBOX_DROPDOWN (IDC_DisplayUnitsCombo, CMCoordinateBar::OnDropDownDisplayUnits)

	EVT_MOTION (CMCoordinateBar::OnMouseMove)
END_EVENT_TABLE ()



CMCoordinateBar::CMCoordinateBar ()

{
	m_areaUnitsCode = kNumberPixelsUnitsMenuItem;
	m_displayUnitsCode = kLineColumnUnitsMenuItem;
	m_displayUnitsListSelection = 0;
	m_areaUnitsListSelection = 0;
	
}	// end "CMCoordinateBar"



CMCoordinateBar::CMCoordinateBar (
				wxWindow*							parent)
		: wxPanel (parent)

{	
	m_areaUnitsCode = kNumberPixelsUnitsMenuItem;
	m_displayUnitsCode = kLineColumnUnitsMenuItem;
	m_displayUnitsListSelection = 0;
	m_areaUnitsListSelection = 0;
	/*
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
	*/
	SetupControls ();
	
}	// end "CMCoordinateBar"



CMCoordinateBar::~CMCoordinateBar ()

{

}	// end "~CMCoordinateBar()"


	
void CMCoordinateBar::InitialUpdate ()

{
	SetCoordinateViewUnitsControl (gActiveImageWindowInfoH,
												(ControlHandle)m_displayUnitsCtrl);
	
	SetCoordinateViewAreaUnitsControl (gActiveImageWindowInfoH,
													(ControlHandle)m_areaUnitsCtrl);
	
}	// end "InitialUpdate"



void CMCoordinateBar::OnDropDownDisplayUnits (
				wxCommandEvent&					event)

{
	//SetUpCoordinateUnitsPopUpMenu (NULL, gActiveImageWindowInfoH, this);

}	// end "OnDropDownDisplayUnits"



void CMCoordinateBar::OnMouseMove (
				wxMouseEvent&						event)

{
	if (gProcessorCode != kOpenImageFileProcessor && gPresentCursor != kArrow)
		{
		SetCursor (wxCursor (wxCURSOR_ARROW));
		wxSetCursor (wxNullCursor);
		gPresentCursor = kArrow;
		
		}	// end "if (gProcessorCode != kOpenImageFileProcessor && ..."
	
}	// end "OnMouseMove"



void CMCoordinateBar::OnSelendokAreaUnits (
				wxCommandEvent&					event)

{
			// Get the actual display units code.

	m_areaUnitsListSelection = m_areaUnitsCtrl->GetSelection ();
    
	SInt64 windowIndex64 =
			(SInt64)((int*)m_areaUnitsCtrl->GetClientData (m_areaUnitsListSelection));
	m_areaUnitsCode = (SInt16)windowIndex64;
    
	if (m_areaUnitsCode != GetCoordinateViewAreaUnits (gActiveImageWindowInfoH))
		{
		SetCoordinateViewAreaUnits (gActiveImageWindowInfoH, m_areaUnitsCode);

		UpdateCoordinateView (gActiveImageWindowInfoH);

		}	// end "if (m_displayUnitsCode != GetCoordinateViewUnits ())"

}	// end "OnSelendokAreaUnits"



void CMCoordinateBar::OnSelendokDisplayUnits (
				wxCommandEvent&					event)

{
			// Get the actual display units code.

	m_displayUnitsListSelection = m_displayUnitsCtrl->GetSelection ();

	SInt64 windowIndex64 = (SInt64)((int*)m_displayUnitsCtrl->
													GetClientData (m_displayUnitsListSelection));
	m_displayUnitsCode = (SInt16)windowIndex64;
               
	if (m_displayUnitsCode != GetCoordinateViewUnits (gActiveImageWindowInfoH))
		{
		SetCoordinateViewUnits (gActiveImageWindowInfoH, m_displayUnitsCode);

		UpdateCoordinateView (gActiveImageWindowInfoH);

		}	// end "if (m_displayUnitsCode != GetCoordinateViewUnits ())"

}	// end "OnSelendokDisplayUnits"



void CMCoordinateBar::SetupControls ()

{
   wxFont  font = GetFont();
   font.SetFamily (wxFONTFAMILY_TELETYPE);
   //font.SetFaceName ("Andale Mono");
   //font.MakeSmaller ();
   
	wxBoxSizer* bHSizer94;
	bHSizer94 = new wxBoxSizer (wxHORIZONTAL);
	
	m_displayUnitsCtrl = new wxChoice (this,
													IDC_DisplayUnitsCombo,
													wxDefaultPosition,
													wxSize (160, -1));
	m_displayUnitsCtrl->Append (wxT("Lines-Columns"));
	m_displayUnitsCtrl->Append (wxT("Kilometers"));
	m_displayUnitsCtrl->Append (wxT("Meters"));
	m_displayUnitsCtrl->Append (wxT("Centimeters"));
	m_displayUnitsCtrl->Append (wxT("Millimeters"));
	m_displayUnitsCtrl->Append (wxT("Micrometers"));
	m_displayUnitsCtrl->Append (wxT("Miles"));
	m_displayUnitsCtrl->Append (wxT("Yards"));
	m_displayUnitsCtrl->Append (wxT("Feet"));
	m_displayUnitsCtrl->Append (wxT("Inches"));
	
   m_displayUnitsCtrl->SetToolTip (wxT("Units of display"));
	bHSizer94->Add (m_displayUnitsCtrl,
						wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));

	wxBoxSizer* bVSizer93;
	bVSizer93 = new wxBoxSizer (wxVERTICAL);

	wxGridSizer* gGridSizer5 = new wxGridSizer (2, 1, 0, 0);

	m_staticText96 = new wxStaticText (this,
													IDC_LineSymbol,
													wxT("L"),
													wxDefaultPosition,
													wxDefaultSize,
													//wxSize (9,-1),
													0);
	m_staticText96->Wrap (-1);
   m_staticText96->SetFont (font);
	gGridSizer5->Add (m_staticText96, 0, wxALIGN_LEFT | wxTOP|wxBOTTOM, 2);

	m_staticText97 = new wxStaticText (this,
													IDC_ColumnSymbol,
													wxT("C"),
													wxDefaultPosition,
													wxDefaultSize,
													//wxSize (9,-1),
													0);
	m_staticText97->Wrap (-1);
   m_staticText97->SetFont (font);
	gGridSizer5->Add (m_staticText97,
							wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 2));

	bVSizer93->Add (gGridSizer5, wxSizerFlags(0).Expand());
	bHSizer94->Add (bVSizer93, wxSizerFlags(0).Expand());

	wxGridSizer* gGridSizer6 = new wxGridSizer (2, 1, 0, 0);

	m_staticText98 = new wxStaticText (this,
													IDC_CursorLine,
													wxT("     "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
													//wxST_NO_AUTORESIZE);
	m_staticText98->Wrap (-1);
   m_staticText98->SetFont (font);
	gGridSizer6->Add (m_staticText98,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 2));

	m_staticText99 = new wxStaticText (this,
													IDC_CursorColumn,
													wxT("     "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
													//wxST_NO_AUTORESIZE);
	m_staticText99->Wrap (-1);
   m_staticText99->SetFont (font);
	gGridSizer6->Add (m_staticText99,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 2));

	bHSizer94->Add (gGridSizer6, wxSizerFlags(0).Expand());

	m_staticline2 = new wxStaticLine (this,
												wxID_ANY,
												wxDefaultPosition,
												wxDefaultSize,
												wxLI_VERTICAL);
	
	bHSizer94->Add (m_staticline2, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT|wxBOTTOM, 3));

	wxGridSizer* gGridSizer7 = new wxGridSizer (2, 1, 0, 0);

	m_staticText100 = new wxStaticText (this,
													IDC_SelectionLine,
													wxT("     "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText100->Wrap (-1);
   m_staticText100->SetFont (font);
	gGridSizer7->Add (m_staticText100,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 2));

	m_staticText101 = new wxStaticText (this,
													IDC_SelectionColumn,
													wxT("     "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText101->Wrap (-1);
   m_staticText101->SetFont (font);
	gGridSizer7->Add (m_staticText101, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL, 2);

	bHSizer94->Add (gGridSizer7, wxSizerFlags(0).Expand());

	m_staticText102 = new wxStaticText (this,
													wxID_ANY,
													wxT("    "),
													wxDefaultPosition,
													wxSize (10,-1),
													0);
	m_staticText102->Wrap (-1);
   m_staticText102->SetFont (font);
	bHSizer94->Add (m_staticText102, 0, wxALL, 2);

	wxGridSizer* gSizer8;
	gSizer8 = new wxGridSizer (2, 1, 0, 0);
	#if defined multispec_wxlin
		m_areaUnitsCtrl = new wxComboBox (this,
														IDC_AreaUnitsCombo,
														wxT("Number pixels"),
														wxDefaultPosition,
														wxSize (-1, 19),
														0,
														NULL,
														wxCB_READONLY);
	#endif
	#if defined multispec_wxmac || defined multispec_wxwin
		m_areaUnitsCtrl = new wxChoice (this,
													IDC_AreaUnitsCombo,
													wxDefaultPosition,
													wxDefaultSize);
	#endif
	m_areaUnitsCtrl->Append (wxT("Number pixels"));
	m_areaUnitsCtrl->SetClientData (0, (void*)kNumberPixelsUnitsMenuItem);
	
	m_areaUnitsCtrl->Append (wxT("Unknown units"));
	m_areaUnitsCtrl->SetClientData (1, (void*)kAreaUnknownUnitsMenuItem);
	gSizer8->Add (m_areaUnitsCtrl,
		#if defined multispec_wxlin
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxTOP|wxBOTTOM, 1));
		#endif
		#if defined multispec_wxmac || defined multispec_wxwin
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxTOP|wxBOTTOM, 2));
		#endif
	
	m_staticText104 = new wxStaticText (this,
													IDC_NumberPixels,
													wxT(" "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText104->Wrap (-1);
   m_staticText104->SetFont (font);
	gSizer8->Add (m_staticText104,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxTOP, 2));

	bHSizer94->Add (gSizer8, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
	
	m_staticline3 = new wxStaticLine (this,
													wxID_ANY,
													wxDefaultPosition,
													wxDefaultSize,
													wxLI_VERTICAL);
	bHSizer94->Add (m_staticline3, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT|wxBOTTOM, 3));

	wxBoxSizer* bSizer941;
	bSizer941 = new wxBoxSizer (wxVERTICAL);

	m_staticText104 = new wxStaticText (this,
													IDC_ScalePrompt,
													wxT("Scale"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText104->Wrap (-1);
   m_staticText104->SetFont (font);
	bSizer941->Add (m_staticText104, wxSizerFlags(0).Border(wxALL,2));

	m_staticText105 = new wxStaticText (this,
													IDC_Scale,
													wxT("1:24,000"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText105->Wrap (-1);
   m_staticText105->SetFont (font);
	bSizer941->Add (m_staticText105, 0, wxALL, 2);

	bHSizer94->Add (bSizer941, wxSizerFlags(1).Expand());

	SetSizer (bHSizer94);
	Layout ();
	
}	// end "SetupControls"



void CMCoordinateBar::UpdateCoordinateView (
				Handle								windowInfoHandle)

{
	SetCoordinateViewLocationParameters (windowInfoHandle);

	UpdateSelectionCoordinates (windowInfoHandle);
	ShowSelection (windowInfoHandle);

}	// end "UpdateCoordinateView"
