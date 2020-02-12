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
//   File:                 xReformatRectifyDialog.cpp : class implementation file
//   Class Definition:     xReformatRectifyDialog.h
//
//   Authors:              Abdur Rahman Maud, Larry L. Biehl
//
//   Revision date:        01/03/2020
//
//   Language:					C++
//
//   System:               Linux and MacOS Operating Systems
//
//   Brief description:  	This file contains functions that relate to the
//                       	CMReformatRectifyDlg class.
//
/*
  Template for debug code.
 	int numberChars = sprintf ((char*)&gTextString3,
 			" LReformatRectifyDlg: (): %s",
 			gEndOfLine);
 ListString ((char*)&gTextString3, numberChars, gOutputTextH);
 */
//------------------------------------------------------------------------------------

#include   "SMultiSpec.h"
#include "xReformatRectifyDialog.h"
#include "wx/gbsizer.h"
#include "xChannelsDialog.h"
#include "xImageView.h"
#include "SImageWindow_class.h"
#include "xDialog_images.cpp"



BEGIN_EVENT_TABLE (CMReformatRectifyDlg, CMDialog)
	EVT_BUTTON (IDEntireImage, CMReformatRectifyDlg::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMReformatRectifyDlg::ToSelectedImage)

	EVT_CHECKBOX (IDC_UseMapOrientationAngle, CMReformatRectifyDlg::OnBnClickedUseMapOrientationAngle)

	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_Channels, CMReformatRectifyDlg::OnSelendokOutChannels)
	#endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_Channels, CMReformatRectifyDlg::OnSelendokOutChannels)
	#endif

	EVT_CHOICE (IDC_ReferenceFileList, CMReformatRectifyDlg::OnCbnSelendokTargetCombo)
	EVT_CHOICE (IDC_ResampleMethod, CMReformatRectifyDlg::OnCbnSelendokResampleMethod)

	EVT_COMBOBOX_DROPDOWN (IDC_Channels, CMReformatRectifyDlg::OnChannelComboDropDown)

	EVT_INIT_DIALOG (CMReformatRectifyDlg::OnInitDialog)

	EVT_RADIOBUTTON (IDC_ReprojectToRadio, CMReformatRectifyDlg::OnBnClickedReprojectToRadio)
	EVT_RADIOBUTTON (IDC_TranslateScaleRotateRadio, CMReformatRectifyDlg::OnBnClickedTranslateScaleRotateRadio)

	EVT_TEXT (IDC_ColumnStart, CMReformatRectifyDlg::CheckColumnStart)
	EVT_TEXT (IDC_ColumnEnd, CMReformatRectifyDlg::CheckColumnEnd)
	EVT_TEXT (IDC_LineStart, CMReformatRectifyDlg::CheckLineStart)
	EVT_TEXT (IDC_LineEnd, CMReformatRectifyDlg::CheckLineEnd)
	EVT_TEXT (IDC_RotationClockwise, CMReformatRectifyDlg::OnEnChangeRotationClockwise)
END_EVENT_TABLE ()



CMReformatRectifyDlg::CMReformatRectifyDlg (
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title/*=NULL*/)
		: CMDialog (CMReformatRectifyDlg::IDD, pParent, title)

{
   m_kNoneMenuItem = kNoneMenuItem;
   m_kArcViewMenuItem = kArcViewMenuItem;
   m_kERDAS74MenuItem = kERDAS74MenuItem;
   m_kGAIAMenuItem = kGAIAMenuItem;
   m_kTIFFGeoTIFFMenuItem = kTIFFGeoTIFFMenuItem;
   m_kMatlabMenuItem = kMatlabMenuItem;
   
   m_backgroundValue = 0;
   m_blankOutsideSelectedAreaFlag = FALSE;
   m_channelSelection = -1;
   m_columnScaleFactor = 0.0;
   m_columnShift = 0;
   m_fileNamesSelection = -1;
   m_headerListSelection = -1;
   m_headerOptionsSelection = -1;
   m_lineScaleFactor = 0.0;
   m_lineShift = 0;
   m_procedureCode = 0;
   m_resampleSelection = 0;
   m_rotationAngle = 0.0;
   m_useMapOrientationAngleFlag = FALSE;
   
   m_referenceWindowInfoHandle = NULL;
   m_initializedFlag = CMDialog::m_initializedFlag;
   
   CreateControls ();
   SetSizerAndFit (bSizer119);
	
}	// end "CMReformatRectifyDlg"



CMReformatRectifyDlg::~CMReformatRectifyDlg ()

{
   
}	// end "~CMReformatRectifyDlg"



void CMReformatRectifyDlg::CreateControls ()

{
   SetSizeHints (wxDefaultSize, wxDefaultSize);
   
   wxBitmap entireimi = wxBITMAP_PNG_FROM_DATA (entireim);
   wxBitmap toentirei = wxBITMAP_PNG_FROM_DATA (toentire);
   wxBitmap selectedi = wxBITMAP_PNG_FROM_DATA (selected);
   wxBitmap bmp4i = wxBITMAP_PNG_FROM_DATA (bmp4);
   
   bSizer119 = new wxBoxSizer (wxVERTICAL);
   
   wxFlexGridSizer* gSizer16;
   gSizer16 = new wxFlexGridSizer (0, 2, 0, 0);
   
   wxBoxSizer* bSizer195;
   bSizer195 = new wxBoxSizer (wxVERTICAL);
   
   wxBoxSizer* bSizer198;
   bSizer198 = new wxBoxSizer (wxVERTICAL);
   
   wxStaticBoxSizer* inputImageSettingsSizer;
   wxStaticBox* inputImageSettingsBox = new wxStaticBox (this,
   																		IDC_LineColFrame,
   																		wxT("Input Image Settings"));
   inputImageSettingsSizer = new wxStaticBoxSizer (
																inputImageSettingsBox, wxHORIZONTAL);
	
   wxBoxSizer* bSizer791;
   bSizer791 = new wxBoxSizer (wxHORIZONTAL);
   
   m_bpButton4 = new wxBitmapButton (inputImageSettingsBox,
   												IDEntireImage,
   												entireimi,
   												wxDefaultPosition,
   												wxDefaultSize,
   												wxBU_AUTODRAW);
   m_bpButton4->SetBitmapDisabled (toentirei);
  	bSizer791->Add (m_bpButton4,
                   wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   m_bpButton5 = new wxBitmapButton (inputImageSettingsBox,
   											IDSelectedImage,
   											selectedi,
   											wxDefaultPosition,
   											wxDefaultSize,
   											wxBU_AUTODRAW);
   m_bpButton5->SetBitmapDisabled (bmp4i);
   bSizer791->Add (m_bpButton5,
                   wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   inputImageSettingsSizer->Add (bSizer791, 0, wxALL|wxEXPAND, 5);
   m_bpButton4->Hide ();
   
   wxGridSizer* gSizer1;
   gSizer1 = new wxGridSizer (3, 3, 0, 0);
   
   wxStaticText* staticText63 = new wxStaticText (inputImageSettingsBox,
   																wxID_ANY,
   																wxEmptyString,
   																wxDefaultPosition,
   																wxDefaultSize,
   																0);
   staticText63->Wrap (-1);
   gSizer1->Add (staticText63, 0, wxALL, 5);
   
   wxStaticText* staticText64 = new wxStaticText (inputImageSettingsBox,
   																wxID_ANY,
   																wxT("Start"),
   																wxDefaultPosition,
   																wxSize (70, -1),
   																0);
   staticText64->Wrap (-1);
   gSizer1->Add (staticText64, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxStaticText* staticText65 = new wxStaticText (inputImageSettingsBox,
   																wxID_ANY,
   																wxT("End"),
   																wxDefaultPosition,
   																wxSize (70, -1),
   																0);
   staticText65->Wrap (-1);
   gSizer1->Add (staticText65, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxStaticText* staticText60 = new wxStaticText (inputImageSettingsBox,
   																wxID_ANY,
   																wxT("Lines"),
   																wxDefaultPosition,
   																wxSize (70, -1),
   																0);
   staticText60->Wrap (-1);
   gSizer1->Add (staticText60, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxTextCtrl* linestartctrl = new wxTextCtrl (inputImageSettingsBox,
   															IDC_LineStart,
   															wxEmptyString,
   															wxDefaultPosition,
   															wxSize (70, -1),
   															0);
   linestartctrl->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_LineStartString));
   gSizer1->Add (linestartctrl, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxTextCtrl* lineendctrl = new wxTextCtrl (inputImageSettingsBox,
   														IDC_LineEnd,
   														wxEmptyString,
   														wxDefaultPosition,
   														wxSize (70, -1),
   														0);
   lineendctrl->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_LineEndString));
   gSizer1->Add (lineendctrl, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxStaticText* staticText62 = new wxStaticText (inputImageSettingsBox,
   																wxID_ANY,
   																wxT("Columns"),
   																wxDefaultPosition,
   																wxSize (70, -1),
   																0);
   staticText62->Wrap (-1);
   gSizer1->Add (staticText62, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxTextCtrl* colstartctrl = new wxTextCtrl (inputImageSettingsBox,
   															IDC_ColumnStart,
   															wxEmptyString,
   															wxDefaultPosition,
   															wxSize (70, -1),
   															0);
   colstartctrl->SetValidator (
   								wxTextValidator (wxFILTER_DIGITS, &m_ColumnStartString));
   gSizer1->Add (colstartctrl, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxTextCtrl* colendctrl = new wxTextCtrl (inputImageSettingsBox,
   														IDC_ColumnEnd,
   														wxEmptyString,
   														wxDefaultPosition,
   														wxSize (70, -1),
   														0);
   colendctrl->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_ColumnEndString));
   
   SetUpToolTip (m_bpButton4, IDS_ToolTip40);
   SetUpToolTip (linestartctrl, IDS_ToolTip19);
   SetUpToolTip (lineendctrl, IDS_ToolTip20);
   SetUpToolTip (colstartctrl, IDS_ToolTip22);
   SetUpToolTip (colendctrl, IDS_ToolTip23);
   
   gSizer1->Add (colendctrl, 0, wxALIGN_CENTER | wxALL, 5);
   
   inputImageSettingsSizer->Add (gSizer1, 1, wxEXPAND|wxALL, 5);
   
   bSizer198->Add (inputImageSettingsSizer, 1, wxEXPAND | wxBOTTOM|wxRIGHT, 6);
   
   bSizer195->Add (bSizer198, 1, wxEXPAND);
   
   wxBoxSizer* bSizer200;
   bSizer200 = new wxBoxSizer (wxVERTICAL);
   
   wxStaticBoxSizer* outputOptionsSizer;
   wxStaticBox* outputOptionsBox = new wxStaticBox (this,
   																	wxID_ANY,
   																	wxT("Output Image Options"));
   outputOptionsSizer = new wxStaticBoxSizer (outputOptionsBox, wxVERTICAL);
   
   wxBoxSizer* bSizer278;
   bSizer278 = new wxBoxSizer (wxHORIZONTAL);
   
   m_staticText402 = new wxStaticText (outputOptionsBox,
													wxID_ANY,
													wxT("Header:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText402->Wrap (-1);
   SetUpToolTip (m_staticText402, IDS_ToolTip203);
   bSizer278->Add (m_staticText402, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
   
   m_fileFormatCtrl = new wxChoice (outputOptionsBox,
   											IDC_Header,
   											wxDefaultPosition,
   											wxDefaultSize);
   m_fileFormatCtrl->Append ("'None'");
   m_fileFormatCtrl->Append ("ArcView format");
   m_fileFormatCtrl->Append ("Erdas 74 format");
   SetUpToolTip (m_fileFormatCtrl, IDS_ToolTip203);
   bSizer278->Add (m_fileFormatCtrl, 0, wxALL, 5);
   
   outputOptionsSizer->Add (bSizer278, 0, wxALL|wxEXPAND, 5);
   
   wxBoxSizer* bSizer279;
   bSizer279 = new wxBoxSizer (wxHORIZONTAL);
   
   m_checkBox20 = new wxCheckBox (outputOptionsBox,
   											IDC_NonSelectedPixels,
   											wxT("Set Nonselected Pixels to Background Value"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_checkBox20, IDS_ToolTip204);
   bSizer279->Add (m_checkBox20, 0, wxALL, 5);
   
   outputOptionsSizer->Add (bSizer279, 0, wxEXPAND);
   
   wxBoxSizer* bSizer280;
   bSizer280 = new wxBoxSizer (wxHORIZONTAL);
   
   m_staticText403 = new wxStaticText (outputOptionsBox,
   												wxID_ANY,
   												wxT("Background value:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText403->Wrap (-1);
   SetUpToolTip (m_staticText403, IDS_ToolTip205);
   bSizer280->Add (m_staticText403, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
   
   m_textCtrl154 = new wxTextCtrl (outputOptionsBox,
												IDC_BackgroundValue,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_textCtrl154, IDS_ToolTip205);
   m_textCtrl154->SetValidator (
   							wxTextValidator (wxFILTER_NUMERIC, &m_backgroundValueString));
   bSizer280->Add (m_textCtrl154, 0, wxALL, 5);
   
   outputOptionsSizer->Add (bSizer280, 0, wxEXPAND);
   
   wxBoxSizer* bSizer281;
   bSizer281 = new wxBoxSizer (wxHORIZONTAL);
   
   m_staticText404 = new wxStaticText (outputOptionsBox,
   												IDC_ChannelsPrompt,
   												wxT("Channels:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText404->Wrap (-1);
   SetUpToolTip (m_staticText404, IDS_ToolTip206);
   bSizer281->Add (m_staticText404, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	GetAllSubsetMenuControl (kChannelsMenu,
										outputOptionsBox,
										IDC_Channels,
										120,
										IDS_ToolTip206);
	bSizer281->Add (m_channelsCtrl, 0, wxALL, 5);
   
   outputOptionsSizer->Add (bSizer281, 0, wxEXPAND);
   
   bSizer200->Add (outputOptionsSizer, 1, wxEXPAND | wxTOP|wxRIGHT, 6);
   
   bSizer195->Add (bSizer200, 1, wxEXPAND);
	
   gSizer16->Add (bSizer195, 1, wxEXPAND);
   
   wxBoxSizer* bSizer197;
   bSizer197 = new wxBoxSizer (wxVERTICAL);
   
   wxStaticBoxSizer* rectifySettingsSizer;
   wxStaticBox* rectifySettingsBox = new wxStaticBox (this,
   																	wxID_ANY,
   																	wxT("Rectification Settings"));
   rectifySettingsSizer = new wxStaticBoxSizer (rectifySettingsBox, wxVERTICAL);
   
   wxTextValidator negativeIntegerValidator = wxTextValidator (
   													wxFILTER_ASCII|wxFILTER_INCLUDE_CHAR_LIST);
   wxString valid_str = "'-','0','1','2','3','4','5','6','7','8','9'";
   negativeIntegerValidator.SetCharIncludes (valid_str);
   
   m_radioBtn18 = new wxRadioButton (rectifySettingsBox,
   												IDC_TranslateScaleRotateRadio,
   												wxT("Translate, Scale and/or Rotate"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   rectifySettingsSizer->Add (m_radioBtn18, 0, wxALL, 5);
   
   wxBoxSizer* bSizer247;
   bSizer247 = new wxBoxSizer (wxHORIZONTAL);
   
   m_staticText361 = new wxStaticText (rectifySettingsBox,
   												IDC_TranslateScaleTitle,
   												wxT("Linear"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText361->Wrap (-1);
   bSizer247->Add (m_staticText361, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5);
   
   
   bSizer247->Add (40, 0, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND);
   
   m_staticText362 = new wxStaticText (rectifySettingsBox,
   												IDC_TranslateScaleTitle2,
   												wxT("Offset"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText362->Wrap (-1);
   bSizer247->Add (m_staticText362, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5);
	
   bSizer247->Add (60, 0, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND);
   
   m_staticText363 = new wxStaticText (rectifySettingsBox,
													IDC_TranslateScaleTitle3,
													wxT("Scale"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText363->Wrap (-1);
   bSizer247->Add (m_staticText363, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5);
   
   
   rectifySettingsSizer->Add (
								bSizer247, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND);
   
   wxBoxSizer* bSizer262;
   bSizer262 = new wxBoxSizer (wxHORIZONTAL);
   
   m_staticText364 = new wxStaticText (rectifySettingsBox,
   												IDC_LineTranslateScalePrompt,
   												wxT("Lines:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText364->Wrap (-1);
   bSizer262->Add (
   				m_staticText364,
					0,
					wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL|wxALL, 5);
   
   bSizer262->Add (20, 0, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND);
   
   m_textCtrl147 = new wxTextCtrl (rectifySettingsBox,
   											IDC_LineOffset,
   											wxEmptyString,
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_textCtrl147, IDS_ToolTip208);
   m_textCtrl147->SetValidator (negativeIntegerValidator);
   
   bSizer262->Add (m_textCtrl147, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5);
   
   m_textCtrl144 = new wxTextCtrl (rectifySettingsBox,
   											IDC_LineScale,
   											wxEmptyString,
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_textCtrl144, IDS_ToolTip210);
   m_textCtrl144->SetValidator (
   								wxTextValidator (wxFILTER_NUMERIC, &m_lineScaleString));
   bSizer262->Add (m_textCtrl144, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5);
	
   rectifySettingsSizer->Add (
   							bSizer262, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND);
   
   wxBoxSizer* bSizer263;
   bSizer263 = new wxBoxSizer (wxHORIZONTAL);
   
   m_staticText366 = new wxStaticText (rectifySettingsBox,
   												IDC_ColumnTranslateScalePrompt,
   												wxT("Columns:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText366->Wrap (-1);
   bSizer263->Add (
   				m_staticText366,
					0,
					wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL|wxALL, 5);
   
   m_textCtrl145 = new wxTextCtrl (rectifySettingsBox,
   											IDC_ColumnOffset,
   											wxEmptyString,
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_textCtrl145, IDS_ToolTip209);
   m_textCtrl145->SetValidator (negativeIntegerValidator);
   bSizer263->Add (m_textCtrl145, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5);
   
   m_textCtrl146 = new wxTextCtrl (rectifySettingsBox,
   											IDC_ColumnScale,
   											wxEmptyString,
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_textCtrl146, IDS_ToolTip211);
   m_textCtrl146->SetValidator (
   						wxTextValidator (wxFILTER_NUMERIC, &m_columnScaleString));
   bSizer263->Add (m_textCtrl146, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5);
	
   rectifySettingsSizer->Add (bSizer263,
   									0,
   									wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND);
   
   wxBoxSizer* bSizer266;
   bSizer266 = new wxBoxSizer (wxHORIZONTAL);
   
   m_staticText378 = new wxStaticText (rectifySettingsBox,
													IDC_RotationTitle,
													wxT("Rotation  (clockwise)"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText378->Wrap (-1);
   bSizer266->Add (m_staticText378, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5);
	
   rectifySettingsSizer->Add (
   							bSizer266, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND);
   
   wxBoxSizer* bSizer268;
   bSizer268 = new wxBoxSizer (wxHORIZONTAL);
   
   m_staticText380 = new wxStaticText (rectifySettingsBox,
   												IDC_RotationClockwisePrompt,
   												wxT("Degrees:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText380->Wrap (-1);
   SetUpToolTip (m_staticText380, IDS_ToolTip207);
   bSizer268->Add (
   				m_staticText380,
					0,
					wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL|wxALL, 5);
   
   m_textCtrl149 = new wxTextCtrl (rectifySettingsBox,
												IDC_RotationClockwise,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_textCtrl149, IDS_ToolTip207);
   m_textCtrl149->SetValidator (
   							wxTextValidator (wxFILTER_NUMERIC, &m_rotationAngleString));
   bSizer268->Add (m_textCtrl149, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5);
   
   rectifySettingsSizer->Add (
   							bSizer268, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND);
   
   wxBoxSizer* bSizer269;
   bSizer269 = new wxBoxSizer (wxHORIZONTAL);
   
   m_checkBox18 = new wxCheckBox (rectifySettingsBox,
   											IDC_UseMapOrientationAngle,
   											wxT("Use image orientation angle"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   bSizer269->Add (m_checkBox18, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5);
	
   rectifySettingsSizer->Add (
								bSizer269, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND);
   
   wxBoxSizer* bSizer270;
   bSizer270 = new wxBoxSizer (wxHORIZONTAL);
   
   m_radioBtn19 = new wxRadioButton (rectifySettingsBox,
   												IDC_ReprojectToRadio,
   												wxT("Reproject"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   SetUpToolTip (m_radioBtn19, IDS_ToolTip214);
   bSizer270->Add (m_radioBtn19, 0, wxALL, 5);
   
   rectifySettingsSizer->Add (bSizer270, 0, wxEXPAND);
   
   wxBoxSizer* bSizer271;
   bSizer271 = new wxBoxSizer (wxHORIZONTAL);
   
   
   bSizer271->Add (20, 0, 0, wxEXPAND);
   
   m_staticText381 = new wxStaticText (rectifySettingsBox,
   												IDC_ReferenceFileListPrompt,
   												wxT("Reference:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText381->Wrap (-1);
   SetUpToolTip (m_staticText381, IDS_ToolTip212);
   bSizer271->Add (m_staticText381, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
   m_referenceListCtrl = new wxChoice (rectifySettingsBox,
													IDC_ReferenceFileList,
													wxDefaultPosition,
													wxSize (300, -1));
   SetUpToolTip (m_referenceListCtrl, IDS_ToolTip212);
   m_referenceListCtrl->SetClientSize (wxSize (260, -1));
   
   bSizer271->Add (m_referenceListCtrl, 0, wxALL, 5);
   
   rectifySettingsSizer->Add (bSizer271, 0, wxEXPAND);
   
   wxBoxSizer* bSizer272;
   bSizer272 = new wxBoxSizer (wxHORIZONTAL);
   
   bSizer272->Add (20, 0, 0, wxEXPAND);
   
   m_staticText382 = new wxStaticText (rectifySettingsBox,
   												IDC_ResampleMethodPrompt,
   												wxT("Algorithm:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText382->Wrap (-1);
   SetUpToolTip (m_staticText382, IDS_ToolTip213);
   bSizer272->Add (m_staticText382, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
   m_algorithmCtrl = new wxChoice (rectifySettingsBox,
   											IDC_ResampleMethod,
   											wxDefaultPosition,
   											wxDefaultSize);
   SetUpToolTip (m_algorithmCtrl, IDS_ToolTip213);
   m_algorithmCtrl->SetMinSize (wxSize (150, -1));
   bSizer272->Add (m_algorithmCtrl, 0, wxALL, 5);
   
   rectifySettingsSizer->Add (bSizer272, 0, wxEXPAND);
	
   bSizer197->Add (rectifySettingsSizer, 1, wxEXPAND | wxLEFT, 6);
   
   gSizer16->Add (bSizer197, 1, wxEXPAND, 5);
	
   bSizer119->Add (gSizer16, 1, wxEXPAND|wxLEFT|wxTOP|wxRIGHT, 12);

	CreateStandardButtons (bSizer119);
   
   Layout ();
   
   		// This forces the positions to be used when the toEntire and toSelected
   		// buttons are switched.
	
   m_bpButton4->Show ();
   m_bpButton5->Hide ();
   Layout ();
   
} 	// end "CreateControls"



Boolean CMReformatRectifyDlg::DoDialog (
				FileInfoPtr                   outFileInfoPtr,
				FileInfoPtr                   fileInfoPtr,
				WindowInfoPtr                 imageWindowInfoPtr,
				LayerInfoPtr                  imageLayerInfoPtr,
				ReformatOptionsPtr            reformatOptionsPtr,
				double                        minBackgroundValue,
				double                        maxBackgroundValue)

{
   SInt16                			returnCode;
   Boolean                  		continueFlag = FALSE;
	
   
   		// Make sure intialization has been complete
   
   if (!m_initializedFlag)
      																					return (FALSE);
   
   m_outputFileInfoPtr = outFileInfoPtr;
   m_fileInfoPtr = fileInfoPtr;
   m_imageWindowInfoPtr = imageWindowInfoPtr;
   m_imageLayerInfoPtr = imageLayerInfoPtr;
   m_reformatOptionsPtr = reformatOptionsPtr;
   m_rectifyImageOptionsPtr = reformatOptionsPtr->rectifyImageOptionsPtr;
   m_minBackgroundValue = minBackgroundValue;
   m_maxBackgroundValue = maxBackgroundValue;
   
   		// Selected area for output file.
	
   m_dialogSelectArea.lineStart = m_LineStart;
   m_dialogSelectArea.lineEnd = m_LineEnd;
   m_dialogSelectArea.lineInterval = m_LineInterval;
   
   m_dialogSelectArea.columnStart = m_ColumnStart;
   m_dialogSelectArea.columnEnd = m_ColumnEnd;
   m_dialogSelectArea.columnInterval = m_ColumnInterval;
	
   returnCode = ShowModal ();
   
   if (returnCode == wxID_OK)
   	{
      continueFlag = TRUE;
      
      m_dialogSelectArea.lineStart = m_LineStart;
      m_dialogSelectArea.lineEnd = m_LineEnd;
      m_dialogSelectArea.lineInterval = 1;
      m_dialogSelectArea.columnStart = m_ColumnStart;
      m_dialogSelectArea.columnEnd = m_ColumnEnd;
      m_dialogSelectArea.columnInterval = 1;
      
      RectifyImageDialogOK (this,
                            m_outputFileInfoPtr,
                            m_fileInfoPtr,
                            m_imageWindowInfoPtr,
                            m_imageLayerInfoPtr,
                            &m_dialogSelectArea,
                            m_reformatOptionsPtr,
                            (SInt16)m_headerOptionsSelection,
                            (Boolean)m_blankOutsideSelectedAreaFlag,
                            (SInt16)m_channelSelection,
                            m_backgroundValue,
                            (SInt16)(m_procedureCode+1),
                            (SInt16)m_resampleMethodCode,
                            m_referenceWindowInfoHandle,
                            m_lineShift,
                            m_columnShift,
                            m_lineScaleFactor,
                            m_columnScaleFactor,
                            m_rotationAngle);
      
   	} 	// end "if (returnCode == IDOK)"
   
   return (continueFlag);
   
}	// end "DoDialog"



void CMReformatRectifyDlg::OnBnClickedUseMapOrientationAngle (
				wxCommandEvent& 					event)

{
	SInt16         					numberDecimalPlaces;
	
   
   wxCheckBox* mapOrientationAngle =
   									(wxCheckBox*)FindWindow (IDC_UseMapOrientationAngle);
   m_useMapOrientationAngleFlag = mapOrientationAngle->GetValue ();
   
   m_rotationAngle = 0.;
   numberDecimalPlaces = 1;
   if (m_useMapOrientationAngleFlag)
   	{
      m_rotationAngle = m_mapOrientationAngle;
      numberDecimalPlaces = 6;
      
   	}	// end "if (m_useMapOrientationAngleFlag)"
   
   wxTextCtrl* rotationAngle = (wxTextCtrl*)FindWindow (IDC_RotationClockwise);
   rotationAngle->ChangeValue (
   				wxString::Format (wxT("%.*f"), numberDecimalPlaces, m_rotationAngle));
   
} 	// end "OnBnClickedUseMapOrientationAngle"



void CMReformatRectifyDlg::OnBnClickedTranslateScaleRotateRadio (
				wxCommandEvent& 					event)

{
   m_procedureCode = kTranslateScaleRotate - 1;
   UpdateProcedureItems (IDC_LineOffset, m_blankOutsideSelectedAreaFlag);
   
}	// end "OnBnClickedTranslateScaleRotateRadio"



void CMReformatRectifyDlg::OnBnClickedReprojectToRadio (
				wxCommandEvent& 					event)

{
   m_procedureCode = kReprojectToReferenceImage - 1;
   UpdateProcedureItems (IDC_LineStart, TRUE);
   
}	// end "OnBnClickedReprojectToRadio"



void CMReformatRectifyDlg::OnCbnSelendokTargetCombo (
				wxCommandEvent& 					event)

{
	SInt16   							savedFileNamesSelection;
   
   
   		// Popup box to select the reference image to register against
   
   if (m_fileNamesSelection >= 0)
   	{
      savedFileNamesSelection = m_fileNamesSelection;
      
      m_fileNamesSelection = m_referenceListCtrl->GetSelection ();
      
      if (savedFileNamesSelection != m_fileNamesSelection)
         RectifyImageDialogOnReferenceFile (this,
                                            m_procedureCode+1,
                                            m_fileNamesSelection+1,
                                            &m_referenceWindowInfoHandle,
                                            &m_dialogSelectArea);
      
   	}	// end "if (m_fileNamesSelection >= 0)"
   
} 	// end "OnCbnSelendokTargetCombo"



void CMReformatRectifyDlg::OnCbnSelendokResampleMethod (
				wxCommandEvent& 					event)

{
   		// Select resampling method popup box
   
   if (m_resampleSelection >= 0)
   	{
      m_resampleSelection = m_algorithmCtrl->GetSelection ();
      
      SInt64 windowIndex64 =
					(SInt64)((int*)m_algorithmCtrl->GetClientData (m_resampleSelection));
      m_resampleMethodCode = (UInt32)windowIndex64;
      
   	}	// end "if (m_resampleSelection >= 0)"
   
}	// end "OnCbnSelendokResamplemethod"



void CMReformatRectifyDlg::OnEnChangeRotationClockwise (
				wxCommandEvent& 					event)

{
   if (m_mapOrientationAngle != 0)
   	{
      wxTextCtrl* rotationAngle = (wxTextCtrl*)FindWindow (IDC_RotationClockwise);
      rotationAngle->ChangeValue (wxString::Format (wxT("%.1f"), m_rotationAngle));
      
      if (m_rotationAngle == m_mapOrientationAngle)
         m_useMapOrientationAngleFlag = TRUE;
	
      else	// m_rotationAngle != m_mapOrientationAngle
         m_useMapOrientationAngleFlag = FALSE;
      
      wxCheckBox* mapOrientationAngle =
      								(wxCheckBox*)FindWindow (IDC_UseMapOrientationAngle);
      mapOrientationAngle->SetValue (m_useMapOrientationAngleFlag);
      
   	}	// end "if (m_mapOrientationAngle != 0)"
   
}	// end "OnEnChangeRotationClockwise"



void CMReformatRectifyDlg::OnInitDialog (
				wxInitDialogEvent& 				event)

{
   SInt16         					channelSelection,
											fileNamesSelection,
											procedureCode,
											resampleMethodCode,
											headerOptionsSelection;
   
   Boolean         					blankOutsideSelectedAreaFlag,
   										mapInfoExistsFlag;
   
	
   RectifyImageDialogInitialize (this,
                                 m_fileInfoPtr,
                                 &m_dialogSelectArea,
                                 m_reformatOptionsPtr,
                                 &headerOptionsSelection,
                                 &channelSelection,
                                 &blankOutsideSelectedAreaFlag,
                                 &m_backgroundValue,
                                 &procedureCode,
                                 &resampleMethodCode,
                                 &fileNamesSelection,
                                 &m_referenceWindowInfoHandle,
                                 (SInt32*)&m_lineShift,
                                 (SInt32*)&m_columnShift,
                                 &m_lineScaleFactor,
                                 &m_columnScaleFactor,
                                 &m_rotationAngle,
                                 &m_mapOrientationAngle);
   
   m_LineStart = m_reformatOptionsPtr->lineStart;
   m_LineEnd = m_reformatOptionsPtr->lineEnd;
   m_ColumnStart = m_reformatOptionsPtr->columnStart;
   m_ColumnEnd = m_reformatOptionsPtr->columnEnd;
   
   m_blankOutsideSelectedAreaFlag = blankOutsideSelectedAreaFlag;
   m_procedureCode = procedureCode - 1;
   m_resampleMethodCode = resampleMethodCode;
   
   		// Get the resample method list selection that matches the input
   		// resample method code.
   
   m_resampleSelection = GetComboListSelection (IDC_ResampleMethod,
																m_resampleMethodCode);
   
   if (m_resampleSelection == -1)
      m_resampleSelection = 0;
	
   if (fileNamesSelection > 0)
   	m_fileNamesSelection = fileNamesSelection - 1;
   
   		// Set text indicating whether the output file format could be
   		// GeoTIFF or TIFF.
	
   mapInfoExistsFlag = FindIfMapInformationExists (gImageWindowInfoPtr);
   
   if (mapInfoExistsFlag)
      m_fileFormatCtrl->Append (wxT ("GeoTIFF format"));
   else
      m_fileFormatCtrl->Append (wxT("TIFF format"));
   
   m_fileFormatCtrl->SetClientData (0, (void*)(SInt64)m_kNoneMenuItem);
   m_fileFormatCtrl->SetClientData (1, (void*)(SInt64)m_kArcViewMenuItem);
   m_fileFormatCtrl->SetClientData (2, (void*)(SInt64)m_kERDAS74MenuItem);
   m_fileFormatCtrl->SetClientData (3, (void*)(SInt64)m_kTIFFGeoTIFFMenuItem);
   
   m_headerOptionsSelection = headerOptionsSelection;
   m_headerListSelection = GetComboListSelection (IDC_Header,
                                                  m_headerOptionsSelection);
   
   m_localActiveFeaturesPtr = (UInt16*)m_rectifyImageOptionsPtr->rectifyChannelPtr;
   m_channelSelection = channelSelection;
   
  	if (TransferDataToWindow ())
      PositionDialogWindow ();
   
   		// Set default text selection to first edit text item
	
   SelectDialogItemText (this, IDC_LineOffset, 0, SInt16_MAX);
   
}	// end "OnInitDialog"



void CMReformatRectifyDlg::OnSelendokOutChannels (
				wxCommandEvent& 					event)

{
   m_localActiveNumberFeatures = m_rectifyImageOptionsPtr->numberChannelsToRectify;
   
   HandleChannelsMenu (IDC_Channels,
                       kNoTransformation,
                       (SInt16)gImageWindowInfoPtr->totalNumberChannels,
                       kImageChannelType,
                       TRUE);
   
   m_rectifyImageOptionsPtr->numberChannelsToRectify = m_localActiveNumberFeatures;
   
}	// end "OnSelendokOutChannels"


void CMReformatRectifyDlg::UpdateProcedureItems (
				int   								selectItemNumber,
				Boolean  							blankOutsideSelectedAreaFlag)

{
   RectifyImageDialogOnRectifyCode (this,
                                    m_procedureCode+1,
                                    blankOutsideSelectedAreaFlag,
                                    m_mapOrientationAngle);
   
   RectifyImageDialogOnReferenceFile (this,
                                      m_procedureCode+1,
                                      m_fileNamesSelection+1,
                                      &m_referenceWindowInfoHandle,
                                      &m_dialogSelectArea);
   
   		// Set default text selection to first edit text item
   
   SelectDialogItemText (this, selectItemNumber, 0, SInt16_MAX);
   
}	// end "UpdateProcedureItems"



bool CMReformatRectifyDlg::TransferDataFromWindow ()

{
   SInt16      						continueCode = 1,
   										returnCode = 0;
	
	
   wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
   m_LineStartString = l_start->GetValue ();
   m_LineStart = wxAtoi (m_LineStartString);
	
   wxTextCtrl* l_end = (wxTextCtrl*)FindWindow (IDC_LineEnd);
   m_LineEndString = l_end->GetValue ();
   m_LineEnd = wxAtoi (m_LineEndString);
	
   wxTextCtrl* c_start = (wxTextCtrl*)FindWindow (IDC_ColumnStart);
   m_ColumnStartString = c_start->GetValue ();
   m_ColumnStart = wxAtoi (m_ColumnStartString);
	
   wxTextCtrl* c_end = (wxTextCtrl*)FindWindow (IDC_ColumnEnd);
   m_ColumnEndString = c_end->GetValue ();
   m_ColumnEnd = wxAtoi (m_ColumnEndString);
	
   wxTextCtrl* backgroundValue = (wxTextCtrl*)FindWindow (IDC_BackgroundValue);
   m_backgroundValueString = backgroundValue->GetValue ();
   m_backgroundValue = wxAtof (m_backgroundValueString);
	
   wxTextCtrl* lineShift = (wxTextCtrl*)FindWindow (IDC_LineOffset);
   m_lineOffsetString = lineShift->GetValue ();
   m_lineShift = wxAtoi (m_lineOffsetString);
	
   wxTextCtrl* columnShift = (wxTextCtrl*)FindWindow (IDC_ColumnOffset);
   m_columnOffsetString = columnShift->GetValue ();
   m_columnShift = wxAtoi (m_columnOffsetString);
	
   wxTextCtrl* lineScaleFactor = (wxTextCtrl*)FindWindow (IDC_LineScale);
   m_lineScaleString = lineScaleFactor->GetValue ();
   m_lineScaleFactor = wxAtof (m_lineScaleString);
	
   wxTextCtrl* columnScaleFactor = (wxTextCtrl*)FindWindow (IDC_ColumnScale);
   m_columnScaleString = columnScaleFactor->GetValue ();
   m_columnScaleFactor = wxAtof (m_columnScaleString);
	
   wxTextCtrl* rotationAngle = (wxTextCtrl*)FindWindow (IDC_RotationClockwise);
   m_rotationAngleString = rotationAngle->GetValue ();
   m_rotationAngle = wxAtof (m_rotationAngleString);
   
   m_headerListSelection = m_fileFormatCtrl->GetSelection ();
   SInt64 headerList =
				(SInt64)((int*)m_fileFormatCtrl->GetClientData (m_headerListSelection));
   m_headerOptionsSelection = (SInt16)headerList;
   
   m_channelSelection = m_channelsCtrl->GetSelection ();
	
   returnCode = CheckDialogRealValue (this,
                                      IDC_BackgroundValue,
                                      m_minBackgroundValue,
                                      m_maxBackgroundValue,
                                      0,
                                      true,      // minValue is allowed.
                                      true,      // maxValue is allowed,
                                      kDisplayRangeAlert);
   
   if (m_procedureCode == kTranslateScaleRotate - 1)
   	{
      if (returnCode == 0)
      	{
         continueCode = CheckMaxValue (this,
                                       IDC_LineOffset,
                                       -kMaxNumberLines,
                                       kMaxNumberLines,
                                       kDisplayRangeAlert);
         if (continueCode != 1)
            returnCode = IDC_LineOffset;
			
      	}	// end "if (returnCode == 0)"
      
      if (returnCode == 0)
      	{
         continueCode = CheckMaxValue (this,
                                       IDC_ColumnOffset,
                                       -kMaxNumberLines,
                                       kMaxNumberLines,
                                       kDisplayRangeAlert);
         if (continueCode != 1)
            returnCode = IDC_ColumnOffset;
			
      	}	// end "if (returnCode == 0)"
      
      
      if (returnCode == 0)
         returnCode = CheckDialogRealValue (this,
                                            IDC_LineScale,
                                            0,
                                            1000,
                                            0,
                                            false,   // minValue not allowed.
                                            true,      // maxValue is allowed,
                                            kDisplayRangeAlert);
      
      if (returnCode == 0)
         returnCode = CheckDialogRealValue (this,
                                            IDC_ColumnScale,
                                            0,
                                            1000,
                                            0,
                                            false,   // minValue not allowed.
                                            true,      // maxValue is allowed,
                                            kDisplayRangeAlert);
      
      if (returnCode == 0)
         returnCode = CheckDialogRealValue (this,
                                            IDC_RotationClockwise,
                                            -180,
                                            180,
                                            0,
                                            true,   // minValue is allowed.
                                            true,      // maxValue is allowed,
                                            kDisplayRangeAlert);
      
   	}	// end "if (m_procedureCode == kTranslateScaleRotate - 1)"
   
   if (returnCode == 0)
      returnCode = VerifyLineColumnValues (IDC_LineStart,
                                          	IDC_ColumnStart,
                                          	false);
   
   return (returnCode == 0);
   
}	// end "TransferDataFromWindow"



bool CMReformatRectifyDlg::TransferDataToWindow ()

{
	
	
   m_radioBtn18->SetValue (m_procedureCode == kTranslateScaleRotate-1);
   m_radioBtn19->SetValue (m_procedureCode == kReprojectToReferenceImage-1);
   
   wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
   l_start->ChangeValue (wxString::Format (wxT("%d"), m_LineStart));
	
   wxTextCtrl* l_end = (wxTextCtrl*)FindWindow (IDC_LineEnd);
   l_end->ChangeValue (wxString::Format (wxT("%d"), m_LineEnd));
	
   wxTextCtrl* c_start = (wxTextCtrl*)FindWindow (IDC_ColumnStart);
   c_start->ChangeValue (wxString::Format (wxT("%d"), m_ColumnStart));
	
   wxTextCtrl* c_end = (wxTextCtrl*)FindWindow (IDC_ColumnEnd);
   c_end->ChangeValue (wxString::Format (wxT("%d"), m_ColumnEnd));
   
   wxTextCtrl* backgroundValue = (wxTextCtrl*)FindWindow (IDC_BackgroundValue);
   backgroundValue->ChangeValue (wxString::Format (wxT("%.1f"), m_backgroundValue));
	
   wxTextCtrl* lineShift = (wxTextCtrl*)FindWindow (IDC_LineOffset);
   lineShift->ChangeValue (wxString::Format (wxT("%ld"), m_lineShift));
	
   wxTextCtrl* columnShift = (wxTextCtrl*)FindWindow (IDC_ColumnOffset);
   columnShift->ChangeValue (wxString::Format (wxT("%ld"), m_columnShift));
	
   wxTextCtrl* lineScaleFactor = (wxTextCtrl*)FindWindow (IDC_LineScale);
   lineScaleFactor->ChangeValue (wxString::Format (wxT("%.1f"), m_lineScaleFactor));
	
   wxTextCtrl* columnScaleFactor = (wxTextCtrl*)FindWindow (IDC_ColumnScale);
   columnScaleFactor->ChangeValue (wxString::Format (wxT("%.1f"), m_columnScaleFactor));
	
   wxTextCtrl* rotationAngle = (wxTextCtrl*)FindWindow (IDC_RotationClockwise);
   rotationAngle->ChangeValue (wxString::Format (wxT("%.1f"), m_rotationAngle));
	
   wxCheckBox* mapOrientationAngle =
   										(wxCheckBox*)FindWindow (IDC_UseMapOrientationAngle);
	mapOrientationAngle->SetValue (m_useMapOrientationAngleFlag);
	
   m_channelsCtrl->SetSelection (m_channelSelection);
	
   m_fileFormatCtrl->SetSelection (m_headerListSelection);
	
   m_referenceListCtrl->SetSelection (m_fileNamesSelection);
	
   m_algorithmCtrl->SetSelection (m_resampleSelection);
   
   return true;
   
}	// end "TransferDataToWindow"

