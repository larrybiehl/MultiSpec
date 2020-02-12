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
// File:						xClassifyKNNDialog.cpp : class implementation file
// Class Definition:    xClassifyKNNDialog.h
//
// Authors:             Larry L. Biehl
//
// Revision date:       05/04/2019
//
// Language:				C++
//
// System:              Linux and MacOS Operating Systems
//
// Brief description:   This file contains functions that relate to the
//                      CMClassifyKNNDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xMultiSpec.h"
#include "xClassifyKNNDialog.h"

BEGIN_EVENT_TABLE (CMKNNClassifyDialog, CMDialog)
	EVT_INIT_DIALOG (CMKNNClassifyDialog::OnInitDialog)
END_EVENT_TABLE ()



CMKNNClassifyDialog::CMKNNClassifyDialog (
				wxWindow*                     pParent,
				wxWindowID                     id,
				const wxString&               title)
		: CMDialog (id, pParent, title)

{
   m_nearestNeighborKValue = 5;
   m_initializedFlag = CMDialog::m_initializedFlag;
   
   CreateControls ();
   SetSizerAndFit (bSizer166);
   
}	// end "CMCorrelationClassifyDialog"



void CMKNNClassifyDialog::CreateControls ()

{
   SetSizeHints (wxDefaultSize, wxDefaultSize);
	
   bSizer166 = new wxBoxSizer (wxVERTICAL);
   
   wxBoxSizer* bSizer167;
   bSizer167 = new wxBoxSizer (wxHORIZONTAL);
   
   m_staticText183 = new wxStaticText (this,
                                       wxID_ANY,
                                       wxT("Nearest Neighbor K value:"),
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       0);
   m_staticText183->Wrap (-1);
   bSizer167->Add (m_staticText183, 0, wxALIGN_CENTER|wxALL, 5);
   
   m_kValueControl = new wxTextCtrl (this,
										  IDC_KValue,
										  wxEmptyString,
										  wxDefaultPosition,
										  wxSize (50, -1),
										  0);
   
   m_kValueControl->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_kValueString));
	bSizer167->Add (m_kValueControl, 0, wxALIGN_CENTER|wxALL, 5);
   
   m_staticText184 = new wxStaticText (this,
                                       wxID_ANY,
                                       wxT("e.g. (3-10)"),
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       0);
   m_staticText184->Wrap (-1);
   bSizer167->Add (m_staticText184, 0, wxALIGN_CENTER|wxALL, 5);
   
   bSizer166->Add (bSizer167, 1, wxALL|wxEXPAND, 12);
   
   CreateStandardButtons (bSizer166);
   
   SetSizer (bSizer166);
   Layout ();
   
   Centre (wxBOTH);
   
}	// end "CreateControls"



//------------------------------------------------------------------------------------
//                   Copyright 2012-2020 Purdue Research Foundation
//
//   Function name:      void DoDialog
//
//   Software purpose:   The purpose of this routine is to present the Correlation
//                     specification dialog box to the user and copy the
//                     revised back to the classify specification structure if
//                     the user selected OK.
//                     This code was adapted from the Windows version of the same class.
//
//   Parameters in:      None
//
//   Parameters out:   None
//
//   Value Returned:   None
//
//   Called By:
//
// Coded By:       	Tsung Tai Yeh			Date: 04/??/2019
// Revised By:       Larry L. Biehl			Date: 05/04/2019

Boolean CMKNNClassifyDialog::DoDialog (
				SInt16*								nearestNeighborKValuePtr)

{
   SInt16                        returnCode;
   
   bool                          continueFlag = false;
   
   
   		// Make sure intialization has been completed.
   
   if (!m_initializedFlag)
																						return (false);
   
   m_nearestNeighborKValue = *nearestNeighborKValuePtr;
   
   returnCode = ShowModal ();
   
   if (returnCode == wxID_OK)
   	{
      continueFlag = true;
	
      *nearestNeighborKValuePtr = m_nearestNeighborKValue;
	
   	}	// end "if (returnCode == IDOK)"
   
   return (continueFlag);
   
}	// end "DoDialog"



void CMKNNClassifyDialog::OnInitDialog (
				wxInitDialogEvent&            event)

{
   CMDialog::OnInitDialog (event);
	
	SelectDialogItemText (this, IDC_KValue, 0, SInt16_MAX);
   
}	// end "OnInitDialog"



bool CMKNNClassifyDialog::TransferDataFromWindow ()

{
	SInt16						continueCode,
									returnCode = 0;
	
	
   m_nearestNeighborKValue = wxAtoi (m_kValueControl->GetValue ());
	
	continueCode = CheckMaxValue (this,
											IDC_KValue,
											1,
											100,
											kDisplayRangeAlert);
	if (continueCode != 1)
		returnCode = IDC_KValue;
	
   return (returnCode == 0);
   
}	// end "TransferDataFromWindow"



bool CMKNNClassifyDialog::TransferDataToWindow ()

{
   m_kValueControl->SetValue(wxString::Format (wxT("%d"), m_nearestNeighborKValue));
	
   return true;
   
}	// end "TransferDataToWindow"


