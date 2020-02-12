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
//	File:						xPrincipalComponentsDialog.cpp : class implementation file
//	Class Definition:		xPrincipalComponentsDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			11/21/2019
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMPrincipalCompDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xMultiSpec.h"
#include "xPrincipalComponentsDialog.h"

		// Pointer to the Principal Components analysis information structure.

extern PrincipalCompSpecsPtr gPrincipalCompSpecsPtr;



IMPLEMENT_DYNAMIC_CLASS (CMPrincipalCompDialog, CMDialog)


BEGIN_EVENT_TABLE (CMPrincipalCompDialog, CMDialog)
	EVT_BUTTON (IDEntireImage, CMPrincipalCompDialog::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMPrincipalCompDialog::ToSelectedImage)

	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_ChannelCombo, CMPrincipalCompDialog::OnChannelComboSelendok)
		EVT_COMBOBOX (IDC_ClassCombo, CMPrincipalCompDialog::OnClassComboSelendok)
	#endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_ChannelCombo, CMPrincipalCompDialog::OnChannelComboSelendok)
		EVT_CHOICE (IDC_ClassCombo, CMPrincipalCompDialog::OnClassComboSelendok)
	#endif

	EVT_COMBOBOX_CLOSEUP (IDC_ChannelCombo, CMPrincipalCompDialog::OnChannelComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_ClassCombo, CMPrincipalCompDialog::OnClassComboCloseUp)

	EVT_COMBOBOX_DROPDOWN (IDC_ChannelCombo, CMPrincipalCompDialog::OnChannelComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_ClassCombo, CMPrincipalCompDialog::OnClassComboDropDown)

	EVT_INIT_DIALOG (CMPrincipalCompDialog::OnInitDialog)

	EVT_RADIOBUTTON (IDC_Classes, CMPrincipalCompDialog::OnClasses)
	EVT_RADIOBUTTON (IDC_ImageArea, CMPrincipalCompDialog::OnImageArea)

	EVT_TEXT (IDC_ColumnEnd, CMPrincipalCompDialog::CheckColumnEnd)
	EVT_TEXT (IDC_ColumnInterval, CMPrincipalCompDialog::CheckColumnInterval)
	EVT_TEXT (IDC_ColumnStart, CMPrincipalCompDialog::CheckColumnStart)
	EVT_TEXT (IDC_LineEnd, CMPrincipalCompDialog::CheckLineEnd)
	EVT_TEXT (IDC_LineInterval, CMPrincipalCompDialog::CheckLineInterval)
	EVT_TEXT (IDC_LineStart, CMPrincipalCompDialog::CheckLineStart)
END_EVENT_TABLE ()



CMPrincipalCompDialog::CMPrincipalCompDialog (
				wxWindow*							pParent,
				const wxString&					title)
		: CMDialog (CMPrincipalCompDialog::IDD, pParent, title)

{
   m_dataCode = -1;
   m_trainingFlag = FALSE;
   m_testFlag = FALSE;
   m_listEigenvectorFlag = FALSE;
   m_equalVariancesFlag = FALSE;
   m_saveEigenvectorFlag = FALSE;
   m_textWindowFlag = FALSE;
   m_diskFileFlag = FALSE;

   m_initializedFlag = CMDialog::m_initializedFlag;

   if (m_initializedFlag)
		{
				// Get pointer to memory for temporary storage of channel list.

      m_localFeaturesPtr = (UInt16*)MNewPointer (
              (UInt32) gImageWindowInfoPtr->totalNumberChannels * sizeof (UInt16));

      m_initializedFlag = (m_localFeaturesPtr != NULL);

		}	// end "if (m_initializedFlag)"

   if (m_initializedFlag && gPrincipalCompSpecsPtr->projectFlag)
		{
				// Get memory for the local class list vector.

      m_classListPtr = (UInt16*)MNewPointer (
              (UInt32) gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));

      m_initializedFlag = (m_classListPtr != NULL);

		}	// end "if (m_initializedFlag && ...->projectFlag)"
	
	CreateControls ();

}	// end "CMPrincipalCompDialog"



CMPrincipalCompDialog::~CMPrincipalCompDialog (void)

{
   m_classListPtr = CheckAndDisposePtr (m_classListPtr);
   m_localFeaturesPtr = CheckAndDisposePtr (m_localFeaturesPtr);

}	// end "~CMPrincipalCompDialog"
 

void CMPrincipalCompDialog::CreateControls ()

{
   wxBoxSizer* bSizer95;
   bSizer95 = new wxBoxSizer (wxVERTICAL);

   wxStaticBoxSizer* sbSizer14;
   sbSizer14 = new wxStaticBoxSizer (new wxStaticBox (this, wxID_ANY, wxT("Area(s)")),
   												wxVERTICAL);

   wxBoxSizer* bSizer100;
   bSizer100 = new wxBoxSizer (wxHORIZONTAL);

   m_radioBtn11 = new wxRadioButton (sbSizer14->GetStaticBox (),
   												IDC_Classes,
   												wxT("Classes:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												wxRB_GROUP);
   SetUpToolTip (m_radioBtn11, IDS_ToolTip103);
   bSizer100->Add (
   		m_radioBtn11, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER | wxALL, 5);

	GetAllSubsetMenuControl (kClassesMenu,
										sbSizer14->GetStaticBox (),
										IDC_ClassCombo,
										120,
										IDS_ToolTip103);
   bSizer100->Add (
		m_classesCtrl, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER | wxALL, 5);

   wxBoxSizer* bSizer101;
   bSizer101 = new wxBoxSizer (wxVERTICAL);

   m_checkBox29 = new wxCheckBox (sbSizer14->GetStaticBox (),
   											IDC_Training,
   											wxT("Training"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_checkBox29, IDS_ToolTip217);
   bSizer101->Add (m_checkBox29, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5);

   m_checkBox30 = new wxCheckBox (sbSizer14->GetStaticBox (),
   											IDC_Test,
   											wxT("Test"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
	SetUpToolTip (m_checkBox30, IDS_ToolTip218);
   bSizer101->Add (m_checkBox30, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5);

   bSizer100->Add (bSizer101, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 5);

   sbSizer14->Add (bSizer100, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 5);

   m_radioBtn10 = new wxRadioButton (sbSizer14->GetStaticBox (),
													IDC_ImageArea,
													wxT("Image Area"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_radioBtn10, IDS_ToolTip216);
   sbSizer14->Add (m_radioBtn10, 0, wxALL, 5);

   wxStaticBoxSizer* sbSizer8;
   sbSizer8 = new wxStaticBoxSizer (new wxStaticBox (sbSizer14->GetStaticBox (),
   																	IDC_LineColFrame,
   																	wxT("Selected Area"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL),
													wxHORIZONTAL);

	CreateLineColumnControls (sbSizer8);
	
   sbSizer14->Add (sbSizer8, wxSizerFlags(0).Expand().Border(wxALL, 6));

   bSizer95->Add (sbSizer14, 0, wxEXPAND | wxALL, 12);

	wxFlexGridSizer* gSizer13;
   gSizer13 = new wxFlexGridSizer (0, 2, 0, 0);
   gSizer13->SetFlexibleDirection (wxHORIZONTAL);
   gSizer13->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);

   wxBoxSizer* leftColumnBoxSizer;
   leftColumnBoxSizer = new wxBoxSizer (wxVERTICAL);

   wxBoxSizer* bSizer98;
   bSizer98 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText118 = new wxStaticText (this,
   												IDC_ChannelPrompt,
   												wxT("Channel:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText118->Wrap (-1);
   SetUpToolTip (m_staticText118, IDS_ToolTip52);
   bSizer98->Add (m_staticText118, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

   GetAllSubsetMenuControl (kChannelsMenu,
										this,
										IDC_ChannelCombo,
										120,
										IDS_ToolTip52);
   bSizer98->Add (m_channelsCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

   leftColumnBoxSizer->Add (bSizer98, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer19;
	wxStaticBox* optionsBox = new wxStaticBox (this, wxID_ANY, wxT("Options"));
   sbSizer19 = new wxStaticBoxSizer (optionsBox, wxVERTICAL);

   m_checkBox26 = new wxCheckBox (optionsBox,
   											IDC_ListEigenvector,
   											wxT("List eigenvectors"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_checkBox26, IDS_ToolTip219);
   sbSizer19->Add (m_checkBox26, 0, wxALL, 5);

   m_checkBox27 = new wxCheckBox (optionsBox,
   											IDC_EqualizeVariances,
   											wxT("Equalize variances (correlation matrix)"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_checkBox27, IDS_ToolTip220);
   sbSizer19->Add (m_checkBox27, 0, wxALL, 5);

   m_checkBox28 = new wxCheckBox (optionsBox,
												IDC_SaveEigenvectors,
												wxT("Save eigenvalues / eigenvectors"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox28, IDS_ToolTip221);
   sbSizer19->Add (m_checkBox28, 0, wxALL, 5);

   leftColumnBoxSizer->Add (sbSizer19, wxSizerFlags(0).Expand().Border(wxTOP, 6));

    gSizer13->Add (leftColumnBoxSizer, wxSizerFlags(1).Expand().Border(wxRIGHT, 6));

   wxBoxSizer* rightColumnBoxSizer;
   rightColumnBoxSizer = new wxBoxSizer (wxVERTICAL);
	
	wxStaticBoxSizer* sbSizer18;
	wxStaticBox* outputResultsBox = new wxStaticBox (this,
																		wxID_ANY,
																		wxT("Output results to"));
   sbSizer18 = new wxStaticBoxSizer (outputResultsBox, wxVERTICAL);

   m_checkBox24 = new wxCheckBox (outputResultsBox,
   											IDC_TextWindow,
												wxT("Text window"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox24, IDS_ToolTip222);
   sbSizer18->Add (m_checkBox24, 0, wxALL, 5);

   m_checkBox25 = new wxCheckBox (outputResultsBox,
   											IDC_DiskFile,
   											wxT("Disk file"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_checkBox25, IDS_ToolTip223);
   sbSizer18->Add (m_checkBox25, 0, wxALL, 5);

   rightColumnBoxSizer->Add (sbSizer18, 0, wxEXPAND, 5);

   gSizer13->Add (rightColumnBoxSizer, wxSizerFlags(1).Expand().Border(wxLEFT, 6));
	
	bSizer95->Add (gSizer13, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	CreateStandardButtons (bSizer95);

	SetSizerAndFit (bSizer95);
	
}	// end "CreateControl"



SInt16 CMPrincipalCompDialog::DoDialog (void)

{
   SInt16 								returnCode;
   Boolean 								continueFlag = FALSE;


   		// Make sure intialization has been completed.

   if (!m_initializedFlag)
      																				return (FALSE);

   returnCode = ShowModal ();

   if (returnCode == wxID_OK)
   	{
      DialogSelectArea dialogSelectArea;

      continueFlag = TRUE;

      dialogSelectArea.lineStart = m_LineStart;
      dialogSelectArea.lineEnd = m_LineEnd;
      dialogSelectArea.lineInterval = m_LineInterval;
      dialogSelectArea.columnStart = m_ColumnStart;
      dialogSelectArea.columnEnd = m_ColumnEnd;
      dialogSelectArea.columnInterval = m_ColumnInterval;

      PrincipalComponentDialogOK (gPrincipalCompSpecsPtr,
											  m_dataCode,
											  m_classSelection,
											  m_localNumberClasses,
											  m_classListPtr,
											  m_trainingFlag,
											  m_testFlag,
											  &dialogSelectArea,
											  m_channelSelection,
											  m_localActiveNumberFeatures,
											  m_localFeaturesPtr,
											  m_listEigenvectorFlag,
											  m_equalVariancesFlag,
											  m_saveEigenvectorFlag,
											  m_textWindowFlag,
											  m_diskFileFlag);

		}	// end "if (returnCode == IDOK)"

   return (continueFlag);

}	// end "DoDialog"



void CMPrincipalCompDialog::HideFieldTypeItems ()

{
	MHideDialogItem (this, IDC_ClassCombo);
	MHideDialogItem (this, IDC_Training);
	MHideDialogItem (this, IDC_Test);
	
}	// end "HideFieldTypeItems"



void CMPrincipalCompDialog::OnClasses (
				wxCommandEvent& 					event)

{
	ShowFieldTypeItems ();
	HideSomeAreaSelectionItems ();
	
	SelectDialogItemText (this, IDC_LineInterval, 0, SInt16_MAX);
	Fit ();
	
}	// end "OnClasses"



void CMPrincipalCompDialog::OnImageArea (
				wxCommandEvent& 					event)

{
	HideFieldTypeItems ();
	ShowSomeAreaSelectionItems ();
	
	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);
	Fit ();
	
}	// end "OnImageArea"



void CMPrincipalCompDialog::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	UInt32 								index;

   UInt16 								selectItem;


   wxDialog::OnInitDialog (event);

   		// Update the modal dialog with the default settings
   		// Set radio button for "Classes".

   if (gPrincipalCompSpecsPtr->dataCode == kTrainingType)
      m_dataCode = 0;

   else if (gPrincipalCompSpecsPtr->dataCode == kAreaType)
      m_dataCode = 1;

   if (!gPrincipalCompSpecsPtr->projectFlag)
      MHideDialogItem (this, IDC_Classes);

   		// Classes to use.

   m_classSelection = gPrincipalCompSpecsPtr->classSet;
   m_localNumberClasses = gPrincipalCompSpecsPtr->numberClasses;

   UInt16* classPtr = (UInt16*)GetHandlePointer (
															gPrincipalCompSpecsPtr->classHandle);

   if (gPrincipalCompSpecsPtr->projectFlag)
   	{
      for (index = 0; index < m_localNumberClasses; index++)
         m_classListPtr[index] = classPtr[index];

  		}	// end "if (gPrincipalCompSpecsPtr->projectFlag)"

   		// Training or Test fields.

   if (gPrincipalCompSpecsPtr->dataCode == kTrainingType)
      ShowFieldTypeItems ();

   else	// gPrincipalCompSpecsPtr->dataCode != kTrainingType"
      HideFieldTypeItems ();

   		// Set control for including training fields.

   m_trainingFlag = (gPrincipalCompSpecsPtr->fieldTypeCode == kTrainingType);

   		// Set control for including test fields.

   m_testFlag = (gPrincipalCompSpecsPtr->fieldTypeCode == kTestingType);
   if (gProjectInfoPtr && gProjectInfoPtr->numberStatTestFields <= 0)
      SetDLogControlHilite (this, IDC_Test, 255);

   if (gImageFileInfoPtr == NULL)
   	{
      SetDLogControlHilite (this, IDC_ImageArea, 255);
      MHideDialogItem (this, IDC_LinePrompt);
      MHideDialogItem (this, IDC_ColumnPrompt);
      MHideDialogItem (this, IDC_LineInterval);
      MHideDialogItem (this, IDC_ColumnInterval);

  		}	// end "if (gImageFileInfoPtr == NULL)"

   		//	Selected area for principal components
   		// 	Initialize selected area structure.

   InitializeDialogSelectArea (&m_dialogSelectArea,
           gImageWindowInfoPtr,
           gActiveImageWindow,
           gPrincipalCompSpecsPtr->columnStart,
           gPrincipalCompSpecsPtr->columnEnd,
           gPrincipalCompSpecsPtr->columnInterval,
           gPrincipalCompSpecsPtr->lineStart,
           gPrincipalCompSpecsPtr->lineEnd,
           gPrincipalCompSpecsPtr->lineInterval,
           14,
           10,
           kAdjustToBaseImage);

   m_LineStart = gPrincipalCompSpecsPtr->lineStart;
   m_LineEnd = gPrincipalCompSpecsPtr->lineEnd;
   m_LineInterval = gPrincipalCompSpecsPtr->lineInterval;
   m_ColumnStart = gPrincipalCompSpecsPtr->columnStart;
   m_ColumnEnd = gPrincipalCompSpecsPtr->columnEnd;
   m_ColumnInterval = gPrincipalCompSpecsPtr->columnInterval;

   		// Set radio button for area.

   if (gPrincipalCompSpecsPtr->dataCode == kAreaType)
   	{
      ShowSomeAreaSelectionItems ();

      selectItem = IDC_LineStart;

   	}	// end "if (gPrincipalCompSpecsPtr->dataCode == kAreaType)"

   else	// gPrincipalCompSpecsPtr->dataCode != kAreaType
   	{
      HideSomeAreaSelectionItems ();

      selectItem = IDC_LineInterval;

  		}	// end "else gPrincipalCompSpecsPtr->dataCode != kAreaType"

   		//	Set the All/Subset channels/features list item

   m_channelSelection = gPrincipalCompSpecsPtr->channelSet;
   m_localNumberFeatures = gPrincipalCompSpecsPtr->numberChannels;

   SInt16* channelsPtr = (SInt16*)GetHandlePointer (
           											gPrincipalCompSpecsPtr->featureHandle);
   for (index = 0; index < m_localNumberFeatures; index++)
      m_localFeaturesPtr[index] = channelsPtr[index];

   		// Set feature parameters

   m_localActiveNumberFeatures = m_localNumberFeatures;
   m_localActiveFeaturesPtr = m_localFeaturesPtr;

   		// Set check box for "List eigenvector vectors".

   m_listEigenvectorFlag = gPrincipalCompSpecsPtr->listEigenvectorFlag;

   		// Set check box for "Equalize variances".

   m_equalVariancesFlag = gPrincipalCompSpecsPtr->equalVariancesFlag;

   		// Set check box for "Save eigenvector flag".

   m_saveEigenvectorFlag = gPrincipalCompSpecsPtr->saveEigenvectorFlag;

   		// Set check box for "text output window".

   if (gPrincipalCompSpecsPtr->outputStorageType & 0x0001)
      m_textWindowFlag = TRUE;

   		// Set check box for "disk file".

   if (gPrincipalCompSpecsPtr->outputStorageType & 0x0002)
      m_diskFileFlag = TRUE;

   if (CMPrincipalCompDialog::TransferDataToWindow ())
      PositionDialogWindow ();

   		// Set default text selection to first edit text item

	SelectDialogItemText (this, selectItem, 0, SInt16_MAX);
   
   Fit ();
	
}	// end "OnInitDialog"



void CMPrincipalCompDialog::OnChannelComboSelendok (
				wxCommandEvent& 					event)

{
	HandleChannelsMenu (IDC_ChannelCombo,
								 kNoTransformation,
								 (SInt16) gImageWindowInfoPtr->totalNumberChannels,
								 1,
								 TRUE);
	
}	// end "OnChannelComboSelendok"



void CMPrincipalCompDialog::OnClassComboSelendok (
				wxCommandEvent& 					event)

{
   HandleClassesMenu (&m_localNumberClasses,
							  (SInt16*)m_classListPtr,
							  1,
							  (SInt16) gProjectInfoPtr->numberStatisticsClasses,
							  IDC_ClassCombo,
							  &m_classSelection);

}	// end "OnClassComboSelendok"



void CMPrincipalCompDialog::ShowFieldTypeItems ()

{
	MShowDialogItem (this, IDC_ClassCombo);
	MShowDialogItem (this, IDC_Training);
	MShowDialogItem (this, IDC_Test);
	
}	// end "ShowFieldTypeItems"



bool CMPrincipalCompDialog::TransferDataFromWindow ()

{
	SInt16								returnCode = 0;
	
	
			// Radio button
	
	wxRadioButton* aia = (wxRadioButton*)FindWindow (IDC_ImageArea);
	if (aia->GetValue ())
		m_dataCode = 1;
	
	else	// !aia->GetValue ()
		m_dataCode = 0;
	
			// Text Control
	
	TransferLinesColumnsFromWindow ();

			// Check boxes
	
	wxCheckBox* train = (wxCheckBox*)FindWindow (IDC_Training);
	m_trainingFlag = train->GetValue ();
	
	wxCheckBox* test = (wxCheckBox*)FindWindow (IDC_Test);
	m_testFlag = test->GetValue ();
	
	wxCheckBox* leigenvec = (wxCheckBox*)FindWindow (IDC_ListEigenvector);
	m_listEigenvectorFlag = leigenvec->GetValue ();
	
	wxCheckBox* eqvar = (wxCheckBox*)FindWindow (IDC_EqualizeVariances);
	m_equalVariancesFlag = eqvar->GetValue ();
	
	wxCheckBox* saveigvec = (wxCheckBox*)FindWindow (IDC_SaveEigenvectors);
	m_saveEigenvectorFlag = saveigvec->GetValue ();
	
	wxCheckBox* optxtf = (wxCheckBox*)FindWindow (IDC_TextWindow);
	m_textWindowFlag = optxtf->GetValue ();
	
	wxCheckBox* opdskf = (wxCheckBox*)FindWindow (IDC_DiskFile);
	m_diskFileFlag = opdskf->GetValue ();
	
	m_channelSelection = m_channelsCtrl->GetSelection ();
	
	m_classSelection = m_classesCtrl->GetSelection ();
	
	if (m_dataCode == 1)
		returnCode = VerifyLineColumnValues (IDC_LineStart,
															IDC_ColumnStart,
															true);
	
	return (returnCode == 0);
	
}	// end "TransferDataFromWindow"



bool CMPrincipalCompDialog::TransferDataToWindow ()

{
			// Radio button
	
	wxRadioButton* acl = (wxRadioButton*)FindWindow (IDC_Classes);
	wxRadioButton* aia = (wxRadioButton*)FindWindow (IDC_ImageArea);
	if (m_dataCode != 0)
		{
		acl->SetValue (false);
		aia->SetValue (true);
		
		}	// end "if (m_dataCode)"
	
	else	// m_dataCode == 0
		{
		acl->SetValue (true);
		aia->SetValue (false);
		
		}	// end "else m_dataCode == 0"
	
			// Check boxes
	
	wxCheckBox* train = (wxCheckBox*)FindWindow (IDC_Training);
	train->SetValue (m_trainingFlag);
	
	wxCheckBox* test = (wxCheckBox*)FindWindow (IDC_Test);
	test->SetValue (m_testFlag);
	
	wxCheckBox* leigenvec = (wxCheckBox*)FindWindow (IDC_ListEigenvector);
	leigenvec->SetValue (m_listEigenvectorFlag);
	
	wxCheckBox* eqvar = (wxCheckBox*)FindWindow (IDC_EqualizeVariances);
	eqvar->SetValue (m_equalVariancesFlag);
	
	wxCheckBox* saveigvec = (wxCheckBox*)FindWindow (IDC_SaveEigenvectors);
	saveigvec->SetValue (m_saveEigenvectorFlag);
	
	wxCheckBox* optxtf = (wxCheckBox*)FindWindow (IDC_TextWindow);
	optxtf->SetValue (m_textWindowFlag);
	
	wxCheckBox* opdskf = (wxCheckBox*)FindWindow (IDC_DiskFile);
	opdskf->SetValue (m_diskFileFlag);
	
   m_channelsCtrl->SetSelection (m_channelSelection);
	
   m_classesCtrl->SetSelection (m_classSelection);
   
			// Text Controls
	
	TransferLinesColumnsToWindow ();

	return true;
	
}	// end "TransferDataToWindow"
