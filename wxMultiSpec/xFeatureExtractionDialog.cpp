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
//	File:						xFeatureExtractionDialog.cpp : class implementation file
//	Class Definition:		xFeatureExtractionDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/28/2019
//
//	Language:				C++
//
//	System:					Linux & MacOS Operating Systems
//
//	Brief description:	This file contains functions related to the
//								CMFeatureExtractionDialog class.
//
// Following is template for debugging
/*
	int numberChars = sprintf ((char*)gTextString3,
									 " xFeatureExtractionDialog:: (): %s",
									 gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
                    
#include "xChannelsDialog.h"  
#include "xFeatureExtractionDialog.h"
#include "xProjectionPursuitDialog.h"



BEGIN_EVENT_TABLE (CMFeatureExtractionDialog, CMDialog)
   EVT_CHECKBOX (IDC_OptimizeClasses, CMFeatureExtractionDialog::OnOptimizeClasses)
	EVT_CHECKBOX (IDC_SpecialOptions, CMFeatureExtractionDialog::OnSpecialOptions)

	EVT_CHOICE (IDC_FEAlgorithmCombo, CMFeatureExtractionDialog::OnDecisionBoundary)

	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_ChannelCombo, CMFeatureExtractionDialog::OnChannelComboSelendok)
		EVT_COMBOBOX (IDC_ClassCombo, CMFeatureExtractionDialog::OnClassComboSelendok)
		EVT_COMBOBOX (IDC_ClassPairWeightsCombo, CMFeatureExtractionDialog::OnClassPairWeightsComboSelendok)
		EVT_COMBOBOX (IDC_PreprocessingCombo, CMFeatureExtractionDialog::OnPreprocessingSelendok)
		EVT_COMBOBOX (IDC_WeightCombo, CMFeatureExtractionDialog::OnClassWeightsComboSelendok)
	#endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_ChannelCombo, CMFeatureExtractionDialog::OnChannelComboSelendok)
		EVT_CHOICE (IDC_ClassCombo, CMFeatureExtractionDialog::OnClassComboSelendok)
		EVT_CHOICE (IDC_ClassPairWeightsCombo, CMFeatureExtractionDialog::OnClassPairWeightsComboSelendok)
		EVT_CHOICE (IDC_PreprocessingCombo, CMFeatureExtractionDialog::OnPreprocessingSelendok)
		EVT_CHOICE (IDC_WeightCombo, CMFeatureExtractionDialog::OnClassWeightsComboSelendok)
	#endif

	EVT_COMBOBOX_CLOSEUP (IDC_ChannelCombo, CMFeatureExtractionDialog::OnChannelComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_ClassCombo, CMFeatureExtractionDialog::OnClassComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_ClassPairWeightsCombo, CMFeatureExtractionDialog::OnClassPairWeightsComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_PreprocessingCombo, CMFeatureExtractionDialog::OnPreprocessingCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_WeightCombo, CMFeatureExtractionDialog::OnClassWeightComboCloseUp)

   EVT_COMBOBOX_DROPDOWN (IDC_ChannelCombo, CMFeatureExtractionDialog::OnChannelComboDropDown)
   EVT_COMBOBOX_DROPDOWN (IDC_ClassCombo, CMFeatureExtractionDialog::OnClassComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_ClassPairWeightsCombo, CMFeatureExtractionDialog::OnClassPairWeightsComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_PreprocessingCombo, CMFeatureExtractionDialog::OnPreprocessingDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_WeightCombo, CMFeatureExtractionDialog::OnClassWeightComboDropDown)

	EVT_INIT_DIALOG (CMFeatureExtractionDialog::OnInitDialog)
END_EVENT_TABLE ()



CMFeatureExtractionDialog::CMFeatureExtractionDialog (
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title)
		: CMDialog (CMFeatureExtractionDialog::IDD, pParent, title)

{
	m_interclassThrehold = (float)0.;
	m_withinClassThreshold = (float)0.;
	m_optimizeClassesFlag = FALSE;
	m_minThresholdNumber = 0;
	m_maxPixelsPerClass = 0;
	m_specialOptionsFlag = FALSE;
	m_listTransformationFlag = FALSE;
	m_classPairWeightsSelection = -1;
	m_preprocessSelection = -1;
	m_algorithmCode = -1;
	m_optimizeThreshold = (float)0.;
	
	m_localClassPairWeightsListPtr = NULL;
	m_localDefaultClassPairWeight = 0;
	
	m_initializedFlag = CMDialog::m_initializedFlag; 
	
	if (m_initializedFlag)
		m_initializedFlag = GetDialogLocalVectors (&m_localActiveFeaturesPtr,
																	NULL,
																	&m_classListPtr,
																	NULL,
																	&m_classWeightsPtr,
																	NULL,
																	NULL,
																	&m_localClassPairWeightsListPtr);
   
   CreateControls ();
	
}	// end "CMFeatureExtractionDialog"



CMFeatureExtractionDialog::~CMFeatureExtractionDialog (void)

{  
	ReleaseDialogLocalVectors (m_localActiveFeaturesPtr,  
											NULL,       
											m_classListPtr,
											NULL,
											m_classWeightsPtr,
											NULL,
											NULL,
											m_localClassPairWeightsListPtr);
	
}	// end "CMFeatureExtractionDialog"



void CMFeatureExtractionDialog::CreateControls ()

{
	SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxBoxSizer* bVSizerMain = new wxBoxSizer (wxVERTICAL);
	
	bSizer250 = new wxBoxSizer (wxHORIZONTAL);
	
	wxBoxSizer* bSizer251;
	bSizer251 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer253;
	bSizer253 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText244 = new wxStaticText (this,
													wxID_ANY,
													wxT("Preprocessing Transformation:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText244->Wrap (-1);
	bSizer253->Add (m_staticText244, 0, wxALL, 5);
	#if defined multispec_wxlin
		m_preprocessingCtrl = new wxComboBox (this,
															IDC_PreprocessingCombo,
															wxT("None"),
															wxDefaultPosition,
															wxSize (210, 25),
															0,
															NULL,
															wxCB_READONLY);
	#endif
	#if defined multispec_wxmac
		m_preprocessingCtrl = new wxChoice (this,
														IDC_PreprocessingCombo,
														wxDefaultPosition,
														wxSize (210, 25));
	#endif
	m_preprocessingCtrl->Append (wxT("None"));
	m_preprocessingCtrl->Append (wxT("Compute new..."));
	m_preprocessingCtrl->Append (wxT("Use last"));
	m_preprocessingCtrl->SetSelection (0);
   SetUpToolTip (m_preprocessingCtrl, IDS_ToolTip137);
	bSizer253->Add (m_preprocessingCtrl, 0, wxLEFT, 15);
	
	bSizer251->Add (bSizer253, 0, wxBOTTOM|wxEXPAND, 5);
	
	wxBoxSizer* bSizer254;
	bSizer254 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText245 = new wxStaticText (this,
													wxID_ANY,
													wxT("Feature Extraction"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText245->Wrap (-1);
	bSizer254->Add (m_staticText245, 0, wxALL, 5);
	
	m_feAlgorithmCtrl = new wxChoice (this,
													IDC_FEAlgorithmCombo,
													wxDefaultPosition,
													wxSize (210, 25));
	m_feAlgorithmCtrl->Append (wxT("Nonparametric Weighting"));
	m_feAlgorithmCtrl->Append (wxT("Decision Boundary"));
	m_feAlgorithmCtrl->Append (wxT("Discriminant Analysis"));
	m_feAlgorithmCtrl->Append (wxT("Preprocessing only"));
   SetUpToolTip (m_feAlgorithmCtrl, IDS_ToolTip138);
	bSizer254->Add (m_feAlgorithmCtrl, 0, wxLEFT, 15);
	
	bSizer251->Add (bSizer254, 0, wxEXPAND, 5);
	
	wxBoxSizer* bSizer255;
	bSizer255 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText246 = new wxStaticText (this,
													IDC_ChannelPrompt,
													wxT("Channels:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText246->Wrap (-1);
   SetUpToolTip (m_staticText246, IDS_ToolTip52);
	bSizer255->Add (m_staticText246, 0, wxALIGN_CENTER|wxALL, 5);

   GetAllSubsetMenuControl (kChannelsMenu,
										this,
										IDC_ChannelCombo,
										100,
										IDS_ToolTip52);
	bSizer255->Add (m_channelsCtrl, 0, wxALIGN_CENTER|wxALL, 5);
	
	bSizer251->Add (bSizer255, 0, wxEXPAND, 5);
	
	wxBoxSizer* bSizer256;
	bSizer256 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText247 = new wxStaticText (this,
													IDC_ClassPrompt,
													wxT("Classes:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText247->Wrap (-1);
   SetUpToolTip (m_staticText247, IDS_ToolTip103);
	bSizer256->Add (m_staticText247, 0, wxALIGN_CENTER|wxALL, 5);

	GetAllSubsetMenuControl (kClassesMenu,
										this,
										IDC_ClassCombo,
										100,
										IDS_ToolTip103);
	bSizer256->Add (m_classesCtrl, 0, wxALL, 5);
	
	bSizer251->Add (bSizer256, 0, wxEXPAND, 5);
	
	wxBoxSizer* bSizer257;
	bSizer257 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText248 = new wxStaticText (this,
													IDC_WeightsPrompt,
													wxT("Class Weights:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText248->Wrap (-1);
   SetUpToolTip (m_staticText248, IDS_ToolTip130);
	bSizer257->Add (m_staticText248, 0, wxALIGN_CENTER|wxALL, 5);

	GetEqualUnequalMenuControl (kWeightsMenu,
											this,
											IDC_WeightCombo,
											120,
											IDS_ToolTip130);
	bSizer257->Add (m_weightsCtrl, 0, wxALL, 5);
	
	m_staticText250 = new wxStaticText (this,
													IDC_WeightsEqual,
													wxT("Equal"),
													wxDefaultPosition,
													wxSize (-1, -1),
													0);
	m_staticText250->Wrap (-1);
	bSizer257->Add (m_staticText250, 0, wxALIGN_CENTER|wxALL, 5);
	
	bSizer251->Add (bSizer257, 0, 0, 5);
	
	wxBoxSizer* bSizer258;
	bSizer258 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText249 = new wxStaticText (this,
													IDC_ClassPairWeightsPrompt,
													wxT("Interclass Weights:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText249->Wrap (-1);
   SetUpToolTip (m_staticText249, IDS_ToolTip139);
	bSizer258->Add (m_staticText249, 0, wxALIGN_CENTER|wxLEFT, 5);

	GetEqualUnequalMenuControl (kClassPairWeightsMenu,
											this,
											IDC_ClassPairWeightsCombo,
											120,
											IDS_ToolTip139);
	bSizer258->Add (m_classPairWeightsCtrl, 0, wxALL, 5);
	
	m_staticText251 = new wxStaticText (this,
													IDC_InterClassWeightsEqual,
													wxT("Equal"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText251->Wrap (-1);
	bSizer258->Add (m_staticText251, 0, wxALIGN_CENTER|wxALL, 5);
	
	bSizer251->Add (bSizer258, 0, 0, 5);
	
	m_checkBox54 = new wxCheckBox (this,
												IDC_ListTransformationMatrix,
												wxT("List Transformation Matrix"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox54, IDS_ToolTip140);
	bSizer251->Add (m_checkBox54, 0, wxALL, 5);
	
	bSizer250->Add (bSizer251, 0, wxLEFT|wxTOP|wxBOTTOM|wxEXPAND, 12);
	
	wxBoxSizer* bSizer252;
	bSizer252 = new wxBoxSizer (wxVERTICAL);
	
	m_checkBox55 = new wxCheckBox (this,
												IDC_SpecialOptions,
												wxT("Special options"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	SetUpToolTip (m_checkBox55, IDS_ToolTip141);
   bSizer252->Add (m_checkBox55, 0, wxALL, 5);
	
	wxBoxSizer* bSizer259;
	bSizer259 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer260;
	bSizer260 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText252 = new wxStaticText (this,
													IDC_WithinClassThresholdPrompt,
													wxT("Within class threshold: "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText252->Wrap (-1);
   SetUpToolTip (m_staticText252, IDS_ToolTip142);
	bSizer260->Add (m_staticText252, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_textCtrl104 = new wxTextCtrl (this,
												IDC_WithinClassThreshold,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (60, -1),
												0);
   m_textCtrl104->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
   																&m_wclassThresString));
   SetUpToolTip (m_textCtrl104, IDS_ToolTip142);
	bSizer260->Add (m_textCtrl104, 0, wxALIGN_CENTER, 5);
	
	bSizer259->Add (bSizer260, 0, wxALIGN_RIGHT|wxALL, 5);
	
	wxBoxSizer* bSizer261;
	bSizer261 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText253 = new wxStaticText (this,
													IDC_InterclassThresholdPrompt,
													wxT("Interclass threshold:     "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText253->Wrap (-1);
   SetUpToolTip (m_staticText253, IDS_ToolTip143);
	bSizer261->Add (m_staticText253, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_textCtrl105 = new wxTextCtrl (this,
												IDC_InterclassThreshold,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (60, -1),
												0);
   m_textCtrl105->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
   																&m_iclassThresString));
   SetUpToolTip (m_textCtrl105, IDS_ToolTip143);
   bSizer261->Add (m_textCtrl105, 0, wxALIGN_CENTER, 5);
	
	bSizer259->Add (bSizer261, 0, wxALIGN_RIGHT|wxALL, 5);
	
	wxBoxSizer* bSizer262;
	bSizer262 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText254 = new wxStaticText (this,
													IDC_MinThresholdNumberPrompt,
													wxT("Min. threshold number:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText254->Wrap (-1);
   SetUpToolTip (m_staticText254, IDS_ToolTip144);
	bSizer262->Add (m_staticText254, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_textCtrl106 = new wxTextCtrl (this,
												IDC_MinThresholdNumber,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (60, -1),
												0);
   m_textCtrl106->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
   																&m_minThrestString));
   SetUpToolTip (m_textCtrl106, IDS_ToolTip144);
	bSizer262->Add (m_textCtrl106, 0, wxALIGN_CENTER, 5);
	
	bSizer259->Add (bSizer262, 0, wxALIGN_RIGHT|wxALL, 5);
	
	bSizer252->Add (bSizer259, 0, wxEXPAND|wxLEFT, 10);
	
	wxBoxSizer* bSizer263;
	bSizer263 = new wxBoxSizer (wxVERTICAL);
	
	m_checkBox56 = new wxCheckBox (this,
												IDC_OptimizeClasses,
												wxT("Optimize classes"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox56, IDS_ToolTip145);
	bSizer263->Add (m_checkBox56, 0, wxLEFT|wxTOP, 10);
	
	wxBoxSizer* bSizer264;
	bSizer264 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText255 = new wxStaticText (this,
													IDC_PercentAccuracyPrompt,
													wxT("Percent of all\rfeature accuracy:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText255->Wrap (-1);
   SetUpToolTip (m_staticText255, IDS_ToolTip146);
	bSizer264->Add (m_staticText255, 0, wxALIGN_CENTER|wxLEFT, 25);
	
	m_textCtrl107 = new wxTextCtrl (this,
												IDC_PercentAccuracy,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (60, -1),
												0);
   m_textCtrl107->SetValidator (wxTextValidator (wxFILTER_NUMERIC, &m_percentString));
   SetUpToolTip (m_textCtrl107, IDS_ToolTip146);
	bSizer264->Add (m_textCtrl107, 0, wxALIGN_CENTER, 5);
	
	bSizer263->Add (bSizer264, 0, wxALIGN_RIGHT, 5);
	
	bSizer252->Add (bSizer263, 0, wxBOTTOM|wxEXPAND|wxLEFT, 10);
	
	wxBoxSizer* bSizer265;
	bSizer265 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText256 = new wxStaticText (this,
													IDC_MaxPixelsPrompt,
													wxT("Maximum training\npixels per class:    "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText256->Wrap (-1);
   SetUpToolTip (m_staticText256, IDS_ToolTip147);
	bSizer265->Add (m_staticText256, 0, wxALIGN_CENTER, 5);
	
	m_textCtrl108 = new wxTextCtrl (this,
												IDC_MaxPixels,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (100, -1),
												0);
   m_textCtrl108->SetValidator (wxTextValidator (wxFILTER_NUMERIC, &m_maxpixeltString));
   SetUpToolTip (m_textCtrl108, IDS_ToolTip147);
	bSizer265->Add (m_textCtrl108, 0, wxALIGN_CENTER|wxALL, 5);
	
	bSizer252->Add (bSizer265, 0, wxALIGN_RIGHT|wxLEFT, 15);

	bSizer250->Add (bSizer252, wxSizerFlags(1).Expand().Border(wxRIGHT|wxTOP, 12));
	
	bVSizerMain->Add (bSizer250, wxSizerFlags(0));
	
	CreateStandardButtons (bVSizerMain);
	
   SetSizerAndFit (bVSizerMain);
	
	Centre (wxBOTH);
	
}	// end "CreateControls"



Boolean CMFeatureExtractionDialog::DoDialog (
				FeatureExtractionSpecsPtr		featureExtractionSpecsPtr)

{                                                  
	SInt16								returnCode;
	
	Boolean								continueFlag = FALSE; 

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																					return (FALSE);
	
	m_featureExtractionSpecsPtr = featureExtractionSpecsPtr;  
	
	returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{
		continueFlag = TRUE;
		
		FeatureExtractionDialogOK (featureExtractionSpecsPtr,
											m_algorithmCode+1,
											m_classWeightsSelection+1,
											m_classWeightsPtr,
											m_classPairWeightsSelection+1,
											m_localClassPairWeightsListPtr,
											m_localDefaultClassPairWeight,
											m_specialOptionsFlag,
											(double)m_withinClassThreshold,
											(double)m_interclassThrehold,
											m_minThresholdNumber,
											(double)m_optimizeThreshold,
											m_optimizeClassesFlag,
											m_maxPixelsPerClass,
											m_channelSelection,
											m_localActiveNumberFeatures,
											m_localActiveFeaturesPtr,
											m_classSelection,
											m_localNumberClasses,
											m_classListPtr,
											m_listTransformationFlag,
											m_preprocessSelection+1);
														
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



void CMFeatureExtractionDialog::OnDecisionBoundary (
				wxCommandEvent& 					event)

{
   m_algorithmCode = m_feAlgorithmCtrl->GetSelection ();
	FeatureExtractionDialogAlgorithm (this, m_algorithmCode+1);
	
	FeatureExtractionDialogUpdateSpecialOptions (this,
																m_algorithmCode+1,
																m_specialOptionsFlag,
																m_optimizeClassesFlag);
	
}	// end "OnDecisionBoundary"



void CMFeatureExtractionDialog::OnInitDialog (
				wxInitDialogEvent& 				event)

{  
	double								interclassThrehold,
											optimizeThreshold,
											withinClassThreshold;

	ProjectionPursuitSpecsPtr		projectionPursuitSpecsPtr;
														
	SInt16								algorithmCode,
											channelSelection,
											classSelection,
											interClassWeightsSelection,
											preprocessSelection,
											weightsSelection;
	

			// Initialize dialog variables.

	FeatureExtractionDialogInitialize (this,
													m_featureExtractionSpecsPtr,
													m_localActiveFeaturesPtr,
													m_classListPtr,
													&m_localClassPairWeightsListPtr,
													&algorithmCode,
													&weightsSelection,
													&interClassWeightsSelection,
													&m_localDefaultClassPairWeight,
													&m_specialOptionsFlag,
													&withinClassThreshold,
													&interclassThrehold,
													(UInt32*)&m_minThresholdNumber,
													&m_optimizeClassesFlag,
													&optimizeThreshold,
													(UInt32*)&m_maxPixelsPerClass,
													&channelSelection,
													&m_localNumberFeatures,
													&m_channelListType,
													&classSelection,
													&m_localNumberClasses,
													&preprocessSelection,
													&m_listTransformationFlag);
	
			//	Set the channels/features list item
			                                             
	m_availableFeaturePtr = gProjectInfoPtr->channelsPtr;
	m_channelSelection = channelSelection;

   		// Set feature parameters
	
	m_localActiveNumberFeatures = m_localNumberFeatures;
   
			// Algorithm.
			// Adjust for 0 base index.
			
	m_algorithmCode = algorithmCode - 1;  
                      
	FeatureExtractionDialogAlgorithm (this, m_algorithmCode+1);							
	HideDialogItem (this, IDC_PreprocessingOnly);
	
			// Class selection.
			
	m_classSelection = classSelection;
	
			// Preprocessing selection.  
			// Adjust for 0 base index.
	
	m_preprocessSelection = preprocessSelection - 1;                            													
	projectionPursuitSpecsPtr = &m_featureExtractionSpecsPtr->projectionPursuitParams;
	if (projectionPursuitSpecsPtr->finalNumberFeatures <= 0)
      m_preprocessingCtrl->Delete (2);
	
			// Class weights.
			// Adjust for 0 base index.
	
	m_classWeightsSelection = weightsSelection - 1;
	HideDialogItem (this, IDC_WeightsEqual);   
	
			// Inter class weights.
			// Adjust for 0 base index.
	
	m_classPairWeightsSelection = interClassWeightsSelection - 1;
	HideDialogItem (this, IDC_InterClassWeightsEqual);
	
	
	
	m_interclassThrehold = (float)interclassThrehold;
	m_optimizeThreshold = (float)optimizeThreshold;
	m_withinClassThreshold = (float)withinClassThreshold;

	                 
	if (TransferDataToWindow ())
		PositionDialogWindow ();
	
}	// end "OnInitDialog"



void CMFeatureExtractionDialog::OnOptimizeClasses (
				wxCommandEvent& 					event)

{         
   wxCheckBox* optimizeClasses = (wxCheckBox*)FindWindow (IDC_OptimizeClasses);
   m_optimizeClassesFlag = optimizeClasses->GetValue ();
	FeatureExtractionDialogOptimizeClass (this, m_optimizeClassesFlag);                                                     
	
}	// end "OnOptimizeClasses"



void CMFeatureExtractionDialog::OnSpecialOptions (
				wxCommandEvent& 					event)

{                                       
	wxCheckBox* specialOptions = (wxCheckBox*)FindWindow (IDC_SpecialOptions);
	m_specialOptionsFlag = specialOptions->GetValue ();
	FeatureExtractionDialogUpdateSpecialOptions (this,
																m_algorithmCode+1,
																m_specialOptionsFlag,
																m_optimizeClassesFlag);

}	// end "OnSpecialOptions"



void CMFeatureExtractionDialog::OnChannelComboSelendok (
				wxCommandEvent& 					event)

{                                                                                                       
	HandleChannelsMenu (IDC_ChannelCombo,
								kNoTransformation,
								(SInt16)gProjectInfoPtr->numberStatisticsChannels,
								m_channelListType,
								TRUE);   
	
}	// end "OnChannelComboSelendok"



void CMFeatureExtractionDialog::OnPreprocessingCloseUp (
				wxCommandEvent& 					event)

{                   
	int preprocessingSelection = m_preprocessingCtrl->GetSelection ();
	
	if (preprocessingSelection == -1)
		m_preprocessingCtrl->SetSelection (m_preprocess_Saved);
	
	event.Skip ();
	
}	// end "OnClassPairWeightsComboCloseUp"



void CMFeatureExtractionDialog::OnPreprocessingDropDown (
				wxCommandEvent& 					event)

{                                                                                                       
	m_preprocess_Saved = m_preprocessingCtrl->GetSelection ();
	m_preprocessingCtrl->SetSelection (-1);
	
}	// end "OnPreprocessingDropDown"



void CMFeatureExtractionDialog::OnClassWeightsComboSelendok (
				wxCommandEvent& 					event)

{                                                           
	HandleClassWeightsMenu ((SInt16)m_localNumberClasses, 
									(SInt16*)m_classListPtr,
									m_classWeightsPtr,
									gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
									IDC_WeightCombo,
									&m_classWeightsSelection);
	
}	// end "OnClassWeightsComboSelendok"



void CMFeatureExtractionDialog::OnClassPairWeightsComboSelendok (
				wxCommandEvent& 					event)

{                                                           
	HandleClassPairWeightsMenu (&m_localClassPairWeightsListPtr,
											IDC_ClassPairWeightsCombo,
											&m_classPairWeightsSelection,
											&m_localDefaultClassPairWeight);
	
}	// end "OnClassPairWeightsComboSelendok"



void CMFeatureExtractionDialog::OnPreprocessingSelendok (
				wxCommandEvent& 					event)

{   
	Boolean					okFlag = FALSE;


   m_preprocessSelection = m_preprocessingCtrl->GetSelection ();
	
	if (m_preprocessSelection == 1)
		okFlag = ProjectionPursuitDialog ();

}	// end "OnPreprocessingSelendok"



bool CMFeatureExtractionDialog::TransferDataFromWindow ()

{
	SInt16 								returnCode = 0;
	
	
   wxTextCtrl* interclassThreshold = (wxTextCtrl*)FindWindow (IDC_InterclassThreshold);
   wxString m_interclassThreholdstring = interclassThreshold->GetValue ();
	double interclassThresholdD;
	m_interclassThreholdstring.ToDouble (&interclassThresholdD);
	m_interclassThrehold = (float)interclassThresholdD;
	
   wxTextCtrl* withinClassThreshold =
   										(wxTextCtrl*)FindWindow (IDC_WithinClassThreshold);
   wxString withinClassThresholdstring = withinClassThreshold->GetValue ();
	double withinClassThresholdD;
	withinClassThresholdstring.ToDouble (&withinClassThresholdD);
	m_withinClassThreshold = (float)withinClassThresholdD;
	
   wxTextCtrl* minThresholdNumber = (wxTextCtrl*)FindWindow (IDC_MinThresholdNumber);
   wxString minThresholdNumberstring = minThresholdNumber->GetValue ();
	m_minThresholdNumber = wxAtoi (minThresholdNumberstring);
	
   wxTextCtrl* maxPixels = (wxTextCtrl*)FindWindow (IDC_MaxPixels);
   wxString maxPixelsstring = maxPixels->GetValue ();
	m_maxPixelsPerClass = wxAtoi (maxPixelsstring);
	
   wxTextCtrl* percentAccuracy = (wxTextCtrl*)FindWindow (IDC_PercentAccuracy);
	wxString percentAccuracystring = percentAccuracy->GetValue ();
   double percentAccuracyD;
	percentAccuracystring.ToDouble (&percentAccuracyD);
   m_optimizeThreshold = (float)percentAccuracyD;
	
   m_classPairWeightsSelection = m_classPairWeightsCtrl->GetSelection ();
	
   m_preprocessSelection = m_preprocessingCtrl->GetSelection ();
	
   m_algorithmCode = m_feAlgorithmCtrl->GetSelection ();
	
   m_channelSelection = m_channelsCtrl->GetSelection ();
	
   m_classSelection = m_classesCtrl->GetSelection ();
	
   m_classWeightsSelection = m_weightsCtrl->GetSelection ();
	
  	wxCheckBox* specialOptions = (wxCheckBox*)FindWindow (IDC_SpecialOptions);
	m_specialOptionsFlag = specialOptions->GetValue ();
	
   wxCheckBox* optimizeClasses = (wxCheckBox*)FindWindow (IDC_OptimizeClasses);
   m_optimizeClassesFlag = optimizeClasses->GetValue ();
	
   wxCheckBox* listTransformationMatrix =
   									(wxCheckBox*)FindWindow (IDC_ListTransformationMatrix);
	m_listTransformationFlag = listTransformationMatrix->GetValue ();
	
   if (m_preprocessSelection < 0)
      m_preprocessSelection = m_preprocess_Saved;
	
   returnCode = CheckDialogRealValue (this,
													IDC_MaxPixels,
													1,
													32000,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
	
   if (returnCode == 0)
      returnCode = CheckDialogRealValue (this,
													IDC_PercentAccuracy,
													0.,
													100.,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
	
   if (returnCode == 0)
      returnCode = CheckDialogRealValue (this,
													IDC_WithinClassThreshold,
													0,
													1,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
	
   if (returnCode == 0)
      returnCode = CheckDialogRealValue (this,
													IDC_InterclassThreshold,
													0,
													1,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
	
   if (returnCode == 0)
      returnCode = CheckDialogRealValue (this,
													IDC_MinThresholdNumber,
													1,
													SInt16_MAX,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
	
   return (returnCode == 0);
	
}	// end "TransferDataFromWindow"



bool CMFeatureExtractionDialog::TransferDataToWindow ()

{
   wxTextCtrl* interclassThreshold = (wxTextCtrl*)FindWindow (IDC_InterclassThreshold);
   interclassThreshold->ChangeValue (
										wxString::Format (wxT("%.2f"), m_interclassThrehold));
	
   wxTextCtrl* withinClassThreshold =
   										(wxTextCtrl*)FindWindow (IDC_WithinClassThreshold);
   withinClassThreshold->ChangeValue (
   									wxString::Format (wxT("%.2f"), m_withinClassThreshold));
	
   wxTextCtrl* percentAccuracy = (wxTextCtrl*)FindWindow (IDC_PercentAccuracy);
	percentAccuracy->ChangeValue (wxString::Format (wxT("%.1f"), m_optimizeThreshold));
	
   wxTextCtrl* minThresholdNumber = (wxTextCtrl*)FindWindow (IDC_MinThresholdNumber);
   minThresholdNumber->ChangeValue (
   									wxString::Format (wxT("%i"), m_minThresholdNumber));
	
   wxTextCtrl* maxPixels = (wxTextCtrl*)FindWindow (IDC_MaxPixels);
   maxPixels->ChangeValue (wxString::Format (wxT("%i"), m_maxPixelsPerClass));
	
   m_classPairWeightsCtrl->SetSelection (m_classPairWeightsSelection);
	
   m_preprocessingCtrl->SetSelection (m_preprocessSelection);
	
   m_feAlgorithmCtrl->SetSelection (m_algorithmCode);
	
   m_channelsCtrl->SetSelection (m_channelSelection);
	
   m_classesCtrl->SetSelection (m_classSelection);
	
   m_weightsCtrl->SetSelection (m_classWeightsSelection);
	
   wxCheckBox* specialOptions = (wxCheckBox*)FindWindow (IDC_SpecialOptions);
   specialOptions->SetValue (m_specialOptionsFlag);
	
   wxCheckBox* optimizeClasses = (wxCheckBox*)FindWindow (IDC_OptimizeClasses);
   optimizeClasses->SetValue (m_optimizeClassesFlag);
	
   wxCheckBox* listTransformationMatrix =
   									(wxCheckBox*)FindWindow (IDC_ListTransformationMatrix);
	listTransformationMatrix->SetValue (m_listTransformationFlag);
	
   return true;
	
}
