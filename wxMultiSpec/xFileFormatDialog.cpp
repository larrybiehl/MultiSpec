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
//	File:						xFileFormatDialog.cpp : class implementation file
//	Class Definition:		xFileFormatDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			02/24/2020
//
//	Language:				C++
//
//	System:					Linux & MacOS Operating Systems
//
//	Brief description:	This file contains functions related to the
//								CMFieldsToThematicDialog class.
//
// Following is template for debugging
/*
	int numberChars = sprintf ((char*)gTextString3,
									 " xFileFormatDialog:: (): %s",
									 gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SImageWindow_class.h"
#include "xFileFormatDialog.h"
#include "xImageView.h"
#include "xMultiSpec.h"
#include "wx/valgen.h"
#include "wx/window.h"



BEGIN_EVENT_TABLE (CMFileFormatSpecsDlg, CMDialog)
	EVT_BUTTON (IDC_HDFDataSetHelp, CMFileFormatSpecsDlg::OnBnClickedHdfdatasethelp)

	EVT_CHECKBOX (IDC_FillDataValueExists, CMFileFormatSpecsDlg::OnBnClickedFillDataValueExists)

	EVT_CHOICE (IDC_BandInterleavePopUp, CMFileFormatSpecsDlg::OnSelendokBandInterleave)
	EVT_CHOICE (IDC_DataValueTypePopUp, CMFileFormatSpecsDlg::OnSelendokDataValueType)
	EVT_CHOICE (IDC_HDFDataSet, CMFileFormatSpecsDlg::OnSelendokHDFDataSet)

	EVT_INIT_DIALOG (CMFileFormatSpecsDlg::OnInitDialog)

	EVT_SHOW (CMFileFormatSpecsDlg::OnShow)
END_EVENT_TABLE ()



CMFileFormatSpecsDlg::CMFileFormatSpecsDlg (
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title /*=NULL*/)
		: CMDialog (CMFileFormatSpecsDlg::IDD, pParent, title)

{
	m_numberLines = 512;
	m_numberColumns = 614;
	m_postChannelBytes = 0;
	m_headerBytes = 128;
	m_postLineBytes = 0;
	m_preChannelBytes = 0;
	m_preLineBytes = 0;
	m_numberChannels = 0;
	m_startColumnNumber = 1;
	m_startLineNumber = 1;
	m_swapBytesFlag = FALSE;
	m_linesBottomToTopFlag = FALSE;
	m_trailerBytes = 0;
	m_imageName = "";
	m_computeNumClasses = FALSE;
	m_dataValueType = 0;
	m_bandInterleave = 0;
	m_hdfDataSetSelection = -1;
	m_collapseClassSelection = -1;

	m_blockHeight = 0;
	m_blockWidth = 0;

	m_fileInfoHandle = NULL;
	m_windowInfoHandle = NULL;
	m_newChannelToHdfDataSetHandle = NULL;

	m_fileInfoPtr = NULL;
	m_windowInfoPtr = NULL;
	m_menuClientData = NULL;

	m_dataCompressionCode = 0;

	m_maxNumberChannelsClasses = 0;

	m_dataSetIndex = 1;

	m_eightBitsPerDataSelection = 0;
	m_forceGroupTableUpdateFlag = FALSE;
	m_initializedFlag = CMDialog::m_initializedFlag;

		// m_onShowCalledFlag is used to account for differences on wxWidgets message
		// passing on mygeohub (debian) and mdw linux box (redhat). OnShow message is
		// called after OnInitDialog on debian but vice versa on redhat. The reason
		// may not be the operating system, but hopefully this accounts for differences
		// in where the code is run.

	m_onShowCalledFlag = FALSE;

	CreateControls ();
	
}	// end "CMFileFormatSpecsDlg"



CMFileFormatSpecsDlg::~CMFileFormatSpecsDlg ()

{
	if (m_menuClientData != NULL)
		free (m_menuClientData);
	
}	// end "~CMFileFormatSpecsDlg"



void CMFileFormatSpecsDlg::CreateControls ()

{
	this->SetSizeHints (wxDefaultSize, wxDefaultSize);

	bSizer47 = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizer48;
	bSizer48 = new wxBoxSizer (wxHORIZONTAL);

	wxStaticText* staticText59 = new wxStaticText (this,
																	wxID_ANY,
																	wxT("For: "),
																	wxDefaultPosition,
																	wxDefaultSize,
																	0);
	staticText59->Wrap (-1);
	bSizer48->Add (staticText59, 0);

	m_staticText60 = new wxStaticText (this,
													IDC_ImageName,
													wxT("Image File Name"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText60->Wrap (-1);
   SetUpToolTip (m_staticText60, IDS_ToolTip277);
	bSizer48->Add (m_staticText60, wxSizerFlags(0).Border(wxBOTTOM,5));

	bSizer47->Add (bSizer48, wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT,12));

	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer (new wxStaticBox (this,
																		wxID_ANY,
																		wxT("Size Specifications")),
												wxHORIZONTAL);

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer (5, 2, 0, 0);
	fgSizer1->SetFlexibleDirection (wxBOTH);
	fgSizer1->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);

	m_staticText89 = new wxStaticText (sbSizer3->GetStaticBox (),
													wxID_ANY,
													wxT("Number of Lines"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText89->Wrap (-1);
   SetUpToolTip (m_staticText89, IDS_ToolTip274);
	fgSizer1->Add (
				m_staticText89,
				wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl41 = new wxTextCtrl (sbSizer3->GetStaticBox (),
												IDC_NumberLines,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	SetUpToolTip (m_textCtrl41, IDS_ToolTip274);
	m_textCtrl41->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
	fgSizer1->Add (m_textCtrl41, 0, wxALL, 5);

	m_staticText90 = new wxStaticText (sbSizer3->GetStaticBox (),
													wxID_ANY,
													wxT("Number of Columns"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText90->Wrap (-1);
   SetUpToolTip (m_staticText90, IDS_ToolTip275);
	fgSizer1->Add (
				m_staticText90,
				wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl42 = new wxTextCtrl (sbSizer3->GetStaticBox (),
												IDC_NumberColumns,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_textCtrl42, IDS_ToolTip275);
	m_textCtrl42->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
	fgSizer1->Add (m_textCtrl42, 0, wxALL, 5);

	m_staticText91 = new wxStaticText (sbSizer3->GetStaticBox (),
													IDC_NumberChannelsClassesPrompt,
													wxT("Number of Channels"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText91->Wrap (-1);
   SetUpToolTip (m_staticText91, IDS_ToolTip276);
	fgSizer1->Add (
					m_staticText91,
					wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl43 = new wxTextCtrl (sbSizer3->GetStaticBox (),
												IDC_NumberChannels,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_textCtrl43, IDS_ToolTip276);
	m_textCtrl43->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
	fgSizer1->Add (m_textCtrl43, 0, wxALL, 5);

	m_staticText92 = new wxStaticText (sbSizer3->GetStaticBox (),
													wxID_ANY, wxT("Start Line Number"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText92->Wrap (-1);
   SetUpToolTip (m_staticText92, IDS_ToolTip278);
	fgSizer1->Add (
				m_staticText92,
				wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl44 = new wxTextCtrl (sbSizer3->GetStaticBox (),
												IDC_StartLineNumber,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	m_textCtrl44->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip (m_textCtrl44, IDS_ToolTip278);
	fgSizer1->Add (m_textCtrl44, 0, wxALL, 5);

	m_staticText93 = new wxStaticText (sbSizer3->GetStaticBox (),
													wxID_ANY,
													wxT("Start Column Number"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText93->Wrap (-1);
   SetUpToolTip (m_staticText93, IDS_ToolTip279);
	fgSizer1->Add (
				m_staticText93,
				wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl45 = new wxTextCtrl (sbSizer3->GetStaticBox (),
												IDC_StartColumnNumber,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	m_textCtrl45->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip (m_textCtrl45, IDS_ToolTip279);
	fgSizer1->Add (m_textCtrl45, 0, wxALL, 5);

   sbSizer3->Add (fgSizer1, 0, wxEXPAND, 5);
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer (8, 2, 0, 0);
	fgSizer2->SetFlexibleDirection (wxBOTH);
	fgSizer2->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);

	m_staticText94 = new wxStaticText (sbSizer3->GetStaticBox (),
													IDC_HeaderPrompt,
													wxT("File Header Bytes"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText94->Wrap (-1);
   SetUpToolTip (m_staticText94, IDS_ToolTip280);
	fgSizer2->Add (
				m_staticText94,
				wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl46 = new wxTextCtrl (sbSizer3->GetStaticBox (),
												IDC_HeaderBytes,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	m_textCtrl46->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip (m_textCtrl46, IDS_ToolTip280);
	fgSizer2->Add (m_textCtrl46, 0, wxALL, 5);

	m_staticText95 = new wxStaticText (sbSizer3->GetStaticBox (),
													IDC_TrailerPrompt,
													wxT("File Trailer Bytes"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText95->Wrap (-1);
   SetUpToolTip (m_staticText95, IDS_ToolTip281);
	fgSizer2->Add (
				m_staticText95,
				wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl47 = new wxTextCtrl (sbSizer3->GetStaticBox (),
												IDC_TrailerBytes,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	m_textCtrl47->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip (m_textCtrl47, IDS_ToolTip281);
	fgSizer2->Add (m_textCtrl47, 0, wxALL, 5);

	m_staticText100 = new wxStaticText (sbSizer3->GetStaticBox (),
													IDC_BlockHeightPrompt,
													wxT("Block Height (lines)"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText100->Wrap (-1);
	fgSizer2->Add (
				m_staticText100,
				wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_staticText101 = new wxStaticText (sbSizer3->GetStaticBox (),
													IDC_BlockHeight,
													wxT("MyLabel"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText101->Wrap (-1);
	fgSizer2->Add (m_staticText101, 0, wxALL, 5);

	m_staticText102 = new wxStaticText (sbSizer3->GetStaticBox (),
													IDC_BlockWidthPrompt,
													wxT("Block Width (cols.)"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText102->Wrap (-1);
	fgSizer2->Add (
				m_staticText102,
				wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_staticText103 = new wxStaticText (sbSizer3->GetStaticBox (),
													IDC_BlockWidth,
													wxT("MyLabel"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText103->Wrap (-1);
	fgSizer2->Add (m_staticText103, 0, wxALL, 5);

	m_staticText96 = new wxStaticText (sbSizer3->GetStaticBox (),
													IDC_PreLinePrompt,
													wxT("Preline Bytes"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText96->Wrap (-1);
   SetUpToolTip (m_staticText96, IDS_ToolTip282);
	fgSizer2->Add (
				m_staticText96,
				wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl48 = new wxTextCtrl (sbSizer3->GetStaticBox (),
												IDC_PreLineBytes,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	m_textCtrl48->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip (m_textCtrl48, IDS_ToolTip282);
	fgSizer2->Add (m_textCtrl48, 0, wxALL, 5);

	m_staticText97 = new wxStaticText (sbSizer3->GetStaticBox (),
													IDC_PostLinePrompt,
													wxT("Postline Bytes"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText97->Wrap (-1);
   SetUpToolTip (m_staticText97, IDS_ToolTip283);
	fgSizer2->Add (
				m_staticText97,
				wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl49 = new wxTextCtrl (sbSizer3->GetStaticBox (),
												IDC_PostLineBytes,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	m_textCtrl49->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip (m_textCtrl49, IDS_ToolTip283);
	fgSizer2->Add (m_textCtrl49, 0, wxALL, 5);

	m_staticText98 = new wxStaticText (sbSizer3->GetStaticBox (),
													IDC_PreChannelPrompt,
													wxT("Prechannel Bytes"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText98->Wrap (-1);
   SetUpToolTip (m_staticText98, IDS_ToolTip284);
	fgSizer2->Add (
				m_staticText98,
				wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl50 = new wxTextCtrl (sbSizer3->GetStaticBox (),
												IDC_PreChannelBytes,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	m_textCtrl50->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip (m_textCtrl50, IDS_ToolTip284);
	fgSizer2->Add (m_textCtrl50, 0, wxALL, 5);

	m_staticText99 = new wxStaticText (sbSizer3->GetStaticBox (),
													IDC_PostChannelPrompt,
													wxT("Postchannel Bytes"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText99->Wrap (-1);
   SetUpToolTip (m_staticText99, IDS_ToolTip285);
	fgSizer2->Add (
				m_staticText99,
				wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl51 = new wxTextCtrl (sbSizer3->GetStaticBox (),
												IDC_PostChannelBytes,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	m_textCtrl51->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip (m_textCtrl51, IDS_ToolTip285);
	fgSizer2->Add (m_textCtrl51, 0, wxALL, 5);

			// Set TAB Order
	
	m_textCtrl42->MoveAfterInTabOrder (m_textCtrl41);
	m_textCtrl43->MoveAfterInTabOrder (m_textCtrl42);
	m_textCtrl44->MoveAfterInTabOrder (m_textCtrl43);
	m_textCtrl45->MoveAfterInTabOrder (m_textCtrl44);
	m_textCtrl46->MoveAfterInTabOrder (m_textCtrl45);
	m_textCtrl47->MoveAfterInTabOrder (m_textCtrl46);
	m_textCtrl48->MoveAfterInTabOrder (m_textCtrl47);
	m_textCtrl49->MoveAfterInTabOrder (m_textCtrl48);
	m_textCtrl50->MoveAfterInTabOrder (m_textCtrl49);
	m_textCtrl51->MoveAfterInTabOrder (m_textCtrl50);

   sbSizer3->Add (fgSizer2, 0, wxEXPAND, 5);

	bSizer47->Add (sbSizer3, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT,12));

	wxBoxSizer* bSizer79;
	bSizer79 = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizer80;
	bSizer80 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText80 = new wxStaticText (this,
													wxID_ANY,
													wxT("Band Interleave Format:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText80->Wrap (-1);
   SetUpToolTip (m_staticText80, IDS_ToolTip286);
	bSizer80->Add (m_staticText80, wxSizerFlags(0).Center().Border(wxALL,5));

	m_interleaveCtrl = new wxChoice (this,
												IDC_BandInterleavePopUp,
												wxDefaultPosition,
												wxSize (230, -1));
	m_interleaveCtrl->Append (wxT("BIL-Band Interleaved by Line"));
	m_interleaveCtrl->Append (wxT("BSQ-Band Sequential"));
	m_interleaveCtrl->Append (wxT("BIS-Band Interleaved by Sample"));
	m_interleaveCtrl->Append (wxT("Band Non-sequential"));
	m_interleaveCtrl->Append (wxT("Band Non-sequential Blocked"));
	m_interleaveCtrl->Append (wxT("Band Interleave by Block"));
   SetUpToolTip (m_interleaveCtrl, IDS_ToolTip286);
	bSizer80->Add (m_interleaveCtrl, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,5));

	bSizer79->Add (bSizer80, wxSizerFlags(0).Expand().Border(wxTOP,5));

	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText81 = new wxStaticText (this,
													wxID_ANY,
													wxT("Data Value Type:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText81->Wrap (-1);
   SetUpToolTip (m_staticText81, IDS_ToolTip287);
	bSizer81->Add (m_staticText81, wxSizerFlags(0).Center().Border(wxALL,5));

	m_valueTypeCtrl = new wxChoice (this,
												IDC_DataValueTypePopUp,
												wxDefaultPosition,
												wxSize (175, -1));
	m_valueTypeCtrl->Append (wxT("4-bit Unsigned Integer"));
	m_valueTypeCtrl->Append (wxT("8-bit Signed Integer"));
	m_valueTypeCtrl->Append (wxT("8-bit Unsigned Integer"));
	m_valueTypeCtrl->Append (wxT("16-bit Signed Integer"));
	m_valueTypeCtrl->Append (wxT("16-bit Unsigned Integer"));
	m_valueTypeCtrl->Append (wxT("32-bit Signed Integer"));
	m_valueTypeCtrl->Append (wxT("32-bit Unsigned Integer"));
	m_valueTypeCtrl->Append (wxT("32-bit Real"));
	m_valueTypeCtrl->Append (wxT("64-bit Real"));
   SetUpToolTip (m_valueTypeCtrl, IDS_ToolTip287);
	bSizer81->Add (m_valueTypeCtrl, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,5));

	m_checkBox2 = new wxCheckBox (this,
											IDC_SwapBytes,
											wxT("Swap Bytes"),
											wxDefaultPosition,
											wxDefaultSize,
											0);
	m_checkBox2->SetValue (true);
   SetUpToolTip (m_checkBox2, IDS_ToolTip288);
	bSizer81->Add (m_checkBox2, wxSizerFlags(0).Center().Border(wxALL,5));

	bSizer79->Add (bSizer81, wxSizerFlags(0).Expand());

	wxBoxSizer* bSizer82;
	bSizer82 = new wxBoxSizer (wxHORIZONTAL);

	m_checkBox3 = new wxCheckBox (this,
											IDC_LinesBottomToTop,
											wxT("Treat Lines from Bottom to Top"),
											wxDefaultPosition,
											wxDefaultSize,
											0);
	m_checkBox3->SetValue (true);
   SetUpToolTip (m_checkBox3, IDS_ToolTip70);
	bSizer82->Add (m_checkBox3, 0, wxALL, 5);

	bSizer79->Add (bSizer82, wxSizerFlags(0).Expand());

	wxBoxSizer* bSizer83;
	bSizer83 = new wxBoxSizer (wxHORIZONTAL);

	m_checkBox4 = new wxCheckBox (this,
											IDC_FillDataValueExists,
											wxT("Data Fill Value:"),
											wxDefaultPosition,
											wxDefaultSize,
											0);
   SetUpToolTip (m_checkBox4, IDS_ToolTip273);
	bSizer83->Add (m_checkBox4, 0, wxALIGN_CENTER | wxALL, 5);

	m_textCtrl35 = new wxTextCtrl (this,
												IDC_FillDataValue,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (200, -1),
												0);
   SetUpToolTip (m_textCtrl35, IDS_ToolTip273);
	bSizer83->Add (m_textCtrl35,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));
	
	bSizer79->Add (bSizer83, wxSizerFlags(0).Expand());

	wxBoxSizer* bSizer84;
	bSizer84 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText82 = new wxStaticText (this,
													IDC_CollapseClassPrompt,
													wxT("Collapse Classes:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText82->Wrap (-1);
	bSizer84->Add (m_staticText82, 0, wxALIGN_CENTER | wxALL, 5);

	m_collapseCtrl = new wxChoice (this,
												IDC_CollapseClass,
												wxDefaultPosition,
												wxDefaultSize);
	m_collapseCtrl->Append (wxT("Yes"));
	m_collapseCtrl->Append (wxT("No"));
	m_collapseCtrl->Append (wxT("To Highest Value"));
	bSizer84->Add (m_collapseCtrl, 0, wxALL, 5);

	bSizer79->Add (bSizer84, wxSizerFlags(0).Expand());

	wxBoxSizer* bSizer85;
	bSizer85 = new wxBoxSizer (wxHORIZONTAL);

	bSizer85->Add (50, 0, 0, wxEXPAND, 5);

	m_checkBox5 = new wxCheckBox (this,
											IDC_ComputeNumClasses,
											wxT("Compute Number of Classes"),
											wxDefaultPosition,
											wxDefaultSize,
											0);
	bSizer85->Add (m_checkBox5, 0, wxALL, 5);

	bSizer79->Add (bSizer85, wxSizerFlags(0).Expand());

	wxBoxSizer* bSizer86;
	bSizer86 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText83 = new wxStaticText (this,
													IDC_HDFDataSetPrompt,
													wxT("HDF Data Set:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText83->Wrap (-1);
   SetUpToolTip (m_staticText83, IDS_ToolTip289);
	bSizer86->Add (m_staticText83, 0, wxALIGN_CENTER | wxALL, 5);

	m_hdfDataSetCtrl = new wxChoice (this,
												IDC_HDFDataSet,
												wxDefaultPosition,
												wxSize (350, -1));
   SetUpToolTip (m_hdfDataSetCtrl, IDS_ToolTip289);
	bSizer86->Add (m_hdfDataSetCtrl, 0, wxALL, 5);

	m_button7 = new wxButton (this,
										IDC_HDFDataSetHelp,
										wxT("?"),
										wxDefaultPosition,
										wxSize (25, 25),
										0);
	bSizer86->Add (m_button7, 0, wxALL, 5);

	bSizer79->Add (bSizer86, wxSizerFlags(0).Expand());

	bSizer47->Add (bSizer79, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	CreateStandardButtons (bSizer47);

	SetSizer (bSizer47);
	Layout ();
	
}	// end "CreateControls"



Boolean CMFileFormatSpecsDlg::DoDialog (
				Handle 								fileInfoHandle,
				Handle 								windowInfoHandle,
				Handle* 								mapInformationHandlePtr,
				Handle* 								hfaHandlePtr,
				Handle* 								newChannelToHdfDataSetHandlePtr,
				Boolean* 							parameterChangedFlagPtr)

{
	SInt16 								returnCode;

	Boolean 								continueFlag = FALSE,
											changedFlag,
											parameterChangedFlag = FALSE;


			// Make sure intialization has been completed.

	if (!m_initializedFlag || fileInfoHandle == NULL)
																						return (FALSE);

	m_fileInfoHandle = fileInfoHandle;
	m_windowInfoHandle = windowInfoHandle;

	m_mapInformationHandle = *mapInformationHandlePtr;
	m_hfaHandle = *hfaHandlePtr;
	m_newChannelToHdfDataSetHandle = *newChannelToHdfDataSetHandlePtr;

	returnCode = ShowModal ();

	if (returnCode == wxID_OK)
    	{
		changedFlag = FALSE;
		continueFlag = TRUE;

		parameterChangedFlag = FileSpecificationDialogOK (
																	this,
																	 m_fileInfoPtr,
																	 fileInfoHandle,
																	 &m_mapInformationHandle,
																	 &m_hfaHandle,
																	 &m_newChannelToHdfDataSetHandle,
																	 m_windowInfoPtr,
																	 windowInfoHandle,
																	 m_numberLines,
																	 m_numberColumns,
																	 m_numberChannels,
																	 m_startLineNumber,
																	 m_startColumnNumber,
																	 m_headerBytes,
																	 m_trailerBytes,
																	 m_preLineBytes,
																	 m_postLineBytes,
																	 m_preChannelBytes,
																	 m_postChannelBytes,
																	 m_blockHeight,
																	 m_blockWidth,
																	 m_forceGroupTableUpdateFlag,
																	 m_bandInterleave + 1,
																	 m_dataValueType,
																	 m_swapBytesFlag,
																	 FALSE,
																	 m_linesBottomToTopFlag,
																	 m_computeNumClasses,
																	 m_fillDataValueExistsFlag,
																	 m_fillDataValue,
																	 m_hdfDataSetSelection + 1,
																	 m_dataSetIndex,
																	 m_collapseClassSelection + 1,
																	 m_dataCompressionCode,
																	 m_gdalDataTypeCode,
																	 m_callGetHDFLineFlag);

		}	// end "if (returnCode == IDOK)"

	if (parameterChangedFlagPtr != NULL)
		*parameterChangedFlagPtr = parameterChangedFlag;

	*mapInformationHandlePtr = m_mapInformationHandle;
	*hfaHandlePtr = m_hfaHandle;
	*newChannelToHdfDataSetHandlePtr = m_newChannelToHdfDataSetHandle;

	return (continueFlag);

}	// end "DoDialog"



void CMFileFormatSpecsDlg::OnBnClickedHdfdatasethelp (
				wxCommandEvent& 					event)

{
	DisplayAlert (kErrorAlertID,
						kNoteAlert,
						kAlertStrID,
						IDS_Alert103,
						IDS_Alert104,
						NULL,
						this,
						kASCIICharString);
	
}	// end "OnBnClickedHdfdatasethelp"



void CMFileFormatSpecsDlg::OnBnClickedFillDataValueExists (
				wxCommandEvent& 					event)

{
    wxCheckBox* fillDataValueExistsFlag =
    										(wxCheckBox*)FindWindow (IDC_FillDataValueExists);
    m_fillDataValueExistsFlag = fillDataValueExistsFlag->GetValue ();

    ShowHideDialogItem (this, IDC_FillDataValue, m_fillDataValueExistsFlag);
    Fit ();
	 
}	// end "OnBnClickedFillDataValueExists"



void CMFileFormatSpecsDlg::OnDetermineNumClasses (
				wxCommandEvent&					event)

{
			// Original argument set to void
		
	if (GetClassesFromHistogram (m_fileInfoPtr, m_collapseClassSelection + 1))
		{
		m_numberChannels = m_fileInfoPtr->numberClasses;
		wxTextCtrl* numberChannels = (wxTextCtrl*)FindWindow (IDC_NumberChannels);
		numberChannels->ChangeValue (wxString::Format (wxT("%i"), m_numberChannels));
		m_forceGroupTableUpdateFlag = TRUE;

		}	// end "if (GetClassesFromHistogram (gFileInfoPtr))"

	if (m_fileInfoPtr->classesComputedFlag)
		FindWindow (IDC_DetermineNumClasses)->Enable (FALSE);

}	// end "OnDetermineNumClasses"



void CMFileFormatSpecsDlg::OnInitDialog (
				wxInitDialogEvent&				event)

{
	FileStringPtr						fileNamePtr;

	SInt16								bandInterleave,
											collapseClassesSelection,
											dataSetSelection,
											dataValueType;

	Boolean								signedDataFlag;


	//wxDialog::OnInitDialog (event);

			// Initialize dialog variables.

	FileSpecificationDialogInitialize (this,
													m_fileInfoHandle,
													m_windowInfoHandle,
													&m_fileInfoPtr,
													&m_windowInfoPtr,
													&m_numberLines,
													&m_numberColumns,
													&m_numberChannels,
													&m_startLineNumber,
													&m_startColumnNumber,
													&m_headerBytes,
													&m_trailerBytes,
													&m_preLineBytes,
													&m_postLineBytes,
													&m_preChannelBytes,
													&m_postChannelBytes,
													&m_blockHeight,
													&m_blockWidth,
													&bandInterleave,
													&dataValueType,
													&m_eightBitsPerDataSelection,
													&m_swapBytesFlag,
													&signedDataFlag,
													&m_linesBottomToTopFlag,
													&m_fillDataValueExistsFlag,
													&m_fillDataValue,
													&dataSetSelection,
													&collapseClassesSelection,
													&m_callGetHDFLineFlag);
					
	m_bandInterleave = bandInterleave - 1;
	m_dataValueType = dataValueType;
	m_hdfDataSetSelection = dataSetSelection - 1;
	m_collapseClassSelection = collapseClassesSelection - 1;
   
	m_maxNumberChannelsClasses = kMaxNumberChannels;
	if (m_fileInfoPtr->thematicType)
		m_maxNumberChannelsClasses = kMaxNumberClasses;

			//	Set the image file name.										 

	fileNamePtr = (FileStringPtr)GetFileNameCPointerFromFileInfo (m_fileInfoPtr);
	if (fileNamePtr != NULL)
		m_imageName = CString (fileNamePtr);
	
	if (m_fileInfoPtr->thematicType)
		{
		if (m_fileInfoPtr->numberClasses >= kMaxNumberClasses &&
         													!m_fileInfoPtr->classesComputedFlag)
			m_computeNumClasses = TRUE;
		
		}	// end "if (m_fileInfoPtr->thematicType)"

	if (TransferDataToWindow ())
		PositionDialogWindow ();
		
	if (m_fileInfoPtr->thematicType) 
		{
		wxStaticText* channelsClassesLabel =
								(wxStaticText*)FindWindow (IDC_NumberChannelsClassesPrompt);
		channelsClassesLabel->SetLabel (wxT("Number of Classes"));
		
		}	// end "if (m_fileInfoPtr->thematicType)"
		
	SetSizerAndFit (bSizer47);
	Layout ();
	Fit ();
		
	SelectDialogItemText (this, IDC_NumberLines, 0, (SInt16)INT_MAX);
	
	if (m_onShowCalledFlag)
		ShowMultipleDataSetMessage ();

}	// end "OnInitDialog" 



void CMFileFormatSpecsDlg::OnSelendokBandInterleave (
				wxCommandEvent& 					event)

{
			// prevent control notifications from being dispatched during 
			// UpdateData

    m_bandInterleave = m_interleaveCtrl->GetSelection ();

    FileSpecificationDialogSetInterleaveItems (this,
																m_bandInterleave + 1,
																m_fileInfoPtr->blockedFlag);

}	// end "OnSelendokBandInterleave"



void CMFileFormatSpecsDlg::OnSelendokDataValueType (
				wxCommandEvent& 					event)

{
	m_dataValueType = m_valueTypeCtrl->GetSelection ();
	m_dataValueType = m_dataValueType +1;
	if (m_dataValueType >= k2ByteSignedIntegerMenuItem) 
		FindWindow (IDC_SwapBytes)->Enable (TRUE);

	else // m_dataValueType < k2ByteSignedIntegerMenuItem
		FindWindow (IDC_SwapBytes)->Enable (FALSE);

}	// end "OnSelendokDataValueType"



void CMFileFormatSpecsDlg::OnSelendokHDFDataSet (
				wxCommandEvent& 					event)

{
	int 									newDataSetSelection;

	SInt16 								bandInterleaveSelection,
											dataValueTypeSelection;

	Boolean 								shiftKeyDownFlag;
	
	
	newDataSetSelection = m_hdfDataSetCtrl->GetSelection ();

	shiftKeyDownFlag = wxGetKeyState (WXK_SHIFT);

   if (newDataSetSelection != m_hdfDataSetSelection || shiftKeyDownFlag) 
    	{
		if (FileSpecificationDialogSetHDFValues (this,
																 m_fileInfoPtr,
																 newDataSetSelection,
																 NULL,
																 shiftKeyDownFlag,
																 &m_mapInformationHandle,
																 &m_hfaHandle,
																 &m_newChannelToHdfDataSetHandle,
																 &bandInterleaveSelection,
																 &dataValueTypeSelection,
																 &m_dataCompressionCode,
																 &m_gdalDataTypeCode,
																 &m_callGetHDFLineFlag) == noErr) 
			{
			m_hdfDataSetSelection = newDataSetSelection;

			m_bandInterleave = bandInterleaveSelection - 1;
			m_interleaveCtrl->SetSelection (m_bandInterleave);

			m_dataValueType = dataValueTypeSelection;
			m_valueTypeCtrl->SetSelection (m_dataValueType);
			
			}	// end "if (FileSpecificationDialogSetHDFValues (..."

		else	// FileSpecificationDialogSetHDFValues != noErr
			m_hdfDataSetCtrl->SetSelection (m_hdfDataSetSelection);

   	}	// end "if (savedDataSetSelection != m_hdfDataSetSelection)"

}	// end "OnSelendokHDFDataSet"



void CMFileFormatSpecsDlg::OnShow (
				wxShowEvent& 						event)

{
	if (event.IsShown ())
		{
		m_onShowCalledFlag = TRUE;
	
		if (m_windowInfoPtr != NULL)
			ShowMultipleDataSetMessage ();
		
		}	// end "if (event.IsShown ())"
		
}	// end "OnShow"



void CMFileFormatSpecsDlg::ShowMultipleDataSetMessage (void)

{
	SInt16								bandInterleaveSelection,
											dataValueTypeSelection;


		  // Do not allow specifications to be changed if the active				
		  // window represents the base image for an open project.					

	if (gProcessorCode == kChangeImageDescriptionProcessor &&
																m_windowInfoPtr->projectWindowFlag) 
		{
		DisplayAlert (kErrorAlertID, kCautionAlert, kAlertStrID, IDS_Alert25, 0, NULL);

		}	// end "if (gProcessorCode == kChangeImageDescriptionProcessor && ..."
	  
		  // Display message about selecting a data set for the first pass through
		  // this section.
	
	if ((gProcessorCode == kOpenImageFileProcessor || 
			gProcessorCode == kRefChangeFileFormatProcessor) &&
				(m_fileInfoPtr->format == kHDF4Type ||
					m_fileInfoPtr->format == kNETCDFType ||
						m_fileInfoPtr->format == kHDF5Type ||
							m_fileInfoPtr->format == kNETCDF2Type ||
								m_fileInfoPtr->format == kHDF4Type2 ||
									m_fileInfoPtr->format == kNITFType) &&
												m_fileInfoPtr->numberHdfDataSets > 1 &&
													!gHDFDataSetSelectionAlertDisplayedFlag)
		{
				// Need to be sure dialog box is shown
		
		CheckSomeEvents (updateMask+activMask);
		
		DisplayAlert (kErrorAlertID,
							kNoteAlert,
							kAlertStrID,
							IDS_Alert103,
							IDS_Alert104,
							NULL,
							this,
							kASCIICharString);
		
		gHDFDataSetSelectionAlertDisplayedFlag = TRUE;

		}	// end "if (gProcessorCode == kOpenImageFileProcessor && fileInfoPtr->...)"
	
			// Force any alert messages concerning the default hdf data set to
			// be shown.
	
	if (gProcessorCode == kOpenImageFileProcessor &&
				(m_fileInfoPtr->format == kHDF4Type ||
					m_fileInfoPtr->format == kNETCDFType ||
						m_fileInfoPtr->format == kHDF5Type ||
							m_fileInfoPtr->format == kNETCDF2Type ||
								m_fileInfoPtr->format == kHDF4Type2 ||
									m_fileInfoPtr->format == kNITFType))
		{
		FileSpecificationDialogSetHDFValues (this,
														  m_fileInfoPtr,
														  m_hdfDataSetSelection,
														  NULL,
														  FALSE,
														  &m_mapInformationHandle,
														  &m_hfaHandle,
														  &m_newChannelToHdfDataSetHandle,
														  &bandInterleaveSelection,
														  &dataValueTypeSelection,
														  &m_dataCompressionCode,
														  &m_gdalDataTypeCode,
														  &m_callGetHDFLineFlag);
		
		m_bandInterleave = bandInterleaveSelection - 1;
		m_interleaveCtrl->SetSelection (m_bandInterleave);

		m_dataValueType = dataValueTypeSelection;
		m_valueTypeCtrl->SetSelection (m_dataValueType);
		
		}	// end "if (gProcessorCode == kOpenImageFileProcessor && ..."
	 
}	// end "ShowMultipleDataSetMessage"


bool CMFileFormatSpecsDlg::TransferDataFromWindow ()

{
	double 								sizeDifference;
	UInt32 								theNum;
	
	SInt16 								numberBytes,
											returnCode = 0;
	
	
	wxTextCtrl* numberLines = (wxTextCtrl*)FindWindow (IDC_NumberLines);
	m_numberLines = wxAtoi (numberLines->GetValue ());
	
	wxTextCtrl* numberColumns = (wxTextCtrl*)FindWindow (IDC_NumberColumns);
	m_numberColumns = wxAtoi (numberColumns->GetValue ());
	
	wxTextCtrl* postChannelBytes = (wxTextCtrl*)FindWindow (IDC_PostChannelBytes);
	m_postChannelBytes = wxAtoi (postChannelBytes->GetValue ());
	
	wxTextCtrl* headerBytes = (wxTextCtrl*)FindWindow (IDC_HeaderBytes);
	m_headerBytes = wxAtoi (headerBytes->GetValue ());
	
	wxTextCtrl* postLineBytes = (wxTextCtrl*)FindWindow (IDC_PostLineBytes);
	m_postLineBytes = wxAtoi (postLineBytes->GetValue ());
	
	wxTextCtrl* preChannelBytes = (wxTextCtrl*)FindWindow (IDC_PreChannelBytes);
	m_preChannelBytes = wxAtoi (preChannelBytes->GetValue ());
	
	wxTextCtrl* preLineBytes = (wxTextCtrl*)FindWindow (IDC_PreLineBytes);
	m_preLineBytes = wxAtoi (preLineBytes->GetValue ());
	
	wxTextCtrl* numberChannels = (wxTextCtrl*)FindWindow (IDC_NumberChannels);
	m_numberChannels = wxAtoi (numberChannels->GetValue ());
	
	wxTextCtrl* startColumnNumber = (wxTextCtrl*)FindWindow (IDC_StartColumnNumber);
	m_startColumnNumber = wxAtoi (startColumnNumber->GetValue ());
	
	wxTextCtrl* startLineNumber = (wxTextCtrl*)FindWindow (IDC_StartLineNumber);
	m_startLineNumber = wxAtoi (startLineNumber->GetValue ());
	
	wxTextCtrl* trailerBytes = (wxTextCtrl*)FindWindow (IDC_TrailerBytes);
	m_trailerBytes = wxAtoi (trailerBytes->GetValue ());

	wxTextCtrl* fillDataValue = (wxTextCtrl*)FindWindow (IDC_FillDataValue);
	wxString fillDataValue_string = fillDataValue->GetValue ();
	fillDataValue_string.ToDouble (&m_fillDataValue);

	m_dataValueType = m_valueTypeCtrl->GetSelection ();
	
	m_bandInterleave = m_interleaveCtrl->GetSelection ();
	
	m_hdfDataSetSelection = m_hdfDataSetCtrl->GetSelection ();
	
	m_collapseClassSelection = m_collapseCtrl->GetSelection ();

	wxCheckBox* swapBytesFlag = (wxCheckBox*)FindWindow (IDC_SwapBytes);
	m_swapBytesFlag = swapBytesFlag->GetValue ();
	
	wxCheckBox* computeNumClasses = (wxCheckBox*)FindWindow (IDC_ComputeNumClasses);
	m_computeNumClasses = computeNumClasses->GetValue ();
	
	wxCheckBox* linesBottomToTopFlag = (wxCheckBox*)FindWindow (IDC_LinesBottomToTop);
	m_linesBottomToTopFlag = linesBottomToTopFlag->GetValue ();
	
	wxCheckBox* fillDataValueExistsFlag =
												(wxCheckBox*)FindWindow (IDC_FillDataValueExists);
	m_fillDataValueExistsFlag = fillDataValueExistsFlag->GetValue ();
	
			// Verify the settings

	if (CheckMaxValue (this,
								IDC_NumberLines,
								1,
								kMaxNumberLines,
								kDisplayRangeAlert) != 1)
		returnCode = IDC_NumberLines;
		
	if (returnCode == 0)
		{
		if (CheckMaxValue (this,
									IDC_NumberColumns,
									1,
									kMaxNumberColumns,
									kDisplayRangeAlert) != 1)
			returnCode = IDC_NumberColumns;
		
		}	// end "if (returnCode == 0)"
		
	if (returnCode == 0)
		{
		if (CheckMaxValue (this,
									IDC_NumberChannels,
									1,
									m_maxNumberChannelsClasses,
									kDisplayRangeAlert) != 1)
			returnCode = IDC_NumberChannels;
		
		}	// end "if (returnCode == 0)"
		
	if (returnCode == 0)
		{
		if (CheckMaxValue (this,
									IDC_StartLineNumber,
									1,
									kMaxNumberLines,
									kDisplayRangeAlert) != 1)
			returnCode = IDC_StartLineNumber;
		
		}	// end "if (returnCode == 0)"
		
	if (returnCode == 0)
		{
		if (CheckMaxValue (this,
									IDC_StartColumnNumber,
									1,
									kMaxNumberColumns,
									kDisplayRangeAlert) != 1)
			returnCode = IDC_StartColumnNumber;
		
		}	// end "if (returnCode == 0)"

	if (m_fileInfoPtr->thematicType &&
			(m_dataValueType < k4BitUnsignedIntegerMenuItem ||
						m_dataValueType > k2ByteUnsignedIntegerMenuItem))
		{
		DisplayAlert (kErrorAlertID,
							 kStopAlert,
							 kAlertStrID,
							 IDS_Alert135,
							 0,
							 NULL);
				 
		returnCode = 1;

		}	// end "if (m_fileInfoPtr->thematicType &&"

	sizeDifference = 0;
	if ((m_fileInfoPtr->format != kHDF4Type &&
				m_fileInfoPtr->format != kNETCDFType &&
					m_fileInfoPtr->format != kHDF5Type &&
						m_fileInfoPtr->format != kNETCDF2Type &&
							m_fileInfoPtr->format != kHDF4Type2 &&
								m_fileInfoPtr->format != kNITFType) ||
											m_fileInfoPtr->numberHdfDataSets <= 1)
		{
		theNum = 1;
		if (!m_fileInfoPtr->thematicType)
			theNum = m_numberChannels;

	  	numberBytes = FileSpecificationDialogGetNumberBytes (m_dataValueType);

	  	sizeDifference = CompareFileInfoAndFileSize (m_fileInfoPtr,
																	 m_numberLines,
																	 m_numberColumns,
																	 theNum,
																	 numberBytes,
																	 m_preLineBytes,
																	 m_postLineBytes,
																	 m_preChannelBytes,
																	 m_postChannelBytes,
																	 m_headerBytes,
																	 m_trailerBytes,
																	 m_bandInterleave + 1,
																	 (m_dataValueType == 0));

		}	// end "if (fileInfoPtr->format != kHDFType || ..."

	if (sizeDifference > 0 && SizeOfImageFileCanBeCalculated (m_fileInfoPtr))
		{
				// Display an alert.

		FileStringPtr							fileNamePtr;
		SInt16									alertReturnCode;
		
		fileNamePtr = (FileStringPtr)GetFileNameCPointerFromFileInfo (m_fileInfoPtr);
		
		MGetString (gTextString2, kAlertStrID, IDS_FileSizeLimit);
		
		sprintf ((char*)gTextString, (char*)gTextString2, fileNamePtr, sizeDifference);
		
		alertReturnCode = DisplayAlert (kRedoDontCancelAlertID,
													 kCautionAlert,
													 kAlertStrID,
													 0,
													 0,
													 gTextString);

		if (alertReturnCode == 1)
			{
			FindWindow (IDC_NumberLines)->SetFocus ();
			returnCode = IDC_NumberLines;
			
			}	// end "if (alertReturnCode == 1)"

		}	// end "if (sizeDifference > 0 && ..."

	else // sizeDifference <= 0
		{
		if (m_fileInfoPtr->hdfHandle != NULL)
			{
					// Get the hdf data set selection in case it is needed.

			m_hdfDataSetCtrl->SetSelection (m_hdfDataSetSelection);
			m_dataSetIndex = m_hdfDataSetCtrl->GetSelection ();

			}	// end "if (m_fileInfoPtr->hdfHandle != NULL)"

		}	// end "else sizeDifference <= 0"
		
	return (returnCode == 0);
	 
}	// end "TransferDataFromWindow"



bool CMFileFormatSpecsDlg::TransferDataToWindow ()

{
	wxStaticText* blockHeight = (wxStaticText*)FindWindow (IDC_BlockHeight);
	blockHeight->SetLabel (wxString::Format (wxT("%i"), m_blockHeight));

	wxStaticText* blockWidth = (wxStaticText*)FindWindow (IDC_BlockWidth);
	blockWidth->SetLabel (wxString::Format (wxT("%i"), m_blockWidth));

	wxStaticText* imageName = (wxStaticText*)FindWindow (IDC_ImageName);
	imageName->SetLabel (m_imageName);

	wxTextCtrl* numberLines = (wxTextCtrl*)FindWindow (IDC_NumberLines);
	numberLines->ChangeValue (wxString::Format (wxT("%i"), m_numberLines));

	wxTextCtrl* numberColumns = (wxTextCtrl*)FindWindow (IDC_NumberColumns);
	numberColumns->ChangeValue (wxString::Format (wxT("%i"), m_numberColumns));

	wxTextCtrl* postChannelBytes = (wxTextCtrl*)FindWindow (IDC_PostChannelBytes);
	postChannelBytes->ChangeValue (wxString::Format (wxT("%i"), m_postChannelBytes));

	wxTextCtrl* headerBytes = (wxTextCtrl*)FindWindow (IDC_HeaderBytes);
	headerBytes->ChangeValue (wxString::Format (wxT("%i"), m_headerBytes));

	wxTextCtrl* postLineBytes = (wxTextCtrl*)FindWindow (IDC_PostLineBytes);
	postLineBytes->ChangeValue (wxString::Format (wxT("%i"), m_postLineBytes));

	wxTextCtrl* preChannelBytes = (wxTextCtrl*)FindWindow (IDC_PreChannelBytes);
	preChannelBytes->ChangeValue (wxString::Format (wxT("%i"), m_preChannelBytes));

	wxTextCtrl* preLineBytes = (wxTextCtrl*)FindWindow (IDC_PreLineBytes);
	preLineBytes->ChangeValue (wxString::Format (wxT("%i"), m_preLineBytes));

	wxTextCtrl* numberChannels = (wxTextCtrl*)FindWindow (IDC_NumberChannels);
	numberChannels->ChangeValue (wxString::Format (wxT("%i"), m_numberChannels));

	wxTextCtrl* startColumnNumber = (wxTextCtrl*)FindWindow (IDC_StartColumnNumber);
	startColumnNumber->ChangeValue (wxString::Format (wxT("%i"), m_startColumnNumber));

	wxTextCtrl* startLineNumber = (wxTextCtrl*)FindWindow (IDC_StartLineNumber);
	startLineNumber->ChangeValue (wxString::Format (wxT("%i"), m_startLineNumber));

	wxTextCtrl* trailerBytes = (wxTextCtrl*)FindWindow (IDC_TrailerBytes);
	trailerBytes->ChangeValue (wxString::Format (wxT("%i"), m_trailerBytes));

	wxCheckBox* swapBytesFlag = (wxCheckBox*)FindWindow (IDC_SwapBytes);
	swapBytesFlag->SetValue (m_swapBytesFlag);
	
	wxCheckBox* computeNumClasses = (wxCheckBox*)FindWindow (IDC_ComputeNumClasses);
	computeNumClasses->SetValue (m_computeNumClasses);
	
	wxCheckBox* linesBottomToTopFlag = (wxCheckBox*)FindWindow (IDC_LinesBottomToTop);
	linesBottomToTopFlag->SetValue (m_linesBottomToTopFlag);
	
	wxCheckBox* fillDataValueExistsFlag =
											(wxCheckBox*)FindWindow (IDC_FillDataValueExists);
	fillDataValueExistsFlag->SetValue (m_fillDataValueExistsFlag);

	wxTextCtrl* fillDataValue = (wxTextCtrl*)FindWindow (IDC_FillDataValue);
	fillDataValue->ChangeValue (wxString::Format (wxT("%.2lf"), m_fillDataValue));

	m_valueTypeCtrl->SetSelection (m_dataValueType);
	
	m_interleaveCtrl->SetSelection (m_bandInterleave);
	
	m_hdfDataSetCtrl->SetSelection (m_hdfDataSetSelection);
	
	m_collapseCtrl->SetSelection (m_collapseClassSelection);

	return true;
	 
}	// end "TransferDataToWindow"
