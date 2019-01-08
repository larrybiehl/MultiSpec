//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LReformatChangeDialog.cpp : class implementation file
//	Class Definition:		LReformatChangeDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/07/2019
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMChangeFormatDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "CImageWindow.h"

#include "LImageView.h"
#include "LImage_dialog.cpp"
#include "LReformatChangeDialog.h"
#include "LReformatTransformDialog.h"

#include "wx/gbsizer.h"

extern void ChangeImageFormatDialogInitialize (
				DialogPtr							dialogPtr,
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				UCharPtr								inputBandInterleaveStringPtr,
				UCharPtr								inputDataValueTypeStringPtr,
				UCharPtr								tiffMenuNameStringPtr,
				SInt16*								outputFileSelectionPtr,
				SInt16*								bandInterleaveSelectionPtr,
				SInt16*								dataValueTypeSelectionPtr,
				SInt16*								savedDataValueTypeSelectionPtr,
				SInt16*								channelSelectionPtr,
				Boolean*								bottomToTopFlagPtr,
				Boolean*								rightToLeftFlagPtr,
				Boolean*								swapBytesFlagPtr,
				Boolean*								channelDescriptionAllowedFlagPtr,
				Boolean*								savedChannelDescriptionFlagPtr,
				Boolean*								outputInWavelengthOrderAllowedFlagPtr,
				Boolean*								outputInWavelengthOrderFlagPtr,
				SInt16*								headerOptionsSelectionPtr,
				Boolean*								GAIAFormatAllowedFlagPtr,
				Boolean*								channelThematicDisplayFlagPtr);

extern Boolean ChangeImageFormatDialogOK (
				DialogPtr							dialogPtr,
				WindowInfoPtr						imageWindowInfoPtr,
				LayerInfoPtr						imageLayerInfoPtr,
				FileInfoPtr							imageFileInfoPtr,
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				SInt16								outputFileSelection,
				SInt16								bandInterleaveSelection,
				SInt16								dataValueTypeSelection,
				Boolean								sessionUserSetDataValueTypeSelectionFlag,
				SInt16								channelSelection,
				Boolean								bottomToTopFlag,
				Boolean								rightToLeftFlag,
				Boolean								swapBytesFlag,
				Boolean								channelDescriptionsFlag,
				Boolean								outputInWavelengthOrderFlag,
				SInt16								headerOptionsSelection,
				Boolean								channelThematicDisplayFlag);

extern void ChangeImageFormatDialogUpdateHeaderMenu (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				UCharPtr								tiffMenuNameStringPtr,
				SInt16*								outputFileSelectionPtr,
				SInt16*								bandInterleaveSelectionPtr,
				SInt16*								headerOptionsSelectionPtr,
				MenuHandle							popUpOutputFileMenu,
				MenuHandle							popUpHeaderOptionsMenu,
				SInt16								transformDataCode,
				SInt16								dataValueTypeSelection,
				Boolean								GAIAFormatAllowedFlag);

extern void ChangeImageFormatDialogUpdateHeaderOptions (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16								headerOptionsSelection,
				SInt16*								outputFileSelectionPtr,
				SInt16*								bandInterleaveSelectionPtr,
				SInt16*								dataValueTypeSelectionPtr,
				Boolean*								setChannelDescriptionFlagPtr,
				MenuHandle							popUpOutputFileMenu,
				Boolean								channelDescriptionAllowedFlag,
				Boolean								channelThematicDisplayFlag);


extern void ChangeImageFormatDialogUpdateTIFFHeader (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16*								outputFileSelectionPtr,
				SInt16*								bandInterleaveSelectionPtr,
				MenuHandle							popUpOutputFileMenu);

extern void ChangeImageFormatDialogVerifyHeaderSetting (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16								bandInterleaveSelection,
				SInt16								dataValueTypeSelection,
				MenuHandle							popUpOutputFileMenu,
				MenuHandle							popUpHeaderOptionsMenu,
				SInt16*								outputFileSelectionPtr,
				SInt16*								headerOptionsSelectionPtr);



extern SInt16 GetPCChannelList (
				ReformatOptionsPtr				reformatOptionsPtr);

extern SInt16 GetTransformChannelList (
				ReformatOptionsPtr				reformatOptionsPtr);



// === Static Member Variable === 

ReformatOptionsPtr CMChangeFormatDlg::s_reformatOptionsPtr = NULL;



IMPLEMENT_DYNAMIC_CLASS (CMChangeFormatDlg, CMDialog)

CMChangeFormatDlg::CMChangeFormatDlg () : CMDialog ()
	{
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
	// The following was used for testing. Not needed now.
	//bSizer124 = NULL;
	
   m_dataValueTypeSelectionFlag = FALSE;

   m_initializedFlag = CMDialog::m_initializedFlag;
	
}	// end "CMChangeFormatDlg"



CMChangeFormatDlg::CMChangeFormatDlg (wxWindow* pParent, wxWindowID id, const wxString& title)
: CMDialog(CMChangeFormatDlg::IDD, pParent, title)

{
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
	// The following was used for testing. Not needed now.
	//bSizer124 = NULL;
  
   //}}AFX_DATA_INIT

   m_dataValueTypeSelectionFlag = FALSE;

   m_initializedFlag = CMDialog::m_initializedFlag;
   CreateControls();
   //GetSizer()->Fit(this);
   //GetSizer()->SetSizeHints(this);
   Centre();
	
}	// end "CMChangeFormatDlg"


BEGIN_EVENT_TABLE(CMChangeFormatDlg, CMDialog)
	EVT_INIT_DIALOG(CMChangeFormatDlg::OnInitDialog)
	EVT_BUTTON(IDEntireImage, CMChangeFormatDlg::ToEntireImage)
	EVT_BUTTON(IDSelectedImage, CMChangeFormatDlg::ToSelectedImage)
	EVT_CHECKBOX(IDC_TransformData, CMChangeFormatDlg::OnTransformData)
	EVT_COMBOBOX(IDC_BandInterleave, CMChangeFormatDlg::OnSelendokBandInterleave)
	EVT_COMBOBOX(IDC_ChannelCombo, CMChangeFormatDlg::OnSelendokOutChannels)
	EVT_COMBOBOX_DROPDOWN(IDC_ChannelCombo, CMChangeFormatDlg::OnSelendokChannelComboDropDown)
	EVT_COMBOBOX(IDC_DataValueType, CMChangeFormatDlg::OnSelendokDataValueType)
	EVT_COMBOBOX(IDC_ChangeHeader, CMChangeFormatDlg::OnSelendokHeader)
	EVT_TEXT(IDC_ColumnEnd, CMChangeFormatDlg::CheckColumnEnd)
	EVT_TEXT(IDC_ColumnStart, CMChangeFormatDlg::CheckColumnStart)
	EVT_TEXT(IDC_LineEnd, CMChangeFormatDlg::CheckLineEnd)
	EVT_TEXT(IDC_LineStart, CMChangeFormatDlg::CheckLineStart)
	EVT_TEXT(IDC_LineInterval, CMChangeFormatDlg::CheckLineInterval)
	EVT_TEXT(IDC_ColumnInterval, CMChangeFormatDlg::CheckColumnInterval)
	//EVT_CHAR_HOOK(CMChangeFormatDlg::OnButtonPress)
	EVT_CHECKBOX (IDC_WriteChanDescriptions, CMChangeFormatDlg::OnWriteChanDescriptions)
	EVT_CHECKBOX (IDC_OutputInWavelengthOrder, CMChangeFormatDlg::OnOutputInWavelengthOrder)
	//EVT_CHECKBOX(IDC_InvertBottomToTop, OnWriteChanDescriptions)
	//EVT_CHECKBOX(IDC_InvertRightToLeft, OnWriteChanDescriptions)
	//EVT_CHECKBOX(IDC_SwapBytes, OnWriteChanDescriptions)
END_EVENT_TABLE()



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
   gbSizer1->Add (m_staticText141, wxGBPosition(0, 3), wxGBSpan (1, 1), wxALL, 5);

   m_staticText142 = new wxStaticText (this,
													wxID_ANY,
													wxT("     Band format:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText142->Wrap (-1);
   gbSizer1->Add(m_staticText142, wxGBPosition(0, 4), wxGBSpan(1, 1), wxALL, 5);

   m_staticText143 = new wxStaticText (this,
													IDC_ChangeFormatInInterleave,
													wxT("BSQ"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText143->Wrap (-1);
   gbSizer1->Add (m_staticText143, wxGBPosition(0, 5), wxGBSpan(1, 1), wxALL, 5);

   m_staticText144 = new wxStaticText (this,
													wxID_ANY,
													wxT("     Columns:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText144->Wrap (-1);
   gbSizer1->Add(m_staticText144, wxGBPosition(1, 0), wxGBSpan(1, 1), wxALL, 5);

   m_staticText145 = new wxStaticText (this,
													IDC_ChangeFormatInColumns,
													wxT("9999999"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText145->Wrap (-1);
   gbSizer1->Add (m_staticText145, wxGBPosition(1, 1), wxGBSpan(1, 1), wxALL, 5);

   m_staticText146 = new wxStaticText (this,
													wxID_ANY,
													wxT("Data value type:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText146->Wrap (-1);
   gbSizer1->Add (m_staticText146, wxGBPosition(1, 2), wxGBSpan(1, 1), wxALL, 5);

   m_staticText147 = new wxStaticText (this,
													IDC_ChangeFormatInBits,
													wxT("8-bit Unsigned Integer"),
													wxPoint(15,330),
													wxDefaultSize,
													0);
   m_staticText147->Wrap (-1);
   gbSizer1->Add (m_staticText147, wxGBPosition(1, 3), wxGBSpan(1, 2), wxALL, 5);

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
   m_staticText148->Wrap(-1);
   SetUpToolTip(m_staticText148, IDS_ToolTip76); 
	bSizer127->Add (
		m_staticText148,
		wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxRIGHT|wxTOP|wxBOTTOM, 5));

   m_comboBox22 = new wxComboBox(this, IDC_OutputFileSelection, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
   m_comboBox22->Append(wxT("New File"));
   m_comboBox22->Append(wxT("Append To"));
   m_comboBox22->Append(wxT("Modify Part"));
   SetUpToolTip (m_comboBox22, IDS_ToolTip76);
	bSizer127->Add (
				m_comboBox22,
				wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxTOP|wxBOTTOM, 5));

	bSizer124->Add (bSizer127, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));

   wxGridBagSizer* gbSizer2;
   gbSizer2 = new wxGridBagSizer (0, 10);
   gbSizer2->SetFlexibleDirection (wxBOTH);
   gbSizer2->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);

   wxStaticBoxSizer* sbSizer8;
   sbSizer8 = new wxStaticBoxSizer (new wxStaticBox (this,
																		IDC_LineColFrame,
																		wxT("Area to Reformat")),
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
		wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxTOP|wxRIGHT|wxBOTTOM, 5));

   m_comboBox23 = new wxComboBox (this,
												IDC_ChannelCombo,
												wxT("All"),
												wxDefaultPosition,
												wxSize(-1, -1),
												0,
												NULL,
												wxCB_READONLY);
   m_comboBox23->Append (wxT("All"));
   m_comboBox23->Append (wxT("Subset..."));
   SetUpToolTip (m_comboBox23, IDS_ToolTip52);
   bSizer130->Add (m_comboBox23,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

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
   //						wxSizerFlags(0).Border(wxALL, 5));

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

   m_comboBox24 = new wxComboBox (sbSizer22->GetStaticBox (),
												IDC_ChangeHeader,
												wxT("None"),
												wxDefaultPosition,
												wxDefaultSize,
												0,
												NULL,
												wxCB_READONLY);
   m_comboBox24->Append (wxT("None"));
   m_comboBox24->Append (wxT("ArcView format"));
   m_comboBox24->Append (wxT("ERDAS 74 format"));
   m_comboBox24->Append (wxT("GAIA format"));
   m_comboBox24->Append (wxT("GeoTIFF format"));
   m_comboBox24->Append (wxT("Matlab format"));
   SetUpToolTip (m_comboBox24, IDS_ToolTip74);
   bSizer131->Add (m_comboBox24,
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
   bSizer132 = new wxBoxSizer(wxHORIZONTAL);

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
   
   
   m_comboBox25 = new wxComboBox (this,
												IDC_DataValueType,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (180, -1),
												0,
												NULL,
												wxCB_READONLY);
   m_comboBox25->Append (wxT("8-bit Signed Integer"));
   m_comboBox25->Append (wxT("8-bit Unsigned Integer"));
   m_comboBox25->Append (wxT("16-bit Signed Integer"));
   m_comboBox25->Append (wxT("16-bit Unsigned Integer"));
   m_comboBox25->Append (wxT("32-bit Signed Integer"));
   m_comboBox25->Append (wxT("32-bit Unsigned Integer"));
   m_comboBox25->Append (wxT("32-bit Real"));
   m_comboBox25->Append (wxT("64-bit Real"));
   SetUpToolTip (m_comboBox25, IDS_ToolTip68);
   bSizer132->Add (m_comboBox25,
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

   m_comboBox26 = new wxComboBox (this,
												IDC_BandInterleave,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (220, -1),
												0,
												NULL,
												wxCB_READONLY);
   m_comboBox26->Append (wxT("BIL- Band Interleave by Line"));
   m_comboBox26->Append (wxT("BSQ- Band Sequential"));
   m_comboBox26->Append (wxT("BIS- Band Interleave by Sample"));
   SetUpToolTip (m_comboBox26, IDS_ToolTip69);
   bSizer133->Add (
				m_comboBox26,
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

   this->Layout();

}		// end "CreateControls"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the reformat
//							options dialog box to the user so that the user can
//							selection which reformat function is to be run.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: ??/??/2011
//	Revised By:			Larry L. Biehl			Date: 07/18/2018

Boolean CMChangeFormatDlg::DoDialog(
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr)

{
   Boolean OKFlag = FALSE;

   SInt16 returnCode;


			// Make sure intialization has been completed.

   if (!m_initializedFlag)
																		return (FALSE);

   s_reformatOptionsPtr = reformatOptionsPtr;

   returnCode = ShowModal();

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
														  s_reformatOptionsPtr,
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

/*
//Press keyboard "Enter"  -> wxID_OK
void CMChangeFormatDlg::OnButtonPress(wxKeyEvent& event)
{
	if (event.GetKeyCode() == WXK_RETURN) {
		if(Validate() && TransferDataFromWindow())
			this->EndModal(wxID_OK);
	} else {
		event.Skip();
	}
}
*/

void CMChangeFormatDlg::OnInitDialog (
				wxInitDialogEvent&				event) 
{
	wxComboBox*							comboBoxPtr;
   CMDialog*							dialogPtr = this;

   SInt16								bandInterleaveSelection,
											outputFileSelection,
											dataValueTypeSelection,
											channelSelection,
											headerOptionsSelection;

   Boolean								invertBottomToTopFlag,
											invertLeftToRightFlag,
											swapBytesFlag;

	//if (bSizer124 == NULL)
	//	wxMessageBox("bSizer124 is NULL at beginning OnInitDialag", "Confirm", wxYES_NO);

   wxDialog::OnInitDialog(event);

	//if (bSizer124 == NULL)
	//	wxMessageBox("bSizer124 is NULL after call to wxDialog::OnInitDialog", "Confirm", wxYES_NO);

   ChangeImageFormatDialogInitialize (dialogPtr,
													gImageWindowInfoPtr,
													gImageFileInfoPtr,
													s_reformatOptionsPtr,
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
   
	//if (bSizer124 == NULL)
	//	wxMessageBox("bSizer124 is NULL in OnInitDialog after ChangeImageFormatDialogInitialize", "Confirm", wxYES_NO);
    
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
   
   if (s_reformatOptionsPtr->transformDataCode != kNoTransform)
		m_transformDataFlag = TRUE;

   
   comboBoxPtr = (wxComboBox*) (FindWindow(IDC_DataValueType));
   //comboBoxPtr->SetClientData(0, (void *) &g4BitUnsignedIntegerMenuItem);
   comboBoxPtr->SetClientData(0, (void *) g1ByteSignedIntegerMenuItem);
   comboBoxPtr->SetClientData(1, (void *) g1ByteUnsignedIntegerMenuItem);
   comboBoxPtr->SetClientData(2, (void *) g2ByteSignedIntegerMenuItem);
   comboBoxPtr->SetClientData(3, (void *) g2ByteUnsignedIntegerMenuItem);
   comboBoxPtr->SetClientData(4, (void *) g4ByteSignedIntegerMenuItem);
   comboBoxPtr->SetClientData(5, (void *) g4ByteUnsignedIntegerMenuItem);
   comboBoxPtr->SetClientData(6, (void *) g4ByteRealMenuItem);
   comboBoxPtr->SetClientData(7, (void *) g8ByteRealMenuItem);

   m_dataValueListSelection = GetComboListSelection(IDC_DataValueType,
           m_dataValueTypeSelection);
			
   if (m_dataValueTypeSelection < 0)
    {
        //MShowDialogItem(dialogPtr, IDC_FixedDataType);
	MHideDialogItem ((DialogPtr)dialogPtr, IDC_DataValueType);
    }
    else
    {// m_dataValueTypeSelection >= 0
        //MShowDialogItem(dialogPtr, IDC_DataValueType);
        MHideDialogItem ((DialogPtr)dialogPtr, IDC_FixedDataType);
        
		
    }

   m_localActiveFeaturesPtr = (UInt16*)s_reformatOptionsPtr->channelPtr;

   comboBoxPtr = (wxComboBox*) (FindWindow(IDC_ChangeHeader));
   if (!m_GAIAFormatAllowedFlag)
      comboBoxPtr->Delete(kGAIAMenuItem);

   m_headerListSelection = GetComboListSelection(IDC_ChangeHeader,
           m_headerOptionsSelection);

   TransferDataToWindow();
      
   // Set default text selection to first edit text item

   SelectDialogItemText(this, IDC_LineStart, 0, SInt16_MAX);
	//if (bSizer124 != NULL)
	SetSizerAndFit(bSizer124);
   this->Fit();
	PositionDialogWindow();
	
} // end "OnInitDialog"



void CMChangeFormatDlg::OnOutputInWavelengthOrder (wxCommandEvent& event)

{
   wxCheckBox* cbox = (wxCheckBox*)FindWindow (IDC_OutputInWavelengthOrder);
   m_outputInWavelengthOrderFlag = cbox->GetValue ();

}	// end "OnOutputInWavelengthOrder"



void CMChangeFormatDlg::OnSelendokBandInterleave(wxCommandEvent& event)

{
   SInt16 headerOptionsSelection, outputFileSelection;

   Boolean changedFlag;


   wxComboBox* cmb = (wxComboBox *) FindWindow(IDC_BandInterleave);
   m_bandInterleaveSelection = cmb->GetSelection();

   headerOptionsSelection = m_headerOptionsSelection;
   outputFileSelection = m_outputFileSelection;
   
//   ChangeImageFormatDialogVerifyHeaderSetting(
//           this,
//           gImageFileInfoPtr,
//           m_bandInterleaveSelection,
//           m_dataValueTypeSelection,
//           gPopUpOutputFileMenu,
//           gPopUpHeaderOptionsMenu,
//           IDC_BandInterleave,
//           IDC_SwapBytes,
//           IDC_ChangeHeader,
//           &headerOptionsSelection);
//   
   ChangeImageFormatDialogVerifyHeaderSetting (
										this,
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
   cmb = (wxComboBox*)FindWindow(IDC_ChangeHeader);
   cmb->SetSelection (m_headerListSelection);

   if (changedFlag)
      OnSelendokHeader(event);

} // end "OnSelendokBandInterleave"


void CMChangeFormatDlg::OnSelendokDataValueType (
				wxCommandEvent&					event)  
{
	wxComboBox*							comboBoxPtr;

   SInt16								bandInterleaveSelection,
											dataValueListSelection,
											headerOptionsSelection,
											outputFileSelection;
											

   comboBoxPtr = (wxComboBox*) FindWindow(IDC_DataValueType);
   dataValueListSelection = m_dataValueListSelection;
   m_dataValueListSelection = comboBoxPtr->GetSelection();

   if (dataValueListSelection != m_dataValueListSelection) 
		{
      SInt64 dataValueList64 = (SInt64)((int*)comboBoxPtr->GetClientData (m_dataValueListSelection));
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
											s_reformatOptionsPtr->transformDataCode,
											m_dataValueTypeSelection,
											m_GAIAFormatAllowedFlag);
      
      m_outputFileSelection = outputFileSelection;
      m_bandInterleaveSelection = bandInterleaveSelection;
      m_headerOptionsSelection = headerOptionsSelection;

      comboBoxPtr = (wxComboBox*)FindWindow (IDC_BandInterleave);
      comboBoxPtr->SetSelection (m_bandInterleaveSelection);

      m_headerListSelection = GetComboListSelection (IDC_ChangeHeader,
																		m_headerOptionsSelection);
      comboBoxPtr = (wxComboBox*)FindWindow(IDC_ChangeHeader);
      comboBoxPtr->SetSelection(m_headerListSelection);

		}	// end "if (dataValueTypeSelection != m_dataValueTypeSelection)"

   if (s_reformatOptionsPtr->transformDataCode == kNoTransform)
      m_savedDataValueTypeSelection = m_dataValueTypeSelection;

   m_sessionUserSetDataValueTypeSelectionFlag = TRUE;

}	// end "OnSelendokDataValueType"



void CMChangeFormatDlg::OnSelendokHeader (
				wxCommandEvent&					event)

{
	wxComboBox*							comboBoxPtr;

   //DialogPtr							dialogPtr = this;

   SInt16								bandInterleaveSelection,
											dataValueTypeSelection,
											outputFileSelection = 0;

   Boolean								setChannelDescriptionFlag;

   //Boolean							setFileAddModifyFlag = FALSE,
   //										setFileDeleteModifyFlag = FALSE,
   //										setByteFlag = FALSE,
   //										setSwapByteFlag = FALSE,
   //										setChannelDescriptionFlag = FALSE;


   dataValueTypeSelection = m_dataValueTypeSelection;

	comboBoxPtr = (wxComboBox*)FindWindow (IDC_ChangeHeader);
   m_headerListSelection = comboBoxPtr->GetSelection();
   m_headerOptionsSelection = (SInt64)((int*)(comboBoxPtr->GetClientData (m_headerListSelection)));
	//m_headerOptionsSelection = (SInt16)headerList;
      
   ChangeImageFormatDialogUpdateHeaderOptions (
										  this,
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

      m_dataValueListSelection = GetComboListSelection(IDC_DataValueType,
              m_dataValueTypeSelection);

		}	// end "if (m_dataValueTypeSelection != dataValueTypeSelection)"
	/*
	comboBoxPtr = (CComboBox*)GetDlgItem(IDC_ChangeHeader);
	m_headerOptionsSelection = comboBoxPtr->GetItemData (m_headerListSelection);
										
	if (setFileAddModifyFlag)
		{
		if (((CComboBox*)GetDlgItem(IDC_OutputFileSelection))->GetCount() == 1)
			{
			((CComboBox*)GetDlgItem(IDC_OutputFileSelection))->AddString("Append To");
			((CComboBox*)GetDlgItem(IDC_OutputFileSelection))->AddString("Modify Part");

			}	// end "...->GetCount() == 1"
				
		}	// end "if (setFileAddModifyFlag)"
				
	if (setFileDeleteModifyFlag)
		{
		if ( ((CComboBox*)GetDlgItem(IDC_OutputFileSelection))->GetCount() > 1)
			{
			((CComboBox*)GetDlgItem(IDC_OutputFileSelection))->DeleteString(kModifyPartMenuItem-1);
			((CComboBox*)GetDlgItem(IDC_OutputFileSelection))->DeleteString(kAppendToMenuItem-1);

			}	// end "...->GetCount() > 1"
				
		}	// end "if (setFileDeleteModifyFlag)"
				
	if (setByteFlag)
		{
		m_bytesPerDataValueSelection = abs(m_bytesPerDataValueSelection);
		m_activeBytesPerDataSelectionFlag  = TRUE;

		}	// end "if (setByteFlag)"
				
	if (setSwapByteFlag)
		{
		//SetDLogControl (dialogPtr, IDC_SwapBytes, (s_reformatOptionsPtr->swapBytes));

		if (gImageWindowInfoPtr->numberBytes == 2 )
			SetDLogControlHilite (dialogPtr, IDC_SwapBytes, 0);

		}		// end "if (setSwapByteFlag)"
	*/
   if (setChannelDescriptionFlag)
      SetChannelDescriptionFlag();

   comboBoxPtr = (wxComboBox*) FindWindow(IDC_BandInterleave);
   comboBoxPtr->SetSelection(m_bandInterleaveSelection);

   comboBoxPtr = (wxComboBox*) FindWindow(IDC_OutputFileSelection);
   comboBoxPtr->SetSelection(m_outputFileSelection);

   comboBoxPtr = (wxComboBox*) FindWindow(IDC_DataValueType);
   comboBoxPtr->SetSelection(m_dataValueListSelection);
  
}	// end "OnSelendokHeader"


void CMChangeFormatDlg::OnSelendokOutChannels (wxCommandEvent& event)

{
   m_localActiveNumberFeatures = s_reformatOptionsPtr->numberChannels;

   HandleChannelsMenu (IDC_ChannelCombo,
								kNoTransformation,
								(SInt16) gImageWindowInfoPtr->totalNumberChannels,
								kImageChannelType,
								TRUE);

	s_reformatOptionsPtr->numberChannels = m_localActiveNumberFeatures;

}	// end "OnSelendokOutChannels"


void CMChangeFormatDlg::OnTransformData (wxCommandEvent& event)

{
   DialogPtr				dialogPtr = this;
	wxComboBox*				comboBoxPtr;
	
   SInt16					bandInterleaveSelection,
								headerOptionsSelection,
								numChars,
								outputFileSelection;
								
   UInt16					recommendNumberOfBits,
								suggestedDataValueTypeSelection;

	Boolean					returnFlag = FALSE;


   SetDLogControlHilite(NULL, wxID_OK, 255);
   CMReformatTransformDlg* transformDialogPtr = NULL;
   
   transformDialogPtr = new CMReformatTransformDlg();

   returnFlag = transformDialogPtr->DoDialog(
           &recommendNumberOfBits, m_bandInterleaveSelection);
   
   if (transformDialogPtr != NULL)
      delete transformDialogPtr;

   if (returnFlag)
		{
      SetChannelDescriptionFlag();

      if (s_reformatOptionsPtr->transformDataCode == kNoTransform)
			{
         m_transformDataFlag = FALSE;

         MHideDialogItem(dialogPtr, IDC_MinimumBitsPrompt);

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

		else // s_reformatOptionsPtr->transformDataCode != kNoTransform
			{
         m_transformDataFlag = TRUE;

         MShowDialogItem(dialogPtr, IDC_MinimumBitsPrompt);
			
			if (recommendNumberOfBits > 16)
				{
				numChars = sprintf ((char*)&gTextString2[1],
											"Recommended minimum data bits: %d-Real",
											recommendNumberOfBits);
				suggestedDataValueTypeSelection = k4ByteRealMenuItem;
				
				}
			
			else	// recommendNumberOfBits <= 16
				{
				numChars = sprintf ((char*)&gTextString2[1],
											"Recommended minimum data bits: %d",
											recommendNumberOfBits);
				suggestedDataValueTypeSelection = k2ByteSignedIntegerMenuItem;
				 
				}
			
					// The suggested data value type selection will be forced if the
					// user has not set it before and the suggested setting contains
					// more bits than the previous setting.
					// Also the header format setting will be verified to match the new
					// data value type setting.
									
			if (!m_sessionUserSetDataValueTypeSelectionFlag &&
					!s_reformatOptionsPtr->userSetDataValueTypeSelectionFlag &&
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
													s_reformatOptionsPtr->transformDataCode,
													m_dataValueTypeSelection,
													m_GAIAFormatAllowedFlag);
													
				m_outputFileSelection = outputFileSelection;
				m_bandInterleaveSelection = bandInterleaveSelection;
				m_headerOptionsSelection = headerOptionsSelection;
													
				}	// end "if (!m_sessionUserSetDataValueTypeSelectionFlag && ..."
				
         gTextString2[0] = (UInt8)numChars;
         LoadDItemString(IDC_MinimumBitsPrompt, (char*)&gTextString2[1]);
							
			if (m_outputInWavelengthOrderAllowedFlag)
				{
				SetDLogControl (this,
										IDC_OutputInWavelengthOrder,
										0);
				SetDLogControlHilite (this, IDC_OutputInWavelengthOrder, 255);
				
				}	// end "if (outputInWavelengthOrderAllowedFlag)"
			
			} // end "else ...->transformDataCode != kNoTransform"
		
		m_dataValueListSelection = GetComboListSelection (IDC_DataValueType,
																			m_dataValueTypeSelection);
		comboBoxPtr = (wxComboBox*)FindWindow(IDC_DataValueType);
		comboBoxPtr->SetSelection(m_dataValueListSelection);

				// Input channels will be defined by the transformation.

      if (gReformatOptionsPtr->transformDataCode == kNoTransform ||
              gReformatOptionsPtr->transformDataCode == kAdjustChannel ||
              gReformatOptionsPtr->transformDataCode == kAdjustChannelsByChannel ||
              gReformatOptionsPtr->transformDataCode == kFunctionOfChannels) {
         MShowDialogItem(dialogPtr, IDC_ChannelsLabel);
         MShowDialogItem(dialogPtr, IDC_ChannelCombo);

			}	// end "if (...->transformDataCode == kNoTransform || ...)"

      else // ...->transformDataCode == kTransformChannels ||
         //			transformDataCode == kCreatePCImage)
			{
			MHideDialogItem(dialogPtr, IDC_ChannelsLabel);
			MHideDialogItem(dialogPtr, IDC_ChannelCombo);

			} // end "else ...->transformDataCode == kTransformChannels || ..."

		} // end "if (returnFlag)"

			// Make sure that the transform check box has the appropriate setting.

   wxCheckBox* cbox = (wxCheckBox *) FindWindow(IDC_TransformData);
   cbox->SetValue(m_transformDataFlag);
   
   SetDLogControlHilite	(NULL, wxID_OK, 0);
   
}	// end "OnTransformData"


void CMChangeFormatDlg::OnWriteChanDescriptions(wxCommandEvent& event)

{
   wxCheckBox* cbox = (wxCheckBox*) FindWindow(IDC_WriteChanDescriptions);
   m_writeChanDescriptionFlag = cbox->GetValue();
   m_savedChannelDescriptionFlag = m_writeChanDescriptionFlag;

}	// end "OnWriteChanDescriptions"



void CMChangeFormatDlg::SetChannelDescriptionFlag (void)

{
   DialogPtr dialogPtr = this;
   wxCheckBox* cbox;

   if (s_reformatOptionsPtr->transformDataCode == kNoTransform ||
           s_reformatOptionsPtr->transformDataCode == kAdjustChannel)
		{
      if (m_channelDescriptionAllowedFlag)
			{
         m_writeChanDescriptionFlag = m_savedChannelDescriptionFlag;

         cbox = (wxCheckBox*) FindWindow(IDC_WriteChanDescriptions);
         cbox->SetValue(m_writeChanDescriptionFlag);
         SetDLogControlHilite(dialogPtr, IDC_WriteChanDescriptions, 0);

			}	// end "if (m_channelDescriptionAllowedFlag)"

		}	// end "if (...->transformDataCode == kNoTransform || ..."

   else	// ...->transformDataCode == kTransformChannels || ...
		{
      m_writeChanDescriptionFlag = FALSE;
      cbox = (wxCheckBox*) FindWindow(IDC_WriteChanDescriptions);
      cbox->SetValue(m_writeChanDescriptionFlag);
      SetDLogControlHilite(dialogPtr, IDC_WriteChanDescriptions, 255);

		}	// end "else ...->transformDataCode == kTransformChannels ..."

}	// end "SetChannelDescriptionFlag"



bool CMChangeFormatDlg::TransferDataFromWindow()

{
	SInt16		returnCode;
	
	
   wxComboBox* opfile = (wxComboBox*) FindWindow(IDC_OutputFileSelection);
   wxComboBox* datatype = (wxComboBox*) FindWindow(IDC_DataValueType);
   wxComboBox* bandf = (wxComboBox*) FindWindow(IDC_BandInterleave);
   wxComboBox* chan = (wxComboBox*) FindWindow(IDC_ChannelCombo);
   wxComboBox* header = (wxComboBox*) FindWindow(IDC_ChangeHeader);

   wxCheckBox* tdata = (wxCheckBox*) FindWindow(IDC_TransformData);
   wxCheckBox* ibt = (wxCheckBox*) FindWindow(IDC_InvertBottomToTop);
   wxCheckBox* irl = (wxCheckBox*) FindWindow(IDC_InvertRightToLeft);
   wxCheckBox* swapb = (wxCheckBox*) FindWindow(IDC_SwapBytes);
   wxCheckBox* writechan = (wxCheckBox*)FindWindow (IDC_WriteChanDescriptions);
   wxCheckBox* wavelengthOrder = (wxCheckBox*)FindWindow (IDC_OutputInWavelengthOrder);
	
   wxTextCtrl* c_end = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
   wxTextCtrl* c_inter = (wxTextCtrl*) FindWindow(IDC_ColumnInterval);
   wxTextCtrl* c_start = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
   wxTextCtrl* l_end = (wxTextCtrl*) FindWindow(IDC_LineEnd);
   wxTextCtrl* l_inter = (wxTextCtrl*) FindWindow(IDC_LineInterval);
   wxTextCtrl* l_start = (wxTextCtrl*) FindWindow(IDC_LineStart);

   m_swapBytesFlag = swapb->GetValue();
   m_transformDataFlag = tdata->GetValue();
   m_invertBottomToTopFlag = ibt->GetValue();
   m_writeChanDescriptionFlag = writechan->GetValue ();
	m_outputInWavelengthOrderFlag = wavelengthOrder->GetValue ();
   m_invertLeftToRightFlag = irl->GetValue();

   m_channelSelection = chan->GetSelection();
   m_headerListSelection = header->GetSelection();
   m_dataValueListSelection = datatype->GetSelection();
   m_outputFileSelection = opfile->GetSelection();
   m_bandInterleaveSelection = bandf->GetSelection();

   m_LineStartString = l_start->GetValue();
   m_LineEndString = l_end->GetValue();
   m_LineIntervalString = l_inter->GetValue();
   m_ColumnStartString = c_start->GetValue();
   m_ColumnEndString = c_end->GetValue();
   m_ColumnIntervalString = c_inter->GetValue();
   m_LineStart = wxAtoi(m_LineStartString);
   m_LineEnd = wxAtoi(m_LineEndString);
   m_LineInterval = wxAtoi(m_LineIntervalString);
   m_ColumnStart = wxAtoi(m_ColumnStartString);
   m_ColumnEnd = wxAtoi(m_ColumnEndString);
   m_ColumnInterval = wxAtoi(m_ColumnIntervalString);

   if(m_channelSelection < 0)
      m_channelSelection = m_channelSelection_Saved;
   
	returnCode = VerifyLineColumnValues(
								IDC_LineStart,
								IDC_ColumnStart,
								true);

   return (returnCode == 0);
	
}	// end "TransferDataFromWindow"



bool CMChangeFormatDlg::TransferDataToWindow()

{

	//wxString tempString;
	
   //wxStaticText* inputBandInterleave = (wxStaticText*) FindWindow(IDC_ChangeFormatInInterleave);
   //wxStaticText* inputDataValueType = (wxStaticText*) FindWindow(IDC_ChangeFormatInBits);
	
   wxComboBox* opfile = (wxComboBox*)FindWindow(IDC_OutputFileSelection);
   wxComboBox* datatype = (wxComboBox*)FindWindow(IDC_DataValueType);
   wxComboBox* bandf = (wxComboBox*)FindWindow(IDC_BandInterleave);
   wxComboBox* chan = (wxComboBox*)FindWindow(IDC_ChannelCombo);
   wxComboBox* header = (wxComboBox*)FindWindow(IDC_ChangeHeader);

   wxCheckBox* tdata = (wxCheckBox*)FindWindow(IDC_TransformData);
   wxCheckBox* ibt = (wxCheckBox*)FindWindow(IDC_InvertBottomToTop);
   wxCheckBox* irl = (wxCheckBox*)FindWindow(IDC_InvertRightToLeft);
   wxCheckBox* swapb = (wxCheckBox*)FindWindow(IDC_SwapBytes);
   wxCheckBox* writechan = (wxCheckBox*)FindWindow (IDC_WriteChanDescriptions);
   wxCheckBox* wavelengthOrder = (wxCheckBox*)FindWindow (IDC_OutputInWavelengthOrder);

   wxTextCtrl* c_end = (wxTextCtrl*)FindWindow(IDC_ColumnEnd);
   wxTextCtrl* c_inter = (wxTextCtrl*)FindWindow(IDC_ColumnInterval);
   wxTextCtrl* c_start = (wxTextCtrl*)FindWindow(IDC_ColumnStart);
   wxTextCtrl* l_end = (wxTextCtrl*)FindWindow(IDC_LineEnd);
   wxTextCtrl* l_inter = (wxTextCtrl*)FindWindow(IDC_LineInterval);
   wxTextCtrl* l_start = (wxTextCtrl*)FindWindow(IDC_LineStart);
	/*
	tempString = wxString::FromAscii((char*)&m_inputBandInterleaveString[1]);
	inputBandInterleave->SetLabel(tempString);
	
	tempString = wxString::FromAscii((char*)&m_inputDataValueTypeString[1]);
	inputDataValueType->SetLabel(tempString);
*/
   tdata->SetValue(m_transformDataFlag);
   ibt->SetValue(m_invertBottomToTopFlag);
   irl->SetValue(m_invertLeftToRightFlag);
   swapb->SetValue(m_swapBytesFlag);
   writechan->SetValue(m_writeChanDescriptionFlag);
	wavelengthOrder->SetValue (m_outputInWavelengthOrderFlag);

   opfile->SetSelection(m_outputFileSelection);
   datatype->SetSelection(m_dataValueListSelection);
   bandf->SetSelection(m_bandInterleaveSelection);
   chan->SetSelection(m_channelSelection);
   header->SetSelection(m_headerListSelection);

    c_end->ChangeValue(wxString::Format(wxT("%i"), m_ColumnEnd));
    c_inter->ChangeValue(wxString::Format(wxT("%i"), m_ColumnInterval));
    c_start->ChangeValue(wxString::Format(wxT("%i"), m_ColumnStart));
    l_end->ChangeValue(wxString::Format(wxT("%i"), m_LineEnd));
    l_inter->ChangeValue(wxString::Format(wxT("%i"), m_LineInterval));
    l_start->ChangeValue(wxString::Format(wxT("%i"), m_LineStart));
    
//   c_end->Clear();
//   c_inter->Clear();
//   c_start->Clear();
//   l_end->Clear();
//   l_inter->Clear();
//   l_start->Clear();
//
//   *c_end << m_ColumnEnd;
//   *c_inter << m_ColumnInterval;
//   *c_start << m_ColumnStart;
//   *l_end << m_LineEnd;
//   *l_inter << m_LineInterval;
//   *l_start << m_LineStart;

 //  *c_end << m_ColumnEnd;
 //  *c_inter << m_ColumnInterval;
 //  *c_start << m_ColumnStart;
 //  *l_end << m_LineEnd;
 //  *l_inter << m_LineInterval;
 //  *l_start << m_LineStart;

   return true;
	
}	// end "TransferDataToWindow"

