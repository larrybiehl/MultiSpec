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
// File:						xClassifyParallelPipedDialog.cpp : class implementation file
// Class Definition:    xClassifyParallelPipedDialog.h
//
// Authors:             Larry L. Biehl
//
// Revision date:       02/17/2025
//
// Language:				C++
//
// System:              Linux and MacOS Operating Systems
//
// Brief description:   This file contains functions that relate to the
//                      CMClassifyParallelPipedDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xMultiSpec.h"
#include "xClassifyParallelPipedDialog.h"
#include "wx/valnum.h"

BEGIN_EVENT_TABLE (CMClassifyParallelPipedDialog, CMDialog)
	EVT_INIT_DIALOG (CMClassifyParallelPipedDialog::OnInitDialog)
END_EVENT_TABLE ()



CMClassifyParallelPipedDialog::CMClassifyParallelPipedDialog (
				wxWindow*                     pParent,
				wxWindowID                     id,
				const wxString&               title)
		: CMDialog (id, pParent, title)

{
   m_parallelPipedLimitCode = kPPStandardDeviationCode;
   m_initializedFlag = CMDialog::m_initializedFlag;
   
   CreateControls ();
   SetSizerAndFit (bSizer161);
   
}	// end "CMClassifyParallelPipedDialog"



void CMClassifyParallelPipedDialog::CreateControls ()

{
   SetSizeHints (wxDefaultSize, wxDefaultSize);
	
   bSizer161 = new wxBoxSizer (wxVERTICAL);

   m_staticText175 = new wxStaticText (
									this, 
									wxID_ANY, 
									wxT("Parallel Piped box limits:"),
									wxDefaultPosition,
									wxDefaultSize, 
									0);
   m_staticText175->Wrap (-1);
   bSizer161->Add (m_staticText175, 0, wxALL, 12);

   wxBoxSizer* bSizer181 = new wxBoxSizer (wxHORIZONTAL);
   
   m_radioBtn11 = new wxRadioButton (this,
													IDC_Limit2StdDeviation,
													wxT("plus/minus"),
													wxDefaultPosition,
													wxDefaultSize, 
													0);
   SetUpToolTip (m_radioBtn11, IDS_ToolTip8415);
	bSizer181->Add (m_radioBtn11,
						wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxTOP|wxBOTTOM, 5));

	wxTextCtrl* textCtr12;
	textCtr12 = new wxTextCtrl (this,
												IDC_StdDeviationValue,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (45, -1),
												0);
	textCtr12->SetValidator (wxTextValidator (wxFILTER_NUMERIC, &m_StdDeviationString));
	wxFloatingPointValidator<double> _val (2, &m_stdDeviationValue);
	_val.SetRange (0.0, 100.);
	textCtr12->SetValidator (_val);
	SetUpToolTip (textCtr12, IDS_ToolTip183);
	bSizer181->Add (textCtr12,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	wxStaticText* staticText13;
   staticText13 = new wxStaticText (
									this,
									wxID_ANY, 
									wxT("standard deviations from mean"),
									wxDefaultPosition,
									wxDefaultSize, 
									0);
   staticText13->Wrap (-1);
	bSizer181->Add (staticText13,
						wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxTOP|wxBOTTOM|wxRIGHT, 5));
   
	bSizer161->Add (bSizer181, wxSizerFlags(0).Border(wxALL, 10));

	wxRadioButton* radioBtn21;
   radioBtn21 = new wxRadioButton (this,
													IDC_LimitMinMax,
													wxT("min/max value of training statistics"),
													wxDefaultPosition,
													wxDefaultSize, 
													0);
   SetUpToolTip (radioBtn21, IDS_ToolTip8416);
	bSizer161->Add (radioBtn21, wxSizerFlags(0).Border(wxALL, 10));
   
   CreateStandardButtons (bSizer161);
   
   SetSizer (bSizer161);
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
// Coded By:       	Larry L. Biehl			Date: 02/10/2025
// Revised By:       Larry L. Biehl			Date: 02/17/2025

Boolean CMClassifyParallelPipedDialog::DoDialog (
				SInt16*								parallelPipedLimitCodePtr,
				double*								stdDeviationValuePtr)

{
   SInt16                        returnCode;
   
   bool                          continueFlag = false;
   
   
   		// Make sure intialization has been completed.
   
   if (!m_initializedFlag)
																						return (false);
   
   m_parallelPipedLimitCode = *parallelPipedLimitCodePtr;
   m_stdDeviationValue = *stdDeviationValuePtr;
   
   returnCode = ShowModal ();
   
   if (returnCode == wxID_OK)
   	{
      continueFlag = true;
	
      *parallelPipedLimitCodePtr = m_parallelPipedLimitCode;
      *stdDeviationValuePtr = m_stdDeviationValue;
	
   	}	// end "if (returnCode == IDOK)"
   
   return (continueFlag);
   
}	// end "DoDialog"



void CMClassifyParallelPipedDialog::OnInitDialog (
				wxInitDialogEvent&            event)

{
   CMDialog::OnInitDialog (event);

			// Set default text selection to first edit text item	

	SelectDialogItemText (this, IDC_StdDeviationValue, 0, SInt16_MAX);
   
}	// end "OnInitDialog"



bool CMClassifyParallelPipedDialog::TransferDataFromWindow ()

{
   wxRadioButton* limitBox = (wxRadioButton*)FindWindow (IDC_Limit2StdDeviation);
   if (limitBox->GetValue () == true)
      m_parallelPipedLimitCode = kPPStandardDeviationCode;
		
   else	// limitBox->GetValue () == false
      m_parallelPipedLimitCode = kPPMinMaxCode;
      
	wxTextCtrl* stdDeviationValuePtr = (wxTextCtrl*)FindWindow (IDC_StdDeviationValue);
	wxString stdDeviationString = stdDeviationValuePtr->GetValue ();
	stdDeviationString.ToDouble (&m_stdDeviationValue);
      
	return true;
   
}	// end "TransferDataFromWindow"



bool CMClassifyParallelPipedDialog::TransferDataToWindow ()

{
	wxRadioButton* 					limitBox;
	
	
   limitBox = (wxRadioButton*)FindWindow (IDC_Limit2StdDeviation);
   if (m_parallelPipedLimitCode == kPPStandardDeviationCode)
       limitBox->SetValue (m_parallelPipedLimitCode == kPPStandardDeviationCode);
       
   limitBox = (wxRadioButton*)FindWindow (IDC_LimitMinMax);
   if (m_parallelPipedLimitCode != kPPStandardDeviationCode)
       limitBox->SetValue (m_parallelPipedLimitCode != kPPStandardDeviationCode);

	wxTextCtrl* stdDeviationValueCtrl = (wxTextCtrl*)FindWindow (IDC_StdDeviationValue);
	stdDeviationValueCtrl->ChangeValue (wxString::Format (wxT("%.2f"), m_stdDeviationValue));
	
   return true;
   
}	// end "TransferDataToWindow"
