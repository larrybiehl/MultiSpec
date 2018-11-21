//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LPrincipalComponentsDialog.cpp : class implementation file
//	Class Definition:		LPrincipalComponentsDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			11/06/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMPrincipalCompDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "LMultiSpec.h"
#include "LPrincipalComponentsDialog.h"
#include "LImage_dialog.cpp"

		// Pointer to the Principal Components analysis information structure.

extern PrincipalCompSpecsPtr gPrincipalCompSpecsPtr;

extern void PrincipalComponentDialogOK (
        PrincipalCompSpecsPtr principalCompSpecsPtr,
        SInt16 dataCode,
        SInt16 classSelection,
        UInt32 localNumberClasses,
        UInt16* classListPtr,
        Boolean trainingFlag,
        Boolean testFlag,
        DialogSelectArea* dialogSelectAreaPtr,
        SInt16 channelSelection,
        UInt32 localNumberChannels,
        UInt16* localChannelsPtr,
        Boolean listEigenvectorFlag,
        Boolean equalVariancesFlag,
        Boolean saveEigenvectorFlag,
        Boolean textWindowFlag,
        Boolean diskFileFlag);



IMPLEMENT_DYNAMIC_CLASS(CMPrincipalCompDialog, CMDialog)

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

      m_localFeaturesPtr = (UInt16*) MNewPointer(
              (UInt32) gImageWindowInfoPtr->totalNumberChannels * sizeof (UInt16));

      m_initializedFlag = (m_localFeaturesPtr != NULL);

		}	// end "if (m_initializedFlag)"

   if (m_initializedFlag && gPrincipalCompSpecsPtr->projectFlag)
		{
				// Get memory for the local class list vector.

      m_classListPtr = (UInt16*) MNewPointer(
              (UInt32) gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));

      m_initializedFlag = (m_classListPtr != NULL);

		}	// end "if (m_initializedFlag && ...->projectFlag)"
	
	CreateControls();

}	// end "CMPrincipalCompDialog"



CMPrincipalCompDialog::~CMPrincipalCompDialog (void)
{
   m_classListPtr = CheckAndDisposePtr(m_classListPtr);
   m_localFeaturesPtr = CheckAndDisposePtr(m_localFeaturesPtr);

}	// end "~CMPrincipalCompDialog"


BEGIN_EVENT_TABLE(CMPrincipalCompDialog, CMDialog)
	EVT_INIT_DIALOG(CMPrincipalCompDialog::OnInitDialog)
	EVT_TEXT(IDC_ColumnEnd, CMPrincipalCompDialog::CheckColumnEnd)
	EVT_TEXT(IDC_ColumnInterval, CMPrincipalCompDialog::CheckColumnInterval)
	EVT_TEXT(IDC_ColumnStart, CMPrincipalCompDialog::CheckColumnStart)
	EVT_TEXT(IDC_LineEnd, CMPrincipalCompDialog::CheckLineEnd)
	EVT_TEXT(IDC_LineInterval, CMPrincipalCompDialog::CheckLineInterval)
	EVT_TEXT(IDC_LineStart, CMPrincipalCompDialog::CheckLineStart)
	EVT_TEXT(IDC_LineInterval, CMPrincipalCompDialog::CheckLineInterval)
	EVT_TEXT(IDC_ColumnInterval, CMPrincipalCompDialog::CheckColumnInterval)
	EVT_RADIOBUTTON(IDC_Classes, CMPrincipalCompDialog::OnClasses)
	EVT_RADIOBUTTON(IDC_ImageArea, CMPrincipalCompDialog::OnImageArea)
	EVT_COMBOBOX(IDC_ClassCombo, CMPrincipalCompDialog::OnSelendokClassCombo)
	EVT_COMBOBOX_DROPDOWN(IDC_ClassCombo, CMPrincipalCompDialog::OnSelendokClassComboDropDown)
	EVT_COMBOBOX(IDC_ChannelCombo, CMPrincipalCompDialog::OnSelendokChannelCombo)
	EVT_COMBOBOX_DROPDOWN(IDC_ChannelCombo, CMPrincipalCompDialog::OnSelendokChannelComboDropDown)
	EVT_BUTTON(IDEntireImage, CMPrincipalCompDialog::ToEntireImage)
	EVT_BUTTON(IDSelectedImage, CMPrincipalCompDialog::ToSelectedImage)
END_EVENT_TABLE()
 

void CMPrincipalCompDialog::CreateControls()
{
	/*
   wxBitmap entireimi = wxBITMAP_PNG_FROM_DATA(entireim);
   wxBitmap toentirei = wxBITMAP_PNG_FROM_DATA(toentire);
   wxBitmap selectedi = wxBITMAP_PNG_FROM_DATA(selected);
   wxBitmap bmp4i = wxBITMAP_PNG_FROM_DATA(bmp4);
	*/
   wxBoxSizer* bSizer95;
   bSizer95 = new wxBoxSizer(wxVERTICAL);

   wxStaticBoxSizer* sbSizer14;
   sbSizer14 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Area(s)")), wxVERTICAL);

   wxBoxSizer* bSizer100;
   bSizer100 = new wxBoxSizer(wxHORIZONTAL);

   m_radioBtn11 = new wxRadioButton(sbSizer14->GetStaticBox(), IDC_Classes, wxT("Classes:"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
   SetUpToolTip(m_radioBtn11, IDS_ToolTip103);  
   bSizer100->Add(m_radioBtn11, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER | wxALL, 5);

   m_comboBox28 = new wxComboBox(sbSizer14->GetStaticBox(), IDC_ClassCombo, wxT("All"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
   m_comboBox28->Append(wxT("All"));
   m_comboBox28->Append(wxT("Subset..."));
   SetUpToolTip(m_comboBox28, IDS_ToolTip103);  
   bSizer100->Add(m_comboBox28, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER | wxALL, 5);

   wxBoxSizer* bSizer101;
   bSizer101 = new wxBoxSizer(wxVERTICAL);

   m_checkBox29 = new wxCheckBox(sbSizer14->GetStaticBox(), IDC_Training, wxT("Training"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox29, IDS_ToolTip217);  
   bSizer101->Add(m_checkBox29, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5);

   m_checkBox30 = new wxCheckBox(sbSizer14->GetStaticBox(), IDC_Test, wxT("Test"), wxDefaultPosition, wxDefaultSize, 0);
      SetUpToolTip(m_checkBox30, IDS_ToolTip218);  
   bSizer101->Add(m_checkBox30, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5);

   bSizer100->Add(bSizer101, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 5);

   sbSizer14->Add(bSizer100, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 5);

   m_radioBtn10 = new wxRadioButton(sbSizer14->GetStaticBox(), IDC_ImageArea, wxT("Image Area"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_radioBtn10, IDS_ToolTip216);  
   sbSizer14->Add(m_radioBtn10, 0, wxALL, 5);

   wxStaticBoxSizer* sbSizer8;
   sbSizer8 = new wxStaticBoxSizer(new wxStaticBox(sbSizer14->GetStaticBox(), IDC_LineColFrame, wxT("Selected Area")), wxHORIZONTAL);
	/*
   wxBoxSizer* bSizer791;
   bSizer791 = new wxBoxSizer(wxHORIZONTAL);

   m_bpButton4 = new wxBitmapButton(sbSizer8->GetStaticBox(), IDEntireImage, entireimi, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
   m_bpButton4->SetBitmapDisabled(toentirei);
   //bSizer791->Add(m_bpButton4, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxALL, 5);
   bSizer791->Add(m_bpButton4, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

   m_bpButton5 = new wxBitmapButton(sbSizer8->GetStaticBox(), IDSelectedImage, selectedi, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
   m_bpButton5->SetBitmapDisabled(bmp4i);
   //bSizer791->Add(m_bpButton5, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxALL, 5);
   bSizer791->Add(m_bpButton5, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	m_bpButton5->Hide();

   //sbSizer8->Add(bSizer791, 0, wxEXPAND, 5);
   sbSizer8->Add (bSizer791, wxSizerFlags(0).Expand());
	*/
	CreateLineColumnControls (sbSizer8);
	
   sbSizer14->Add (sbSizer8, wxSizerFlags(0).Expand().Border(wxALL, 6));

   bSizer95->Add(sbSizer14, 0, wxEXPAND | wxALL, 12);

	wxFlexGridSizer* gSizer13;
   gSizer13 = new wxFlexGridSizer(0, 2, 0, 0);
   gSizer13->SetFlexibleDirection(wxHORIZONTAL);
   gSizer13->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

   wxBoxSizer* leftColumnBoxSizer;
   leftColumnBoxSizer = new wxBoxSizer(wxVERTICAL);

   wxBoxSizer* bSizer98;
   bSizer98 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText118 = new wxStaticText(this, IDC_ChannelPrompt, wxT("Channel:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText118->Wrap(-1);
   SetUpToolTip(m_staticText118, IDS_ToolTip52);
   bSizer98->Add (m_staticText118, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    
   m_comboBox29 = new wxComboBox(this, IDC_ChannelCombo, wxT("All"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
   m_comboBox29->Append(wxT("All"));
   m_comboBox29->Append(wxT("Subset..."));
   SetUpToolTip(m_comboBox29, IDS_ToolTip52);  
   bSizer98->Add(m_comboBox29, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

   leftColumnBoxSizer->Add(bSizer98, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer19;
	wxStaticBox* optionsBox = new wxStaticBox(this, wxID_ANY, wxT("Options"));
   sbSizer19 = new wxStaticBoxSizer(optionsBox, wxVERTICAL);

   m_checkBox26 = new wxCheckBox(optionsBox, IDC_ListEigenvector, wxT("List eigenvectors"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox26, IDS_ToolTip219);
   sbSizer19->Add(m_checkBox26, 0, wxALL, 5);

   m_checkBox27 = new wxCheckBox(optionsBox, IDC_EqualizeVariances, wxT("Equalize variances (correlation matrix)"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox27, IDS_ToolTip220);
   sbSizer19->Add(m_checkBox27, 0, wxALL, 5);

   m_checkBox28 = new wxCheckBox(optionsBox, IDC_SaveEigenvectors, wxT("Save eigenvalues/ eigenvectors"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox28, IDS_ToolTip221);
   sbSizer19->Add(m_checkBox28, 0, wxALL, 5);

   leftColumnBoxSizer->Add (sbSizer19, wxSizerFlags(0).Expand().Border(wxTOP, 6));

    gSizer13->Add (leftColumnBoxSizer, wxSizerFlags(1).Expand().Border(wxRIGHT, 6));

   wxBoxSizer* rightColumnBoxSizer;
   rightColumnBoxSizer = new wxBoxSizer(wxVERTICAL);
	
	wxStaticBoxSizer* sbSizer18;
	wxStaticBox* outputResultsBox = new wxStaticBox(this, wxID_ANY, wxT("Output results to"));
   sbSizer18 = new wxStaticBoxSizer(outputResultsBox, wxVERTICAL);

   m_checkBox24 = new wxCheckBox(outputResultsBox, IDC_TextWindow, wxT("Text window"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox24, IDS_ToolTip222);
   sbSizer18->Add(m_checkBox24, 0, wxALL, 5);

   m_checkBox25 = new wxCheckBox(outputResultsBox, IDC_DiskFile, wxT("Disk file"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox25, IDS_ToolTip223);
   sbSizer18->Add(m_checkBox25, 0, wxALL, 5);

   rightColumnBoxSizer->Add(sbSizer18, 0, wxEXPAND, 5);
	/*
	wxBoxSizer* bSizer77 = new wxBoxSizer(wxHORIZONTAL);
	wxButton* cancelbtn = new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer77->Add (cancelbtn, wxSizerFlags(0).Border(wxRIGHT,6));
	wxButton* okbtn = new wxButton(this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer77->Add (okbtn, wxSizerFlags(0));
	rightColumnBoxSizer->Add (bSizer77, wxSizerFlags(0).Right().Border(wxRIGHT,0).Border(wxTOP,42));
	*/
   gSizer13->Add (rightColumnBoxSizer, wxSizerFlags(1).Expand().Border(wxLEFT, 6));
	
	bSizer95->Add (gSizer13, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	wxSizer* standardButtonSizer = CreateButtonSizer (wxOK | wxCANCEL);
	bSizer95->Add (standardButtonSizer, wxSizerFlags(0).Right());

   this->SetSizer (bSizer95);
   this->Layout();
   bSizer95->Fit(this);
	
}	// end "CreateControl"



//-----------------------------------------------------------------------------
//					 Copyright (1988-1998)
//                                  c Purdue Research Foundation
//					All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the principal
//				component specification dialog box to the user and copy the
//				revised back to the principal component specification structure if
//				the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/16/97
//	Revised By:			Larry L. Biehl			Date: 05/19/97	

SInt16 CMPrincipalCompDialog::DoDialog(void)
{
   Boolean continueFlag = FALSE;

   SInt16 returnCode;



   // Make sure intialization has been completed.

   if (!m_initializedFlag)
      return (FALSE);

   returnCode = ShowModal();

   if (returnCode == wxID_OK) {
      DialogSelectArea dialogSelectArea;


      continueFlag = TRUE;

      dialogSelectArea.lineStart = m_LineStart;
      dialogSelectArea.lineEnd = m_LineEnd;
      dialogSelectArea.lineInterval = m_LineInterval;
      dialogSelectArea.columnStart = m_ColumnStart;
      dialogSelectArea.columnEnd = m_ColumnEnd;
      dialogSelectArea.columnInterval = m_ColumnInterval;

      PrincipalComponentDialogOK(
              gPrincipalCompSpecsPtr,
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

   } // end "if (returnCode == IDOK)"

   return (continueFlag);

} // end "DoDialog"


void CMPrincipalCompDialog::HideFieldTypeItems() {
	MHideDialogItem(this, IDC_ClassCombo);
	MHideDialogItem(this, IDC_Training);
	MHideDialogItem(this, IDC_Test);
	
} // end "HideFieldTypeItems"

/*
//Press keyboard "Enter"  -> wxID_OK
void CMPrincipalCompDialog::OnButtonPress(wxKeyEvent& event)
{
	if (event.GetKeyCode() == WXK_RETURN) {
		if(Validate() && TransferDataFromWindow())
			this->EndModal(wxID_OK);
	} else {
		event.Skip();
	}
}
*/

void CMPrincipalCompDialog::OnClasses(wxCommandEvent& event)
{
	ShowFieldTypeItems();
	HideSomeAreaSelectionItems();
	
	//	GetDlgItem(IDC_LineInterval)->SetFocus();
	//	SendDlgItemMessage( IDC_LineInterval, EM_SETSEL, 0, MAKELPARAM(0, -1) );
	SelectDialogItemText(this, IDC_LineInterval, 0, SInt16_MAX);
	this->Fit();
	
} // end "OnClasses"


void CMPrincipalCompDialog::OnImageArea(wxCommandEvent& event)
{
	HideFieldTypeItems();
	ShowSomeAreaSelectionItems();
	
	//	GetDlgItem(IDC_LineStart)->SetFocus();
	//	SendDlgItemMessage( IDC_LineStart, EM_SETSEL, 0, MAKELPARAM(0, -1) );
	SelectDialogItemText(this, IDC_LineStart, 0, SInt16_MAX);
	this->Fit();
} // end "OnImageArea"


void CMPrincipalCompDialog::OnInitDialog(wxInitDialogEvent& event) {
   UInt32 index;

   UInt16 selectItem;


   wxDialog::OnInitDialog(event);

   // Make sure that we have the bitmaps for the entire image buttons.

   //VERIFY(toEntireButton.AutoLoad(IDEntireImage, this));
   //VERIFY(toSelectedButton.AutoLoad(IDSelectedImage, this));

   // Update the modal dialog with the default settings
   // Set radio button for "Classes".

   if (gPrincipalCompSpecsPtr->dataCode == kTrainingType)
      m_dataCode = 0;

   else if (gPrincipalCompSpecsPtr->dataCode == kAreaType)
      m_dataCode = 1;

   if (!gPrincipalCompSpecsPtr->projectFlag)
      MHideDialogItem(this, IDC_Classes);

   // Classes to use.

   m_classSelection = gPrincipalCompSpecsPtr->classSet;
   m_localNumberClasses = gPrincipalCompSpecsPtr->numberClasses;

   UInt16* classPtr = (UInt16*) GetHandlePointer(
           gPrincipalCompSpecsPtr->classHandle,
           kNoLock,
           kNoMoveHi);

   if (gPrincipalCompSpecsPtr->projectFlag) {
      for (index = 0; index < m_localNumberClasses; index++)
         m_classListPtr[index] = classPtr[index];

   } // end "if (gPrincipalCompSpecsPtr->projectFlag)"

   // Training or Test fields.

   if (gPrincipalCompSpecsPtr->dataCode == kTrainingType)
      ShowFieldTypeItems();

   else // gPrincipalCompSpecsPtr->dataCode != kTrainingType"
      HideFieldTypeItems();

   // Set control for including training fields.

   m_trainingFlag = (gPrincipalCompSpecsPtr->fieldTypeCode == kTrainingType);

   // Set control for including test fields.

   m_testFlag = (gPrincipalCompSpecsPtr->fieldTypeCode == kTestingType);
   if (gProjectInfoPtr && gProjectInfoPtr->numberStatTestFields <= 0)
      SetDLogControlHilite(this, IDC_Test, 255);

   if (gImageFileInfoPtr == NULL) {
      SetDLogControlHilite(this, IDC_ImageArea, 255);
      MHideDialogItem(this, IDC_LinePrompt);
      MHideDialogItem(this, IDC_ColumnPrompt);
      MHideDialogItem(this, IDC_LineInterval);
      MHideDialogItem(this, IDC_ColumnInterval);

   } // end "if (gImageFileInfoPtr == NULL)"

   //	Selected area for principal components
   // 	Initialize selected area structure.

   InitializeDialogSelectArea(&m_dialogSelectArea,
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

   if (gPrincipalCompSpecsPtr->dataCode == kAreaType) {
      ShowSomeAreaSelectionItems();

      selectItem = IDC_LineStart;

   }// end "if (gPrincipalCompSpecsPtr->dataCode == kAreaType)"

   else // gPrincipalCompSpecsPtr->dataCode != kAreaType
   {
      HideSomeAreaSelectionItems();

      selectItem = IDC_LineInterval;

   } // end "else gPrincipalCompSpecsPtr->dataCode != kAreaType"

   //	Set the All/Subset channels/features list item

   m_channelSelection = gPrincipalCompSpecsPtr->channelSet;
   m_localNumberFeatures = gPrincipalCompSpecsPtr->numberChannels;

   SInt16* channelsPtr = (SInt16*) GetHandlePointer(
           gPrincipalCompSpecsPtr->featureHandle, kNoLock, kNoMoveHi);
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

   if (CMPrincipalCompDialog::TransferDataToWindow())
      PositionDialogWindow();

   // Set default text selection to first edit text item

   //	GetDlgItem(selectItem)->SetFocus();
   //	SendDlgItemMessage( selectItem, EM_SETSEL, 0, MAKELPARAM(0, -1) );
   SelectDialogItemText(this, selectItem, 0, SInt16_MAX);
   
   this->Fit();
   
   //return FALSE; // return TRUE  unless you set the focus to a control

} // end "OnInitDialog"


void CMPrincipalCompDialog::OnSelendokChannelCombo(wxCommandEvent& event)
{
	HandleChannelsMenu(IDC_ChannelCombo,
							 kNoTransformation,
							 (SInt16) gImageWindowInfoPtr->totalNumberChannels,
							 1,
							 TRUE);
	
} // end "OnSelendokChannelCombo"

//void CMPrincipalCompDialog::OnSelendokChannelComboDropDown(wxCommandEvent& event)
//{
//    ResetComboBox(IDC_ChannelCombo);
//} // end "OnSelendokClassComboDropDown"

void CMPrincipalCompDialog::OnSelendokClassCombo(wxCommandEvent& event)
{
   HandleClassesMenu(&m_localNumberClasses,
           (SInt16*) m_classListPtr,
           1,
           (SInt16) gProjectInfoPtr->numberStatisticsClasses,
           IDC_ClassCombo,
           &m_classSelection);

} // end "OnSelendokClassCombo"

//void CMPrincipalCompDialog::OnSelendokClassComboDropDown(wxCommandEvent& event)
//{
//   ResetComboBox(IDC_ClassCombo);
//} // end "OnSelendokClassComboDropDown"

void CMPrincipalCompDialog::ShowFieldTypeItems()
{
	MShowDialogItem(this, IDC_ClassCombo);
	MShowDialogItem(this, IDC_Training);
	MShowDialogItem(this, IDC_Test);
	
} // end "ShowFieldTypeItems"

/*
void CMPrincipalCompDialog::HideSomeAreaSelectionItems()

{                                                          
                MHideDialogItem (this, IDEntireImage);
                MHideDialogItem (this, IDSelectedImage);
                MHideDialogItem (this, IDC_LineStart);
                MHideDialogItem (this, IDC_LineEnd);
                MHideDialogItem (this, IDC_ColumnStart);
                MHideDialogItem (this, IDC_ColumnEnd);
	
}		// end "HideSomeAreaSelectionItems" 

 

void CMPrincipalCompDialog::ShowSomeAreaSelectionItems()

{                                
                MShowDialogItem (this, IDEntireImage);
                MShowDialogItem (this, IDSelectedImage);
		
                        // Determine if this is the entire area and set the
                        // to entire image icon.
	                     
                SetEntireImageButtons (
                                                NULL,
                                                m_LineStart,
                                                m_LineEnd,
                                                m_ColumnStart,
                                                m_ColumnEnd);
						                                                      
                MShowDialogItem (this, IDC_LineStart);
                MShowDialogItem (this, IDC_LineEnd);
                MShowDialogItem (this, IDC_ColumnStart);
                MShowDialogItem (this, IDC_ColumnEnd);
	
}		// end "ShowSomeAreaSelectionItems"
 */


bool CMPrincipalCompDialog::TransferDataFromWindow()

{
	SInt16				returnCode = 0;
	
	
	// Radio button
	wxRadioButton* acl = (wxRadioButton *) FindWindow(IDC_Classes);
	wxRadioButton* aia = (wxRadioButton *) FindWindow(IDC_ImageArea);
	if (aia->GetValue())
		m_dataCode = 1;
	else
		m_dataCode = 0;
	// Text Control
	wxTextCtrl* c_end = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
	wxTextCtrl* c_inter = (wxTextCtrl*) FindWindow(IDC_ColumnInterval);
	wxTextCtrl* c_start = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
	wxTextCtrl* l_end = (wxTextCtrl*) FindWindow(IDC_LineEnd);
	wxTextCtrl* l_inter = (wxTextCtrl*) FindWindow(IDC_LineInterval);
	wxTextCtrl* l_start = (wxTextCtrl*) FindWindow(IDC_LineStart);
	
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
	
	// Check boxes
	wxCheckBox* train = (wxCheckBox*) FindWindow(IDC_Training);
	wxCheckBox* test = (wxCheckBox*) FindWindow(IDC_Test);
	wxCheckBox* leigenvec = (wxCheckBox*) FindWindow(IDC_ListEigenvector);
	wxCheckBox* eqvar = (wxCheckBox*) FindWindow(IDC_EqualizeVariances);
	wxCheckBox* saveigvec = (wxCheckBox*) FindWindow(IDC_SaveEigenvectors);
	wxCheckBox* optxtf = (wxCheckBox*) FindWindow(IDC_TextWindow);
	wxCheckBox* opdskf = (wxCheckBox*) FindWindow(IDC_DiskFile);
	
	m_trainingFlag = train->GetValue();
	m_testFlag = test->GetValue();
	m_listEigenvectorFlag = leigenvec->GetValue();
	m_equalVariancesFlag = eqvar->GetValue();
	m_saveEigenvectorFlag = saveigvec->GetValue();
	m_textWindowFlag = optxtf->GetValue();
	m_diskFileFlag = opdskf->GetValue();
	
	// wxChoice
	wxComboBox* classc = (wxComboBox *) FindWindow(IDC_ClassCombo);
	wxComboBox* channelc = (wxComboBox *) FindWindow(IDC_ChannelCombo);
	m_channelSelection = channelc->GetSelection();
	m_classSelection = classc->GetSelection();
	
   if(m_channelSelection < 0)
      m_channelSelection = m_channelSelection_Saved;
   if(m_classSelection < 0)
      m_classSelection = m_classSelection_Saved;
   
	if (m_dataCode == 1)
		returnCode = VerifyLineColumnValues(
														IDC_LineStart,
														IDC_ColumnStart,
														true);
	
	return (returnCode == 0);
	
}		// end "TransferDataFromWindow"


bool CMPrincipalCompDialog::TransferDataToWindow()
{
	// Radio button
	wxRadioButton* acl = (wxRadioButton *) FindWindow(IDC_Classes);
	wxRadioButton* aia = (wxRadioButton *) FindWindow(IDC_ImageArea);
	if (m_dataCode) {
		acl->SetValue(false);
		aia->SetValue(true);
	} else {
		acl->SetValue(true);
		aia->SetValue(false);
	}
	// Check boxes
	wxCheckBox* train = (wxCheckBox*) FindWindow(IDC_Training);
	wxCheckBox* test = (wxCheckBox*) FindWindow(IDC_Test);
	wxCheckBox* leigenvec = (wxCheckBox*) FindWindow(IDC_ListEigenvector);
	wxCheckBox* eqvar = (wxCheckBox*) FindWindow(IDC_EqualizeVariances);
	wxCheckBox* saveigvec = (wxCheckBox*) FindWindow(IDC_SaveEigenvectors);
	wxCheckBox* optxtf = (wxCheckBox*) FindWindow(IDC_TextWindow);
	wxCheckBox* opdskf = (wxCheckBox*) FindWindow(IDC_DiskFile);
	
	train->SetValue(m_trainingFlag);
	test->SetValue(m_testFlag);
	leigenvec->SetValue(m_listEigenvectorFlag);
	eqvar->SetValue(m_equalVariancesFlag);
	saveigvec->SetValue(m_saveEigenvectorFlag);
	optxtf->SetValue(m_textWindowFlag);
	opdskf->SetValue(m_diskFileFlag);
	
   wxComboBox* channelc = (wxComboBox *) FindWindow(IDC_ChannelCombo);
   wxComboBox* classc = (wxComboBox *) FindWindow(IDC_ClassCombo);
   classc->SetSelection(m_classSelection);
   channelc->SetSelection(m_channelSelection);
   
	// Text Controls
	wxTextCtrl* c_end = (wxTextCtrl*) FindWindow(ID3C_ColumnEnd);
	wxTextCtrl* c_inter = (wxTextCtrl*) FindWindow(ID3C_ColumnInterval);
	wxTextCtrl* c_start = (wxTextCtrl*) FindWindow(ID3C_ColumnStart);
	wxTextCtrl* l_end = (wxTextCtrl*) FindWindow(ID3C_LineEnd);
	wxTextCtrl* l_inter = (wxTextCtrl*) FindWindow(ID3C_LineInterval);
	wxTextCtrl* l_start = (wxTextCtrl*) FindWindow(ID3C_LineStart);
	
	/*c_end->Clear();
	 c_inter->Clear();
	 c_start->Clear();
	 l_end->Clear();
	 l_inter->Clear();
	 l_start->Clear();
	 
	 *c_end << m_ColumnEnd;
	 *c_inter << m_ColumnInterval;
	 *c_start << m_ColumnStart;
	 *l_end << m_LineEnd;
	 *l_inter << m_LineInterval;
	 *l_start << m_LineStart;
	 * */
	c_end->ChangeValue(wxString::Format(wxT("%i"), m_ColumnEnd));
	c_inter->ChangeValue(wxString::Format(wxT("%i"), m_ColumnInterval));
	c_start->ChangeValue(wxString::Format(wxT("%i"), m_ColumnStart));
	l_end->ChangeValue(wxString::Format(wxT("%i"), m_LineEnd));
	l_inter->ChangeValue(wxString::Format(wxT("%i"), m_LineInterval));
	l_start->ChangeValue(wxString::Format(wxT("%i"), m_LineStart));
	return true;
}
