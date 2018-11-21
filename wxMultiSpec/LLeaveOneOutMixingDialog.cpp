//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Implementation file:	LLeaveOneOutMixingDialog.cpp
//
//	Authors:					Wei-Kang Hsu, Larry L. Biehl
//
//	Revision date:			01/26/2018 by Wei-Kang Hsu
//								11/16/2018 by Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMLOOMixingDialog class.
//	
//------------------------------------------------------------------------------------
                     
#include "SMultiSpec.h"
#include "LLeaveOneOutMixingDialog.h"

//#include	"SExternalGlobals.h"



CMLOOMixingDialog::CMLOOMixingDialog (
				wxWindow*							pParent, 
				wxWindowID							id, 
				const wxString&					title /*=NULL*/)
		: CMDialog (CMLOOMixingDialog::IDD, pParent, title)
		
{
	m_mixingParameterCode = -1;
	m_userMixingParameter = 0.0;

	m_initializedFlag = CMDialog::m_initializedFlag;

	m_loocMixingParameter = 0.0;
   
   CreateControls ();
   
}	// end "CMLOOMixingDialog"

 

BEGIN_EVENT_TABLE (CMLOOMixingDialog, CMDialog)
	EVT_INIT_DIALOG (CMLOOMixingDialog::OnInitDialog)
	EVT_RADIOBUTTON (IDC_UserMixing, CMLOOMixingDialog::OnUserMixing)
	EVT_RADIOBUTTON (IDC_OptimumMixing, CMLOOMixingDialog::OnOptimumMixing)
	EVT_RADIOBUTTON (IDC_IdentityMatrix, CMLOOMixingDialog::OnIdentityMatrix)
END_EVENT_TABLE ()



void CMLOOMixingDialog::CreateControls () 

{
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxBoxSizer* bSizer311;
	bSizer311 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer312;
	bSizer312 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer314;
	bSizer314 = new wxBoxSizer (wxHORIZONTAL);
	
	m_radioBtn36 = new wxRadioButton (this, 
													IDC_OptimumMixing, 
													wxT("Optimum Mixing Parameter (Computed)"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	bSizer314->Add (m_radioBtn36, 0, wxALL, 5);
	
	m_staticText291 = new wxStaticText (this, 
													IDC_OptimumValue, 
													wxT("MyLabel"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText291->Wrap (-1);
	bSizer314->Add (m_staticText291, 0, wxALIGN_CENTER|wxALL, 5);
	
	
	bSizer312->Add (bSizer314, 0, wxEXPAND|wxLEFT|wxRIGHT, 5);
	
	wxBoxSizer* bSizer315;
	bSizer315 = new wxBoxSizer (wxHORIZONTAL);
	
	m_radioBtn38 = new wxRadioButton (this, 
													IDC_UserMixing, 
													wxT("*User Set Mixing Parameter:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	bSizer315->Add (m_radioBtn38, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_textCtrl131 = new wxTextCtrl (this, 
												IDC_UserValue, 
												wxEmptyString, 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
	bSizer315->Add (m_textCtrl131, 0, wxALL, 5);
	
	
	bSizer312->Add (bSizer315, 0, wxEXPAND|wxLEFT|wxRIGHT, 5);
	
	wxBoxSizer* bSizer316;
	bSizer316 = new wxBoxSizer (wxHORIZONTAL);
	
	m_radioBtn37 = new wxRadioButton (this, 
													IDC_IdentityMatrix, 
													wxT("Use Identity Matrix for Covariance"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	bSizer316->Add (m_radioBtn37, 0, wxALL, 5);
	
	
	bSizer312->Add (bSizer316, 0, wxEXPAND|wxLEFT|wxRIGHT, 5);
	
	
	bSizer311->Add (bSizer312, 0, wxEXPAND, 5);
	
	wxBoxSizer* bSizer313;
	bSizer313 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText293 = new wxStaticText (
			this, 
			wxID_ANY, 
			wxT("*Note:\r\n 0-diagonal portion of class covariance\r\n 1-class covariance\r\n 2-common covariance\r\n 3-diagonal portion of common covariance\r\n\r\nexample: 1.3 represents .7 of class covariance and\r\n  .3 of common covariance."), 
			wxDefaultPosition, 
			wxDefaultSize, 
			0);
	m_staticText293->Wrap (-1);
	bSizer313->Add (m_staticText293, 0, wxALL, 5);
	
	
	bSizer311->Add (bSizer313, 0, wxEXPAND|wxLEFT, 20);
	/*
	wxBoxSizer* bSizer317;
	bSizer317 = new wxBoxSizer (wxHORIZONTAL);
	
	m_button76 = new wxButton (this, 
										wxID_CANCEL, 
										wxT("Cancel"), 
										wxDefaultPosition, 
										wxDefaultSize, 
										0);
	bSizer317->Add (m_button76, 0, wxALL, 5);
	
	m_button77 = new wxButton (this, 
										wxID_OK, 
										wxT("OK"), 
										wxDefaultPosition, 
										wxDefaultSize, 
										0);
	bSizer317->Add (m_button77, 0, wxALL, 5);
	
	
	bSizer311->Add (bSizer317, 0, wxALIGN_RIGHT, 5);
	*/
	CreateStandardButtons (bSizer311);
	
	this->SetSizer (bSizer311);
	this->Layout ();
	
	this->Centre (wxBOTH);
	
}	// end "CreateControls"



//-----------------------------------------------------------------------------
//								 Copyright (2009-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the leave one out
//							mixing parameter specification dialog box to the user and 
//							copy the revised back to the principal component 
//							specification structure if the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/07/2000
//	Revised By:			Larry L. Biehl			Date: 03/07/2000	

Boolean CMLOOMixingDialog::DoDialog (
				SInt16								statsWindowMode)

{                                 
	SInt16								returnCode;
	
	Boolean								continueFlag = FALSE;
								

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																			return (continueFlag);
																					
	m_statsWindowMode = statsWindowMode;
	
	returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{		
		LOOCOptionsDialogOK (statsWindowMode,
									m_mixingParameterCode+1,
									m_userMixingParameter);

		continueFlag = TRUE;
									
		}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog" 



void CMLOOMixingDialog::OnInitDialog (
				wxInitDialogEvent&				event) 

{
	SInt16						mixingParameterCode;


	LOOCOptionsDialogInitialize (this,
											m_statsWindowMode,
											&mixingParameterCode,
											&m_loocMixingParameter,
											&m_userMixingParameter); 

	m_mixingParameterCode = mixingParameterCode - 1;									
			
	LoadDItemRealValue (this, IDC_OptimumValue, m_loocMixingParameter, 3);
						  
	if (TransferDataToWindow ())                   
		PositionDialogWindow ();

			// Set default text selection to first edit text item	
																			  
	SelectDialogItemText (this, IDC_UserValue, 0, SInt16_MAX);
	
}	// end "OnInitDialog"



void CMLOOMixingDialog::OnUserMixing (
				wxCommandEvent&					event) 

{
	ShowDialogItem (this, IDC_UserValue);
	SelectDialogItemText (this, IDC_UserValue, 0, SInt16_MAX);

}	// end "OnUserMixing"



void CMLOOMixingDialog::OnOptimumMixing (
				wxCommandEvent&					event) 
{
	HideDialogItem (this, IDC_UserValue);
	
}	// end "OnIdentityMatrix"



void CMLOOMixingDialog::OnIdentityMatrix (
				wxCommandEvent&					event) 
				
{
	HideDialogItem (this, IDC_UserValue);
	
}	// end "OnIdentityMatrix"



bool CMLOOMixingDialog::TransferDataFromWindow ()

{
	wxTextCtrl* mix_param = (wxTextCtrl*)FindWindow (IDC_UserValue);
	wxRadioButton* opt_mixing = (wxRadioButton*)FindWindow (IDC_OptimumMixing);
	wxRadioButton* user_mixing = (wxRadioButton*)FindWindow (IDC_UserMixing);
	wxRadioButton* identity = (wxRadioButton*)FindWindow (IDC_IdentityMatrix);

	wxString m_userMixingParameter_string = mix_param->GetValue ();
	m_userMixingParameter_string.ToDouble (&m_userMixingParameter);

	if (opt_mixing->GetValue ())
		m_mixingParameterCode = 0;
	else if (user_mixing->GetValue ())
		m_mixingParameterCode = 1;
	else
		m_mixingParameterCode = 2;

	return true;
	 
}	// end "TransferDataFromWindow"


 
bool CMLOOMixingDialog::TransferDataToWindow ()

{
	wxTextCtrl* mix_param = (wxTextCtrl*)FindWindow (IDC_UserValue);
	wxStaticText* optimum = (wxStaticText*)FindWindow (IDC_OptimumValue);
	wxRadioButton* opt_mixing = (wxRadioButton*)FindWindow (IDC_OptimumMixing);
	wxRadioButton* user_mixing = (wxRadioButton*)FindWindow (IDC_UserMixing);
	wxRadioButton* identity = (wxRadioButton*)FindWindow (IDC_IdentityMatrix);
	mix_param->ChangeValue (wxString::Format (wxT("%.2f"), m_userMixingParameter));
	optimum->SetLabel (wxString::Format (wxT("%.3f"), m_loocMixingParameter));

	if (m_mixingParameterCode == 0)
		opt_mixing->SetValue (true);
	else if (m_mixingParameterCode == 1)
		user_mixing->SetValue (true);
	else
		identity->SetValue (true);

	return true;
	 
}	// end "TransferDataToWindow"
