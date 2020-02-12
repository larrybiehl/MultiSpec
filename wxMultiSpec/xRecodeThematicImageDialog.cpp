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
//	File:						xRecodeThematicImageDialog.cpp : class implementation file
//	Class Definition:		xRecodeThematicImageDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			11/08/2019
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMRecodeThematicDialog class.
//
/* Template for debugging
		int numberChars = sprintf (
				(char*)&gTextString3,
				" xRecodeThematicImageDialog::xxx (entered routine. %s",
				gEndOfLine);
		ListString ((char*)&gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h"
#include "xRecodeThematicImageDialog.h"



BEGIN_EVENT_TABLE (CMRecodeThematicDialog, CMDialog)
	EVT_CHOICE (IDC_ThresholdFileCombo, CMRecodeThematicDialog::OnSelendokThresholdFileCombo)
	EVT_INIT_DIALOG (CMRecodeThematicDialog::OnInitDialog)
END_EVENT_TABLE ()



CMRecodeThematicDialog::CMRecodeThematicDialog (
				wxWindow* 							pParent,
   			wxWindowID 							id,
   			const wxString& 					title/*=NULL*/)
		: CMDialog (CMRecodeThematicDialog::IDD, pParent, title)

{
	m_recodedValue = 0;
	m_thresholdValue = 0;
	m_compareParameterCode = -1;
	m_thresholdImageSelection = -1;
	
	m_initializedFlag = CMDialog::m_initializedFlag;
   
   CreateControls ();
	
}	// end "CMRecodeThematicDialog"



CMRecodeThematicDialog::~CMRecodeThematicDialog (void)

{

}	// end "~CMRecodeThematicDialog"



void CMRecodeThematicDialog::CreateControls ()

{
   SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxBoxSizer* bSizer292;
	bSizer292 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer374;
	bSizer374 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText339 = new wxStaticText (this,
													wxID_ANY,
													wxT("Set data to:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText339->Wrap (-1);
	bSizer374->Add (m_staticText339, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	m_textCtrl173 = new wxTextCtrl (this,
												IDC_OldValue,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer374->Add (m_textCtrl173, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	m_staticText344 = new wxStaticText (this,
													IDC_InputFileNameText,
													wxT(""),
													wxDefaultPosition,
													wxSize (300, -1),
													0);
	m_staticText344->Wrap (-1);
	bSizer374->Add (m_staticText344, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	bSizer292->Add (bSizer374, 0, wxEXPAND|wxALL, 5);
	
	wxBoxSizer* bSizer376;
	bSizer376 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText341 = new wxStaticText (this,
													wxID_ANY,
													wxT("When data value"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText341->Wrap (-1);
	bSizer376->Add (m_staticText341, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	m_operationCtrl = new wxChoice (this,
												IDC_CompareCombo,
												wxDefaultPosition,
												wxDefaultSize);
	m_operationCtrl->Append (wxT("<="));
	m_operationCtrl->Append (wxT(">="));
	m_operationCtrl->Append (wxT("="));
	m_operationCtrl->Append (wxT("~="));
   bSizer376->Add (m_operationCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	m_textCtrl175 = new wxTextCtrl (this,
												IDC_NewValue,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer376->Add (m_textCtrl175, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	bSizer292->Add (bSizer376, 0, wxEXPAND|wxALL, 5);
	
	wxBoxSizer* bSizer377;
	bSizer377 = new wxBoxSizer (wxHORIZONTAL);
	
	
	bSizer377->Add (50, 0, 0, wxEXPAND, 5);
	
	m_staticText342 = new wxStaticText (this,
													wxID_ANY,
													wxT("in file:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText342->Wrap (-1);
	bSizer377->Add (m_staticText342, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	m_thresholdFileCtrl = new wxChoice (this,
													IDC_ThresholdFileCombo,
													wxDefaultPosition,
													wxSize (300, -1));
	m_thresholdFileCtrl->Append (wxT("Select threshold file..."));
   
   bSizer377->Add (m_thresholdFileCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	
	bSizer292->Add (bSizer377, 0, wxEXPAND|wxLEFT, 12);
	
			// Add Cancel and OK buttons

	CreateStandardButtons (bSizer292);
	
   SetSizerAndFit (bSizer292);
	Layout ();
	Centre (wxBOTH);
	
}	// end "CreateControls"



void CMRecodeThematicDialog::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	SInt16								compareParameterCode,
											thresholdImageSelection;
	
	
	CMDialog::OnInitDialog (event);
	
			// Initialize dialog variables.
			
	RecodeThematicImageDialogInitialize (this,
														m_recodeThematicImagePtr,
														&m_thresholdFileInfoHandle,
														&m_recodedValue,
														&m_thresholdValue,
														&compareParameterCode,
														&thresholdImageSelection);
												
	m_compareParameterCode = compareParameterCode - 1;
	m_thresholdImageSelection = thresholdImageSelection - 1;
   
   		// Set default text selection to first edit text item
		                                                     
	SelectDialogItemText (this, IDC_NewValue, 0, SInt16_MAX);
   
   if (TransferDataToWindow ())
		PositionDialogWindow ();
	
}	// end "OnInitDialog"



Boolean CMRecodeThematicDialog::DoDialog (
				RecodeThematicImagePtr			recodeThematicImagePtr)

{	   
	SInt16								returnCode = -1;
	
	Boolean								continueFlag = FALSE,
											changedFlag;
							
	
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																					return (FALSE);
	
	m_recodeThematicImagePtr = recodeThematicImagePtr;
	
   if (TransferDataFromWindow ())
      returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{
		changedFlag = FALSE;
		continueFlag = TRUE; 
				
		RecodeThematicImageDialogOK (recodeThematicImagePtr,
												m_thresholdFileInfoHandle,
												m_recodedValue,
												m_thresholdValue,
												m_compareParameterCode+1,
												m_thresholdImageSelection+1);
														
		}	// end "if (returnCode == IDOK)"
	
	return (continueFlag); 
	
}	// end "DoDialog"



void CMRecodeThematicDialog::OnSelendokThresholdFileCombo (
				wxCommandEvent& 					event)

{
	int									thresholdImageSelection;
	
	                                                   
   thresholdImageSelection = m_thresholdFileCtrl->GetSelection ();

	SetDLogControlHilite (this, wxID_OK, 255);
	                                       
	thresholdImageSelection = RecodeThematicImageDialogSelectThresholdItem (
																		&m_thresholdFileInfoHandle,
																		this,
																		0,
																		thresholdImageSelection+1,
																		m_thresholdImageSelection+1,
																		IDS_FileIO105,
																		IDC_ThresholdFileCombo);

	SetDLogControlHilite (this, wxID_OK, 0);

	m_thresholdImageSelection = thresholdImageSelection - 1;
	
	m_thresholdFileCtrl->SetSelection (m_thresholdImageSelection);
	
}	// end "OnSelendokThresholdFileCombo"



bool CMRecodeThematicDialog::TransferDataToWindow ()

{
   m_operationCtrl->SetSelection (m_compareParameterCode);
	
   m_thresholdFileCtrl->SetSelection (m_thresholdImageSelection);
	
   wxTextCtrl* newValueText = (wxTextCtrl*)FindWindow (IDC_NewValue);
   newValueText->SetValue (wxString::Format (wxT("%i"), m_thresholdValue));
	
   wxTextCtrl* oldValueText = (wxTextCtrl*)FindWindow (IDC_OldValue);
   oldValueText->SetValue (wxString::Format (wxT("%i"), m_recodedValue));
	
   return (true);
	
}	// end "TransferDataToWindow"



bool CMRecodeThematicDialog::TransferDataFromWindow ()

{
   m_compareParameterCode = m_operationCtrl->GetSelection ();
	
   m_thresholdImageSelection = m_thresholdFileCtrl->GetSelection ();
   
   wxTextCtrl* newValueText = (wxTextCtrl*)FindWindow (IDC_NewValue);
   m_thresholdValue = wxAtoi (newValueText->GetValue());
	
   wxTextCtrl* oldValueText = (wxTextCtrl*)FindWindow (IDC_OldValue);
   m_recodedValue = wxAtoi (oldValueText->GetValue());
   
	return (true);
	
}	// end "TransferDataFromWindow"

