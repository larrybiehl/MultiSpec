//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LListResultsOptionsDialog.cpp : class implementation file
//	Class Definition:		LListResultsOptionsDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/16/2017
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMListResultsOptionsDlg class.
//
// Following is template for debugging
/*
		int numberChars = sprintf ((char*)gTextString3,
									 " LListResultsOptionsDialog:: (): %s",
									 gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------
//
#include "SMultiSpec.h"
                     
#include "LListResultsOptionsDialog.h"

//#include	"SExternalGlobals.h"



BEGIN_EVENT_TABLE (CMListResultsOptionsDlg, CMDialog)
   EVT_INIT_DIALOG (CMListResultsOptionsDlg::OnInitDialog)
END_EVENT_TABLE ()



CMListResultsOptionsDlg::CMListResultsOptionsDlg (
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title)
	: CMDialog(CMListResultsOptionsDlg::IDD, pParent, title)

{
	m_trainAreasUsedFlag = FALSE;
	m_trainAreaSummariesFlag = FALSE;
	m_trainAreaPerformanceFlag = FALSE;
	m_trainClassPerformanceFlag = FALSE;
	m_testAreasUsedFlag = FALSE;
	m_testAreaSummariesFlag = FALSE;
	m_testAreaPerformanceFlag = FALSE;
	m_testClassPerformanceFlag = FALSE;

	m_initializedFlag = CMDialog::m_initializedFlag;
   CreateControls ();
   SetSizerAndFit (bSizer142);
	
}	// end "CMListResultsOptionsDlg"



void CMListResultsOptionsDlg::CreateControls()

{
	
   this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	//wxBoxSizer* bSizer142;
	bSizer142 = new wxBoxSizer( wxVERTICAL );
	
		wxBoxSizer* bSizer143;
	bSizer143 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText169 = new wxStaticText( this, wxID_ANY, wxT("Training Areas"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText169->Wrap( -1 );
	bSizer143->Add( m_staticText169, 0, wxALL, 5 );
	
	
	bSizer142->Add( bSizer143, 0, wxLEFT|wxRIGHT|wxTOP, 12 );
	
	wxBoxSizer* bSizer144;
	bSizer144 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox17 = new wxCheckBox( this, IDC_CHECK1, wxT("List Training Areas Used"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer144->Add( m_checkBox17, 0, wxALL, 3 );
	
	m_checkBox18 = new wxCheckBox( this, IDC_CHECK2, wxT("List Training Area Classification Summaries.\n(Includes likelihood probability if available.)"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer144->Add( m_checkBox18, 0, wxALL, 3 );
	
	m_checkBox19 = new wxCheckBox( this, IDC_CHECK3, wxT("List Training Area Perfomance Summary Table"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer144->Add( m_checkBox19, 0, wxALL, 3 );
	
	m_checkBox20 = new wxCheckBox( this, IDC_CHECK4, wxT("List Training Class Perfomance Summary Table"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer144->Add( m_checkBox20, 0, wxALL, 3 );
	
	
	bSizer142->Add( bSizer144, 0, wxLEFT|wxRIGHT, 30 );
	
	wxBoxSizer* bSizer145;
	bSizer145 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText170 = new wxStaticText( this, wxID_ANY, wxT("Test Areas"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText170->Wrap( -1 );
	bSizer145->Add( m_staticText170, 12, wxALL, 5 );
	
	
	bSizer142->Add( bSizer145, 0, wxLEFT|wxRIGHT|wxTOP, 12 );
	
	wxBoxSizer* bSizer146;
	bSizer146 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox21 = new wxCheckBox( this, IDC_CHECK5, wxT("List Test Areas Used"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer146->Add( m_checkBox21, 0, wxALL, 3 );
	
	m_checkBox22 = new wxCheckBox( this, IDC_CHECK6, wxT("List Test Area Classification Summaries"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer146->Add( m_checkBox22, 0, wxALL, 3 );
	
	m_checkBox23 = new wxCheckBox( this, IDC_CHECK7, wxT("List Test Area Performance Summary Table"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer146->Add( m_checkBox23, 0, wxALL, 3 );
	
	m_checkBox24 = new wxCheckBox( this, IDC_CHECK8, wxT("List Test Class Performance Summary Table"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer146->Add( m_checkBox24, 0, wxALL, 3 );
	
	bSizer142->Add (bSizer146, 0, wxLEFT|wxRIGHT, 30);
	
	CreateStandardButtons (bSizer142);
	
	this->SetSizer (bSizer142);
	this->Layout ();
	
	this->Centre (wxBOTH);
	
}	// end "CreateControls"



//-----------------------------------------------------------------------------
//								 Copyright (2009-2019)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the classification
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
//	Coded By:			Larry L. Biehl			Date: 04/09/97
//	Revised By:			Larry L. Biehl			Date: 04/09/97	

void CMListResultsOptionsDlg::DoDialog(
				SInt16*			listResultsTrainingCodePtr, 
				SInt16*			listResultsTestCodePtr)

{  
	Boolean						continueFlag = FALSE;
	
	SInt16						returnCode;
								

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																					return;
																			
	m_listResultsTestCode = *listResultsTestCodePtr;		
	m_listResultsTrainingCode = *listResultsTrainingCodePtr;
																					
	returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{              
				// Codes for training and test listings.
						 
		*listResultsTestCodePtr = m_listResultsTestCode;
		*listResultsTrainingCodePtr = m_listResultsTrainingCode;
				
				// List Results options for training areas.	
				
		*listResultsTrainingCodePtr = 0;		
		if ( m_trainAreasUsedFlag )
			*listResultsTrainingCodePtr += kAreasUsed;
							
		if ( m_trainAreaSummariesFlag )
			*listResultsTrainingCodePtr += kAreasSummary;	
						
		if ( m_trainAreaPerformanceFlag )
			*listResultsTrainingCodePtr += kFieldSummary;	
						
		if ( m_trainClassPerformanceFlag )
			*listResultsTrainingCodePtr += kClassSummary;
				
				// List Results options for test areas.	
				
		*listResultsTestCodePtr = 0;		
		if ( m_testAreasUsedFlag )
			*listResultsTestCodePtr += kAreasUsed;
							
		if ( m_testAreaSummariesFlag )
			*listResultsTestCodePtr += kAreasSummary;	
						
		if ( m_testAreaPerformanceFlag )
			*listResultsTestCodePtr += kFieldSummary;	
						
		if ( m_testClassPerformanceFlag )
			*listResultsTestCodePtr += kClassSummary;
				
		continueFlag = TRUE; 
		
		}		// end "if (returnCode == IDOK)"
		
	return;
		
}	// end "DoDialog"



void CMListResultsOptionsDlg::OnInitDialog (wxInitDialogEvent& event)
{
//	CMDialog::OnInitDialog(); 
	
			// Set current settings	for training set 
			
	m_trainAreasUsedFlag = ( (m_listResultsTrainingCode & kAreasUsed) > 0);
	m_trainAreaSummariesFlag = ( (m_listResultsTrainingCode & kAreasSummary) > 0);
	m_trainAreaPerformanceFlag = ( (m_listResultsTrainingCode & kFieldSummary) > 0);
	m_trainClassPerformanceFlag = ( (m_listResultsTrainingCode & kClassSummary) > 0);
	
			// Set current settings	for test set	
	
	m_testAreasUsedFlag = ( (m_listResultsTestCode & kAreasUsed) > 0);
	m_testAreaSummariesFlag = ( (m_listResultsTestCode & kAreasSummary) > 0);
	m_testAreaPerformanceFlag = ( (m_listResultsTestCode & kFieldSummary) > 0);
	m_testClassPerformanceFlag = ( (m_listResultsTestCode & kClassSummary) > 0);   
	                 
	if (TransferDataToWindow())                   
		PositionDialogWindow ();
	
	//return TRUE;  // return TRUE  unless you set the focus to a control
   
	
}		// end "OnInitDialog"



bool CMListResultsOptionsDlg::TransferDataFromWindow ()

{
   wxCheckBox* check1 = (wxCheckBox*) FindWindow(IDC_CHECK1);
   wxCheckBox* check2 = (wxCheckBox*) FindWindow(IDC_CHECK2);
   wxCheckBox* check3 = (wxCheckBox*) FindWindow(IDC_CHECK3);
   wxCheckBox* check4 = (wxCheckBox*) FindWindow(IDC_CHECK4);
   wxCheckBox* check5 = (wxCheckBox*) FindWindow(IDC_CHECK5);
   wxCheckBox* check6 = (wxCheckBox*) FindWindow(IDC_CHECK6);
   wxCheckBox* check7 = (wxCheckBox*) FindWindow(IDC_CHECK7);
   wxCheckBox* check8 = (wxCheckBox*) FindWindow(IDC_CHECK8);
	
   m_trainAreasUsedFlag = check1->GetValue();
   m_trainAreaSummariesFlag = check2->GetValue();
   m_trainAreaPerformanceFlag = check3->GetValue();
   m_trainClassPerformanceFlag = check4->GetValue();
   m_testAreasUsedFlag = check5->GetValue();
   m_testAreaSummariesFlag = check6->GetValue();
   m_testAreaPerformanceFlag = check7->GetValue();
   m_testClassPerformanceFlag = check8->GetValue();
	
	return true;
	
}



bool CMListResultsOptionsDlg::TransferDataToWindow ()

{
   wxCheckBox* check1 = (wxCheckBox*)FindWindow(IDC_CHECK1);
   wxCheckBox* check2 = (wxCheckBox*)FindWindow(IDC_CHECK2);
   wxCheckBox* check3 = (wxCheckBox*)FindWindow(IDC_CHECK3);
   wxCheckBox* check4 = (wxCheckBox*)FindWindow(IDC_CHECK4);
   wxCheckBox* check5 = (wxCheckBox*)FindWindow(IDC_CHECK5);
   wxCheckBox* check6 = (wxCheckBox*)FindWindow(IDC_CHECK6);
   wxCheckBox* check7 = (wxCheckBox*)FindWindow(IDC_CHECK7);
   wxCheckBox* check8 = (wxCheckBox*)FindWindow(IDC_CHECK8);
	
   check1->SetValue(m_trainAreasUsedFlag);
   check2->SetValue(m_trainAreaSummariesFlag);
   check3->SetValue(m_trainAreaPerformanceFlag);
   check4->SetValue(m_trainClassPerformanceFlag);
   check5->SetValue(m_testAreasUsedFlag);
   check6->SetValue(m_testAreaSummariesFlag);
   check7->SetValue(m_testAreaPerformanceFlag);
   check8->SetValue(m_testClassPerformanceFlag);
	
   return true;
}

