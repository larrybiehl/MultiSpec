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
//	File:						xEvaluateTransformationDialog.cpp : class implementation file
//	Class Definition:		xEvaluateTransformationDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			10/29/2019
//
//	Language:				C++
//
//	System:					Linux & MacOS Operating Systems
//
//	Brief description:	This file contains functions related to the
//								CMEvalTransformDialog class.
//
// Following is template for debugging
/*
	int numberChars = sprintf ((char*)gTextString3,
									 " xEvaluateTransformationDialog:: (): %s",
									 gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
                      
#include "xEvaluateTransformationDialog.h"



BEGIN_EVENT_TABLE (CMEvalTransformDialog, CMDialog)
	EVT_INIT_DIALOG (CMEvalTransformDialog::OnInitDialog)
	EVT_CHECKBOX (IDC_CheckTransform, CMEvalTransformDialog::OnCheckTransform)
END_EVENT_TABLE ()



CMEvalTransformDialog::CMEvalTransformDialog (
				wxWindow* 							pParent,
   			wxWindowID 							id,
   			const wxString& 					title/*=NULL*/)
		:CMDialog (CMEvalTransformDialog::IDD, pParent, title)

{
   m_checkTransformationFlag = FALSE;
	m_listCheckMatrixFlag = FALSE;
	m_listEigenvaluesFlag = FALSE;
	m_listOriginalMatrixFlag = FALSE;
   
   m_initializedFlag = CMDialog::m_initializedFlag;
   
   CreateControls ();
   
}	// end "CMEvalTransformDialog"



CMEvalTransformDialog::~CMEvalTransformDialog (void)

{

}	// end "~CMEvalTransformDialog"



void CMEvalTransformDialog::CreateControls ()

{
   SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxBoxSizer* bSizer107;
	bSizer107 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer108;
	bSizer108 = new wxBoxSizer (wxHORIZONTAL);
	
	wxBoxSizer* bSizer175;
	bSizer175 = new wxBoxSizer (wxVERTICAL);
	
	m_checkBox14 = new wxCheckBox (this,
												IDC_ListEigenvalues,
												wxT("List Eigenvalues"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox14, IDS_ToolTip243);
	bSizer175->Add (m_checkBox14, 0, wxALL, 5);
	
	m_checkBox15 = new wxCheckBox (this,
												IDC_ListCheckMatrix,
												wxT("List transformation matrix (eigenvectors)"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox15, IDS_ToolTip243);
	bSizer175->Add (m_checkBox15, 0, wxALL, 5);
	
	m_checkBox16 = new wxCheckBox (this,
												IDC_CheckTransform,
												wxT("Check Transformation matrix"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox16, IDS_ToolTip248);
	bSizer175->Add (m_checkBox16, 0, wxALL, 5);
	
	bSizer108->Add (bSizer175, 1, wxEXPAND, 5);
	
	bSizer107->Add (bSizer108, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12);
	
	wxBoxSizer* bSizer109;
	bSizer109 = new wxBoxSizer (wxVERTICAL);
	
	m_checkBox23 = new wxCheckBox (this,
												IDC_ListTransform,
												wxT("List Transposed Transformation matrix"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox23, IDS_ToolTip243);
	bSizer109->Add (m_checkBox23, 0, wxLEFT, 30);
	
	m_staticText196 = new wxStaticText (
							this,
							IDC_ListCheckMatrix2,
							wxT("times Transformation matrix.\n(Should be identity matrix)"),
							wxDefaultPosition,
							wxDefaultSize,
							0);
	m_staticText196->Wrap (-1);
	bSizer109->Add (m_staticText196, 0, wxLEFT, 55);
	
	bSizer107->Add (bSizer109, 0, wxEXPAND|wxLEFT|wxRIGHT, 12);

	CreateStandardButtons (bSizer107);
	
	Layout ();

   SetSizerAndFit (bSizer107);
	
}	// end "CreateControls"



Boolean CMEvalTransformDialog::DoDialog (
				EvaluateTransformSpecsPtr		evaluateTransformSpecsPtr)

{
	SInt16						returnCode = -1;
   Boolean						continueFlag = FALSE;

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
																			
	m_evaluateTransformSpecsPtr = evaluateTransformSpecsPtr;
	
   if (TransferDataFromWindow ())
      returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{
		continueFlag = TRUE;
					
				// List options.	
			
		EvaluateTransformationOK (evaluateTransformSpecsPtr,
											m_listEigenvaluesFlag,
											m_listOriginalMatrixFlag,
											m_checkTransformationFlag,
											m_listCheckMatrixFlag);

		}	// end "if (returnCode == IDOK)"
	
   else
      continueFlag = FALSE;
		
	return (continueFlag);
	
}	// end "DoDialog"



void CMEvalTransformDialog::OnCheckTransform (
				wxCommandEvent& 					event)

{
	wxCheckBox* m_dialogFromPtr =
										(wxCheckBox*)wxWindow::FindWindow (IDC_CheckTransform);
   m_checkTransformationFlag = m_dialogFromPtr->GetValue ();
	SetListCheckTransform (m_checkTransformationFlag);
	
}	// end "OnCheckTransform"



void CMEvalTransformDialog::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	CMDialog::OnInitDialog (event);
	
			// Initialize dialog variables.

	EvaluateTransformationInitialize (this,
													m_evaluateTransformSpecsPtr,
													&m_listEigenvaluesFlag,
													&m_listOriginalMatrixFlag,
													&m_checkTransformationFlag,
													&m_listCheckMatrixFlag);
	                                                                       
	SetListCheckTransform (m_checkTransformationFlag);
	                  
	if (TransferDataToWindow ())
		PositionDialogWindow ();
	
}	// end "OnInitDialog"



void CMEvalTransformDialog::SetListCheckTransform (
				Boolean								checkTransformFlag)

{
	if (checkTransformFlag)
		{ 
		MShowDialogItem (this, IDC_ListCheckMatrix2);
		MShowDialogItem (this, IDC_ListTransform);
		
		}	// end "if (checkTransformFlag)"
		
	else	// !checkTransformFlag
		{
      MShowDialogItem (this, IDC_CheckTransform);
      MShowDialogItem (this, IDC_ListCheckMatrix);
		MHideDialogItem (this, IDC_ListCheckMatrix2); 
		MHideDialogItem (this, IDC_ListTransform);
		
		}	// end "else !checkTransformFlag"
	
}	// end "SetListCheckTransform"



bool CMEvalTransformDialog::TransferDataFromWindow ()

{
	
   wxCheckBox* listEigenvaluesFlag = (wxCheckBox*)FindWindow (IDC_ListEigenvalues);
   m_listEigenvaluesFlag = listEigenvaluesFlag->GetValue ();
	
   wxCheckBox* listOriginalMatrixFlag = (wxCheckBox*)FindWindow (IDC_ListCheckMatrix);
   m_listOriginalMatrixFlag = listOriginalMatrixFlag->GetValue ();
	
   wxCheckBox* checkTransformationFlag = (wxCheckBox*)FindWindow (IDC_CheckTransform);
   m_checkTransformationFlag = checkTransformationFlag->GetValue ();
	
  	wxCheckBox* listCheckMatrixFlag = (wxCheckBox*)FindWindow (IDC_ListTransform);
   m_listCheckMatrixFlag = listCheckMatrixFlag->GetValue ();
	
   return TRUE;
	
}	// end "TransferDataFromWindow"



bool CMEvalTransformDialog::TransferDataToWindow ()

{
	
   wxCheckBox* listEigenvaluesFlag = (wxCheckBox*)FindWindow (IDC_ListEigenvalues);
   listEigenvaluesFlag->SetValue (m_listEigenvaluesFlag);
	
   wxCheckBox* listOriginalMatrixFlag = (wxCheckBox*)FindWindow (IDC_ListCheckMatrix);
   listOriginalMatrixFlag->SetValue (m_listOriginalMatrixFlag);
	
   wxCheckBox* checkTransformationFlag = (wxCheckBox*)FindWindow (IDC_CheckTransform);
   checkTransformationFlag->SetValue (m_checkTransformationFlag);
	
   wxCheckBox* listCheckMatrixFlag = (wxCheckBox*)FindWindow (IDC_ListTransform);
   listCheckMatrixFlag->SetValue (m_listCheckMatrixFlag);
	
   return TRUE;
	
}	// end "TransferDataToWindow"
