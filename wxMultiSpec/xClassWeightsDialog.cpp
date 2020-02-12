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
//	File:						xClassWeightsDialog.cpp : class implementation file
//	Class Definition:		xClassWeightsDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			11/15/2018
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMClassWeightsDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "xClassWeightsDialog.h"



CMClassWeightsDlg::CMClassWeightsDlg (
				wxWindow*							pParent,
				wxWindowID							id,
				const wxString&					title)
		: CMDialog (CMClassWeightsDlg::IDD, pParent, title)

{
   m_saveNewWeight = 0.0;
   m_weightUnits = -1;

   m_weightSum = 0;

   m_weightsListPtr = NULL;
   m_classListPtr = NULL;

   m_numberOutputClassesToUse = 0;
   m_selectedCell = -1;
   m_weightsSelection = -1;

   m_useEnhancedStatFlag = FALSE;

   m_initializedFlag = CMDialog::m_initializedFlag;
   CreateControls ();
	
}	// end "CMClassWeightsDlg"



CMClassWeightsDlg::~CMClassWeightsDlg ()

{

}	// end "~CMClassWeightsDlg"



BEGIN_EVENT_TABLE (CMClassWeightsDlg, CMDialog)
	EVT_INIT_DIALOG (CMClassWeightsDlg::OnInitDialog)
	EVT_BUTTON (IDC_EnterNewWeightButton, CMClassWeightsDlg::OnEnterNewWeightButton)
	EVT_BUTTON (IDC_EqualWeightButton, CMClassWeightsDlg::OnEqualWeightButton)
	EVT_BUTTON (IDC_HelpButton, CMClassWeightsDlg::OnHelpButton)
	EVT_TEXT (IDC_Weight, CMClassWeightsDlg::OnChangeWeight)
	EVT_LISTBOX (IDC_List, CMClassWeightsDlg::OnSelchangeList)
END_EVENT_TABLE ()



void CMClassWeightsDlg::CreateControls ()

{
   SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxFont  font (gFontSize,
						wxFONTFAMILY_MODERN,
						wxFONTSTYLE_NORMAL,
						wxFONTWEIGHT_NORMAL);

   bSizer136 = new wxBoxSizer (wxVERTICAL);

   wxBoxSizer* bSizer137;
   bSizer137 = new wxBoxSizer (wxHORIZONTAL);

   wxBoxSizer* bSizer139;
   bSizer139 = new wxBoxSizer (wxVERTICAL);

   m_staticText188 = new wxStaticText (
							this,
							wxID_ANY,
							wxT("Class                                            - Weight"),
							wxDefaultPosition,
							wxDefaultSize,
							0);
   m_staticText188->Wrap (-1);
   SetUpToolTip (m_staticText188, IDS_ToolTip329);
   bSizer139->Add (m_staticText188, 0, wxLEFT, 25);

   m_listBox1 = new wxListBox (this,
											IDC_List,
											wxDefaultPosition,
											wxSize (320, 250),
											0,
											NULL,
											wxLB_MULTIPLE|wxLB_NEEDED_SB);
	m_listBox1->SetFont (font);
	bSizer139->Add (m_listBox1, 0, wxALL, 5);

   bSizer137->Add (bSizer139, 0, wxALL, 12);

   wxBoxSizer* bSizer140;
   bSizer140 = new wxBoxSizer (wxVERTICAL);

   m_staticText189 = new wxStaticText (this,
													wxID_ANY,
													wxT("Units"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText189->Wrap (-1);
   bSizer140->Add (m_staticText189, 0, wxALL, 5);

   m_radioBtn11 = new wxRadioButton (this,
													IDC_UnitsRelative,
													wxT("Relative"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_radioBtn11, IDS_ToolTip333);
   bSizer140->Add (m_radioBtn11, 0, wxLEFT, 15);

   m_radioBtn12 = new wxRadioButton (this,
													IDC_UnitsPercent,
													wxT("Percent"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_radioBtn12, IDS_ToolTip334);
   bSizer140->Add (m_radioBtn12, 0, wxLEFT, 15);

   m_staticText190 = new wxStaticText (this,
													wxID_ANY,
													wxT("Weight Total"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText190->Wrap (-1);
   SetUpToolTip (m_staticText190, IDS_ToolTip335);
   bSizer140->Add (m_staticText190, 0, wxALL, 5);

   m_staticText191 = new wxStaticText (this,
													IDC_WeightTotal,
													wxT("100.000"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText191->Wrap (-1);
   SetUpToolTip (m_staticText191, IDS_ToolTip335);
   bSizer140->Add (m_staticText191, 0, wxLEFT, 15);

   bSizer137->Add (bSizer140, 0, wxALL, 12);

   bSizer136->Add (bSizer137, 0, wxEXPAND, 5);

   wxBoxSizer* bSizer138;
	bSizer138 = new wxBoxSizer (wxHORIZONTAL);
	
	wxBoxSizer* bSizer141;
	bSizer141 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText192 = new wxStaticText (this,
													wxID_ANY,
													wxT("Edit weight(s) for selected class(es):"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText192->Wrap (-1);
	bSizer141->Add (m_staticText192, 0, wxALL, 5);
	
	wxBoxSizer* bSizer143;
	bSizer143 = new wxBoxSizer (wxHORIZONTAL);
	
	m_button30 = new wxButton (this,
										IDC_EnterNewWeightButton,
										wxT("Enter New Weight"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
   SetUpToolTip (m_button30, IDS_ToolTip330);
	bSizer143->Add (m_button30, 0, wxALL, 5);
	
	m_textCtrl89 = new wxTextCtrl (this,
												IDC_Weight,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   m_textCtrl89->SetValidator (wxTextValidator (wxFILTER_NUMERIC, &m_weightString));
   SetUpToolTip (m_textCtrl89, IDS_ToolTip331);
	bSizer143->Add (m_textCtrl89, 0, wxALL, 5);
	
	bSizer141->Add (bSizer143, 0, wxEXPAND, 5);
	
	m_button31 = new wxButton (this,
										IDC_EqualWeightButton,
										wxT("Add To Equal Weight Set"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
   SetUpToolTip (m_button31, IDS_ToolTip332);
	bSizer141->Add (m_button31, 0, wxALL, 5);
	
	bSizer138->Add (bSizer141, 0, wxALL|wxEXPAND, 12);
	
	bSizer138->Add (0, 0, 0, wxEXPAND, 5);
	
	wxBoxSizer* bSizer142;
	bSizer142 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer144;
	bSizer144 = new wxBoxSizer (wxHORIZONTAL);
	
	m_button34 = new wxButton (this,
										IDC_HelpButton,
										wxT("Help..."),
										wxDefaultPosition,
										wxDefaultSize,
										0);
   SetUpToolTip (m_button34, IDS_ToolTip336);
	bSizer144->Add (m_button34, 0, wxALL, 5);
	
	bSizer142->Add (bSizer144, 1, wxALIGN_RIGHT);

	bSizer138->Add (bSizer142, 0, wxEXPAND|wxLEFT|wxTOP|wxRIGHT, 12);
	
	bSizer136->Add (bSizer138, 0, wxEXPAND);
	
	CreateStandardButtons (bSizer136);
	
	SetSizer (bSizer136);
	Layout ();
	Fit ();
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
//	Coded By:			Larry L. Biehl			Date: 12/15/2009
//	Revised By:			Larry L. Biehl			Date: 03/13/2018

SInt16 CMClassWeightsDlg::DoDialog (
				UInt16								numberOutputClassesToUse,
				SInt16*								classListPtr,
				float*								weightsListPtr,
				SInt16								weightsSelection,
				Boolean								useEnhancedStatFlag)

{
	SInt16								returnCode;
   Boolean								OKFlag = FALSE;
	
	
			// Make sure intialization has been completed.

   if (!m_initializedFlag)
																			return (weightsSelection);

   m_numberOutputClassesToUse = (SInt16) numberOutputClassesToUse;
   m_classListPtr = classListPtr;
   m_weightsListPtr = weightsListPtr;
   m_weightsSelection = weightsSelection;
   m_useEnhancedStatFlag = useEnhancedStatFlag;

   returnCode = ShowModal ();

   if (returnCode == wxID_OK)
		{
      OKFlag = TRUE;

      weightsSelection = m_weightsSelection;

		}	// end "if (returnCode == IDOK)"

   return (weightsSelection);

}	// end "DoDialog"



void CMClassWeightsDlg::OnChangeWeight (
				wxCommandEvent&					event)

{
	wxString								tempString;


   wxTextCtrl* weight = (wxTextCtrl*)FindWindow (IDC_Weight);
   tempString = weight->GetValue ();
   if (tempString.length () > 0)
		{
      tempString.ToDouble (&m_saveNewWeight);
      SetDLogControlHilite (this, IDC_EnterNewWeightButton, 0);

		}	// end "if (tempString.GetLength () > 0)"

   else	// tempString.GetLength () == 0
		{
      m_saveNewWeight = -1;
      SetDLogControlHilite (this, IDC_EnterNewWeightButton, 255);

		}	// end "else tempString.GetLength () == 0"

}	// end "OnChangeWeight"



void CMClassWeightsDlg::OnEnterNewWeightButton (
				wxCommandEvent&					event)

{
   wxListBox* listBoxCPtr;

   listBoxCPtr = (wxListBox*)FindWindow (IDC_List);

   m_saveNewWeight = ClassWeightsDialogChangeWeights (this,
																		listBoxCPtr,
																		&m_weightSum,
																		m_saveNewWeight,
																		wxID_OK);

}	// end "OnEnterNewWeightButton"



void CMClassWeightsDlg::OnEqualWeightButton (
				wxCommandEvent&					event)

{
   double defaultEqualWeight = 10.;
   wxListBox* listBoxCPtr;
   listBoxCPtr = (wxListBox*)FindWindow (IDC_List);
   ClassWeightsDialogSetEqualWeights (this,
													listBoxCPtr,
													&m_weightSum,
													defaultEqualWeight,
													wxID_OK);

}	// end "OnEqualWeightButton"



void CMClassWeightsDlg::OnHelpButton (
				wxCommandEvent&					event)

{
   SetDLogControlHilite (this, wxID_OK, 255);
   DisplayAlert (0, 0, kAlertStrID, IDS_Alert7, 0, NULL);
   //TextFont (gWindowTextFont); 				// monaco
   //LUpdate (((GrafPtr)dialogPtr)->visRgn, gDialogListHandle);
   SetDLogControlHilite (this, wxID_OK, 0);
	
}	// end "OnHelpButton"



void CMClassWeightsDlg::OnInitDialog (
				wxInitDialogEvent&				event)

{
	wxListBox*							listBoxPtr;
   SInt16								weightUnits;
   /*
   wxFont								font (9,
													wxFONTFAMILY_MODERN,
													wxFONTSTYLE_NORMAL,
													wxFONTWEIGHT_NORMAL);
	*/

   ClassWeightsDialogInitialize (this, &weightUnits);
   m_weightUnits = weightUnits;

   if (TransferDataToWindow ())
		{
      PositionDialogWindow ();

      listBoxPtr = (wxListBox*)FindWindow (IDC_List);
      // Set font for the weights list box.
      //memset (&logFont, 0, sizeof (LOGFONT)); // Clear out structure.
      //logFont.lfHeight = -10;
      //strcpy (logFont.lfFaceName, "Courier");
      //m_weightsListFont.CreateFontIndirect (&logFont);
      //m_weightsListFont.SetFamily (wxFONTFAMILY_ROMAN);
      //listBoxPtr->SetFont (&m_weightsListFont);
      //m_weightsListFont = font;
      //listBoxPtr->SetFont (m_weightsListFont);

      m_weightSum = LoadClassWeightsIntoList (listBoxPtr,
																m_numberOutputClassesToUse,
																m_classListPtr,
																m_weightsListPtr,
																m_useEnhancedStatFlag);

				//	Load class weight total.

      LoadDItemRealValue (this, IDC_WeightTotal, m_weightSum, 3);

				// Set default text selection to first edit text item

      SelectDialogItemText (this, IDC_Weight, 0, SInt16_MAX);

		}	// end "if (TransferDataToWindow ())"

}	// end "OnInitDialog"



void CMClassWeightsDlg::OnOK (
				wxCommandEvent&					event)

{
	wxListBox*							listBoxPtr;
	
	
   listBoxPtr = (wxListBox*)FindWindow (IDC_List);

   m_weightsSelection = ClassWeightsDialogOK (this,
																listBoxPtr,
																m_numberOutputClassesToUse,
																m_classListPtr,
																m_weightsListPtr);
	
}	// end "OnOK"



void CMClassWeightsDlg::OnSelchangeList (
				wxCommandEvent&					event)

{
	wxListBox*							listBoxCPtr;
	
	
   listBoxCPtr = (wxListBox*)FindWindow (IDC_List);
   m_selectedCell = ClassWeightsDialogClassSelectionChange (this,
																				listBoxCPtr,
																				m_saveNewWeight);

}	// end "OnSelchangeList"



bool CMClassWeightsDlg::TransferDataFromWindow ()

{
   if (m_weightString.length () > 0)
      m_weightString.ToDouble (&m_saveNewWeight);
	
	else
      m_saveNewWeight = -1;
	
   wxRadioButton* weightunit = (wxRadioButton*)FindWindow (IDC_UnitsRelative);
   if (weightunit->GetValue () == true)
      m_weightUnits = 0;
   else
      m_weightUnits = 1;
   
   wxListBox* listBoxPtr= (wxListBox*)FindWindow (IDC_List);

   m_weightsSelection = ClassWeightsDialogOK (this,
																listBoxPtr,
																m_numberOutputClassesToUse,
																m_classListPtr,
																m_weightsListPtr);
	
   return true;
	
}	// end "TransferDataFromWindow"



bool CMClassWeightsDlg::TransferDataToWindow ()

{
   wxRadioButton* weightunit = (wxRadioButton*)FindWindow (IDC_UnitsRelative);
   if (m_weightUnits == 0)
      weightunit->SetValue (true);
   else
      weightunit->SetValue (false);
   
   return true;
	
}	// end "TransferDataToWindow"
