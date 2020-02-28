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
//	File:						xFeatureSelectionDialog.cpp : class implementation file
//	Class Definition:		xFeatureSelectionDialog.h
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
//								CMFeatureSelectionDialog class.
//
// Following is template for debugging
/*
	int numberChars = sprintf ((char*)gTextString3,
									 " xFeatureSelectionDialog:: (): %s",
									 gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "wx/cshelp.h"
#include "wx/aboutdlg.h"

#include "SMultiSpec.h"

#include "xChannelsDialog.h"
#include "xClassPairWeightsDialog.h"
#include "xFeatureSelectionDialog.h"
#include "xMultiSpec.h"



BEGIN_EVENT_TABLE (CMFeatureSelectionDialog, CMDialog)
	EVT_BUTTON (IDS_HelpButton , CMFeatureSelectionDialog::OnHelpButton)
	EVT_BUTTON (IDC_ListOptions, CMFeatureSelectionDialog::OnListOptions)

	EVT_CHECKBOX (IDC_FeatureTransformation, CMFeatureSelectionDialog::OnFeatureTransformation)
	EVT_CHECKBOX (IDC_StepProcedure, CMFeatureSelectionDialog::OnStepProcedure)

	EVT_CHOICE (IDC_DistanceMeasureCombo, CMFeatureSelectionDialog::OnDistanceMeasureComboSelendok)

	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_ChannelCombo, CMFeatureSelectionDialog::OnChannelComboSelendok)
		EVT_COMBOBOX (IDC_ClassCombo, CMFeatureSelectionDialog::OnClassComboSelendok)
		EVT_COMBOBOX (IDC_ClassPairWeightsCombo, CMFeatureSelectionDialog::OnClassPairWeightsComboSelendok)
		EVT_COMBOBOX (IDC_NumberChannelsCombo, CMFeatureSelectionDialog::OnNumberChannelsComboSelendok)
	#endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_ChannelCombo, CMFeatureSelectionDialog::OnChannelComboSelendok)
		EVT_CHOICE (IDC_ClassCombo, CMFeatureSelectionDialog::OnClassComboSelendok)
		EVT_CHOICE (IDC_ClassPairWeightsCombo, CMFeatureSelectionDialog::OnClassPairWeightsComboSelendok)
		EVT_CHOICE (IDC_NumberChannelsCombo, CMFeatureSelectionDialog::OnNumberChannelsComboSelendok)
	#endif


	EVT_COMBOBOX_CLOSEUP (IDC_ChannelCombo, CMFeatureSelectionDialog::OnChannelComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_ClassCombo, CMFeatureSelectionDialog::OnClassComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_ClassPairWeightsCombo, CMFeatureSelectionDialog::OnClassPairWeightsComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_NumberChannelsCombo, CMFeatureSelectionDialog::OnNumberChannelsComboCloseUp)

	EVT_COMBOBOX_DROPDOWN (IDC_ChannelCombo, CMFeatureSelectionDialog::OnChannelComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_ClassCombo, CMFeatureSelectionDialog::OnClassComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_ClassPairWeightsCombo, CMFeatureSelectionDialog::OnClassPairWeightsComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_NumberChannelsCombo, CMFeatureSelectionDialog::OnNumberChannelsComboDropDown)

	EVT_INIT_DIALOG (CMFeatureSelectionDialog::OnInitDialog)

	EVT_TEXT (IDC_ChannelsPerGroup, CMFeatureSelectionDialog::OnChangeChannelsPerGroup)
	EVT_TEXT (IDC_NumberBestToList, CMFeatureSelectionDialog::OnChangeNumberBestToList)
END_EVENT_TABLE ()



CMFeatureSelectionDialog::CMFeatureSelectionDialog (
				wxWindow* 								parent,
				wxWindowID 								id,
				const wxString& 						title)
		: CMDialog (CMFeatureSelectionDialog::IDD, parent, title)

{
   m_separabilityDistance = -1;
   m_savedContiguousChannelsPerGroup = 1;
   m_localCombinationsToList = 1;
   m_textWindowFlag = FALSE;
   m_diskFileFlag = FALSE;
   m_channelCombinationSelection = -1;
   m_classPairWeightsSelection = -1;
   m_searchFlag = FALSE;

   m_separabilitySpecsPtr = NULL;
   m_localChannelCombinationsPtr = NULL;
   m_maxContiguousChannelsPerGroup = 1;
   m_localClassPairWeightsListPtr = NULL;
   m_localDefaultClassPairWeight = 0;

   m_initializedFlag = CMDialog::m_initializedFlag;

   if (m_initializedFlag)
      m_initializedFlag = GetDialogLocalVectors (&m_localFeaturesPtr,
																	&m_localTransformFeaturesPtr,
																	&m_classListPtr,
																	NULL,
																	NULL,
																	&m_localSymbolsPtr,
																	&m_localChannelCombinationsPtr,
																	&m_localClassPairWeightsListPtr);
	
   CreateControls ();

}	// end "CMFeatureSelectionDialog"



CMFeatureSelectionDialog::~CMFeatureSelectionDialog (void)

{
   ReleaseDialogLocalVectors (m_localFeaturesPtr,
										m_localTransformFeaturesPtr,
										m_classListPtr,
										NULL,
										NULL,
										m_localSymbolsPtr,
										m_localChannelCombinationsPtr,
										m_localClassPairWeightsListPtr);

} 	// end "~CMFeatureSelectionDialog"



void CMFeatureSelectionDialog::CreateControls ()

{
   SetSizeHints (wxDefaultSize, wxDefaultSize);

   wxBoxSizer* bVSizerMain = new wxBoxSizer (wxVERTICAL);
	
   bSizer211 = new wxBoxSizer (wxHORIZONTAL);

   wxBoxSizer* bSizer212;
   bSizer212 = new wxBoxSizer (wxVERTICAL);

   wxBoxSizer* bSizer214;
   bSizer214 = new wxBoxSizer (wxVERTICAL);

   m_staticText213 = new wxStaticText (this,
   												wxID_ANY,
   												wxT("Distance measure:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText213->Wrap (-1);
   bSizer214->Add (m_staticText213, 0, wxALL, 5);

   m_distanceMeasureCtrl = new wxChoice (this,
														IDC_DistanceMeasureCombo,
														wxDefaultPosition,
														wxSize (230, -1));
	m_distanceMeasureCtrl->Append (wxT("Bhattacharyya"));
   m_distanceMeasureCtrl->Append (wxT("Error function Bhattacharyya"));
   m_distanceMeasureCtrl->Append (wxT("'Mean' Bhattacharyya"));
   m_distanceMeasureCtrl->Append (wxT("'Covariance' Bhattacharyya"));
   m_distanceMeasureCtrl->Append (wxT("'Noncovariance' Bhattacharyya"));
   m_distanceMeasureCtrl->Append (wxT("Transformed divergence"));
   m_distanceMeasureCtrl->Append (wxT("Divergence"));
   SetUpToolTip (m_distanceMeasureCtrl, IDS_ToolTip149);
   bSizer214->Add (m_distanceMeasureCtrl, 0, wxLEFT, 25);

   m_checkBox50 = new wxCheckBox (this,
   											IDC_FeatureTransformation,
   											wxT("Use feature transformation"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_checkBox50, IDS_ToolTip150);
   bSizer214->Add (m_checkBox50, 0, wxLEFT | wxTOP, 15);


   bSizer212->Add (bSizer214, 0, wxEXPAND, 5);

   wxBoxSizer* bSizer215;
   bSizer215 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText214 = new wxStaticText (this,
   												IDC_ChannelPrompt,
   												wxT("Channels:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText214->Wrap (-1);
   SetUpToolTip (m_staticText214, IDS_ToolTip52);
   bSizer215->Add (m_staticText214, 0, wxALIGN_CENTER | wxALL, 5);

   GetAllSubsetMenuControl (kChannelsMenu,
										this,
										IDC_ChannelCombo,
										100,
										IDS_ToolTip52);
   bSizer215->Add (m_channelsCtrl, 0, wxALIGN_CENTER | wxALL, 5);

   bSizer212->Add (bSizer215, 0, wxEXPAND, 5);

   wxStaticBoxSizer* sbSizer19;
   sbSizer19 = new wxStaticBoxSizer (new wxStaticBox (this,
																		wxID_ANY,
																		wxT("Channel Combinations")),
													wxVERTICAL);

   wxBoxSizer* bSizer216;
   bSizer216 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText215 = new wxStaticText (sbSizer19->GetStaticBox (),
   												IDC_NumberChannelsPrompt,
   												wxT("Number of channels:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText215->Wrap (-1);
   SetUpToolTip (m_staticText215, IDS_ToolTip151);
   bSizer216->Add (m_staticText215, 0, wxALIGN_CENTER | wxALL, 5);

   GetAllSubsetMenuControl (kChannelsPossibleMenu,
										sbSizer19->GetStaticBox (),
										IDC_NumberChannelsCombo,
										140,
										IDS_ToolTip151);
   bSizer216->Add (m_channelsPossibleCtrl, 0, wxALIGN_CENTER | wxALL, 5);

   sbSizer19->Add (bSizer216, 0, wxEXPAND | wxLEFT, 15);

   wxBoxSizer* bSizer217;
   bSizer217 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText216 = new wxStaticText (sbSizer19->GetStaticBox (),
   												IDC_ChannelsPerGroupPrompt,
   												wxT("Contiguous channels per group:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText216->Wrap (-1);
	SetUpToolTip (m_staticText216, IDS_ToolTip152);
   bSizer217->Add (m_staticText216, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl120 = new wxTextCtrl (sbSizer19->GetStaticBox (),
   											IDC_ChannelsPerGroup,
   											wxEmptyString,
   											wxDefaultPosition,
   											wxSize (50, -1),
   											0);
   m_textCtrl120->SetValidator (wxTextValidator (wxFILTER_DIGITS,
   																&m_channelgroupString));
   SetUpToolTip (m_textCtrl120, IDS_ToolTip152);
   bSizer217->Add (m_textCtrl120, 0, wxALL, 5);

   sbSizer19->Add (bSizer217, 1, wxEXPAND | wxLEFT, 15);

   m_checkBox47 = new wxCheckBox (sbSizer19->GetStaticBox (),
   											IDC_StepProcedure,
   											wxT("Use step procedure"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_checkBox47, IDS_ToolTip153);
   sbSizer19->Add (m_checkBox47, 0, wxLEFT, 20);

   wxBoxSizer* bSizer218;
   bSizer218 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText217 = new wxStaticText (sbSizer19->GetStaticBox (),
													wxID_ANY,
													wxT("Max search combinations:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText217->Wrap (-1);
   SetUpToolTip (m_staticText217, IDS_ToolTip154);
   bSizer218->Add (m_staticText217, 0, wxALIGN_CENTER | wxALL, 5);

   m_staticText218 = new wxStaticText (sbSizer19->GetStaticBox (),
   												IDC_MaxSearchCombinations,
   												wxT("MyLabel"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText218->Wrap (-1);
   SetUpToolTip (m_staticText218, IDS_ToolTip154);
   bSizer218->Add (m_staticText218, 0, wxALIGN_CENTER | wxALL, 5);

   sbSizer19->Add (bSizer218, 0, wxEXPAND | wxLEFT, 15);

   wxBoxSizer* bSizer219;
   bSizer219 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText219 = new wxStaticText (sbSizer19->GetStaticBox (),
   												wxID_ANY,
   												wxT("Max number combinations to list:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText219->Wrap (-1);
   SetUpToolTip (m_staticText219, IDS_ToolTip155);
   bSizer219->Add (m_staticText219, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl121 = new wxTextCtrl (sbSizer19->GetStaticBox (),
   											IDC_NumberBestToList,
   											wxEmptyString,
   											wxDefaultPosition,
   											wxSize (50, -1),
   											0);
   SetUpToolTip (m_textCtrl121, IDS_ToolTip155);
   m_textCtrl121->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_numBestString));
   bSizer219->Add (m_textCtrl121, 0, wxALL, 5);

   sbSizer19->Add (bSizer219, 0, wxEXPAND | wxLEFT, 15);

   bSizer212->Add (sbSizer19, 0, wxEXPAND, 5);

   bSizer211->Add (bSizer212, 0, wxALL | wxEXPAND, 12);

   wxBoxSizer* bSizer213;
   bSizer213 = new wxBoxSizer (wxVERTICAL);

   wxBoxSizer* bSizer220;
   bSizer220 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText221 = new wxStaticText (this,
   												IDC_ClassPrompt,
   												wxT("Classes:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText221->Wrap (-1);
   SetUpToolTip (m_staticText221, IDS_ToolTip103);
   bSizer220->Add (m_staticText221, 0, wxALIGN_CENTER | wxALL, 5);

	GetAllSubsetMenuControl (kClassesMenu,
										this,
										IDC_ClassCombo,
										100,
										IDS_ToolTip103);
   bSizer220->Add (m_classesCtrl, 0, wxALIGN_CENTER | wxALL, 5);

   bSizer213->Add (bSizer220, 0, wxEXPAND, 5);

   wxBoxSizer* bSizer221;
   bSizer221 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText222 = new wxStaticText (this,
   												IDC_NumberClassCombinations,
   												wxT("MyLabel"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText222->Wrap (-1);
   bSizer221->Add (m_staticText222, 0, wxALIGN_CENTER | wxALL, 5);
   m_staticText223 = new wxStaticText (this,
   												wxID_ANY,
   												wxT("combinations"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText223->Wrap (-1);
   bSizer221->Add (m_staticText223, 0, wxALL, 5);

   bSizer213->Add (bSizer221, 0, wxEXPAND | wxLEFT, 25);

   wxBoxSizer* bSizer222 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText224 = new wxStaticText (this,
   												IDC_ClassPairWeightsPrompt,
   												wxT("Weights:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText224->Wrap (-1);
   SetUpToolTip (m_staticText224, IDS_ToolTip156);
   bSizer222->Add (m_staticText224, 0, wxALIGN_CENTER | wxALL, 5);

	GetEqualUnequalMenuControl (kClassPairWeightsMenu,
											this,
											IDC_ClassPairWeightsCombo,
											120,
											IDS_ToolTip156);
   bSizer222->Add (m_classPairWeightsCtrl, 0, wxALIGN_CENTER | wxALL, 5);

   m_staticText229 = new wxStaticText (this,
   												IDC_WeightsEqual,
   												wxT("Equal"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText229->Wrap (-1);
   bSizer222->Add (m_staticText229, 0, wxALIGN_CENTER | wxALL, 5);

   bSizer213->Add (bSizer222, 0, wxEXPAND, 5);

   wxBoxSizer* bSizer223 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText225 = new wxStaticText (this,
   												IDC_SymbolPrompt,
   												wxT("Symbols:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText225->Wrap (-1);
   bSizer223->Add (m_staticText225, 0, wxALIGN_CENTER | wxALL, 5);

   m_comboBox40 = new wxComboBox (this,
   											IDC_SymbolCombo,
   											wxT("Combo!"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0,
   											NULL,
   											wxCB_READONLY);
   m_comboBox40->Append (wxT("Default set"));
   m_comboBox40->Append (wxT("User defined..."));
   SetUpToolTip (m_comboBox40, IDS_ToolTip157);
   bSizer223->Add (m_comboBox40, 0, wxALIGN_CENTER | wxALL, 5);

   m_staticText228 = new wxStaticText (this,
   												IDC_SymbolsDefaultSet,
   												wxT("Default set"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText228->Wrap (-1);
   bSizer223->Add (m_staticText228, 0, wxALIGN_CENTER | wxALL, 5);

   bSizer213->Add (bSizer223, 0, wxEXPAND, 5);

   m_button46 = new wxButton (this,
   									IDC_ListOptions,
   									wxT("List Options..."),
   									wxDefaultPosition,
   									wxSize (180, -1),
   									0);
   SetUpToolTip (m_button46, IDS_ToolTip158);
   bSizer213->Add (m_button46, 0, wxALL, 5);

   wxBoxSizer* bSizer224;
   bSizer224 = new wxBoxSizer (wxVERTICAL);

   m_staticText226 = new wxStaticText (this,
   												wxID_ANY,
   												wxT("Write results to:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText226->Wrap (-1);
   bSizer224->Add (m_staticText226, 0, wxALL, 5);

   m_checkBox48 = new wxCheckBox (this,
   											IDC_TextWindow,
   											wxT("project text window"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_checkBox48, IDS_ToolTip159);
   bSizer224->Add (m_checkBox48, 0, wxLEFT, 15);

   m_checkBox49 = new wxCheckBox (this,
   											IDC_DiskFile,
   											wxT("disk file"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_checkBox49, IDS_ToolTip160);
   bSizer224->Add (m_checkBox49, 0, wxLEFT, 15);

   bSizer213->Add (bSizer224, 0, wxEXPAND);

   bSizer211->Add (bSizer213, 0, wxEXPAND | wxLEFT|wxTOP|wxRIGHT, 12);
	
   bVSizerMain->Add (bSizer211, 0);
	
	CreateStandardButtons (bVSizerMain);

   SetSizer (bVSizerMain);
   Layout ();

   Centre (wxBOTH);
	
}	// end "CreateControls"



Boolean CMFeatureSelectionDialog::DoDialog (
				SeparabilitySpecsPtr 			separabilitySpecsPtr,
				UInt16* 								allChanCombinationsPtr)

{
   SInt16 								returnCode;

   Boolean 								continueFlag = FALSE;
   SInt16 								index;
	

   		// Make sure initialization has been completed.

   if (!m_initializedFlag)
      																			return (FALSE);

   m_separabilitySpecsPtr = separabilitySpecsPtr;

   m_allChanCombinationsPtr = allChanCombinationsPtr;

   returnCode = ShowModal ();

   while (m_separabilitySpecsPtr->maxNumberFeatureCombinations <= 0 &&
																		returnCode == wxID_OK)
		
		{
		index = IDS_FeatureSelection11;
		if (m_featureTransformationFlag)
			index = IDS_FeatureSelection12;

		if (m_savedContiguousChannelsPerGroup > 1)
			index = IDS_FeatureSelection13;

		SetDLogControlHilite (NULL, wxID_OK, 255);
		DisplayAlert (kErrorAlertID, 1, kFeatureSelectionStrID, index, 0, NULL);
		SetDLogControlHilite (NULL, wxID_OK, 0);
		returnCode = ShowModal ();
		
   	}	// end "if m_separabilitySpecsPtr->maxNumberFeatureCombinations <= 0)
   
   if (returnCode == wxID_OK)
		{
      continueFlag = TRUE;
      
      SeparabilityDialogOK (separabilitySpecsPtr,
										m_separabilityDistance,
										m_featureTransformationFlag,
										m_channelSelection,
										m_localActiveNumberFeatures,
										m_localActiveFeaturesPtr,
										m_channelCombinationSelection,
										m_localNumberChannelGroupCombinations,
										m_localChannelCombinationsPtr,
										m_savedContiguousChannelsPerGroup,
										m_searchFlag,
										m_localCombinationsToList,
										m_classSelection,
										m_localNumberClasses,
										m_classListPtr,
										m_classPairWeightsSelection + 1,
										m_localClassPairWeightsListPtr,
										m_localDefaultClassPairWeight,
										m_symbolSelection + 1,
										m_localSymbolsPtr,
										m_textWindowFlag,
										m_diskFileFlag);
		
   	}	// end "if (returnCode == IDOK)"

   return (continueFlag);

}	// end "DoDialog"



void CMFeatureSelectionDialog::HandleChannelCombinationsMenu ()

{
   Boolean 								returnFlag;


   m_channelCombinationSelection = m_channelsPossibleCtrl->GetSelection ();

   if (m_channelCombinationSelection == kSubsetMenuItem)
		{
      		// Subset of channels to be used.
		
      SetDLogControlHilite (NULL, wxID_OK, 255);
      CMChannelCombinationsDlg* channelCombinationsDialogPtr = NULL;

      try
			{
         channelCombinationsDialogPtr = new CMChannelCombinationsDlg ();

         returnFlag = channelCombinationsDialogPtr->DoDialog (
											m_channelCombinationSelection,
											&m_localNumberChannelGroupCombinations,
											m_localChannelCombinationsPtr,
											m_separabilitySpecsPtr->numberAllChanCombinations,
											m_allChanCombinationsPtr,
											m_savedContiguousChannelsPerGroup,
											m_featureTransformationFlag);

         if (channelCombinationsDialogPtr != NULL)
            delete channelCombinationsDialogPtr;
			
			}
		
		catch (int e)
			{
         MemoryMessage (0, kObjectMessage);
			
      	}	// end catch

      if (m_localNumberChannelGroupCombinations ==
											m_separabilitySpecsPtr->numberAllChanCombinations)
         m_channelCombinationSelection = kAllMenuItem;

      if (m_channelCombinationSelection == kAllMenuItem)
         m_channelsPossibleCtrl->SetSelection (m_channelCombinationSelection);

      SetDLogControlHilite (NULL, wxID_OK, 0);

   	}	// end "if (m_channelCombinationsSelection == kSubsetMenuItem)"

}	// end "HandleChannelCombinationsMenu"



void CMFeatureSelectionDialog::OnInitDialog (
				wxInitDialogEvent& 				event)

{
   SInt16 								channelCombinationSelection,
											channelSelection,
											classSelection,
											interClassWeightsSelection,
											symbolSelection;


   		// Initialize dialog variables.

   SeparabilityDialogInitialize (this,
											m_separabilitySpecsPtr,
											m_localFeaturesPtr,
											m_localTransformFeaturesPtr,
											m_classListPtr,
											m_localSymbolsPtr,
											m_localChannelCombinationsPtr,
											&m_localClassPairWeightsListPtr,
											&m_separabilityDistance,
											&m_numberEigenvectors,
											&m_featureTransformAllowedFlag,
											&m_featureTransformationFlag,
											&channelSelection,
											&m_localActiveNumberFeatures,
											&channelCombinationSelection,
											&m_localNumberChannelGroupCombinations,
											&m_savedNumberChannelGroupCombinations,
											&m_savedContiguousChannelsPerGroup,
											&m_searchFlag,
											&m_maxContiguousChannelsPerGroup,
											&m_localCombinationsToList,
											&classSelection,
											&m_localNumberClasses,
											&m_numberClassCombinations,
											&interClassWeightsSelection,
											&m_localDefaultClassPairWeight,
											&symbolSelection,
											&m_textWindowFlag,
											&m_diskFileFlag);

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

   		// Get the feature selection algorithm list selection that matches the input
   		// feature selection algorithm.

   m_separabilityListSelection = GetComboListSelection (IDC_DistanceMeasureCombo,
      																	m_separabilityDistance);
 
   if (m_separabilityListSelection == -1)
      m_separabilityListSelection = 0;

   		//	Set the channels/features list item

   m_availableFeaturePtr = gProjectInfoPtr->channelsPtr;
   m_channelSelection = channelSelection;

   		// Number channel combinations.

   m_channelCombinationSelection = channelCombinationSelection;

			// Current maximum number of channel combinations

   LoadDItemRealValue (this,
								IDC_MaxSearchCombinations,
								(double)m_separabilitySpecsPtr->maxNumberFeatureCombinations,
								0);

   		// Classes to use.

   m_classSelection = classSelection;
   ::LoadDItemValue (this, IDC_NumberClassCombinations, m_numberClassCombinations);

			// Interclass weights to use.
			// Adjust for 0 base index.
			// Unequal interclass weights are not available yet.																	

   m_classPairWeightsSelection = interClassWeightsSelection - 1;
   HideDialogItem (this, IDC_WeightsEqual);

			// Symbols to use.
			// Adjust for 0 base index.
			// User defined symbols are not available yet.																	

   m_symbolSelection = symbolSelection - 1;
   wxComboBox* symbolcombo = (wxComboBox*)FindWindow (IDC_SymbolCombo);
   if (symbolcombo->HasClientData ())
      symbolcombo->Delete (1);
   
   HideDialogItem (this, IDC_SymbolCombo);


   if (TransferDataToWindow ())
      PositionDialogWindow ();

   		// Set default text selection to first edit text item

   SelectDialogItemText (this, IDC_ChannelsPerGroup, 0, SInt16_MAX);

	Layout ();
   Fit ();
	
}	// end "OnInitDialog"



void CMFeatureSelectionDialog::OnDistanceMeasureComboSelendok (
				wxCommandEvent& 					event)

{
	SInt16 								channelSelection,
      									separabilityDistance,
      									savedSeparabilityListSelection;


   savedSeparabilityListSelection = m_separabilityListSelection;

   m_separabilityListSelection = m_distanceMeasureCtrl->GetSelection ();
	
   		// Get the actual classification procedure code.

   SInt64 separabilityDistance64 = (SInt64)((int*)m_distanceMeasureCtrl->
   												GetClientData (m_separabilityListSelection));
   separabilityDistance = (SInt16)separabilityDistance64;


   if (separabilityDistance != 0)
		{
      m_separabilityDistance = separabilityDistance;

      m_featureTransformAllowedFlag =
							SeparabilityDialogGetFeatureTransformAllowedFlag (
																			m_separabilityDistance,
																			m_numberEigenvectors);

      SeparabilityDialogCheckFeatureTransformation (
														this,
														&m_featureTransformationFlag,
														&m_localActiveNumberFeatures,
														&m_localActiveFeaturesPtr,
														m_featureTransformAllowedFlag,
														IDC_FeatureTransformation,
														IDC_ChannelPrompt,
														&channelSelection,
														m_localNumberFeatures,
														m_localFeaturesPtr,
														m_localNumberTransformFeatures,
														m_localTransformFeaturesPtr);

      m_channelSelection = channelSelection;
		
      m_channelsCtrl->SetSelection (m_channelSelection);

      if (m_featureTransformationFlag && !m_featureTransformAllowedFlag)
			{
         SeparabilityDialogUpdateChannelCombinationItems (
															this,
															m_separabilitySpecsPtr,
															TRUE,
															m_localActiveNumberFeatures,
															m_allChanCombinationsPtr,
															m_savedContiguousChannelsPerGroup,
															m_searchFlag,
															m_channelCombinationSelection,
															m_localChannelCombinationsPtr,
															&m_localNumberChannelGroupCombinations,
															&m_savedNumberChannelGroupCombinations,
															&m_localCombinationsToList,
															&m_maxContiguousChannelsPerGroup);

         SeparabilityDialogUpdateChannelFeatureGroupText (
													this,
													(SInt16)m_savedContiguousChannelsPerGroup,
													m_featureTransformationFlag);

      	}	// end "if (m_featureTransformFlag && !m_featureTransformAllowedFlag)"

   	}	// end "if (separabilityDistance != 0)"

   else	// separabilityDistance == 0
      m_separabilityListSelection = savedSeparabilityListSelection;

   m_distanceMeasureCtrl->SetSelection (m_separabilityListSelection);

   separabilityDistance64 = (SInt64)((int*)m_distanceMeasureCtrl->
													GetClientData (m_separabilityListSelection));
   m_separabilityDistance = (SInt16)separabilityDistance;

}	// end "OnDistanceMeasureComboSelendok"



void CMFeatureSelectionDialog::OnFeatureTransformation (
				wxCommandEvent& 					event)

{
	SInt16 								channelSelection;


   wxCheckBox* feature = (wxCheckBox*)FindWindow (IDC_FeatureTransformation);
   m_featureTransformationFlag = feature->GetValue ();

   SeparabilityDialogCheckFeatureTransformation (
   													this,
														&m_featureTransformationFlag,
														&m_localActiveNumberFeatures,
														&m_localActiveFeaturesPtr,
														m_featureTransformAllowedFlag,
														IDC_FeatureTransformation,
														IDC_ChannelPrompt,
														&channelSelection,
														m_localNumberFeatures,
														m_localFeaturesPtr,
														m_localNumberTransformFeatures,
														m_localTransformFeaturesPtr);

   m_channelSelection = channelSelection;

   m_channelsCtrl->SetSelection (m_channelSelection);


   SeparabilityDialogUpdateChannelCombinationItems (
														this,
														m_separabilitySpecsPtr,
														TRUE,
														m_localActiveNumberFeatures,
														m_allChanCombinationsPtr,
														m_savedContiguousChannelsPerGroup,
														m_searchFlag,
														m_channelCombinationSelection,
														m_localChannelCombinationsPtr,
														&m_localNumberChannelGroupCombinations,
														&m_savedNumberChannelGroupCombinations,
														&m_localCombinationsToList,
														&m_maxContiguousChannelsPerGroup);

   SeparabilityDialogUpdateChannelFeatureGroupText (
														this,
														(SInt16)m_savedContiguousChannelsPerGroup,
														m_featureTransformationFlag);

}	// end "OnFeatureTransformation"



void CMFeatureSelectionDialog::OnChannelComboSelendok (
				wxCommandEvent& 					event)

{
   HandleChannelsMenu (IDC_ChannelCombo,
								m_featureTransformationFlag,
								(SInt16)gProjectInfoPtr->numberStatisticsChannels,
								2,
								TRUE);

   SeparabilityDialogUpdateChannelCombinationItems (
														this,
														m_separabilitySpecsPtr,
														TRUE,
														m_localActiveNumberFeatures,
														m_allChanCombinationsPtr,
														m_savedContiguousChannelsPerGroup,
														m_searchFlag,
														m_channelCombinationSelection,
														m_localChannelCombinationsPtr,
														&m_localNumberChannelGroupCombinations,
														&m_savedNumberChannelGroupCombinations,
														&m_localCombinationsToList,
														&m_maxContiguousChannelsPerGroup);

}	// end "OnChannelComboSelendok"



void CMFeatureSelectionDialog::OnNumberChannelsComboSelendok (
				wxCommandEvent& 					event)

{
   HandleChannelCombinationsMenu ();

   SeparabilityDialogUpdateChannelCombinationItems (
   														this,
															m_separabilitySpecsPtr,
															FALSE,
															m_localActiveNumberFeatures,
															m_allChanCombinationsPtr,
															m_savedContiguousChannelsPerGroup,
															m_searchFlag,
															m_channelCombinationSelection,
															m_localChannelCombinationsPtr,
															&m_localNumberChannelGroupCombinations,
															&m_savedNumberChannelGroupCombinations,
															&m_localCombinationsToList,
															&m_maxContiguousChannelsPerGroup);

}	// end "OnNumberChannelsComboSelendok"



void CMFeatureSelectionDialog::OnNumberChannelsComboDropDown (
				wxCommandEvent& 					event)

{
	m_channelsPossibleCtrl->SetSelection (-1);
	
}	// end "OnSelendokNumberChannelsComboDropDown"



void CMFeatureSelectionDialog::OnChangeChannelsPerGroup (
				wxCommandEvent& 					event)

{
	SInt32 								newContiguousChannelsPerGroup;
   SInt16 								returncode = 0;
	
	
   returncode = CheckValues (IDC_ChannelsPerGroup,
   									1,
   									m_maxContiguousChannelsPerGroup,
   									&newContiguousChannelsPerGroup);

	if (returncode == 0)
   	{
      if (m_savedContiguousChannelsPerGroup != newContiguousChannelsPerGroup)
      	{
         m_savedContiguousChannelsPerGroup = newContiguousChannelsPerGroup;

         SeparabilityDialogUpdateChannelCombinationItems (
															this,
															m_separabilitySpecsPtr,
															TRUE,
															m_localActiveNumberFeatures,
															m_allChanCombinationsPtr,
															m_savedContiguousChannelsPerGroup,
															m_searchFlag,
															m_channelCombinationSelection,
															m_localChannelCombinationsPtr,
															&m_localNumberChannelGroupCombinations,
															&m_savedNumberChannelGroupCombinations,
															&m_localCombinationsToList,
															&m_maxContiguousChannelsPerGroup);

         SeparabilityDialogUpdateChannelFeatureGroupText (
														this,
														(SInt16) m_savedContiguousChannelsPerGroup,
														m_featureTransformationFlag);

      	}	// end "if (m_savedContiguousChannelsPerGroup != ..."
		
   	}	// end "if (returncode == 0)"

}	// end "OnChangeChannelsPerGroup"



SInt16 CMFeatureSelectionDialog::CheckValues (
				int 									controlID,
				SInt16 								minValue,
				SInt16 								maxValue,
				SInt32* 								newValue)

{
	SInt16 								errorFlag = 0;
	
	
   wxTextCtrl*	textControlPtr = wxDynamicCast (FindWindow (controlID), wxTextCtrl);
        
   if (textControlPtr != NULL)
   	{
      wxString value = textControlPtr->GetValue ();

      if (value.Length () == 0)
         errorFlag = 1;
		
		else	// value.Length () != 0
			{
         SInt32 outputValue;
         outputValue = wxAtoi (value);
         if (outputValue > maxValue || outputValue < minValue)
         	{
            errorFlag = 1;
            if (outputValue < minValue)
               outputValue = minValue;
				
				else
               outputValue = maxValue;
				
            textControlPtr->ChangeValue (wxString::Format(wxT("%i"), outputValue));
				
          	}	// end "valueDouble > maxValue || valueDouble < minValue"
			
         *newValue = outputValue;
			
      	}	// end "if (m_value.Length () != 0)"

   	}	// end "if (textControlPtr != NULL)"

   return errorFlag;
	
}	// end "CheckValues"



void CMFeatureSelectionDialog::OnStepProcedure (
				wxCommandEvent& 					event)

{
   wxCheckBox* search = (wxCheckBox*)FindWindow (IDC_StepProcedure);
   m_searchFlag = search->GetValue ();

   SeparabilityDialogUpdateChannelCombinationItems (
															this,
															m_separabilitySpecsPtr,
															TRUE,
															m_localActiveNumberFeatures,
															m_allChanCombinationsPtr,
															m_savedContiguousChannelsPerGroup,
															m_searchFlag,
															m_channelCombinationSelection,
															m_localChannelCombinationsPtr,
															&m_localNumberChannelGroupCombinations,
															&m_savedNumberChannelGroupCombinations,
															&m_localCombinationsToList,
															&m_maxContiguousChannelsPerGroup);

}	// end "OnStepProcedure"



void CMFeatureSelectionDialog::OnChangeNumberBestToList (
				wxCommandEvent& 					event)

{
	SInt32 								newCombinationsToList;


   wxTextCtrl* numberbest = (wxTextCtrl*)FindWindow (IDC_NumberBestToList);
   wxString numberbestString = numberbest->GetValue ();
   newCombinationsToList = wxAtoi (numberbestString);
   if (newCombinationsToList < 1 || newCombinationsToList > SInt16_MAX)
		numberbest->ChangeValue (wxString::Format(wxT("%i"), m_localCombinationsToList));

	else	// newCombinationsToList in proper range
      m_localCombinationsToList = newCombinationsToList;

}	// end "OnChangeNumberBestToList"



void CMFeatureSelectionDialog::OnClassComboSelendok (
				wxCommandEvent& 					event)

{
   HandleClassesMenu (&m_localNumberClasses,
								(SInt16*)m_classListPtr,
								2,
								(SInt16)gProjectInfoPtr->numberStatisticsClasses,
								IDC_ClassCombo,
								&m_classSelection);

   SeparabilityDialogUpdateNumberClassCombintations (this,
																		m_classSelection,
																		&m_localNumberClasses,
																		m_classListPtr);

}	// end "OnClassComboSelendok"



void CMFeatureSelectionDialog::OnListOptions (
				wxCommandEvent& 					event)

{
   SetDLogControlHilite (NULL, wxID_OK, 255);
   SeparabilityListDialog (&m_localCombinationsToList);
   SetDLogControlHilite (NULL, wxID_OK, 0);

   wxTextCtrl* numberbest = (wxTextCtrl*)FindWindow (IDC_NumberBestToList);
   numberbest->ChangeValue (wxString::Format (wxT("%i"), m_localCombinationsToList));

}	// end "OnListOptions"



void CMFeatureSelectionDialog::OnNumberChannelsComboCloseUp (
				wxCommandEvent& 					event)

{
	if (m_channelsPossibleCtrl->GetSelection () == -1)
		m_channelsPossibleCtrl->SetSelection (m_channelCombinationSelection);
	
	event.Skip ();
	
}	// end "OnNumberChannelsComboCloseUp"



void CMFeatureSelectionDialog::OnClassPairWeightsComboSelendok (
				wxCommandEvent& 					event)

{
   HandleClassPairWeightsMenu (&m_localClassPairWeightsListPtr,
											IDC_ClassPairWeightsCombo,
											&m_classPairWeightsSelection,
											&m_localDefaultClassPairWeight);

}	// end "OnClassPairWeightsComboSelendok"



void CMFeatureSelectionDialog::OnHelpButton (
				wxCommandEvent& 					event)

{
   DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_DialogMsg0, 0, NULL);
	
}	// end "OnHelpButton"



bool CMFeatureSelectionDialog::TransferDataFromWindow ()

{
	SInt16 								returnCode = 0;


   m_separabilityListSelection = m_distanceMeasureCtrl->GetSelection ();
	
   m_channelCombinationSelection = m_channelsPossibleCtrl->GetSelection ();

   m_classPairWeightsSelection = m_classPairWeightsCtrl->GetSelection ();
	
   m_channelSelection = m_channelsCtrl->GetSelection ();
	
   m_classSelection = m_classesCtrl->GetSelection ();
	
   wxComboBox* symbolcombo = (wxComboBox*)FindWindow (IDC_SymbolCombo);
   m_symbolSelection = symbolcombo->GetSelection ();

   wxCheckBox* textwindow = (wxCheckBox*)FindWindow (IDC_TextWindow);
   m_textWindowFlag = textwindow->GetValue ();
	
   wxCheckBox* disk = (wxCheckBox*)FindWindow (IDC_DiskFile);
   m_diskFileFlag = disk->GetValue ();
	
   wxCheckBox* step = (wxCheckBox*)FindWindow (IDC_StepProcedure);
   m_searchFlag = step->GetValue ();
	
   wxCheckBox* feature = (wxCheckBox*)FindWindow (IDC_FeatureTransformation);
   m_featureTransformationFlag = feature->GetValue ();

   wxTextCtrl* channelpergroup = (wxTextCtrl*)FindWindow (IDC_ChannelsPerGroup);
   wxString channelpergroupString = channelpergroup->GetValue ();
   m_savedContiguousChannelsPerGroup = wxAtoi(channelpergroupString);
	
   wxTextCtrl* numberbest = (wxTextCtrl*)FindWindow (IDC_NumberBestToList);
   wxString numberbestString = numberbest->GetValue ();
   m_localCombinationsToList = wxAtoi(numberbestString);

   returnCode = CheckDialogRealValue (this,
													IDC_ChannelsPerGroup,
													1,
													m_maxContiguousChannelsPerGroup,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
   if (returnCode == 0)
      returnCode = CheckDialogRealValue (this,
													IDC_NumberBestToList,
													1,
													SInt16_MAX,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);      
   return (returnCode == 0);
	
}	// end "TransferDataFromWindow"



bool CMFeatureSelectionDialog::TransferDataToWindow ()

{
   m_distanceMeasureCtrl->SetSelection (m_separabilityListSelection);
	
   m_channelsPossibleCtrl->SetSelection (m_channelCombinationSelection);
	
   m_classPairWeightsCtrl->SetSelection (m_classPairWeightsSelection);
	
   m_channelsCtrl->SetSelection (m_channelSelection);
	
   m_classesCtrl->SetSelection (m_classSelection);
	
   wxComboBox* symbolcombo = (wxComboBox*)FindWindow (IDC_SymbolCombo);
   symbolcombo->SetSelection (m_symbolSelection);

   wxCheckBox* textwindow = (wxCheckBox*)FindWindow (IDC_TextWindow);
   textwindow->SetValue (m_textWindowFlag);
	
   wxCheckBox* disk = (wxCheckBox*)FindWindow (IDC_DiskFile);
   disk->SetValue (m_diskFileFlag);
	
   wxCheckBox* step = (wxCheckBox*)FindWindow (IDC_StepProcedure);
   step->SetValue (m_searchFlag);
	
   wxCheckBox* feature = (wxCheckBox*)FindWindow (IDC_FeatureTransformation);
   feature->SetValue (m_featureTransformationFlag);

   wxTextCtrl* channelpergroup = (wxTextCtrl*)FindWindow (IDC_ChannelsPerGroup);
   channelpergroup->ChangeValue (
   					wxString::Format (wxT("%i"), m_savedContiguousChannelsPerGroup));
	
   wxTextCtrl* numberbest = (wxTextCtrl*)FindWindow (IDC_NumberBestToList);
   numberbest->ChangeValue (wxString::Format (wxT("%i"), m_localCombinationsToList));

   return true;
	
}	// end "TransferDataToWindow"



/////////////////////////////////////////////////////////////////////////////
// CMFeatureSelectionListDialog dialog

BEGIN_EVENT_TABLE (CMFeatureSelectionListDialog, CMDialog)
	EVT_BUTTON (IDS_HelpButton , CMFeatureSelectionListDialog::OnHelpButton)
	EVT_INIT_DIALOG (CMFeatureSelectionListDialog::OnInitDialog)
END_EVENT_TABLE ()



CMFeatureSelectionListDialog::CMFeatureSelectionListDialog (
				wxWindow* 							parent,
				wxWindowID 							id,
				const wxString& 					title)
		: CMDialog (CMFeatureSelectionListDialog::IDD, parent, title)
		
{
   m_sortChannelCombinations = -1;
	
   m_sortChannelCombinations = 0;
	
   m_thresholdedGroupTableFlag = FALSE;
   m_thresholdedClassPairTableFlag = FALSE;
   m_localDistancesLessThanToGroup = 0;
   m_localDistancesLessThanToList = 0;
   m_listClassPairDistancesFlag = FALSE;
   m_localMinDistanceRangeToList = 0;
   m_localMaxDistanceRangeToList = 0;
   m_localCombinationsToList = 1;
   m_separabilityTableFlag = FALSE;

   m_initializedFlag = CMDialog::m_initializedFlag;

   CreateControls ();
	
}	// end "CMFeatureSelectionListDialog"



Boolean CMFeatureSelectionListDialog::DoDialog (
				SeparabilitySpecsPtr 			separabilitySpecsPtr,
				SInt32* 								combinationsToListPtr)
   
{
	SInt16 								returnCode;

   Boolean 								continueFlag = FALSE;


   		// Make sure intialization has been completed.

	if (!m_initializedFlag)
      																				return (FALSE);

   m_separabilitySpecsPtr = separabilitySpecsPtr;
   m_combinationsToListPtr = combinationsToListPtr;

   returnCode = ShowModal ();

   if (returnCode == wxID_OK)
   	{
      continueFlag = TRUE;

      		// Adjust sort channel combinations variable to be 1 based.

      m_sortChannelCombinations++;

      SeparabilityListDialogOK (m_separabilitySpecsPtr,
											m_combinationsToListPtr,
											m_separabilityTableFlag,
											m_localCombinationsToList,
											(float) m_localMinDistanceRangeToList,
											(float) m_localMaxDistanceRangeToList,
											(m_sortChannelCombinations == 1),
											(m_sortChannelCombinations == 2),
											(m_sortChannelCombinations == 0),
											m_thresholdedClassPairTableFlag,
											m_localDistancesLessThanToList,
											m_thresholdedGroupTableFlag,
											m_localDistancesLessThanToGroup,
											m_listClassPairDistancesFlag);

   	}	// end "if (returnCode == IDOK)"

   return (continueFlag);

}	// end "DoDialog"



void CMFeatureSelectionListDialog::CreateControls ()

{
   SetSizeHints (wxDefaultSize, wxDefaultSize);

   bSizer226 = new wxBoxSizer (wxVERTICAL);

   wxBoxSizer* bSizer227;
   bSizer227 = new wxBoxSizer (wxVERTICAL);

   m_checkBox53 = new wxCheckBox (this,
												IDC_SeparabilityTable,
												wxT("Separability table with:"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	//SetUpToolTip (m_checkBox53, IDS_ToolTip161);
   bSizer227->Add (m_checkBox53, 0, wxALL, 5);

   wxBoxSizer* bSizer231;
   bSizer231 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText230 = new wxStaticText (this,
													wxID_ANY,
													wxT("Best"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText230->Wrap (-1);
   bSizer231->Add (m_staticText230, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl124 = new wxTextCtrl (this,
												IDC_NumberBestCombinations,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (50, -1),
												0);
   m_textCtrl124->SetValidator (
   								wxTextValidator (wxFILTER_DIGITS, &m_numberbestString));
   SetUpToolTip (m_textCtrl124, IDS_ToolTip161);
   bSizer231->Add (m_textCtrl124, 0, wxALL, 5);

   m_staticText231 = new wxStaticText (this,
   												IDC_PossibleCombinations,
   												wxT("out of 9999999999 possible combinations"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText231->Wrap (-1);
   bSizer231->Add (m_staticText231, 0, wxALIGN_CENTER | wxALL, 5);

   bSizer227->Add (bSizer231, 0, wxEXPAND | wxLEFT, 30);

   wxBoxSizer* bSizer232;
   bSizer232 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText232 = new wxStaticText (this,
   												wxID_ANY,
   												wxT("Distance range:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText232->Wrap (-1);
   bSizer232->Add (m_staticText232, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl125 = new wxTextCtrl (this,
   											IDC_MinDistanceRange,
   											wxEmptyString,
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   m_textCtrl125->SetValidator (
   								wxTextValidator (wxFILTER_NUMERIC, &m_mindistrangeString));
   SetUpToolTip (m_textCtrl125, IDS_ToolTip169);
   bSizer232->Add (m_textCtrl125, 0, wxALL, 5);

   m_staticText233 = new wxStaticText (this,
   												wxID_ANY,
   												wxT("to"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText233->Wrap (-1);
   bSizer232->Add (m_staticText233, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl126 = new wxTextCtrl (this,
   											IDC_MaxDistanceRange,
   											wxEmptyString,
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   m_textCtrl126->SetValidator (
   								wxTextValidator (wxFILTER_NUMERIC, &m_maxdistrangeString));
   SetUpToolTip (m_textCtrl126, IDS_ToolTip170);
   bSizer232->Add (m_textCtrl126, 0, wxALL, 5);

   bSizer227->Add (bSizer232, 0, wxEXPAND | wxLEFT, 30);

   wxBoxSizer* bSizer233;
   bSizer233 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText234 = new wxStaticText (this,
   												wxID_ANY,
   												wxT("Sort channel combinations by:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText234->Wrap (-1);
   bSizer233->Add (m_staticText234, 0, wxALL, 5);

   wxBoxSizer* bSizer239;
   bSizer239 = new wxBoxSizer (wxVERTICAL);

   m_radioBtn24 = new wxRadioButton (this,
   												IDC_AverageClassDistance,
   												wxT("Average class distance"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   SetUpToolTip (m_radioBtn24, IDS_ToolTip171);
   bSizer239->Add (m_radioBtn24, 0, wxLEFT | wxTOP | wxRIGHT, 5);

   m_radioBtn25 = new wxRadioButton (this,
   												IDC_MinimumClassDistance,
   												wxT("Minimum class distance"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   SetUpToolTip (m_radioBtn25, IDS_ToolTip172);
   bSizer239->Add (m_radioBtn25, 0, wxLEFT | wxTOP | wxRIGHT, 5);

   m_radioBtn26 = new wxRadioButton (this,
   												IDC_OrderComputed,
   												wxT("Order computed"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   SetUpToolTip (m_radioBtn26, IDS_ToolTip173);
   bSizer239->Add (m_radioBtn26, 0, wxLEFT | wxTOP | wxRIGHT, 5);

   bSizer233->Add (bSizer239, 0, wxEXPAND);

   bSizer227->Add (bSizer233, 0, wxEXPAND | wxLEFT, 30);

   wxBoxSizer* bSizer234;
   bSizer234 = new wxBoxSizer (wxHORIZONTAL);

   wxBoxSizer* bSizer235;
   bSizer235 = new wxBoxSizer (wxVERTICAL);

   m_checkBox54 = new wxCheckBox (this,
   											IDC_ListClassPairDistances,
   											wxT("List class pair distances"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_checkBox54, IDS_ToolTip174);
   bSizer235->Add (m_checkBox54, 0, wxALL, 5);

   bSizer234->Add (bSizer235, 1, wxEXPAND, 5);

   wxBoxSizer* bSizer236;
   bSizer236 = new wxBoxSizer (wxVERTICAL);

   wxBoxSizer* bSizer237;
   bSizer237 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText235 = new wxStaticText (this,
   												IDC_IncludeChannelComboPrompt,
   												wxT("Include channel combinations"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText235->Wrap (-1);
   bSizer237->Add (m_staticText235, 0, wxALIGN_CENTER | wxALL, 5);
   m_staticText237 = new wxStaticText (this,
   												IDC_IncludeChannelCombo,
   												wxEmptyString,
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText237->Wrap (-1);
   bSizer237->Add (m_staticText237, 0, wxALIGN_CENTER | wxALL, 5);

   bSizer236->Add (bSizer237, 1, wxEXPAND, 5);

   wxBoxSizer* bSizer238;
   bSizer238 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText236 = new wxStaticText (this,
   												IDC_ExcludeChannelComboPrompt,
   												wxT("Exclude channel combinations"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText236->Wrap (-1);
   bSizer238->Add (m_staticText236, 0, wxALIGN_CENTER | wxALL, 5);

   m_staticText238 = new wxStaticText (this,
   												IDC_ExcludeChannelCombo,
   												wxEmptyString,
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText238->Wrap (-1);
   bSizer238->Add (m_staticText238, 0, wxALIGN_CENTER | wxALL, 5);

   bSizer236->Add (bSizer238, 1, wxEXPAND, 5);
   bSizer234->Add (bSizer236, 1, wxEXPAND, 5);
   bSizer227->Add (bSizer234, 0, wxEXPAND | wxLEFT, 30);
   bSizer226->Add (bSizer227, 0, wxEXPAND | wxLEFT | wxTOP, 12);

   wxBoxSizer* bSizer228;
   bSizer228 = new wxBoxSizer (wxHORIZONTAL);

   m_checkBox51 = new wxCheckBox (this,
   											IDC_ClassPairThresholdTable,
   											wxT("Table of class pairs with distance < :"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   bSizer228->Add (m_checkBox51, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl122 = new wxTextCtrl (this,
   											IDC_ClassPairThreshold,
   											wxEmptyString,
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   bSizer228->Add (m_textCtrl122, 0, wxALL, 5);

   bSizer226->Add (bSizer228, 0,  wxLEFT | wxEXPAND, 12);
   wxBoxSizer* bSizer229;
   bSizer229 = new wxBoxSizer (wxHORIZONTAL);

   m_checkBox52 = new wxCheckBox (this,
   											IDC_ClassGroupThresholdTable,
   											wxT("Class grouping table with threshold of:"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   bSizer229->Add (m_checkBox52, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl123 = new wxTextCtrl (this,
   											IDC_ClassGroupThreshold,
   											wxEmptyString,
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   bSizer229->Add (m_textCtrl123, 0, wxALIGN_CENTER | wxALL, 5);

   bSizer226->Add (bSizer229, 0, wxEXPAND | wxLEFT | wxRIGHT, 12);
	
	/*
   wxBoxSizer* bSizer230;
   bSizer230 = new wxBoxSizer (wxHORIZONTAL);

	wxContextHelpButton* helpButton = new wxContextHelpButton (this,
																					IDS_HelpButton,
																					wxDefaultPosition,
																					wxDefaultSize,
																					0);

	bSizer230->Add (helpButton, 0, wxALIGN_CENTER|wxALL, 5);
	*/
	CreateStandardButtons (bSizer226);

   SetSizer (bSizer226);
   Layout ();
	Centre (wxBOTH);
	
}	// end "CMFeatureSelectionListDialog::CreateControls"



void CMFeatureSelectionListDialog::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	SInt16 								sortChannelCombinations;


   		// Initialize dialog variables.

   SeparabilityListDialogInitialize (this,
													m_separabilitySpecsPtr,
													*m_combinationsToListPtr,
													&m_separabilityTableFlag,
													&m_localCombinationsToList,
													&m_localMinDistanceRangeToList,
													&m_localMaxDistanceRangeToList,
													&sortChannelCombinations,
													&m_thresholdedClassPairTableFlag,
													&m_localDistancesLessThanToList,
													&m_thresholdedGroupTableFlag,
													&m_localDistancesLessThanToGroup,
													&m_listClassPairDistancesFlag);

   		// Adjust sort channel combinations variable to be 0 based.

   m_sortChannelCombinations = sortChannelCombinations - 1;

   if (TransferDataToWindow ())
      PositionDialogWindow ();

   		// Set default text selection to first edit text item

   SelectDialogItemText (this, IDC_NumberBestCombinations, 0, SInt16_MAX);
   Layout ();
   Fit ();

}	// end "OnInitDialog"



void CMFeatureSelectionListDialog::OnHelpButton (
				wxCommandEvent& 					event)
				
{
	//wxMessageBox (wxT("Information... "), wxT("Help"), wxOK| wxSTAY_ON_TOP, this);
	//DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_DialogMsg0, 0, NULL);
	
}	// end "OnHelpButton"



bool CMFeatureSelectionListDialog::TransferDataFromWindow ()

{
   SInt16 								returnCode = 0;
   
	
   wxCheckBox* thresholdgroup = (wxCheckBox*)FindWindow (IDC_ClassGroupThresholdTable);
   m_thresholdedGroupTableFlag = thresholdgroup->GetValue ();
	
   wxCheckBox* thresholdclass = (wxCheckBox*)FindWindow (IDC_ClassPairThresholdTable);
   m_thresholdedClassPairTableFlag = thresholdclass->GetValue ();
	
   wxCheckBox* seperatetabe = (wxCheckBox*)FindWindow (IDC_SeparabilityTable);
	m_separabilityTableFlag = seperatetabe->GetValue ();
	
   wxCheckBox* listclasspair = (wxCheckBox*)FindWindow (IDC_ListClassPairDistances);
	m_listClassPairDistancesFlag = listclasspair->GetValue ();

   wxTextCtrl* classgroupthres = (wxTextCtrl*)FindWindow (IDC_ClassGroupThreshold);
   wxString classgroupthresString = classgroupthres->GetValue ();
   classgroupthresString.ToDouble (&m_localDistancesLessThanToGroup);
	
   wxTextCtrl* classpairthres = (wxTextCtrl*)FindWindow (IDC_ClassPairThreshold);
   wxString classpairthresString = classpairthres->GetValue ();
   classpairthresString.ToDouble (&m_localDistancesLessThanToList);
	
   wxTextCtrl* mindistrange = (wxTextCtrl*)FindWindow (IDC_MinDistanceRange);
   m_mindistrangeString = mindistrange->GetValue ();
   m_mindistrangeString.ToDouble (&m_localMinDistanceRangeToList);
	
   wxTextCtrl* maxdistrange = (wxTextCtrl*)FindWindow (IDC_MaxDistanceRange);
   m_maxdistrangeString = maxdistrange->GetValue ();
   m_maxdistrangeString.ToDouble (&m_localMaxDistanceRangeToList);
	
   wxTextCtrl* numberbest = (wxTextCtrl*)FindWindow (IDC_NumberBestCombinations);
	m_numberbestString = numberbest->GetValue ();
   m_localCombinationsToList = wxAtoi (m_numberbestString);

   wxRadioButton* avgclassdist = (wxRadioButton*)FindWindow (IDC_AverageClassDistance);
   wxRadioButton* minclassdist = (wxRadioButton*)FindWindow (IDC_MinimumClassDistance);
	if (avgclassdist->GetValue () == true)
      m_sortChannelCombinations = 0;
	
   else if (minclassdist->GetValue () == true)
      m_sortChannelCombinations = 1;
	
   else
      m_sortChannelCombinations = 2;

   returnCode = CheckDialogRealValue (this,
													IDC_ClassGroupThreshold,
													0,       // minValue
													32767,   // maxValue
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
    
   if (returnCode == 0)	
      returnCode = CheckDialogRealValue (this,
														IDC_ClassPairThreshold,
														0,       // minValue
														32767,   // maxValue
														0,
														true,		// minValue is allowed.
														true,		// maxValue is allowed,
														kDisplayRangeAlert);
   if (returnCode == 0)	
      returnCode = CheckDialogRealValue (this,
														IDC_MinDistanceRange,
														0,       // minValue
														32767,   // maxValue
														0,
														true,		// minValue is allowed.
														true,		// maxValue is allowed,
														kDisplayRangeAlert);
   
   if (returnCode == 0)	
      returnCode = CheckDialogRealValue (this,
														IDC_MaxDistanceRange,
														0,       // minValue
														32767,   // maxValue
														0,
														true,		// minValue is allowed.
														true,		// maxValue is allowed,
														kDisplayRangeAlert);
   
   if (returnCode == 0)	
      returnCode = CheckDialogRealValue (this,
														IDC_NumberBestCombinations,
														1,       // minValue
														32767,   // maxValue
														0,
														true,		// minValue is allowed.
														true,		// maxValue is allowed,
														kDisplayRangeAlert);
      
   return (returnCode == 0);

}	// end "TransferDataFromWindow"



bool CMFeatureSelectionListDialog::TransferDataToWindow ()

{
   wxCheckBox* thresholdgroup = (wxCheckBox*)FindWindow (IDC_ClassGroupThresholdTable);
   thresholdgroup->SetValue (m_thresholdedGroupTableFlag);
	
   wxCheckBox* thresholdclass = (wxCheckBox*)FindWindow (IDC_ClassPairThresholdTable);
   thresholdclass->SetValue (m_thresholdedClassPairTableFlag);
	
   wxCheckBox* seperatetabe = (wxCheckBox*)FindWindow (IDC_SeparabilityTable);
   seperatetabe->SetValue (m_separabilityTableFlag);
	
   wxCheckBox* listclasspair = (wxCheckBox*)FindWindow (IDC_ListClassPairDistances);
	listclasspair->SetValue (m_listClassPairDistancesFlag);

   wxTextCtrl* classgroupthres = (wxTextCtrl*)FindWindow (IDC_ClassGroupThreshold);
   classgroupthres->ChangeValue (
   						wxString::Format (wxT("%.1f"), m_localDistancesLessThanToGroup));
	
   wxTextCtrl* classpairthres = (wxTextCtrl*)FindWindow (IDC_ClassPairThreshold);
   classpairthres->ChangeValue (
   						wxString::Format (wxT("%.1f"), m_localDistancesLessThanToList));
	
   wxTextCtrl* mindistrange = (wxTextCtrl*)FindWindow (IDC_MinDistanceRange);
   mindistrange->ChangeValue (
   						wxString::Format (wxT("%.1f"), m_localMinDistanceRangeToList));
	
   wxTextCtrl* maxdistrange = (wxTextCtrl*)FindWindow (IDC_MaxDistanceRange);
   maxdistrange->ChangeValue (
   						wxString::Format (wxT("%.1f"), m_localMaxDistanceRangeToList));
	
   wxTextCtrl* numberbest = (wxTextCtrl*)FindWindow (IDC_NumberBestCombinations);
	numberbest->ChangeValue (wxString::Format (wxT("%i"), m_localCombinationsToList));

   wxRadioButton* avgclassdist = (wxRadioButton*)FindWindow (IDC_AverageClassDistance);
   wxRadioButton* minclassdist = (wxRadioButton*)FindWindow (IDC_MinimumClassDistance);
   wxRadioButton* ordercompute = (wxRadioButton*)FindWindow (IDC_OrderComputed);
	if (m_sortChannelCombinations == 0)
		{
      avgclassdist->SetValue (true);
      minclassdist->SetValue (false);
      ordercompute->SetValue (false);
		
		}	// end "if (m_sortChannelCombinations == 0)"
	
	else if (m_sortChannelCombinations == 1)
		{
      avgclassdist->SetValue (false);
      minclassdist->SetValue (true);
      ordercompute->SetValue (false);
		
   	}	// end "else if (m_sortChannelCombinations == 1)"
	
	else	// m_sortChannelCombinations != 0 and != 1
		{
      avgclassdist->SetValue (false);
      minclassdist->SetValue (false);
      ordercompute->SetValue (true);
		
   	}	// end "else m_sortChannelCombinations != 0 and != 1"

   return true;
	
}	// end "TransferDataToWindow"
