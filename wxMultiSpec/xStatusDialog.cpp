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
//	File:						xStatusDialog.cpp : class implementation file
//	Class Definition:		xStatusDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			10/07/2019
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CShortStatusDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "xStatusDialog.h"

IMPLEMENT_DYNAMIC_CLASS (CShortStatusDlg, CMDialog)


BEGIN_EVENT_TABLE (CShortStatusDlg, CMDialog)
   EVT_BUTTON (IDC_CancelOperation, CShortStatusDlg::OnCancel)
	EVT_CHAR_HOOK (CShortStatusDlg::OnCharHook)
	EVT_CLOSE (CShortStatusDlg::OnClose)
	EVT_INIT_DIALOG (CShortStatusDlg::OnInitDialog)
	EVT_KEY_DOWN (CShortStatusDlg::OnKeyDown)
END_EVENT_TABLE ()



CShortStatusDlg::CShortStatusDlg ()

{

}	// end "CShortStatusDlg"



CShortStatusDlg::CShortStatusDlg (
				UInt16 								identifier,
				wxWindow* 							pParent /*=NULL*/)
		: CMDialog (CShortStatusDlg::IDD, pParent, wxT("Status"))

{
			 // Add this here so that the window is automatically
			 // created (i.e., displayed) when the C++ object
			 // is constructed.

	m_initializedFlag = CMDialog::m_initializedFlag;
	
	m_canceledCommandInitiatedFlag = FALSE;
	
	m_identifier = identifier;
	if (identifier == kShortStatusInfoID)
		CreateControl_Short ();
	
	else if (identifier == kUpdateStatsInfoID)
		CreateControl ();
	
	else     //kGraphicStatusDialogID
		CreateControl_Graphics ();
	
}	// end "CShortStatusDlg"



void CShortStatusDlg::CreateControl_Graphics ()

{
	SetSize (wxSize (480, 160));
	
	bSizer195 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer196;
	bSizer196 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText308 = new wxStaticText (this,
													IDC_Status2,
													wxT("MyLabel"),
													wxDefaultPosition,
													wxSize (350, -1),
													0);
	m_staticText308->Wrap (-1);
	bSizer196->Add (
			m_staticText308,
			wxSizerFlags(0).ReserveSpaceEvenIfHidden().
											Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL, 5));
	
	bSizer195->Add (bSizer196, 0, wxEXPAND, 5);
	
	wxBoxSizer* bSizer197;
	bSizer197 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText309 = new wxStaticText (this,
													IDC_Status3,
													wxT("Field"),
													wxDefaultPosition,
													wxSize (50, -1),
													0);
	m_staticText309->Wrap (-1);
	bSizer197->Add (m_staticText309, wxSizerFlags(0).Border(wxALL, 5));
	
	m_staticText310 = new wxStaticText (this,
													IDC_Status4,
													wxT("0"),
													wxDefaultPosition,
													wxSize (50, -1),
													wxALIGN_RIGHT);
	m_staticText310->Wrap (-1);
	bSizer197->Add (m_staticText310, 0, wxALL, 5);
	
	m_staticText311 = new wxStaticText (this,
													IDC_Status5,
													wxT("of"),
													wxDefaultPosition,
													wxSize (50, -1),
													wxALIGN_CENTRE);
	m_staticText311->Wrap (-1);
	bSizer197->Add (m_staticText311, 0, wxALL, 5);
	
	m_staticText312 = new wxStaticText (this,
													wxID_ANY,
													wxT(" "),
													wxDefaultPosition,
													wxSize (50, -1),
													wxALIGN_RIGHT);
	m_staticText312->Wrap (-1);
	bSizer197->Add (m_staticText312,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
	
	bSizer195->Add (bSizer197, 0, wxALIGN_CENTER, 5);
	
	wxBoxSizer* bSizer198;
	bSizer198 = new wxBoxSizer (wxVERTICAL);
	
	m_gauge3 = new wxGauge (this,
									IDC_Status7,
									100,
									wxDefaultPosition,
									wxSize (150, -1),
									wxGA_HORIZONTAL|wxGA_SMOOTH);
	m_gauge3->SetValue (0);
	bSizer198->Add (m_gauge3,
							wxSizerFlags(0).Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL, 5));
	
	bSizer195->Add (bSizer198, 0, wxEXPAND, 5);
	
	wxBoxSizer* bSizer199;
	bSizer199 = new wxBoxSizer (wxVERTICAL);

	m_button6 = new wxButton (this,
										IDC_CancelOperation,
										wxT("Cancel Operation"),
										wxDefaultPosition,
										wxDefaultSize,
										0);

	bSizer195->Add (m_button6, wxSizerFlags(0).Right().Border(wxRIGHT|wxBOTTOM, 12));
	
	SetSizer (bSizer195);
	Layout ();
	
	Centre (wxBOTH);
        
}	// end "CreateControl_Graphics"



void CShortStatusDlg::CreateControl_Short ()

{
	SetSizeHints (wxDefaultSize, wxDefaultSize);

	bSizer93 = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizer94;
	bSizer94 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText105 = new wxStaticText (this,
													IDC_ShortStatusText,
													wxT("Percent Complete"),
													wxDefaultPosition,
													wxSize (350, -1),
													0);
	m_staticText105->Wrap (-1);
	bSizer94->Add (m_staticText105, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL, 5);

	bSizer94->Add (50, 0, 0, wxEXPAND, 5);

	m_staticText106 = new wxStaticText (this,
													IDC_ShortStatusValue,
													wxT("100"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText106->Wrap (-1);
	bSizer94->Add (m_staticText106, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL, 5);

	bSizer93->Add (bSizer94, 0, wxALL | wxEXPAND, 5);

	m_button6 = new wxButton (this,
										IDC_CancelOperation,
										wxT("Cancel Operation"),
										wxDefaultPosition,
										wxDefaultSize,
										0);


	bSizer93->Add (m_button6, wxSizerFlags(0).Right().Border(wxRIGHT|wxBOTTOM, 12));

	SetSizer (bSizer93);
	Layout ();
	Centre (wxBOTH);
	
}	// end "CreateControl_Short"



void CShortStatusDlg::CreateControl ()

{
	SetSizeHints (wxDefaultSize, wxDefaultSize);

	bSizer35 = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer (wxVERTICAL);

	m_staticText39 = new wxStaticText (this,
													IDC_Status11,
													wxT("Status Line 1"),
													wxDefaultPosition,
													wxSize (350, -1),
													0);
	m_staticText39->Wrap (-1);
	bSizer36->Add (m_staticText39, 0, wxALL, 5);

	bSizer35->Add (bSizer36, wxSizerFlags(0).Border(wxALL, 5));

	wxBoxSizer* bSizer37;
	bSizer37 = new wxBoxSizer (wxVERTICAL);

	m_staticText40 = new wxStaticText (this,
													IDC_Status21,
													wxT("Status Line 2"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText40->Wrap (-1);
	bSizer37->Add (m_staticText40, 0, wxALL, 5);

	bSizer35->Add (bSizer37, wxSizerFlags(0).Border(wxALL, 5));

	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText41 = new wxStaticText (this,
													IDC_Status2,
													wxT("Class"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText41->Wrap (-1);
	bSizer39->Add (m_staticText41, 0, wxALL, 5);

	m_staticText42 = new wxStaticText (this,
													IDC_Status3,
													wxT("         "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText42->Wrap (-1);
	bSizer39->Add (m_staticText42, 0, wxALL, 5);

	m_staticText43 = new wxStaticText (this,
													IDC_Status4,
													wxT("     of"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText43->Wrap (-1);
	bSizer39->Add (m_staticText43, 0, wxALL, 5);

	m_staticText44 = new wxStaticText (this,
													IDC_Status5,
													wxT("         "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText44->Wrap (-1);
	bSizer39->Add (m_staticText44, 0, wxALL, 5);

	m_staticText45 = new wxStaticText (this,
													IDC_Status12,
													wxT(":      "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText45->Wrap (-1);
	bSizer39->Add (m_staticText45, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL, 5);

	m_staticText46 = new wxStaticText (this,
													IDC_Status6,
													wxT("              "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText46->Wrap (-1);
	bSizer39->Add (m_staticText46, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL, 5);


	bSizer38->Add (bSizer39, 0, wxALL, 5);

	wxBoxSizer* bSizer40;
	bSizer40 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText47 = new wxStaticText (this,
	 												IDC_Status7,
	 												wxT("Field"),
	 												wxDefaultPosition,
	 												wxDefaultSize,
	 												0);
	m_staticText47->Wrap (-1);
	bSizer40->Add (m_staticText47, 0, wxALL, 5);

	m_staticText48 = new wxStaticText (this,
													IDC_Status8,
													wxT("      "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText48->Wrap (-1);
	bSizer40->Add (m_staticText48, 0, wxALL, 5);

	m_staticText49 = new wxStaticText (this,
													IDC_Status9,
													wxT("     of"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText49->Wrap (-1);
	bSizer40->Add (m_staticText49, 0, wxALL, 5);

	m_staticText50 = new wxStaticText (this,
													IDC_Status10,
													wxT("      "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText50->Wrap (-1);
	bSizer40->Add (m_staticText50, 0, wxALL, 5);

	bSizer38->Add (bSizer40, 0, wxALL, 5);

	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText51 = new wxStaticText (this,
													IDC_Status15,
													wxT("Percent of pixels not changed"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText51->Wrap (-1);
	bSizer41->Add (m_staticText51, 0, wxALL, 5);

	m_staticText52 = new wxStaticText (this,
													IDC_Status16,
													wxT("      "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText52->Wrap (-1);
	bSizer41->Add (m_staticText52, 0, wxALL, 5);

	bSizer38->Add (bSizer41, 0, wxALL, 5);

	wxBoxSizer* bSizer45;
	bSizer45 = new wxBoxSizer (wxHORIZONTAL);

	wxBoxSizer* bSizer46;
	bSizer46 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText56 = new wxStaticText (this,
													IDC_Status17,
													wxT("Line"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText56->Wrap (-1);
	bSizer46->Add (m_staticText56, 0, wxALL, 5);

	m_staticText57 = new wxStaticText (this,
													IDC_Status18,
													wxT("      "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText57->Wrap (-1);
	bSizer46->Add (m_staticText57, 0, wxALL, 5);

	m_staticText58 = new wxStaticText (this,
													IDC_Status19,
													wxT("     of"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText58->Wrap (-1);
	bSizer46->Add (m_staticText58, 0, wxALL, 5);

	m_staticText59 = new wxStaticText (this,
													IDC_Status20,
													wxT("      "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText59->Wrap (-1);
	bSizer46->Add (m_staticText59, 0, wxALL, 5);

	bSizer45->Add (bSizer46, 1, wxALL, 5);

	bSizer38->Add (bSizer45, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer42;
	bSizer42 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText53 = new wxStaticText (this,
													IDC_Status13,
													wxT("Minutes until completion:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText53->Wrap (-1);
	bSizer42->Add (m_staticText53, 0, wxALL, 5);

	m_staticText54 = new wxStaticText (this,
													IDC_Status14,
													wxT("?????"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText54->Wrap (-1);
	bSizer42->Add (m_staticText54, 0, wxALL, 5);

	bSizer38->Add (bSizer42, 0, wxALL, 5);

	bSizer35->Add (bSizer38, wxSizerFlags(0).Border(wxALL, 5));
	
	m_button6 = new wxButton (this,
										IDC_CancelOperation,
										wxT("Cancel Operation"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
	bSizer35->Add (m_button6, wxSizerFlags(0).Right().Border(wxRIGHT|wxBOTTOM, 12));

	Layout ();
	Centre (wxBOTH);

}	// end "CreateControl"



void CShortStatusDlg::OnCancel (
				wxCommandEvent& 					event)

{
			// User cancelled the operation. Let CheckSomeEvents handle it.
	
	m_canceledCommandInitiatedFlag = TRUE;

}	// end "OnCancel"



void CShortStatusDlg::OnCharHook (
				wxKeyEvent& 						event)

{
	Boolean								escapeFlag = FALSE;
	
	
	if (event.GetKeyCode () == WXK_ESCAPE)
		escapeFlag = TRUE;
	
}	// end "OnCharHook"



void CShortStatusDlg::OnClose (
				wxCloseEvent& 						event)

{
	if (event.CanVeto ())
		{
		m_canceledCommandInitiatedFlag = TRUE;
		event.Veto ();
		
																									return;
		}
	
	event.Skip ();
	
}	// end "OnClose"



void CShortStatusDlg::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	wxDialog::OnInitDialog (event);

	PositionDialogWindow ();

	if (m_identifier == kShortStatusInfoID)
		SetSizerAndFit (bSizer93);

	else if (m_identifier == kUpdateStatsInfoID)
		SetSizerAndFit (bSizer35);

	else // kGraphicStatusDialogID
		SetSizerAndFit (bSizer195);
	
}	// end "OnInitDialog"



void CShortStatusDlg::OnKeyDown (
				wxKeyEvent& 						event)

{
	Boolean								escapeFlag = FALSE;


	if (event.GetKeyCode () == WXK_ESCAPE)
		{
		escapeFlag = TRUE;
		
		}	// end "if (event.GetKeyCode () == WXK_ESCAPE)"
	
}	// end "OnKeyDown"
