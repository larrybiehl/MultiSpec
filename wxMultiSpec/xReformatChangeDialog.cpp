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
//	File:						xReformatChangeDialog.cpp : class implementation file
//	Class Definition:		xReformatChangeDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			05/12/2020
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMChangeFormatDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SImageWindow_class.h"

#include "xImageView.h"
#include "xReformatChangeDialog.h"
#include "xReformatTransformDialog.h"

#include "wx/gbsizer.h"



BEGIN_EVENT_TABLE (CMChangeFormatDlg, CMDialog)
	EVT_BUTTON (IDEntireImage, CMChangeFormatDlg::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMChangeFormatDlg::ToSelectedImage)

	EVT_CHOICE (IDC_BandInterleave, CMChangeFormatDlg::OnSelendokBandInterleave)
	EVT_CHOICE (IDC_ChangeHeader, CMChangeFormatDlg::OnSelendokHeader)
	EVT_CHOICE (IDC_DataValueType, CMChangeFormatDlg::OnSelendokDataValueType)

	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_ChannelCombo, CMChangeFormatDlg::OnSelendokOutChannels)
	#endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_ChannelCombo, CMChangeFormatDlg::OnSelendokOutChannels)
	#endif

	EVT_CHECKBOX (IDC_OutputInWavelengthOrder, CMChangeFormatDlg::OnOutputInWavelengthOrder)
	EVT_CHECKBOX (IDC_TransformData, CMChangeFormatDlg::OnTransformData)
	EVT_CHECKBOX (IDC_WriteChanDescriptions, CMChangeFormatDlg::OnWriteChanDescriptions)

	EVT_COMBOBOX_CLOSEUP (IDC_ChannelCombo, CMChangeFormatDlg::OnChannelComboCloseUp)

	EVT_COMBOBOX_DROPDOWN (IDC_ChannelCombo, CMChangeFormatDlg::OnChannelComboDropDown)

	EVT_INIT_DIALOG (CMChangeFormatDlg::OnInitDialog)

	EVT_TEXT (IDC_ColumnEnd, CMChangeFormatDlg::CheckColumnEnd)
	EVT_TEXT (IDC_ColumnStart, CMChangeFormatDlg::CheckColumnStart)
	EVT_TEXT (IDC_ColumnInterval, CMChangeFormatDlg::CheckColumnInterval)
	EVT_TEXT (IDC_LineEnd, CMChangeFormatDlg::CheckLineEnd)
	EVT_TEXT (IDC_LineStart, CMChangeFormatDlg::CheckLineStart)
	EVT_TEXT (IDC_LineInterval, CMChangeFormatDlg::CheckLineInterval)
END_EVENT_TABLE ()



CMChangeFormatDlg::CMChangeFormatDlg (
				ReformatOptionsPtr				reformatOptionsPtr,
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title)
		: CMDialog (CMChangeFormatDlg::IDD, pParent, title)

{
	m_reformatOptionsPtr = reformatOptionsPtr;
   m_swapBytesFlag = FALSE;
   m_transformDataFlag = FALSE;
   m_channelSelection = -1;
   m_headerListSelection = -1;
   m_headerOptionsSelection = -1;
   m_dataValueListSelection = -1;
   m_outputFileSelection = -1;
   m_bandInterleaveSelection = -1;
   m_invertBottomToTopFlag = FALSE;
   m_writeChanDescriptionFlag = FALSE;
   m_invertLeftToRightFlag = FALSE;
   m_channelThematicDisplayFlag = FALSE;
   m_sessionUserSetDataValueTypeSelectionFlag = FALSE;

   m_dataValueTypeSelectionFlag = FALSE;

   m_initializedFlag = CMDialog::m_initializedFlag;
	
   CreateControls ();
	
}	// end "CMChangeFormatDlg"



void CMChangeFormatDlg::CreateControls ()

{
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);
   bSizer124 = new wxBoxSizer (wxVERTICAL);

   wxBoxSizer* bSizer125;
   bSizer125 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText136 = new wxStaticText (this,
													wxID_ANY,
													wxT("Input file:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText136->Wrap (-1);
 	bSizer125->Add (m_staticText136, wxSizerFlags(0).Border(wxRIGHT|wxBOTTOM, 5));

   m_staticText137 = new wxStaticText (this,
													IDC_InputFileName,
													wxT("filename"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText137->Wrap (-1);
	bSizer125->Add (m_staticText137, wxSizerFlags(0).Border(wxBOTTOM, 5));

	bSizer124->Add (bSizer125,
							wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT, 12));

   wxGridBagSizer* gbSizer1;
   gbSizer1 = new wxGridBagSizer (0, 0);
   gbSizer1->SetFlexibleDirection (wxBOTH);
   gbSizer1->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);

   m_staticText138 = new wxStaticText (this,
													wxID_ANY, wxT("     Lines:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText138->Wrap (-1);
   gbSizer1->Add (m_staticText138, wxGBPosition (0, 0), wxGBSpan (1, 1), wxALL, 5);
 
   m_staticText139 = new wxStaticText (this,
													IDC_ChangeFormatInLines,
													wxT("9999999"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText139->Wrap (-1);
   gbSizer1->Add (m_staticText139, wxGBPosition (0, 1), wxGBSpan (1, 1), wxALL, 5);

   m_staticText140 = new wxStaticText (this,
													IDC_ChannelsClassesLabel,
													wxT("Channels:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText140->Wrap (-1);
   gbSizer1->Add (m_staticText140,
						wxGBPosition (0, 2),
						wxGBSpan (1, 1),
						wxALL,
						5);

   m_staticText141 = new wxStaticText (this,
													IDC_ChangeFormatInChan,
													wxT("7"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText141->Wrap (-1);
   gbSizer1->Add (m_staticText141, wxGBPosition (0, 3), wxGBSpan (1, 1), wxALL, 5);

   m_staticText142 = new wxStaticText (this,
													wxID_ANY,
													wxT("     Band format:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText142->Wrap (-1);
   gbSizer1->Add (m_staticText142, wxGBPosition (0, 4), wxGBSpan (1, 1), wxALL, 5);

   m_staticText143 = new wxStaticText (this,
													IDC_ChangeFormatInInterleave,
													wxT("BSQ"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText143->Wrap (-1);
   gbSizer1->Add (m_staticText143, wxGBPosition (0, 5), wxGBSpan (1, 1), wxALL, 5);

   m_staticText144 = new wxStaticText (this,
													wxID_ANY,
													wxT("     Columns:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText144->Wrap (-1);
   gbSizer1->Add (m_staticText144, wxGBPosition (1, 0), wxGBSpan (1, 1), wxALL, 5);

   m_staticText145 = new wxStaticText (this,
													IDC_ChangeFormatInColumns,
													wxT("9999999"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText145->Wrap (-1);
   gbSizer1->Add (m_staticText145, wxGBPosition (1, 1), wxGBSpan (1, 1), wxALL, 5);

   m_staticText146 = new wxStaticText (this,
													wxID_ANY,
													wxT("Data value type:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText146->Wrap (-1);
   gbSizer1->Add (m_staticText146, wxGBPosition (1, 2), wxGBSpan (1, 1), wxALL, 5);

   m_staticText147 = new wxStaticText (this,
													IDC_ChangeFormatInBits,
													wxT("8-bit Unsigned Integer"),
													wxPoint (15,330),
													wxDefaultSize,
													0);
   m_staticText147->Wrap (-1);
   gbSizer1->Add (m_staticText147, wxGBPosition (1, 3), wxGBSpan (1, 2), wxALL, 5);

	bSizer124->Add (gbSizer1, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));

   bSizer124->Add (5,5);
	
	wxBoxSizer* bSizer127;
   bSizer127 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText148 = new wxStaticText (this,
													wxID_ANY,
													wxT("Output file:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText148->Wrap (-1);
   SetUpToolTip (m_staticText148, IDS_ToolTip76);
	bSizer127->Add (
		m_staticText148,
		wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxRIGHT|wxTOP|wxBOTTOM, 5));

	//wxChoice*		comboBox22;
   m_outputFileCtrl = new wxChoice (this,
												IDC_OutputFileSelection,
												wxDefaultPosition,
												wxDefaultSize);
   m_outputFileCtrl->Append (wxT("New File"));
   m_outputFileCtrl->Append (wxT("Append To"));
   m_outputFileCtrl->Append (wxT("Modify Part"));
   SetUpToolTip (m_outputFileCtrl, IDS_ToolTip76);
	bSizer127->Add (
				m_outputFileCtrl,
				wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxTOP|wxBOTTOM, 5));

	bSizer124->Add (bSizer127, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));

   wxGridBagSizer* gbSizer2;
   gbSizer2 = new wxGridBagSizer (0, 10);
   gbSizer2->SetFlexibleDirection (wxBOTH);
   gbSizer2->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);

   wxStaticBoxSizer* sbSizer8;
   sbSizer8 = new wxStaticBoxSizer (new wxStaticBox (this,
																		IDC_LineColFrame,
																		wxT("Area to Reformat"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL),
												wxHORIZONTAL);

	CreateLineColumnControls (sbSizer8);
	
   gbSizer2->Add (sbSizer8,
						wxGBPosition (0, 0),
						wxGBSpan (2, 1),
						wxEXPAND | wxTOP|wxRIGHT|wxBOTTOM,
						5);

   wxBoxSizer* bSizer130;
   bSizer130 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText161 = new wxStaticText (this,
													IDC_ChannelsLabel,
													wxT("Channels:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText161->Wrap (-1);
   SetUpToolTip (m_staticText161, IDS_ToolTip52);
   bSizer130->Add (
		m_staticText161,
		wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).ReserveSpaceEvenIfHidden().
																Border(wxTOP|wxRIGHT|wxBOTTOM, 5));

	GetAllSubsetMenuControl (kChannelsMenu,
										this,
										IDC_ChannelCombo,
										120,
										IDS_ToolTip52);

   bSizer130->Add (m_channelsCtrl,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).
													ReserveSpaceEvenIfHidden().Border(wxALL, 5));

   gbSizer2->Add (bSizer130, wxGBPosition (0, 1), wxGBSpan (1, 1), wxALL|wxEXPAND, 5);

   wxStaticBoxSizer* sbSizer22;
   sbSizer22 = new wxStaticBoxSizer (new wxStaticBox (this,
																		wxID_ANY,
																		wxT("Options")),
													wxVERTICAL);

   m_checkBox41 = new wxCheckBox (sbSizer22->GetStaticBox (),
												IDC_InvertBottomToTop,
												wxT("Invert bottom to top"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox41, IDS_ToolTip70);
   sbSizer22->Add (m_checkBox41, wxSizerFlags(0).Border(wxALL, 5));

   m_checkBox42 = new wxCheckBox (sbSizer22->GetStaticBox (),
												IDC_InvertRightToLeft,
												wxT("Invert right to left"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox42, IDS_ToolTip71);
   sbSizer22->Add (m_checkBox42, wxSizerFlags(0).Border(wxALL, 5));

   m_checkBox43 = new wxCheckBox (sbSizer22->GetStaticBox (),
												IDC_SwapBytes,
												wxT("Swap Bytes"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox43, IDS_ToolTip72);
   sbSizer22->Add (m_checkBox43, wxSizerFlags(0).Border(wxALL, 5));

   m_checkBox44 = new wxCheckBox (sbSizer22->GetStaticBox (),
												IDC_WriteChanDescriptions,
												wxT("Write channel descriptions"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox44, IDS_ToolTip73);
   sbSizer22->Add (m_checkBox44, wxSizerFlags(0).Border(wxALL, 5));

   m_checkBox46 = new wxCheckBox (sbSizer22->GetStaticBox (),
												IDC_OutputInWavelengthOrder,
												wxT("Output in wavelength order"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox46, IDS_ToolTip77);
   sbSizer22->Add (m_checkBox46,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

   wxBoxSizer* bSizer131;
   bSizer131 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText162 = new wxStaticText (sbSizer22->GetStaticBox (),
													wxID_ANY,
													wxT("Header"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText162->Wrap (-1);
   bSizer131->Add (m_staticText162,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

	//wxChoice*		comboBox24;
   m_fileFormatCtrl = new wxChoice (sbSizer22->GetStaticBox (),
												IDC_ChangeHeader,
												wxDefaultPosition,
												wxDefaultSize);
   m_fileFormatCtrl->Append (wxT("None"));
   m_fileFormatCtrl->Append (wxT("ArcView format"));
   m_fileFormatCtrl->Append (wxT("ERDAS 74 format"));
   m_fileFormatCtrl->Append (wxT("GAIA format"));
   m_fileFormatCtrl->Append (wxT("GeoTIFF format"));
   m_fileFormatCtrl->Append (wxT("Matlab format"));
   SetUpToolTip (m_fileFormatCtrl, IDS_ToolTip74);
   bSizer131->Add (m_fileFormatCtrl,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   sbSizer22->Add (bSizer131, wxSizerFlags(1).Expand().Border(wxALL, 5));

   gbSizer2->Add (sbSizer22,
						wxGBPosition (1, 1),
						wxGBSpan (5, 1),
						wxEXPAND | wxLEFT|wxTOP|wxBOTTOM,
						5);

   m_checkBox45 = new wxCheckBox (this,
												IDC_TransformData,
												wxT("Transform Data..."),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox45, IDS_ToolTip75);
   gbSizer2->Add (m_checkBox45,
						wxGBPosition (2, 0),
						wxGBSpan (1, 1),
						wxTOP|wxRIGHT|wxBOTTOM,
						5);
   

			// The position of m_staticText163 should not be in default
	
   m_staticText163 = new wxStaticText (this,
													IDC_MinimumBitsPrompt,
													wxT("Recommended minimum data bits:"),
													wxPoint (15, 300),
													wxDefaultSize,
													0);
   m_staticText163->Wrap (-1);
   gbSizer2->Add (m_staticText163, wxGBPosition (3, 0), wxGBSpan (1, 1), wxALL, 5);
   
   
   wxBoxSizer* bSizer132;
   bSizer132 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText164 = new wxStaticText (this,
													wxID_ANY,
													wxT("Data value type:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText164->Wrap (-1);
   bSizer132->Add (
		m_staticText164,
		wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxTOP|wxRIGHT|wxBOTTOM, 5));

   m_staticText166 = new wxStaticText (this,
													IDC_FixedDataType,
													wxT("8-bit Unsigned Integer"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText166->Wrap (-1);
   SetUpToolTip (m_staticText166, IDS_ToolTip68);
   bSizer132->Add (m_staticText166,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   m_dataValueTypeCtrl = new wxChoice (this,
													IDC_DataValueType,
													wxDefaultPosition,
													wxSize (180, -1));
   m_dataValueTypeCtrl->Append (wxT("8-bit Signed Integer"));
   m_dataValueTypeCtrl->Append (wxT("8-bit Unsigned Integer"));
   m_dataValueTypeCtrl->Append (wxT("16-bit Signed Integer"));
   m_dataValueTypeCtrl->Append (wxT("16-bit Unsigned Integer"));
   m_dataValueTypeCtrl->Append (wxT("32-bit Signed Integer"));
   m_dataValueTypeCtrl->Append (wxT("32-bit Unsigned Integer"));
   m_dataValueTypeCtrl->Append (wxT("32-bit Real"));
   m_dataValueTypeCtrl->Append (wxT("64-bit Real"));
   SetUpToolTip (m_dataValueTypeCtrl, IDS_ToolTip68);
   bSizer132->Add (m_dataValueTypeCtrl,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   gbSizer2->Add (bSizer132,
						wxGBPosition (4, 0),
						wxGBSpan (1, 1),
						wxEXPAND | wxTOP|wxRIGHT|wxBOTTOM,
						5);

   wxBoxSizer* bSizer133;
   bSizer133 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText165 = new wxStaticText (this,
													IDC_BandInterleaveLabel,
													wxT("Band format:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText165->Wrap (-1);
   SetUpToolTip (m_staticText165, IDS_ToolTip69);
   bSizer133->Add (
		m_staticText165,
		wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxTOP|wxRIGHT|wxBOTTOM, 5));

	//wxChoice*		comboBox26;
   m_bandInterleaveCtrl = new wxChoice (this,
														IDC_BandInterleave,
														wxDefaultPosition,
														wxSize (220, -1));
   m_bandInterleaveCtrl->Append (wxT("BIL- Band Interleave by Line"));
   m_bandInterleaveCtrl->Append (wxT("BSQ- Band Sequential"));
   m_bandInterleaveCtrl->Append (wxT("BIS- Band Interleave by Sample"));
   SetUpToolTip (m_bandInterleaveCtrl, IDS_ToolTip69);
   bSizer133->Add (
				m_bandInterleaveCtrl,
				wxSizerFlags(0).ReserveSpaceEvenIfHidden().
								Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT|wxTOP, 5));

   gbSizer2->Add (bSizer133,
						wxGBPosition (5, 0),
						wxGBSpan (1, 1),
						wxEXPAND | wxTOP|wxRIGHT,
						5);

	bSizer124->Add (gbSizer2,
							wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	CreateStandardButtons (bSizer124);

   Layout ();
   Centre ();

}	// end "CreateControls"



Boolean CMChangeFormatDlg::DoDialog (
				FileInfoPtr							outFileInfoPtr)

{
   SInt16 								returnCode;
	Boolean 								OKFlag = FALSE;


			// Make sure intialization has been completed.

   if (!m_initializedFlag)
																					return (FALSE);

   returnCode = ShowModal ();

   if (returnCode == wxID_OK)
		{
      OKFlag = TRUE;

				// Reformat area

      m_dialogSelectArea.lineStart = m_LineStart;
      m_dialogSelectArea.lineEnd = m_LineEnd;
      m_dialogSelectArea.lineInterval = m_LineInterval;

      m_dialogSelectArea.columnStart = m_ColumnStart;
      m_dialogSelectArea.columnEnd = m_ColumnEnd;
      m_dialogSelectArea.columnInterval = m_ColumnInterval;

      OKFlag = ChangeImageFormatDialogOK (this,
														  gImageWindowInfoPtr,
														  gImageLayerInfoPtr,
														  gImageFileInfoPtr,
														  outFileInfoPtr,
														  m_reformatOptionsPtr,
														  &m_dialogSelectArea,
														  m_outputFileSelection + 1,
														  m_bandInterleaveSelection + 1,
														  m_dataValueTypeSelection,
														  m_sessionUserSetDataValueTypeSelectionFlag,
														  m_channelSelection,
														  m_invertBottomToTopFlag,
														  m_invertLeftToRightFlag,
														  m_swapBytesFlag,
														  m_writeChanDescriptionFlag,
														  m_outputInWavelengthOrderFlag,
														  m_headerOptionsSelection,
														  m_channelThematicDisplayFlag);

		}	// end "if (returnCode == IDOK)"

   return (OKFlag);

}	// end "DoDialog"



void CMChangeFormatDlg::OnInitDialog (
				wxInitDialogEvent&				event)

{
   CMDialog*							dialogPtr = this;

   SInt16								bandInterleaveSelection,
											outputFileSelection,
											dataValueTypeSelection,
											channelSelection,
											headerOptionsSelection;

   Boolean								invertBottomToTopFlag,
											invertLeftToRightFlag,
											swapBytesFlag;


   //wxDialog::OnInitDialog (event);

   ChangeImageFormatDialogInitialize (dialogPtr,
													gImageWindowInfoPtr,
													gImageFileInfoPtr,
													m_reformatOptionsPtr,
													&m_dialogSelectArea,
													m_inputBandInterleaveString,
													m_inputDataValueTypeString,
													m_tiffMenuNameString,
													&outputFileSelection,
													&bandInterleaveSelection,
													&dataValueTypeSelection,
													&m_savedDataValueTypeSelection,
													&channelSelection,
													&invertBottomToTopFlag,
													&invertLeftToRightFlag,
													&swapBytesFlag,
													&m_channelDescriptionAllowedFlag,
													&m_savedChannelDescriptionFlag,
													&m_outputInWavelengthOrderAllowedFlag,
													&m_outputInWavelengthOrderFlag,
													&headerOptionsSelection,
													&m_GAIAFormatAllowedFlag,
													&m_channelThematicDisplayFlag);
   
	m_LineStart = m_dialogSelectArea.lineStart;
	m_LineEnd = m_dialogSelectArea.lineEnd;
	m_LineInterval = m_dialogSelectArea.lineInterval;
	m_ColumnInterval = m_dialogSelectArea.columnInterval;
	m_ColumnStart = m_dialogSelectArea.columnStart;
	m_ColumnEnd = m_dialogSelectArea.columnEnd;

			// Be sure to make 0 based.

   m_outputFileSelection = outputFileSelection - 1;
   m_bandInterleaveSelection = bandInterleaveSelection - 1;
   m_dataValueTypeSelection = dataValueTypeSelection;
   m_channelSelection = channelSelection;
   m_invertBottomToTopFlag = invertBottomToTopFlag;
   m_invertLeftToRightFlag = invertLeftToRightFlag;
   m_swapBytesFlag = swapBytesFlag;
   m_writeChanDescriptionFlag = m_savedChannelDescriptionFlag;
   m_headerOptionsSelection = headerOptionsSelection;
   
   if (m_reformatOptionsPtr->transformDataCode != kNoTransform)
		m_transformDataFlag = TRUE;

   m_dataValueTypeCtrl->SetClientData (0, (void*)(SInt64)g1ByteSignedIntegerMenuItem);
   m_dataValueTypeCtrl->SetClientData (1, (void*)(SInt64)g1ByteUnsignedIntegerMenuItem);
   m_dataValueTypeCtrl->SetClientData (2, (void*)(SInt64)g2ByteSignedIntegerMenuItem);
   m_dataValueTypeCtrl->SetClientData (3, (void*)(SInt64)g2ByteUnsignedIntegerMenuItem);
   m_dataValueTypeCtrl->SetClientData (4, (void*)(SInt64)g4ByteSignedIntegerMenuItem);
   m_dataValueTypeCtrl->SetClientData (5, (void*)(SInt64)g4ByteUnsignedIntegerMenuItem);
   m_dataValueTypeCtrl->SetClientData (6, (void*)(SInt64)g4ByteRealMenuItem);
   m_dataValueTypeCtrl->SetClientData (7, (void*)(SInt64)g8ByteRealMenuItem);

   m_dataValueListSelection = GetComboListSelection (IDC_DataValueType,
           															m_dataValueTypeSelection);
			
	if (m_dataValueTypeSelection < 0)
		MHideDialogItem ((DialogPtr)dialogPtr, IDC_DataValueType);
	
	else	// m_dataValueTypeSelection >= 0
		MHideDialogItem ((DialogPtr)dialogPtr, IDC_FixedDataType);

   m_localActiveFeaturesPtr = (UInt16*)m_reformatOptionsPtr->channelPtr;

   if (!m_GAIAFormatAllowedFlag)
      m_fileFormatCtrl->Delete (kGAIAMenuItem);

   m_headerListSelection = GetComboListSelection (IDC_ChangeHeader,
           														m_headerOptionsSelection);

   TransferDataToWindow ();
      
   		// Set default text selection to first edit text item

   SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);
	SetSizerAndFit (bSizer124);
   Fit ();
	PositionDialogWindow ();
	
}	// end "OnInitDialog"



void CMChangeFormatDlg::OnOutputInWavelengthOrder (
				wxCommandEvent& 					event)

{
   wxCheckBox* cbox = (wxCheckBox*)FindWindow (IDC_OutputInWavelengthOrder);
   m_outputInWavelengthOrderFlag = cbox->GetValue ();

}	// end "OnOutputInWavelengthOrder"



void CMChangeFormatDlg::OnSelendokBandInterleave (
				wxCommandEvent& 					event)

{
	SInt16 								headerOptionsSelection,
   										outputFileSelection;

   Boolean 								changedFlag;


   m_bandInterleaveSelection = m_bandInterleaveCtrl->GetSelection ();

   headerOptionsSelection = m_headerOptionsSelection;
   outputFileSelection = m_outputFileSelection;
   
   ChangeImageFormatDialogVerifyHeaderSetting (this,
																gImageFileInfoPtr,
																m_bandInterleaveSelection,
																m_dataValueTypeSelection,
																gPopUpOutputFileMenu,
																gPopUpHeaderOptionsMenu,
																&outputFileSelection,
																&headerOptionsSelection);

   changedFlag = (m_headerOptionsSelection != headerOptionsSelection);

   m_headerOptionsSelection = headerOptionsSelection;
   m_outputFileSelection = outputFileSelection;
   m_headerListSelection = GetComboListSelection (IDC_ChangeHeader,
																	m_headerOptionsSelection);
	m_fileFormatCtrl->SetSelection (m_headerListSelection);

   if (changedFlag)
      OnSelendokHeader (event);

}	// end "OnSelendokBandInterleave"



void CMChangeFormatDlg::OnSelendokDataValueType (
				wxCommandEvent&					event)

{
   SInt16								bandInterleaveSelection,
											dataValueListSelection,
											headerOptionsSelection,
											outputFileSelection;
											

   dataValueListSelection = m_dataValueListSelection;
   m_dataValueListSelection = m_dataValueTypeCtrl->GetSelection ();

   if (dataValueListSelection != m_dataValueListSelection) 
		{
      SInt64 dataValueList64 =
      			(SInt64)((int*)m_dataValueTypeCtrl->GetClientData (m_dataValueListSelection));
      m_dataValueTypeSelection = (SInt16)dataValueList64;
      
      outputFileSelection = m_outputFileSelection;
      
      bandInterleaveSelection = m_bandInterleaveSelection;

      headerOptionsSelection = m_headerOptionsSelection;

      ChangeImageFormatDialogUpdateHeaderMenu (
														this,
														gImageFileInfoPtr,
														m_tiffMenuNameString,
														&outputFileSelection,
														&bandInterleaveSelection,
														&headerOptionsSelection,
														gPopUpOutputFileMenu,
														gPopUpHeaderOptionsMenu,
														m_reformatOptionsPtr->transformDataCode,
														m_dataValueTypeSelection,
														m_GAIAFormatAllowedFlag);
      
      m_outputFileSelection = outputFileSelection;
      m_bandInterleaveSelection = bandInterleaveSelection;
      m_headerOptionsSelection = headerOptionsSelection;

      m_bandInterleaveCtrl->SetSelection (m_bandInterleaveSelection);

      m_headerListSelection = GetComboListSelection (IDC_ChangeHeader,
																		m_headerOptionsSelection);
      m_fileFormatCtrl->SetSelection (m_headerListSelection);

		}	// end "if (dataValueTypeSelection != m_dataValueTypeSelection)"

   if (m_reformatOptionsPtr->transformDataCode == kNoTransform)
      m_savedDataValueTypeSelection = m_dataValueTypeSelection;

   m_sessionUserSetDataValueTypeSelectionFlag = TRUE;

}	// end "OnSelendokDataValueType"



void CMChangeFormatDlg::OnSelendokHeader (
				wxCommandEvent&					event)

{
	SInt16								bandInterleaveSelection,
											dataValueTypeSelection,
											outputFileSelection = 0;

   Boolean								setChannelDescriptionFlag;


   dataValueTypeSelection = m_dataValueTypeSelection;
	bandInterleaveSelection = m_bandInterleaveSelection;

	m_headerListSelection = m_fileFormatCtrl->GetSelection ();
   m_headerOptionsSelection =
   		(SInt64)((int*)(m_fileFormatCtrl->GetClientData (m_headerListSelection)));
	
   ChangeImageFormatDialogUpdateHeaderOptions (this,
															  gImageFileInfoPtr,
															  m_headerOptionsSelection,
															  &outputFileSelection,
															  &bandInterleaveSelection,
															  &dataValueTypeSelection,
															  &setChannelDescriptionFlag,
															  gPopUpOutputFileMenu,
															  m_channelDescriptionAllowedFlag,
															  m_channelThematicDisplayFlag);

   if (outputFileSelection == kNewFileMenuItem)
      m_outputFileSelection = outputFileSelection - 1;
	
   m_bandInterleaveSelection = bandInterleaveSelection;

   if (m_dataValueTypeSelection != dataValueTypeSelection) 
		{
      m_dataValueTypeSelection = dataValueTypeSelection;

      m_dataValueListSelection = GetComboListSelection (
      											IDC_DataValueType, m_dataValueTypeSelection);

		}	// end "if (m_dataValueTypeSelection != dataValueTypeSelection)"

   if (setChannelDescriptionFlag)
      SetChannelDescriptionFlag ();

   m_bandInterleaveCtrl->SetSelection (m_bandInterleaveSelection);

   m_outputFileCtrl->SetSelection (m_outputFileSelection);

   m_dataValueTypeCtrl->SetSelection (m_dataValueListSelection);
  
}	// end "OnSelendokHeader"



void CMChangeFormatDlg::OnSelendokOutChannels (
				wxCommandEvent& 					event)

{
   m_localActiveNumberFeatures = m_reformatOptionsPtr->numberChannels;

   HandleChannelsMenu (IDC_ChannelCombo,
								kNoTransformation,
								(SInt16) gImageWindowInfoPtr->totalNumberChannels,
								kImageChannelType,
								TRUE);

	m_reformatOptionsPtr->numberChannels = m_localActiveNumberFeatures;

}	// end "OnSelendokOutChannels"



void CMChangeFormatDlg::OnTransformData (
				wxCommandEvent& 					event)

{
	DialogPtr							dialogPtr = this;
	
   SInt16								bandInterleaveSelection,
											headerOptionsSelection,
											numChars,
											outputFileSelection;
								
   UInt16								recommendNumberOfBits,
											suggestedDataValueTypeSelection;

	Boolean								returnFlag = FALSE;


   SetDLogControlHilite (NULL, wxID_OK, 255);
   CMReformatTransformDlg* transformDialogPtr = NULL;
   
   transformDialogPtr = new CMReformatTransformDlg (m_reformatOptionsPtr);

   returnFlag = transformDialogPtr->DoDialog (
           							&recommendNumberOfBits, m_bandInterleaveSelection);
   
   if (transformDialogPtr != NULL)
      delete transformDialogPtr;

   if (returnFlag)
		{
      SetChannelDescriptionFlag ();

      if (m_reformatOptionsPtr->transformDataCode == kNoTransform)
			{
         m_transformDataFlag = FALSE;

         MHideDialogItem (dialogPtr, IDC_MinimumBitsPrompt);

         m_dataValueTypeSelectionFlag = TRUE;
         m_dataValueTypeSelection = m_savedDataValueTypeSelection;
							
			if (m_outputInWavelengthOrderAllowedFlag)
				{
				SetDLogControl (this,
										IDC_OutputInWavelengthOrder,
										m_outputInWavelengthOrderFlag);
				SetDLogControlHilite (this, IDC_OutputInWavelengthOrder, 0);
				
				}	// end "if (outputInWavelengthOrderAllowedFlag)"
			
			}	// end "if (...->transformDataCode == kNoTransform)"

		else // m_reformatOptionsPtr->transformDataCode != kNoTransform
			{
         m_transformDataFlag = TRUE;

         MShowDialogItem (dialogPtr, IDC_MinimumBitsPrompt);
			
			if (recommendNumberOfBits > 16)
				{
				numChars = sprintf ((char*)&gTextString2[1],
											"Recommended minimum data bits: %d-Real",
											recommendNumberOfBits);
				suggestedDataValueTypeSelection = k4ByteRealMenuItem;
				
				}	// end "if (recommendNumberOfBits > 16)"
			
			else	// recommendNumberOfBits <= 16
				{
				numChars = sprintf ((char*)&gTextString2[1],
											"Recommended minimum data bits: %d",
											recommendNumberOfBits);
				suggestedDataValueTypeSelection = k2ByteSignedIntegerMenuItem;
				 
				}	// end "else	// recommendNumberOfBits <= 16"
			
					// The suggested data value type selection will be forced if the
					// user has not set it before and the suggested setting contains
					// more bits than the previous setting.
					// Also the header format setting will be verified to match the new
					// data value type setting.
									
			if (!m_sessionUserSetDataValueTypeSelectionFlag &&
					!m_reformatOptionsPtr->userSetDataValueTypeSelectionFlag &&
						m_dataValueTypeSelection == m_savedDataValueTypeSelection &&
								suggestedDataValueTypeSelection > m_dataValueTypeSelection)
				{
				m_dataValueTypeSelection = suggestedDataValueTypeSelection;
				
				outputFileSelection = m_outputFileSelection;
				bandInterleaveSelection = m_bandInterleaveSelection;
				headerOptionsSelection = m_headerOptionsSelection;
			
				ChangeImageFormatDialogUpdateHeaderMenu (
															dialogPtr,
															gImageFileInfoPtr,
															m_tiffMenuNameString,
															&outputFileSelection,
															&bandInterleaveSelection,
															&headerOptionsSelection,
															gPopUpOutputFileMenu,
															gPopUpHeaderOptionsMenu,
															m_reformatOptionsPtr->transformDataCode,
															m_dataValueTypeSelection,
															m_GAIAFormatAllowedFlag);
													
				m_outputFileSelection = outputFileSelection;
				m_bandInterleaveSelection = bandInterleaveSelection;
				m_headerOptionsSelection = headerOptionsSelection;
													
				}	// end "if (!m_sessionUserSetDataValueTypeSelectionFlag && ..."
				
         gTextString2[0] = (UInt8)numChars;
         LoadDItemString (IDC_MinimumBitsPrompt, (char*)&gTextString2[1]);
							
			if (m_outputInWavelengthOrderAllowedFlag)
				{
				SetDLogControl (this,
										IDC_OutputInWavelengthOrder,
										0);
				SetDLogControlHilite (this, IDC_OutputInWavelengthOrder, 255);
				
				}	// end "if (outputInWavelengthOrderAllowedFlag)"
			
			}	// end "else ...->transformDataCode != kNoTransform"
		
		m_dataValueListSelection = GetComboListSelection (IDC_DataValueType,
																			m_dataValueTypeSelection);
		m_dataValueTypeCtrl->SetSelection (m_dataValueListSelection);

				// Input channels will be defined by the transformation.

      if (m_reformatOptionsPtr->transformDataCode == kNoTransform ||
              m_reformatOptionsPtr->transformDataCode == kAdjustChannel ||
              m_reformatOptionsPtr->transformDataCode == kAdjustChannelsByChannel ||
              m_reformatOptionsPtr->transformDataCode == kFunctionOfChannels)
			{
         MShowDialogItem (dialogPtr, IDC_ChannelsLabel);
         MShowDialogItem (dialogPtr, IDC_ChannelCombo);

			}	// end "if (...->transformDataCode == kNoTransform || ...)"

      else // ...->transformDataCode == kTransformChannels ||
         //			transformDataCode == kCreatePCImage)
			{
			MHideDialogItem (dialogPtr, IDC_ChannelsLabel);
			MHideDialogItem (dialogPtr, IDC_ChannelCombo);

			}	// end "else ...->transformDataCode == kTransformChannels || ..."

		}	// end "if (returnFlag)"

			// Make sure that the transform check box has the appropriate setting.

   wxCheckBox* cbox = (wxCheckBox*)FindWindow (IDC_TransformData);
   cbox->SetValue (m_transformDataFlag);
   
   SetDLogControlHilite	(NULL, wxID_OK, 0);
   
}	// end "OnTransformData"


void CMChangeFormatDlg::OnWriteChanDescriptions (
				wxCommandEvent& 					event)

{
   wxCheckBox* cbox = (wxCheckBox*)FindWindow (IDC_WriteChanDescriptions);
   m_writeChanDescriptionFlag = cbox->GetValue ();
   m_savedChannelDescriptionFlag = m_writeChanDescriptionFlag;

}	// end "OnWriteChanDescriptions"



void CMChangeFormatDlg::SetChannelDescriptionFlag (void)

{
   DialogPtr dialogPtr = this;
   wxCheckBox* cbox;

   if (m_reformatOptionsPtr->transformDataCode == kNoTransform ||
           			m_reformatOptionsPtr->transformDataCode == kAdjustChannel)
		{
      if (m_channelDescriptionAllowedFlag)
			{
         m_writeChanDescriptionFlag = m_savedChannelDescriptionFlag;

         cbox = (wxCheckBox*)FindWindow (IDC_WriteChanDescriptions);
         cbox->SetValue (m_writeChanDescriptionFlag);
         SetDLogControlHilite (dialogPtr, IDC_WriteChanDescriptions, 0);

			}	// end "if (m_channelDescriptionAllowedFlag)"

		}	// end "if (...->transformDataCode == kNoTransform || ..."

   else	// ...->transformDataCode == kTransformChannels || ...
		{
      m_writeChanDescriptionFlag = FALSE;
      cbox = (wxCheckBox*)FindWindow (IDC_WriteChanDescriptions);
      cbox->SetValue (m_writeChanDescriptionFlag);
      SetDLogControlHilite (dialogPtr, IDC_WriteChanDescriptions, 255);

		}	// end "else ...->transformDataCode == kTransformChannels ..."

}	// end "SetChannelDescriptionFlag"



bool CMChangeFormatDlg::TransferDataFromWindow ()

{
	SInt16								returnCode;
	
	
   wxCheckBox* swapb = (wxCheckBox*)FindWindow (IDC_SwapBytes);
   m_swapBytesFlag = swapb->GetValue ();
	
   wxCheckBox* tdata = (wxCheckBox*)FindWindow (IDC_TransformData);
   m_transformDataFlag = tdata->GetValue ();
	
   wxCheckBox* ibt = (wxCheckBox*)FindWindow (IDC_InvertBottomToTop);
   m_invertBottomToTopFlag = ibt->GetValue ();
	
   wxCheckBox* writechan = (wxCheckBox*)FindWindow (IDC_WriteChanDescriptions);
   m_writeChanDescriptionFlag = writechan->GetValue ();
	
	wxCheckBox* wavelengthOrder =
										(wxCheckBox*)FindWindow (IDC_OutputInWavelengthOrder);
	m_outputInWavelengthOrderFlag = wavelengthOrder->GetValue ();
	
   wxCheckBox* irl = (wxCheckBox*)FindWindow (IDC_InvertRightToLeft);
   m_invertLeftToRightFlag = irl->GetValue ();

   m_channelSelection = m_channelsCtrl->GetSelection ();
	
   m_headerListSelection = m_fileFormatCtrl->GetSelection ();
	
   m_dataValueListSelection = m_dataValueTypeCtrl->GetSelection ();
	
   m_outputFileSelection = m_outputFileCtrl->GetSelection ();
	
   m_bandInterleaveSelection = m_bandInterleaveCtrl->GetSelection ();
	
	TransferLinesColumnsFromWindow ();
	
	returnCode = VerifyLineColumnValues (IDC_LineStart,
														IDC_ColumnStart,
														true);

   return (returnCode == 0);
	
}	// end "TransferDataFromWindow"



bool CMChangeFormatDlg::TransferDataToWindow ()

{
	TransferLinesColumnsToWindow ();

   wxCheckBox* tdata = (wxCheckBox*)FindWindow (IDC_TransformData);
   tdata->SetValue (m_transformDataFlag);
	
   wxCheckBox* ibt = (wxCheckBox*)FindWindow (IDC_InvertBottomToTop);
   ibt->SetValue (m_invertBottomToTopFlag);
	
   wxCheckBox* irl = (wxCheckBox*)FindWindow (IDC_InvertRightToLeft);
   irl->SetValue (m_invertLeftToRightFlag);
	
   wxCheckBox* swapb = (wxCheckBox*)FindWindow (IDC_SwapBytes);
   swapb->SetValue (m_swapBytesFlag);
	
   wxCheckBox* writechan = (wxCheckBox*)FindWindow (IDC_WriteChanDescriptions);
   writechan->SetValue (m_writeChanDescriptionFlag);
	
	wxCheckBox* wavelengthOrder = (wxCheckBox*)FindWindow (IDC_OutputInWavelengthOrder);
	wavelengthOrder->SetValue (m_outputInWavelengthOrderFlag);

   m_outputFileCtrl->SetSelection (m_outputFileSelection);
	
   m_dataValueTypeCtrl->SetSelection (m_dataValueListSelection);
	
   m_bandInterleaveCtrl->SetSelection (m_bandInterleaveSelection);
	
   m_channelsCtrl->SetSelection (m_channelSelection);
   
   m_fileFormatCtrl->SetSelection (m_headerListSelection);

   return true;
	
}	// end "TransferDataToWindow"

