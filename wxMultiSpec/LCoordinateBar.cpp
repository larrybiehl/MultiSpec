//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LCoordinateBar.cpp : class implementation file
//	Class Definition:		LCoordinateBar.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			05/01/2018
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
			" LCoordinateBar: (): %s",
			gEndOfLine);
		ListString ((char*)&gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "LImageView.h"
#include "LCoordinateBar.h"

IMPLEMENT_DYNAMIC_CLASS (CMCoordinateBar, wxPanel)



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
	
	//wxFont current = GetFont ();
	//current.SetPointSize (9);
	//SetFont (current);
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
	SetupControls ();
	
}	// end "CMCoordinateBar"



CMCoordinateBar::~CMCoordinateBar ()

{

}	// end "~CMCoordinateBar()"




BEGIN_EVENT_TABLE (CMCoordinateBar, wxPanel)
    EVT_COMBOBOX (IDC_AreaUnitsCombo, CMCoordinateBar::OnSelendokAreaUnits)
    EVT_COMBOBOX (IDC_DisplayUnitsCombo, CMCoordinateBar::OnSelendokDisplayUnits)
	 EVT_COMBOBOX_DROPDOWN (IDC_DisplayUnitsCombo, CMCoordinateBar::OnDropDownDisplayUnits)
END_EVENT_TABLE ()

	
void CMCoordinateBar::InitialUpdate ()

{
	wxComboBox*							comboBoxPtr;


	comboBoxPtr = (wxComboBox*)FindWindow (IDC_DisplayUnitsCombo);
	SetCoordinateViewUnitsControl (gActiveImageWindowInfoH, (ControlHandle)comboBoxPtr);
		
	comboBoxPtr = (wxComboBox*)FindWindow (IDC_AreaUnitsCombo);
	SetCoordinateViewAreaUnitsControl (gActiveImageWindowInfoH,
													(ControlHandle)comboBoxPtr);
	
}	// end "InitialUpdate"



void CMCoordinateBar::OnDropDownDisplayUnits (
				wxCommandEvent&					event)
{
	SetUpCoordinateUnitsPopUpMenu (NULL, gActiveImageWindowInfoH, this);	

}	// end "OnDropDownDisplayUnits"



void CMCoordinateBar::OnSelendokAreaUnits (
				wxCommandEvent&					event)
{
	wxComboBox*							comboBoxPtr;


			// Get the actual display units code.

	comboBoxPtr = (wxComboBox*)FindWindow (IDC_AreaUnitsCombo);
	m_areaUnitsListSelection = comboBoxPtr->GetSelection ();
    
	SInt64 windowIndex64 =
					(SInt64)((int*)comboBoxPtr->GetClientData (m_areaUnitsListSelection));
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
	wxComboBox*							comboBoxPtr;


			// Get the actual display units code.

	comboBoxPtr = (wxComboBox*)FindWindow (IDC_DisplayUnitsCombo);
	m_displayUnitsListSelection = comboBoxPtr->GetSelection ();

	SInt64 windowIndex64 =
				(SInt64)((int*)comboBoxPtr->GetClientData (m_displayUnitsListSelection));
	m_displayUnitsCode = (SInt16)windowIndex64;
               
	if (m_displayUnitsCode != GetCoordinateViewUnits (gActiveImageWindowInfoH))
		{
		SetCoordinateViewUnits (gActiveImageWindowInfoH, m_displayUnitsCode);

		UpdateCoordinateView (gActiveImageWindowInfoH);

		}		// end "if (m_displayUnitsCode != GetCoordinateViewUnits ())"

}	// end "OnSelendokDisplayUnits"



void CMCoordinateBar::SetupControls ()

{
	wxBoxSizer* bSizer94;
	bSizer94 = new wxBoxSizer (wxHORIZONTAL);

	m_comboBox20 = new wxComboBox (this,
												IDC_DisplayUnitsCombo,
												wxT("Lines-Columns"),
												wxDefaultPosition,
												//wxDefaultSize,
												wxSize (160, -1),
												0,
												NULL,
												wxCB_READONLY);
	
	m_comboBox20->Append (wxT("Lines-Columns"));
	m_comboBox20->Append (wxT("Kilometers"));
	m_comboBox20->Append (wxT("Meters"));
	m_comboBox20->Append (wxT("Centimeters"));
	m_comboBox20->Append (wxT("Millimeters"));
	m_comboBox20->Append (wxT("Micrometers"));
	m_comboBox20->Append (wxT("Miles"));
	m_comboBox20->Append (wxT("Yards"));
	m_comboBox20->Append (wxT("Feet"));
	m_comboBox20->Append (wxT("Inches"));
   m_comboBox20->SetToolTip (wxT("Units of display"));
	bSizer94->Add (m_comboBox20,
						0,
						wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,
						5);

	wxBoxSizer* bSizer93;
	bSizer93 = new wxBoxSizer (wxVERTICAL);

	wxGridSizer* gSizer5;
	gSizer5 = new wxGridSizer (2, 1, 0, 0);

	m_staticText96 = new wxStaticText (this,
													IDC_LineSymbol,
													wxT("L"),
													wxDefaultPosition,
													wxDefaultSize,
													//wxSize (9,-1),
													0);
	m_staticText96->Wrap (-1);
	gSizer5->Add (m_staticText96, 0, wxALIGN_LEFT | wxTOP|wxBOTTOM, 2);

	m_staticText97 = new wxStaticText (this,
													IDC_ColumnSymbol,
													wxT("C"),
													wxDefaultPosition,
													wxDefaultSize,
													//wxSize (9,-1),
													0);
	m_staticText97->Wrap (-1);
	gSizer5->Add (m_staticText97, 0, wxALIGN_LEFT | wxTOP, 4);

	bSizer93->Add (gSizer5, 0, wxEXPAND);
	bSizer94->Add (bSizer93, 0, wxEXPAND);

	wxGridSizer* gSizer6;
	gSizer6 = new wxGridSizer (2, 1, 0, 0);

	m_staticText98 = new wxStaticText (this,
													IDC_CursorLine,
													wxT("     "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText98->Wrap (-1);
	gSizer6->Add (m_staticText98,
						0,
						wxST_NO_AUTORESIZE|wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL,
						2);

	m_staticText99 = new wxStaticText (this,
													IDC_CursorColumn,
													wxT("     "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText99->Wrap (-1);
	gSizer6->Add (m_staticText99,
						0,
						wxST_NO_AUTORESIZE|wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL,
						2);

	bSizer94->Add (gSizer6, 0, wxEXPAND, 2);

	m_staticline2 = new wxStaticLine (this,
												wxID_ANY,
												wxDefaultPosition,
												wxDefaultSize,
												wxLI_VERTICAL);
	bSizer94->Add (m_staticline2, 0, wxEXPAND | wxALL, 2);

	wxGridSizer* gSizer7;
	gSizer7 = new wxGridSizer (2, 1, 0, 0);

	m_staticText100 = new wxStaticText (this,
													IDC_SelectionLine,
													wxT("     "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText100->Wrap (-1);
	gSizer7->Add (m_staticText100, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL, 2);

	m_staticText101 = new wxStaticText (this,
													IDC_SelectionColumn,
													wxT("     "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText101->Wrap (-1);
	gSizer7->Add (m_staticText101, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL, 2);

	bSizer94->Add (gSizer7, 0, wxEXPAND, 2);

	m_staticText102 = new wxStaticText (this,
													wxID_ANY,
													wxT("    "),
													wxDefaultPosition,
													wxSize (10,-1),
													0);
	m_staticText102->Wrap (-1);
	bSizer94->Add (m_staticText102, 0, wxALL, 2);

	wxGridSizer* gSizer8;
	gSizer8 = new wxGridSizer (2, 1, 0, 0);

	m_staticText103 = new wxStaticText (this,
													IDC_NumberPixelsPrompt,
													wxT("Number pixels"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText103->Wrap (-1);
	gSizer8->Add (m_staticText103, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL, 2);

	m_staticText104 = new wxStaticText (this,
													IDC_NumberPixels,
													wxT(" "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText104->Wrap (-1);
	gSizer8->Add (m_staticText104, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL, 2);

	bSizer94->Add (gSizer8, 0, wxEXPAND, 2);

	m_comboBox21 = new wxComboBox (this,
												IDC_AreaUnitsCombo,
												wxT("Number pixels"),
												wxDefaultPosition,
												wxDefaultSize,
												0,
												NULL,
												wxCB_READONLY);
	m_comboBox21->Append (wxT("Number pixels"));
	m_comboBox21->Append (wxT("Sq. kilometers"));
	m_comboBox21->Append (wxT("Hectares"));
	m_comboBox21->Append (wxT("Sq. meters"));
	m_comboBox21->Append (wxT("Sq. centimeters"));
	m_comboBox21->Append (wxT("Sq. millimeters"));
	m_comboBox21->Append (wxT("Sq. micrometers"));
	m_comboBox21->Append (wxT("Sq. miles"));
	m_comboBox21->Append (wxT("Acres"));
	m_comboBox21->Append (wxT("Sq. yards"));
	m_comboBox21->Append (wxT("Sq. feet"));
	m_comboBox21->Append (wxT("Sq. inches"));
	bSizer94->Add (
		m_comboBox21,
		0,
		wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL,
		2);

	m_staticline3 = new wxStaticLine (this,
													wxID_ANY,
													wxDefaultPosition,
													wxDefaultSize,
													wxLI_VERTICAL);
	bSizer94->Add (m_staticline3, 0, wxEXPAND | wxALL, 2);

	wxBoxSizer* bSizer941;
	bSizer941 = new wxBoxSizer (wxVERTICAL);

	m_staticText104 = new wxStaticText (this,
													IDC_ScalePrompt,
													wxT("Scale"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText104->Wrap (-1);
	bSizer941->Add (m_staticText104, 0, wxALL, 2);

	m_staticText105 = new wxStaticText (this,
													IDC_Scale,
													wxT("1:24,000"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText105->Wrap (-1);
	bSizer941->Add (m_staticText105, 0, wxALL, 2);

	bSizer94->Add (bSizer941, 1, wxEXPAND, 2);

	this->SetSizer (bSizer94);
	this->Layout ();
	
}	// end "SetupControls"



void CMCoordinateBar::UpdateCoordinateView (
				Handle								windowInfoHandle)

{
	SetCoordinateViewLocationParameters (windowInfoHandle);

	UpdateSelectionCoordinates (windowInfoHandle);
	ShowSelection (windowInfoHandle);

}	// end "UpdateCoordinateView"
