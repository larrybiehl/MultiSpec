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
//	File:						xEnhanceStatisticsDialog.cpp : class implementation file
//	Class Definition:		xEnhanceStatisticsDialog.h
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
//								CMEnhanceStatisticsDialog class.
//
// Following is template for debugging
/*
	int numberChars = sprintf ((char*)gTextString3,
									 " xEnhanceStatisticsDialog:: (): %s",
									 gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xEnhanceStatisticsDialog.h"

		// Enhance Statistics constants												
	
#define	kUseNoThreshold					1
#define	kUseChiChiThreshold				2
#define	kUsePercentThreshold				3
#define	kUseFarthestPixelThreshold		4



BEGIN_EVENT_TABLE (CMEnhanceStatisticsDialog, CMDialog)
	EVT_BUTTON (IDEntireImage, CMEnhanceStatisticsDialog::ToStatEntireImage)
	EVT_BUTTON (IDSelectedImage, CMEnhanceStatisticsDialog::ToStatSelectedImage)

	EVT_CHECKBOX (IDC_WeightLabeledSamples, CMEnhanceStatisticsDialog::OnWeightLabeledSamples)
	EVT_CHECKBOX (IDC_UseEnhancedStats, CMEnhanceStatisticsDialog::OnUseEnhancedStats)

	EVT_CHOICE (IDC_HardThresholdCombo, CMEnhanceStatisticsDialog::OnSelchangeHardThresholdCombo)
	EVT_CHOICE (IDC_SoftThresholdCombo, CMEnhanceStatisticsDialog::OnSelchangeSoftThresholdCombo)

	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_ClassCombo, CMEnhanceStatisticsDialog::OnSelendokStatClassCombo)
		EVT_COMBOBOX (IDC_WeightCombo, CMEnhanceStatisticsDialog::OnClassWeightsComboSelendok)
	#endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_ClassCombo, CMEnhanceStatisticsDialog::OnSelendokStatClassCombo)
		EVT_CHOICE (IDC_WeightCombo, CMEnhanceStatisticsDialog::OnClassWeightsComboSelendok)
	#endif


	EVT_COMBOBOX_CLOSEUP (IDC_ClassCombo, CMEnhanceStatisticsDialog::OnClassComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_WeightCombo, CMEnhanceStatisticsDialog::OnClassWeightComboCloseUp)

	EVT_COMBOBOX_DROPDOWN (IDC_ClassCombo, CMEnhanceStatisticsDialog::OnClassComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_WeightCombo, CMEnhanceStatisticsDialog::OnClassWeightComboDropDown)

	EVT_INIT_DIALOG (CMEnhanceStatisticsDialog::OnInitDialog)

	EVT_TEXT (IDC_ColumnEnd, CMEnhanceStatisticsDialog::CheckStatColumnEnd)
	EVT_TEXT (IDC_ColumnStart, CMEnhanceStatisticsDialog::CheckStatColumnStart)
	EVT_TEXT (IDC_ColumnInterval, CMEnhanceStatisticsDialog::CheckStatColumnInterval)
	EVT_TEXT (IDC_LineEnd, CMEnhanceStatisticsDialog::CheckStatLineEnd)
	EVT_TEXT (IDC_LineStart, CMEnhanceStatisticsDialog::CheckStatLineStart)
	EVT_TEXT (IDC_LineInterval, CMEnhanceStatisticsDialog::CheckStatLineInterval)
	EVT_TEXT (IDC_HardChiChiThreshold, CMEnhanceStatisticsDialog::OnChangeHardChiChiThreshold)
	EVT_TEXT (IDC_HardPercentThreshold, CMEnhanceStatisticsDialog::OnChangeHardPercentThreshold)
	EVT_TEXT (IDC_SoftPercentThreshold, CMEnhanceStatisticsDialog::OnChangeSoftPercentThreshold)
	EVT_TEXT (IDC_SoftChiChiThreshold, CMEnhanceStatisticsDialog::OnChangeSoftChiChiThreshold)
END_EVENT_TABLE()



CMEnhanceStatisticsDialog::CMEnhanceStatisticsDialog (
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title,
				const wxPoint& 					pos,
				const wxSize& 						size,
				long 									style)
		: CMDialog (CMEnhanceStatisticsDialog::IDD, pParent, title)

{
   m_logLikeStopPercent = 1;
	m_iterationStopLength = 1;
	m_iterationMax = 1;
	m_labelWeight = 1;
	m_useEnhancedStatisticsFlag = FALSE;
	m_weightLabeledFlag = FALSE;
	m_classWeightsSelection = 0;
	m_softChiChiThreshold = 0.0;
	m_hardChiChiThreshold = 0.0;
	m_softPercentThreshold = 0.0;
	m_hardPercentThreshold = 0.0;
	m_softThresholdCode = -1;
	m_hardThresholdCode = -1;
   saveSoftThresholdSelection = -1;
   
   m_classWeightSet = 0;

	m_updatingThresholdItemsFlag = TRUE;
	
	m_initializedFlag = CMDialog::m_initializedFlag; 
   
	if (m_initializedFlag)
		m_initializedFlag = GetDialogLocalVectors (NULL,
																	NULL,
																	&m_classListPtr,
																	NULL,
																	&m_classWeightsPtr,
																	NULL,
																	NULL,
																	NULL);
   
   CreateControls ();
   
}	// end "CMEnhanceStatisticsDialog"



CMEnhanceStatisticsDialog::~CMEnhanceStatisticsDialog (void)

{  
	ReleaseDialogLocalVectors (NULL,
										NULL,
										m_classListPtr,
										NULL,
										m_classWeightsPtr,
										NULL,
										NULL,
										NULL);

}	// end "~CMEnhanceStatisticsDialog"



void  CMEnhanceStatisticsDialog::CheckStatColumnEnd (
				wxCommandEvent& 					event)

{
	if (!m_settingSelectedEntireButton)
		{
		CMDialog::CheckColumnEnd (event);
		
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
		
		}	// end "if (!m_settingSelectedEntireButton)"
	
}	// end "CheckColumnEnd"



void CMEnhanceStatisticsDialog::CheckStatColumnInterval (
				wxCommandEvent& 					event)

{
	CMDialog::CheckColumnInterval (event);
	
	m_dialogSelectArea.columnInterval = m_ColumnInterval;
	UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
	
}	// end "CheckStatColumnInterval"



void  CMEnhanceStatisticsDialog::CheckStatColumnStart (
				wxCommandEvent& 					event)

{
	if (!m_settingSelectedEntireButton)
		{
		CMDialog::CheckColumnStart (event);
		
		m_dialogSelectArea.columnStart = m_ColumnStart;
		UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
		
		}	// end "if (!m_settingSelectedEntireButton)"
	
}	// end "CheckStatColumnStart"



void CMEnhanceStatisticsDialog::CheckStatLineEnd (
				wxCommandEvent& 					event)

{
	if (!m_settingSelectedEntireButton)
		{
		CMDialog::CheckLineEnd (event);
		
		m_dialogSelectArea.lineEnd = m_LineEnd;
		UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
		
		}	// end "if (!m_settingSelectedEntireButton)"
	
}	// end "CheckStatLineEnd"



void CMEnhanceStatisticsDialog::CheckStatLineInterval (
				wxCommandEvent& 					event)

{
	CMDialog::CheckLineInterval (event);
	
	m_dialogSelectArea.lineInterval = m_LineInterval;
	UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
	
}	// end "CheckStatLineInterval"



void CMEnhanceStatisticsDialog::CheckStatLineStart (
				wxCommandEvent& 					event)

{
	if (!m_settingSelectedEntireButton)
		{
		CMDialog::CheckLineStart (event);
		
		m_dialogSelectArea.lineStart = m_LineStart;
		UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
		
		}	// end "if (!m_settingSelectedEntireButton)"
	
}	// end "CheckLineStart"



void CMEnhanceStatisticsDialog::CreateControls ()

{
	SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	bSizer119 = new wxBoxSizer (wxVERTICAL);
	
	fgSizer15 = new wxFlexGridSizer (0, 2, 0, 0);
	fgSizer15->SetFlexibleDirection (wxHORIZONTAL);
	fgSizer15->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	wxBoxSizer* bSizerL15;
	bSizerL15 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer247;
	bSizer247 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText249 = new wxStaticText (this,
													wxID_ANY,
													wxT("Unlabeled samples:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	bSizer247->Add (m_staticText249, 0, wxALIGN_CENTER_VERTICAL, 0);
	
	m_textCtrl170 = new wxStaticText (this,
													IDC_UnlabeledSamples,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_textCtrl170, IDS_ToolTip121);
	bSizer247->Add (m_textCtrl170, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	
	bSizerL15->Add (bSizer247, 0, wxEXPAND|wxBOTTOM, 5);
	
	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer (new wxStaticBox (this,
																		IDC_LineColFrame,
																		wxT("Unlabeled Samples Area"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL),
												wxHORIZONTAL);
	
	CreateLineColumnControls (sbSizer8);

	bSizerL15->Add (sbSizer8, wxSizerFlags(1).Expand().Border(wxTOP|wxBOTTOM,5));
	
	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer (new wxStaticBox (this,
																		wxID_ANY,
																		wxT("Unlabeled Samples Options"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL),
													wxVERTICAL);
	
	m_checkBox58 = new wxCheckBox (
							sbSizer9->GetStaticBox(),
							IDC_UseEnhancedStats,
							wxT("Use enhanced statistics for initial conditions             "),
							wxDefaultPosition,
							wxDefaultSize,
							0);
   SetUpToolTip (m_checkBox58, IDS_ToolTip122);
	sbSizer9->Add (m_checkBox58, wxSizerFlags(0).Expand().Border(wxLEFT,5));
	
	wxBoxSizer* bSizer3001;
	bSizer3001 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText3071 = new wxStaticText (sbSizer9->GetStaticBox(),
														wxID_ANY,
														wxT("Outlier thresholds"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	bSizer3001->Add (m_staticText3071, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	
	m_staticText3081 = new wxStaticText (sbSizer9->GetStaticBox(),
														wxID_ANY,
														wxT("Chi*Chi"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	bSizer3001->Add (m_staticText3081, 0, wxLEFT, 80);
	
	sbSizer9->Add (bSizer3001, wxSizerFlags(0).Expand().Border(wxTOP|wxBOTTOM,5));
	
	wxBoxSizer* bSizer30011;
	bSizer30011 = new wxBoxSizer (wxHORIZONTAL);
	m_staticText30711 = new wxStaticText (sbSizer9->GetStaticBox(),
														wxID_ANY,
														wxT("Soft:"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	bSizer30011->Add (m_staticText30711, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 20);
	
	m_softThresholdCtrl = new wxChoice (sbSizer9->GetStaticBox(),
												IDC_SoftThresholdCombo,
												//wxT("Labeled pixel"),
												wxDefaultPosition,
												wxDefaultSize);
												//0,
												//NULL,
												//wxCB_READONLY);
	m_softThresholdCtrl->Append ("None");
	m_softThresholdCtrl->Append ("Chi*Chi");
	m_softThresholdCtrl->Append ("Percent");
	m_softThresholdCtrl->Append ("Labeled pixel");
   SetUpToolTip (m_softThresholdCtrl, IDS_ToolTip123);
	bSizer30011->Add (m_softThresholdCtrl, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 10);
	
	m_textCtrl184 = new wxTextCtrl (sbSizer9->GetStaticBox(),
												IDC_SoftChiChiThreshold,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   m_textCtrl184->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
   																&m_softChiThresString));
   SetUpToolTip (m_textCtrl184, IDS_ToolTip124);
	bSizer30011->Add (m_textCtrl184, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	
	sbSizer9->Add (bSizer30011, wxSizerFlags(0).Expand().Border(wxTOP|wxBOTTOM,5));
	
	wxBoxSizer* bSizer30012;
	bSizer30012 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText30712 = new wxStaticText (sbSizer9->GetStaticBox(),
														IDC_SoftPercentThresholdPrompt,
														wxT("Percent:"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	SetUpToolTip (m_staticText30712, IDS_ToolTip125);
   bSizer30012->Add (m_staticText30712,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   											Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT,40));
	
	m_textCtrl185 = new wxTextCtrl (sbSizer9->GetStaticBox(),
												IDC_SoftPercentThreshold,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   m_textCtrl185->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
   																&m_softpercentThresString));
   SetUpToolTip (m_textCtrl185, IDS_ToolTip125);
	bSizer30012->Add (m_textCtrl185,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxLEFT,5));

	sbSizer9->Add (bSizer30012,
						wxSizerFlags(0).Expand().ReserveSpaceEvenIfHidden().
																				Border(wxTOP|wxBOTTOM,5));
	
	wxBoxSizer* bSizer300121;
	bSizer300121 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText307121 = new wxStaticText (sbSizer9->GetStaticBox(),
														wxID_ANY, wxT("Hard:"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	bSizer300121->Add (m_staticText307121, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 20);
	
	m_hardThresholdCtrl = new wxChoice (sbSizer9->GetStaticBox(),
													IDC_HardThresholdCombo,
													//wxT("Percent"),
													wxDefaultPosition,
													wxDefaultSize);
													//0,
													//NULL,
													//wxCB_READONLY);
	m_hardThresholdCtrl->Append ("None");
	m_hardThresholdCtrl->Append ("Chi*Chi");
	m_hardThresholdCtrl->Append ("Percent");
   SetUpToolTip (m_hardThresholdCtrl, IDS_ToolTip126);
	bSizer300121->Add (m_hardThresholdCtrl, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	
	m_textCtrl1851 = new wxTextCtrl (sbSizer9->GetStaticBox(),
												IDC_HardChiChiThreshold,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   m_textCtrl1851->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
   																&m_hardchiThresString));
   SetUpToolTip (m_textCtrl1851, IDS_ToolTip127);
	bSizer300121->Add (m_textCtrl1851, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	
	sbSizer9->Add (bSizer300121, wxSizerFlags(0).Expand().Border(wxTOP|wxBOTTOM, 5));
	
	wxBoxSizer* bSizer3001211;
	bSizer3001211 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText3071211 = new wxStaticText (sbSizer9->GetStaticBox(),
															IDC_HardPercentThresholdPrompt,
															wxT("Percent:"),
															wxDefaultPosition,
															wxDefaultSize,
															0);
	bSizer3001211->Add (m_staticText3071211,
								wxSizerFlags(0).ReserveSpaceEvenIfHidden().
												Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT, 40));
	
	m_textCtrl18511 = new wxTextCtrl (sbSizer9->GetStaticBox(),
													IDC_HardPercentThreshold,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_textCtrl18511, IDS_ToolTip128);
   m_textCtrl18511->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
   																&m_hardpercentThresString));
	bSizer3001211->Add (m_textCtrl18511,
								wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxLEFT,5));

	sbSizer9->Add (bSizer3001211,
						wxSizerFlags(0).Expand().ReserveSpaceEvenIfHidden().
																				Border(wxTOP|wxBOTTOM,5));
	
	bSizerL15->Add (sbSizer9, wxSizerFlags(0).Border (wxTOP, 5));
	
	fgSizer15->Add (bSizerL15, wxSizerFlags(0));
	
	wxBoxSizer* bSizerR15;
	bSizerR15 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer248;
	bSizer248 = new wxBoxSizer (wxHORIZONTAL);
	m_staticText250 = new wxStaticText (this,
													wxID_ANY,
													wxT("Labeled samples:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	SetUpToolTip (m_staticText250, IDS_ToolTip129);
   bSizer248->Add (m_staticText250, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
	
	m_textCtrl171 = new wxStaticText (this,
													IDC_LabeledSamples,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_textCtrl171, IDS_ToolTip129);
	bSizer248->Add (m_textCtrl171,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border (wxLEFT, 5));
	
	bSizerR15->Add (bSizer248, wxSizerFlags(0));
	
	wxBoxSizer* bSizer290;
	bSizer290 = new wxBoxSizer (wxHORIZONTAL);
	m_staticText326 = new wxStaticText (this,
													wxID_ANY,
													wxT("Classes:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	SetUpToolTip (m_staticText326, IDS_ToolTip103);
   bSizer290->Add (m_staticText326,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border (wxLEFT, 10));

	GetAllSubsetMenuControl (kClassesMenu,
										this,
										IDC_ClassCombo,
										120,
										IDS_ToolTip103);
	bSizer290->Add (m_classesCtrl,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border (wxLEFT, 5));
	
	bSizerR15->Add (bSizer290, wxSizerFlags(1).Expand());
	
	wxBoxSizer* bSizer291;
	bSizer291 = new wxBoxSizer (wxHORIZONTAL);
	m_staticText327 = new wxStaticText (this,
													wxID_ANY,
													wxT("Class weights:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	SetUpToolTip (m_staticText327, IDS_ToolTip130);
   bSizer291->Add (m_staticText327,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border (wxLEFT, 10));

	GetEqualUnequalMenuControl (kWeightsMenu,
											this,
											IDC_WeightCombo,
											130,
											IDS_ToolTip130);
	bSizer291->Add (m_weightsCtrl,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border (wxLEFT, 5));
	
	bSizerR15->Add (bSizer291, wxSizerFlags(1).Expand());
	
	wxBoxSizer* bSizer292;
	bSizer292 = new wxBoxSizer (wxHORIZONTAL);
	m_checkBox59 = new wxCheckBox (this,
												IDC_WeightLabeledSamples,
												wxT("Weight labeled samples "),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox59, IDS_ToolTip131);
	bSizer292->Add (m_checkBox59,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT, 10));
	
	bSizerR15->Add (bSizer292, wxSizerFlags(1).Expand());
	
	wxBoxSizer* bSizer168;
	bSizer168 = new wxBoxSizer (wxHORIZONTAL);
	m_staticText188 = new wxStaticText (this,
													IDC_WeightingPrompt,
													wxT("weighting:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	SetUpToolTip (m_staticText188, IDS_ToolTip132);
   bSizer168->Add (m_staticText188,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).
   												ReserveSpaceEvenIfHidden().Border(wxLEFT, 30));
	
	m_textCtrl204 = new wxTextCtrl (this,
												IDC_Weighting,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   m_textCtrl204->SetValidator (wxTextValidator (wxFILTER_NUMERIC, &m_weightString));
   SetUpToolTip (m_textCtrl204, IDS_ToolTip132);
	bSizer168->Add (m_textCtrl204,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).
													ReserveSpaceEvenIfHidden().Border(wxLEFT, 5));
	
	bSizerR15->Add (bSizer168, wxSizerFlags(1).Expand().Border(wxBOTTOM, 5));
	
	wxBoxSizer* bSizer259;
	bSizer259 = new wxBoxSizer (wxVERTICAL);
	m_staticText278 = new wxStaticText (this,
													wxID_ANY,
													wxT("Iteration stop conditions"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	bSizer259->Add (m_staticText278, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 5));
	
	wxFlexGridSizer* fgSizer29;
	fgSizer29 = new wxFlexGridSizer (0, 2, 0, 0);
	fgSizer29->SetFlexibleDirection (wxHORIZONTAL);
	fgSizer29->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	m_staticText279 = new wxStaticText (this,
													wxID_ANY,
													wxT("Maximum number:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	SetUpToolTip (m_staticText279, IDS_ToolTip133);
   fgSizer29->Add (m_staticText279,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT, 20));
	
	m_textCtrl180 = new wxTextCtrl (this,
												IDC_MaximumNumber,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (80,-1),
												0);
   m_textCtrl180->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_maxNumberString));
   SetUpToolTip (m_textCtrl180, IDS_ToolTip133);
	fgSizer29->Add (m_textCtrl180,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).
																		Border(wxLEFT|wxTOP|wxBOTTOM, 5));
	
	m_staticText280 = new wxStaticText (this,
													wxID_ANY,
													wxT("Iteration length:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	SetUpToolTip (m_staticText280, IDS_ToolTip134);
   fgSizer29->Add (m_staticText280,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT, 20));
	
	m_textCtrl181 = new wxTextCtrl (this,
												IDC_IterationLength,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (80,-1),
												0);
   m_textCtrl181->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_iterlengthString));
   SetUpToolTip (m_textCtrl181, IDS_ToolTip134);
	fgSizer29->Add (m_textCtrl181,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).
																		Border(wxLEFT|wxTOP|wxBOTTOM, 5));
	
	m_staticText281 = new wxStaticText (this,
													wxID_ANY,
													wxT("Log like change(%):"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	SetUpToolTip (m_staticText281, IDS_ToolTip135);
   fgSizer29->Add (m_staticText281,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT, 20));
	
	m_textCtrl182 = new wxTextCtrl (this,
												IDC_LogLikeChange,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (80, -1),
												0);
   m_textCtrl182->SetValidator (wxTextValidator (wxFILTER_NUMERIC, &m_loglikeString));
   SetUpToolTip (m_textCtrl182, IDS_ToolTip135);
	fgSizer29->Add (m_textCtrl182,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).
																		Border(wxLEFT|wxTOP|wxBOTTOM, 5));
	
	bSizer259->Add (fgSizer29, wxSizerFlags(0).Border(wxBOTTOM,5));
	
	bSizerR15->Add (bSizer259, wxSizerFlags(0).Expand());

	fgSizer15->Add (bSizerR15, wxSizerFlags(1).Border(wxLEFT,15));
	
	bSizer119->Add (fgSizer15, wxSizerFlags(1).Border(wxLEFT|wxTOP|wxBOTTOM|wxRIGHT,12));
	
	CreateStandardButtons (bSizer119);
	
	SetSizerAndFit (bSizer119);
	
}	// end "CreateControls"



void CMEnhanceStatisticsDialog::OnChangeHardChiChiThreshold (
				wxCommandEvent& 					event)

{
	if (!m_updatingThresholdItemsFlag)
		{
		m_updatingThresholdItemsFlag = TRUE;
		
		if (m_hardThresholdCode+1 != kUseChiChiThreshold)
			{
			m_hardThresholdCode = kUseChiChiThreshold - 1;
			
			StatisticsEnhanceDialogUpdateThresholdItems (this,
																		m_hardThresholdCode + 1,
																		IDC_HardChiChiThreshold,
																		IDC_HardPercentThresholdPrompt,
																		IDC_HardPercentThreshold,
																		m_hardPercentThreshold,
																		&m_hardChiChiThreshold);
			
			m_hardThresholdCtrl->SetSelection (m_hardThresholdCode);
			
			}	// end "if (m_usePercentThresholdFlag && ..."
		
		m_updatingThresholdItemsFlag = FALSE;
		
		}	// end "if (!m_updatingThresholdItemsFlag)"
	
}	// end "OnChangeHardChiChiThreshold"



void CMEnhanceStatisticsDialog::OnChangeHardPercentThreshold (
				wxCommandEvent& 					event)

{
	if (!m_updatingThresholdItemsFlag)
		{
		m_updatingThresholdItemsFlag = TRUE;
		
		wxTextCtrl* hardPercentThreshold =
												(wxTextCtrl*)FindWindow (IDC_HardPercentThreshold);
		hardPercentThreshold->GetValue().ToDouble (&m_hardPercentThreshold);
		
		StatisticsEnhanceDialogUpdateChiSquaredValue (this,
																	 (SInt16)(m_hardThresholdCode+1),
																	 IDC_HardChiChiThreshold,
																	 m_hardPercentThreshold,
																	 &m_hardChiChiThreshold);
		
		m_updatingThresholdItemsFlag = FALSE;
		
		}	// end "if (!m_updatingThresholdItemsFlag)"
	
}	// end "OnChangeHardPercentThreshold"



void CMEnhanceStatisticsDialog::OnChangeSoftChiChiThreshold (
				wxCommandEvent& 					event)

{
	if (!m_updatingThresholdItemsFlag)
		{
		m_updatingThresholdItemsFlag = TRUE;
		
		if (m_softThresholdCode+1 != kUseChiChiThreshold)
			{
			m_softThresholdCode = kUseChiChiThreshold - 1;
			
			StatisticsEnhanceDialogUpdateThresholdItems (this,
																		m_softThresholdCode + 1,
																		IDC_SoftChiChiThreshold,
																		IDC_SoftPercentThresholdPrompt,
																		IDC_SoftPercentThreshold,
																		m_softPercentThreshold,
																		&m_softChiChiThreshold);
			
			m_softThresholdCtrl->SetSelection (m_softThresholdCode);
			
			}	// end "if (m_usePercentThresholdFlag && ..."
		
		m_updatingThresholdItemsFlag = FALSE;
		
		}	// end "if (!m_updatingThresholdItemsFlag)"
	
}	// end "OnChangeSoftChiChiThreshold"



void CMEnhanceStatisticsDialog::OnChangeSoftPercentThreshold (
				wxCommandEvent& 					event)

{
	if (!m_updatingThresholdItemsFlag)
		{
		m_updatingThresholdItemsFlag = TRUE;
		wxTextCtrl* softPercentThreshold =
											(wxTextCtrl*)FindWindow (IDC_SoftPercentThreshold);
		softPercentThreshold->GetValue().ToDouble (&m_softPercentThreshold);
		
		StatisticsEnhanceDialogUpdateChiSquaredValue (this,
																	 m_softThresholdCode+1,
																	 IDC_SoftChiChiThreshold,
																	 m_softPercentThreshold,
																	 &m_softChiChiThreshold);
		
		m_updatingThresholdItemsFlag = FALSE;
		
		}	// end "if (!m_updatingThresholdItemsFlag)"
	
}	// end "OnChangeSoftPercentThreshold"



SInt16 CMEnhanceStatisticsDialog::DoDialog (
				StatEnhanceSpecsPtr				statEnhanceSpecsPtr)

{
	SInt16								returnCode;
	
	Boolean								continueFlag = FALSE;
	
	
			// Make sure intialization has been completed.
	
	if (!m_initializedFlag)
																							return (FALSE);
	
	m_statEnhanceSpecsPtr = statEnhanceSpecsPtr;
	
	returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{
		continueFlag = TRUE;
		
		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;
		
		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;
		
		StatisticsEnhanceDialogOK (m_statEnhanceSpecsPtr,
											&m_dialogSelectArea,
											m_classListPtr,
											m_classSelection,
											(SInt16)m_localNumberClasses,
											m_useEnhancedStatisticsFlag,
											m_classWeightsSelection+1,
											m_classWeightsPtr,
											m_softThresholdCode+1,
											m_softChiChiThreshold,
											m_softPercentThreshold,
											m_hardThresholdCode+1,
											m_hardChiChiThreshold,
											m_hardPercentThreshold,
											m_weightLabeledFlag,
											m_labelWeight,
											m_iterationMax,
											m_iterationStopLength,
											m_logLikeStopPercent);
		
		}	// end "if (returnCode == wxID_OK)"
	
	return continueFlag;
	
}	// end "DoDialog"



void CMEnhanceStatisticsDialog::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	SInt16      						classSelection,
											hardThresholdSelection,
											softThresholdSelection,
											weightsSelection;
   
   
   wxDialog::OnInitDialog (event);
   
   		// Initialize dialog variables.
   
	m_updatingThresholdItemsFlag = TRUE;
	StatisticsEnhanceDialogInitialize (this,
													m_statEnhanceSpecsPtr,
													&m_dialogSelectArea,
													m_classListPtr,
													&classSelection,
													&m_localNumberClasses,
													&m_useEnhancedStatisticsFlag,
													&m_classWeightSet,
													&weightsSelection,
													&softThresholdSelection,
													&m_softChiChiThreshold,
													&m_softPercentThreshold,
													&hardThresholdSelection,
													&m_hardChiChiThreshold,
													&m_hardPercentThreshold,
													&m_weightLabeledFlag,
													&m_labelWeight,
													&m_iterationMax,
													&m_iterationStopLength,
													&m_logLikeStopPercent);
   
			// Selected area to use for non-labeled samples.
                  
	m_LineStart = m_dialogSelectArea.lineStart;
	m_LineEnd = m_dialogSelectArea.lineEnd;
	m_LineInterval = m_dialogSelectArea.lineInterval;
	m_ColumnStart = m_dialogSelectArea.columnStart;
	m_ColumnEnd = m_dialogSelectArea.columnEnd;
	m_ColumnInterval = m_dialogSelectArea.columnInterval; 	
						
			// Classes to use.
			
	m_classSelection = classSelection;
	
			// Class weights. Allow for 0 base, not 1 base as for Mac version				 
	
	m_classWeightsSelection = weightsSelection - 1;

			// Threshold codes.
   
	m_hardThresholdCode = hardThresholdSelection - 1;
   m_softThresholdCode = softThresholdSelection - 1;
	
	if (TransferDataToWindow ())
		{
				// Now allow changes from the user for threshold items.
				
		m_updatingThresholdItemsFlag = FALSE;
      
				// Set default text selection to first edit text item

		SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);
		
		SetSizerAndFit (bSizer119);
		Fit ();
		PositionDialogWindow ();
		
		}	// end "if (TransferDataToWindow ())"

}	// end "OnInitDialog"



void  CMEnhanceStatisticsDialog::OnSelchangeHardThresholdCombo (
				wxCommandEvent& 					event)

{
   m_hardThresholdCode = m_hardThresholdCtrl->GetSelection ();

	m_updatingThresholdItemsFlag = TRUE;

	StatisticsEnhanceDialogUpdateThresholdItems (this,
																(SInt16)(m_hardThresholdCode+1),
																IDC_HardChiChiThreshold,
																IDC_HardPercentThresholdPrompt,
																IDC_HardPercentThreshold,
																m_hardPercentThreshold,
																&m_hardChiChiThreshold);
   
	m_updatingThresholdItemsFlag = FALSE;
	
}	// end "OnSelchangeHardThresholdCombo"



void  CMEnhanceStatisticsDialog::OnSelchangeSoftThresholdCombo (
				wxCommandEvent& 					event)

{
    m_softThresholdCode = m_softThresholdCtrl->GetSelection ();
   
   m_updatingThresholdItemsFlag = TRUE;
   
	StatisticsEnhanceDialogUpdateThresholdItems (this,
																(SInt16)(m_softThresholdCode+1),
																IDC_SoftChiChiThreshold,
																IDC_SoftPercentThresholdPrompt,
																IDC_SoftPercentThreshold,
																m_softPercentThreshold,
																&m_softChiChiThreshold);
   
   wxTextCtrl* softChiChiThreshold = (wxTextCtrl*)FindWindow (IDC_SoftChiChiThreshold);
   softChiChiThreshold->GetValue().ToDouble (&m_softChiChiThreshold);
   
   m_updatingThresholdItemsFlag = FALSE;   
   
}	// end "OnSelchangeSoftThresholdCombo"



void CMEnhanceStatisticsDialog::OnClassWeightsComboSelendok (
				wxCommandEvent& 					event)

{                                                           
	HandleClassWeightsMenu ((SInt16)m_localNumberClasses, 
									(SInt16*)m_classListPtr,
									m_classWeightsPtr,
									gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
									IDC_WeightCombo,
									&m_classWeightsSelection);
	
}	// end "OnClassWeightsComboSelendok"



void CMEnhanceStatisticsDialog::OnSelendokStatClassCombo (
				wxCommandEvent& 					event)

{
	SInt16								classSelection;
	
	
	CMDialog::OnClassComboSelendok (event);
	
	if (m_classSelection == kAllMenuItem)
		{
				// Make sure that the class settings are setup for all
				// classes in case they will be used by the symbols
				// or weights popup menu.
				
		classSelection = m_classSelection;
										
		LoadTrainClassVector (&m_localNumberClasses,
										&classSelection, 
										(SInt16*)m_classListPtr);
										
		m_classSelection = classSelection;
										
		}	// end "if (gClassSelection == kAllMenuItem)"
	
	UpdateNumberLabeledSamples (this,
											m_classSelection,
											(SInt16*)m_classListPtr,
											m_localNumberClasses);
	
}	// end "OnSelendokStatClassCombo"



void CMEnhanceStatisticsDialog::OnUseEnhancedStats (
				wxCommandEvent& 					event)

{	
	wxCheckBox* useEnhancedStatisticsFlag =
												(wxCheckBox*)FindWindow (IDC_UseEnhancedStats);
	
	m_useEnhancedStatisticsFlag = useEnhancedStatisticsFlag->GetValue ();
	
	m_classWeightsSelection = UpdateDialogClassWeightsInfo (
																		(SInt16)m_classWeightsSelection,
																		m_useEnhancedStatisticsFlag,
																		&m_classWeightSet,
																		FALSE);
	
	m_classWeightsSelection = m_weightsCtrl->GetSelection ();
		
}	// end "OnUseEnhancedStats"



void CMEnhanceStatisticsDialog::OnWeightLabeledSamples (
				wxCommandEvent& 					event)

{
	
	wxCheckBox* weightLabeledFlag = (wxCheckBox*)FindWindow (IDC_WeightLabeledSamples);
	m_weightLabeledFlag = weightLabeledFlag->GetValue();
	
	ShowHideDialogItem (this, IDC_WeightingPrompt, m_weightLabeledFlag);
	ShowHideDialogItem (this, IDC_Weighting, m_weightLabeledFlag);
	
}	// end "OnWeightLabeledSamples"



void CMEnhanceStatisticsDialog::ToStatEntireImage (
				wxCommandEvent& 					event)

{
	CMDialog::ToEntireImage (event);
	
	m_dialogSelectArea.lineStart = m_LineStart;
	m_dialogSelectArea.lineEnd = m_LineEnd;
	m_dialogSelectArea.columnStart = m_ColumnStart;
	m_dialogSelectArea.columnEnd = m_ColumnEnd;
	UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
	
}	// end "ToEntireImage"



void  CMEnhanceStatisticsDialog::ToStatSelectedImage (
				wxCommandEvent& 					event)

{
	CMDialog::ToSelectedImage (event);
	
	m_dialogSelectArea.lineStart = m_LineStart;
	m_dialogSelectArea.lineEnd = m_LineEnd;
	m_dialogSelectArea.columnStart = m_ColumnStart;
	m_dialogSelectArea.columnEnd = m_ColumnEnd;
	UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
	
}	// end "ToSelectedImage"


bool CMEnhanceStatisticsDialog::TransferDataFromWindow ()

{
	wxString 							m_softChiChiThresholdString,
		 									m_softPercentThresholdString,
		 									m_hardChiChiThresholdString,
		 									m_hardPercentThresholdString,
		 									m_labelWeightString,
		 									m_iterationMaxString,
		 									m_iterationStopLengthString,
		 									m_logLikeStopPercentString;
	
	SInt16								returnCode = 0;
	
	
	TransferLinesColumnsFromWindow ();

	wxCheckBox* useEnhancedStatisticsFlag =
													(wxCheckBox*)FindWindow (IDC_UseEnhancedStats);
	m_useEnhancedStatisticsFlag = useEnhancedStatisticsFlag->GetValue();
	
	m_softThresholdCode = m_softThresholdCtrl->GetSelection();
	
	wxTextCtrl* softChiChiThreshold = (wxTextCtrl*)FindWindow (IDC_SoftChiChiThreshold);
	m_softChiChiThresholdString = softChiChiThreshold->GetValue();
	if (m_softThresholdCode == 1)
		m_softChiChiThreshold = wxAtof (m_softChiChiThresholdString);
	
	wxTextCtrl* softPercentThreshold =
											(wxTextCtrl*)FindWindow (IDC_SoftPercentThreshold);
	m_softPercentThresholdString = softPercentThreshold->GetValue();
	m_softPercentThreshold = wxAtof (m_softPercentThresholdString);
	
	m_hardThresholdCode = m_hardThresholdCtrl->GetSelection ();
	wxTextCtrl* hardChiChiThreshold = (wxTextCtrl*)FindWindow (IDC_HardChiChiThreshold);
	m_hardChiChiThresholdString = hardChiChiThreshold->GetValue ();
	if (m_hardThresholdCode == 1)
		m_hardChiChiThreshold = wxAtof (m_hardChiChiThresholdString);
		
	wxTextCtrl* hardPercentThreshold =
											(wxTextCtrl*)FindWindow (IDC_HardPercentThreshold);
	m_hardPercentThresholdString = hardPercentThreshold->GetValue();
	m_hardPercentThreshold = wxAtof (m_hardPercentThresholdString);
	
	m_classSelection = m_classesCtrl->GetSelection();
	m_classWeightsSelection = m_weightsCtrl->GetSelection();
   
	wxCheckBox* weightLabeledFlag = (wxCheckBox*)FindWindow (IDC_WeightLabeledSamples);
	m_weightLabeledFlag = weightLabeledFlag->GetValue();
	
	wxTextCtrl* labelWeight = (wxTextCtrl*)FindWindow (IDC_Weighting);
	m_labelWeightString = labelWeight->GetValue();
	m_labelWeight = wxAtof (m_labelWeightString);
	
	wxTextCtrl* iterationMax = (wxTextCtrl*)FindWindow (IDC_MaximumNumber);
	m_iterationMaxString = iterationMax->GetValue();
	m_iterationMax = wxAtoi (m_iterationMaxString);
	
	wxTextCtrl* iterationStopLength = (wxTextCtrl*)FindWindow (IDC_IterationLength);
	m_iterationStopLengthString = iterationStopLength->GetValue();
	m_iterationStopLength = wxAtoi (m_iterationStopLengthString);
	
	wxTextCtrl* logLikeStopPercent = (wxTextCtrl*)FindWindow (IDC_LogLikeChange);
	m_logLikeStopPercentString = logLikeStopPercent->GetValue();
	m_logLikeStopPercent = wxAtof (m_logLikeStopPercentString);
	
   returnCode = CheckDialogRealValue (this,
													IDC_LogLikeChange,
													1.e-006, 
                                       10.,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);

   if (returnCode == 0)
		returnCode = CheckDialogRealValue (this,
														IDC_IterationLength,
														1,
														200,
														0,
														true,		// minValue is allowed.
														true,		// maxValue is allowed,
														kDisplayRangeAlert);
	if (returnCode == 0)
		returnCode = CheckDialogRealValue (this,
														IDC_Weighting,
														1.e-005,
														1000.,
														0,
														true,		// minValue is allowed.
														true,		// maxValue is allowed,
														kDisplayRangeAlert);
	if (returnCode == 0)
		returnCode = CheckDialogRealValue (this,
														IDC_SoftPercentThreshold,
														0.,
														100.,
														0,
														true,		// minValue is allowed.
														true,		// maxValue is allowed,
														kDisplayRangeAlert);
	if (returnCode == 0)
		returnCode = CheckDialogRealValue (this,
														IDC_HardPercentThreshold,
														0.,
														100.,
														0,
														true,		// minValue is allowed.
														true,		// maxValue is allowed,
														kDisplayRangeAlert);
 
	if (returnCode == 0)
		returnCode = VerifyLineColumnValues (IDC_LineStart,
															IDC_ColumnStart,
															true);
													
	return (returnCode == 0);
	
}	// end "TransferDataFromWindow"



bool CMEnhanceStatisticsDialog::TransferDataToWindow ()

{
	SInt16								returnCode = TRUE;
	
	
	TransferLinesColumnsToWindow ();

	wxTextCtrl* softChiChiThreshold =
										(wxTextCtrl*)FindWindow (IDC_SoftChiChiThreshold);
	softChiChiThreshold->SetValue (
										wxString::Format (wxT("%.4f"), m_softChiChiThreshold));
	
	wxTextCtrl* softPercentThreshold =
										(wxTextCtrl*)FindWindow (IDC_SoftPercentThreshold);
	softPercentThreshold->SetValue (
										wxString::Format (wxT("%.g"), m_softPercentThreshold));
	
	wxTextCtrl* hardChiChiThreshold =
										(wxTextCtrl*)FindWindow (IDC_HardChiChiThreshold);
	hardChiChiThreshold->SetValue (
										wxString::Format (wxT("%.4f"), m_hardChiChiThreshold));
	
	wxTextCtrl* hardPercentThreshold =
										(wxTextCtrl*)FindWindow (IDC_HardPercentThreshold);
	hardPercentThreshold->SetValue (
										wxString::Format (wxT("%.g"), m_hardPercentThreshold));
	
	wxTextCtrl* labelWeight = (wxTextCtrl*)FindWindow (IDC_Weighting);
	labelWeight->SetValue (wxString::Format (wxT("%.2f"), m_labelWeight));
	
	wxTextCtrl* iterationMax = (wxTextCtrl*)FindWindow (IDC_MaximumNumber);
	iterationMax->SetValue (wxString::Format (wxT("%d"), m_iterationMax));
	
	wxTextCtrl* iterationStopLength = (wxTextCtrl*)FindWindow (IDC_IterationLength);
	iterationStopLength->SetValue (wxString::Format (wxT("%d"), m_iterationStopLength));
	
	wxTextCtrl* logLikeStopPercent = (wxTextCtrl*)FindWindow (IDC_LogLikeChange);
	logLikeStopPercent->SetValue (wxString::Format (wxT("%.g"), m_logLikeStopPercent));
	
	m_classesCtrl->SetSelection (m_classSelection);
	m_weightsCtrl->SetSelection (m_classWeightsSelection);
	m_softThresholdCtrl->SetSelection (m_softThresholdCode);
	m_hardThresholdCtrl->SetSelection (m_hardThresholdCode);
	
	wxCheckBox* useEnhancedStatisticsFlag =
													(wxCheckBox*)FindWindow (IDC_UseEnhancedStats);
	useEnhancedStatisticsFlag->SetValue (m_useEnhancedStatisticsFlag);
	
	wxCheckBox* weightLabeledFlag = (wxCheckBox*)FindWindow (IDC_WeightLabeledSamples);
	weightLabeledFlag->SetValue (m_weightLabeledFlag);
	
	return (returnCode);
	
}	// end "TransferDataToWindow"
