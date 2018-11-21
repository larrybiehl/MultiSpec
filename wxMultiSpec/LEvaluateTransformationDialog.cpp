// LEvaluateTransformationDialog.cpp : implementation file
//                  
// Revised by Larry Biehl on 06/21/2017
//
#include "SMultiSpec.h"
                      
#include "LEvaluateTransformationDialog.h"

//#include	"SExternalGlobals.h"  

extern void 		EvaluateTransformationInitialize (
							DialogPtr							dialogPtr,
							EvaluateTransformSpecsPtr		evaluateTransformSpecsPtr,
							Boolean*								listEigenvaluesFlagPtr,
							Boolean*								listOriginalMatrixFlagPtr,
							Boolean*								checkTransformationFlagPtr,
							Boolean*								listOriginalXInvertedMatrixFlagPtr);
							
extern void 		EvaluateTransformationOK (
							EvaluateTransformSpecsPtr		evaluateTransformSpecsPtr,
							Boolean								listEigenvaluesFlag,
							Boolean								listOriginalMatrixFlag,
							Boolean								checkTransformationFlag,
							Boolean								listOriginalXInvertedMatrixFlag);

/////////////////////////////////////////////////////////////////////////////
// CMEvalTransformDialog dialog

CMEvalTransformDialog::CMEvalTransformDialog(wxWindow* pParent,
   wxWindowID id, const wxString& title/*=NULL*/)
:CMDialog(CMEvalTransformDialog::IDD, pParent, title)
{
   m_checkTransformationFlag = FALSE;
	m_listCheckMatrixFlag = FALSE;
	m_listEigenvaluesFlag = FALSE;
	m_listOriginalMatrixFlag = FALSE;
   
   m_initializedFlag = CMDialog::m_initializedFlag;
   
   CreateControls();
   //SetSizerAndFit(bSizer107);
   
}

CMEvalTransformDialog::~CMEvalTransformDialog(void)
{  
}

BEGIN_EVENT_TABLE(CMEvalTransformDialog, CMDialog)
EVT_INIT_DIALOG(CMEvalTransformDialog::OnInitDialog)
EVT_CHECKBOX(IDC_CheckTransform, CMEvalTransformDialog::OnCheckTransform)

END_EVENT_TABLE()

bool CMEvalTransformDialog::TransferDataToWindow(){
   wxCheckBox* listEigenvaluesFlag = (wxCheckBox*) FindWindow(IDC_ListEigenvalues);
   wxCheckBox* listOriginalMatrixFlag = (wxCheckBox*) FindWindow(IDC_ListCheckMatrix);
   wxCheckBox* checkTransformationFlag = (wxCheckBox*) FindWindow(IDC_CheckTransform);
   wxCheckBox* listCheckMatrixFlag = (wxCheckBox*) FindWindow(IDC_ListTransform);
   
   listEigenvaluesFlag->SetValue(m_listEigenvaluesFlag);
   listOriginalMatrixFlag->SetValue(m_listOriginalMatrixFlag);
   checkTransformationFlag->SetValue(m_checkTransformationFlag);
   listCheckMatrixFlag->SetValue(m_listCheckMatrixFlag);
   
   return TRUE;
}

bool CMEvalTransformDialog::TransferDataFromWindow(){
   wxCheckBox* listEigenvaluesFlag = (wxCheckBox*) FindWindow(IDC_ListEigenvalues);
   wxCheckBox* listOriginalMatrixFlag = (wxCheckBox*) FindWindow(IDC_ListCheckMatrix);
   wxCheckBox* checkTransformationFlag = (wxCheckBox*) FindWindow(IDC_CheckTransform);
   wxCheckBox* listCheckMatrixFlag = (wxCheckBox*) FindWindow(IDC_ListTransform);
   
   m_listEigenvaluesFlag = listEigenvaluesFlag->GetValue();
   m_listOriginalMatrixFlag = listOriginalMatrixFlag->GetValue();
   m_checkTransformationFlag = checkTransformationFlag->GetValue();
   m_listCheckMatrixFlag = listCheckMatrixFlag->GetValue();
   
   return TRUE;
  
}

//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
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
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/20/97
//	Revised By:			Larry L. Biehl			Date: 05/13/98	

Boolean 
CMEvalTransformDialog::DoDialog(
				EvaluateTransformSpecsPtr		evaluateTransformSpecsPtr)

{
   Boolean						continueFlag = FALSE; 
	
	SInt16						returnCode;					

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
																			
	m_evaluateTransformSpecsPtr = evaluateTransformSpecsPtr;
	
   if(TransferDataFromWindow()){
      returnCode = ShowModal();
      
   }
	
	if (returnCode == wxID_OK)
		{
		continueFlag = TRUE;
					
				// List options.	
			
		EvaluateTransformationOK (evaluateTransformSpecsPtr,
											m_listEigenvaluesFlag,
											m_listOriginalMatrixFlag,
											m_checkTransformationFlag,
											m_listCheckMatrixFlag );														

		}		// end "if (returnCode == IDOK)"
   else{
      printf("cancel\n");
      continueFlag = FALSE;
   }
		
	return (continueFlag);
} // end "DoDialog"

/////////////////////////////////////////////////////////////////////////////
// CMEvalTransformDialog message handlers

void  
CMEvalTransformDialog::OnInitDialog(wxInitDialogEvent& event)

{
	CMDialog::OnInitDialog(event);  
	
			// Initialize dialog variables.

	EvaluateTransformationInitialize (this,
													m_evaluateTransformSpecsPtr,
													(Boolean*)&m_listEigenvaluesFlag,
													(Boolean*)&m_listOriginalMatrixFlag,
													(Boolean*)&m_checkTransformationFlag,
													(Boolean*)&m_listCheckMatrixFlag);
	                                                                       
	SetListCheckTransform (m_checkTransformationFlag);
	                  
	if (TransferDataToWindow() )                   
		PositionDialogWindow ();
	
	//return TRUE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"

void 
CMEvalTransformDialog::OnCheckTransform(wxCommandEvent& event)

{                                                                                          
	//DDX_Check(m_dialogFromPtr, IDC_CheckTransform, m_checkTransformationFlag);
	wxCheckBox* m_dialogFromPtr = (wxCheckBox*)wxWindow::FindWindow(IDC_CheckTransform);
   m_checkTransformationFlag = m_dialogFromPtr->GetValue(); 
	SetListCheckTransform (m_checkTransformationFlag);
	
}		// end "OnCheckTransform"    

void 
CMEvalTransformDialog::SetListCheckTransform(
				Boolean				checkTransformFlag)

{ 
   
	if (checkTransformFlag)
		{ 
		MShowDialogItem (this, IDC_ListCheckMatrix2);
		MShowDialogItem (this, IDC_ListTransform);
      //MShowDialogItem (this, IDC_ListCheckMatrix3);
		
		}		// end "if (checkTransformFlag)"
		
	else		// !checkTransformFlag
		{
      MShowDialogItem (this, IDC_CheckTransform);
      MShowDialogItem (this, IDC_ListCheckMatrix);
		MHideDialogItem (this, IDC_ListCheckMatrix2); 
		MHideDialogItem (this, IDC_ListTransform);
      //MHideDialogItem (this, IDC_ListCheckMatrix3);
		
		}		// end "else !checkTransformFlag"
	
}		// end "SetListCheckTransform"



void CMEvalTransformDialog::CreateControls ()

{
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxBoxSizer* bSizer107;
	bSizer107 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer108;
	bSizer108 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer175;
	bSizer175 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox14 = new wxCheckBox( this, IDC_ListEigenvalues, wxT("List Eigenvalues"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox14, IDS_ToolTip243);
	bSizer175->Add( m_checkBox14, 0, wxALL, 5 );
	
	m_checkBox15 = new wxCheckBox( this, IDC_ListCheckMatrix, wxT("List transformation matrix (eigenvectors)"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox15, IDS_ToolTip243);
	bSizer175->Add( m_checkBox15, 0, wxALL, 5 );
	
	m_checkBox16 = new wxCheckBox( this, IDC_CheckTransform, wxT("Check Transformation matrix"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox16, IDS_ToolTip248);
	bSizer175->Add( m_checkBox16, 0, wxALL, 5 );
	
	bSizer108->Add( bSizer175, 1, wxEXPAND, 5 );
	
	bSizer107->Add( bSizer108, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );
	
	wxBoxSizer* bSizer109;
	bSizer109 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox23 = new wxCheckBox( this, IDC_ListTransform, wxT("List Transposed Transformation matrix"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox23, IDS_ToolTip243);
	bSizer109->Add( m_checkBox23, 0, wxLEFT, 30 );
	
	m_staticText196 = new wxStaticText( this, IDC_ListCheckMatrix2, wxT("times Transformation matrix.\n(Should be identity matrix)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText196->Wrap( -1 );
	bSizer109->Add( m_staticText196, 0, wxLEFT, 55 );
	
	
	bSizer107->Add( bSizer109, 0, wxEXPAND|wxLEFT|wxRIGHT, 12 );
	/*
	wxBoxSizer* bSizer110;
	bSizer110 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button20 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer110->Add( m_button20, 0, wxALL, 5 );
	
	m_button21 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer110->Add( m_button21, 0, wxALL, 5 );
	
	bSizer107->Add( bSizer110, 0, wxALIGN_RIGHT|wxBOTTOM|wxRIGHT, 12 );
	*/
	CreateStandardButtons (bSizer107);
	

	//this->SetSizer (bSizer107);
	this->Layout ();
	
	//this->Centre (wxBOTH);

   SetSizerAndFit (bSizer107);
	
}	// end "CreateControls"
