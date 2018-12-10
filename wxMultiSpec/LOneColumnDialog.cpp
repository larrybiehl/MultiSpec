// LOneColumnDialog.cpp
//
// Revised by Larry Biehl on 12/04/2018
//
//------------------------------------------------------------------------------------
#include "LOneColumnDialog.h"
#include "wx/valnum.h"

IMPLEMENT_CLASS (CMOneColDlg, CMDialog)

BEGIN_EVENT_TABLE (CMOneColDlg, CMDialog)
	EVT_BUTTON (IDC_AllSelected, CMOneColDlg::OnAllSelected)
	EVT_BUTTON (IDC_EnterNewRange, CMOneColDlg::OnEnterNewRange)
	EVT_BUTTON (IDC_NoneSelected, CMOneColDlg::OnNoneSelected)
	EVT_LISTBOX (IDC_List1, CMOneColDlg::OnSelchangeList1)
	EVT_TEXT (IDC_First, CMOneColDlg::OnChangeFirst)
	EVT_TEXT (IDC_Interval, CMOneColDlg::OnChangeInterval)
	EVT_TEXT (IDC_Last, CMOneColDlg::OnChangeLast)
END_EVENT_TABLE()



CMOneColDlg::CMOneColDlg (
				wxWindow* 							parent,
				wxWindowID 							id,
				const wxString& 					title,
				const wxPoint& 					pos,
				const wxSize& 						size,
				long 									style) :
		CMDialog (id, parent, title)//: wxDialog( parent, id, title, pos, size, style )
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

    		// Setting up dialog GUI
	
    m_initializedFlag = CMDialog::m_initializedFlag;
    //this->SetSizeHints( wxDefaultSize, wxDefaultSize );
    
    CreateControls();    
    OnInitDialog();
	
}



CMOneColDlg::~CMOneColDlg()

{
}



void CMOneColDlg::CheckValue (
				UInt16 itemNumber,
				UINT lastValue,
				UINT* lastValuePtr)

{
	UINT newValue;
	wxTextCtrl* cntrl = (wxTextCtrl*) FindWindow(itemNumber);
	wxString strval = cntrl->GetValue();
	newValue = wxAtoi(cntrl->GetValue());

	if (newValue <= 0 || newValue > m_numberInputVecItems)
		{
		//(*cntrl)<<lastValue;
		cntrl->ChangeValue(wxString::Format(wxT("%i"), (int) lastValue));

		wxMessageBox(wxT("Invalid Value ..."), wxT("Error"), wxOK | wxICON_ERROR | wxSTAY_ON_TOP, this);

		}	// end "if (m_listInterval <= 0 || ..."

	else	// newValue > 0 && newValue <= m_numberInputVecItems
		*lastValuePtr = newValue;
	
	wxCommandEvent ev (wxEVT_NULL);
	OnEnterNewRange (ev);

}	// end "CheckValue"



void CMOneColDlg::CreateControls ()

{
	this->SetSizeHints (wxDefaultSize, wxDefaultSize);

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
	wxFont font (gFontSize,
						wxFONTFAMILY_TELETYPE,
						wxFONTSTYLE_NORMAL,
						wxFONTWEIGHT_NORMAL,
						false,
						wxEmptyString);
   m_listBox1->SetFont (font);
   bSizervl->Add (
				m_listBox1,
				wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxTOP|wxRIGHT|wxBOTTOM, 5));

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
	bSizervr->Add (m_button2,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
							Align(wxALIGN_CENTER_HORIZONTAL).Border(wxLEFT|wxRIGHT|wxBOTTOM, 5));

	m_button3 = new wxButton (this,
										IDC_NoneSelected,
										wxT("None"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
   SetUpToolTip (m_button3, IDS_ToolTip163);
	bSizervr->Add (m_button3,
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
													wxT("First \t"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText5->Wrap (-1);
	bSizer11->Add (m_staticText5,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
							Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

	m_textCtrl2 = new wxTextCtrl (sbSizer1->GetStaticBox(),
											IDC_First,
											wxEmptyString,
											wxDefaultPosition,
											wxDefaultSize,
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
													wxT("Last \t"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText6->Wrap (-1);
	bSizer13->Add (m_staticText6,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
							Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

	m_textCtrl3 = new wxTextCtrl (sbSizer1->GetStaticBox(),
											IDC_Last,
											wxEmptyString,
											wxDefaultPosition,
											wxDefaultSize,
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
													wxT("Interval \t"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText7->Wrap (-1);
	bSizer14->Add (m_staticText7,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
							Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

	m_textCtrl4 = new wxTextCtrl (sbSizer1->GetStaticBox(),
											IDC_Interval,
											wxEmptyString,
											wxDefaultPosition,
											wxDefaultSize,
											0);
	m_textCtrl4->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip (m_textCtrl4, IDS_ToolTip166);
	bSizer14->Add (m_textCtrl4,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	sbSizer1->Add (bSizer14, wxSizerFlags(0).Expand());

	bSizervr->Add (sbSizer1,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Expand().Border(wxLEFT|wxTOP|wxBOTTOM, 5));

	m_button4 = new wxButton (this,
										IDC_EnterNewRange,
										wxT("Enter New Range"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
   SetUpToolTip (m_button4, IDS_ToolTip167);
	bSizervr->Add (m_button4,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
							Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL, 5));
	/*
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer (wxHORIZONTAL);

	m_button5 = new wxButton (this,
										wxID_CANCEL,
										wxT("Cancel"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
	bSizer10->Add (m_button5,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
							Border(wxLEFT|wxTOP|wxRIGHT, 6));

	m_button6 = new wxButton (this,
										wxID_OK,
										wxT("OK"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
	bSizer10->Add (m_button6, wxSizerFlags(0).Border(wxTOP, 6));

	bSizervr->Add (bSizer10, wxSizerFlags(0).Right());
	*/
	bSizerm->Add (bSizervr, wxSizerFlags(0).Expand().Border(wxTOP|wxRIGHT, 12));
	
	bVSizerMain->Add (bSizerm, wxSizerFlags(0));
	
	wxSizer* standardButtonSizer = CreateButtonSizer (wxOK | wxCANCEL);
	#if defined multispec_wxmac
		bVSizerMain->Add (standardButtonSizer, wxSizerFlags(0).Right());
	#else
		bVSizerMain->Add (standardButtonSizer, wxSizerFlags(0).Right().Border(wxTOP|wxBOTTOM, 12));
	#endif

	this->SetSizer (bVSizerMain);
	this->Layout ();
	 
}	// end "CreateControls"



void CMOneColDlg::OnAllSelected (
				wxCommandEvent& 							event)

{
	wxListBox*		wxListBoxPtr;
	UInt16			item;


	wxListBoxPtr = (wxListBox*)FindWindow(IDC_List1);
	if (wxListBoxPtr != NULL && wxListBoxPtr->GetCount() > 0)
		{
		for (item=0; item<m_numberInputVecItems; item++)
			wxListBoxPtr->Select(item);
		
		}	// end "if (wxListBoxPtr != NULL && wxListBoxPtr->GetCount() > 0)"

	m_numberSelections = (UInt16) m_numberInputVecItems;

	UpdateNumberOfSelections();

}	// end "OnAllSelected"


void CMOneColDlg::OnChangeFirst (
				wxCommandEvent& event)

{
    //CheckValue (IDC_First, m_listStart, &m_listStart); 

}	// end "OnChangeFirst"



void CMOneColDlg::OnChangeInterval (
				wxCommandEvent& event)

{
    //CheckValue (IDC_Interval, m_listInterval, &m_listInterval);

}	// end "OnChangeInterval"



void CMOneColDlg::OnChangeLast (
				wxCommandEvent& event)

{
    wxString newval = event.GetString();
    //CheckValue (IDC_Last, m_listEnd, &m_listEnd);

}	// end "OnChangeLast"



void CMOneColDlg::OnEnterNewRange (
				wxCommandEvent& event)

{
    TransferDataFromWindow();
    m_numberSelections = UpdateOneColListSelections((UInt16) m_numberInputVecItems,
            m_listStart,
            m_listEnd,
            m_listInterval);

    UpdateNumberOfSelections();

}	// end "OnEnterNewRange"



bool CMOneColDlg::OnInitDialog (void)

{
			// Initialize the channel range text edit items.

	m_listStart = 1;
	m_listInterval = 1;
	m_listEnd = (UInt16) m_numberInputVecItems;

			// Now set text validators
	
	wxTextCtrl* firstctrl = (wxTextCtrl *) FindWindow(IDC_First);
	wxTextCtrl* lastctrl = (wxTextCtrl *) FindWindow(IDC_Last);
	wxTextCtrl* intervalctrl = (wxTextCtrl *) FindWindow(IDC_Interval);
	wxIntegerValidator<unsigned long int> vlinestart(&m_listStart);
	vlinestart.SetRange(1, m_numberInputVecItems);
	firstctrl->SetValidator(vlinestart);
	lastctrl->SetValidator(vlinestart);
	intervalctrl->SetValidator(vlinestart);

	return true; // return TRUE  unless you set the focus to a control

}	// end "OnInitDialog"



void CMOneColDlg::OnNoneSelected (
				wxCommandEvent& event)

{
	wxListBox*		wxListBoxPtr;
	UInt16 item;


	wxListBoxPtr = (wxListBox*)FindWindow(IDC_List1);
	if (wxListBoxPtr != NULL && wxListBoxPtr->GetCount() > 0)
		{
		for (item = 0; item < m_numberInputVecItems; item++)
			wxListBoxPtr->Deselect(item);
		
		}	// end "if (wxListBoxPtr != NULL && wxListBoxPtr->GetCount() > 0)"

	m_numberSelections = 0;

	UpdateNumberOfSelections();

}	// end "OnNoneSelected"



//------------------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CMOneColDlg::OnOK
//
//	Software purpose:	The purpose of this routine is to fill the vector
//							with the selected items in the one column list.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 05/01/2015

void CMOneColDlg::OnOK ()

{
			// Get items to use.	    

	if (m_listType == kSelectItemsList || m_listType == kSelectPCList)
		{
		wxListBox*		wxListBoxPtr;
		UInt16 index = 0;
		UInt16 item;

		wxListBoxPtr = (wxListBox*)FindWindow(IDC_List1);
		if (wxListBoxPtr != NULL && wxListBoxPtr->GetCount() > 0)
			{
			for (item = 0; item < m_numberInputVecItems; item++)
				{
				if (wxListBoxPtr->IsSelected(item))
					{
					m_selectedItemsPtr[index] = item + m_indexStart;
					index++;

					}	// end "if ( LGetSelect (FALSE, &cell, ..."

				}	// end "for ( channel=0; channel<..."

			m_numberSelections = index;
			
			}	// end "if (wxListBoxPtr != NULL && wxListBoxPtr->GetCount() > 0)"

		}	// end "if ( m_listType == kSelectItemsList || ..."

}	// end "OnOK"



void CMOneColDlg::OnSelchangeList1 (
				wxCommandEvent& event)

{
    wxArrayInt selindex;
	
	
    m_numberSelections = ((wxListBox*)FindWindow(IDC_List1))->GetSelections(selindex);

    UpdateNumberOfSelections();

}	// end "OnSelchangeList1"


/*
void CMOneColDlg::OnShowWindow(bool bShow, UINT nStatus)
{
	CMDialog::OnShowWindow(bShow, nStatus);
	
	if (bShow)
		{
				// Set default text selection to first edit text item
			                                       
		SelectDialogItemText (this, IDC_First, 0, SInt16_MAX);
		
		}	// end "if (bShow)"
	
}	// end "OnShowWindow"
*/


bool CMOneColDlg::TransferDataFromWindow ()

{
    wxListBox* list = (wxListBox*)FindWindow (IDC_List1);
    wxTextCtrl* first = (wxTextCtrl*)FindWindow (IDC_First);
    wxTextCtrl* interval = (wxTextCtrl*)FindWindow (IDC_Interval);
    wxTextCtrl* last = (wxTextCtrl*)FindWindow (IDC_Last);
    m_listStart = wxAtoi (first->GetValue ());
    m_listInterval = wxAtoi (interval->GetValue ());
    m_listEnd = wxAtoi (last->GetValue ());
    OnOK ();
	
    return true;

}	// end "TransferDataFromWindow"


bool CMOneColDlg::TransferDataToWindow ()

{
    int item;
	
	
    wxListBox* list = (wxListBox*)FindWindow(IDC_List1);
    wxTextCtrl* firstt = (wxTextCtrl*)FindWindow(IDC_First);
    wxTextCtrl* intervalt = (wxTextCtrl*)FindWindow(IDC_Interval);
    wxTextCtrl* lastt = (wxTextCtrl*)FindWindow(IDC_Last);
    //(*first) << m_listStart;
    //(*interval) << m_listInterval;
    //(*last) << m_listEnd;
    firstt->ChangeValue(wxString::Format (wxT("%i"), (int)m_listStart));
    intervalt->ChangeValue(wxString::Format (wxT("%i"), (int)m_listInterval));
    lastt->ChangeValue(wxString::Format (wxT("%i"), (int)m_listEnd));
    int listcount = list->GetCount ();
    //for (item=0; item<m_listEnd; item++)
    //	list->Select(item);
	
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
			SetDLogControlHilite(this, wxID_OK, 255);

				// Set the number of selected items.

		::LoadDItemValue(this, IDC_SelectionCount, m_numberSelections);

    	}	// end "if (m_listType != kChannelsListOnly && ..."

}	// end "UpdateNumberOfSelections"
