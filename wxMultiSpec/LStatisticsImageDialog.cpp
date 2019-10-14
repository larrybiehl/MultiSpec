//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LStatisticsImageDialog.cpp : class implementation file
//	Class Definition:		LStatisticsImageDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			10/10/2019
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMStatImageDialog class.
//
//------------------------------------------------------------------------------------
//
#include "LStatisticsImageDialog.h"
#include "LImage_dialog.cpp"



BEGIN_EVENT_TABLE (CMStatImageDialog, CMDialog)
	EVT_BUTTON(IDEntireImage, CMStatImageDialog::ToEntireImage)
	EVT_BUTTON(IDSelectedImage, CMStatImageDialog::ToSelectedImage)

	EVT_COMBOBOX (IDC_ChannelCombo, CMStatImageDialog::OnChannelComboSelendok)
	EVT_COMBOBOX (IDC_ClassCombo, CMStatImageDialog::OnClassComboSelendok)

	EVT_COMBOBOX_CLOSEUP (IDC_ChannelCombo, CMStatImageDialog::OnChannelComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_ClassCombo, CMStatImageDialog::OnClassComboCloseUp)

	EVT_COMBOBOX_DROPDOWN (IDC_ChannelCombo, CMStatImageDialog::OnChannelComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_ClassCombo, CMStatImageDialog::OnClassComboDropDown)

	EVT_INIT_DIALOG(CMStatImageDialog::OnInitDialog)

	EVT_RADIOBUTTON (IDC_IndividualRadio, CMStatImageDialog::OnClickIndividualRadio)
	EVT_RADIOBUTTON (IDC_OverallRadio, CMStatImageDialog::OnClickOverallRadio)
	EVT_RADIOBUTTON (IDC_UserSettingRadio, CMStatImageDialog::OnClickUserSettingRadio)

	EVT_TEXT (IDC_ColumnEnd, CMStatImageDialog::CheckColumnEnd)
	EVT_TEXT (IDC_ColumnStart, CMStatImageDialog::CheckColumnStart)
	EVT_TEXT (IDC_ColumnInterval, CMStatImageDialog::CheckColumnInterval)
	EVT_TEXT (IDC_LineEnd, CMStatImageDialog::CheckLineEnd)
	EVT_TEXT (IDC_LineStart, CMStatImageDialog::CheckLineStart)
	EVT_TEXT (IDC_LineInterval, CMStatImageDialog::CheckLineInterval)
END_EVENT_TABLE ()



CMStatImageDialog::CMStatImageDialog (
				wxWindow* 							parent,
				wxWindowID 							id,
				const wxString& 					title)
		: CMDialog (CMStatImageDialog::IDD, parent, title)

{
   UInt16**					classPtrPtr;
   
   m_classPtr = NULL;
   m_featurePtr = NULL;
   m_channelsPtr = NULL;
   
   m_channelSelection = 0;
   m_featureTransformationFlag = FALSE;
   
	m_perClassFieldCode = 0;
   
   m_areaCode = 0;
   
   m_overallMinMaxCode = 0;
	m_individualMinMaxCode = 0;
	m_userMinMaxCode = 0;
   m_userMinimum = 0.0;
   m_userMaximum = 0.0;
   
   m_initializedFlag = CMDialog::m_initializedFlag;
   m_channelsAllAvailableFlag = FALSE;
   
   if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)
      classPtrPtr = &m_classListPtr;
   else
      classPtrPtr = NULL;
   
	m_initializedFlag = GetDialogLocalVectors (&m_localFeaturesPtr,
																	&m_localTransformFeaturesPtr,
																	classPtrPtr,
																	NULL,
																	NULL,
																	NULL,
																	NULL,
																	NULL);
   
   
   CreateControls();
   this->SetSizerAndFit (bSizer292);
	
}	// end "CMStatImageDialog"


CMStatImageDialog::~CMStatImageDialog ()

{
   if (m_classListPtr != NULL)
      ReleaseDialogLocalVectors (m_localFeaturesPtr,
											m_localTransformFeaturesPtr,
											m_classListPtr,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	
}	// end "~CMStatImageDialog"



void CMStatImageDialog::CreateControls ()

{
   this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	bSizer292 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer332;
	bSizer332 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText312 = new wxStaticText( this, wxID_ANY, wxT("Channels:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText312->Wrap( -1 );
   SetUpToolTip(m_staticText312, IDS_ToolTip52);
	bSizer332->Add( m_staticText312, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_comboBox57 = new wxComboBox( this, IDC_ChannelCombo, wxT("All"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
   m_comboBox57->Append( wxT("All") );
	m_comboBox57->Append( wxT("Subset...") );
   SetUpToolTip(m_comboBox57, IDS_ToolTip52);
	//bSizer332->Add (m_comboBox57, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
   //bSizer332->Add (m_comboBox57, 0, wxALIGN_CENTER);
   bSizer332->Add (m_comboBox57, wxSizerFlags(0).Align(wxALIGN_CENTER));
	
	m_checkBox74 = new wxCheckBox( this, IDC_FeatureTransformation, wxT("Use feature transformation"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox74, IDS_ToolTip225);
	bSizer332->Add( m_checkBox74, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxLEFT, 5 );
	
	
	bSizer292->Add( bSizer332, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );
	
	wxBoxSizer* bSizer333;
	bSizer333 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText313 = new wxStaticText( this, wxID_ANY, wxT("Area(s)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText313->Wrap( -1 );
	bSizer333->Add( m_staticText313, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer334;
	bSizer334 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer334->Add( 20, 0, 0, wxEXPAND, 5 );
	
	m_radioBtn36 = new wxRadioButton (this,
													IDC_ClassesRadio,
													wxT("Classes:"),
													wxDefaultPosition,
													wxDefaultSize,
													wxRB_SINGLE);
   SetUpToolTip(m_radioBtn36, IDS_ToolTip103);
	//bSizer334->Add( m_radioBtn36, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxEXPAND|wxALL, 5 );
	//bSizer334->Add( m_radioBtn36, 0, wxALL, 5 );
   bSizer334->Add (m_radioBtn36, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));
	
	m_comboBox56 = new wxComboBox( this, IDC_ClassCombo, wxT("All"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
   m_comboBox56->Append( wxT("All") );
	m_comboBox56->Append( wxT("Subset...") );
   SetUpToolTip(m_comboBox56, IDS_ToolTip103);
	//bSizer334->Add( m_comboBox56, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	//bSizer334->Add( m_comboBox56, 0, wxALIGN_CENTER, 5 );
   bSizer334->Add (m_comboBox56, wxSizerFlags(0).Align(wxALIGN_CENTER));
	
	bSizer334->Add( 10, 0, 0, wxEXPAND, 5 );
	
	m_staticText315 = new wxStaticText( this, wxID_ANY, wxT("Fields:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText315->Wrap( -1 );
	m_staticText315->Hide ();
	//bSizer334->Add (m_staticText315, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL, 5 );
   //bSizer334->Add (m_staticText315, 0, wxALIGN_CENTER|wxALIGN_CENTER, 5 );
   bSizer334->Add (m_staticText315, wxSizerFlags(0).Align(wxALIGN_CENTER));
   
	m_comboBox58 = new wxComboBox( this, IDC_Fields, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	//bSizer334->Add( m_comboBox58, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
   //bSizer334->Add( m_comboBox58, 0, wxALIGN_CENTER, 5 );
   bSizer334->Add (m_comboBox58, wxSizerFlags(0).Align(wxALIGN_CENTER));
	m_comboBox58->Hide();
	
	bSizer333->Add (bSizer334, 0, wxEXPAND, 10);
	
	wxBoxSizer* bSizer335;
	bSizer335 = new wxBoxSizer (wxHORIZONTAL);
	
	bSizer335->Add (20, 0, 0, wxEXPAND, 5);
	
	m_staticText316 = new wxStaticText( this, IDC_StatPrompt, wxT("Create statistics image for each selected"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText316->Wrap( -1 );
	//bSizer335->Add( m_staticText316, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL, 5 );
   //bSizer335->Add( m_staticText316, 0, wxALIGN_CENTER, 5 );
   bSizer335->Add (m_staticText316, wxSizerFlags(0).Align(wxALIGN_CENTER));
	
	m_radioBtn37 = new wxRadioButton (this,
													IDC_SelectedClassRadio,
													wxT("Class"),
													wxDefaultPosition,
													wxDefaultSize,
													wxRB_GROUP);
   SetUpToolTip(m_radioBtn37, IDS_ToolTip226);
	//bSizer335->Add( m_radioBtn37, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL, 5 );
   //bSizer335->Add( m_radioBtn37, 0, wxALIGN_CENTER, 5 );
   bSizer335->Add (m_radioBtn37, wxSizerFlags(0).Align(wxALIGN_CENTER));
	
	m_radioBtn38 = new wxRadioButton (this,
													IDC_SelectedFieldRadio,
													wxT("Field"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip(m_radioBtn38, IDS_ToolTip227);
	//bSizer335->Add( m_radioBtn38, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL, 5 );
	//bSizer335->Add( m_radioBtn38, 0, wxALIGN_CENTER, 5 );
   bSizer335->Add (m_radioBtn38, wxSizerFlags(0).Align(wxALIGN_CENTER));
	
	bSizer333->Add( bSizer335, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	wxBoxSizer* bSizer336;
	bSizer336 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer336->Add( 18, 0, 0, wxEXPAND, 5 );
	
	m_radioBtn39 = new wxRadioButton (this,
													IDC_SelectedAreaRadio,
													wxT("Selected Area"),
													wxDefaultPosition,
													wxDefaultSize,
													wxRB_SINGLE);
   SetUpToolTip(m_radioBtn39, IDS_ToolTip228);
	//bSizer336->Add( m_radioBtn39, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL, 5 );
   //bSizer336->Add( m_radioBtn39, 0, wxALIGN_CENTER, 5 );
   bSizer336->Add (m_radioBtn39, wxSizerFlags(0).Align(wxALIGN_CENTER));
	
	bSizer333->Add( bSizer336, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );
	
	bSizer292->Add( bSizer333, 0, wxEXPAND|wxLEFT|wxRIGHT, 12 );
	
	wxBoxSizer* bSizer339;
	bSizer339 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer8;
	wxStaticBox* areaForImageStatistics = new wxStaticBox (this, IDC_LineColFrame, wxT("Area for Image Statistics"));
	sbSizer8 = new wxStaticBoxSizer (areaForImageStatistics, wxHORIZONTAL);
	
	CreateLineColumnControls (sbSizer8);

	bSizer339->Add( sbSizer8, 0, wxALL|wxEXPAND, 12 );
   
   //sbSizer8->GetStaticBox()->Hide();
	
	wxBoxSizer* bSizer340;
	bSizer340 = new wxBoxSizer( wxVERTICAL );
	
	bSizer340->Add( 0, 50, 0, wxEXPAND, 5 );
	
	m_staticText319 = new wxStaticText( this, IDC_StatPrompt, wxT("The statistics"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText319->Wrap( -1 );
	bSizer340->Add( m_staticText319, 0, wxALIGN_CENTER, 5 );
   m_staticText319->Hide();
	
	m_staticText320 = new wxStaticText( this, IDC_OptionPrompt, wxT("Options"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText320->Wrap( -1 );
	m_staticText320->Hide();
	//bSizer340->Add( m_staticText320, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
   //bSizer340->Add( m_staticText320, 0, wxALIGN_CENTER, 5 );
   bSizer340->Add (m_staticText320, wxSizerFlags(0).Align(wxALIGN_CENTER));
	
	bSizer339->Add( bSizer340, 0, wxEXPAND, 5 );
	
	bSizer292->Add( bSizer339, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer293;
	bSizer293 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText283 = new wxStaticText( this, wxID_ANY, wxT("Base minimum/maximum settings for each image on"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText283->Wrap( -1 );
	bSizer293->Add( m_staticText283, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer307;
	bSizer307 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer308;
	bSizer308 = new wxBoxSizer( wxVERTICAL );
	
	m_radioBtn40 = new wxRadioButton (this,
													IDC_OverallRadio,
													wxT("Overall values - same for all images"),
													wxDefaultPosition,
													wxDefaultSize,
													wxRB_GROUP);
   SetUpToolTip(m_radioBtn40, IDS_ToolTip229);
	bSizer308->Add( m_radioBtn40, 0, wxLEFT, 5 );
	
	m_radioBtn41 = new wxRadioButton (this,
													IDC_IndividualRadio,
													wxT("Individual values - each image differs"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip(m_radioBtn41, IDS_ToolTip230);
	bSizer308->Add( m_radioBtn41, 0, wxLEFT|wxTOP, 5 );
	
	bSizer307->Add( bSizer308, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer337;
	bSizer337 = new wxBoxSizer( wxHORIZONTAL );
	
	m_radioBtn42 = new wxRadioButton (this,
													IDC_UserSettingRadio,
													wxT("User settings"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip(m_radioBtn42, IDS_ToolTip231);
	bSizer337->Add( m_radioBtn42, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText317 = new wxStaticText( this, IDC_MinPrompt, wxT("-min:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText317->Wrap( -1 );
	bSizer337->Add( m_staticText317, 0, wxALIGN_CENTER, 5 );
	
	m_textCtrl154 = new wxTextCtrl( this, IDC_StatisticMin, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer337->Add( m_textCtrl154, 0, wxALIGN_CENTER, 5 );
	
	bSizer337->Add( 10, 0, 0, wxEXPAND, 5 );
	
	m_staticText318 = new wxStaticText( this, IDC_MaxPrompt, wxT("max:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText318->Wrap( -1 );
	bSizer337->Add( m_staticText318, 0, wxALIGN_CENTER, 5 );
	
	m_textCtrl155 = new wxTextCtrl( this, IDC_StatisticMax, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer337->Add( m_textCtrl155, 0, wxALIGN_CENTER, 5 );
	
	bSizer307->Add( bSizer337, 0, 0, 1 );

	bSizer293->Add (bSizer307, 0, wxEXPAND, 5);
		
   bSizer292->Add (bSizer293, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	CreateStandardButtons (bSizer292);
	
	this->SetSizer (bSizer292);
	this->Layout ();
	this->Centre (wxBOTH);
	
}	// end "CreateControls"



Boolean CMStatImageDialog::DoDialog ()

{
   SInt16		returnCode = wxID_CANCEL;
	
	Boolean		continueFlag = FALSE;
	
   
   if (TransferDataFromWindow ())
		{
      returnCode = ShowModal ();
		
		}
   
   //if (gProjectInfoPtr != NULL) 
   //   m_areaCode = 0;
   
   if (returnCode == wxID_OK)
		{
      StatisticsImageDialogOK (this,
											gStatisticsImageSpecsPtr,
											&m_dialogSelectArea,
											(SInt16)m_channelSelection,
											m_featureTransformationFlag,
											(SInt16*)m_localActiveFeaturesPtr,
											m_localActiveNumberFeatures,
											m_maximumNumberChannels,
											(SInt16)m_classSelection,
											m_localNumberClasses,
											(SInt16*)m_classListPtr,
											m_areaCode,
											m_perClassFieldCode,
											m_overallMinMaxCode,
											m_individualMinMaxCode,
											m_userMinMaxCode,
											m_userMinimum,
											m_userMaximum);
      
		continueFlag = TRUE;
		
   	}
   
   return (continueFlag);
	
}		// end "DoDialog"


void CMStatImageDialog::OnInitDialog (wxInitDialogEvent& event)
{
	SInt16							channelSelection,
										classSelection,
										selectItem;
  
	
	StatisticsImageDialogInitialize (this,
												gStatisticsImageSpecsPtr,
												&m_dialogSelectArea,
												gImageWindowInfoPtr,
												gActiveImageWindow,
												&channelSelection,
												&m_localNumberFeatures,
												m_localFeaturesPtr,
												m_localTransformFeaturesPtr,
												&m_channelsAllAvailableFlag,
												(UInt16**)&m_channelsPtr,
												&m_maximumNumberChannels,
												&classSelection,
												&m_localNumberClasses,
												(UInt16*)m_classListPtr,
												&m_MinMaxCode,
												&m_userMinimum,
												&m_userMaximum,
												&m_areaCode,
												&m_perClassFieldCode,
												&selectItem,
												&m_featureTransformationFlag,
												&m_featureTransformAllowedFlag);
	
   m_LineStart = m_dialogSelectArea.lineStart;
   m_LineEnd = m_dialogSelectArea.lineEnd;
   m_LineInterval = m_dialogSelectArea.lineInterval;
   m_ColumnStart = m_dialogSelectArea.columnStart;
   m_ColumnEnd = m_dialogSelectArea.columnEnd;
   m_ColumnInterval = m_dialogSelectArea.columnInterval;

			// Set feature parameters

   m_localActiveNumberFeatures = m_localNumberFeatures;
   m_localActiveFeaturesPtr = m_localFeaturesPtr;
   m_channelSelection = channelSelection;
   m_classSelection = classSelection;
	/*
	if (m_areaCode == kAreaType)
		{
		wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
		l_start->SetFocus ();
		
		}	// end "if (m_areaCode == kAreaType)"
	
	else	// m_areaCode != kAreaType
		{
		wxTextCtrl* mintext = (wxTextCtrl*)FindWindow (IDC_StatisticMin);
		mintext->SetFocus ();
		
		}	// end "else m_areaCode != kAreaType"
	*/
   SelectDialogItemText (this, selectItem, 0, SInt16_MAX);
      
	if (TransferDataToWindow ())
		PositionDialogWindow ();
     
}	// end "OnInitDialog"


void CMStatImageDialog::OnClassComboSelendok(wxCommandEvent& event)
{                                                           
   HandleClassesMenu(&m_localNumberClasses,
            (SInt16*)m_classListPtr,
            (SInt16)1,
            (SInt16)gProjectInfoPtr->numberStatisticsClasses,
            IDC_ClassCombo,
            (int*)&m_classSelection);
	
}		// end "OnClassComboSelendok"


void CMStatImageDialog::OnChannelComboSelendok(wxCommandEvent& event)
{                                                           
	HandleChannelsMenu(IDC_ChannelCombo,
                     kNoTransformation,
                     (SInt16) gImageWindowInfoPtr->totalNumberChannels,
                     2,
                     TRUE);
                     //m_channelsAllAvailableFlag);
	
}	// end "OnChannelComboSelendok"


void CMStatImageDialog::OnClickUserSettingRadio(wxCommandEvent& event)
{
   ShowDialogItem (this, IDC_MinPrompt);
   ShowDialogItem (this, IDC_StatisticMin);
   ShowDialogItem (this, IDC_MaxPrompt);
   ShowDialogItem (this, IDC_StatisticMax);
   
}


void CMStatImageDialog::OnClickIndividualRadio (wxCommandEvent& event)

{
   HideDialogItem (this, IDC_MinPrompt);
   HideDialogItem (this, IDC_StatisticMin);
   HideDialogItem (this, IDC_MaxPrompt);
   HideDialogItem (this, IDC_StatisticMax);  
}


void CMStatImageDialog::OnClickOverallRadio (wxCommandEvent& event){
   HideDialogItem (this, IDC_MinPrompt);
   HideDialogItem (this, IDC_StatisticMin);
   HideDialogItem (this, IDC_MaxPrompt);
   HideDialogItem (this, IDC_StatisticMax);  
}

/*
void CMStatImageDialog::ToStatEntireImage(wxCommandEvent& event)
{
	CMDialog::ToEntireImage(event);
	
	m_dialogSelectArea.lineStart = m_LineStart;
	m_dialogSelectArea.lineEnd = m_LineEnd;
	m_dialogSelectArea.columnStart = m_ColumnStart;
	m_dialogSelectArea.columnEnd = m_ColumnEnd;
	UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
	
}		// end "ToEntireImage"


void  CMStatImageDialog::ToStatSelectedImage(wxCommandEvent& event)
{
	CMDialog::ToSelectedImage(event);
	
	m_dialogSelectArea.lineStart = m_LineStart;
	m_dialogSelectArea.lineEnd = m_LineEnd;
	m_dialogSelectArea.columnStart = m_ColumnStart;
	m_dialogSelectArea.columnEnd = m_ColumnEnd;
	UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
	
}		// end "ToSelectedImage"
*/


bool CMStatImageDialog::TransferDataFromWindow ()

{
   SInt16			returnCode = 0;
   
   wxTextCtrl* c_end = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
	wxTextCtrl* c_inter = (wxTextCtrl*) FindWindow(IDC_ColumnInterval);
	wxTextCtrl* c_start = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
	wxTextCtrl* l_end = (wxTextCtrl*) FindWindow(IDC_LineEnd);
	wxTextCtrl* l_inter = (wxTextCtrl*) FindWindow(IDC_LineInterval);
	wxTextCtrl* l_start = (wxTextCtrl*) FindWindow(IDC_LineStart);
   wxTextCtrl* mintext = (wxTextCtrl*) FindWindow(IDC_StatisticMin);
   wxTextCtrl* maxtext = (wxTextCtrl*) FindWindow(IDC_StatisticMax);
   
   wxCheckBox* featureTrans = (wxCheckBox*)FindWindow(IDC_FeatureTransformation);
   
   wxRadioButton* classesbut = (wxRadioButton*)FindWindow(IDC_ClassesRadio);
   wxRadioButton* seleclassbut = (wxRadioButton*)FindWindow(IDC_SelectedClassRadio);
   wxRadioButton* selecfieldsbut = (wxRadioButton*)FindWindow(IDC_SelectedFieldRadio);
   wxRadioButton* selecareabut = (wxRadioButton*)FindWindow(IDC_SelectedAreaRadio);
   wxRadioButton* overallbut = (wxRadioButton*)FindWindow(IDC_OverallRadio);
   wxRadioButton* idvbut = (wxRadioButton*)FindWindow(IDC_IndividualRadio);
   wxRadioButton* userbut = (wxRadioButton*)FindWindow(IDC_UserSettingRadio);
   
   wxComboBox* channelscb = (wxComboBox *) FindWindow(IDC_ChannelCombo);
   wxComboBox* classescb = (wxComboBox *) FindWindow(IDC_ClassCombo);
   wxComboBox* fieldscb = (wxComboBox *) FindWindow(IDC_Fields);
   
   m_channelSelection = channelscb->GetSelection();
   //if(m_channelSelection < 0)
   //   m_channelSelection = m_channelSelection_Saved;
   
   m_classSelection = classescb->GetSelection();
   //if(m_classSelection < 0)
   //   m_classSelection = m_classSelection_Saved;
   
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
   
   m_featureTransformationFlag = featureTrans->GetValue();
	
   m_areaCode = kTrainingType;
   if (selecareabut->GetValue ())
   	m_areaCode = kAreaType;
	
   if (seleclassbut->GetValue())
		m_perClassFieldCode = 1;
   else	// seleclassbut->GetValue() == 0
		m_perClassFieldCode = 2;
	
   m_overallMinMaxCode = overallbut->GetValue ();
   m_individualMinMaxCode = idvbut->GetValue ();
   m_userMinMaxCode = userbut->GetValue ();
   
   wxString UserSettingMinString = mintext->GetValue();
   wxString UserSettingMaxString = maxtext->GetValue();
   
   m_userMinimum = wxAtof(UserSettingMinString);
   m_userMaximum = wxAtof(UserSettingMaxString);
   
   return (returnCode == 0);
   
}	// end "TransferDataFromWindow"



bool CMStatImageDialog::TransferDataToWindow ()

{
   SInt16			returnCode = 0;
	
   
   wxTextCtrl* c_end = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
	wxTextCtrl* c_inter = (wxTextCtrl*) FindWindow(IDC_ColumnInterval);
	wxTextCtrl* c_start = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
	wxTextCtrl* l_end = (wxTextCtrl*) FindWindow(IDC_LineEnd);
	wxTextCtrl* l_inter = (wxTextCtrl*) FindWindow(IDC_LineInterval);
	wxTextCtrl* l_start = (wxTextCtrl*) FindWindow(IDC_LineStart);
   wxTextCtrl* mintext = (wxTextCtrl*) FindWindow(IDC_StatisticMin);
   wxTextCtrl* maxtext = (wxTextCtrl*) FindWindow(IDC_StatisticMax);
   
   wxCheckBox* featureTrans = (wxCheckBox*)FindWindow(IDC_FeatureTransformation);
   
   wxRadioButton* classesbut = (wxRadioButton*)FindWindow(IDC_ClassesRadio);
   wxRadioButton* seleclassbut = (wxRadioButton*)FindWindow(IDC_SelectedClassRadio);
   wxRadioButton* selecfieldsbut = (wxRadioButton*)FindWindow(IDC_SelectedFieldRadio);
   wxRadioButton* selecareabut = (wxRadioButton*)FindWindow(IDC_SelectedAreaRadio);
   wxRadioButton* overallbut = (wxRadioButton*)FindWindow(IDC_OverallRadio);
   wxRadioButton* idvbut = (wxRadioButton*)FindWindow(IDC_IndividualRadio);
   wxRadioButton* userbut = (wxRadioButton*)FindWindow(IDC_UserSettingRadio);
   
   wxComboBox* channelscb = (wxComboBox *) FindWindow(IDC_ChannelCombo);
   wxComboBox* classescb = (wxComboBox *) FindWindow(IDC_ClassCombo);
   wxComboBox* fieldscb = (wxComboBox *) FindWindow(IDC_Fields);
   
   featureTrans->SetValue(m_featureTransformationFlag);
   
   c_end->ChangeValue(wxString::Format(wxT("%d"), m_ColumnEnd));
	c_inter->ChangeValue(wxString::Format(wxT("%d"), m_ColumnInterval));
	c_start->ChangeValue(wxString::Format(wxT("%d"), m_ColumnStart));
	l_end->ChangeValue(wxString::Format(wxT("%d"), m_LineEnd));
	l_inter->ChangeValue(wxString::Format(wxT("%d"), m_LineInterval));
	l_start->ChangeValue(wxString::Format(wxT("%d"), m_LineStart));
   
   //channelscb->SetSelection(m_channelSelection);
   channelscb->SetSelection(m_channelSelection);
   classescb->SetSelection(m_classSelection);
   
   classesbut->SetValue (m_areaCode == kTrainingType);
   seleclassbut->SetValue (m_perClassFieldCode == 1);
   selecfieldsbut->SetValue (m_perClassFieldCode == 2);
   selecareabut->SetValue (m_areaCode == kAreaType);
   overallbut->SetValue (m_MinMaxCode == 1);
   idvbut->SetValue (m_MinMaxCode == 2);
   userbut->SetValue (m_MinMaxCode == 3);
   
   mintext->ChangeValue(wxString::Format(wxT("%.2lf"), m_userMinimum));
   maxtext->ChangeValue(wxString::Format(wxT("%.2lf"), m_userMaximum));
   
   return (returnCode == 0);
	
}	// end "TransferDataToWindow"
