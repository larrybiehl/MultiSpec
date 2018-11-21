//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClusterDialog.cpp : class implementation file
//	Class Definition:		LClusterDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			11/13/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMClusterDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "LClusterDialog.h"
#include "LImage_dialog.cpp"



IMPLEMENT_DYNAMIC_CLASS (CMClusterDialog, CMDialog)


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
   
   mMaxClassifyThreshold = gImageFileInfoPtr->numberBins - 1;

   m_clusterProcedureSetFlag = FALSE;

   m_transparencyValue = 0;

   m_initializedFlag = CMDialog::m_initializedFlag;

   if (m_initializedFlag)
		{
      // Get pointer to memory for temporary storage of channel list. 

      m_localFeaturesPtr = (UInt16*)MNewPointer (
              (UInt32) gImageWindowInfoPtr->totalNumberChannels * sizeof (UInt16));

      m_initializedFlag = (m_localFeaturesPtr != NULL);

		}	// end "if (m_initializedFlag)"
	
   CreateControls ();

}	// end "CMClusterDialog"



CMClusterDialog::~CMClusterDialog (void)

{
   m_localFeaturesPtr = CheckAndDisposePtr (m_localFeaturesPtr);

}	// end "~CMClusterDialog"



BEGIN_EVENT_TABLE (CMClusterDialog, CMDialog)
	EVT_INIT_DIALOG (CMClusterDialog::OnInitDialog)
	EVT_COMBOBOX (IDC_ChannelCombo, CMClusterDialog::OnSelendokChannelCombo)
	EVT_COMBOBOX_DROPDOWN (IDC_ChannelCombo, CMClusterDialog::OnSelendokChannelComboDropDown)
	EVT_RADIOBOX (IDC_Algorithm, CMClusterDialog::OnAlgorithmSel)
	EVT_RADIOBOX (IDC_ClassificationMap, CMClusterDialog::OnClassifySelectedArea)
	EVT_TEXT (IDC_ColumnEnd, CMClusterDialog::CheckColumnEnd)
	EVT_TEXT (IDC_ColumnStart, CMClusterDialog::CheckColumnStart)
	EVT_TEXT (IDC_LineEnd, CMClusterDialog::CheckLineEnd)
	EVT_TEXT (IDC_LineStart, CMClusterDialog::CheckLineStart)
	EVT_TEXT (IDC_LineInterval, CMClusterDialog::CheckLineInterval)
	EVT_TEXT (IDC_ColumnInterval, CMClusterDialog::CheckColumnInterval)
	EVT_BUTTON (IDEntireImage, CMClusterDialog::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMClusterDialog::ToSelectedImage)
	EVT_CHECKBOX (IDC_ImageOverlay, CMClusterDialog::OnImageOverlay)
	EVT_CHOICE (IDC_ImageOverlayCombo, CMClusterDialog::OnSelendokImageOverlayCombo)
	EVT_CHECKBOX (IDC_CreateClusterMaskCheckBox, CMClusterDialog::OnCreateMaskFile)
	EVT_CHOICE (IDC_DiskCombo, CMClusterDialog::OnSelendokMaskFileDiskCombo)
END_EVENT_TABLE ()



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
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);

   wxBoxSizer* bVSizer1 = new wxBoxSizer (wxVERTICAL);

   bSizer137 = new wxBoxSizer (wxHORIZONTAL);

   wxBoxSizer* bSizer139;
   bSizer139 = new wxBoxSizer (wxVERTICAL);

   wxString m_radioBox3Choices[] = {wxT("Single Pass...       "),
												wxT("ISODATA...      "),
												wxT("None...      ")};
   int m_radioBox3NChoices = sizeof (m_radioBox3Choices) / sizeof (wxString);
   m_radioBox3 = new wxRadioBox (this,
											IDC_Algorithm,
											wxT("Algorithm"),
											wxDefaultPosition,
											wxDefaultSize,
											//wxSize (-1, 100),
											m_radioBox3NChoices,
											m_radioBox3Choices,
											3,
											wxRA_SPECIFY_ROWS);
   m_radioBox3->SetSelection (2);
   m_radioBox3->Show (2, false);
   SetUpToolTip (m_radioBox3, IDS_ToolTip80);  
   bSizer139->Add (m_radioBox3, wxSizerFlags(0).Border(wxBOTTOM, 5));

	wxStaticBoxSizer* sbSizer11;
   sbSizer11 = new wxStaticBoxSizer (new wxStaticBox (this,
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
											wxRA_SPECIFY_ROWS|wxNO_BORDER);
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
	/*
   wxBoxSizer* bSizer791;
   bSizer791 = new wxBoxSizer (wxHORIZONTAL);

   m_bpButton4 = new wxBitmapButton (areaStaticBox,
													IDEntireImage,
													entireimi,
													wxDefaultPosition,
													wxDefaultSize,
													wxBU_AUTODRAW);
	m_bpButton4->SetBitmapDisabled (toentirei);
   SetUpToolTip (m_bpButton4, IDS_ToolTip40);
   bSizer791->Add (m_bpButton4,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT, 5));

   m_bpButton5 = new wxBitmapButton (areaStaticBox,
													IDSelectedImage,
													selectedi,
													wxDefaultPosition,
													wxDefaultSize,
													wxBU_AUTODRAW);
	m_bpButton5->SetBitmapDisabled (selectedi);
	bSizer791->Add (m_bpButton5,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT, 5));
	m_bpButton5->Hide ();
	
   sbSizer8->Add (bSizer791, wxSizerFlags(0).Expand());
	*/
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
   m_textCtrl84->SetValidator (wxTextValidator (wxFILTER_NUMERIC, &m_classifyThresholdString));
   SetUpToolTip (m_textCtrl84, IDS_ToolTip82);
   bSizer142->Add (m_textCtrl84, wxSizerFlags(0).Border(wxALL, 5));

   sbSizer11->Add (bSizer142, wxSizerFlags(0).Expand());

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

	m_comboBox29 = new wxComboBox (this,
												IDC_ChannelCombo,
												wxT("All"),
												wxDefaultPosition,
												wxDefaultSize,
												0,
												NULL,
												wxCB_READONLY);
	m_comboBox29->Append (wxT("All"));
	m_comboBox29->Append (wxT("Subset..."));
   SetUpToolTip (m_comboBox29, IDS_ToolTip52);
   bSizer138->Add (m_comboBox29, wxSizerFlags(0).Border(wxLEFT|wxBOTTOM, 5));
   
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
   //sbSizer25->Add (m_checkBox46, 0, wxALL, 5);
   sbSizer25->Add (m_checkBox46, wxSizerFlags(0).Border(wxALL, 5));

   m_checkBox47 = new wxCheckBox (sbSizer25->GetStaticBox (),
												IDC_DiskFile,
												wxT("Text Disk File"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox47, IDS_ToolTip86);
   //sbSizer25->Add (m_checkBox47, 0, wxALL, 5);
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
	/*
	wxSizer* standardButtonSizer = CreateButtonSizer (wxOK | wxCANCEL);
	#if defined multispec_wxmac
		bVSizer1->Add (standardButtonSizer, wxSizerFlags(0).Right());
	#else
		bVSizer1->Add (standardButtonSizer, wxSizerFlags(0).Right().Border(wxTOP|wxBOTTOM, 12));
	#endif
	*/
	CreateStandardButtons (bVSizer1);

   this->SetSizerAndFit (bVSizer1);
	
}	// end "CreateControls"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
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
   wxRadioBox* algsel = (wxRadioBox*)FindWindow (IDC_Algorithm);
   if (returnFlag)
		{
      if (!m_clusterProcedureSetFlag)
         FindWindow (wxID_OK)->Enable (true);

      m_clusterProcedureSetFlag = TRUE;

      m_clusterProcedure = algsel->GetSelection ();

		}	// end "if (returnFlag)"

   else	// !returnFlag
		{
      if (!m_clusterProcedureSetFlag)
         m_clusterProcedure = 2;

      algsel->SetSelection (m_clusterProcedure);

		}	// end "else !returnFlag"

}	// end "OnAlgorithm"



void CMClusterDialog::OnAlgorithmSel (
				wxCommandEvent&					event)

{
   int selalg = event.GetInt ();
   if (selalg == 0)
      OnSinglePassAlgorithm (event);
   else
      OnISODATAAlgorithm (event);
		
}	// end "OnAlgorithmSel"


/*
void CMClusterDialog::OnButtonPress (
				wxKeyEvent&							event)
 
{
	if (event.GetKeyCode () == WXK_RETURN && m_clusterProcedure != -1) 
		{
		if (Validate () && TransferDataFromWindow ())
			EndModal (wxID_OK);
 
		}	// end "if (event.GetKeyCode () == WXK_RETURN && ..."
	
	else	// event.GetKeyCode () != WXK_RETURN || m_clusterProcedure == -1
		{
		event.Skip ();
 
		}	// end "else	// event.GetKeyCode () != WXK_RETURN || "
 
}	// end "OnButtonPress"
*/


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

	this->Fit ();

}	// end "OnCreateMaskFile"



void CMClusterDialog::OnImageOverlay (
				wxCommandEvent&					event)

{
   m_createImageOverlayFlag = ((wxCheckBox*)FindWindow (IDC_ImageOverlay))->GetValue ();

   if (m_createImageOverlayFlag)
      ShowDialogItem (this, IDC_ImageOverlayCombo);
	
   else // m_createImageOverlayFlag
      HideDialogItem (this, IDC_ImageOverlayCombo);
   
   this->Fit ();

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

   UInt16								index;

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
									  (Boolean*)&m_textWindowFlag,
									  (Boolean*)&m_diskFileFlag,
									  (Boolean*)&m_createClusterMaskFlag,
									  &maskFileFormatCode,
									  (Boolean*)&m_createImageOverlayFlag,
									  &selectImageOverlaySelection,
									  (Boolean*)&m_thresholdFlag,
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

   //m_symbolSelection = gClusterSpecsPtr->symbolSet;
   ((wxChoice*)FindWindow (IDC_SymbolCombo))->Delete (1);

    m_saveStatisticsSelection = 0;
   //((wxChoice*)FindWindow (IDC_ClusterStatsCombo))->Enable (false);
	
   if (TransferDataToWindow ())
      PositionDialogWindow ();

			// Set default text selection to first edit text item

   SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);
	//this->Fit ();

}	// end "OnInitDialog"
 


void CMClusterDialog::OnISODATAAlgorithm (
				wxCommandEvent&					event)
				
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



void CMClusterDialog::OnSelendokChannelCombo (
				wxCommandEvent&					event)
				
{
   HandleChannelsMenu (IDC_ChannelCombo,
								kNoTransformation,
								(SInt16) gImageWindowInfoPtr->totalNumberChannels,
								2,
								m_clusterProcedureSetFlag);

}	// end "OnSelendokChannelCombo"


/*
void CMClusterDialog::OnSelendokChannelComboDropDown (
				wxCommandEvent&					event)
 
{
    wxComboBox* classcombo = (wxComboBox *)FindWindow (IDC_ChannelCombo);
    classcombo->SetSelection (0);
 
}	// end "OnSelendokChannelComboDropDown"
*/


void CMClusterDialog::OnSelendokImageOverlayCombo (
				wxCommandEvent&					event)
				
{
	m_selectImageOverlaySelection =
							((wxChoice*)FindWindow (IDC_ImageOverlayCombo))->GetSelection ();

}	// end "OnSelendokImageOverlayCombo"



void CMClusterDialog::OnSelendokMaskFileDiskCombo (
				wxCommandEvent&					event)
				
{
   m_maskFileFormatCode = ((wxChoice*)FindWindow (IDC_DiskCombo))->GetSelection ();
   m_maskFileFormatCode++;

}	// end "OnSelendokMaskFileDiskCombo"



void CMClusterDialog::OnSinglePassAlgorithm (
				wxCommandEvent&					event)
				
{
   Boolean returnFlag = OnePassClusterDialog (gImageFileInfoPtr, this);

   OnAlgorithm (returnFlag);

}	// end "OnSinglePassAlgorithm"  



bool CMClusterDialog::TransferDataFromWindow ()
 
{
	SInt16			returnCode = 0;
	
	
   wxTextCtrl* c_end = (wxTextCtrl*)FindWindow (IDC_ColumnEnd);
   wxTextCtrl* c_inter = (wxTextCtrl*)FindWindow (IDC_ColumnInterval);
   wxTextCtrl* c_start = (wxTextCtrl*)FindWindow (IDC_ColumnStart);
   wxTextCtrl* l_end = (wxTextCtrl*)FindWindow (IDC_LineEnd);
   wxTextCtrl* l_inter = (wxTextCtrl*)FindWindow (IDC_LineInterval);
   wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
   wxTextCtrl* clsfythresh = (wxTextCtrl*)FindWindow (IDC_ClassifyThreshold);

   wxCheckBox* textwin = (wxCheckBox*)FindWindow (IDC_TextWindow);
   wxCheckBox* diskf = (wxCheckBox*)FindWindow (IDC_DiskFile);
   wxCheckBox* crtcluster = (wxCheckBox*)FindWindow (IDC_CreateClusterMaskCheckBox);
   wxCheckBox* imgoverlay = (wxCheckBox*)FindWindow (IDC_ImageOverlay);

   wxChoice* symbolc = (wxChoice*)FindWindow (IDC_SymbolCombo);
   wxChoice* clusterstats = (wxChoice*)FindWindow (IDC_ClusterStatsCombo);
   wxComboBox* chanc = (wxComboBox*)FindWindow (IDC_ChannelCombo);
   wxChoice* imgoverlayc = (wxChoice*)FindWindow (IDC_ImageOverlayCombo);
   wxChoice* diskc = (wxChoice*)FindWindow (IDC_DiskCombo);

   m_LineStartString = l_start->GetValue ();
   m_LineEndString = l_end->GetValue ();
   m_LineIntervalString = l_inter->GetValue ();
   m_ColumnStartString = c_start->GetValue ();
   m_ColumnEndString = c_end->GetValue ();
   m_ColumnIntervalString = c_inter->GetValue ();
   //m_classifyThreshold = wxAtoi (clsfythresh->GetValue ());
   wxString clthresh = clsfythresh->GetValue ();
   clthresh.ToDouble (&m_classifyThreshold);

   m_textWindowFlag = textwin->GetValue ();
   m_diskFileFlag = diskf->GetValue ();
   m_createClusterMaskFlag = crtcluster->GetValue ();
   m_createImageOverlayFlag = imgoverlay->GetValue ();

   m_symbolSelection = symbolc->GetSelection ();
   m_saveStatisticsSelection = clusterstats->GetSelection ();
   m_channelSelection = chanc->GetSelection ();
   m_selectImageOverlaySelection = imgoverlayc->GetSelection ();
   m_maskFileFormatCode = diskc->GetSelection ();

   ClassifyAreaSetting (true, IDC_NoClassificationMap, m_classificationArea);
	
   if (m_channelSelection < 0)
      m_channelSelection = m_channelSelection_Saved;
   
	//if (m_classificationArea == 2)
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
			returnCode = VerifyLineColumnValues (
								  IDC_LineStart,
								  IDC_ColumnStart,
								  true);
								  
		}	// end "if (m_classificationArea == 2)"

   return (returnCode == 0);

}	// end "TransferDataFromWindow"



bool CMClusterDialog::TransferDataToWindow () 
{

   wxTextCtrl* c_end = (wxTextCtrl*)FindWindow (IDC_ColumnEnd);
   wxTextCtrl* c_inter = (wxTextCtrl*)FindWindow (IDC_ColumnInterval);
   wxTextCtrl* c_start = (wxTextCtrl*)FindWindow (IDC_ColumnStart);
   wxTextCtrl* l_end = (wxTextCtrl*)FindWindow (IDC_LineEnd);
   wxTextCtrl* l_inter = (wxTextCtrl*)FindWindow (IDC_LineInterval);
   wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
   wxTextCtrl* clsfythresh = (wxTextCtrl*)FindWindow (IDC_ClassifyThreshold);

   wxCheckBox* textwin = (wxCheckBox*)FindWindow (IDC_TextWindow);
   wxCheckBox* diskf = (wxCheckBox*)FindWindow (IDC_DiskFile);
   wxCheckBox* crtcluster = (wxCheckBox*)FindWindow (IDC_CreateClusterMaskCheckBox);
   wxCheckBox* imgoverlay = (wxCheckBox*)FindWindow (IDC_ImageOverlay);

   wxChoice* symbolc = (wxChoice*)FindWindow (IDC_SymbolCombo);
   wxChoice* clusterstats = (wxChoice*)FindWindow (IDC_ClusterStatsCombo);
   wxComboBox* chanc = (wxComboBox*)FindWindow (IDC_ChannelCombo);
   wxChoice* imgoverlayc = (wxChoice*)FindWindow (IDC_ImageOverlayCombo);
   wxChoice* diskc = (wxChoice*)FindWindow (IDC_DiskCombo);

   c_end->ChangeValue (wxString::Format (wxT("%i"), m_ColumnEnd));
   c_inter->ChangeValue (wxString::Format (wxT("%i"), m_ColumnInterval));
   c_start->ChangeValue (wxString::Format (wxT("%i"), m_ColumnStart));
   l_end->ChangeValue (wxString::Format (wxT("%i"), m_LineEnd));
   l_inter->ChangeValue (wxString::Format (wxT("%i"), m_LineInterval));
   l_start->ChangeValue (wxString::Format (wxT("%i"), m_LineStart));
   clsfythresh->ChangeValue (wxString::Format (wxT("%.1f"), m_classifyThreshold));

   textwin->SetValue (m_textWindowFlag);
   diskf->SetValue (m_diskFileFlag);
   crtcluster->SetValue (m_createClusterMaskFlag);
   imgoverlay->SetValue (m_createImageOverlayFlag);

   symbolc->SetSelection (m_symbolSelection);
   clusterstats->SetSelection (m_saveStatisticsSelection);
   chanc->SetSelection (m_channelSelection);
   imgoverlayc->SetSelection (m_selectImageOverlaySelection);
   diskc->SetSelection (m_maskFileFormatCode);
   
   if (m_clusterProcedureSetFlag)
		{
      wxRadioBox* algsel = (wxRadioBox*)FindWindow (IDC_Algorithm);
      algsel->SetSelection (m_clusterProcedure);
		
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

	//this->Fit ();

}	// end "UpdateClassificationItems"  
