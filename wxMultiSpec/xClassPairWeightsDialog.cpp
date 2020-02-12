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
//	File:						xClassPairWeightsDialog.cpp : class implementation file
//	Class Definition:		xClassPairWeightsDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			11/16/2018
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMClassPairWeightDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "xClassPairWeightsDialog.h"



BEGIN_EVENT_TABLE (CMClassPairWeightDlg, CMDialog)
	EVT_INIT_DIALOG (CMClassPairWeightDlg::OnInitDialog)
	EVT_BUTTON (IDC_HelpButton, CMClassPairWeightDlg::OnHelpButton)
	EVT_LISTBOX (IDC_ClassList, CMClassPairWeightDlg::OnSelchangeClassList)
	EVT_BUTTON (IDC_AddButton, CMClassPairWeightDlg::OnAddButton)
	EVT_TEXT (IDC_NewWeight, CMClassPairWeightDlg::OnChangeNewWeight)
	EVT_BUTTON (IDC_RemoveButton, CMClassPairWeightDlg::OnRemoveButton)
	EVT_LISTBOX (IDC_ClassPairWeightList, CMClassPairWeightDlg::OnSelchangeClassPairWeightList)
	EVT_TEXT (IDC_DefaultWeight, CMClassPairWeightDlg::OnChangeDefaultWeight)
END_EVENT_TABLE ()



CMClassPairWeightDlg::CMClassPairWeightDlg (
				wxWindow*							pParent,
				wxWindowID							id,
				const wxString&					title)
		: CMDialog (CMClassPairWeightDlg::IDD, pParent, title)
		 
{
   m_newClassPairWeight = 0;
   m_localDefaultClassPairWeight = 0;

   m_weightsListPtrPtr = NULL;

   m_numberOfClassesToUse = 0;

   m_selectedClassCell = -1;
   m_classPairWeightsSelection = -1;

   m_initializedFlag = CMDialog::m_initializedFlag;
   
   CreateControls ();
   SetSizerAndFit (bSizer240);
	
}	// end "CMClassPairWeightDlg"



void CMClassPairWeightDlg::CreateControls ()

{
   SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	bSizer240 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer241;
	bSizer241 = new wxBoxSizer (wxHORIZONTAL);
	
	wxBoxSizer* bSizer243;
	bSizer243 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText239 = new wxStaticText (this,
													wxID_ANY,
													wxT("Class List"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText239->Wrap (-1);
   SetUpToolTip (m_staticText239,IDS_ToolTip337);
	bSizer243->Add (m_staticText239, 0, wxALIGN_CENTER, 5);
	
	m_listBox2 = new wxListBox (this,
											IDC_ClassList,
											wxDefaultPosition,
											wxSize (160,200),
											0,
											NULL,
											wxLB_MULTIPLE);
	bSizer243->Add (m_listBox2, 0, wxALL, 5);
	
	
	bSizer241->Add (bSizer243, 0, wxEXPAND, 5);
	
	wxBoxSizer* bSizer244;
	bSizer244 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText240 = new wxStaticText (this,
													wxID_ANY,
													wxT("New Weight"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText240->Wrap (-1);
   SetUpToolTip (m_staticText240,IDS_ToolTip339);
	bSizer244->Add (m_staticText240, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_textCtrl127 = new wxTextCtrl (this,
												IDC_NewWeight,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (70,-1),
												0);
   SetUpToolTip (m_textCtrl127,IDS_ToolTip339);
	bSizer244->Add (m_textCtrl127, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_button51 = new wxButton (this,
										IDC_AddButton,
										wxT("Add"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
   SetUpToolTip (m_button51,IDS_ToolTip340);
	bSizer244->Add (m_button51, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_button52 = new wxButton (this,
										IDC_RemoveButton,
										wxT("Remove"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
   SetUpToolTip (m_button52,IDS_ToolTip341);
	bSizer244->Add (m_button52, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_button53 = new wxButton (this,
										IDC_HelpButton,
										wxT("Help..."),
										wxDefaultPosition,
										wxDefaultSize,
										0);
   SetUpToolTip (m_button53,IDS_ToolTip342);
	bSizer244->Add (m_button53, 0, wxALIGN_CENTER|wxALL, 5);
	
	
	bSizer241->Add (bSizer244, 0, wxTOP, 30);
	
	wxBoxSizer* bSizer245;
	bSizer245 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText241 = new wxStaticText (this,
													wxID_ANY,
													wxT("Wt.-Class Pairs"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText241->Wrap (-1);
   SetUpToolTip (m_staticText241,IDS_ToolTip343);
	bSizer245->Add (m_staticText241, 0, wxALIGN_CENTER, 5);
	
	m_listBox3 = new wxListBox (this,
											IDC_ClassPairWeightList,
											wxDefaultPosition,
											wxSize (180,200),
											0,
											NULL,
											wxLB_MULTIPLE);
	bSizer245->Add (m_listBox3, 0, wxALL, 5);
	
	
	bSizer241->Add (bSizer245, 0, wxEXPAND, 5);
	
	
	bSizer240->Add (bSizer241, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12);
	
	wxBoxSizer* bSizer242;
	bSizer242 = new wxBoxSizer (wxHORIZONTAL);
	
	wxBoxSizer* bSizer246;
	bSizer246 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText242 = new wxStaticText (this,
													wxID_ANY,
													wxT("Default weight:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText242->Wrap (-1);
   SetUpToolTip (m_staticText242,IDS_ToolTip338);
	bSizer246->Add (m_staticText242, 0, wxALIGN_CENTER|wxLEFT, 5);
	
	m_textCtrl128 = new wxTextCtrl (this,
												IDC_DefaultWeight,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (70,-1),
												0);
   SetUpToolTip (m_textCtrl128,IDS_ToolTip338);
	bSizer246->Add (m_textCtrl128, 0, wxALL, 5);
	
	
	bSizer242->Add (bSizer246, 0, wxALIGN_LEFT, 5);
	
	
	bSizer242->Add (0, 0, 1, wxEXPAND, 5);
	
	bSizer240->Add (bSizer242,
							wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT, 12));
	
	CreateStandardButtons (bSizer240);
	
	SetSizer (bSizer240);
	Layout ();
	
	Centre (wxBOTH);
   
}	// end "CreateControls"



//------------------------------------------------------------------------------------
//                   Copyright 2009-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the display
//							specification dialog box to the user and copy the
//							revised back to the display specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:			Larry L. Biehl			Date: 03/13/2018

SInt16 CMClassPairWeightDlg::DoDialog (
				UInt16								numberOfClassesToUse,
				SInt16**								classPairWeightsListPtrPtr,
				SInt16								interClassWeightsSelection,
				SInt16*								defaultClassPairWeightPtr)

{
   wxListBox*							listBoxPtr;
	SInt16								returnCode;
   Boolean								OKFlag = FALSE;

			// Make sure intialization has been completed.

   if (!m_initializedFlag)
															return (interClassWeightsSelection);
	
	listBoxPtr = (wxListBox*)FindWindow (IDC_ClassPairWeightList);

   m_numberOfClassesToUse = (SInt16) numberOfClassesToUse;
   m_weightsListPtrPtr = classPairWeightsListPtrPtr;
   m_classPairWeightsSelection = interClassWeightsSelection;
   m_defaultClassPairWeightPtr = defaultClassPairWeightPtr;

   returnCode = ShowModal ();
   
   if (returnCode == wxID_OK)
		{
      OKFlag = TRUE;

      interClassWeightsSelection = m_classPairWeightsSelection;

      ClassPairWeightsDialogOK (this,
											listBoxPtr,
											m_weightsListPtrPtr,
											&interClassWeightsSelection,
											(SInt16) m_localDefaultClassPairWeight,
											m_defaultClassPairWeightPtr);
   
		}	// end "if (returnCode == IDOK)"

   return (interClassWeightsSelection);

}	// end "DoDialog"



void CMClassPairWeightDlg::OnAddButton (
				wxCommandEvent&					event)

{
   wxListBox							*classListBoxCPtr,
											*weightListBoxCPtr;


   classListBoxCPtr = (wxListBox*)FindWindow (IDC_ClassList);
   weightListBoxCPtr = (wxListBox*)FindWindow (IDC_ClassPairWeightList);

   ClassPairWeightsDialogChangeWeight (this,
													classListBoxCPtr,
													weightListBoxCPtr,
													(SInt16)m_newClassPairWeight);

}	// end "OnAddButton"



void CMClassPairWeightDlg::OnChangeDefaultWeight (
				wxCommandEvent&					event)

{
	wxString								tempString;
   wxTextCtrl*							defaultweight;
	
	
	defaultweight = (wxTextCtrl*)FindWindow (IDC_DefaultWeight);
   tempString = defaultweight->GetValue ();
   
   if (tempString.length () > 0)
		{
      m_localDefaultClassPairWeight = wxAtoi (tempString);
		
		}	// end "if (tempString.GetLength () > 0)"

   else // tempString.GetLength () == 0
		{
      m_localDefaultClassPairWeight = -1;

		}	// end "else tempString.GetLength () == 0"

}	// end "OnChangeDefaultWeight"



void CMClassPairWeightDlg::OnChangeNewWeight (
				wxCommandEvent&					event)

{
	wxString								tempString;
   wxTextCtrl*							newweight;
	
	
   newweight = (wxTextCtrl*)FindWindow (IDC_NewWeight);
   tempString = newweight->GetValue ();
   
   if (tempString.length () > 0)
		{
		m_newClassPairWeight = wxAtoi (tempString);
      
      if (m_selectedClassCell >= 0)
         SetDLogControlHilite (this, IDC_AddButton, 0);

		}	// end "if (tempString.GetLength () > 0)"

   else	// tempString.GetLength () == 0
		{
      m_newClassPairWeight = -1;
      SetDLogControlHilite (this, IDC_AddButton, 255);

		}	// end "else tempString.GetLength () == 0"

}	// end "OnChangeNewWeight"



void CMClassPairWeightDlg::OnHelpButton (
				wxCommandEvent&					event)

{
   SetDLogControlHilite (this, wxID_OK, 255);

   DisplayAlert (0, 0, kAlertStrID, IDS_Alert9, 0, NULL);

   SetDLogControlHilite (this, wxID_OK, 0);

}	// end "OnHelpButton"



void CMClassPairWeightDlg::OnInitDialog (
				wxInitDialogEvent&				event)

{
	wxListBox							*classListBoxPtr,
											*weightsListBoxPtr;
	
   wxFont								font (gFontSize,
													wxFONTFAMILY_MODERN,
													wxFONTSTYLE_NORMAL,
													wxFONTWEIGHT_NORMAL);

   SInt16								localDefaultClassPairWeight;


			// Initialize some dialog parameters.

   ClassPairWeightsDialogInitialize (this,
													*m_defaultClassPairWeightPtr,
													&localDefaultClassPairWeight);

   m_localDefaultClassPairWeight = localDefaultClassPairWeight;

   if (TransferDataToWindow ())
		{
      PositionDialogWindow ();

				// Set font for the class list box.

      m_weightsListFont = font;
      
      classListBoxPtr = (wxListBox*)FindWindow (IDC_ClassList);
      classListBoxPtr->SetFont (m_weightsListFont);

				// Set font for the class pair list box

      weightsListBoxPtr = (wxListBox*)FindWindow (IDC_ClassPairWeightList);
      weightsListBoxPtr->SetFont (m_weightsListFont);

				// Load the class list.

      LoadClassList (this,
							classListBoxPtr,
							m_numberOfClassesToUse,
							NULL,
							NULL,
							FALSE);

				// Load the class weight group list if any exist.

      LoadClassWeightGroups (this, weightsListBoxPtr, *m_weightsListPtrPtr);

				// Set default text selection to first edit text item

      SelectDialogItemText (this, IDC_NewWeight, 0, SInt16_MAX);

   	}	// end "if (TransferDataToWindow ())"

}	// end "OnInitDialog"



void CMClassPairWeightDlg::OnRemoveButton (
				wxCommandEvent&					event)

{
	wxListBox*							weightListBoxCPtr;


   weightListBoxCPtr = (wxListBox*)FindWindow (IDC_ClassPairWeightList);

   m_selectedWeightGroupCell = ClassPairWeightsDialogRemoveWeightSelection (
																			this,
																			weightListBoxCPtr,
																			m_selectedWeightGroupCell);

}	// end "OnRemoveButton"



void CMClassPairWeightDlg::OnSelchangeClassList (
				wxCommandEvent&					event)

{
   wxListBox* classListBoxCPtr;


   classListBoxCPtr = (wxListBox*)FindWindow (IDC_ClassList);

   m_selectedClassCell = ClassPairWeightsDialogClassSelectionChange (
																		this,
																		classListBoxCPtr,
																		(SInt16)m_newClassPairWeight);

}	// end "OnSelchangeClassList"



void CMClassPairWeightDlg::OnSelchangeClassPairWeightList (
				wxCommandEvent&					event)

{
   wxListBox* weightListBoxCPtr;


   weightListBoxCPtr = (wxListBox*)FindWindow (IDC_ClassPairWeightList);

   m_selectedWeightGroupCell = ClassPairWeightsDialogWeightSelectionChange (
																					this,
																					weightListBoxCPtr);

}	// end "OnSelchangeClassPairWeightList"
	
	

bool CMClassPairWeightDlg::TransferDataFromWindow ()

{
   wxTextCtrl* newweight = (wxTextCtrl*)FindWindow (IDC_NewWeight);
   wxString newweightString = newweight->GetValue ();
   m_newClassPairWeight = wxAtoi (newweightString);
	
   wxTextCtrl* defaultweight = (wxTextCtrl*)FindWindow (IDC_DefaultWeight);
   wxString defaultweightString = defaultweight->GetValue ();
   m_localDefaultClassPairWeight = wxAtoi (defaultweightString);
   
   return true;
	
}	// end "TransferDataFromWindow"



bool CMClassPairWeightDlg::TransferDataToWindow ()

{
   wxTextCtrl* newweight = (wxTextCtrl*)FindWindow (IDC_NewWeight);
   newweight->ChangeValue (wxString::Format (wxT("%i"), (int)m_newClassPairWeight));
	
   wxTextCtrl* defaultweight = (wxTextCtrl*)FindWindow (IDC_DefaultWeight);
   defaultweight->ChangeValue (wxString::Format (wxT("%i"),
											(int) m_localDefaultClassPairWeight));
   
   return true;
	
}	// end "TransferDataToWindow"
