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
//	File:						xClassifyDialog.cpp : class implementation file
//	Class Definition:		xClassifyDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			05/28/2020
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMClassifyDialog class.
//
/* Template for debugging
	int numberChars = sprintf ((char*)gTextString3,
				" xClassifyDialog:: (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xClassifyDialog.h"
#include "wx/valnum.h"



BEGIN_EVENT_TABLE (CMClassifyDialog, CMDialog)
	//EVT_ACTIVATE (CMClassifyDialog::OnActivate)

	EVT_BUTTON (IDC_ListOptions, CMClassifyDialog::OnListOptions)
	EVT_BUTTON (IDEntireImage, CMClassifyDialog::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMClassifyDialog::ToSelectedImage)

	EVT_CHECKBOX (IDC_DiskFile, CMClassifyDialog::OnDiskFile)
	EVT_CHECKBOX (IDC_ThresholdResults, CMClassifyDialog::OnThresholdResults)
	EVT_CHECKBOX (IDC_TextWindow, CMClassifyDialog::OnTextWindow)
	EVT_CHECKBOX (IDC_Training, CMClassifyDialog::OnTraining)
	EVT_CHECKBOX (IDC_TestAreas, CMClassifyDialog::OnTestAreas)
	EVT_CHECKBOX (IDC_ImageArea, CMClassifyDialog::OnImageArea)
	EVT_CHECKBOX (IDC_FeatureTransformation, CMClassifyDialog::OnFeatureTransformation)
	EVT_CHECKBOX (IDC_CreateProbabilityFile, CMClassifyDialog::OnCreateProbabilityFile)
	EVT_CHECKBOX (IDC_TrainingLOO, CMClassifyDialog::OnTrainingLOO)
	EVT_CHECKBOX (IDC_ImageWindowOverlay, CMClassifyDialog::OnImageOverlay)

	EVT_CHOICE (IDC_DiskCombo, CMClassifyDialog::OnDiskComboSelendok)
	EVT_CHOICE (IDC_ImageOverlayCombo, CMClassifyDialog::OnImageOverlayComboSelendok)
	EVT_CHOICE (IDC_TargetCombo, CMClassifyDialog::OnTargetComboSelendok)

	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_AreasCombo, CMClassifyDialog::OnAreasComboSelendok)
		EVT_COMBOBOX (IDC_ChannelCombo, CMClassifyDialog::OnChannelComboSelendok)
		EVT_COMBOBOX (IDC_ClassCombo, CMClassifyDialog::OnClassComboSelendok)
		EVT_COMBOBOX (IDC_ClassificationProcedure, CMClassifyDialog::OnClassificationProcedureSelendok)
		EVT_COMBOBOX (IDC_PaletteCombo, CMClassifyDialog::OnPaletteComboSelendok)
		EVT_COMBOBOX (IDC_WeightCombo, CMClassifyDialog::OnClassWeightsComboSelendok)
	#endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_AreasCombo, CMClassifyDialog::OnAreasComboSelendok)
		EVT_CHOICE (IDC_ChannelCombo, CMClassifyDialog::OnChannelComboSelendok)
		EVT_CHOICE (IDC_ClassCombo, CMClassifyDialog::OnClassComboSelendok)
		EVT_CHOICE (IDC_ClassificationProcedure, CMClassifyDialog::OnClassificationProcedureSelendok)
		EVT_CHOICE (IDC_PaletteCombo, CMClassifyDialog::OnPaletteComboSelendok)
		EVT_CHOICE (IDC_WeightCombo, CMClassifyDialog::OnClassWeightsComboSelendok)
	#endif

	EVT_COMBOBOX_CLOSEUP (IDC_AreasCombo, CMClassifyDialog::OnAreasComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_ChannelCombo, CMClassifyDialog::OnChannelComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_ClassCombo, CMClassifyDialog::OnClassComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_ClassificationProcedure, CMClassifyDialog::OnClassificationProcedureCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_PaletteCombo, CMClassifyDialog::OnPaletteComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_WeightCombo, CMClassifyDialog::OnClassWeightComboCloseUp)

	EVT_COMBOBOX_DROPDOWN (IDC_AreasCombo, CMClassifyDialog::OnAreasComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_ChannelCombo, CMClassifyDialog::OnChannelComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_ClassCombo, CMClassifyDialog::OnClassComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_ClassificationProcedure, CMClassifyDialog::OnClassificationProcedureDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_PaletteCombo, CMClassifyDialog::OnPaletteComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_WeightCombo, CMClassifyDialog::OnClassWeightComboDropDown)

	EVT_INIT_DIALOG (CMClassifyDialog::OnInitDialog)

	EVT_TEXT (IDC_CEMThreshold, CMClassifyDialog::OnChangeCEMThreshold)
	EVT_TEXT (IDC_CorrelationCoefficientThreshold, CMClassifyDialog::OnChangeCorrelationCoefficient)
	EVT_TEXT (IDC_CorrelationAngleThreshold, CMClassifyDialog::OnChangeCorrelationThreshold)
	EVT_TEXT (IDC_ColumnEnd, CMClassifyDialog::CheckColumnEnd)
	EVT_TEXT (IDC_ColumnInterval, CMClassifyDialog::CheckColumnInterval)
	EVT_TEXT (IDC_ColumnStart, CMClassifyDialog::CheckColumnStart)
	EVT_TEXT (IDC_LineEnd, CMClassifyDialog::CheckLineEnd)
	EVT_TEXT (IDC_LineInterval, CMClassifyDialog::CheckLineInterval)
	EVT_TEXT (IDC_LineStart, CMClassifyDialog::CheckLineStart)
	EVT_TEXT (IDC_NearestNeighborThreshold, CMClassifyDialog::OnChangeKNNThreshold)
	EVT_TEXT (IDC_ThresholdValue, CMClassifyDialog::OnChangeThreshold)
END_EVENT_TABLE()


CMClassifyDialog::CMClassifyDialog (
				wxWindow*							parent, 
				wxWindowID							id, 
				const wxString&					title)
		: CMDialog (CMClassifyDialog::IDD, parent, title) 
		
{
	m_trainingAreaFlag = FALSE;
	m_imageAreaFlag = FALSE;
	m_thresholdResultsFlag = FALSE;
	m_classWeightsSelection = -1;
	m_thresholdPercent = (float)0.;
	m_knnThreshold = 1;
	m_diskFileFlag = FALSE;
	m_createImageOverlayFlag = FALSE;
	m_classifyListSelection = -1;
	m_covarianceEstimate = 1;
	m_classAreaSelection = 0;
	m_createProbabilityFileFlag = FALSE;
	m_testAreaFlag = FALSE;
	m_fileNamesSelection = -1;
	m_outputFormatCode = 1;
	m_paletteSelection = 0;
	m_trainingAreaLOOFlag = FALSE;
	m_cemThreshold = (float)0.;
	m_angleThreshold = (float)0.;
	m_correlationThreshold = (float)0.;
	m_selectImageOverlaySelection = 1;

	m_selectImageOverlaySelection = 1;

	m_createImageOverlayFlag = FALSE;

	m_classWeightsPtr = NULL;
	m_classAreaListPtr = NULL;
	m_optionKeyFlag = FALSE;

	m_nearestNeighborKValue = 0;
	m_classifyProcedureEnteredCode = 0;

	m_initializedFlag = CMDialog::m_initializedFlag;

	if (m_initializedFlag)
		m_initializedFlag = GetDialogLocalVectors (&m_localFeaturesPtr,
																	&m_localTransformFeaturesPtr,
																	&m_classListPtr,
																	&m_classAreaListPtr,
																	&m_classWeightsPtr,
																	NULL,
																	NULL,
																	NULL);
	CreateControls ();
	 
}	// end "CMClassifyDialog"



CMClassifyDialog::~CMClassifyDialog (void)

{
	ReleaseDialogLocalVectors (m_localFeaturesPtr,
										m_localTransformFeaturesPtr,
										m_classListPtr,
										m_classAreaListPtr,
										m_classWeightsPtr,
										NULL,
										NULL,
										NULL);

}	// end "~CMClassifyDialog"



void CMClassifyDialog::AdjustDlgLayout ()

{
    bSizer122->Layout ();
    bSizer123->Layout ();
    bSizer125->Layout ();
    bSizer126->Layout ();
    bSizer127->Layout ();
    bSizer128->Layout ();
    bSizer129->Layout ();
    bSizer114->Layout ();
	
}	// end "AdjustDlgLayout"



void CMClassifyDialog::CheckAreaSettings (void)
	{
	Boolean 								enableFlag = FALSE;


	if (m_trainingAreaFlag ||
            m_trainingAreaLOOFlag ||
            m_testAreaFlag ||
            m_imageAreaFlag)
		enableFlag = TRUE;
	
	wxButton* okbutton = (wxButton*)FindWindow (wxID_OK);
	okbutton->Enable (enableFlag);
	
}	// end "CheckAreaSettings"



void CMClassifyDialog::CheckColumnEnd (
				wxCommandEvent& 					event)

{
	SignedByte 							handleStatus;


	if (!m_settingSelectedEntireButton)
		{
		m_dialogSelectArea.imageWindowInfoPtr =
                (WindowInfoPtr)GetHandleStatusAndPointer (m_targetWindowInfoHandle,
																			 &handleStatus);

		CMDialog::CheckColumnEnd (event);

		MHSetState (m_targetWindowInfoHandle, handleStatus);

		m_dialogSelectArea.imageWindowInfoPtr = NULL;

		}	// end "if (!m_settingSelectedEntireButton)"

}	// end "CheckColumnEnd"



void CMClassifyDialog::CheckColumnStart (
				wxCommandEvent& 					event)

{
	SignedByte 							handleStatus;


	if (!m_settingSelectedEntireButton)
    	{
		m_dialogSelectArea.imageWindowInfoPtr =
                (WindowInfoPtr)GetHandleStatusAndPointer (m_targetWindowInfoHandle,
																			 &handleStatus);

		CMDialog::CheckColumnStart (event);

		MHSetState (m_targetWindowInfoHandle, handleStatus);

		m_dialogSelectArea.imageWindowInfoPtr = NULL;

    	}	// end "if (!m_settingSelectedEntireButton)"

}	// end "CheckColumnStart"



void CMClassifyDialog::CheckLineEnd (
				wxCommandEvent& 					event)

{
	SignedByte 							handleStatus;


	if (!m_settingSelectedEntireButton)
    	{
		m_dialogSelectArea.imageWindowInfoPtr =
                (WindowInfoPtr)GetHandleStatusAndPointer (m_targetWindowInfoHandle,
																			 &handleStatus);

		CMDialog::CheckLineEnd (event);

		MHSetState (m_targetWindowInfoHandle, handleStatus);

		m_dialogSelectArea.imageWindowInfoPtr = NULL;

		}	// end "if (!m_settingSelectedEntireButton)"

}	// end "CheckLineEnd"



void CMClassifyDialog::CheckLineStart (
				wxCommandEvent& 					event)

{
	SignedByte 							handleStatus;


	if (!m_settingSelectedEntireButton)
    	{
		m_dialogSelectArea.imageWindowInfoPtr =
                (WindowInfoPtr)GetHandleStatusAndPointer (m_targetWindowInfoHandle,
																			 &handleStatus);

		CMDialog::CheckLineStart (event);

		MHSetState (m_targetWindowInfoHandle, handleStatus);

		m_dialogSelectArea.imageWindowInfoPtr = NULL;

		}	// end "if (!m_settingSelectedEntireButton)"

}	// end "CheckLineStart"



void CMClassifyDialog::CheckOutputFormatItems ()

{
			// Make certain that disk file output formats and thresholding
			// are consistent with the requested classification
			// specifications.

	m_outputFormatCode = m_fileFormatCtrl->GetSelection ();

	m_outputFormatCode++;

	ClassifyDialogSetPaletteItems (this,
												m_outputFormatCode,
												m_createImageOverlayFlag);

	if (m_outputFormatCode != 1)
		m_outputAsciiCode = (m_outputAsciiCode & 0xfffd);

	else // m_outputFormatCode == 1
		m_outputAsciiCode = (m_outputAsciiCode | 0x0002);

			// Check threshold items.

	ClassifyDialogSetThresholdItems (this,
												m_classificationProcedure,
												m_imageAreaFlag,
												m_createProbabilityFileFlag,
												m_thresholdResultsFlag,
												m_thresholdAllowedFlag);

}	// end "CheckOutputFormatItems"



void CMClassifyDialog::CreateControls ()

{
	SetSizeHints (wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bVSizerMain = new wxBoxSizer (wxVERTICAL);
	
	bFlexGrid110 = new wxFlexGridSizer (0, 2, 0, 0);
	bFlexGrid110->SetFlexibleDirection (wxBOTH);

	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer (wxVERTICAL);

	m_staticText145 = new wxStaticText (this, 
													wxID_ANY, 
													wxT("Procedure:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText145->Wrap (-1);
	SetUpToolTip (m_staticText145, IDS_ToolTip175);
	bSizer111->Add (m_staticText145, 0, wxALL, 5);
	#if defined multispec_wxlin
		m_procedureCtrl = new wxComboBox (this,
														IDC_ClassificationProcedure,
														wxT("Gaussian Maximum Likelihood"),
														wxDefaultPosition,
														wxSize (275, -1),
														0,
														NULL,
														wxCB_READONLY);
	#endif
	#if defined multispec_wxmac
		m_procedureCtrl = new wxChoice (this,
													IDC_ClassificationProcedure,
													wxDefaultPosition, 
													wxSize (275, -1));
	#endif
	m_procedureCtrl->Append (wxT("Gaussian Maximum Likelihood"));
	m_procedureCtrl->Append (wxT("Mahalanobis"));
	m_procedureCtrl->Append (wxT("Fisher Linear Likelihood"));
	m_procedureCtrl->Append (wxT("ECHO Spectral-spatial..."));
	if (!gProjectInfoPtr->includesStatisticsFromClusterOperationFlag)
		{
  		m_procedureCtrl->Append (wxT("Support Vector Machine (SVM)  +\u21E7..."));
		m_procedureCtrl->Append (wxT("k Nearest Neighbor (KNN)  +\u21E7..."));
		
		}	// end "if (!...->includesStatisticsFromClusterOperationFlag)"
	
	m_procedureCtrl->Append (wxT("Minimum Euclidean Distance"));
	m_procedureCtrl->Append (wxT("Correlation (SAM)  +\u21E7..."));
	m_procedureCtrl->Append (wxT("Matched Filter (CEM)..."));
	m_procedureCtrl->Append (wxT("Parallel Piped"));
	SetUpToolTip (m_procedureCtrl, IDS_ToolTip175);
	bSizer111->Add (m_procedureCtrl, 0, wxALL, 5);

	m_checkBox8 = new wxCheckBox (this, 
											IDC_FeatureTransformation, 
											wxT("Use feature transformation"), 
											wxDefaultPosition, 
											wxDefaultSize, 
											0);
	SetUpToolTip (m_checkBox8, IDS_ToolTip150);
	bSizer111->Add (m_checkBox8, 0, wxALL, 5);

	wxBoxSizer* bSizer113;
	bSizer113 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText146 = new wxStaticText (this, 
													IDC_ChannelPrompt, 
													wxT("Channels:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText146->Wrap (-1);
	SetUpToolTip (m_staticText146, IDS_ToolTip52);
	bSizer113->Add (m_staticText146, 0, wxALIGN_CENTER | wxALL, 5);

   GetAllSubsetMenuControl (kChannelsAvailableMenu,
										this,
										IDC_ChannelCombo,
										130,
										IDS_ToolTip52);
	bSizer113->Add (m_channelsCtrl, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL, 5);

	bSizer111->Add (bSizer113, 0, 0, 5);

	bSizer114 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText147 = new wxStaticText (this, 
													wxID_ANY, 
													wxT("Target:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText147->Wrap (-1);
	SetUpToolTip (m_staticText147, IDS_ToolTip177);
	bSizer114->Add (m_staticText147, 0, wxALIGN_CENTER | wxALL, 5);

	m_staticText175 = new wxStaticText (this,
													IDC_TargetBase,
													wxT("Base Image"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText175->Wrap (-1);
	SetUpToolTip (m_staticText175, IDS_ToolTip177);
	bSizer114->Add (m_staticText175, 0, wxALIGN_CENTER | wxALL, 5);

	m_targetCtrl = new wxChoice (this,
											IDC_TargetCombo,
											//wxT("Combo!"),
											wxDefaultPosition,
											wxSize (300, -1));
											//0,
											//NULL,
											//wxCB_READONLY);
	SetUpToolTip (m_targetCtrl, IDS_ToolTip177);
	bSizer114->Add (m_targetCtrl, 0, wxALL, 5);

	bSizer111->Add (bSizer114, 0, 0, 5);

	wxBoxSizer* bSizer115;
	bSizer115 = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizer116;
	bSizer116 = new wxBoxSizer (wxVERTICAL);

	m_staticText148 = new wxStaticText (this,
													wxID_ANY,
													wxT("Classify:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText148->Wrap (-1);
	bSizer116->Add (m_staticText148, 0, wxALL, 5);

	wxBoxSizer* bSizer132;
	bSizer132 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText149 = new wxStaticText (this,
													wxID_ANY,
													wxT("Class areas:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText149->Wrap (-1);
	SetUpToolTip (m_staticText149, IDS_ToolTip188);
	bSizer132->Add (m_staticText149, 0, wxALIGN_CENTER | wxLEFT, 15);

	GetAllSubsetMenuControl (kClassAreasMenu,
										this,
										IDC_AreasCombo,
										100,
										IDS_ToolTip103);
	bSizer132->Add (m_classAreasCtrl, 0, wxLEFT, 5);

	bSizer116->Add (bSizer132, 1, 0, 5);

	wxBoxSizer* bVSizerTrainTest = new wxBoxSizer (wxVERTICAL);
	m_checkBox9 = new wxCheckBox (this,
											IDC_Training,
											wxT("Training (resubstitution)"),
											wxDefaultPosition,
											wxDefaultSize,
											0);
	SetUpToolTip (m_checkBox9, IDS_ToolTip189);
	bVSizerTrainTest->Add (m_checkBox9, 0, wxTOP, 3);

	m_checkBox11 = new wxCheckBox (this,
												IDC_TrainingLOO,
												wxT("Training (leave-one-out)"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	SetUpToolTip (m_checkBox11, IDS_ToolTip193);
	bVSizerTrainTest->Add (m_checkBox11, 0, wxTOP, 3);

	m_checkBox10 = new wxCheckBox (this,
												IDC_TestAreas,
												wxT("Test areas (holdout)"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	SetUpToolTip (m_checkBox10, IDS_ToolTip168);
	bVSizerTrainTest->Add (m_checkBox10, 0, wxTOP|wxBOTTOM, 3);
	
	bSizer116->Add (bVSizerTrainTest, 0, wxLEFT, 25);

	bSizer115->Add (bSizer116, 0, wxEXPAND);

	wxBoxSizer* bSizer117;
	bSizer117 = new wxBoxSizer (wxVERTICAL);

	m_checkBox12 = new wxCheckBox (this,
												IDC_ImageArea,
												wxT("Image selection"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer117->Add (m_checkBox12, 0, wxLEFT, 15);

	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer (new wxStaticBox (this,
																		IDC_LineColFrame,
																		wxT("Area to Classify"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL),
													wxHORIZONTAL);
	
	CreateLineColumnControls (sbSizer8);

	bSizer117->Add (sbSizer8, 1, wxEXPAND|wxTOP, 3);
	
	bSizer115->Add (bSizer117, 0, 0, 5);
	bSizer111->Add (bSizer115, 0, 0, 5);
	bFlexGrid110->Add (bSizer111, 1, wxALL, 12);

	wxBoxSizer* bSizer112;
	bSizer112 = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizer120;
	bSizer120 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText176 = new wxStaticText (this,
													IDC_ClassPrompt,
													wxT("Classes:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText176->Wrap (-1);
	SetUpToolTip (m_staticText176, IDS_ToolTip103);
	bSizer120->Add (m_staticText176, 0, wxALIGN_CENTER, 5);

	GetAllSubsetMenuControl (kClassesMenu,
										this,
										IDC_ClassCombo,
										100,
										IDS_ToolTip103);
	bSizer120->Add (m_classesCtrl, 0, wxALIGN_CENTER|wxLEFT, 5);

	bSizer112->Add (bSizer120, 0, 0, 5);

	bSizer122 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText178 = new wxStaticText (this,
													IDC_WeightsPrompt,
													wxT("Class weights:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText178->Wrap (-1);
	SetUpToolTip (m_staticText178, IDS_ToolTip156);
	bSizer122->Add (m_staticText178, 0, wxALIGN_CENTER, 5);

	m_staticText186 = new wxStaticText (this,
													IDC_WeightsEqual,
													wxT("Equal"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText186->Wrap (-1);
	bSizer122->Add (m_staticText186, 0, wxALIGN_CENTER|wxLEFT, 5);

	GetEqualUnequalMenuControl (kWeightsMenu,
											this,
											IDC_WeightCombo,
											120,
											IDS_ToolTip157);
	bSizer122->Add (m_weightsCtrl,
							0,
							wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALIGN_CENTER|wxLEFT, 5);


	bSizer112->Add (bSizer122, 0, wxBOTTOM|wxTOP, 5);

	bSizer123 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText179 = new wxStaticText (this,
													IDC_SymbolPrompt,
													wxT("Symbols:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText179->Wrap (-1);
	SetUpToolTip (m_staticText179, IDS_ToolTip157);
	bSizer123->Add (m_staticText179, 0, wxALIGN_CENTER, 5);

	m_staticText187 = new wxStaticText (this,
													IDC_SymbolsDefaultSet,
													wxT("Default set"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText187->Wrap (-1);
	SetUpToolTip (m_staticText187, IDS_ToolTip157);
	bSizer123->Add (m_staticText187, 0, wxALIGN_CENTER|wxLEFT, 5);

	m_symbolCtrl = new wxChoice (this,
											IDC_SymbolCombo,
											wxDefaultPosition,
											wxDefaultSize);
	m_symbolCtrl->Append (wxT("Default set"));
	m_symbolCtrl->Append (wxT("User defined..."));
	bSizer123->Add (m_symbolCtrl, 0, wxALIGN_CENTER|wxLEFT, 5);


	bSizer112->Add (bSizer123, 0, wxBOTTOM|wxTOP, 5);

	wxBoxSizer* bSizer124;
	bSizer124 = new wxBoxSizer (wxVERTICAL);

	m_staticText180 = new wxStaticText (this,
													wxID_ANY,
													wxT("Write classification results to:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText180->Wrap (-1);
	bSizer124->Add (m_staticText180, 0, wxBOTTOM|wxTOP, 5);

	bSizer125 = new wxBoxSizer (wxHORIZONTAL);

	m_checkBox13 = new wxCheckBox (this,
												IDC_DiskFile,
												wxT("Disk File:"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	SetUpToolTip (m_checkBox13, IDS_ToolTip178);
	bSizer125->Add (m_checkBox13, 0, wxALIGN_CENTER|wxLEFT, 15);

	m_fileFormatCtrl = new wxChoice (this,
												IDC_DiskCombo,
												wxDefaultPosition,
												wxDefaultSize);
	m_fileFormatCtrl->Append (wxT("ASCII"));
	m_fileFormatCtrl->Append (wxT("ERDAS .GIS"));
	m_fileFormatCtrl->Append (wxT("GAIA"));
	m_fileFormatCtrl->Append (wxT("GeoTIFF"));
	SetUpToolTip (m_fileFormatCtrl, IDS_ToolTip215);
	bSizer125->Add (m_fileFormatCtrl, 0, wxALIGN_CENTER|wxLEFT, 5);

	bSizer124->Add (bSizer125, 0, wxALL, 5);

	bSizer126 = new wxBoxSizer (wxVERTICAL);

	m_checkBox14 = new wxCheckBox (this,
												IDC_ImageWindowOverlay,
												wxT("Image window overlay"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	SetUpToolTip (m_checkBox14, IDS_ToolTip179);
	bSizer126->Add (m_checkBox14, 0, wxLEFT, 15);

	m_overlayCtrl = new wxChoice (this,
												IDC_ImageOverlayCombo,
												wxDefaultPosition,
												wxSize (300, -1));
	m_overlayCtrl->Append (wxT("Add new overlay"));
	SetUpToolTip (m_overlayCtrl, IDS_ToolTip180);
	bSizer126->Add (m_overlayCtrl, 0, wxLEFT, 35);

	bSizer124->Add (bSizer126, 0, wxALL, 5);

	bSizer127 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText182 = new wxStaticText (this,
													IDC_PalettePrompt,
													wxT("Palette:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText182->Wrap (-1);
	bSizer127->Add (m_staticText182, 0, wxALIGN_CENTER|wxLEFT, 15);
	#if defined multispec_wxlin
		m_paletteCtrl = new wxComboBox (this,
													IDC_PaletteCombo,
													wxT("Default Colors"),
													wxDefaultPosition,
													wxDefaultSize,
													0,
													NULL,
													wxCB_READONLY);
	#endif
	#if defined multispec_wxmac
		m_paletteCtrl = new wxChoice (this,
												IDC_PaletteCombo,
												wxDefaultPosition,
												wxDefaultSize);
	#endif
	m_paletteCtrl->Append (wxT("Default Colors"));
	m_paletteCtrl->Append (wxT("Default Gray Levels"));
	m_paletteCtrl->Append (wxT("Blue-Green-Red"));
	m_paletteCtrl->Append (wxT("AVHRR NDVI"));
	m_paletteCtrl->Append (wxT("MODIS NDVI"));
	m_paletteCtrl->Append (wxT("False Color..."));
	m_paletteCtrl->Append (wxT("ERDAS .trl file"));
	m_paletteCtrl->Append (wxT("User Defined"));
	SetUpToolTip (m_paletteCtrl, IDS_ToolTip181);
	bSizer127->Add (m_paletteCtrl, 0, wxALIGN_CENTER|wxLEFT, 5);

	bSizer124->Add (bSizer127, 0, wxALL, 5);

	bSizer128 = new wxBoxSizer (wxHORIZONTAL);

	m_checkBox15 = new wxCheckBox (this,
												IDC_ThresholdResults,
												wxT("Threshold results at"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	SetUpToolTip (m_checkBox15, IDS_ToolTip182);
	bSizer128->Add (m_checkBox15, 0, wxALIGN_CENTER|wxALL, 5);

	m_textCtrl85 = new wxTextCtrl (this,
												IDC_ThresholdValue,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (60, -1),
												0);
	m_textCtrl85->SetValidator (wxTextValidator (wxFILTER_NUMERIC, &m_ThresholdString));
	wxFloatingPointValidator<double> _val (4, &m_thresholdPercent);
	_val.SetRange (0.0, 100.);
	m_textCtrl85->SetValidator (_val);
	SetUpToolTip (m_textCtrl85, IDS_ToolTip183);
	bSizer128->Add (m_textCtrl85, 0, wxALL, 5);

	m_staticText183 = new wxStaticText (this,
													IDC_PercentSymbol,
													wxT("%"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText183->Wrap (-1);
	bSizer128->Add (m_staticText183, 0, wxALIGN_CENTER|wxALL, 5);

	m_textCtrl87 = new wxTextCtrl (this,
												IDC_CEMThreshold,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (40, -1),
												0);
	SetUpToolTip (m_textCtrl87, IDS_ToolTip184);
	m_textCtrl87->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
																&m_cemThresholdString));
	wxFloatingPointValidator<double> _val187 (3, &m_cemThreshold);
	_val187.SetRange (0.0, 1.0);
	m_textCtrl87->SetValidator (_val);

	bSizer128->Add (m_textCtrl87, 0, wxALL, 5);

	m_textCtrl88 = new wxTextCtrl (this,
												IDC_CorrelationAngleThreshold,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (40, -1),
												0);
	m_textCtrl88->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
																&m_corrThresholdString));
	wxFloatingPointValidator<double> _val188 (3, &m_angleThreshold);
	_val188.SetRange (0.0, 90.);
	m_textCtrl88->SetValidator (_val188);
	SetUpToolTip (m_textCtrl88, IDS_ToolTip148);
	bSizer128->Add (m_textCtrl88, 0, wxALL, 5);

	m_staticText185 = new wxStaticText (this,
													IDC_DegreeSymbol,
													wxT("o"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText185->Wrap (-1);
	bSizer128->Add (m_staticText185, 0, 0);

	wxTextCtrl* m_textCtrl88 = new wxTextCtrl (this,
																IDC_NearestNeighborThreshold,
																wxEmptyString,
																wxDefaultPosition,
																wxSize (40, -1),
																0);
	m_textCtrl88->SetValidator (wxTextValidator (wxFILTER_DIGITS,
																&m_knnThresholdString));
	SetUpToolTip (m_textCtrl88, IDS_ToolTip401);
	bSizer128->Add (m_textCtrl88, 0, wxALL, 5);

	wxStaticText* staticText1852 = new wxStaticText (this,
																		IDC_NearestNeighbor,
																		wxT("neighbors"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		0);
	staticText1852->Wrap (-1);
	bSizer128->Add (staticText1852, 0, wxALIGN_CENTER|wxALL, 5);

	bSizer124->Add (bSizer128, 0);

	//	wxBoxSizer* bSizer129;
	bSizer129 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText184 = new wxStaticText (this,
													IDC_correlationPrompt,
													wxT("or correlation     \n coefficient of"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText184->Wrap (-1);
	SetUpToolTip (m_staticText184, IDS_ToolTip148);
	bSizer129->Add (m_staticText184, 0, wxALIGN_CENTER|wxLEFT, 25);

	m_textCtrl86 = new wxTextCtrl (this,
												IDC_CorrelationCoefficientThreshold,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (80, -1),
												0);
	m_textCtrl86->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
																&m_corrThresholdString));
	wxFloatingPointValidator<double> _val186 (4, &m_correlationThreshold);
	_val186.SetRange (0.0, 1.0);
	m_textCtrl86->SetValidator (_val186);
	SetUpToolTip (m_textCtrl86, IDS_ToolTip148);
	bSizer129->Add (m_textCtrl86, 0, wxALL, 5);

	bSizer124->Add (bSizer129, 0, wxEXPAND|wxLEFT, 5);

	wxBoxSizer* bSizer130;
	bSizer130 = new wxBoxSizer (wxHORIZONTAL);

	m_checkBox16 = new wxCheckBox (this,
												IDC_CreateProbabilityFile,
												wxT("Create Probability Results File"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	SetUpToolTip (m_checkBox16, IDS_ToolTip186);
	bSizer130->Add (m_checkBox16, 0, wxALL, 5);

	bSizer124->Add (bSizer130, 0, wxEXPAND);

	m_button23 = new wxButton (this,
										IDC_ListOptions,
										wxT("Results List Options..."),
										wxDefaultPosition,
										wxSize (200, -1),
										0);
	SetUpToolTip (m_button23, IDS_ToolTip187);
	bSizer124->Add (m_button23, 0, wxALL, 5);
	
	bSizer112->Add (bSizer124, 0, wxEXPAND, 5);

	bFlexGrid110->Add (bSizer112, 1, wxLEFT|wxTOP|wxRIGHT, 12);
	
	bVSizerMain->Add (bFlexGrid110, 1, wxLEFT|wxTOP|wxRIGHT, 12);
	
	CreateStandardButtons (bVSizerMain);

	m_textCtrl87->Hide ();
	m_textCtrl88->Hide ();
	m_staticText185->Hide ();
	SetSizerAndFit (bVSizerMain);
	Layout ();
	bVSizerMain->Fit (this);
	Centre (wxBOTH);

}	// end "CreateControls"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the classification
//							specification dialog box to the user and copy the
//							revised back to the classify specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/27/1996
//	Revised By:			Larry L. Biehl			Date: 05/04/2019

SInt16 CMClassifyDialog::DoDialog (void)

{
	SInt16 								returnCode;

	Boolean 								continueFlag = FALSE;


			// Make sure intialization has been completed.

	if (!m_initializedFlag)
																							return (FALSE);

	returnCode = ShowModal ();

	if (returnCode == wxID_OK)
		{
		continueFlag = TRUE;

				// Classification area

		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;

		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;

		ClassifyDialogOK (m_classificationProcedure,
								m_covarianceEstimate,
								m_featureTransformationFlag,
								m_channelSelection,
								(SInt16)m_localActiveNumberFeatures,
								(SInt16*)m_localActiveFeaturesPtr,
								m_targetWindowInfoHandle,
								m_fileNamesSelection+1,
								m_classAreaSelection,
								(SInt16)m_localNumberClassAreas,
								(SInt16*)m_classAreaListPtr,
								m_trainingAreaFlag,
								m_trainingAreaLOOFlag,
								m_testAreaFlag,
								m_imageAreaFlag,
								&m_dialogSelectArea,
								m_classSelection,
								(SInt16)m_localNumberClasses,
								(SInt16*)m_classListPtr,
								m_classWeightsSelection + 1,
								m_classWeightsPtr,
								m_symbolSelection,
								m_localSymbolsPtr,
								m_diskFileFlag,
								m_createImageOverlayFlag,
								m_selectImageOverlaySelection + 1,
								m_outputFormatCode + 1,
								m_thresholdResultsFlag,
								m_correlationThreshold,
								m_angleThreshold,
								m_cemThreshold,
								m_thresholdPercent,
								m_knnThreshold,
								m_createProbabilityFileFlag,
								m_paletteSelection + 1,
								m_listResultsTestCode,
								m_listResultsTrainingCode,
								m_parallelPipedCode,
								m_nearestNeighborKValue);

		}	// end "if (returnCode == IDOK)"
    
			// Set the variable containing the pointer to the overlay combo box
			// to NULL.

	gPopUpImageOverlayMenu = NULL;

	return (continueFlag);

}	// end "DoDialog"



void CMClassifyDialog::HideShowClassAreasItem ()

{
	if (!m_trainingAreaFlag && !m_trainingAreaLOOFlag && !m_testAreaFlag)
		MHideDialogItem (this, IDC_AreasCombo);

	else	// m_trainingAreaFlag || m_trainingAreaLOOFlag ...
		MShowDialogItem (this, IDC_AreasCombo);

	
}	// end "HideShowClassAreasItem"


/*
void CMClassifyDialog::OnActivate (
				wxActivateEvent&								event)

{
			// Have tried many things to be able to use the first mouse click
			// after a dialog box is display on top of the Classify Dialog when
			// called from a combo box event. Seems to be related only to XCode
	
	if (event.GetActive ())
		{
		if (!IsThisEnabled ())
			Enable ();
		
		if (!IsEnabled ())
			Enable ();

		Lower ();
		Raise ();
		SetFocus ();
		FindWindow (wxID_OK)->SetFocus ();

		}
	
	else
		{

		}
	
	event.Skip ();
	
}	// end "OnActivate"
*/


void CMClassifyDialog::OnAreasComboCloseUp (
				wxCommandEvent& 					event)

{
	int areasSelection = m_classAreasCtrl->GetSelection ();
	
	if (areasSelection == -1)
		m_classAreasCtrl->SetSelection (m_classAreaSelection);
	
	event.Skip ();
	
}	// end "OnAreasComboCloseUp"



void CMClassifyDialog::OnAreasComboDropDown (
				wxCommandEvent&					event)

{
   if (m_classAreasCtrl != NULL)
		m_classAreasCtrl->SetSelection (-1);
	
}	// end "OnAreasComboDropDown"



void CMClassifyDialog::OnAreasComboSelendok (
				wxCommandEvent& 					event)

{
	HandleClassesMenu (&m_localNumberClassAreas,
								(SInt16*)m_classAreaListPtr,
								1,
								(SInt16)gProjectInfoPtr->numberStatisticsClasses,
								IDC_AreasCombo,
								&m_classAreaSelection);

}	// end "OnAreasComboSelendok"



void CMClassifyDialog::OnChangeCEMThreshold (
				wxCommandEvent& 					event)

{
	/*
	wxTextCtrl* cemThreshold = (wxTextCtrl*)FindWindow (IDC_CEMThreshold);
	wxString cemThresholdString = cemThreshold->GetValue ();
	cemThresholdString.ToDouble (&m_cemThreshold);
	double	savedCEMThreshold = m_cemThreshold;

	if (m_cemThreshold < 0)
		m_cemThreshold = 0;
	
	else if (m_cemThreshold > 1)
		m_cemThreshold = 1;
	
	if (m_cemThreshold != savedCEMThreshold)
		{
		cemThreshold->ChangeValue (wxString::Format (wxT("%.1f"), m_cemThreshold));
	 
		}	// end "if (m_cemThreshold != savedCEMThreshold)"
	*/
}	// end "OnChangeCEMThreshold"



void CMClassifyDialog::OnChangeCorrelationCoefficient (
				wxCommandEvent& 					event)

{
	wxTextCtrl* correlationcoeff =
								(wxTextCtrl*)FindWindow (IDC_CorrelationCoefficientThreshold);
	wxString m_correlationThresholdString = correlationcoeff->GetValue ();
	m_correlationThresholdString.ToDouble (&m_correlationThreshold);

	if (m_correlationThreshold >= 0 && m_correlationThreshold <= 1)
		{
		m_saveCorrelationThreshold = m_correlationThreshold;
		m_angleThreshold = (float)(acos (m_saveCorrelationThreshold)*kRadiansToDegrees);
		m_saveAngleThreshold = m_angleThreshold;
		wxTextCtrl* correlationthres =
										(wxTextCtrl*)FindWindow (IDC_CorrelationAngleThreshold);
		correlationthres->ChangeValue (wxString::Format (wxT("%.1f"), m_angleThreshold));

		}	// end "if (m_correlationThreshold >= 0 && ..."
	
}	// end "OnChangeCorrelationCoefficient"



void CMClassifyDialog::OnChangeCorrelationThreshold (
				wxCommandEvent& 					event)

{
	wxTextCtrl* correlationthres =
									(wxTextCtrl*)FindWindow (IDC_CorrelationAngleThreshold);
	wxString correlationthresString = correlationthres->GetValue ();
	correlationthresString.ToDouble (&m_angleThreshold);

	if (m_angleThreshold >= 0 && m_angleThreshold <= 90)
		{
		m_saveAngleThreshold = m_angleThreshold;
		m_correlationThreshold = (float)cos (m_angleThreshold * kDegreesToRadians);
		m_saveCorrelationThreshold = m_correlationThreshold;
		wxTextCtrl* correlationcoef =
							(wxTextCtrl*)FindWindow (IDC_CorrelationCoefficientThreshold);
		correlationcoef->ChangeValue (
									wxString::Format (wxT("%.4f"), m_correlationThreshold));
		
		}	// end "if (thresholdValue >= 0 && ..."

}	// end "OnChangeCorrelationThreshold"



void CMClassifyDialog::OnChangeKNNThreshold (
				wxCommandEvent& 					event)

{
	wxTextCtrl* knnThreshold = (wxTextCtrl*)FindWindow (IDC_NearestNeighborThreshold);
	wxString knnThresholdString = knnThreshold->GetValue ();
	knnThresholdString.ToLong (&m_knnThreshold);
	int	savedKNNThresholdValue = m_knnThreshold;

	if (m_knnThreshold < 1)
		m_knnThreshold = 1;
	
	else if (m_knnThreshold > m_nearestNeighborKValue)
		m_knnThreshold = m_nearestNeighborKValue;
	
	if (m_knnThreshold != savedKNNThresholdValue)
		{
		knnThreshold->ChangeValue (wxString::Format (wxT("%ld"), m_knnThreshold));
		
		}	// end "if (m_knnThreshold != savedKNNThresholdValue)"

}	// end "OnChangeKNNThreshold"



void CMClassifyDialog::OnChangeThreshold (
				wxCommandEvent& 					event)

{
	//double					savedThresholdPercent;
	
	
	//savedThresholdPercent = m_thresholdPercent;
	/*
	wxTextCtrl* thresholdValue = (wxTextCtrl*)FindWindow (IDC_ThresholdValue);
	wxString thresholdValueString = thresholdValue->GetValue ();
	if (thresholdValueString.ToDouble (&m_thresholdPercent))
		{
		savedThresholdPercent = m_thresholdPercent;
	
	//else	// bad string number
	//	savedThresholdPercent = -1;

		if (m_thresholdPercent < 0)
			m_thresholdPercent = 0;
	 
		else if (m_thresholdPercent > 100)
			m_thresholdPercent = 100;
	 
		if (m_thresholdPercent != savedThresholdPercent)
			thresholdValue->ChangeValue (wxString::Format (wxT("%.1f"), m_thresholdPercent));
	 
		}	// end "if (thresholdValueString.ToDouble (&m_thresholdPercent))"
	*/
}	// end "OnChangeThreshold"



void CMClassifyDialog::OnChannelComboSelendok (
				wxCommandEvent& 					event)

{
	HandleChannelsMenu (IDC_ChannelCombo,
								m_featureTransformationFlag,
								(SInt16)gProjectInfoPtr->numberStatisticsChannels,
								2,
								TRUE);

}	// end "OnChannelComboSelendok"



void CMClassifyDialog::OnClassificationProcedureCloseUp (
				wxCommandEvent& 					event)

{
	int classificationSelection = m_procedureCtrl->GetSelection ();
	
	if (classificationSelection == -1)
		m_procedureCtrl->SetSelection (m_classifyListSelection);
	
	event.Skip ();
	
}	// end "OnClassificationProcedureCloseUp"



void CMClassifyDialog::OnClassificationProcedureDropDown (
				wxCommandEvent&					event)

{
	wxMouseState						mousestate;
	
	
	m_optionKeyFlag = mousestate.RightIsDown () || wxGetKeyState (WXK_SHIFT);
	
	m_procedureCtrl->SetSelection (-1);
	
}	// end "OnClassificationProcedureDropDown"



void CMClassifyDialog::OnClassificationProcedureSelendok (
				wxCommandEvent& 					event)

{
	wxMouseState						mousestate;
	
	SInt16 								classificationProcedure = 0,
            							savedClassifyListSelection,
            							weightsSelection;
	

	savedClassifyListSelection = m_classifyListSelection;

	m_classifyListSelection = m_procedureCtrl->GetSelection ();

			// Get the actual classification procedure code.

	SInt64 classifyListSelection64 =
				(SInt64)((int*)m_procedureCtrl->GetClientData (m_classifyListSelection));
	classificationProcedure = (SInt16)classifyListSelection64;
	
    		// Get the current weights selection. Force to 1 base.

	weightsSelection = m_classWeightsSelection + 1;
	
	m_optionKeyFlag = mousestate.RightIsDown () || wxGetKeyState (WXK_SHIFT);

	classificationProcedure = ClassifyDialogOnClassificationProcedure (
																this,
																wxID_OK,
																&m_thresholdAllowedFlag,
																&m_featureTransformAllowedFlag,
																&weightsSelection,
																&m_parallelPipedCode,
																classificationProcedure,
																&m_covarianceEstimate,
																m_numberEigenvectors,
																&m_nearestNeighborKValue,
																&m_classifyProcedureEnteredCode,
																m_correlationComboListItem,
																m_optionKeyFlag);

	m_classWeightsSelection = weightsSelection - 1;

	if (classificationProcedure != 0)
		{
		m_classificationProcedure = classificationProcedure;

		ClassifyDialogSetLeaveOneOutItems (this,
														 m_classificationProcedure,
														 m_fileNamesSelection,
														 m_savedLeaveOneOutFlag,
														 m_trainingFieldsExistFlag,
														 &m_trainingAreaLOOFlag);

		CheckFeatureTransformationDialog (this,
													 m_featureTransformAllowedFlag,
													 IDC_FeatureTransformation,
													 IDC_ChannelPrompt,
													 &m_featureTransformationFlag);

		ClassifyDialogSetThresholdItems (this,
													 m_classificationProcedure,
													 m_imageAreaFlag,
													 m_createProbabilityFileFlag,
													 m_thresholdResultsFlag,
													 m_thresholdAllowedFlag);

		if (weightsSelection > 0)
			{
			HideDialogItem (this, IDC_WeightsEqual);
			ShowDialogItem (this, IDC_WeightCombo);

			m_weightsCtrl->SetSelection (m_classWeightsSelection);

			}	// end "if (weightsSelection > 0)"

		else // weightsSelection <= 0
			{
			ShowDialogItem (this, IDC_WeightsEqual);
			HideDialogItem (this, IDC_WeightCombo);

			}	// end "else weightsSelection <= 0"

		}	// end "if (classificationProcedure != 0)"

	else // classificationProcedure == 0
		{
		m_classifyListSelection = savedClassifyListSelection;
		
		}	// end "else m_classificationProcedure == 0"

	m_procedureCtrl->SetSelection (m_classifyListSelection);
	
	SInt64 classificationProcedure64 =(SInt64)((int*)m_procedureCtrl->
														GetClientData (m_classifyListSelection));
	m_classificationProcedure = (SInt16)classificationProcedure64;
	
	m_optionKeyFlag = FALSE;
	
   switch (m_classifyListSelection)
   	{
      case 5:
         SetUpToolTip (m_checkBox16, IDS_ToolTip232);
         break;
			
      case 6:
         SetUpToolTip (m_checkBox16, IDS_ToolTip232);
         break;
			
      default:
			SetUpToolTip (m_checkBox16, IDS_ToolTip186);
			
   	}	// end "switch (m_classifyListSelection)"
	
	AdjustDlgLayout ();
	
	event.Skip ();
	
}	// end "OnClassificationProcedureSelendok"



void CMClassifyDialog::OnClassWeightsComboSelendok (
				wxCommandEvent& 					event)

{
	HandleClassWeightsMenu ((SInt16)m_localNumberClasses,
									(SInt16*)m_classListPtr,
									m_classWeightsPtr,
									gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
									IDC_WeightCombo,
									&m_classWeightsSelection);

}	// end "OnClassWeightsComboSelendok"



void CMClassifyDialog::OnCreateProbabilityFile (
				wxCommandEvent& 					event)

{
    wxCheckBox* probfilecb = (wxCheckBox*)FindWindow (IDC_CreateProbabilityFile);
    m_createProbabilityFileFlag = probfilecb->GetValue ();

}	// end "OnCreateProbabilityFile"



void CMClassifyDialog::OnDiskComboSelendok (
				wxCommandEvent& 					event)

{
	CheckOutputFormatItems ();

}	// end "OnDiskComboSelendok"



void CMClassifyDialog::OnDiskFile (
				wxCommandEvent& 					event)

{
	wxCheckBox* diskfcb = (wxCheckBox*)FindWindow (IDC_DiskFile);
	m_diskFileFlag = diskfcb->GetValue ();

	if (!m_diskFileFlag)
		HideDialogItem (this, IDC_DiskCombo);

	else	// !m_diskFileFlag)
		ShowDialogItem (this, IDC_DiskCombo);

	CheckOutputFormatItems ();
	AdjustDlgLayout ();
	
}	// end "OnDiskFile"



void CMClassifyDialog::OnFeatureTransformation (
				wxCommandEvent& 					event)

{
    wxCheckBox* featurecb = (wxCheckBox*)FindWindow (IDC_FeatureTransformation);
    m_featureTransformationFlag = featurecb->GetValue ();
	
    CheckFeatureTransformationDialog (this,
													m_featureTransformAllowedFlag,
													IDC_FeatureTransformation,
													IDC_ChannelPrompt,
													(SInt16*)&m_featureTransformationFlag);

    m_channelSelection = UpdateDialogFeatureParameters (
														m_featureTransformationFlag,
														&m_localActiveNumberFeatures,
														&m_localActiveFeaturesPtr,
														m_localNumberFeatures,
														m_localFeaturesPtr,
														gProjectInfoPtr->numberStatisticsChannels,
														m_localNumberTransformFeatures,
														m_localTransformFeaturesPtr,
														gTransformationMatrix.numberFeatures);

}	// end "OnFeatureTransformation"



void CMClassifyDialog::OnImageArea (
				wxCommandEvent& 					event)

{
	SignedByte 							handleStatus;


    wxCheckBox* imageareacb = (wxCheckBox*)FindWindow (IDC_ImageArea);
    m_imageAreaFlag = imageareacb->GetValue ();
    HideShowAreaItems (m_imageAreaFlag);

			// Determine if this is the entire area and set the
			// to entire image icon.

	if (m_imageAreaFlag)
    	{
		m_dialogSelectArea.imageWindowInfoPtr =
                (WindowInfoPtr)GetHandleStatusAndPointer (m_targetWindowInfoHandle,
                &handleStatus);

		SetEntireImageButtons (NULL,
										 m_LineStart,
										 m_LineEnd,
										 m_ColumnStart,
										 m_ColumnEnd);

		MHSetState (m_targetWindowInfoHandle, handleStatus);

		m_dialogSelectArea.imageWindowInfoPtr = NULL;

    	}	// end "if (m_imageAreaFlag)"

    ClassifyDialogSetThresholdItems (this,
													m_classificationProcedure,
													m_imageAreaFlag,
													m_createProbabilityFileFlag,
													m_thresholdResultsFlag,
													m_thresholdAllowedFlag);

    CheckAreaSettings ();

}	// end "OnImageArea"



void CMClassifyDialog::OnImageOverlay (
				wxCommandEvent& 					event)

{
	wxCheckBox* imageovercb = (wxCheckBox*)FindWindow (IDC_ImageWindowOverlay);
	m_createImageOverlayFlag = imageovercb->GetValue ();

	if (m_createImageOverlayFlag)
		ShowDialogItem (this, IDC_ImageOverlayCombo);

	else // m_createImageOverlayFlag
		HideDialogItem (this, IDC_ImageOverlayCombo);

	ClassifyDialogSetPaletteItems (this,
											m_outputFormatCode,
											m_createImageOverlayFlag);

	AdjustDlgLayout ();
	
}	// end "OnImageOverlay"



void CMClassifyDialog::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	SInt16 								channelSelection,
											classAreaSelection,
											classSelection,
											fileNamesSelection,
											outputFormatCode,
											paletteSelection,
											selectImageOverlaySelection,
											symbolSelection,
											weightsSelection;


			// Make sure that we have the bitmaps for the entire image buttons.

	ClassifyDialogInitialize (this,
										m_localFeaturesPtr,
										m_localTransformFeaturesPtr,
										m_classListPtr,
										m_classAreaListPtr,
										m_localSymbolsPtr,
										&m_classificationProcedure,
										&m_thresholdAllowedFlag,
										&m_covarianceEstimate,
										&m_numberEigenvectors,
										&m_featureTransformAllowedFlag,
										&m_featureTransformationFlag,
										&channelSelection,
										&m_localActiveNumberFeatures,
										&fileNamesSelection,
										&m_targetWindowInfoHandle,
										&classAreaSelection,
										&m_localNumberClassAreas,
										&m_trainingAreaFlag,
										&m_trainingFieldsExistFlag,
										&m_savedLeaveOneOutFlag,
										&m_trainingAreaLOOFlag,
										&m_testAreaFlag,
										&m_imageAreaFlag,
										&m_dialogSelectArea,
										&classSelection,
										&m_localNumberClasses,
										&weightsSelection,
										&symbolSelection,
										&m_outputAsciiCode,
										&m_createImageOverlayFlag,
										&selectImageOverlaySelection,
										&outputFormatCode,
										&m_diskFileFlag,
										&paletteSelection,
										&m_thresholdResultsFlag,
										&m_createProbabilityFileFlag,
										&m_saveThresholdPercent,
										&m_saveAngleThreshold,
										&m_saveCorrelationThreshold,
										&m_saveCEMThreshold,
										&m_saveKNNThreshold,
										&m_listResultsTestCode,
										&m_listResultsTrainingCode,
										&m_parallelPipedCode,
										&m_nearestNeighborKValue);

			// Set feature/transform feature parameters  

	InitializeDialogFeatureParameters (m_featureTransformationFlag,
													m_localActiveNumberFeatures,
													gProjectInfoPtr->numberStatisticsChannels,
													gTransformationMatrix.numberFeatures,
													m_localFeaturesPtr,
													m_localTransformFeaturesPtr,
													&m_localNumberFeatures,
													&m_localNumberTransformFeatures,
													&m_localActiveFeaturesPtr);

			// Set up classification popup menu list
	
	m_supportVectorMachineComboListItem = kSupportVectorMachineMode - 1;
	m_kNearestNeighborComboListItem = kKNearestNeighborMode - 1;
	m_correlationComboListItem = kCorrelationMode - 1;
	if (gProjectInfoPtr->statisticsCode != kMeanCovariance)
		{
		m_supportVectorMachineComboListItem = 0;
		m_kNearestNeighborComboListItem = 1;
		m_correlationComboListItem = 3;
		
		}	// end "if (gProjectInfoPtr->statisticsCode != kMeanCovariance)"

			// Get the classification list selection that matches the input
			// classification procedure.

	m_classifyListSelection = GetComboListSelection2 (this,
																		IDC_ClassificationProcedure,
																		m_classificationProcedure);

	if (m_classifyListSelection == -1)
		m_classifyListSelection = 0;

			//	Set the channels/features list item								

	m_availableFeaturePtr = gProjectInfoPtr->channelsPtr;
	m_channelSelection = channelSelection;

			// Target File

	m_fileNamesSelection = fileNamesSelection - 1;

			// Class areas to classify.

	m_classAreaSelection = classAreaSelection;

			// Selected area to classify.

	m_LineStart = m_dialogSelectArea.lineStart;
	m_LineEnd = m_dialogSelectArea.lineEnd;
	m_LineInterval = m_dialogSelectArea.lineInterval;
	m_ColumnStart = m_dialogSelectArea.columnStart;
	m_ColumnEnd = m_dialogSelectArea.columnEnd;
	m_ColumnInterval = m_dialogSelectArea.columnInterval;

			// Classes to use.

	m_classSelection = classSelection;

			// Class weights.				             
			// Adjust for 0 base index. 

	m_classWeightsSelection = weightsSelection - 1;

	if (weightsSelection > 0)
	  HideDialogItem (this, IDC_WeightsEqual);

	else // weightsSelection <= 0
		HideDialogItem (this, IDC_WeightCombo);

			// Symbols to use.             
			// Adjust for 0 base index.
			// User defined symbols are not available yet.																	

	m_symbolSelection = symbolSelection - 1;
	HideDialogItem (this, IDC_SymbolCombo);
	m_symbolCtrl->Delete (1);

			// Classification output to disk file.
			// Adjust for 0 base index.

	m_outputFormatCode = abs (outputFormatCode);
	m_outputFormatCode -= 1;

	if (!(gClassifySpecsPtr->outputStorageType & 0x0006))
		gOutputFormatCode = -gOutputFormatCode;

	if (!m_diskFileFlag)
		MHideDialogItem (this, IDC_DiskCombo);

	m_selectImageOverlaySelection = selectImageOverlaySelection - 1;

			// Palette to use.             
			// Adjust for 0 base index.

	m_paletteSelection = paletteSelection - 1;

			// Threshold output data. 															
			// Threshold percent (to be used for output files).						

	m_thresholdPercent = (float)m_saveThresholdPercent;

			// Thresholds for Correlation Classifier.

	m_angleThreshold = (float)m_saveAngleThreshold;
	m_correlationThreshold = (float)m_saveCorrelationThreshold;

			// Threshold for CEM Classifier.

	m_cemThreshold = (float)m_saveCEMThreshold;

			// Threshold for KNN Classifier.

	m_knnThreshold = (int)m_saveKNNThreshold;

	if (TransferDataToWindow ())
		PositionDialogWindow ();

			// Set default text selection to first edit text item	

	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);

	AdjustDlgLayout ();

}	// end "OnInitDialog"



void CMClassifyDialog::OnListOptions (
				wxCommandEvent& 					event)

{
	SetDLogControlHilite (this, wxID_OK, 255);

	ListResultsOptionsDialog (&m_listResultsTrainingCode, &m_listResultsTestCode);

	SetDLogControlHilite (this, wxID_OK, 0);

}	// end "OnListOptions"



void CMClassifyDialog::OnImageOverlayComboSelendok (
				wxCommandEvent& 					event)

{
    m_selectImageOverlaySelection = m_overlayCtrl->GetSelection ();
	
}	// end "OnImageOverlayComboSelendok"



void CMClassifyDialog::OnPaletteComboCloseUp (
				wxCommandEvent& 					event)

{
	int paletteSelection = m_paletteCtrl->GetSelection ();
	
	if (paletteSelection == -1)
		m_paletteCtrl->SetSelection (m_paletteSelection);
	
	event.Skip ();
	
}	// end "OnPaletteComboCloseUp"



void CMClassifyDialog::OnPaletteComboDropDown (
				wxCommandEvent& 					event)

{
			// The drop down is forced to be -1 setting with each combo
			// box selection so that the 'user selection' box will be opened if
			// it is the last selection. Otherwise it will not open the dialog window.
	
	m_paletteCtrl->SetSelection (-1);
	
}	// end "OnPaletteComboDropDown"



void CMClassifyDialog::OnPaletteComboSelendok (
				wxCommandEvent& 					event)

{
	int 									lastPaletteSelection;


	lastPaletteSelection = m_paletteSelection;
	m_paletteSelection = m_paletteCtrl->GetSelection ();

	if (m_paletteSelection + 1 == kFalseColors)
		{
		if (!FalseColorPaletteDialog ())
			{
			if (lastPaletteSelection != m_paletteSelection)
				{
				m_paletteSelection = lastPaletteSelection;
				m_paletteCtrl->SetSelection (m_paletteSelection);

				}	// end "if (lastPaletteSelection != m_paletteSelection)"

			m_paletteSelection = lastPaletteSelection;

	  		}	// end "else !FalseColorPaletteDialog ()"

		}	// end "if (m_paletteSelection+1 == kFalseColors)"

}	// end "OnPaletteComboSelendok"



void CMClassifyDialog::OnStnClickedStartendinterval ()

{
	
}	// end "OnStnClickedStartendinterval"



void CMClassifyDialog::OnTargetComboSelendok (
				wxCommandEvent& 					event)

{
	int 									savedFileNamesSelection;

	Boolean 								checkOKFlag,
            							createImageOverlayFlag;


	savedFileNamesSelection = m_fileNamesSelection;
	 m_fileNamesSelection = m_targetCtrl->GetSelection ();
	
	if (m_fileNamesSelection != savedFileNamesSelection)
		{
		createImageOverlayFlag = m_createImageOverlayFlag;
		ClassifyDialogOnTargetFile (this,
											 m_fileNamesSelection + 1,
											 &m_targetWindowInfoHandle,
											 &checkOKFlag,
											 &m_dialogSelectArea,
											 &createImageOverlayFlag);
		m_createImageOverlayFlag = createImageOverlayFlag;
		
				// Update the selected area information.
				
		m_LineStart = m_dialogSelectArea.lineStart;
		m_LineEnd = m_dialogSelectArea.lineEnd;
		m_LineInterval = m_dialogSelectArea.lineInterval;
		m_ColumnStart = m_dialogSelectArea.columnStart;
		m_ColumnEnd = m_dialogSelectArea.columnEnd;
		m_ColumnInterval = m_dialogSelectArea.columnInterval;

				// Recheck the image area items. Some may have gotten displayed
				// when they should be hidden.

		if (!m_imageAreaFlag)
			HideShowAreaItems (m_imageAreaFlag);

		if (checkOKFlag)
			{
			wxCheckBox* traincb = (wxCheckBox*)FindWindow (IDC_Training);
			m_trainingAreaFlag = traincb->GetValue ();
			
			wxCheckBox* imagecb = (wxCheckBox*)FindWindow (IDC_TestAreas);
			m_testAreaFlag = imagecb->GetValue ();
			
			wxCheckBox* diskfcb = (wxCheckBox*)FindWindow (IDC_TrainingLOO);
			m_trainingAreaLOOFlag = diskfcb->GetValue ();
			
			wxCheckBox* probfilecb = (wxCheckBox*)FindWindow (IDC_ImageArea);
			m_imageAreaFlag = probfilecb->GetValue ();
			
			CheckAreaSettings ();

			}	// end "if (checkOKFlag)"

    	}	// end "if (m_fileNamesSelection != savedFileNamesSelection)"

}	// end "OnTargetComboSelendok"



void CMClassifyDialog::OnTestAreas (
				wxCommandEvent& 					event)

{
    wxCheckBox* testareacb = (wxCheckBox*)FindWindow (IDC_TestAreas);
    m_testAreaFlag = testareacb->GetValue ();
    HideShowClassAreasItem ();

    CheckAreaSettings ();
    AdjustDlgLayout ();
	
}	// end "OnTestAreas"



void CMClassifyDialog::OnTextWindow (
				wxCommandEvent& 					event)

{
			// Add your control notification handler code here

}	// end "OnTextWindow"



void CMClassifyDialog::OnThresholdResults (
				wxCommandEvent& 					event)

{
    wxCheckBox* thresholdcb = (wxCheckBox*)FindWindow (IDC_ThresholdResults);
    m_thresholdResultsFlag = thresholdcb->GetValue ();
    CheckOutputFormatItems ();
    AdjustDlgLayout ();
	 
}	// end "OnThresholdResults"



void CMClassifyDialog::OnTraining (
				wxCommandEvent& 					event)

{
    wxCheckBox* trainingcb = (wxCheckBox*)FindWindow (IDC_Training);
    m_trainingAreaFlag = trainingcb->GetValue ();
    HideShowClassAreasItem ();

    CheckAreaSettings ();
    AdjustDlgLayout ();
	 
}	// end "OnTraining" 



void CMClassifyDialog::OnTrainingLOO (
				wxCommandEvent& 					event)

{
    wxCheckBox* trainingloocb = (wxCheckBox*)FindWindow (IDC_TrainingLOO);
    m_trainingAreaLOOFlag = trainingloocb->GetValue ();
    
    HideShowClassAreasItem ();

    CheckAreaSettings ();
    AdjustDlgLayout ();
	
}	// end "OnTrainingLOO"



void CMClassifyDialog::ToEntireImage (
				wxCommandEvent& 					event)

{
	SignedByte 							handleStatus;


	m_dialogSelectArea.imageWindowInfoPtr =
            (WindowInfoPtr)GetHandleStatusAndPointer (m_targetWindowInfoHandle,
																		&handleStatus);

	CMDialog::ToEntireImage (event);

	MHSetState (m_targetWindowInfoHandle, handleStatus);

	m_dialogSelectArea.imageWindowInfoPtr = NULL;
    
}	// end "ToEntireImage"



void CMClassifyDialog::ToSelectedImage (
				wxCommandEvent& 					event)

{
	SignedByte 							handleStatus;


	m_dialogSelectArea.imageWindowInfoPtr =
            (WindowInfoPtr)GetHandleStatusAndPointer (m_targetWindowInfoHandle,
																		&handleStatus);

    CMDialog::ToSelectedImage (event);

    MHSetState (m_targetWindowInfoHandle, handleStatus);

    m_dialogSelectArea.imageWindowInfoPtr = NULL;
    
}	// end "ToSelectedImage" 



bool CMClassifyDialog::TransferDataFromWindow ()

{   
   SInt16								returnCode = 0;
	

	wxCheckBox* traincb = (wxCheckBox*)FindWindow (IDC_Training);
	m_trainingAreaFlag = traincb->GetValue ();
	
	wxCheckBox* imagecb = (wxCheckBox*)FindWindow (IDC_ImageArea);
	m_imageAreaFlag = imagecb->GetValue ();
	
	wxCheckBox* diskfcb = (wxCheckBox*)FindWindow (IDC_DiskFile);
	m_diskFileFlag = diskfcb->GetValue ();
	
	wxCheckBox* probfilecb = (wxCheckBox*)FindWindow (IDC_CreateProbabilityFile);
	m_createProbabilityFileFlag = probfilecb->GetValue ();
	
	wxCheckBox* testareacb = (wxCheckBox*)FindWindow (IDC_TestAreas);
	m_testAreaFlag = testareacb->GetValue ();
	
	wxCheckBox* trainloocb = (wxCheckBox*)FindWindow (IDC_TrainingLOO);
	m_trainingAreaLOOFlag = trainloocb->GetValue ();
	
	wxCheckBox* featuretrancb = (wxCheckBox*)FindWindow (IDC_FeatureTransformation);
	m_featureTransformationFlag= featuretrancb->GetValue ();
	
	wxCheckBox* thresholdcb = (wxCheckBox*)FindWindow (IDC_ThresholdResults);
	m_thresholdResultsFlag = thresholdcb->GetValue ();
	
	wxCheckBox* imagewindowcb = (wxCheckBox*)FindWindow (IDC_ImageWindowOverlay);
	m_createImageOverlayFlag = imagewindowcb->GetValue ();

	m_classSelection = m_classesCtrl->GetSelection ();
	m_classWeightsSelection = m_weightsCtrl->GetSelection ();
	m_symbolSelection =    m_symbolCtrl->GetSelection ();
	m_classifyListSelection = m_procedureCtrl->GetSelection ();
	m_classAreaSelection = m_classAreasCtrl->GetSelection ();
	m_fileNamesSelection = m_targetCtrl->GetSelection ();
	m_outputFormatCode = m_fileFormatCtrl->GetSelection ();
	m_paletteSelection = m_paletteCtrl->GetSelection ();
	m_channelSelection = m_channelsCtrl->GetSelection ();
	m_selectImageOverlaySelection = m_overlayCtrl->GetSelection ();
	
	TransferLinesColumnsFromWindow ();

   if (m_thresholdResultsFlag)
		{
      SInt64 classificationProcedure =
				(SInt64)((int*)m_procedureCtrl->GetClientData (m_classifyListSelection));
      m_classificationProcedure = (SInt16)classificationProcedure;
	
      if (m_classificationProcedure == kCorrelationMode)
      	{
         wxTextCtrl* clsfycorrthresh =
         					(wxTextCtrl*)FindWindow (IDC_CorrelationCoefficientThreshold);
			wxString clcorrthresh = clsfycorrthresh->GetValue ();
         clcorrthresh.ToDouble (&m_correlationThreshold);   
			
			wxTextCtrl* clsfycorrcoef =
										(wxTextCtrl*)FindWindow (IDC_CorrelationAngleThreshold);
			wxString clcorrcoef = clsfycorrcoef->GetValue ();
         clcorrcoef.ToDouble (&m_angleThreshold);
			
      	}	// end "if (m_classificationProcedure == kCorrelationMode)"
		
		else if (m_classificationProcedure == kCEMMode)
			{
         wxTextCtrl* clsfyCEMthresh = (wxTextCtrl*)FindWindow (IDC_CEMThreshold);
			wxString clCEMthresh = clsfyCEMthresh->GetValue ();
         clCEMthresh.ToDouble (&m_cemThreshold);
			
      	}	// end "else if (m_classificationProcedure == kCEMMode)"
		
		else if (m_classificationProcedure == kKNearestNeighborMode)
			{
         wxTextCtrl* classifyKnnThreshold =
         							(wxTextCtrl*)FindWindow (IDC_NearestNeighborThreshold);
			wxString clKNNThreshold = classifyKnnThreshold->GetValue ();
         clKNNThreshold.ToLong (&m_knnThreshold);
			
      	}	// end "else if (m_classificationProcedure == kKNearestNeighborMode)"
		
		else	// m_classificationProcedure != kCEMMode || ...
			{
         wxTextCtrl* clsfythresh = (wxTextCtrl*)FindWindow (IDC_ThresholdValue);
			wxString clthresh = clsfythresh->GetValue ();
         clthresh.ToDouble (&m_thresholdPercent);
				
      	}	// end "else m_classificationProcedure != kCEMMode || ..."
		
   	}	// end "if (m_thresholdResultsFlag)"
   
   if (m_classifyListSelection < 0)
      m_classifyListSelection = 0;
   
   SInt64 classificationProcedure =
				(SInt64)((int*)m_procedureCtrl->GetClientData (m_classifyListSelection));
   m_classificationProcedure = (SInt16)classificationProcedure;
   
   if (m_imageAreaFlag)
		returnCode = VerifyLineColumnValues (IDC_LineStart,
															IDC_ColumnStart,
															true);

	return (returnCode == 0);
   
}	// end "TransferDataFromWindow"



bool CMClassifyDialog::TransferDataToWindow ()

{
	wxCheckBox* traincb = (wxCheckBox*)FindWindow (IDC_Training);
	traincb->SetValue (m_trainingAreaFlag);
	
	wxCheckBox* imagecb = (wxCheckBox*)FindWindow (IDC_ImageArea);
	imagecb->SetValue (m_imageAreaFlag);
	
	wxCheckBox* diskfcb = (wxCheckBox*)FindWindow (IDC_DiskFile);
	diskfcb->SetValue (m_diskFileFlag);
	
	wxCheckBox* probfilecb = (wxCheckBox*)FindWindow (IDC_CreateProbabilityFile);
	probfilecb->SetValue (m_createProbabilityFileFlag);
	
	wxCheckBox* testareacb = (wxCheckBox*)FindWindow (IDC_TestAreas);
	testareacb->SetValue (m_testAreaFlag);
	
	wxCheckBox* trainloocb = (wxCheckBox*)FindWindow (IDC_TrainingLOO);
	trainloocb->SetValue (m_trainingAreaLOOFlag);
	
	wxCheckBox* featuretrancb = (wxCheckBox*)FindWindow (IDC_FeatureTransformation);
	featuretrancb->SetValue (m_featureTransformationFlag);
	
	wxCheckBox* thresholdcb = (wxCheckBox*)FindWindow (IDC_ThresholdResults);
	thresholdcb->SetValue (m_thresholdResultsFlag);
	
	wxCheckBox* imagewindowcb = (wxCheckBox*)FindWindow (IDC_ImageWindowOverlay);
	imagewindowcb->SetValue (m_createImageOverlayFlag);

	m_classesCtrl->SetSelection (m_classSelection);
	if (m_classWeightsSelection >= 0)
		m_weightsCtrl->SetSelection (m_classWeightsSelection);
	
	if (m_symbolSelection >= 0)
		m_symbolCtrl->SetSelection (m_symbolSelection);
	
	m_procedureCtrl->SetSelection (m_classifyListSelection);
	m_classAreasCtrl->SetSelection (m_classAreaSelection);

	int fileNamesSelection = 0;
	if (m_fileNamesSelection > 0)
		fileNamesSelection = m_fileNamesSelection;
	m_targetCtrl->SetSelection (fileNamesSelection);

	m_fileFormatCtrl->SetSelection (m_outputFormatCode);
	m_paletteCtrl->SetSelection (m_paletteSelection);
	m_channelsCtrl->SetSelection (m_channelSelection);
	m_overlayCtrl->SetSelection (m_selectImageOverlaySelection);
	
	TransferLinesColumnsToWindow ();

	wxTextCtrl* clsfythresh = (wxTextCtrl*)FindWindow (IDC_ThresholdValue);
	clsfythresh->ChangeValue (wxString::Format (wxT("%.4f"), m_thresholdPercent));
	
	wxTextCtrl* classifyKnnThreshold = (wxTextCtrl*)FindWindow (IDC_NearestNeighborThreshold);
	classifyKnnThreshold->ChangeValue (wxString::Format (wxT("%ld"), m_knnThreshold));
	
	wxTextCtrl* clsfycorrthresh = (wxTextCtrl*)FindWindow (IDC_CorrelationCoefficientThreshold);
	clsfycorrthresh->ChangeValue (wxString::Format (wxT("%.4f"), m_correlationThreshold));
	
	wxTextCtrl* clsfycorrcoef = (wxTextCtrl*)FindWindow (IDC_CorrelationAngleThreshold);
	clsfycorrcoef->ChangeValue (wxString::Format (wxT("%.1f"), m_angleThreshold));
	
	wxTextCtrl* clsfyCEMthresh = (wxTextCtrl*)FindWindow (IDC_CEMThreshold);
	clsfyCEMthresh->ChangeValue (wxString::Format (wxT("%.1f"), m_cemThreshold));
	
	return true;
	 
}	// end "TransferDataToWindow"
