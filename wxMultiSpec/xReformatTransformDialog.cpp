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
//   File:                 xReformatTransformDialog.cpp : class implementation file
//   Class Definition:     xReformatTransformDialog.h
//
//   Authors:              Abdur Rahman Maud, Larry L. Biehl
//
//   Revision date:        05/06/2020
//
//   Language:					C++
//
//   System:               Linux and MacOS Operating Systems
//
//   Brief description:  	This file contains functions that relate to the
//                       	CMReformatTransformDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 
#include "xReformatTransformDialog.h"
#include "xChannelsDialog.h"

#include "wx/valnum.h"



BEGIN_EVENT_TABLE (CMReformatTransformDlg, CMDialog)
	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_EV_Eigenvectors, CMReformatTransformDlg::OnSelendokEVEigenvectors)
	#endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_EV_Eigenvectors, CMReformatTransformDlg::OnSelendokEVEigenvectors)
	#endif

	EVT_CHOICE (IDC_AlgebraicTransformOptions, CMReformatTransformDlg::OnSelendokAlgebraicTransformOptions)
	EVT_CHOICE (IDC_ReformatFunctions, CMReformatTransformDlg::OnSelendokReformatFunctions)

	EVT_INIT_DIALOG (CMReformatTransformDlg::OnInitDialog)

	EVT_RADIOBUTTON (IDC_AdjustSelectedChannels, CMReformatTransformDlg::OnAdjustSelectedChannels)
	EVT_RADIOBUTTON (IDC_AdjustSelectedChannelsbyChannel, CMReformatTransformDlg::OnAdjustSelectedChannelsByChannel)
	EVT_RADIOBUTTON (IDC_RT_AlgebraicTransformation, CMReformatTransformDlg::OnRTAlgebraicTransformation)
	EVT_RADIOBUTTON (IDC_RT_Eigenvectors, CMReformatTransformDlg::OnRTEigenvectors)
	EVT_RADIOBUTTON (IDC_RT_FunctionOfChannels, CMReformatTransformDlg::OnRTFunctionOfChannels)
	EVT_RADIOBUTTON (IDC_RT_NoTransformation, CMReformatTransformDlg::OnRTNoTransformation)
END_EVENT_TABLE ()



CMReformatTransformDlg::CMReformatTransformDlg (
				ReformatOptionsPtr 				reformatOptionsPtr,
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title /*=NULL*/)
		: CMDialog (CMReformatTransformDlg::IDD, pParent, title)

{
	m_reformatOptionsPtr = reformatOptionsPtr;
	m_adjustDivisor = 0;
	m_adjustFactor = 0;
	m_adjustOffset = 0;
	m_algebraicTransformOption = 0;
	m_transformFactor = 0;
	m_transformOffset = 0;
	m_denominatorString = "";
	m_numeratorString = "";
	m_scaleFactor = 0;
	m_channelSelection = -1;
	m_minimumNumberBits = 1;
	m_minSelectedNumberBits = 8;
	m_transformCode = -1;
	m_functionFactor = 1;
	m_kthSmallestElement = 1;
	m_functionCode = -1;
	m_adjustSelectedChannelsFactor = -1.;
	m_adjustSelectedChannel = 1;

	m_maxAdjustOffset = SInt16_MAX;
	m_maxAdjustFactor = SInt16_MAX;
	m_maxAdjustDivisor = SInt16_MAX;

	m_maxValue = SInt16_MAX;
	m_minValue = SInt16_MAX;

	m_initializedFlag = CMDialog::m_initializedFlag;

	CreateControls ();
	
}	// end "CMReformatTransformDlg"



CMReformatTransformDlg::~CMReformatTransformDlg ()

{

}	// end "~CMReformatTransformDlg"



void CMReformatTransformDlg::CreateControls ()

{
	wxFloatingPointValidator<double> doubleValue8Digits (8, &m_double8DigitValueCheck);
	
	SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	bSizer84 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer85;
	bSizer85 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer (wxHORIZONTAL);
	
	m_radioBtn5 = new wxRadioButton (this,
												IDC_AdjustSelectedChannels,
												wxT("Adjust Selected Channels"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_radioBtn5, IDS_ToolTip249);
	bSizer91->Add (m_radioBtn5, wxSizerFlags(0).Border(wxBOTTOM, 5));
	
	bSizer85->Add (bSizer91, 0, wxALL | wxEXPAND, 5);
	
	wxBoxSizer* bSizer92;
	bSizer92 = new wxBoxSizer (wxHORIZONTAL);
	
	bSizer92->Add (25, 0, 0, wxEXPAND);
	
	m_staticText127 = new wxStaticText (this,
													IDC_RT_AC_Equal,
													wxT("="),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText127->Wrap (-1);
	bSizer92->Add (m_staticText127, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_textCtrl48 = new wxTextCtrl (this,
												IDC_RT_AC_Offset,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_textCtrl48, IDS_ToolTip250);
	bSizer92->Add (m_textCtrl48, 0, wxALL, 5);
	
	m_staticText128 = new wxStaticText (this,
													IDC_RT_AC_Plus1,
													wxT("+"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText128->Wrap (-1);
	bSizer92->Add (m_staticText128, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_textCtrl49 = new wxTextCtrl (this,
												IDC_RT_AC_Factor,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_textCtrl49, IDS_ToolTip251);
	bSizer92->Add (m_textCtrl49, 0, wxALL, 5);
	
	m_staticText129 = new wxStaticText (this,
													IDC_RT_AC_OldValue,
													wxT("* OldValue /"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText129->Wrap (-1);
	bSizer92->Add (m_staticText129, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_textCtrl50 = new wxTextCtrl (this,
												IDC_RT_AC_Divisor,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_textCtrl50, IDS_ToolTip251);
	bSizer92->Add (m_textCtrl50, 0, wxALL, 5);
	
	//bSizer85->Add (bSizer92, 0, wxEXPAND);
	bSizer85->Add (bSizer92, wxSizerFlags(0).Expand());
	
	bSizer84->Add (bSizer85,
						wxSizerFlags(0).Expand().ReserveSpaceEvenIfHidden().
							Border(wxLEFT|wxTOP|wxRIGHT, 12));
	
	wxBoxSizer* bSizer86;
	bSizer86 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer93;
	bSizer93 = new wxBoxSizer (wxHORIZONTAL);
	
	m_radioBtn6 = new wxRadioButton (
										this,
										IDC_AdjustSelectedChannelsbyChannel,
										wxT("Adjust Selected Channels by Selected Channel"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
   SetUpToolTip (m_radioBtn6, IDS_ToolTip252);
	bSizer93->Add (m_radioBtn6, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 5));
	
	
	bSizer86->Add(bSizer93, 0, wxALL | wxEXPAND, 5);
	
	wxBoxSizer* bSizer94;
	bSizer94 = new wxBoxSizer (wxHORIZONTAL);
	
	
	bSizer94->Add (25, 0, 0, wxEXPAND, 5);
	
	m_staticText130 = new wxStaticText (this,
													IDC_RT_ACbyC_OldValue,
													wxT("=  OldValue + "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText130->Wrap (-1);
	bSizer94->Add (m_staticText130, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_textCtrl51 = new wxTextCtrl (this,
												IDC_RT_ACbyC_Factor,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer94->Add (m_textCtrl51, 0, wxALL, 5);
	
	m_staticText131 = new wxStaticText (this,
													IDC_RT_ACbyC_ChannelPrompt,
													wxT("* Channel:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText131->Wrap (-1);
	bSizer94->Add (m_staticText131, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_textCtrl52 = new wxTextCtrl (this,
												IDC_RT_ACbyC_ChannelNumber,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer94->Add (m_textCtrl52, 0, wxALL, 5);
	
	bSizer86->Add (bSizer94, 0, wxEXPAND);
	
	bSizer84->Add (bSizer86,
						wxSizerFlags(0).Expand().ReserveSpaceEvenIfHidden().
								Border(wxLEFT|wxRIGHT, 12));
	
	wxBoxSizer* bSizer87;
	bSizer87 = new wxBoxSizer (wxVERTICAL);
	
	bSizer103 = new wxBoxSizer (wxHORIZONTAL);
	
	m_radioBtn7 = new wxRadioButton (this,
												IDC_RT_Eigenvectors,
												wxT("New Channels from"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	SetUpToolTip (m_radioBtn7, IDS_ToolTip253);
	bSizer103->Add (
				m_radioBtn7,
				wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxTOP|wxBOTTOM, 5));
	
	m_staticText145 = new wxStaticText (this,
													IDC_RT_EV_PCEigenvectors,
													wxT("PC Eigenvectors"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText145->Wrap (-1);
   SetUpToolTip (m_staticText145, IDS_ToolTip254);
	bSizer103->Add (m_staticText145, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_staticText134 = new wxStaticText (this,
													IDC_RT_EV_FEEigenvectors,
													wxT("FE Eigenvectors"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText134->Wrap (-1);
   SetUpToolTip (m_staticText134, IDS_ToolTip255);
	bSizer103->Add (m_staticText134, 0, wxALIGN_CENTER | wxALL, 5);

	GetAllSubsetMenuControl (kChannelsMenu,
										this,
										IDC_EV_Eigenvectors,
										120,
										IDS_ToolTip256);
 
	bSizer103->Add (m_channelsCtrl, 0, wxALL, 5);
	
	bSizer87->Add (bSizer103, 1, wxALL | wxEXPAND, 5);
	
	wxBoxSizer* bSizer104;
	bSizer104 = new wxBoxSizer (wxHORIZONTAL);
	
	
	bSizer104->Add (25, 0, 0, wxEXPAND, 5);
	
	wxBoxSizer* bSizer106;
	bSizer106 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText135 = new wxStaticText (
					this,
					IDC_RT_EV_NumberBits,
					wxT("Recommended minimum number of bits - All PC Components:\r                                                                 Selected Components:"),
					wxDefaultPosition,
					wxDefaultSize,
					0);
	m_staticText135->Wrap (-1);
	bSizer106->Add (m_staticText135, 0, wxALIGN_RIGHT | wxALL, 5);
	
	bSizer104->Add (bSizer106, 0, 0, 5);
	
	wxBoxSizer* bSizer105;
	bSizer105 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText136 = new wxStaticText (this,
													IDC_RT_EV_AllComponents,
													wxT("16"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText136->Wrap (-1);
	bSizer105->Add (m_staticText136, 0, wxTOP, 5);
	
	m_staticText137 = new wxStaticText (this,
													IDC_RT_EV_SelComponents,
													wxT("16"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText137->Wrap (-1);
	bSizer105->Add (m_staticText137, 0, 0, 5);
	
	bSizer104->Add (bSizer105, 0, wxEXPAND);
	
	bSizer87->Add (bSizer104, 1, wxEXPAND);
	
	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer (wxHORIZONTAL);
	
	bSizer111->Add (25, 0, 0, wxEXPAND);
	
	m_staticText140 = new wxStaticText (this,
													IDC_ScaleFactorPrompt,
													wxT("Scale Factor:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText140->Wrap (-1);
	bSizer111->Add (m_staticText140, 0, wxALIGN_CENTER | wxLEFT, 5);
	
	m_textCtrl56 = new wxTextCtrl (this,
												IDC_EV_ScaleFactor,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer111->Add (m_textCtrl56, 0, wxALIGN_CENTER | wxLEFT, 5);
	
	bSizer87->Add (bSizer111, 1, wxEXPAND);
	
	bSizer84->Add (bSizer87,
						wxSizerFlags(0).Expand().ReserveSpaceEvenIfHidden().
								Border(wxLEFT|wxRIGHT, 12));
	
	wxBoxSizer* bSizer88;
	bSizer88 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer96;
	bSizer96 = new wxBoxSizer (wxHORIZONTAL);
	
	m_radioBtn8 = new wxRadioButton (
									this,
									IDC_RT_AlgebraicTransformation,
									wxT("New Channel from Algebraic Transformation: "),
									wxDefaultPosition,
									wxDefaultSize,
									0);
   SetUpToolTip (m_radioBtn8, IDS_ToolTip257);
	bSizer96->Add (m_radioBtn8, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 5));
	
	wxChoice*		comboBox14;
	comboBox14 = new wxChoice (this,
										IDC_AlgebraicTransformOptions,
										wxDefaultPosition,
										wxDefaultSize);
	comboBox14->Append (wxT("General"));
	comboBox14->Append (wxT("to Radiant Temperature (K)"));
	comboBox14->Append (wxT("to Radiant Temperature (C)"));
	comboBox14->Append (wxT("to Radiant Temperature (F)"));
	bSizer96->Add (comboBox14, wxSizerFlags(0).Border(wxALL, 5));
	
	bSizer88->Add (bSizer96, 0, wxALL | wxEXPAND, 5);
	
	wxBoxSizer* bSizer97;
	bSizer97 = new wxBoxSizer (wxHORIZONTAL);
	
	bSizer97->Add (25, 0, 0, wxEXPAND);
	
	m_AlgebraicEqualSign1 = new wxStaticText (this,
													IDC_RT_AT_Equal,
													//wxT("="),
													wxT("      Radiance ="),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_AlgebraicEqualSign1->Wrap (-1);
	//bSizer97->Add (m_AlgebraicEqualSign1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	bSizer97->Add (m_AlgebraicEqualSign1,
						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
	
	m_AT_offsetTextCtrl = new wxTextCtrl (this,
														IDC_RT_AT_Offset,
														wxEmptyString,
														wxDefaultPosition,
														wxDefaultSize,
														0);
	m_AT_offsetTextCtrl->SetValidator (doubleValue8Digits);
   SetUpToolTip (m_AT_offsetTextCtrl, IDS_ToolTip250);
	bSizer97->Add (m_AT_offsetTextCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	
	m_staticText142 = new wxStaticText (this,
													IDC_RT_AT_Plus,
													wxT("+"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText142->Wrap (-1);
	bSizer97->Add (m_staticText142, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	
	wxBoxSizer* bSizer112;
	bSizer112 = new wxBoxSizer (wxVERTICAL);
	
	m_AT_numereratorTextCtrl = new wxTextCtrl (this,
																IDC_RT_AT_Numerator,
																wxEmptyString,
																wxDefaultPosition,
																wxSize (200, -1),
																0);
   SetUpToolTip (m_AT_numereratorTextCtrl, IDS_ToolTip259);
	//bSizer112->Add (m_AT_numereratorTextCtrl, 0, wxALIGN_CENTER, 5);
	bSizer112->Add (m_AT_numereratorTextCtrl, wxSizerFlags(0).Align(wxALIGN_CENTER));
	
	m_staticText144 = new wxStaticText (this,
													IDC_RT_AT_Line,
													wxT("_________________________________"),
													wxDefaultPosition,
													//wxDefaultSize,
													wxSize (-1, 18),
													0);
	m_staticText144->Wrap (-1);
	//bSizer112->Add (m_staticText144, 0, wxALIGN_CENTER | wxBOTTOM, 10);
	bSizer112->Add (m_staticText144,
							wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxBOTTOM, 10));
	
	m_AT_denominatorTextCtrl = new wxTextCtrl (this,
																IDC_RT_AT_Denominator,
																wxEmptyString,
																wxDefaultPosition,
																wxSize (200, -1),
																0);
   SetUpToolTip (m_AT_denominatorTextCtrl, IDS_ToolTip260);
	//bSizer112->Add (m_AT_denominatorTextCtrl, 0, wxALIGN_CENTER);
	bSizer112->Add (m_AT_denominatorTextCtrl, wxSizerFlags(0).Align(wxALIGN_CENTER));
	
	//bSizer97->Add (bSizer112, 0, wxEXPAND);
	bSizer97->Add (bSizer112, wxSizerFlags(0).Expand());
	
	m_staticText143 = new wxStaticText (this,
													IDC_RT_AT_Multiply,
													wxT("*"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText143->Wrap (-1);
	bSizer97->Add (m_staticText143, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_AT_factorTextCtrl = new wxTextCtrl (this,
														IDC_RT_AT_Factor,
														wxEmptyString,
														wxDefaultPosition,
														wxDefaultSize,
														0);
	m_AT_factorTextCtrl->SetValidator (doubleValue8Digits);
   SetUpToolTip (m_AT_factorTextCtrl, IDS_ToolTip258);
	bSizer97->Add (m_AT_factorTextCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	
	bSizer88->Add (bSizer97, 0, wxEXPAND);
	
	wxBoxSizer* bHSizer97B;
	bHSizer97B = new wxBoxSizer (wxHORIZONTAL);
	
	bHSizer97B->Add (25, 0, 0, wxEXPAND);
	
	wxStaticText*	staticText97B_1;
	staticText97B_1 = new wxStaticText (this,
													IDC_RT_AT_Therm_Equal,
													wxT("="),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	staticText97B_1->Wrap (-1);
	bHSizer97B->Add (staticText97B_1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	
	wxTextCtrl*	textCtrl97B_2;
	textCtrl97B_2 = new wxTextCtrl (this,
												IDC_RT_AT_Therm_K2,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	textCtrl97B_2->SetValidator (doubleValue8Digits);
   SetUpToolTip (textCtrl97B_2, IDS_ToolTip8702);
	//bHSizer97B->Add (textCtrl97B_2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	bHSizer97B->Add (textCtrl97B_2,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).
														Border(wxLEFT|wxTOP|wxBOTTOM, 5));
	
	wxStaticText*	staticText97B_3;
	staticText97B_3 = new wxStaticText (this,
													IDC_RT_AT_Therm_ln,
													wxT(" / ln("),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	staticText97B_3->Wrap (-1);
	//bHSizer97B->Add (staticText97B_3, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	bHSizer97B->Add (staticText97B_3,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).
														Border(wxTOP|wxBOTTOM, 5));
	
	wxTextCtrl*	textCtrl97B_4;
	textCtrl97B_4 = new wxTextCtrl (this,
												IDC_RT_AT_Therm_K1,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	textCtrl97B_4->SetValidator (doubleValue8Digits);
   SetUpToolTip (textCtrl97B_4, IDS_ToolTip8701);
	//bHSizer97B->Add (textCtrl97B_4, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	bHSizer97B->Add (textCtrl97B_4,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).
														Border(wxTOP|wxBOTTOM, 5));
	
	wxStaticText*	staticText97B_5;
	staticText97B_5 = new wxStaticText (this,
													IDC_RT_AT_Therm_TOAr,
													wxT(" / Radiance + 1)"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	staticText97B_5->Wrap (-1);
	//bHSizer97B->Add (staticText97B_5, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	bHSizer97B->Add (staticText97B_5,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).
														Border(wxTOP|wxRIGHT|wxBOTTOM, 5));
	
	//bSizer88->Add (bHSizer97B, 0, wxEXPAND);
	bSizer88->Add (bHSizer97B, wxSizerFlags(0).Expand().Border(wxTOP, 5));
	// ----
	
	bSizer84->Add (bSizer88,
						wxSizerFlags(0).Expand().ReserveSpaceEvenIfHidden().
								Border(wxLEFT|wxRIGHT, 12));
	
	wxBoxSizer* bSizer89;
	bSizer89 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer98;
	bSizer98 = new wxBoxSizer (wxHORIZONTAL);
	
	m_radioBtn9 = new wxRadioButton (this,
												IDC_RT_FunctionOfChannels,
												wxT("New Channel from Function"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_radioBtn9, IDS_ToolTip261);
	bSizer98->Add (m_radioBtn9, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 5));
	
	bSizer89->Add (bSizer98, 0, wxALL | wxEXPAND, 5);
	
	wxBoxSizer* bSizer99;
	bSizer99 = new wxBoxSizer (wxHORIZONTAL);
	
	bSizer99->Add (25, 0, 0, wxEXPAND);
	
	wxChoice*		comboBox13;
	comboBox13 = new wxChoice (this,
										IDC_ReformatFunctions,
										wxDefaultPosition,
										wxDefaultSize);
	comboBox13->Append (wxT("Minimum"));
	comboBox13->Append (wxT("Maximum"));
	comboBox13->Append (wxT("Sum"));
	comboBox13->Append (wxT("Average"));
	comboBox13->Append (wxT("Median"));
	comboBox13->Append (wxT("Kth Smallest Value"));
	bSizer99->Add (comboBox13, 0, wxALL, 5);
	
	m_staticText132 = new wxStaticText (this,
													IDC_FunctionFactorPrompt,
													wxT("of selected channels *"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText132->Wrap (-1);
	bSizer99->Add (m_staticText132, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_textCtrl53 = new wxTextCtrl (this,
												IDC_FunctionFactor,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer99->Add (m_textCtrl53, 0, wxALL, 5);
	
	bSizer89->Add (bSizer99, 0, wxEXPAND);
	
	wxBoxSizer* bSizer102;
	bSizer102 = new wxBoxSizer (wxHORIZONTAL);
	
	bSizer102->Add (60, 0, 0, wxEXPAND);
	
	m_staticText133 = new wxStaticText (this,
													IDC_kthSmallestElementPrompt,
													wxT("kth smallest value:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText133->Wrap (-1);
	bSizer102->Add (m_staticText133, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_textCtrl54 = new wxTextCtrl (this,
												IDC_kthSmallestElement,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer102->Add (m_textCtrl54, 0, wxALL, 5);
	
	bSizer89->Add (bSizer102, 1, wxEXPAND);
	
	bSizer84->Add (bSizer89,
						wxSizerFlags(0).Expand().ReserveSpaceEvenIfHidden().
								Border(wxLEFT|wxRIGHT, 12));
	
	wxBoxSizer* bSizer90;
	bSizer90 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer100;
	bSizer100 = new wxBoxSizer (wxHORIZONTAL);
	
	m_radioBtn10 = new wxRadioButton (this,
													IDC_RT_NoTransformation,
													wxT("No Transformation to be Done"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	bSizer100->Add (m_radioBtn10, wxSizerFlags(0).Border(wxTOP, 5));
	
	bSizer90->Add (bSizer100, 1, wxALL | wxEXPAND, 5);
	
	bSizer84->Add (bSizer90, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));

	CreateStandardButtons (bSizer84);
	
	SetSizer (bSizer84);
	Layout ();
	Centre (wxBOTH);
	SetSizerAndFit (bSizer84);
	
}	// end "CreateControls"



Boolean CMReformatTransformDlg::DoDialog (
				UInt16* 								recommendNumberOfBitsPtr,
				SInt16 								bandInterleaveSelection)

{
	SInt16 								returnCode;

	UInt16 								item;

	Boolean 								OKFlag = FALSE;


			// Make sure intialization has been completed.

	if (!m_initializedFlag)
																							return (FALSE);

	m_bandInterleaveSelection = bandInterleaveSelection;

	returnCode = ShowModal ();

	if (returnCode == wxID_OK)
		{
				// Get transform data code.

		m_reformatOptionsPtr->transformDataCode = m_transformCode;

				// Number to offset data values by.

		m_reformatOptionsPtr->adjustOffset = m_adjustOffset;
        
				// Number to multiply data values by.
        
		m_reformatOptionsPtr->adjustFactor = m_adjustFactor;
        
				// Number to divide data values by.

		m_reformatOptionsPtr->adjustDivisor = m_adjustDivisor;

				// Output data can be signed flag.
				// Only valid for transform code of 1.

        		// Get the list of principal component channels to create
				// if transform code 'kCreatePCImage' has been selected.

		if (m_reformatOptionsPtr->transformDataCode == kCreatePCImage)
			{
			m_reformatOptionsPtr->pcScaleFactor = m_scaleFactor;
			m_reformatOptionsPtr->numberPCChannels = m_localActiveNumberFeatures;

			if (m_channelSelection == kAllMenuItem)
				{
				m_reformatOptionsPtr->numberPCChannels = m_numberEigenvectors;
				for (item = 0; item < m_numberEigenvectors; item++)
					m_reformatOptionsPtr->pcChannelPtr[item] = item;

				}	// end "if (m_channelSelection == kAllMenuItem)"

			}	// end "if (...->transformDataCode == kCreatePCImage)"

				// Decode the numerator and denominator if transform code
				// 2 has been selected.

		if (m_reformatOptionsPtr->transformDataCode == kTransformChannels)
			{
			item = 0;
				
			m_reformatOptionsPtr->algebraicTransformOption = m_algebraicTransformOption;

					// Transform offset.

			m_reformatOptionsPtr->transformOffset = m_transformOffset;

					// Decode the denominator.
					// Decode the numerator.
					//		These have already been decoded in "DoDataExchange".

					// Estimate recommendation for minimum number bits needed.

			if (m_reformatOptionsPtr->numberDenominatorTerms > 0)
				{
				if (m_transformFactor <= 10)
							// This implies 32-bit real
					m_minSelectedNumberBits = 32;

				else if (m_transformFactor <= 100)
					m_minSelectedNumberBits = 8;

				else
					m_minSelectedNumberBits = 16;

            }	// end "if (...->numberDenominatorTerms > 0)"

			m_reformatOptionsPtr->defaultThermalChannel = m_defaultThermalChannel;
			m_reformatOptionsPtr->algebraicTransformRadianceMult = m_radianceMult;
			m_reformatOptionsPtr->algebraicTransformRadianceOffset = m_radianceOffset;
			m_reformatOptionsPtr->algebraicTransformK1Value = m_thermalK1;
			m_reformatOptionsPtr->algebraicTransformK2Value = m_thermalK2;

					// Transform factor.

			m_reformatOptionsPtr->transformFactor = m_transformFactor;

			m_reformatOptionsPtr->transformChannelPtrSetFlag = FALSE;

			}	// end "if (...->transformDataCode == kTransformChannels)"

		if (m_reformatOptionsPtr->transformDataCode == kFunctionOfChannels)
			{
					// Get info for function of channels.

			m_reformatOptionsPtr->functionCode = m_functionCode + 1;
			m_reformatOptionsPtr->functionFactor = m_functionFactor;
			m_reformatOptionsPtr->kthSmallestValue = m_kthSmallestElement;

			}	// end "if (...->transformDataCode == kFunctionOfChannels)"

		if (m_reformatOptionsPtr->transformDataCode == kAdjustChannelsByChannel)
			{
					// Get info for adjusting selected channels by a channel

			m_reformatOptionsPtr->transformAdjustSelectedChannelsFactor = m_adjustSelectedChannelsFactor;
			m_reformatOptionsPtr->transformAdjustSelectedChannel = m_adjustSelectedChannel;
            
			}	// end "if (...->transformDataCode == kFunctionOfChannels)"

		m_reformatOptionsPtr->eigenSource = m_eigenSource;
        
		*recommendNumberOfBitsPtr = m_minSelectedNumberBits;

		OKFlag = TRUE;

		}	// end "if (returnCode == IDOK)"

	return (OKFlag);

}	// end "DoDialog"



void CMReformatTransformDlg::OnAdjustSelectedChannels (
				wxCommandEvent& 					event)

{
	DialogPtr 							dialogPtr = this;
	
	
	ShowHideAdjustChannelItems (dialogPtr, kShow);
	ShowHideAdjustSelectedChannelsByChannelItems (dialogPtr, kHide);
	ShowHidePCTransformItems (dialogPtr, kHide, m_eigenSource);
	ShowHideAlgebraicTransformItems (dialogPtr, kHide);
	ShowHideFunctionChannelsItems (dialogPtr, kHide, 0);
	
			// Set default text selection to first edit text item
	
	SelectDialogItemText (this, IDC_RT_AC_Offset, 0, SInt16_MAX);
	m_transformCode = kAdjustChannel;
	
}	// end "OnAdjustSelectedChannels"



void CMReformatTransformDlg::OnAdjustSelectedChannelsByChannel (
				wxCommandEvent& 					event)

{
	DialogPtr 							dialogPtr = this;
	
	
	ShowHideAdjustChannelItems (dialogPtr, kHide);
	ShowHideAdjustSelectedChannelsByChannelItems (dialogPtr, kShow);
	ShowHidePCTransformItems (dialogPtr, kHide, m_eigenSource);
	ShowHideAlgebraicTransformItems (dialogPtr, kHide);
	ShowHideFunctionChannelsItems (dialogPtr, kHide, 0);
	
	// Set default text selection to first edit text item
	
	SelectDialogItemText (this, IDC_RT_ACbyC_Factor, 0, SInt16_MAX);
	m_transformCode = kAdjustChannelsByChannel;
	
}	// end "OnBnClickedAdjustSelectedChannelsByChannel"



void CMReformatTransformDlg::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	CMDialog* 							dialogPtr = this;

	UInt16 								selectedItem;


	m_maxChannel = gImageWindowInfoPtr->totalNumberChannels;

			// Adjust data radio button.
			// Hide related items until the box is checked.

	selectedItem = 0;

	m_transformCode = m_reformatOptionsPtr->transformDataCode;

			// Adjust Selected Channels

	if (m_transformCode != kAdjustChannel)
		ShowHideAdjustChannelItems (dialogPtr, kHide);

	else	// m_transformCode == kAdjustChannel
		selectedItem = IDC_RT_AC_Offset;

	m_adjustOffset = m_reformatOptionsPtr->adjustOffset;

	m_adjustFactor = m_reformatOptionsPtr->adjustFactor;
	m_adjustDivisor = m_reformatOptionsPtr->adjustDivisor;

			// Adjust Selected Channels by a Selected Channel

	if (m_transformCode != kAdjustChannelsByChannel)
	  ShowHideAdjustSelectedChannelsByChannelItems (dialogPtr, kHide);

	else	// m_transformCode == kAdjustChannel
	  selectedItem = IDC_RT_ACbyC_Factor;

	m_adjustSelectedChannelsFactor = m_reformatOptionsPtr->transformAdjustSelectedChannelsFactor;
	m_adjustSelectedChannel = m_reformatOptionsPtr->transformAdjustSelectedChannel;

			// Use transformation radio button.

	EigenvectorInfoExists (kNoProject, &m_eigenSource, &m_numberEigenvectors);
     
	m_scaleFactor = m_reformatOptionsPtr->pcScaleFactor;
	if (m_numberEigenvectors > 0 && m_bandInterleaveSelection != kBSQ)
    	{
		if (m_eigenSource == 1)
			MHideDialogItem (dialogPtr, IDC_RT_EV_FEEigenvectors);

		if (m_eigenSource == 2)
			MHideDialogItem (dialogPtr, IDC_RT_EV_PCEigenvectors);

		if (m_transformCode != kCreatePCImage)
			ShowHidePCTransformItems (dialogPtr, kHide, m_eigenSource);

		else	// m_transformCode == kCreatePCImage
			selectedItem = IDC_EV_ScaleFactor;

		m_minimumNumberBits = DeterminePCMinimumNumberBits (
                												m_scaleFactor, m_eigenSource, 0);

		m_minSelectedNumberBits = DeterminePCMinimumNumberBits (
                m_scaleFactor, m_eigenSource, m_reformatOptionsPtr->pcChannelPtr[0]);

    	}	// end "if (m_numberEigenvectors > 0 && ...)"

	else	// m_numberEigenvectors <= 0 || m_bandInterleaveSelection == kBSQ
		ShowHidePCTransformItems (dialogPtr, kHide, m_eigenSource);

	m_channelSelection = kAllMenuItem;
	m_localNumberFeatures = m_reformatOptionsPtr->numberPCChannels;

	if (m_localNumberFeatures < m_numberEigenvectors)
		m_channelSelection = kSubsetMenuItem;

	m_localFeaturesPtr = (UInt16*)m_reformatOptionsPtr->pcChannelPtr;

			// Set feature parameters

	m_localActiveNumberFeatures = m_localNumberFeatures;
	m_localActiveFeaturesPtr = m_localFeaturesPtr;

			// Use general algebraic transformation radio button.

	if (gImageFileInfoPtr->thematicType)
		MHideDialogItem (dialogPtr, IDC_RT_AlgebraicTransformation);
		
	m_algebraicTransformOption = m_reformatOptionsPtr->algebraicTransformOption;

	if (m_transformCode != kTransformChannels)
		ShowHideAlgebraicTransformItems (dialogPtr, kHide);

	else	// m_transformCode == kTransformChannels
		selectedItem = IDC_RT_AT_Offset;

	if (!m_reformatOptionsPtr->algebraicStringsLoadedFlag)
		{
		if (!GetDefaultBandRatio (gImageWindowInfoPtr,
											gImageFileInfoPtr,
											m_reformatOptionsPtr))
			{
			m_reformatOptionsPtr->transformFactor = 1.;
			m_reformatOptionsPtr->transformOffset = 0.;

			}	// end "if (!GetDefaultBandRatio (gImageWindowInfoPtr, ..."
		
		m_reformatOptionsPtr->algebraicStringsLoadedFlag = TRUE;
		
		}	// end "if (!m_reformatOptionsPtr->algebraicStringsLoadedFlag)"

	m_transformOffset = m_reformatOptionsPtr->transformOffset;

	strncpy (m_numeratorStringPtr,
            (CharPtr)&m_reformatOptionsPtr->numeratorString[1],
            m_reformatOptionsPtr->numeratorString[0]);
	m_numeratorStringPtr[m_reformatOptionsPtr->numeratorString[0]] = 0;

	strncpy (m_denominatorStringPtr,
            (CharPtr)&m_reformatOptionsPtr->denominatorString[1],
            m_reformatOptionsPtr->denominatorString[0]);
	m_denominatorStringPtr[m_reformatOptionsPtr->denominatorString[0]] = 0;

	m_transformFactor = m_reformatOptionsPtr->transformFactor;
		
	m_defaultThermalChannel = m_reformatOptionsPtr->defaultThermalChannel;
	m_radianceMult = m_reformatOptionsPtr->algebraicTransformRadianceMult;
	m_radianceOffset = m_reformatOptionsPtr->algebraicTransformRadianceOffset;
	m_thermalK1 = m_reformatOptionsPtr->algebraicTransformK1Value;
	m_thermalK2 = m_reformatOptionsPtr->algebraicTransformK2Value;

			// Function of channels items

	m_functionCode = m_reformatOptionsPtr->functionCode - 1;
	m_functionFactor = m_reformatOptionsPtr->functionFactor;
	m_kthSmallestElement = m_reformatOptionsPtr->kthSmallestValue;

	ShowHideFunctionChannelsItems (dialogPtr,
												m_transformCode == kFunctionOfChannels,
												m_reformatOptionsPtr->functionCode);

	if (m_transformCode == kFunctionOfChannels)
		{
		selectedItem = IDC_FunctionFactor;
		if (m_functionCode == kFunctionKthSmallestElement - 1)
			selectedItem = IDC_kthSmallestElement;

    	}	// end "if (m_transformCode == kFunctionOfChannels)"

    		// Make certain that all radio buttons are showing for now.

	MShowDialogItem (dialogPtr, IDC_AdjustSelectedChannelsbyChannel);
	MShowDialogItem (dialogPtr, IDC_RT_Eigenvectors);
	MShowDialogItem (dialogPtr, IDC_RT_AlgebraicTransformation);
	MShowDialogItem (dialogPtr, IDC_RT_FunctionOfChannels);

	if (TransferDataToWindow ())
		PositionDialogWindow ();

			// Update the radio buttons.

	if (m_numberEigenvectors <= 0 || m_bandInterleaveSelection == kBSQ)
    	{
		MHideDialogItem (dialogPtr, IDC_RT_Eigenvectors);
		MHideDialogItem (dialogPtr, IDC_RT_EV_PCEigenvectors);
		MHideDialogItem (dialogPtr, IDC_RT_EV_FEEigenvectors);
		
		}	// if (m_numberEigenvectors <= 0 || ...
	
	else	// if (m_numberEigenvectors > 0 || ...
		{
		if (m_eigenSource == 1)
			MHideDialogItem (dialogPtr, IDC_RT_EV_FEEigenvectors);
		
		else if (m_eigenSource == 2)
			MHideDialogItem (dialogPtr, IDC_RT_EV_PCEigenvectors);
		
		}	// end "else if (m_numberEigenvectors > 0 || ..."

	if (gImageFileInfoPtr->thematicType)
		MHideDialogItem (dialogPtr, IDC_RT_AlgebraicTransformation);

    		// Set default text selection to first edit text item

	if (selectedItem > 0)
		SelectDialogItemText (this, selectedItem, 0, SInt16_MAX);

	Fit ();
	
}	// end "OnInitDialog"



void CMReformatTransformDlg::OnRTAlgebraicTransformation (
				wxCommandEvent& 					event)

{
	DialogPtr 							dialogPtr = this;


	ShowHideAdjustChannelItems (dialogPtr, kHide);
	ShowHideAdjustSelectedChannelsByChannelItems (dialogPtr, kHide);
	ShowHidePCTransformItems (dialogPtr, kHide, m_eigenSource);
	ShowHideAlgebraicTransformItems (dialogPtr, kShow);
	ShowHideFunctionChannelsItems (dialogPtr, kHide, 0);

			// Set default text selection to first edit text item

	SelectDialogItemText (this, IDC_RT_AT_Offset, 0, SInt16_MAX);
	m_transformCode = kTransformChannels;
	
}	// end "OnRTAlgebraicTransformation"



void CMReformatTransformDlg::OnRTEigenvectors (
				wxCommandEvent& 					event)

{
	DialogPtr 							dialogPtr = this;


	ShowHideAdjustChannelItems (dialogPtr, kHide);
	ShowHideAdjustSelectedChannelsByChannelItems (dialogPtr, kHide);
	ShowHidePCTransformItems (dialogPtr, kShow, m_eigenSource);
	ShowHideAlgebraicTransformItems (dialogPtr, kHide);
	ShowHideFunctionChannelsItems (dialogPtr, kHide, 0);

			// Set default text selection to first edit text item

	SelectDialogItemText (this, IDC_EV_ScaleFactor, 0, SInt16_MAX);
	m_transformCode = kCreatePCImage;
	
}	// end "OnRTEigenvectors"



void CMReformatTransformDlg::OnRTNoTransformation (
				wxCommandEvent& 					event)

{
	DialogPtr 							dialogPtr = this;


	ShowHideAdjustChannelItems (dialogPtr, kHide);
	ShowHideAdjustSelectedChannelsByChannelItems (dialogPtr, kHide);
	ShowHidePCTransformItems (dialogPtr, kHide, m_eigenSource);
	ShowHideAlgebraicTransformItems (dialogPtr, kHide);
	ShowHideFunctionChannelsItems (dialogPtr, kHide, 0);
	m_transformCode = kNoTransform;
	
}	// end "OnRTNoTransformation"



void CMReformatTransformDlg::OnRTFunctionOfChannels (
				wxCommandEvent& 					event)

{
	DialogPtr 							dialogPtr = this;


	ShowHideAdjustChannelItems (dialogPtr, kHide);
	ShowHideAdjustSelectedChannelsByChannelItems (dialogPtr, kHide);
	ShowHidePCTransformItems (dialogPtr, kHide, m_eigenSource);
	ShowHideAlgebraicTransformItems (dialogPtr, kHide);
	ShowHideFunctionChannelsItems (dialogPtr, kShow, m_functionCode + 1);

			// Set default text selection to first edit text item

	SelectDialogItemText (this, IDC_FunctionFactor, 0, SInt16_MAX);
	m_transformCode = kFunctionOfChannels;
	
}	// end "OnRTFunctionOfChannels"



void CMReformatTransformDlg::OnSelendokAlgebraicTransformOptions (
				wxCommandEvent& 					event)

{
	Boolean 								showFlag = FALSE;


	wxChoice* optionCode = (wxChoice*)FindWindow (IDC_AlgebraicTransformOptions);
	m_algebraicTransformOption = optionCode->GetSelection ();

	if (m_algebraicTransformOption >= kAlgebraicTransformThermal_K)
		showFlag = TRUE;
	
	ShowHideAlgebraicTransformThermItems (this, showFlag);
	
}	// end "OnSelendokAlgebraicTransformOptions"



void CMReformatTransformDlg::OnSelendokEVEigenvectors (
				wxCommandEvent& 					event)
				
{
	HandleChannelsMenu (IDC_EV_Eigenvectors,
							 	kUseTransformation,
							 	(SInt16) m_reformatOptionsPtr->numberPCChannels,
							 	2,
							 	TRUE);
	m_transformCode = kCreatePCImage;

}	// end "OnSelendokEVEigenvectors"



void CMReformatTransformDlg::OnSelendokReformatFunctions (
				wxCommandEvent& 					event)

{
	Boolean 								showFlag = FALSE;


	wxChoice* functionCode = (wxChoice*)FindWindow (IDC_ReformatFunctions);
	m_functionCode = functionCode->GetSelection ();

	if (m_functionCode == kFunctionKthSmallestElement - 1)
		showFlag = TRUE;

	ShowHideDialogItem (this, IDC_kthSmallestElementPrompt, showFlag);
	ShowHideDialogItem (this, IDC_kthSmallestElement, showFlag);
	m_transformCode = kFunctionOfChannels;
	
}	// end "OnSelendokReformatFunctions"



void CMReformatTransformDlg::ShowHideAdjustChannelItems (
				DialogPtr 							dialogPtr,
				Boolean 								showFlag)

{
	ShowHideDialogItem (dialogPtr, IDC_RT_AC_Equal, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AC_Offset, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AC_Plus1, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AC_Factor, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AC_OldValue, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AC_Divisor, showFlag);
	
}	// end "ShowHideAdjustChannelItems"



void CMReformatTransformDlg::ShowHideAdjustSelectedChannelsByChannelItems (
				DialogPtr 							dialogPtr,
				Boolean 								showFlag)

{
	ShowHideDialogItem (dialogPtr, IDC_RT_ACbyC_OldValue, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_ACbyC_Factor, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_ACbyC_ChannelPrompt, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_ACbyC_ChannelNumber, showFlag);
	
}	// end "ShowHideAdjustSelectedChannelsByChannelItems"



void CMReformatTransformDlg::ShowHideAlgebraicTransformItems (
				DialogPtr 							dialogPtr,
				Boolean 								showFlag)

{
	ShowHideDialogItem (dialogPtr, IDC_AlgebraicTransformOptions, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AT_Equal, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AT_Offset, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AT_Plus, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AT_Numerator, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AT_Line, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AT_Denominator, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AT_Multiply, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AT_Factor, showFlag);
	
	ShowHideAlgebraicTransformThermItems (dialogPtr, showFlag);
	
}	// end "ShowHideAlgebraicTransformItems"



void CMReformatTransformDlg::ShowHideAlgebraicTransformThermItems (
				DialogPtr 							dialogPtr,
				Boolean 								showFlag)

{
	if (m_algebraicTransformOption == kAlgebraicTransformGeneral && showFlag)
		showFlag = FALSE;
		
	if (showFlag)
		{
		m_AlgebraicEqualSign1->SetLabel (wxT("      Radiance ="));
		m_AT_numereratorTextCtrl->ChangeValue (wxString::Format (wxT("C%d"), m_defaultThermalChannel));
		m_AT_denominatorTextCtrl->ChangeValue (wxT("1"));
		m_AT_factorTextCtrl->ChangeValue (wxString::Format (wxT("%.8f"), m_radianceMult));
		m_AT_offsetTextCtrl->ChangeValue (wxString::Format (wxT("%.8f"), m_radianceOffset));
		
		}	// showFlag
		
	else	// !showFlag
		{
		m_AlgebraicEqualSign1->SetLabel (wxT("               ="));
		m_AT_numereratorTextCtrl->ChangeValue (
										wxString::Format (wxT("%s"), m_numeratorStringPtr));
		m_AT_denominatorTextCtrl->ChangeValue (
										wxString::Format (wxT("%s"), m_denominatorStringPtr));
		m_AT_factorTextCtrl->ChangeValue (wxString::Format (wxT("%.8f"), m_transformFactor));
		m_AT_offsetTextCtrl->ChangeValue (wxString::Format (wxT("%.8f"), m_transformOffset));
		
		}	// end "else !showFlag"
		
	ShowHideDialogItem (this, IDC_RT_AT_Therm_Equal, showFlag);
	ShowHideDialogItem (this, IDC_RT_AT_Therm_K2, showFlag);
	ShowHideDialogItem (this, IDC_RT_AT_Therm_ln, showFlag);
	ShowHideDialogItem (this, IDC_RT_AT_Therm_K1, showFlag);
	ShowHideDialogItem (this, IDC_RT_AT_Therm_TOAr, showFlag);
		
}	// end "ShowHideAlgebraicTransformTempItems"



void CMReformatTransformDlg::ShowHideFunctionChannelsItems (
				DialogPtr 							dialogPtr,
				Boolean 								showFlag,
				UInt16 								functionChannelCode)

{
	ShowHideDialogItem (dialogPtr, IDC_ReformatFunctions, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_FunctionFactorPrompt, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_FunctionFactor, showFlag);
	
	if (functionChannelCode != kFunctionKthSmallestElement)
		showFlag = FALSE;
	
	ShowHideDialogItem (dialogPtr, IDC_kthSmallestElementPrompt, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_kthSmallestElement, showFlag);
	
}	// end "ShowHideFunctionChannelsItems"



void CMReformatTransformDlg::ShowHidePCTransformItems (
				DialogPtr 							dialogPtr,
				Boolean 								showFlag,
				SInt16								eigenSource)

{
	ShowHideDialogItem (dialogPtr, IDC_EV_Eigenvectors, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_EV_NumberBits, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_EV_AllComponents, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_EV_SelComponents, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_ScaleFactorPrompt, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_EV_ScaleFactor, showFlag);

	bSizer103->Layout ();
	
}	// end "ShowHidePCTransformItems"



bool CMReformatTransformDlg::TransferDataFromWindow ()

{
	SInt16 								item,
											numberTerms;
	
	UInt16 								stringID;
	
	
	wxTextCtrl* adjustDivisor = (wxTextCtrl*)FindWindow (IDC_RT_AC_Divisor);
	m_adjustDivisor = wxAtof (adjustDivisor->GetValue());
	
	wxTextCtrl* adjustFactor = (wxTextCtrl*)FindWindow (IDC_RT_AC_Factor);
	m_adjustFactor = wxAtof (adjustFactor->GetValue());
	
	wxTextCtrl* adjustOffset = (wxTextCtrl*)FindWindow (IDC_RT_AC_Offset);
	m_adjustOffset = wxAtof (adjustOffset->GetValue());
	
	wxChoice* optionCode = (wxChoice*)FindWindow (IDC_AlgebraicTransformOptions);
	m_algebraicTransformOption = optionCode->GetSelection ();
	
	m_transformFactor = wxAtof (m_AT_factorTextCtrl->GetValue());
	
	m_transformOffset = wxAtof (m_AT_offsetTextCtrl->GetValue());
	
	wxTextCtrl* denominatorString = (wxTextCtrl*)FindWindow (IDC_RT_AT_Denominator);
	m_denominatorString = denominatorString->GetValue ();
	
	m_numeratorString = m_AT_numereratorTextCtrl->GetValue ();
	
	wxTextCtrl* thermal_K2 = (wxTextCtrl*)FindWindow (IDC_RT_AT_Therm_K2);
	m_thermalK2 = wxAtof (thermal_K2->GetValue());
	
	wxTextCtrl* thermal_K1 = (wxTextCtrl*)FindWindow (IDC_RT_AT_Therm_K1);
	m_thermalK1 = wxAtof (thermal_K1->GetValue());
	
	wxTextCtrl* scaleFactor = (wxTextCtrl*)FindWindow (IDC_EV_ScaleFactor);
	m_scaleFactor = wxAtof (scaleFactor->GetValue());
	
	wxTextCtrl* functionFactor = (wxTextCtrl*)FindWindow (IDC_FunctionFactor);
	m_functionFactor = wxAtof (functionFactor->GetValue());
	
	wxTextCtrl* kthSmallestElement = (wxTextCtrl*)FindWindow (IDC_kthSmallestElement);
	m_kthSmallestElement = wxAtoi (kthSmallestElement->GetValue());
	
	wxTextCtrl* adjustSelectedChannelsFactor = (wxTextCtrl*)FindWindow (IDC_RT_ACbyC_Factor);
	m_adjustSelectedChannelsFactor = wxAtof (adjustSelectedChannelsFactor->GetValue());
	
	wxTextCtrl* adjustSelectedChannel = (wxTextCtrl*)FindWindow (IDC_RT_ACbyC_ChannelNumber);
	m_adjustSelectedChannel = wxAtoi (adjustSelectedChannel->GetValue());
	
	m_channelSelection = m_channelsCtrl->GetSelection ();
	
	wxChoice* functionCode = (wxChoice*)FindWindow (IDC_ReformatFunctions);
	m_functionCode = functionCode->GetSelection ();
	
			// the value of kFunctionOfChannels = 4
	
	if (m_transformCode == kTransformChannels)
		{
		item = 0;
		
		strncpy (m_denominatorStringPtr,
					(const char*)m_denominatorString.mb_str (wxConvUTF8),
					254);
		CtoPstring ((UCharPtr)m_denominatorStringPtr,
						(UCharPtr)m_reformatOptionsPtr->denominatorString);
		
		numberTerms = DecodeAlgebraicFormula (
									  (unsigned char*)m_reformatOptionsPtr->denominatorString,
									  m_reformatOptionsPtr->coefficientsPtr,
									  m_reformatOptionsPtr->transformChannelPtr,
									  m_reformatOptionsPtr->transformOperatorPtr);
		
		if (numberTerms >= 0)
			m_reformatOptionsPtr->numberDenominatorTerms = numberTerms;
		
		else		// numberTerms < 0)
			item = IDC_RT_AT_Denominator;
		
				// Decode the numerator
		
		if (item == 0)
			{
			strncpy (m_numeratorStringPtr,
						(const char*)m_numeratorString.mb_str (wxConvUTF8),
						254);
		
			CtoPstring ((UCharPtr)m_numeratorStringPtr,
							(UCharPtr)m_reformatOptionsPtr->numeratorString);
			numberTerms = DecodeAlgebraicFormula (
						  (unsigned char*)m_reformatOptionsPtr->numeratorString,
						  &m_reformatOptionsPtr->coefficientsPtr[
													m_reformatOptionsPtr->numberDenominatorTerms],
						  &m_reformatOptionsPtr->transformChannelPtr[
													m_reformatOptionsPtr->numberDenominatorTerms],
						  &m_reformatOptionsPtr->transformOperatorPtr[
													m_reformatOptionsPtr->numberDenominatorTerms]);
			
			if (numberTerms >= 0)
				m_reformatOptionsPtr->numberNumeratorTerms = numberTerms;
			
			else	// numberTerms < 0)
				item = IDC_RT_AT_Numerator;
			
			}	// end "if (item == 0)"
		
		if (item > 0)
			{
			stringID = IDS_Alert118 + abs (numberTerms) - 1;
			
			DisplayAlert (kErrorAlert2ID,
							  kStopAlert,
							  kAlertStrID,
							  stringID,
							  0,
							  NIL);
			
			}	// end "if (item > 0)"
		
		}	// end "if (m_transformCode == kTransformChannels)"
	
	return true;
	
}	// end "TransferDataFromWindow"



bool CMReformatTransformDlg::TransferDataToWindow ()

{
	wxRadioButton* notransform =
							(wxRadioButton*)FindWindow (IDC_RT_NoTransformation);
	wxRadioButton* adjustSelChan =
							(wxRadioButton*)FindWindow (IDC_AdjustSelectedChannels);
	wxRadioButton* adjustSelChanBySelChan =
							(wxRadioButton*)FindWindow (IDC_AdjustSelectedChannelsbyChannel);
	wxRadioButton* newChanFromEV = (wxRadioButton*)FindWindow (IDC_RT_Eigenvectors);
	wxRadioButton* newChanFromFunc =
							(wxRadioButton*)FindWindow (IDC_RT_FunctionOfChannels);
	wxRadioButton* NewChanFromGAlgebra =
							(wxRadioButton*)FindWindow (IDC_RT_AlgebraicTransformation);
	switch (m_transformCode)
		{
		case kNoTransform:
			notransform->SetValue (true);
			break;
			
		case kAdjustChannel:
			adjustSelChan->SetValue (true);
			break;
			
		case kCreatePCImage:
			newChanFromEV->SetValue (true);
			break;
			
		case kTransformChannels:
			NewChanFromGAlgebra->SetValue (true);
			break;
			
		case kFunctionOfChannels:
			newChanFromFunc->SetValue (true);
			break;
			
		case kAdjustChannelsByChannel:
			adjustSelChanBySelChan->SetValue (true);
			break;
			
		default:
			notransform->SetValue (true);
			break;
			
		}	// end "switch (m_transformCode)"
	
	wxTextCtrl* adjustDivisor = (wxTextCtrl*)FindWindow (IDC_RT_AC_Divisor);
	adjustDivisor->ChangeValue (wxString::Format (wxT("%.1f"), m_adjustDivisor));
	
	wxTextCtrl* adjustFactor = (wxTextCtrl*)FindWindow (IDC_RT_AC_Factor);
	adjustFactor->ChangeValue (wxString::Format (wxT("%.1f"), m_adjustFactor));
	
	wxTextCtrl* adjustOffset = (wxTextCtrl*)FindWindow (IDC_RT_AC_Offset);
	adjustOffset->ChangeValue (wxString::Format (wxT("%.1f"), m_adjustOffset));
	
	wxChoice* optionCode = (wxChoice*)FindWindow (IDC_AlgebraicTransformOptions);
	optionCode->SetSelection (m_algebraicTransformOption);
	
	m_AT_factorTextCtrl->ChangeValue (wxString::Format (wxT("%.8f"), m_transformFactor));
	
	m_AT_offsetTextCtrl->ChangeValue (wxString::Format (wxT("%.8f"), m_transformOffset));
	
	//wxTextCtrl* denominatorString = (wxTextCtrl*)FindWindow (IDC_RT_AT_Denominator);
	m_AT_denominatorTextCtrl->ChangeValue (
										wxString::Format (wxT("%s"), m_denominatorStringPtr));
	
	m_AT_numereratorTextCtrl->ChangeValue (wxString::Format (wxT("%s"), m_numeratorStringPtr));
	
	wxTextCtrl* thermal_K2 = (wxTextCtrl*)FindWindow (IDC_RT_AT_Therm_K2);
	thermal_K2->ChangeValue (wxString::Format (wxT("%.6f"), m_thermalK2));
	
	wxTextCtrl* thermal_K1 = (wxTextCtrl*)FindWindow (IDC_RT_AT_Therm_K1);
	thermal_K1->ChangeValue (wxString::Format (wxT("%.6f"), m_thermalK1));
	
	wxTextCtrl* scaleFactor = (wxTextCtrl*)FindWindow (IDC_EV_ScaleFactor);
	scaleFactor->ChangeValue (wxString::Format (wxT("%.1f"), m_scaleFactor));
	
	wxTextCtrl* functionFactor = (wxTextCtrl*)FindWindow (IDC_FunctionFactor);
	functionFactor->ChangeValue (wxString::Format (wxT("%.1f"), m_functionFactor));
	
	wxTextCtrl* kthSmallestElement = (wxTextCtrl*)FindWindow (IDC_kthSmallestElement);
	kthSmallestElement->ChangeValue (
											wxString::Format (wxT("%i"), m_kthSmallestElement));
	
	wxTextCtrl* adjustSelectedChannelsFactor =
							(wxTextCtrl*)FindWindow (IDC_RT_ACbyC_Factor);
	adjustSelectedChannelsFactor->ChangeValue (
							wxString::Format (wxT("%.1f"), m_adjustSelectedChannelsFactor));
	
	wxTextCtrl* adjustSelectedChannel =
										(wxTextCtrl*)FindWindow (IDC_RT_ACbyC_ChannelNumber);
	adjustSelectedChannel->ChangeValue (
										wxString::Format (wxT("%i"), m_adjustSelectedChannel));
	
	m_channelsCtrl->SetSelection (m_channelSelection);
	
	wxChoice* functionCode = (wxChoice*)FindWindow (IDC_ReformatFunctions);
	functionCode->SetSelection (m_functionCode);
	
	wxStaticText* minimumNumberBits =
										(wxStaticText*)FindWindow (IDC_RT_EV_AllComponents);
	minimumNumberBits->SetLabel (wxString::Format (wxT("%i"), m_minimumNumberBits));
	
	wxStaticText* minSelectedNumberBits = (
										wxStaticText*)FindWindow (IDC_RT_EV_SelComponents);
	minSelectedNumberBits->SetLabel (
										wxString::Format (wxT("%i"), m_minSelectedNumberBits));
	
	return true;
	
}	// end "TransferDataToWindow"
