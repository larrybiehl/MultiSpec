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
//	File:						xOneColumnDialog.cpp : class implementation file
//	Class Definition:		xOneColumnDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			02/16/2025
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMOneColDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "xOneColumnDialog.h"
#include "wx/valnum.h"

IMPLEMENT_CLASS (CMOneColDlg, CMDialog)

BEGIN_EVENT_TABLE (CMOneColDlg, CMDialog)
	EVT_BUTTON (IDC_AllSelected, CMOneColDlg::OnAllSelected)
	EVT_BUTTON (IDC_EnterNewRange, CMOneColDlg::OnEnterNewRange)
	EVT_BUTTON (IDC_NoneSelected, CMOneColDlg::OnNoneSelected)

	EVT_LISTBOX (IDC_List1, CMOneColDlg::OnSelchangeList1)

	EVT_TEXT (IDC_First, CMOneColDlg::OnEnterNewRange)
	EVT_TEXT (IDC_Interval, CMOneColDlg::OnEnterNewRange)
	EVT_TEXT (IDC_Last, CMOneColDlg::OnEnterNewRange)
END_EVENT_TABLE()



CMOneColDlg::CMOneColDlg (
				wxWindow* 							parent,
				wxWindowID 							id,
				const wxString& 					title,
				const wxPoint& 					pos,
				const wxSize& 						size,
				long 									style)
		: CMDialog (id, parent, title)

{
    		// Initialization of variables
	
	m_listStart = 1;
	m_listInterval = 1;
	m_listEnd = 1;
	m_selectionCount = wxT("1");

			// The m_indexStart variable is used to specify a 0 base or 1 base list.
			// I.E. the list starts at 0 or 1.

	m_currentSelection = 0;
	m_indexStart = 0;
	m_listType = 0;
	m_numberOutputItems = 0;
	m_selectedItemsPtr = NULL;
	m_numberInputVecItems = gImageFileInfoPtr->numberClasses;
	m_numberSelections = 0;
	m_minimumItemsRequired = 1;
	m_showListFlag = true;

			// Setting up dialog GUI

	m_initializedFlag = CMDialog::m_initializedFlag;

	CreateControls ();
	OnInitDialog ();
	
}	// end "CMOneColDlg"



CMOneColDlg::~CMOneColDlg (void)

{

}	// end "~CMOneColDlg"



void CMOneColDlg::CreateControls ()

{
	SetSizeHints (wxDefaultSize, wxDefaultSize);

   wxFont  font = GetFont();
   font.SetFamily (wxFONTFAMILY_TELETYPE);
   font.SetWeight (wxFONTWEIGHT_NORMAL);		// wxFONTWEIGHT_NORMAL

	wxBoxSizer* bVSizerMain = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizerm = new wxBoxSizer (wxHORIZONTAL);

	wxBoxSizer* bSizervl;
	bSizervl = new wxBoxSizer (wxVERTICAL);

	m_staticText9 = new wxStaticText (this,
													IDC_ListTitle,
													wxT("Item List"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText9->Wrap (-1);
   bSizervl->Add (
   				m_staticText9,
					wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxRIGHT|wxBOTTOM, 5));

	m_listBox1 = new wxListBox (this,
											IDC_List1,
											wxDefaultPosition,
											wxSize (300, 225),
											0,
											NULL,
											wxLB_MULTIPLE|wxLB_NEEDED_SB /*|wxLB_EXTENDED*/);
	m_listBox1->SetFont (font);
	
   bSizervl->Add (
				m_listBox1,
				wxSizerFlags(0).ReserveSpaceEvenIfHidden().Align(wxALIGN_CENTER).Border(wxTOP|wxRIGHT|wxBOTTOM, 5));

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText4 = new wxStaticText (this,
													IDC_SelectionCountLabel,
													wxT("Number Selected:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText4->Wrap (-1);
	bSizer15->Add (m_staticText4,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
							Align(wxALIGN_CENTER).Border(wxTOP|wxRIGHT, 5));

	m_staticText8 = new wxStaticText (this,
													IDC_SelectionCount,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText8->Wrap (-1);
	bSizer15->Add (m_staticText8,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxTOP|wxRIGHT, 5));

	bSizervl->Add (bSizer15, wxSizerFlags(0).Expand());

	bSizerm->Add (bSizervl, wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP,12));

	wxBoxSizer* bSizervr;
	bSizervr = new wxBoxSizer (wxVERTICAL);

	m_button2 = new wxButton (this,
										IDC_AllSelected,
										wxT("All"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
   SetUpToolTip (m_button2, IDS_ToolTip162);
	bSizervr->Add (
				m_button2,
				wxSizerFlags(0).ReserveSpaceEvenIfHidden().
						Align(wxALIGN_CENTER_HORIZONTAL).Border(wxLEFT|wxRIGHT|wxBOTTOM, 5));

	m_noneButton = new wxButton (this,
										IDC_NoneSelected,
										wxT("None"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
   SetUpToolTip (m_noneButton, IDS_ToolTip163);
	bSizervr->Add (m_noneButton,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
							Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL, 5));

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer (new wxStaticBox (this,
																		IDC_RangeOutline,
																		wxT("Range-Interval Choice")),
													wxVERTICAL);

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText5 = new wxStaticText (sbSizer1->GetStaticBox(),
													IDC_FirstLabel,
													wxT("First"),
													wxDefaultPosition,
													wxSize(45, -1),
													0);
	m_staticText5->Wrap (-1);
	bSizer11->Add (m_staticText5,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
							Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

	m_textCtrl2 = new wxTextCtrl (sbSizer1->GetStaticBox(),
											IDC_First,
											wxEmptyString,
											wxDefaultPosition,
											wxSize(100, -1),
											0);
	m_textCtrl2->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip (m_textCtrl2, IDS_ToolTip164);
	bSizer11->Add (m_textCtrl2,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	sbSizer1->Add (bSizer11, wxSizerFlags(0).Expand());

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText6 = new wxStaticText (sbSizer1->GetStaticBox(),
													IDC_LastLabel,
													wxT("Last"),
													wxDefaultPosition,
													wxSize(45, -1),
													0);
	m_staticText6->Wrap (-1);
	bSizer13->Add (m_staticText6,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
							Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

	m_textCtrl3 = new wxTextCtrl (sbSizer1->GetStaticBox(),
											IDC_Last,
											wxEmptyString,
											wxDefaultPosition,
											wxSize(100, -1),
											0);
	m_textCtrl3->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip (m_textCtrl3, IDS_ToolTip165);
	bSizer13->Add (m_textCtrl3,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	sbSizer1->Add (bSizer13, wxSizerFlags(0).Expand());

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText7 = new wxStaticText (sbSizer1->GetStaticBox(),
													IDC_IntervalLabel,
													wxT("Interval"),
													wxDefaultPosition,
													wxSize(45, -1),
													0);
	m_staticText7->Wrap (-1);
	bSizer14->Add (m_staticText7,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
							Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

	m_textCtrl4 = new wxTextCtrl (sbSizer1->GetStaticBox(),
											IDC_Interval,
											wxEmptyString,
											wxDefaultPosition,
											wxSize(100, -1),
											0);
	m_textCtrl4->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip (m_textCtrl4, IDS_ToolTip166);
	bSizer14->Add (m_textCtrl4,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	sbSizer1->Add (bSizer14, wxSizerFlags(0).Expand());

	bSizervr->Add (sbSizer1,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Expand().
																	Border(wxLEFT|wxTOP|wxBOTTOM, 5));

	m_enterNewRangebutton = new wxButton (this,
										IDC_EnterNewRange,
										wxT("Enter New Range"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
   SetUpToolTip (m_enterNewRangebutton, IDS_ToolTip167);
	bSizervr->Add (m_enterNewRangebutton,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
											Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL, 5));
	
	bSizerm->Add (bSizervr, wxSizerFlags(0).Expand().Border(wxTOP|wxRIGHT, 12));
	
	bVSizerMain->Add (bSizerm, wxSizerFlags(0));
	
	wxSizer* standardButtonSizer = CreateButtonSizer (wxOK | wxCANCEL);
	#if defined multispec_wxmac
		bVSizerMain->Add (standardButtonSizer, wxSizerFlags(0).Right());
	#else
		bVSizerMain->Add (standardButtonSizer, wxSizerFlags(0).Right().
																			Border(wxTOP|wxBOTTOM, 12));
	#endif

	SetSizer (bVSizerMain);
	Layout ();
	Fit ();
	 
}	// end "CreateControls"



void CMOneColDlg::OnAllSelected (
				wxCommandEvent& 					event)

{
	wxListBox*							wxListBoxPtr;
	UInt16								item;


	wxListBoxPtr = (wxListBox*)FindWindow(IDC_List1);
	if (wxListBoxPtr != NULL && wxListBoxPtr->GetCount () > 0)
		{
		for (item=0; item<m_numberInputVecItems; item++)
			wxListBoxPtr->Select(item);
		
		}	// end "if (wxListBoxPtr != NULL && wxListBoxPtr->GetCount () > 0)"
		
	else if (!m_showListFlag)
		{
				// List not being show because it is too long;
				// Use all to adjust the range choice to be all channels

		m_listStart = 1;
		m_listInterval = 1;
		m_listEnd = (UInt16)m_numberInputVecItems;
			
		TransferDataToWindow ();
				
		}	// else if (!m_showListFlag)

	m_numberSelections = (UInt16)m_numberInputVecItems;

	UpdateNumberOfSelections ();

}	// end "OnAllSelected"



void CMOneColDlg::OnEnterNewRange (
				wxCommandEvent& 					event)

{
    TransferDataFromWindow ();
	
	
	if (m_listStart >= 1 && m_listStart <= m_numberInputVecItems &&
			m_listEnd >= 1 && m_listEnd <= m_numberInputVecItems &&
				m_listInterval >= 1 && m_listInterval <= m_numberInputVecItems)
    	m_numberSelections = UpdateOneColListSelections ((UInt16)m_numberInputVecItems,
																			m_listStart,
																			m_listEnd,
																			m_listInterval);

    UpdateNumberOfSelections ();

}	// end "OnEnterNewRange"



bool CMOneColDlg::OnInitDialog (void)

{
			// Initialize the channel range text edit items.

	m_listStart = 1;
	m_listInterval = 1;
	m_listEnd = (UInt16)m_numberInputVecItems;

			// Now set text validators
	
	wxIntegerValidator<unsigned long int> vlinestart (&m_listStart);
	vlinestart.SetRange (1, m_numberInputVecItems);
	
	wxTextCtrl* firstctrl = (wxTextCtrl*)FindWindow (IDC_First);
	firstctrl->SetValidator (vlinestart);
	
	wxTextCtrl* lastctrl = (wxTextCtrl*)FindWindow (IDC_Last);
	lastctrl->SetValidator (vlinestart);
	
	wxTextCtrl* intervalctrl = (wxTextCtrl*)FindWindow (IDC_Interval);
	intervalctrl->SetValidator (vlinestart);

	return true;

}	// end "OnInitDialog"



void CMOneColDlg::OnNoneSelected (
				wxCommandEvent& 					event)

{
	wxListBox*							wxListBoxPtr;
	UInt16 								item;


	wxListBoxPtr = (wxListBox*)FindWindow(IDC_List1);
	if (wxListBoxPtr != NULL && wxListBoxPtr->GetCount() > 0)
		{
		for (item = 0; item < m_numberInputVecItems; item++)
			wxListBoxPtr->Deselect (item);
		
		}	// end "if (wxListBoxPtr != NULL && wxListBoxPtr->GetCount() > 0)"

	m_numberSelections = 0;

	UpdateNumberOfSelections ();

}	// end "OnNoneSelected"



void CMOneColDlg::OnOK ()

{
			// Get items to use.	    

	if (m_listType == kSelectItemsList || m_listType == kSelectPCList)
		{
		wxListBox*		wxListBoxPtr;
		UInt16 			index = 0;
		UInt16 			item;

		wxListBoxPtr = (wxListBox*)FindWindow (IDC_List1);
		if (wxListBoxPtr != NULL && wxListBoxPtr->GetCount() > 0)
			{
			for (item=0; item<m_numberInputVecItems; item++)
				{
				if (wxListBoxPtr->IsSelected (item))
					{
					m_selectedItemsPtr[index] = item + m_indexStart;
					index++;

					}	// end "if (wxListBoxPtr->IsSelected (item))"

				}	// end "for (item=0; item<m_numberInputVecItems; item++)"

			m_numberSelections = index;
			
			}	// end "if (wxListBoxPtr != NULL && wxListBoxPtr->GetCount() > 0)"
			
		else if (!m_showListFlag)
			{
					// The number of items for the list is too large;
					// use only the start, end, interval values
					
			for (item=m_listStart; item<=m_listEnd; item+=m_listInterval)
				{
				m_selectedItemsPtr[index] = item - 1;
				index++;

				}	// end "for (item=0; item<m_numberInputVecItems; item++)"
			
			}	// else if (!m_showListFlag)

		}	// end "if (m_listType == kSelectItemsList || ..."

}	// end "OnOK"



void CMOneColDlg::OnSelchangeList1 (
				wxCommandEvent& 					event)

{
    wxArrayInt 						selindex;
	
	
    m_numberSelections =
	 						((wxListBox*)FindWindow (IDC_List1))->GetSelections (selindex);

    UpdateNumberOfSelections ();

}	// end "OnSelchangeList1"



bool CMOneColDlg::TransferDataFromWindow ()

{
    wxTextCtrl* first = (wxTextCtrl*)FindWindow (IDC_First);
    m_listStart = wxAtoi (first->GetValue ());
	
    wxTextCtrl* interval = (wxTextCtrl*)FindWindow (IDC_Interval);
    m_listInterval = wxAtoi (interval->GetValue ());
	
    wxTextCtrl* last = (wxTextCtrl*)FindWindow (IDC_Last);
    m_listEnd = wxAtoi (last->GetValue ());
	
    OnOK ();
	
    return true;

}	// end "TransferDataFromWindow"



bool CMOneColDlg::TransferDataToWindow ()

{
    wxTextCtrl* firstt = (wxTextCtrl*)FindWindow(IDC_First);
    firstt->ChangeValue(wxString::Format (wxT("%i"), (int)m_listStart));
	
    wxTextCtrl* intervalt = (wxTextCtrl*)FindWindow(IDC_Interval);
    intervalt->ChangeValue(wxString::Format (wxT("%i"), (int)m_listInterval));
	 
    wxTextCtrl* lastt = (wxTextCtrl*)FindWindow(IDC_Last);
    lastt->ChangeValue(wxString::Format (wxT("%i"), (int)m_listEnd));
	
    return true;
	 
}	// end "TransferDataToWindow"



void CMOneColDlg::UpdateNumberOfSelections (void)

{
	if (m_listType != kItemsListOnly && m_listType != kSelectedItemsListOnly)
		{
				// Hilite OK button depending on number of selections.

		if (m_numberSelections >= m_minimumItemsRequired)
			SetDLogControlHilite (this, wxID_OK, 0);

		else	// m_numberSelections < m_minimumItemsRequired
			SetDLogControlHilite (this, wxID_OK, 255);

				// Set the number of selected items.

		::LoadDItemValue (this, IDC_SelectionCount, m_numberSelections);

    	}	// end "if (m_listType != kChannelsListOnly && ..."

}	// end "UpdateNumberOfSelections"
