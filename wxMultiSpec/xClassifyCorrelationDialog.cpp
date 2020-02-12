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
//	File:						xClassifyCorrelationDialog.cpp : class implementation file
//	Class Definition:		xClassifyCorrelationDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/13/2019
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMCorrelationClassifyDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 
                    
#include "xMultiSpec.h"  
#include "xClassifyCorrelationDialog.h"

BEGIN_EVENT_TABLE (CMCorrelationClassifyDialog, CMDialog)
   EVT_INIT_DIALOG (CMCorrelationClassifyDialog::OnInitDialog)
	EVT_CHOICE (IDC_CovarianceCombo, CMCorrelationClassifyDialog::OnCovarianceComboSelendok)
END_EVENT_TABLE ()



CMCorrelationClassifyDialog::CMCorrelationClassifyDialog (
				wxWindow*							pParent, 
				wxWindowID							id, 
				const wxString&					title)
		: CMDialog (id, pParent, title)
		 
{
	m_covarianceEstimate = -1;
   m_initializedFlag = CMDialog::m_initializedFlag;

   CreateControls ();
	SetSizerAndFit (bSizer166);
	
}	// end "CMCorrelationClassifyDialog"
  
  

void CMCorrelationClassifyDialog::CreateControls ()

{
   SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	bSizer166 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer167;
	bSizer167 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText183 = new wxStaticText (this, 
													wxID_ANY, 
													wxT("Class Covariance:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText183->Wrap (-1);
	bSizer167->Add (m_staticText183, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_covarianceCtrl = new wxChoice (this,
												IDC_CovarianceCombo, 
												//wxT("Combo!"),
												wxDefaultPosition, 
												wxSize (180,-1));
												//0,
												//NULL,
												//wxCB_READONLY);
	m_covarianceCtrl->Append (wxT("Do not use"));
	m_covarianceCtrl->Append (wxT("Common covariance"));
	m_covarianceCtrl->Append (wxT("Sample covariance"));
   SetUpToolTip (m_covarianceCtrl, IDS_ToolTip327);
	bSizer167->Add (m_covarianceCtrl, 0, wxALIGN_CENTER|wxALL, 5);
	
	bSizer166->Add (bSizer167, 1, wxALL|wxEXPAND, 12);
	
	CreateStandardButtons (bSizer166);
	
	SetSizer (bSizer166);
	Layout ();
	Centre (wxBOTH);
   
}	// end "CreateControls"



//------------------------------------------------------------------------------------
//                   Copyright 2009-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the Correlation
//							specification dialog box to the user and copy the
//							revised back to the classify specification structure if
//							the user selected OK.
//							This code was adapted from the Windows version of the same class.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: ??/??/2012
//	Revised By:			Larry L. Biehl			Date: ??/??/2012

Boolean CMCorrelationClassifyDialog::DoDialog (
				SInt16*								covarianceEstimatePtr)

{  
	SInt16								returnCode;
	
	bool									continueFlag = false;
	
	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																							return (false);
																			
	m_covarianceEstimate = *covarianceEstimatePtr - 1;
																					
	returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{
		continueFlag = true;
		
		*covarianceEstimatePtr = m_covarianceEstimate + 1; 
		
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



void CMCorrelationClassifyDialog::OnInitDialog (
				wxInitDialogEvent&				event)
				
{
	CMDialog::OnInitDialog (event);
	
}	// end "OnInitDialog"



void CMCorrelationClassifyDialog::OnCovarianceComboSelendok (
				wxCommandEvent&					event)
				
{
			// Add your control notification handler code here
	
}	// end "OnCovarianceComboSelendok"



bool CMCorrelationClassifyDialog::TransferDataFromWindow ()

{
   m_covarianceEstimate = m_covarianceCtrl->GetSelection ();
   
   return true;
	
}	// end "TransferDataFromWindow"



bool CMCorrelationClassifyDialog::TransferDataToWindow ()

{
   m_covarianceCtrl->SetSelection (m_covarianceEstimate);
   
   return true;
	
}	// end "TransferDataToWindow"
