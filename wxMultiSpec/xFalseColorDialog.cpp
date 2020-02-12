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
//	File:						xFalseColorDialog.cpp : class implementation file
//	Class Definition:		xFalseColorDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/13/2019
//
//	Language:				C++
//
//	System:					Linux & MacOS Operating Systems
//
//	Brief description:	This file contains functions related to the
//								CMFalseColorDlg class.
//
// Following is template for debugging
/*
	int numberChars = sprintf ((char*)gTextString3,
									 " xFalseColorDialog:: (): %s",
									 gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h"
                        
#include "SDisplay_class.h"
#include "xFalseColorDialog.h" 


BEGIN_EVENT_TABLE (CMFalseColorDlg, CMDialog)
	EVT_INIT_DIALOG (CMFalseColorDlg::OnInitDialog)
END_EVENT_TABLE ()



CMFalseColorDlg::CMFalseColorDlg (
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title)
		: CMDialog (CMFalseColorDlg::IDD, pParent, title)

{
	m_redChannel = 1;
	m_greenChannel = 1;
	m_blueChannel = 1;
	
	m_layerInfoPtr = NULL;
	m_fileInfoPtr = NULL;
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
	m_maxChannelFeatureNum = 1; 
	
   CreateControls ();
   SetSizerAndFit (bVSizerMain);
	
}	// end "CMFalseColorDlg"



void CMFalseColorDlg::CreateControls ()

{
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	bVSizerMain = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer322;
	bSizer322 = new wxBoxSizer (wxHORIZONTAL);
	
	wxBoxSizer* bSizer323;
	bSizer323 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer325;
	bSizer325 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText293 = new wxStaticText (this,
													wxID_ANY,
													wxT("Red:"),
													wxDefaultPosition,
													wxSize (45, -1),
													0);
	m_staticText293->Wrap (-1);
	bSizer325->Add (m_staticText293, 0, wxALL, 5);
	
	m_textCtrl132 = new wxTextCtrl (this,
												IDC_EDITRed,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (60, -1),
												0);
	bSizer325->Add (m_textCtrl132, 0, wxALL, 5);
	
	bSizer323->Add (bSizer325, 0, wxALIGN_CENTER|wxALL, 5);
	
	wxBoxSizer* bSizer326;
	bSizer326 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText294 = new wxStaticText (this,
													wxID_ANY,
													wxT("Green:"),
													wxDefaultPosition,
													wxSize (45, -1),
													0);
	m_staticText294->Wrap (-1);
	bSizer326->Add (m_staticText294, 0, wxALL, 5);
	
	m_textCtrl133 = new wxTextCtrl (this,
												IDC_EDITGreen,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (60, -1),
												0);
	bSizer326->Add (m_textCtrl133, 0, wxALL, 5);
	
	
	bSizer323->Add (bSizer326, 0, wxALIGN_CENTER|wxALL, 5);
	
	wxBoxSizer* bSizer327;
	bSizer327 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText295 = new wxStaticText (this,
													wxID_ANY,
													wxT("Blue:"),
													wxDefaultPosition,
													wxSize (45, -1),
													0);
	m_staticText295->Wrap (-1);
	bSizer327->Add (m_staticText295, 0, wxALL, 5);
	
	m_textCtrl134 = new wxTextCtrl (this,
												IDC_EDITBlue,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (60, -1),
												0);
	bSizer327->Add (m_textCtrl134, 0, wxALL, 5);
	
	bSizer323->Add (bSizer327, 0, wxALIGN_CENTER|wxALL, 5);
	
	bSizer322->Add (bSizer323, 1, wxALIGN_CENTER, 5);
	
	wxBoxSizer* bSizer324;
	bSizer324 = new wxBoxSizer(wxVERTICAL);
	
	wxBoxSizer* bSizer328;
	bSizer328 = new wxBoxSizer(wxVERTICAL);
	
	m_staticText296 = new wxStaticText (this,
													wxID_ANY,
													wxT("Available Channels"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText296->Wrap (-1);
	bSizer328->Add (m_staticText296, 0, wxLEFT, 5);
	
	m_listBox5 = new wxListBox (this,
											IDC_List1,
											wxDefaultPosition,
											wxSize (180, 160),
											0,
											NULL,
											0);
	bSizer328->Add (m_listBox5, 0, wxALL, 5);

   bSizer324->Add (bSizer328, 0, wxEXPAND, 5);

	bSizer322->Add (bSizer324, 0, wxALIGN_CENTER|wxALL, 5);
	
	bVSizerMain->Add (bSizer322, 0);
	
	CreateStandardButtons (bVSizerMain);
	
	SetSizer (bVSizerMain);
	Layout ();
	Fit ();
	Centre (wxBOTH);
	
}	// end "CreateControls"



Boolean CMFalseColorDlg::DoDialog (void)

{
	SInt16			returnCode;
	Boolean			continueFlag = FALSE;

	
			// Make sure intialization has been completed.
	
	if (!m_initializedFlag || gProjectInfoPtr == NULL)
																						return (FALSE);
	
	returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{
		continueFlag = TRUE;
	
				// Cell Width.
		
		gProjectInfoPtr->falseColorPaletteRed = m_redChannelIndex;
		gProjectInfoPtr->falseColorPaletteGreen = m_greenChannelIndex;
		gProjectInfoPtr->falseColorPaletteBlue = m_blueChannelIndex;
		
		}	// end "if (returnCode == IDOK)"
	
   else	// returnCode != wxID_OK
      SelectDialogItemText (this, (SInt16)IDC_EDITRed, 0, SInt16_MAX);
	
	return (continueFlag);
	
}	// end "DoDialog"



void CMFalseColorDlg::OnInitDialog (
				wxInitDialogEvent&				event)

{
	wxListBox*							listBoxPtr = NULL;
	WindowInfoPtr						projectWindowInfoPtr;
	
	SInt16								projectHandleStatus;
	
	
			// Get Project window information structure pointer.
	
	GetProjectImageFileInfo (kDoNotPrompt,
										kSetupGlobalInfoPointersIfCan,
										&projectWindowInfoPtr,
										&m_layerInfoPtr,
										&m_fileInfoPtr,
										&projectHandleStatus);
	
			// Load default project values if needed.
	
	LoadDefaultProjectFalseColorChannels ();
	
			// Get default values.
	
	m_redChannel =
			gProjectInfoPtr->channelsPtr[ gProjectInfoPtr->falseColorPaletteRed ] + 1;
	m_greenChannel =
			gProjectInfoPtr->channelsPtr[ gProjectInfoPtr->falseColorPaletteGreen ] + 1;
	m_blueChannel =
			gProjectInfoPtr->channelsPtr[ gProjectInfoPtr->falseColorPaletteBlue ] + 1;
	
	m_maxChannelFeatureNum = projectWindowInfoPtr->totalNumberChannels;
	
	if (TransferDataToWindow ())
		{
		PositionDialogWindow ();
		
		listBoxPtr = (wxListBox*)FindWindow (IDC_List1);
		
		}	// end "if (UpdateData (FALSE))"

	if (listBoxPtr != NULL)
		{
		AddChannelsToDialogList (listBoxPtr,
											NULL,
											NULL,
											m_layerInfoPtr,
											m_fileInfoPtr,
											kItemsListOnly,
											(SInt16*)gProjectInfoPtr->channelsPtr,
											gProjectInfoPtr->numberStatisticsChannels,
											kSubsetMenuItem);
		
		}	// end "if (listBoxPtr != NULL)"
	
	SelectDialogItemText (this, IDC_EDITRed, 0, SInt16_MAX);
	
	return;
	
}	// end "OnInitDialog"



bool CMFalseColorDlg::TransferDataFromWindow ()

{
   SInt16								badChannel;
	
	
	wxTextCtrl* red = (wxTextCtrl*)FindWindow (IDC_EDITRed);
   m_redChannel = wxAtoi (red->GetValue ());
	SInt16 itemHit = FalseColorCheckColorChannel (
												this, 4, m_redChannel, &m_redChannelIndex);
	
	if (itemHit == 1)
		{                      
      wxTextCtrl* green = (wxTextCtrl*)FindWindow (IDC_EDITGreen);
   	m_greenChannel =  wxAtoi (green->GetValue ());
		itemHit = FalseColorCheckColorChannel (
												this, 6, m_greenChannel, &m_greenChannelIndex);
		if (itemHit == 1)
			{          
         wxTextCtrl* blue = (wxTextCtrl*)FindWindow (IDC_EDITBlue);
			m_blueChannel = wxAtoi (blue->GetValue ());
			itemHit = FalseColorCheckColorChannel (
												this, 8, m_blueChannel, &m_blueChannelIndex);
				
			if (itemHit != 1)  
				badChannel = IDC_EDITBlue;
				
			}	// end "if (itemHit == 1)"
			
		else	// itemHit != 1
			badChannel = IDC_EDITGreen;
			
		}	// end "if (itemHit == 1)"
	
	else	// itemHit != 1
		badChannel = IDC_EDITRed;
   
   return true;
	
}	// end "TransferDataFromWindow"



bool CMFalseColorDlg::TransferDataToWindow ()

{
   wxTextCtrl* red = (wxTextCtrl*)FindWindow (IDC_EDITRed);
   red->ChangeValue (wxString::Format (wxT("%i"), (int) m_redChannel));
	
   wxTextCtrl* green = (wxTextCtrl*)FindWindow (IDC_EDITGreen);
   green->ChangeValue (wxString::Format (wxT("%i"), (int) m_greenChannel));
	
   wxTextCtrl* blue = (wxTextCtrl*)FindWindow (IDC_EDITBlue);
  	blue->ChangeValue (wxString::Format (wxT("%i"), (int) m_blueChannel));
   
   return true;
	
}	// end "TransferDataToWindow"
