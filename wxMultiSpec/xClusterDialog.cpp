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
//	File:						xClusterDialog.cpp : class implementation file
//	Class Definition:		xClusterDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			05/14/2020
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMClusterDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xClusterDialog.h"



IMPLEMENT_DYNAMIC_CLASS (CMClusterDialog, CMDialog)

BEGIN_EVENT_TABLE (CMClusterDialog, CMDialog)
	EVT_BUTTON (IDEntireImage, CMClusterDialog::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMClusterDialog::ToSelectedImage)

	EVT_CHECKBOX (IDC_CreateClusterMaskCheckBox, CMClusterDialog::OnCreateMaskFile)
	EVT_CHECKBOX (IDC_ImageOverlay, CMClusterDialog::OnImageOverlay)

	EVT_CHOICE (IDC_ImageOverlayCombo, CMClusterDialog::OnImageOverlayComboSelendok)
	EVT_CHOICE (IDC_DiskCombo, CMClusterDialog::OnSelendokMaskFileDiskCombo)

	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_ChannelCombo, CMClusterDialog::OnChannelComboSelendok)
	#endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_ChannelCombo, CMClusterDialog::OnChannelComboSelendok)
	#endif

	EVT_COMBOBOX_CLOSEUP (IDC_ChannelCombo, CMClusterDialog::OnChannelComboCloseUp)
	EVT_COMBOBOX_DROPDOWN (IDC_ChannelCombo, CMClusterDialog::OnChannelComboDropDown)

	EVT_INIT_DIALOG (CMClusterDialog::OnInitDialog)

	EVT_RADIOBOX (IDC_Algorithm, CMClusterDialog::OnAlgorithmSelection)
	EVT_RADIOBOX (IDC_ClassificationMap, CMClusterDialog::OnClassifySelectedArea)

	EVT_TEXT (IDC_ColumnEnd, CMClusterDialog::CheckColumnEnd)
	EVT_TEXT (IDC_ColumnStart, CMClusterDialog::CheckColumnStart)
	EVT_TEXT (IDC_LineEnd, CMClusterDialog::CheckLineEnd)
	EVT_TEXT (IDC_LineStart, CMClusterDialog::CheckLineStart)
	EVT_TEXT (IDC_LineInterval, CMClusterDialog::CheckLineInterval)
	EVT_TEXT (IDC_ColumnInterval, CMClusterDialog::CheckColumnInterval)
END_EVENT_TABLE ()


CMClusterDialog::CMClusterDialog ()

{

}	// end "CMClusterDialog"



CMClusterDialog::CMClusterDialog (
				wxWindow*							parent,
				wxWindowID							id,
				const wxString&					title)
		: CMDialog (CMClusterDialog::IDD, parent, title)

{
   m_thresholdFlag = FALSE;
   m_textWindowFlag = FALSE;
   m_symbolSelection = -1;
   m_clusterProcedure = -1;
   m_probabilityThreshold = 0;
   m_classifyThreshold = 1;
   m_diskFileFlag = FALSE;
   m_classificationArea = -1;
   m_saveStatisticsSelection = -1;
   m_createClusterMaskFlag = FALSE;
   m_createImageOverlayFlag = FALSE;
   m_selectImageOverlaySelection = 1;
	m_localFeaturesPtr = NULL;
   m_maskFileFormatCode = 0;
   
   m_maxClassifyThreshold = gImageFileInfoPtr->numberBins - 1;

   m_clusterProcedureSetFlag = FALSE;

   m_transparencyValue = 0;

   m_initializedFlag = CMDialog::m_initializedFlag;

   if (m_initializedFlag)
		{
      // Get pointer to memory for temporary storage of channel list. 

      m_localFeaturesPtr = (UInt16*)MNewPointer (
              (UInt32)gImageWindowInfoPtr->totalNumberChannels * sizeof (UInt16));

      m_initializedFlag = (m_localFeaturesPtr != NULL);

		}	// end "if (m_initializedFlag)"
	
   CreateControls ();

}	// end "CMClusterDialog"



CMClusterDialog::~CMClusterDialog (void)

{
   m_localFeaturesPtr = CheckAndDisposePtr (m_localFeaturesPtr);

}	// end "~CMClusterDialog"



void CMClusterDialog::ClassifyAreaSetting (
				bool									fromRadioGroupFlag,
				int									nIDC,
				int&									value)

{
   wxRadioBox* clareasel = (wxRadioBox*)FindWindow (IDC_ClassificationMap);

   if (fromRadioGroupFlag)
		{
				// Reading from radio group. Get index ignoring the disabled items.

      value = -1;

      value = clareasel->GetSelection ();
		
		}	// end "if (fromRadioGroupFlag)"

   else // !fromRadioGroupFlag
		{
      int localValue = value;

      clareasel->SetSelection (localValue);

		}	// end "else !fromRadioGroupFlag"

}	// end "ClassifyAreaSetting"    



void CMClusterDialog::CreateControls ()

{
   SetSizeHints (wxDefaultSize, wxDefaultSize);

   wxBoxSizer* bVSizer1 = new wxBoxSizer (wxVERTICAL);

   bSizer137 = new wxBoxSizer (wxHORIZONTAL);

   wxBoxSizer* bSizer139;
   bSizer139 = new wxBoxSizer (wxVERTICAL);
	
   wxString radioBox3Choices[] = {wxT("Single Pass...    "),
												wxT("ISODATA...      "),
												wxT("None...      ")};
	
   int radioBox3NChoices = sizeof (radioBox3Choices) / sizeof (wxString);
   m_algorithmSelectionRadioBox = new wxRadioBox (this,
																	IDC_Algorithm,
																	wxT("Algorithm"),
																	wxDefaultPosition,
																	wxDefaultSize,
																	radioBox3NChoices,
																	radioBox3Choices,
																	3,
																	wxRA_SPECIFY_ROWS);
   m_algorithmSelectionRadioBox->SetSelection (2);
   m_algorithmSelectionRadioBox->Show (2, false);
   SetUpToolTip (m_algorithmSelectionRadioBox, IDS_ToolTip80);
   bSizer139->Add (m_algorithmSelectionRadioBox, wxSizerFlags(0).Border(wxBOTTOM, 5));

	wxStaticBoxSizer* sbSizer11;
   sbSizer11 = new wxStaticBoxSizer (new wxStaticBox (
   													this,
														wxID_ANY,
														wxT("Cluster Classification Map Area(s)"),
														wxDefaultPosition,
														wxDefaultSize,
														wxTAB_TRAVERSAL),
													wxVERTICAL);

   wxString m_radioBox4Choices[] = {wxT("No classification map"),
												wxT("Training Area(s)"),
												wxT("Image Area")};
   int m_radioBox4NChoices = sizeof (m_radioBox4Choices) / sizeof (wxString);

	wxStaticBox* sbSizerStaticBox11 = sbSizer11->GetStaticBox();
   m_radioBox4 = new wxRadioBox (sbSizerStaticBox11,
											IDC_ClassificationMap,
											wxT(""),
											wxDefaultPosition,
											wxDefaultSize,
											//wxSize (-1, 99),
											m_radioBox4NChoices,
											m_radioBox4Choices,
											3,
											wxRA_SPECIFY_ROWS);
   m_radioBox4->SetSelection (2);
   SetUpToolTip (m_radioBox4, IDS_ToolTip81);
   sbSizer11->Add (m_radioBox4, wxSizerFlags(0).Border(wxRIGHT, 5));

   wxStaticBoxSizer* sbSizer8;
   wxStaticBox* areaStaticBox = new wxStaticBox (sbSizerStaticBox11,
																	IDC_LineColFrame,
																	wxT("Area to Cluster"),
																	wxDefaultPosition,
																	wxDefaultSize,
																	wxTAB_TRAVERSAL);
   sbSizer8 = new wxStaticBoxSizer (areaStaticBox, wxHORIZONTAL);

	CreateLineColumnControls (sbSizer8);
	
   sbSizer11->Add (sbSizer8, wxSizerFlags(1).Expand().Border(wxALL, 3));

   wxBoxSizer* bSizer142 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText174 = new wxStaticText (sbSizerStaticBox11,
													IDC_ClassifyThresholdPrompt,
													wxT("Classification threshold:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText174->Wrap (-1);
   SetUpToolTip (m_staticText174, IDS_ToolTip82);
   bSizer142->Add (m_staticText174,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   m_textCtrl84 = new wxTextCtrl (sbSizerStaticBox11,
												IDC_ClassifyThreshold,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   m_textCtrl84->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
   															&m_classifyThresholdString));
   SetUpToolTip (m_textCtrl84, IDS_ToolTip82);
   bSizer142->Add (m_textCtrl84, wxSizerFlags(0).Border(wxALL, 5));

   sbSizer11->Add (bSizer142, wxSizerFlags(0).Expand ());

   bSizer139->Add (sbSizer11, wxSizerFlags(1).Expand().Border(wxTOP|wxRIGHT, 5));
	
	bSizer137->Add (bSizer139,
							wxSizerFlags(1).Expand().Border(wxLEFT|wxTOP|wxBOTTOM, 12));

   wxBoxSizer* bSizer143 = new wxBoxSizer (wxVERTICAL);

   wxBoxSizer* bSizer138 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText160 = new wxStaticText (this,
													IDC_ChannelPrompt,
													wxT("Channel:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText160->Wrap (-1);
   SetUpToolTip (m_staticText160, IDS_ToolTip52);
   bSizer138->Add (
			m_staticText160,
			wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxBOTTOM|wxRIGHT, 5));

	GetAllSubsetMenuControl (kChannelsMenu,
										this,
										IDC_ChannelCombo,
										120,
										IDS_ToolTip52);
   bSizer138->Add (m_channelsCtrl, wxSizerFlags(0).Border(wxLEFT|wxBOTTOM, 5));
   
   bSizer143->Add (bSizer138, wxSizerFlags(0).Expand().Border(wxLEFT, 5));

   wxBoxSizer* bSizer144;
   bSizer144 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText175 = new wxStaticText (this,
													IDC_SymbolPrompt,
													wxT("Symbols:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText175->Wrap (-1);
   SetUpToolTip (m_staticText175, IDS_ToolTip83);
   bSizer144->Add (m_staticText175,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT, 5));

   wxString m_choice18Choices[] = {wxT("Default set"), wxT("User defined...")};
   int m_choice18NChoices = sizeof (m_choice18Choices) / sizeof (wxString);
   m_choice18 = new wxChoice (this,
										IDC_SymbolCombo,
										wxDefaultPosition,
										wxDefaultSize,
										m_choice18NChoices,
										m_choice18Choices,
										0);
   m_choice18->SetSelection (0);
   SetUpToolTip (m_choice18, IDS_ToolTip83);
   bSizer144->Add (m_choice18, wxSizerFlags(0).Border(wxALL, 5));

   bSizer143->Add (bSizer144, wxSizerFlags(0).Expand());

   wxBoxSizer* bSizer145;
   bSizer145 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText176 = new wxStaticText (this,
													IDC_ClusterStatsPrompt,
													wxT("Cluster Stats:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText176->Wrap (-1);
   SetUpToolTip (m_staticText176, IDS_ToolTip84);
   bSizer145->Add (
		m_staticText176,
		wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxTOP|wxBOTTOM|wxRIGHT, 5));

   wxString m_choice19Choices[] = {wxT("Do Not Save"), wxT("To New Project")};
   int m_choice19NChoices = sizeof (m_choice19Choices) / sizeof (wxString);
   m_choice19 = new wxChoice (this,
										IDC_ClusterStatsCombo,
										wxDefaultPosition,
										wxDefaultSize,
										m_choice19NChoices,
										m_choice19Choices,
										0);
   m_choice19->SetSelection (0);
   SetUpToolTip (m_choice19, IDS_ToolTip84);
   bSizer145->Add (m_choice19, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxBOTTOM, 5));

   bSizer143->Add (bSizer145, wxSizerFlags(0).Expand().Border(wxLEFT, 5));

   wxStaticBoxSizer* sbSizer25;
   sbSizer25 = new wxStaticBoxSizer (
										new wxStaticBox (this,
																wxID_ANY,
																wxT("Write Cluster Report/ Map To:")),
										wxVERTICAL);

   m_checkBox46 = new wxCheckBox (sbSizer25->GetStaticBox (),
												IDC_TextWindow,
												wxT("Text Window"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox46, IDS_ToolTip85);
   sbSizer25->Add (m_checkBox46, wxSizerFlags(0).Border(wxALL, 5));

   m_checkBox47 = new wxCheckBox (sbSizer25->GetStaticBox (),
												IDC_DiskFile,
												wxT("Text Disk File"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox47, IDS_ToolTip86);
   sbSizer25->Add (m_checkBox47, wxSizerFlags(0).Border(wxALL, 5));

   wxBoxSizer* bSizer146;
   bSizer146 = new wxBoxSizer (wxHORIZONTAL);

   m_checkBox48 = new wxCheckBox (sbSizer25->GetStaticBox (),
												IDC_CreateClusterMaskCheckBox,
												wxT("Cluster mask file:"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox48, IDS_ToolTip87);
   bSizer146->Add (m_checkBox48,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   wxString m_choice20Choices[] = {wxT("ERDAS .GIS"), wxT("GeoTIFF")};
   int m_choice20NChoices = sizeof (m_choice20Choices) / sizeof (wxString);
   m_choice20 = new wxChoice (sbSizer25->GetStaticBox (),
										IDC_DiskCombo,
										wxDefaultPosition,
										wxDefaultSize,
										m_choice20NChoices,
										m_choice20Choices,
										0);
   m_choice20->SetSelection (0);
   m_choice20->Show (false);
   bSizer146->Add (m_choice20,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

   sbSizer25->Add (bSizer146, wxSizerFlags(1).Expand());

   m_checkBox49 = new wxCheckBox (sbSizer25->GetStaticBox (),
												IDC_ImageOverlay,
												wxT("Image window overlay"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox49, IDS_ToolTip88);
   sbSizer25->Add (m_checkBox49, wxSizerFlags(0).Border(wxALL, 5));

   wxString m_choice21Choices[] = {wxT("Add new overlay")};
   int m_choice21NChoices = sizeof (m_choice21Choices) / sizeof (wxString);
   m_choice21 = new wxChoice (sbSizer25->GetStaticBox (),
										IDC_ImageOverlayCombo,
										wxDefaultPosition,
										wxDefaultSize,
										m_choice21NChoices,
										m_choice21Choices,
										0);
   SetUpToolTip (m_choice21, IDS_ToolTip89);
   m_choice21->SetSelection (0);
   sbSizer25->Add (m_choice21,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().
											Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL, 5));

   bSizer143->Add (sbSizer25,
							wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxBOTTOM, 5));

   bSizer137->Add (bSizer143,
							wxSizerFlags(0).Expand().Border(wxTOP|wxRIGHT|wxBOTTOM, 12));
	
	bVSizer1->Add (bSizer137, wxSizerFlags(0));

	CreateStandardButtons (bVSizer1);

   SetSizerAndFit (bVSizer1);
	
}	// end "CreateControls"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the cluster
//							specification dialog box to the user and copy the
//							revised back to the display specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			ClusterDialog in SCluster.cpp
//
//	Coded By:			Abdur Rachman Maud	Date: 08/02/2009
//	Revised By:			Larry L. Biehl			Date: 02/12/2018

SInt16 CMClusterDialog::DoDialog (
				Boolean								newProjectFlag)

{
	Boolean								continueFlag = FALSE;

   SInt16								returnCode;


			// Make sure intialization has been completed.

   if (!m_initializedFlag)
																							return (FALSE);

   m_newProjectFlag = newProjectFlag;

   returnCode = ShowModal ();

   if (returnCode == wxID_OK)
		{
      continueFlag = TRUE;

      		// Cluster classification area

      m_dialogSelectArea.lineStart = m_LineStart;
      m_dialogSelectArea.lineEnd = m_LineEnd;
      m_dialogSelectArea.lineInterval = m_LineInterval;

      m_dialogSelectArea.columnStart = m_ColumnStart;
      m_dialogSelectArea.columnEnd = m_ColumnEnd;
      m_dialogSelectArea.columnInterval = m_ColumnInterval;

      ClusterDialogOK (this,
							  (SInt16)(m_clusterProcedure + 1),
							  (Boolean)(m_classificationArea == 1),
							  (Boolean)(m_classificationArea == 2),
							  &m_dialogSelectArea,
							  (double)m_classifyThreshold,
							  (SInt16)m_channelSelection,
							  (SInt16)m_localActiveNumberFeatures,
							  (SInt16*)m_localActiveFeaturesPtr,
							  (SInt16)m_symbolSelection + 1,
							  (unsigned char*)m_localSymbolsPtr,
							  (SInt16)m_saveStatisticsSelection + 1,
							  (Boolean)m_textWindowFlag,
							  (Boolean)m_diskFileFlag,
							  (Boolean)m_createClusterMaskFlag,
							  (SInt16)m_maskFileFormatCode + 1,
							  (Boolean)m_createImageOverlayFlag,
							  (SInt16)m_selectImageOverlaySelection + 1,
							  (Boolean)m_thresholdFlag,
							  (double)m_probabilityThreshold,
							  (SInt32)m_transparencyValue);

   	}	// end "if (returnCode == IDOK)"

			// Set the variable containing the pointer to the overlay combo box to NULL.

   gPopUpImageOverlayMenu = NULL;

   return (continueFlag);

}	// end "DoDialog"  



void CMClusterDialog::OnAlgorithm (
				Boolean								returnFlag)

{
   if (returnFlag)
		{
      if (!m_clusterProcedureSetFlag)
         FindWindow (wxID_OK)->Enable (true);

      m_clusterProcedureSetFlag = TRUE;

      m_clusterProcedure = m_algorithmSelectionRadioBox->GetSelection ();

		}	// end "if (returnFlag)"

   else	// !returnFlag
		{
      if (!m_clusterProcedureSetFlag)
         m_clusterProcedure = 2;

      m_algorithmSelectionRadioBox->SetSelection (m_clusterProcedure);

		}	// end "else !returnFlag"

}	// end "OnAlgorithm"



void CMClusterDialog::OnAlgorithmSelection (
				wxCommandEvent&					event)

{
   int algorithm = event.GetInt ();
   if (algorithm == 0)
      OnSinglePassAlgorithm ();
	
   else if (algorithm == 1)
      OnISODATAAlgorithm ();
		
}	// end "OnAlgorithmSelection"



void CMClusterDialog::OnClassifySelectedArea (
				wxCommandEvent&					event)

{
   ClassifyAreaSetting (true,
								IDC_NoClassificationMap,
								m_classificationArea);

   UpdateClassificationItems (m_classificationArea);

}	// end "OnClassifySelectedArea"



void CMClusterDialog::OnCreateMaskFile (
				wxCommandEvent&					event)

{
	m_createClusterMaskFlag =
				((wxCheckBox*)FindWindow (IDC_CreateClusterMaskCheckBox))->GetValue ();

	if (!m_createClusterMaskFlag)
		HideDialogItem (this, IDC_DiskCombo);

	else // m_createClusterMaskFlag
		ShowDialogItem (this, IDC_DiskCombo);

	Fit ();

}	// end "OnCreateMaskFile"



void CMClusterDialog::OnImageOverlay (
				wxCommandEvent&					event)

{
   m_createImageOverlayFlag =
   								((wxCheckBox*)FindWindow (IDC_ImageOverlay))->GetValue ();

   if (m_createImageOverlayFlag)
      ShowDialogItem (this, IDC_ImageOverlayCombo);
	
   else // m_createImageOverlayFlag
      HideDialogItem (this, IDC_ImageOverlayCombo);
   
   Fit ();

}	// end "OnImageOverlay"



void CMClusterDialog::OnInitDialog (
				wxInitDialogEvent&				event)
				
{
	SInt16								channelSelection,
											classificationArea,
											clusterProcedure,
											maskFileFormatCode,
											saveStatisticsSelection,
											selectImageOverlaySelection,
											symbolSelection;

   Boolean								modeSetFlag;


	ClusterDialogInitialize (this,
									  m_localFeaturesPtr,
									  m_localSymbolsPtr,
									  m_newProjectFlag,
									  &clusterProcedure,
									  &modeSetFlag,
									  &classificationArea,
									  &m_dialogSelectArea,
									  &m_classifyThreshold,
									  &channelSelection,
									  &m_localActiveNumberFeatures,
									  &symbolSelection,
									  &saveStatisticsSelection,
									  &m_textWindowFlag,
									  &m_diskFileFlag,
									  &m_createClusterMaskFlag,
									  &maskFileFormatCode,
									  &m_createImageOverlayFlag,
									  &selectImageOverlaySelection,
									  &m_thresholdFlag,
									  &m_probabilityThreshold,
									  (SInt32*)&m_transparencyValue);
	
			// Set feature/transform feature parameters

   InitializeDialogFeatureParameters (kNoTransformation,
												  m_localActiveNumberFeatures,
												  gImageFileInfoPtr->numberChannels,
												  0,
												  m_localFeaturesPtr,
												  NULL,
												  &m_localNumberFeatures,
												  NULL,
												  &m_localActiveFeaturesPtr);

   m_clusterProcedure = clusterProcedure - 1;
   m_classificationArea = classificationArea;
   m_channelSelection = channelSelection;
   m_symbolSelection = symbolSelection - 1;
	m_saveStatisticsSelection = saveStatisticsSelection - 1;
   m_selectImageOverlaySelection = selectImageOverlaySelection - 1;
   m_channelListType = 1;

			//	Cluster Procedure
			// Make the "Single Pass" the default cluster algorithm.
			// Single Pass Cluster.

   m_clusterProcedureSetFlag = TRUE;
   if (m_clusterProcedure == -1)
		{
      FindWindow (wxID_OK)->Enable (FALSE);
      m_clusterProcedureSetFlag = FALSE;

		}	// end "if (m_clusterProcedure == -1)"
	
			// Cluster Mask File output to disk file.
			//		Adjust for 0 base index.
	
	m_maskFileFormatCode = abs (maskFileFormatCode);	
	m_maskFileFormatCode -= 1;
	
   m_LineStart = m_dialogSelectArea.lineStart;
   m_LineEnd = m_dialogSelectArea.lineEnd;
   m_LineInterval = m_dialogSelectArea.lineInterval;
   m_ColumnStart = m_dialogSelectArea.columnStart;
   m_ColumnEnd = m_dialogSelectArea.columnEnd;
   m_ColumnInterval = m_dialogSelectArea.columnInterval;

   UpdateClassificationItems (m_classificationArea);
 
			// Symbols to use.
			// User defined symbos are not available yet.

   ((wxChoice*)FindWindow (IDC_SymbolCombo))->Delete (1);

	//m_saveStatisticsSelection = 0;
	
   if (TransferDataToWindow ())
      PositionDialogWindow ();

			// Set default text selection to first edit text item

   SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);

}	// end "OnInitDialog"
 


void CMClusterDialog::OnISODATAAlgorithm ()
				
{
   Boolean returnFlag = ISODATAClusterDialog (gImageFileInfoPtr, this);

   OnAlgorithm (returnFlag);

}	// end "OnISODATAAlgorithm"   



void CMClusterDialog::OnOK (
				wxCommandEvent&					event)
				
{
	Boolean okFlag = TRUE;


   if (TransferDataFromWindow ())
		{
				// Check if the save statistics option will require the current project
				// to be saved.  Set item hit to 0 if the user cancels from the save.

      if (m_saveStatisticsSelection == 1) // To new project
			{
         if (gProjectInfoPtr->changedFlag)
            if (SaveProjectFile (2) != 0)
               okFlag = FALSE;

			}	// end "if (m_saveStatisticsSelection == 1)"

      else if (m_saveStatisticsSelection == 2) // Add to project
			{
         if (gImageFileInfoPtr->numberChannels != gClusterSpecsPtr->numberChannels ||
                 gProjectInfoPtr->numberStatisticsChannels !=
															(SInt16)gClusterSpecsPtr->numberChannels)
				{
            SInt16 itemHit;

            itemHit = DisplayAlert (kOKCancelAlertID,
												  2, kAlertStrID,
												  IDS_Alert11,
												  0,
												  NULL);
            if (itemHit != 1)
               okFlag = FALSE;

				}	// end "if (fileInfoPtr->numberChannels != ..."

			}	// end "else if (m_saveStatisticsSelection == 2)"

      if (okFlag)
         EndModal (wxID_OK);

		}	// end "if (UpdateData (TRUE))" 

}	// end "OnOK"  



void CMClusterDialog::OnChannelComboSelendok (
				wxCommandEvent&					event)
				
{
   HandleChannelsMenu (IDC_ChannelCombo,
								kNoTransformation,
								(SInt16)gImageWindowInfoPtr->totalNumberChannels,
								2,
								m_clusterProcedureSetFlag);

}	// end "OnChannelComboSelendok"



void CMClusterDialog::OnImageOverlayComboSelendok (
				wxCommandEvent&					event)
				
{
	m_selectImageOverlaySelection =
							((wxChoice*)FindWindow (IDC_ImageOverlayCombo))->GetSelection ();

}	// end "OnImageOverlayComboSelendok"



void CMClusterDialog::OnSelendokMaskFileDiskCombo (
				wxCommandEvent&					event)
				
{
   m_maskFileFormatCode = ((wxChoice*)FindWindow (IDC_DiskCombo))->GetSelection ();
   m_maskFileFormatCode++;

}	// end "OnSelendokMaskFileDiskCombo"



void CMClusterDialog::OnSinglePassAlgorithm ()
				
{
   Boolean returnFlag = OnePassClusterDialog (gImageFileInfoPtr, this);

   OnAlgorithm (returnFlag);

}	// end "OnSinglePassAlgorithm"  



bool CMClusterDialog::TransferDataFromWindow ()
 
{
	SInt16								returnCode = 0;
	
	
	TransferLinesColumnsFromWindow ();

   wxTextCtrl* clsfythresh = (wxTextCtrl*)FindWindow (IDC_ClassifyThreshold);
   wxString clthresh = clsfythresh->GetValue ();
   clthresh.ToDouble (&m_classifyThreshold);

   wxCheckBox* textwin = (wxCheckBox*)FindWindow (IDC_TextWindow);
   m_textWindowFlag = textwin->GetValue ();
	
   wxCheckBox* diskf = (wxCheckBox*)FindWindow (IDC_DiskFile);
   m_diskFileFlag = diskf->GetValue ();
	
   wxCheckBox* crtcluster = (wxCheckBox*)FindWindow (IDC_CreateClusterMaskCheckBox);
   m_createClusterMaskFlag = crtcluster->GetValue ();
	
   wxCheckBox* imgoverlay = (wxCheckBox*)FindWindow (IDC_ImageOverlay);
   m_createImageOverlayFlag = imgoverlay->GetValue ();

   wxChoice* symbolc = (wxChoice*)FindWindow (IDC_SymbolCombo);
   m_symbolSelection = symbolc->GetSelection ();
	
   wxChoice* clusterstats = (wxChoice*)FindWindow (IDC_ClusterStatsCombo);
   m_saveStatisticsSelection = clusterstats->GetSelection ();
	
   m_channelSelection = m_channelsCtrl->GetSelection ();
	
   wxChoice* imgoverlayc = (wxChoice*)FindWindow (IDC_ImageOverlayCombo);
   m_selectImageOverlaySelection = imgoverlayc->GetSelection ();
	
   wxChoice* diskc = (wxChoice*)FindWindow (IDC_DiskCombo);
   m_maskFileFormatCode = diskc->GetSelection ();

   ClassifyAreaSetting (true, IDC_NoClassificationMap, m_classificationArea);
	
   if (m_classificationArea != 0)
		{
		returnCode = CheckDialogRealValue (this,
														IDC_ClassifyThreshold,
														0,
														gImageFileInfoPtr->maxUsableDataValue,
														0,
														false,	// minValue not allowed.
														true,		// maxValue is allowed,
														kDisplayRangeAlert);
		
		if (returnCode == 0)
			returnCode = VerifyLineColumnValues (IDC_LineStart,
															  IDC_ColumnStart,
															  true);
								  
		}	// end "if (m_classificationArea == 2)"

   return (returnCode == 0);

}	// end "TransferDataFromWindow"



bool CMClusterDialog::TransferDataToWindow ()

{
	TransferLinesColumnsToWindow ();

   wxTextCtrl* clsfythresh = (wxTextCtrl*)FindWindow (IDC_ClassifyThreshold);
   clsfythresh->ChangeValue (wxString::Format (wxT("%.1f"), m_classifyThreshold));

   wxCheckBox* textwin = (wxCheckBox*)FindWindow (IDC_TextWindow);
   textwin->SetValue (m_textWindowFlag);
	
   wxCheckBox* diskf = (wxCheckBox*)FindWindow (IDC_DiskFile);
   diskf->SetValue (m_diskFileFlag);
	
   wxCheckBox* crtcluster = (wxCheckBox*)FindWindow (IDC_CreateClusterMaskCheckBox);
   crtcluster->SetValue (m_createClusterMaskFlag);
	
   wxCheckBox* imgoverlay = (wxCheckBox*)FindWindow (IDC_ImageOverlay);
   imgoverlay->SetValue (m_createImageOverlayFlag);

   wxChoice* symbolc = (wxChoice*)FindWindow (IDC_SymbolCombo);
   symbolc->SetSelection (m_symbolSelection);
	
   wxChoice* clusterstats = (wxChoice*)FindWindow (IDC_ClusterStatsCombo);
   clusterstats->SetSelection (m_saveStatisticsSelection);
	
   m_channelsCtrl->SetSelection (m_channelSelection);
	
   wxChoice* imgoverlayc = (wxChoice*)FindWindow (IDC_ImageOverlayCombo);
   imgoverlayc->SetSelection (m_selectImageOverlaySelection);
	
   wxChoice* diskc = (wxChoice*)FindWindow (IDC_DiskCombo);
   diskc->SetSelection (m_maskFileFormatCode);
   
   if (m_clusterProcedureSetFlag)
		{
      m_algorithmSelectionRadioBox->SetSelection (m_clusterProcedure);
		
		}	// end "if (m_clusterProcedureSetFlag)"

   wxCommandEvent event;
   OnCreateMaskFile (event);
   ClassifyAreaSetting (false, IDC_NoClassificationMap, m_classificationArea);
	
   if (m_classificationArea == 2)
		SetEntireImageButtons (NULL,
										m_LineStart,
										m_LineEnd,
										m_ColumnStart,
										m_ColumnEnd);

   return true;
	
}	// end "TransferDataToWindow"



void CMClusterDialog::UpdateClassificationItems (
				UInt16								classificationArea)
				
{
   
   if (classificationArea == 0)
		{
      HideShowAreaItems (kHide);
            
      MHideDialogItem (this, IDC_ClassifyThresholdPrompt);
      MHideDialogItem (this, IDC_ClassifyThreshold);

		}	// end "if (classificationArea == 0)"

   else if (classificationArea == 1)
		{
      HideShowAreaItems (kHide);
      MShowDialogItem (this, IDC_IntervalPrompt);
      MShowDialogItem (this, IDC_LinePrompt);
      MShowDialogItem (this, IDC_ColumnPrompt);
      MShowDialogItem (this, IDC_LineInterval);
      MShowDialogItem (this, IDC_ColumnInterval);

      MShowDialogItem (this, IDC_ClassifyThresholdPrompt);
      MShowDialogItem (this, IDC_ClassifyThreshold);
		
		SelectDialogItemText (this, IDC_ClassifyThreshold, 0, SInt16_MAX);

		}	// end "else if (classificationArea == 1)"

   else // classificationArea == 2
		{
      HideShowAreaItems (kShow);
      
      SetEntireImageButtons (NULL,
									  m_LineStart,
									  m_LineEnd,
									  m_ColumnStart,
									  m_ColumnEnd);

      MShowDialogItem (this, IDC_ClassifyThresholdPrompt);
      MShowDialogItem (this, IDC_ClassifyThreshold);
		
		SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);

		}	// end "else if (classificationArea == 2)"

}	// end "UpdateClassificationItems"  
