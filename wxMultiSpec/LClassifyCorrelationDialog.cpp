//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClassifyCorrelationDialog.cpp : class implementation file
//	Class Definition:		LClassifyCorrelationDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/16/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMCorrelationClassifyDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 
                    
#include "LMultiSpec.h"  
#include "LClassifyCorrelationDialog.h"

BEGIN_EVENT_TABLE (CMCorrelationClassifyDialog, CMDialog)
   EVT_INIT_DIALOG (CMCorrelationClassifyDialog::OnInitDialog)
	EVT_COMBOBOX (IDC_CovarianceCombo, CMCorrelationClassifyDialog::OnSelendokCovarianceCombo)
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
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	//wxBoxSizer* bSizer166;
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
	
	m_comboBox27 = new wxComboBox (this, 
												IDC_CovarianceCombo, 
												wxT("Combo!"), 
												wxDefaultPosition, 
												wxSize (180,-1), 
												0, 
												NULL, 
												0);
	m_comboBox27->Append (wxT("Do not use"));
	m_comboBox27->Append (wxT("Common covariance"));
	m_comboBox27->Append (wxT("Sample covariance"));
   SetUpToolTip (m_comboBox27, IDS_ToolTip327);
	bSizer167->Add (m_comboBox27, 0, wxALIGN_CENTER|wxALL, 5);
	
	
	bSizer166->Add (bSizer167, 1, wxALL|wxEXPAND, 12);
	/*
	wxBoxSizer* bSizer169;
	bSizer169 = new wxBoxSizer (wxHORIZONTAL);
	
	m_button37 = new wxButton (this, 
										wxID_CANCEL, 
										wxT("Cancel"), 
										wxDefaultPosition, 
										wxDefaultSize, 
										0);
	bSizer169->Add (m_button37, 0, wxALL, 5);
	
	m_button38 = new wxButton (this, 
										wxID_OK, 
										wxT("OK"), 
										wxDefaultPosition, 
										wxDefaultSize, 
										0);
	bSizer169->Add (m_button38, 0, wxALL, 5);
	
	bSizer166->Add (bSizer169, 0, wxALIGN_RIGHT|wxALL, 12);
	*/
	CreateStandardButtons (bSizer166);
	
	this->SetSizer (bSizer166);
	this->Layout ();
	
	this->Centre (wxBOTH);
   
}	// end "CreateControls"



//------------------------------------------------------------------------------------
//								 Copyright (2012-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
//	Coded By:			Larry L. Biehl			Date: 04/09/1998
//	Revised By:			Larry L. Biehl			Date: 04/09/1998	

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



void CMCorrelationClassifyDialog::OnSelendokCovarianceCombo (
				wxCommandEvent&					event)
				
{
			// Add your control notification handler code here
	
}	// end "OnSelendokCovarianceCombo"



bool CMCorrelationClassifyDialog::TransferDataFromWindow ()

{
   wxComboBox* covEst = (wxComboBox*)FindWindow (IDC_CovarianceCombo);
   m_covarianceEstimate = covEst->GetSelection ();
   
   return true;
	
}	// end "TransferDataFromWindow"



bool CMCorrelationClassifyDialog::TransferDataToWindow ()

{
   wxComboBox* covEst = (wxComboBox*)FindWindow (IDC_CovarianceCombo);
   covEst->SetSelection (m_covarianceEstimate);
   
   return true;
	
}	// end "TransferDataToWindow"
