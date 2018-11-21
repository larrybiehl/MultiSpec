//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LChangeChannelDescriptionDialog.cpp : class implementation file
//	Class Definition:		LChangeChannelDescriptionDialog.h
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
//								CMChangeChannelDescriptionDlg class.
//
//------------------------------------------------------------------------------------
                   
#include "SMultiSpec.h"
#include "LChangeChannelDescriptionDialog.h"



CMChangeChannelDescriptionDlg::CMChangeChannelDescriptionDlg (
				wxWindow*							pParent,
				wxWindowID							id, 
				const wxString&					title/*=NULL*/)
							: CMDialog (CMChangeChannelDescriptionDlg::IDD, pParent, title)
	
{
   m_description = _T("");
	m_value = 0.0f;
   
   m_channelDescriptionPtr = NULL; 
	m_fileInfoPtr = NULL;
	m_channelValuesPtr = NULL;
	m_channelIndex = 0;
	m_lastSelectedTextItem = 0;
	m_changeFlag = FALSE;
	m_addModifiyDeleteCode = 1;
   
   m_initializedFlag = CMDialog::m_initializedFlag;
   CreateControls ();
   
}	// end "CMChangeChannelDescriptionDlg"



CMChangeChannelDescriptionDlg::~CMChangeChannelDescriptionDlg ()
 
{

}	// end "~CMChangeChannelDescriptionDlg"



BEGIN_EVENT_TABLE (CMChangeChannelDescriptionDlg, CMDialog)
EVT_INIT_DIALOG (CMChangeChannelDescriptionDlg::OnInitDialog)
EVT_BUTTON (IDC_NextChannelButton, CMChangeChannelDescriptionDlg::OnNextChannelButton)
EVT_BUTTON (IDC_PreviousChannelButton, 
			CMChangeChannelDescriptionDlg::OnPreviousChannelButton)
EVT_SET_FOCUS (CMChangeChannelDescriptionDlg::OnSetfocusDescription)
EVT_SET_FOCUS (CMChangeChannelDescriptionDlg::OnSetfocusValue)
EVT_RADIOBUTTON (IDC_AddModifyChannelDescriptionRadio, 
			CMChangeChannelDescriptionDlg::OnBnClickedAddModifyChannelDescriptionRadio)
EVT_RADIOBUTTON (IDC_DeleteChannelDescriptionRadio, 
			CMChangeChannelDescriptionDlg::OnBnClickedDeleteChannelDescriptionRadio)
END_EVENT_TABLE ()



void CMChangeChannelDescriptionDlg::CreateControls () 

{
	this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxBoxSizer* bSizer292;
	bSizer292 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer328;
	bSizer328 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer333;
	bSizer333 = new wxBoxSizer (wxHORIZONTAL);
	
	m_radioBtn38 = new wxRadioButton (this, 
													IDC_AddModifyChannelDescriptionRadio, 
													wxT("Modify Channel Information for Channel:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	bSizer333->Add (m_radioBtn38, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);
	
	m_staticText306 = new wxStaticText (this, 
													IDC_ChannelNumber, 
													wxEmptyString, 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText306->Wrap (-1);
	bSizer333->Add (m_staticText306, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	
	bSizer328->Add (bSizer333, 0, wxEXPAND, 12);
	
	wxBoxSizer* bSizer329;
	bSizer329 = new wxBoxSizer (wxHORIZONTAL);
	
	
	bSizer329->Add (30, 0, 0, wxEXPAND, 5);
	
	m_button84 = new wxButton (this, 
										IDC_PreviousChannelButton, 
										wxT("<-Prev channel"), 
										wxDefaultPosition, 
										wxDefaultSize, 
										0);
	bSizer329->Add (m_button84, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);
	
	m_button85 = new wxButton (this, 
										IDC_NextChannelButton, 
										wxT("Next channel->"), 
										wxDefaultPosition, 
										wxDefaultSize, 
										0);
	bSizer329->Add (m_button85, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);
	
	bSizer328->Add (bSizer329, 0, wxEXPAND, 12);
	
	wxBoxSizer* bSizer330;
	bSizer330 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText324 = new wxStaticText (this, 
													IDC_DescriptionPrompt, 
													wxT("         Description"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText324->Wrap (-1);
	bSizer330->Add (m_staticText324, 0, wxALL, 5);
	
	m_staticText325 = new wxStaticText (this, 
													IDC_DescriptionExample, 
													wxString::Format (
															wxT("           example: 0.45-0.52:%cm"), 
													wxChar (181)), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText325->Wrap (-1);
	bSizer330->Add (m_staticText325, 0, wxALL, 5);
	
	m_staticText326 = new wxStaticText (this, 
													IDC_DescriptionLimit, 
													wxT("           (Limit is 16 characters.)"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText326->Wrap (-1);
	bSizer330->Add (m_staticText326, 0, wxALL, 5);
	
	wxBoxSizer* bSizer334;
	bSizer334 = new wxBoxSizer (wxHORIZONTAL);
	
	
	bSizer334->Add (45, 0, 0, wxEXPAND, 5);
	
	m_textCtrl143 = new wxTextCtrl (this, 
												IDC_Description, 
												wxEmptyString, 
												wxDefaultPosition, 
												wxSize (200,30), 
												0);
	m_textCtrl143->SetMaxLength (16); 
	bSizer334->Add (m_textCtrl143, 0, wxALL, 5);
	
	
	bSizer330->Add (bSizer334, 0, wxEXPAND, 5);
	
	
	bSizer328->Add (bSizer330, 0, wxEXPAND, 12);
	
	wxBoxSizer* bSizer332;
	bSizer332 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText327 = new wxStaticText (this, 
													IDC_ValuePrompt, 
													wxT("         Value"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText327->Wrap (-1);
	bSizer332->Add (m_staticText327, 0, wxALL, 5);
	
	m_staticText328 = new wxStaticText (this, 
													IDC_ValueExample, 
													wxT("           example: 0.485"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText328->Wrap (-1);
	bSizer332->Add (m_staticText328, 0, wxALL, 5);
	
	m_staticText329 = new wxStaticText (this, 
													IDC_ValueLimit, 
													wxT("           (Limit is 10 digits.)"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText329->Wrap (-1);
	bSizer332->Add (m_staticText329, 0, wxALL, 5);
	
	wxBoxSizer* bSizer335;
	bSizer335 = new wxBoxSizer (wxHORIZONTAL);
	
	
	bSizer335->Add (45, 0, 0, wxEXPAND, 5);
	
	m_textCtrl144 = new wxTextCtrl (this, 
												IDC_Value, 
												wxEmptyString, 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
	m_textCtrl144->SetMaxLength (10); 
	bSizer335->Add (m_textCtrl144, 0, wxALL, 5);
	
	
	bSizer332->Add (bSizer335, 1, wxEXPAND, 5);
	
	
	bSizer328->Add (bSizer332, 0, wxEXPAND, 12);
	
	wxBoxSizer* bSizer336;
	bSizer336 = new wxBoxSizer (wxHORIZONTAL);
	
	m_radioBtn39 = new wxRadioButton (this, 
													IDC_DeleteChannelDescriptionRadio, 
													wxT("Delete Channel Information at end of file"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	bSizer336->Add (m_radioBtn39, 0, wxALL, 5);
	
	bSizer328->Add (bSizer336, 0, wxEXPAND);
	
	bSizer292->Add (bSizer328, 0, wxEXPAND | wxLEFT|wxTOP|wxRIGHT, 12);
	/*
	wxBoxSizer* bSizer378;
	bSizer378 = new wxBoxSizer (wxHORIZONTAL);
	
	bSizer378->Add (220, 0, 0, wxEXPAND, 5);
	
	m_button88 = new wxButton (this, 
										wxID_CANCEL, 
										wxT("Cancel"), 
										wxDefaultPosition, 
										wxDefaultSize, 
										0);
	bSizer378->Add (m_button88, 0, wxALL, 5);
	
	m_button89 = new wxButton (this, 
										wxID_OK, 
										wxT("OK"), 
										wxDefaultPosition, 
										wxDefaultSize, 
										0);
	bSizer378->Add (m_button89, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxRIGHT, 5);
	
	bSizer292->Add (bSizer378, 0, wxALL|wxEXPAND|wxRIGHT, 12);
	*/
	CreateStandardButtons (bSizer292);
	
	//this->SetSizer (bSizer292);
	SetSizerAndFit (bSizer292);
	
	//this->Centre (wxBOTH);
	
}	// end "CreateControls"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the CEM
//							specification dialog box to the user and copy the
//							revised back to the classify specification structure if
//							the user selected OK.
//
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:			
// 
//	Called By:			ModifyChannelDescriptionsViaKeyboard in SReformatUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/01/2016
//	Revised By:			Larry L. Biehl			Date: 01/10/2017	

SInt16 CMChangeChannelDescriptionDlg::DoDialog (
				FileInfoPtr							fileInfoPtr, 
				ChannelDescriptionPtr			channelDescriptionPtr, 
				float*								channelValuesPtr)

{     
	SInt16								returnCode,
											returnValue;
										

	m_fileInfoPtr = fileInfoPtr;						
	m_channelDescriptionPtr = channelDescriptionPtr; 
	m_channelValuesPtr = channelValuesPtr;

	returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{			
		OnOK();
			
		if (m_addModifiyDeleteCode == 1 && m_changeFlag)
			returnValue = 1;
					
		else if (m_addModifiyDeleteCode == 2)
			returnValue = 2;
			
		}	// end "if (returnCode == wxID_OK)"

	return (returnValue); 
	
}	// end "DoDialog"



void CMChangeChannelDescriptionDlg::OnBnClickedAddModifyChannelDescriptionRadio (
				wxCommandEvent&					event)

{
	m_addModifiyDeleteCode = 1;
	
	ShowDialogItem (this, IDC_ChannelNumber);
	ShowDialogItem (this, IDC_NextChannelButton);
	ShowDialogItem (this, IDC_PreviousChannelButton);
	ShowDialogItem (this, IDC_DescriptionPrompt);
	ShowDialogItem (this, IDC_DescriptionExample);
	ShowDialogItem (this, IDC_DescriptionLimit);
	ShowDialogItem (this, IDC_Description);
	ShowDialogItem (this, IDC_ValuePrompt);
	ShowDialogItem (this, IDC_ValueExample);
	ShowDialogItem (this, IDC_ValueLimit);
	ShowDialogItem (this, IDC_Value);
   
}	// end "OnBnClickedTranslateScaleRotateRadio"



void CMChangeChannelDescriptionDlg::OnBnClickedDeleteChannelDescriptionRadio (
				wxCommandEvent&					event)

{
	m_addModifiyDeleteCode = 2;
	
	HideDialogItem (this, IDC_ChannelNumber);
	HideDialogItem (this, IDC_NextChannelButton);
	HideDialogItem (this, IDC_PreviousChannelButton);
	HideDialogItem (this, IDC_DescriptionPrompt);
	HideDialogItem (this, IDC_DescriptionExample);
	HideDialogItem (this, IDC_DescriptionLimit);
	HideDialogItem (this, IDC_Description);
	HideDialogItem (this, IDC_ValuePrompt);
	HideDialogItem (this, IDC_ValueExample);
	HideDialogItem (this, IDC_ValueLimit);
	HideDialogItem (this, IDC_Value);
	
}	// end "OnBnClickedReprojectToRadio"



void CMChangeChannelDescriptionDlg::OnInitDialog (
				wxInitDialogEvent&				event) 

{ 
   m_channelIndex = 0;
   m_value = *m_channelValuesPtr;
	
   CMDialog::OnInitDialog (event);
	
	::LoadDItemValue (this, IDC_ChannelNumber, m_channelIndex+1);
	
	m_lastSelectedTextItem = IDC_Description;
   
   if (TransferDataToWindow ())                   
		PositionDialogWindow ();
   
}	// end "OnInitDialog"



void CMChangeChannelDescriptionDlg::OnNextChannelButton (
				wxCommandEvent&					event)

{		
	Update ();
	
	m_channelIndex++;
	if (m_channelIndex >= m_fileInfoPtr->numberChannels)
		m_channelIndex = 0;
							
	ModifyChannelDescriptionsChangeChannel (this, 
															m_channelDescriptionPtr, 
															m_channelValuesPtr,
															m_channelIndex,
															m_lastSelectedTextItem);
	
}	// end "OnNextChannelButton"



void CMChangeChannelDescriptionDlg::OnPreviousChannelButton (
				wxCommandEvent&					event)

{	
	Update ();
	
	m_channelIndex--;
	if (m_channelIndex < 0)
		m_channelIndex = m_fileInfoPtr->numberChannels - 1;
							
	ModifyChannelDescriptionsChangeChannel (this, 
															m_channelDescriptionPtr, 
															m_channelValuesPtr,
															m_channelIndex,
															m_lastSelectedTextItem);
	
}	// end "OnPreviousChannelButton"



void CMChangeChannelDescriptionDlg::OnOK ()

{
	Update ();
	
}	// end "OnOK"



void CMChangeChannelDescriptionDlg::OnSetfocusDescription (
				wxFocusEvent&						event) 

{
	m_lastSelectedTextItem = IDC_Description;
	
}	// end "OnSetfocusDescription"



void CMChangeChannelDescriptionDlg::OnSetfocusValue (
				wxFocusEvent&						event) 

{
	m_lastSelectedTextItem = IDC_Value;
	
}	// end "OnSetfocusValue"



bool CMChangeChannelDescriptionDlg::TransferDataFromWindow ()
 
{   
   wxTextCtrl* newDesText = (wxTextCtrl*)FindWindow (IDC_Description);
   wxTextCtrl* newValueText = (wxTextCtrl*)FindWindow (IDC_Value);
   
   m_description = newDesText->GetValue ();
   m_value = wxAtof (newValueText->GetValue ());	
	
   return TRUE;
	
}	// end "TransferDataFromWindow"



bool CMChangeChannelDescriptionDlg::TransferDataToWindow () 

{
   //wxTextCtrl* newDesText = (wxTextCtrl*)FindWindow (IDC_Description);
   wxTextCtrl* newValueText = (wxTextCtrl*)FindWindow (IDC_Value);
	
   LoadDescriptionIntoDItem (this, IDC_Description, m_channelDescriptionPtr);	
	
   newValueText->SetValue (wxString::Format (wxT("%.3f"), m_value));
	
	if (!m_fileInfoPtr->descriptionsFlag)
		SetDLogControlHilite (this, IDC_DeleteChannelDescriptionRadio, 255);
   
   return TRUE;
	
}	// end "TransferDataToWindow"



void CMChangeChannelDescriptionDlg::Update ()

{
	Str255						descriptionString;

	SInt16						stringLength;
	
	
			// Get the latest description for the current channel

	wxTextCtrl* description = (wxTextCtrl*)FindWindow (IDC_Description);
   m_description = description->GetValue ();
   
	MGetString (descriptionString, kFileIOStrID, IDS_BlankString16);
	stringLength = m_description.length ();
   
	BlockMoveData (m_description, &descriptionString[1], stringLength);
	
			// Get the new value

   wxTextCtrl* value = (wxTextCtrl*)FindWindow (IDC_Value);
   m_value = atof (value->GetValue ());
   
	m_changeFlag = ModifyChannelDescriptionsUpdate (this, 
																	m_channelDescriptionPtr, 
																	m_channelValuesPtr,
																	m_channelIndex,
																	(char*)&descriptionString,
																	m_value,
																	m_changeFlag);

}	// end "Update"
