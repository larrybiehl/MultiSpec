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
//	File:						xStatisticsListDialog.cpp : class implementation file
//	Class Definition:		xStatisticsListDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			02/24/2020
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMListStatsDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "xStatisticsListDialog.h"



BEGIN_EVENT_TABLE (CMListStatsDialog, CMDialog)
	EVT_CHECKBOX (IDC_Classes, CMListStatsDialog::OnClasses)
	EVT_CHECKBOX (IDC_Fields, CMListStatsDialog::OnFields)

	EVT_INIT_DIALOG (CMListStatsDialog::OnInitDialog)
END_EVENT_TABLE ()



CMListStatsDialog::CMListStatsDialog (
				wxWindow* 							pParent,
   			wxWindowID 							id,
   			const 								wxString& title/*=NULL*/)
		: CMDialog (CMListStatsDialog::IDD, pParent, title)

{
    m_listClassFlag = FALSE;
    m_listFieldFlag = FALSE;
    m_listMeansStdDevFlag = FALSE;
    m_listCovarianceFlag = FALSE;
    m_listCorrelationFlag = FALSE;
    m_featureTransformationFlag = FALSE;
    m_listMeanStdPrecision = 1;
    m_listCovCorPrecision = 2;
    
    CreateControls ();

}	// end "CMListStatsDialog"



void CMListStatsDialog::CreateControls ()

{
	this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxBoxSizer* bSizer107;
	bSizer107 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer108;
	bSizer108 = new wxBoxSizer (wxHORIZONTAL);
	
	wxGridSizer* gSizer13;
	gSizer13 = new wxGridSizer (0, 2, 0, 0);
	
	wxBoxSizer* bSizer256;
	bSizer256 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText258 = new wxStaticText (this,
													wxID_ANY,
													wxT("List Statistics For:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText258->Wrap (-1);
	bSizer256->Add (m_staticText258, 0, wxALL, 5);
	
	m_classCheckBox = new wxCheckBox (this,
													IDC_Classes,
													wxT("Classes"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	bSizer256->Add (m_classCheckBox, 0, wxALL, 5);
	
	m_fieldCheckBox = new wxCheckBox (this,
													IDC_Fields,
													wxT("Fields"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	bSizer256->Add (m_fieldCheckBox, 0, wxALL, 5);
	
	gSizer13->Add (bSizer256, 0, wxEXPAND, 5);
	
	wxBoxSizer* bSizer257;
	bSizer257 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText259 = new wxStaticText (this,
													wxID_ANY,
													wxT("Include:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText259->Wrap (-1);
	bSizer257->Add (m_staticText259, 0, wxALL, 5);
	
	m_checkBox46 = new wxCheckBox (this,
												IDC_MeansStdDev,
												wxT("Means & standard deviations"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer257->Add (m_checkBox46, 0, wxALL, 5);
	
	m_checkBox47 = new wxCheckBox (this,
												IDC_CovarianceMatrix,
												wxT("Covariance matrix"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer257->Add (m_checkBox47, 0, wxALL, 5);
	
	m_checkBox48 = new wxCheckBox (this,
												IDC_CorrelationMatrix,
												wxT("Correlation matrix"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer257->Add (m_checkBox48, 0, wxALL, 5);
	
	
	gSizer13->Add (bSizer257, 0, wxEXPAND, 5);
	
	
	bSizer108->Add (gSizer13, 0, wxFIXED_MINSIZE, 5);
	
	
	bSizer107->Add (bSizer108, 0, wxFIXED_MINSIZE|wxLEFT|wxRIGHT|wxTOP, 12);
	
	wxBoxSizer* bSizer260;
	bSizer260 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText264 = new wxStaticText (this,
													wxID_ANY,
													wxT("Other Options"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText264->Wrap (-1);
	bSizer260->Add (m_staticText264, 0, 0, 5);
	
	wxBoxSizer* bSizer271;
	bSizer271 = new wxBoxSizer (wxVERTICAL);
	
	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer (0, 2, 0, 0);
	fgSizer11->SetFlexibleDirection (wxBOTH);
	fgSizer11->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	m_staticText265 = new wxStaticText (
										this,
										wxID_ANY,
										wxT("Mean and Std. Dev. precision (decimal digits):"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
	m_staticText265->Wrap (-1);
	fgSizer11->Add (m_staticText265, 0, wxALIGN_CENTER_VERTICAL, 5);
	
	m_textCtrl125 = new wxTextCtrl (this,
												IDC_MeanPrecision,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (50, -1),
												0);
   m_textCtrl125->SetValidator (
								wxTextValidator (wxFILTER_DIGITS, &m_meanPrecisionString));
	fgSizer11->Add (m_textCtrl125, 0, wxALIGN_CENTER_VERTICAL, 5);
	
	m_staticText266 = new wxStaticText (
									this,
									wxID_ANY,
									wxT("Covariance/Correlation precision (decimal digits):"),
									wxDefaultPosition,
									wxDefaultSize,
									0);
	m_staticText266->Wrap (-1);
	fgSizer11->Add (m_staticText266, 0, wxALIGN_CENTER_VERTICAL, 5);
	
	m_textCtrl126 = new wxTextCtrl (this,
												IDC_CovariancePrecision,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (50, -1),
												0);
   m_textCtrl126->SetValidator (
   							wxTextValidator (wxFILTER_DIGITS, &m_covPrecisionString));
	fgSizer11->Add (m_textCtrl126, 0, wxALIGN_CENTER_VERTICAL, 5);
	
	bSizer271->Add (fgSizer11, 0, 0, 5);
	
	bSizer260->Add (bSizer271, 0, 0, 5);
	
	wxBoxSizer* bSizer275;
	bSizer275 = new wxBoxSizer (wxVERTICAL);
	
	m_checkBox49 = new wxCheckBox (this,
												IDC_UseTransformation,
												wxT("Use transformation"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer275->Add (m_checkBox49, 0, wxALL, 5);
	
	bSizer260->Add (bSizer275, 0, 0, 5);
	
	bSizer107->Add (bSizer260, 0, wxLEFT|wxRIGHT|wxTOP, 12);

	CreateStandardButtons (bSizer107);
	
	Centre (wxBOTH);
	
   SetSizerAndFit (bSizer107);
	
}	// end "CreateControls"



Boolean CMListStatsDialog::DoDialog (
				SInt16 								statsWindowMode)

{
	SInt16 								returnCode = wxID_CANCEL;

	Boolean 								continueFlag = FALSE;


    		// Make sure intialization has been completed.

	if (!m_initializedFlag)
																						return (FALSE);

    m_statsWindowMode = statsWindowMode;

	if (TransferDataFromWindow ())
		returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
    	{
		continueFlag = TRUE;

		ListStatsDialogOK (m_listFieldFlag,
								 m_listClassFlag,
								 m_listMeansStdDevFlag,
								 m_listCovarianceFlag,
								 m_listCorrelationFlag,
								 m_featureTransformationFlag,
								 m_listMeanStdPrecision,
								 m_listCovCorPrecision);

    	}	// end "if (returnCode == IDOK)"

	return (continueFlag);

}	// end "DoDialog"



void CMListStatsDialog::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	SInt16 								listCovCorPrecision,
											listMeanStdPrecision;


	wxDialog::OnInitDialog (event);

	ListStatsDialogInitialize (this,
											m_statsWindowMode,
											&m_listFieldFlag,
											&m_listClassFlag,
											&m_listMeansStdDevFlag,
											&m_listCovarianceFlag,
											&m_listCorrelationFlag,
											&m_featureTransformationFlag,
											&listMeanStdPrecision,
											&listCovCorPrecision);

	m_listMeanStdPrecision = listMeanStdPrecision;
	m_listCovCorPrecision = listCovCorPrecision;

	if (TransferDataToWindow ())
		PositionDialogWindow ();

			// Set default text selection to first edit text item

	SelectDialogItemText (this, IDC_MeanPrecision, 0, SInt16_MAX);

}	// end "OnInitDialog"



void CMListStatsDialog::CheckListFieldClassSettings (void)

{
	Boolean enableFlag = FALSE;


	if (m_listFieldFlag || m_listClassFlag)
		enableFlag = TRUE;

	FindWindow (wxID_OK)->Enable (enableFlag);

}	// end "CheckListFieldClassSettings"



void CMListStatsDialog::OnClasses (
				wxCommandEvent& 					event)

{
   m_listClassFlag = m_classCheckBox->GetValue ();

    CheckListFieldClassSettings ();

}	// end "OnClasses"



void CMListStatsDialog::OnFields (
				wxCommandEvent& 					event)

{
   m_listFieldFlag = m_fieldCheckBox->GetValue ();

	CheckListFieldClassSettings ();

}	// end "OnFields"



bool CMListStatsDialog::TransferDataFromWindow ()

{
   m_listClassFlag = m_classCheckBox->GetValue ();
	
   m_listFieldFlag = m_fieldCheckBox->GetValue ();
	
   wxCheckBox* listMeansStdDevFlag = (wxCheckBox*)FindWindow (IDC_MeansStdDev);
   m_listMeansStdDevFlag = listMeansStdDevFlag->GetValue ();
	
   wxCheckBox* listCovarianceFlag = (wxCheckBox*)FindWindow (IDC_CovarianceMatrix);
   m_listCovarianceFlag = listCovarianceFlag->GetValue ();
	
   wxCheckBox* listCorrelationFlag = (wxCheckBox*)FindWindow (IDC_CorrelationMatrix);
   m_listCorrelationFlag = listCorrelationFlag->GetValue ();
	
   wxCheckBox* featureTransformationFlag =
   											(wxCheckBox*)FindWindow (IDC_UseTransformation);
	m_featureTransformationFlag = featureTransformationFlag->GetValue ();
	
   wxTextCtrl* listMeanStdPrecision = (wxTextCtrl*)FindWindow (IDC_MeanPrecision);
   m_listMeanStdPrecision = wxAtoi (listMeanStdPrecision->GetValue ());
	
   wxTextCtrl* listCovCorPrecision = (wxTextCtrl*)FindWindow (IDC_CovariancePrecision);
	m_listCovCorPrecision = wxAtoi (listCovCorPrecision->GetValue ());
	
   return TRUE;
	
}	// end "TransferDataFromWindow"



bool CMListStatsDialog::TransferDataToWindow ()

{
   m_classCheckBox->SetValue (m_listClassFlag);
	
   m_fieldCheckBox->SetValue (m_listFieldFlag);
	
   wxCheckBox* listMeansStdDevFlag = (wxCheckBox*)FindWindow (IDC_MeansStdDev);
   listMeansStdDevFlag->SetValue (m_listMeansStdDevFlag);
	
   wxCheckBox* listCovarianceFlag = (wxCheckBox*)FindWindow (IDC_CovarianceMatrix);
   listCovarianceFlag->SetValue (m_listCovarianceFlag);
	
   wxCheckBox* listCorrelationFlag = (wxCheckBox*)FindWindow (IDC_CorrelationMatrix);
   listCorrelationFlag->SetValue (m_listCorrelationFlag);
	
   wxCheckBox* featureTransformationFlag =
   												(wxCheckBox*)FindWindow (IDC_UseTransformation);
  	featureTransformationFlag->SetValue (m_featureTransformationFlag);
   
   wxTextCtrl* listMeanStdPrecision = (wxTextCtrl*)FindWindow (IDC_MeanPrecision);
   listMeanStdPrecision->SetValue (
   								wxString::Format (wxT("%d"), (int) m_listMeanStdPrecision));
	
   wxTextCtrl* listCovCorPrecision = (wxTextCtrl*)FindWindow (IDC_CovariancePrecision);
   listCovCorPrecision->SetValue (
   								wxString::Format (wxT("%d"), (int) m_listCovCorPrecision));
   
   return TRUE;
	
}	// end "TransferDataToWindow"
