//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LStatisticsImageDialog.cpp : class implementation file
//	Class Definition:		LStatisticsImageDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			11/16/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMStatImageDialog class.
//
//------------------------------------------------------------------------------------

#include "LStatisticsImageDialog.h"
#include "LImage_dialog.cpp"

extern void			StatisticsImageDialogInitialize (
							DialogPtr							dialogPtr,
							StatisticsImageSpecsPtr			statisticsImageSpecsPtr,
							DialogSelectArea*					dialogSelectAreaPtr,
							WindowInfoPtr						imageWindowInfoPtr,
							WindowPtr							activeImageWindow,
							SInt16*								channelSelectionPtr,
							UInt16*								localNumberChannelsPtr,
							UInt16*								localFeaturesPtr, 
							UInt16*								localTransformFeaturesPtr,
							Boolean*								channelsAllAvailableFlagPtr,
							UInt16**								projectChannelsPtrPtr,
							SInt16*								maxNumberChannelsPtr,
							SInt16*								classSelectionPtr,
							UInt32*								localNumberClassesPtr,
							UInt16*								localClassPtr,
							SInt16*								minMaxSettingCodePtr,
							double*								userMinimumPtr,
							double*								userMaximumPtr,
							SInt16*								areaCodePtr,
							SInt16*								selectItemPtr,
							Boolean*								featureTransformationFlagPtr,
							Boolean*								featureTransformAllowedFlagPtr);

extern void			StatisticsImageDialogOK (
							DialogPtr							dialogPtr,
                     StatisticsImageSpecsPtr			statisticsImageSpecsPtr,
                     DialogSelectArea*					dialogSelectAreaPtr,
                     SInt16								classCode,
                     SInt16								areaCode,
                     SInt16								channelSelection,
                     Boolean								featureTransformationFlag,
                     SInt16*								featurePtr,
                     SInt16                        localNumberFeatures,
							SInt16								maxNumberChannels,
                     SInt16								classSelection,
                     SInt32                        localNumberClasses,
                     SInt16*								classListPtr,
                     SInt16								perClassCode,
                     SInt16								perFieldCode,
                     SInt16								overallMinMaxCode,
                     SInt16								individualMinMaxCode,
                     SInt16								userMinMaxCode,
                     double								userMinimum,
                     double								userMaximum);


CMStatImageDialog::CMStatImageDialog(wxWindow* parent, wxWindowID id, const wxString& title)
: CMDialog(CMStatImageDialog::IDD, parent, title) 
{
   UInt16**					classPtrPtr;
   
   m_classPtr = NULL;
   m_featurePtr = NULL;
   m_channelsPtr = NULL;
   
   m_channelSelection = 0;
   m_featureTransformationFlag = FALSE;
   
   m_classCode = 0;
   
	m_perClassCode = 0;
	m_perFieldCode = 0;
   
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
   this->SetSizerAndFit(bSizer292);
}


CMStatImageDialog::~CMStatImageDialog()
{
   if (m_classListPtr != NULL)
		{
      ReleaseDialogLocalVectors (m_localFeaturesPtr,
											m_localTransformFeaturesPtr,
											m_classListPtr,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
		}
}


BEGIN_EVENT_TABLE(CMStatImageDialog, CMDialog)
EVT_INIT_DIALOG(CMStatImageDialog::OnInitDialog)
EVT_COMBOBOX(IDC_ClassCombo, CMStatImageDialog::OnSelendokClassCombo)
EVT_COMBOBOX_DROPDOWN(IDC_ClassCombo, CMStatImageDialog::OnSelendokClassComboDropDown)
EVT_COMBOBOX(IDC_ChannelCombo, CMStatImageDialog::OnSelendokChannelCombo)
EVT_COMBOBOX_DROPDOWN(IDC_ChannelCombo, CMStatImageDialog::OnSelendokChannelComboDropDown)
EVT_RADIOBUTTON(IDC_UserSettingRadio, CMStatImageDialog::OnClickUserSettingRadio)
EVT_RADIOBUTTON(IDC_IndividualRadio, CMStatImageDialog::OnClickIndividualRadio)
EVT_RADIOBUTTON(IDC_OverallRadio, CMStatImageDialog::OnClickOverallRadio)
EVT_BUTTON(IDEntireImage, CMStatImageDialog::ToEntireImage)
EVT_BUTTON(IDSelectedImage, CMStatImageDialog::ToSelectedImage)
EVT_TEXT(IDC_ColumnEnd, CMStatImageDialog::CheckColumnEnd)
EVT_TEXT(IDC_ColumnStart, CMStatImageDialog::CheckColumnStart)
EVT_TEXT(IDC_LineEnd, CMStatImageDialog::CheckLineEnd)
EVT_TEXT(IDC_LineStart, CMStatImageDialog::CheckLineStart)
EVT_TEXT(IDC_LineInterval, CMStatImageDialog::CheckLineInterval)
EVT_TEXT(IDC_ColumnInterval, CMStatImageDialog::CheckColumnInterval)
END_EVENT_TABLE()

/*
void  CMStatImageDialog::CheckStatColumnEnd(wxCommandEvent& event)
{
	if (!m_settingSelectedEntireButton)
	{
		CMDialog::CheckColumnEnd(event);
		
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		//UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
		
	}		// end "if (!m_settingSelectedEntireButton)"
	
}		// end "CheckColumnEnd"


void CMStatImageDialog::CheckStatColumnInterval(wxCommandEvent& event)
{
	CMDialog::CheckColumnInterval(event);
	
	m_dialogSelectArea.columnInterval = m_ColumnInterval;
	//UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
	
}		// end "CheckStatColumnInterval"


void  CMStatImageDialog::CheckStatColumnStart(wxCommandEvent& event)
{
	if (!m_settingSelectedEntireButton)
	{
		CMDialog::CheckColumnStart(event);
		
		m_dialogSelectArea.columnStart = m_ColumnStart;
		//UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
		
	}		// end "if (!m_settingSelectedEntireButton)"
	
}		// end "CheckStatColumnStart"


void CMStatImageDialog::CheckStatLineEnd(wxCommandEvent& event)
{
	if (!m_settingSelectedEntireButton)
	{
		CMDialog::CheckLineEnd(event);
		
		m_dialogSelectArea.lineEnd = m_LineEnd;
		//UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
		
	}		// end "if (!m_settingSelectedEntireButton)"
	
}		// end "CheckStatLineEnd"


void CMStatImageDialog::CheckStatLineInterval(wxCommandEvent& event)
{
	CMDialog::CheckLineInterval(event);
	
	m_dialogSelectArea.lineInterval = m_LineInterval;
	//UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
	
}		// end "CheckStatLineInterval"


void CMStatImageDialog::CheckStatLineStart(wxCommandEvent& event)
{
	if (!m_settingSelectedEntireButton)
	{
		CMDialog::CheckLineStart(event);
		
		m_dialogSelectArea.lineStart = m_LineStart;
		//UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
		
	}		// end "if (!m_settingSelectedEntireButton)"
	
}		// end "CheckLineStart"
*/

void CMStatImageDialog::CreateControls ()

{
	/*
   wxBitmap entireimi = wxBITMAP_PNG_FROM_DATA(entireim);
   wxBitmap toentirei = wxBITMAP_PNG_FROM_DATA(toentire);
   wxBitmap selectedi = wxBITMAP_PNG_FROM_DATA(selected);
   wxBitmap bmp4i = wxBITMAP_PNG_FROM_DATA(bmp4);
   */
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
	
	m_radioBtn36 = new wxRadioButton( this, IDC_ClassesRadio, wxT("Classes:"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
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
	
	m_radioBtn37 = new wxRadioButton( this, IDC_SelectedClassRadio, wxT("Class"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
   SetUpToolTip(m_radioBtn37, IDS_ToolTip226);
	//bSizer335->Add( m_radioBtn37, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL, 5 );
   //bSizer335->Add( m_radioBtn37, 0, wxALIGN_CENTER, 5 );
   bSizer335->Add (m_radioBtn37, wxSizerFlags(0).Align(wxALIGN_CENTER));
	
	m_radioBtn38 = new wxRadioButton( this, IDC_SelectedFieldRadio, wxT("Field"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_radioBtn38, IDS_ToolTip227);
	//bSizer335->Add( m_radioBtn38, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL, 5 );
	//bSizer335->Add( m_radioBtn38, 0, wxALIGN_CENTER, 5 );
   bSizer335->Add (m_radioBtn38, wxSizerFlags(0).Align(wxALIGN_CENTER));
	
	bSizer333->Add( bSizer335, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	wxBoxSizer* bSizer336;
	bSizer336 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer336->Add( 18, 0, 0, wxEXPAND, 5 );
	
	m_radioBtn39 = new wxRadioButton( this, IDC_SelectedAreaRadio, wxT("Selected Area"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
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
	/*
	wxBoxSizer* bSizer791;
	bSizer791 = new wxBoxSizer(wxHORIZONTAL);
	
	m_bpButton4 = new wxBitmapButton (areaForImageStatistics, IDEntireImage, entireimi, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
   m_bpButton4->SetBitmapDisabled(toentirei);
	//bSizer791->Add (m_bpButton4, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
   //bSizer791->Add (m_bpButton4, 0, wxALL, 5);
   bSizer791->Add (m_bpButton4,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
	
	m_bpButton5 = new wxBitmapButton (areaForImageStatistics, IDSelectedImage, selectedi, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_bpButton5->SetBitmapDisabled(bmp4i);
	m_bpButton5->Hide();
	//bSizer791->Add( m_bpButton5, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
   //bSizer791->Add( m_bpButton5, 0, wxALL, 5 );
 	bSizer791->Add (m_bpButton5,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
	
	sbSizer8->Add( bSizer791, 0, wxEXPAND|wxALL, 5 );
   //bSizer339->Add( bSizer791, 0, wxEXPAND, 12 );
	*/
	//wxBoxSizer* bSizer121;
	//bSizer121 = new wxBoxSizer( wxVERTICAL );
	
	//m_staticText177 = new wxStaticText( this, IDC_StartEndInterval, wxT("                      Start       \tEnd          Interval"), wxDefaultPosition, wxDefaultSize, 0 );
	//m_staticText177->Wrap( -1 );
	//bSizer121->Add( m_staticText177, 0, wxALL, 12 );
	
	CreateLineColumnControls (sbSizer8);
	/*
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer (3, 4, 0, 0);
	
	wxStaticText* m_staticText63 = new wxStaticText(areaForImageStatistics, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   m_staticText63->Wrap(-1);
   gSizer1->Add(m_staticText63, 0, wxALL, 5);

   wxStaticText* m_staticText64 = new wxStaticText(areaForImageStatistics, IDC_StartPrompt, wxT("Start"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText64->Wrap(-1);
   gSizer1->Add(m_staticText64, 0, wxALIGN_CENTER | wxALL, 5);

   wxStaticText* m_staticText65 = new wxStaticText(areaForImageStatistics, IDC_EndPrompt, wxT("End"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText65->Wrap(-1);
   gSizer1->Add(m_staticText65, 0, wxALIGN_CENTER | wxALL, 5);

   wxStaticText* m_staticText66 = new wxStaticText(areaForImageStatistics, IDC_IntervalPrompt, wxT("Interval"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText66->Wrap(-1);
   gSizer1->Add(m_staticText66, 0, wxALIGN_CENTER | wxALL, 5);

	m_staticText60 = new wxStaticText (areaForImageStatistics, IDC_LinePrompt, wxT("Lines"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText60->Wrap( -1 );
	gSizer1->Add( m_staticText60, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_linestartctrl = new wxTextCtrl (areaForImageStatistics, IDC_LineStart, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), 0 );
	m_linestartctrl->SetMaxLength( 0 ); 
	m_linestartctrl->SetValidator( wxTextValidator( wxFILTER_NUMERIC, &m_LineStartString ) );
	
	gSizer1->Add( m_linestartctrl, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_lineendctrl = new wxTextCtrl (areaForImageStatistics, IDC_LineEnd, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), 0 );
	m_lineendctrl->SetMaxLength( 0 ); 
	m_lineendctrl->SetValidator( wxTextValidator( wxFILTER_NUMERIC, &m_LineEndString ) );
	
	gSizer1->Add( m_lineendctrl, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_lineintctrl = new wxTextCtrl (areaForImageStatistics, IDC_LineInterval, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), 0 );
	m_lineintctrl->SetMaxLength( 0 ); 
	m_lineintctrl->SetValidator( wxTextValidator( wxFILTER_NUMERIC, &m_LineIntervalString ) );
	
	gSizer1->Add( m_lineintctrl, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_staticText62 = new wxStaticText (areaForImageStatistics, IDC_ColumnPrompt, wxT("Columns"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText62->Wrap( -1 );
	gSizer1->Add( m_staticText62, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_colstartctrl = new wxTextCtrl (areaForImageStatistics, IDC_ColumnStart, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), 0 );
	m_colstartctrl->SetMaxLength( 0 ); 
	m_colstartctrl->SetValidator( wxTextValidator( wxFILTER_NUMERIC, &m_ColumnStartString ) );
	
	gSizer1->Add( m_colstartctrl, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_colendctrl = new wxTextCtrl (areaForImageStatistics, IDC_ColumnEnd, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), 0 );
	m_colendctrl->SetMaxLength( 0 ); 
	m_colendctrl->SetValidator( wxTextValidator( wxFILTER_NUMERIC, &m_ColumnEndString ) );
	
	gSizer1->Add( m_colendctrl, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_colintctrl = new wxTextCtrl (areaForImageStatistics, IDC_ColumnInterval, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), 0 );
	m_colintctrl->SetMaxLength( 0 ); 
	m_colintctrl->SetValidator( wxTextValidator( wxFILTER_NUMERIC, &m_ColumnIntervalString ) );
	
   SetUpToolTip(m_bpButton4, IDS_ToolTip40);  
   SetUpToolTip(m_linestartctrl, IDS_ToolTip19);
   SetUpToolTip(m_lineendctrl, IDS_ToolTip20);
   SetUpToolTip(m_lineintctrl, IDS_ToolTip21);
   SetUpToolTip(m_colstartctrl, IDS_ToolTip22);
   SetUpToolTip(m_colendctrl, IDS_ToolTip23);
   SetUpToolTip(m_colintctrl, IDS_ToolTip24);
   
	gSizer1->Add( m_colintctrl, 0, wxALIGN_CENTER|wxALL, 2 );
	
	//bSizer121->Add( gSizer1, 0, wxEXPAND, 12 );
	
	//bSizer339->Add( bSizer121, 0, wxEXPAND, 12 );
   
	sbSizer8->Add (gSizer1, 0, wxEXPAND, 5 );
	*/
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
	
	m_radioBtn40 = new wxRadioButton( this, IDC_OverallRadio, wxT("Overall values - same for all images"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
   SetUpToolTip(m_radioBtn40, IDS_ToolTip229);
	bSizer308->Add( m_radioBtn40, 0, wxLEFT, 5 );
	
	m_radioBtn41 = new wxRadioButton( this, IDC_IndividualRadio, wxT("Individual values - each image differs"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_radioBtn41, IDS_ToolTip230);
	bSizer308->Add( m_radioBtn41, 0, wxLEFT|wxTOP, 5 );
	
	bSizer307->Add( bSizer308, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer337;
	bSizer337 = new wxBoxSizer( wxHORIZONTAL );
	
	m_radioBtn42 = new wxRadioButton( this, IDC_UserSettingRadio, wxT("User settings"), wxDefaultPosition, wxDefaultSize, 0 );
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
	/*
	wxBoxSizer* bSizer338;
	bSizer338 = new wxBoxSizer( wxHORIZONTAL );
	//bSizer338->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button81 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	//bSizer338->Add (m_button81, 0, wxALIGN_BOTTOM|wxALIGN_RIGHT, 5 );
	bSizer338->Add (m_button81, wxSizerFlags(0).Border(wxRIGHT,6));
	
	m_button82 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	//bSizer338->Add( m_button82, 0, wxALIGN_BOTTOM|wxALIGN_RIGHT, 5 );
	bSizer338->Add (m_button82, wxSizerFlags(0));
	
	
	//bSizer307->Add( bSizer338, 0, wxEXPAND|wxTOP, 12 );
	//bSizer307->Add (bSizer338, wxSizerFlags(0).Bottom().Right().Border(wxTOP,1));
   bSizer307->Add (bSizer338, wxSizerFlags(0).Right().Border(wxTOP, 1));
	*/
	bSizer293->Add (bSizer307, 0, wxEXPAND, 5);
		
	//bSizer292->Add (bSizer293, 0, wxEXPAND|wxLEFT|wxRIGHT|wxBOTTOM, 12);
   //bSizer292->Add (bSizer293, 0, wxEXPAND|wxRIGHT, 12);
   bSizer292->Add (bSizer293, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	CreateStandardButtons (bSizer292);
	
	this->SetSizer (bSizer292);
	this->Layout ();
	this->Centre (wxBOTH);
	
}		// end "CreateControls"



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
      StatisticsImageDialogOK (
							this,
							gStatisticsImageSpecsPtr,
							&m_dialogSelectArea,
							m_classCode,
							m_areaCode,
							m_channelSelection,
							m_featureTransformationFlag,
							(SInt16*)m_localActiveFeaturesPtr,
                     m_localActiveNumberFeatures,
							m_maximumNumberChannels,
							m_classSelection,
                     m_localNumberClasses,
							(SInt16*)m_classListPtr,
							m_perClassCode,
							m_perFieldCode,
							m_overallMinMaxCode,
							m_individualMinMaxCode,
							m_userMinMaxCode,
							m_userMinimum,
							m_userMaximum);
      
		continueFlag = TRUE;
		
   	}
   
   return (continueFlag);
	
}		// end "DoDialog"


void CMStatImageDialog::OnInitDialog(wxInitDialogEvent& event) 
{
	SInt16							areaCode,
										channelsPopUpMenuID,
										channelSelection,
										//itemHit,
		//								maxNumberChannels,
										//minMaxSettingCode,
										//numberClasses,
										selectItem;
										//theType;
   
	//SInt16								eigenSource,
	//									entireIconItem;
									
	//UInt16								numberEigenvectors;
	
	//Boolean							channelsAllAvailableFlag;
  
	
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
													&m_classSelection,
													&m_localNumberClasses,
													(UInt16*)m_classListPtr,
													&m_MinMaxCode,
													&m_userMinimum,
													&m_userMaximum,
													&m_areaCode,
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
   
/*
      numberClasses = gStatisticsImageSpecsPtr->numberClasses;
      
      m_classPtr = (SInt16*)GetHandlePointer(
										gStatisticsImageSpecsPtr->classHandle,
										kNoLock,
										kNoMoveHi);
                              
      m_featurePtr = (SInt16*)GetHandlePointer(
										gStatisticsImageSpecsPtr->featureHandle,
										kNoLock,
										kNoMoveHi);
      
      m_channelsPtr = (SInt16*)GetHandlePointer(
										gStatisticsImageSpecsPtr->channelsHandle,
										kNoLock,
										kNoMoveHi);	
      
      gChannelSelection = gStatisticsImageSpecsPtr->channelSet;

      if (gProjectInfoPtr != NULL && 
				gProjectInfoPtr->numberStatTrainClasses > 0 &&
					gProjectInfoPtr->statisticsCode == kMeanCovariance)
		{
         m_channelsAllAvailableFlag = TRUE;
         
         SetDLogControl (this, IDC_ClassesRadio, (gStatisticsImageSpecsPtr->areaCode == 1));
         
         if(gProjectInfoPtr->keepClassStatsOnlyFlag)
            MDisableDialogItem(this, IDC_SelectedFieldRadio);
         
         if (gStatisticsImageSpecsPtr->areaCode != 1)
            HideDialogItem (this, IDC_ClassesRadio);
         
         if (gStatisticsImageSpecsPtr->numberClasses > 0)
				{
				}
			else
				{
				HideDialogItem (this, IDC_ClassCombo);
				}
         
         //gClassSelection = gStatisticsImageSpecsPtr->classSet;
         m_classSelection = gStatisticsImageSpecsPtr->classSet;
         
         m_localNumberClasses = gStatisticsImageSpecsPtr->numberClasses;
         
         if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)
				{
            ShowDialogItem (this, IDC_ClassCombo);
            ShowDialogItem (this, IDC_StatPrompt);
            ShowDialogItem (this, IDC_SelectedClassRadio);
            ShowDialogItem (this, IDC_SelectedFieldRadio);
          
				}		// end "if (statisticsImageSpecsPtr->areaCode == kTrainingType)"
		
         else		// statisticsImageSpecsPtr->areaCode != kTrainingType"
				{
            HideDialogItem (this, IDC_ClassCombo);
            HideDialogItem (this, IDC_StatPrompt);
            HideDialogItem (this, IDC_SelectedClassRadio);
            HideDialogItem (this, IDC_SelectedFieldRadio);
			
				}		// end "else statisticsImageSpecsPtr->areaCode != kTrainingType"
         
					// Set control for creating one image for each class.
			
         
         SetDLogControl (this, IDC_SelectedClassRadio, 
									gStatisticsImageSpecsPtr->perFieldClassCode == 1);
         
         // Set control for creating one image for each field.						
				
         SetDLogControl (this, IDC_SelectedFieldRadio, 
									gStatisticsImageSpecsPtr->perFieldClassCode == 2);
		
         
         m_channelsPtr = (SInt16*)gProjectInfoPtr->channelsPtr;
    
         m_maximumNumberChannels = gImageWindowInfoPtr->totalNumberChannels;
         
            
      } // end "if (gProjectInfoPtr != NULL && ..."
      else		// gProjectInfoPtr == NULL || ...
		{
         m_channelsAllAvailableFlag = FALSE;
         
         HideDialogItem (this, IDC_StatPrompt);
         HideDialogItem (this, IDC_ClassesRadio);
         HideDialogItem (this, IDC_SelectedClassRadio);
         HideDialogItem (this, IDC_SelectedFieldRadio);
         HideDialogItem (this, IDC_ClassCombo);

         m_channelsPtr = NULL;
   
         m_maximumNumberChannels = gImageWindowInfoPtr->totalNumberChannels;
         //m_localActiveNumberFeatures = gImageWindowInfoPtr->totalNumberChannels;
      } // end "else gProjectInfoPtr == NULL || ..."
      
      
      //	Set the All/Subset channels/features list item

      m_channelSelection = gStatisticsImageSpecsPtr->channelSet;
      m_localNumberFeatures = gStatisticsImageSpecsPtr->numberChannels;

      SInt16* channelsPtr = (SInt16*) GetHandlePointer(
           gStatisticsImageSpecsPtr->featureHandle, kNoLock, kNoMoveHi);
      for (int index = 0; index < m_localNumberFeatures; index++)
         m_localFeaturesPtr[index] = channelsPtr[index];

      m_localActiveNumberFeatures = m_localNumberFeatures;
      m_localActiveFeaturesPtr = m_localFeaturesPtr;
      
      // Set control bullet for "Use overall min/max".				
      
      m_MinMaxCode = gStatisticsImageSpecsPtr->minMaxSettingCode;
      
      if(m_MinMaxCode == 1){
         m_overallMinMaxCode = 1;
         m_individualMinMaxCode = 0;
         m_userMinMaxCode = 0;
      }
      if(m_MinMaxCode == 2){
         m_overallMinMaxCode = 0;
         m_individualMinMaxCode = 1;
         m_userMinMaxCode = 0;
      }
      if(m_MinMaxCode == 3){
         m_overallMinMaxCode = 0;
         m_individualMinMaxCode = 0;
         m_userMinMaxCode = 1;
      }
      SetDLogControl (this, IDC_OverallRadio, 
								(gStatisticsImageSpecsPtr->minMaxSettingCode == 1));
      
      // Set control bullet for "Use individual min/max".						
			
      SetDLogControl (this, IDC_IndividualRadio, 
								(gStatisticsImageSpecsPtr->minMaxSettingCode == 2));
   
      // Set control bullet for "User defined min/max".						
			
      SetDLogControl (this, IDC_UserSettingRadio, 
								(gStatisticsImageSpecsPtr->minMaxSettingCode == 3));
      
      if (gStatisticsImageSpecsPtr->minMaxSettingCode == 3)
		{
         ShowDialogItem (this, IDC_MinPrompt);
         ShowDialogItem (this, IDC_StatisticMin);
         ShowDialogItem (this, IDC_MaxPrompt);
         ShowDialogItem (this, IDC_StatisticMax);
		
		}		// end "if (statisticsImageSpecsPtr->minMaxSettingCode == 3)"
      
      else		// statisticsImageSpecsPtr->minMaxSettingCode != 3
		{
         HideDialogItem (this, IDC_MinPrompt);
         HideDialogItem (this, IDC_StatisticMin);
         HideDialogItem (this, IDC_MaxPrompt);
         HideDialogItem (this, IDC_StatisticMax);
		
		}		// end "else statisticsImageSpecsPtr->minMaxSettingCode != 3"
      
      	// Set the current user specified min and max values.
      
      m_userMinimum = gStatisticsImageSpecsPtr->userMinimum;
      m_userMaximum = gStatisticsImageSpecsPtr->userMaximum;
      
      LoadDItemRealValue (this, IDC_StatisticMin, gStatisticsImageSpecsPtr->userMinimum, 1);
      LoadDItemRealValue (this, IDC_StatisticMax, gStatisticsImageSpecsPtr->userMaximum, 1);
      
      // To entire image icon.															
			//	Selected Statistics Image area												
			
      LoadLineColumnItems (&m_dialogSelectArea, this);
   
      // Set radio button for area.														
			
      SetDLogControl (this, IDC_SelectedAreaRadio, (gStatisticsImageSpecsPtr->areaCode == kAreaType));
   
      if (gStatisticsImageSpecsPtr->areaCode != kAreaType)
         HideDialogItem (this, IDC_SelectedAreaRadio);
      
   
      if (gStatisticsImageSpecsPtr->areaCode == kAreaType)
		{
         ShowDialogItem (this, IDC_StartEndInterval);
         ShowDialogItem (this, IDEntireImage);
         //ShowDialogItem (this, IDSelectedImage);
         ShowDialogItem (this, IDC_LinePrompt);
         ShowDialogItem (this, IDC_LineStart);
         ShowDialogItem (this, IDC_LineEnd);
         ShowDialogItem (this, IDC_LineInterval);
         ShowDialogItem (this, IDC_ColumnPrompt);
         ShowDialogItem (this, IDC_ColumnStart);
         ShowDialogItem (this, IDC_ColumnEnd);
         ShowDialogItem (this, IDC_ColumnInterval);
         
         printf("Image\n");
         wxTextCtrl* l_start = (wxTextCtrl*) FindWindow(IDC_LineStart);
         l_start->SetFocus();
		
		}		// end "if (statisticsImageSpecsPtr->areaCode == kAreaType)" 
      
      else		// statisticsImageSpecsPtr->areaCode != kAreaType 
		{
         HideDialogItem (this, IDC_StartEndInterval);
         HideDialogItem (this, IDEntireImage);
         //HideDialogItem (this, IDSelectedImage);
         HideDialogItem (this, IDC_LinePrompt);
         HideDialogItem (this, IDC_LineStart);
         HideDialogItem (this, IDC_LineEnd);
         HideDialogItem (this, IDC_LineInterval);
         HideDialogItem (this, IDC_ColumnPrompt);
         HideDialogItem (this, IDC_ColumnStart);
         HideDialogItem (this, IDC_ColumnEnd);
         HideDialogItem (this, IDC_ColumnInterval);
         
         
         wxTextCtrl* mintext = (wxTextCtrl*) FindWindow(IDC_StatisticMin);
         mintext->SetFocus();
		}		// end "else statisticsImageSpecsPtr->areaCode != kAreaType" 
      
      // Set check box for "Save image files to disk".							
	
			// Dialog items that are not currently used.
      
      EigenvectorInfoExists (kProject, &eigenSource, &numberEigenvectors);
      m_featureTransformAllowedFlag = (numberEigenvectors > 0);
      
      if (numberEigenvectors <= 0)
		{		
         m_featureTransformationFlag = FALSE;
         HideDialogItem (this, IDC_FeatureTransformation);
		
		}		// end "if (numberEigenvectors <= 0)" 
      
      else		// numberEigenvectors > 0 
		m_featureTransformationFlag = 
									gStatisticsImageSpecsPtr->featureTransformationFlag;
      
      CheckFeatureTransformationDialog (
							this, 
							m_featureTransformAllowedFlag,
							IDC_FeatureTransformation, 
							IDC_ChannelCombo, 
							&m_featureTransformationFlag);
	*/						
   SelectDialogItemText (this, selectItem, 0, SInt16_MAX);
      
	if (TransferDataToWindow())     
		PositionDialogWindow();     
     
}


void CMStatImageDialog::OnSelendokClassCombo(wxCommandEvent& event)
{                                                           
   HandleClassesMenu(&m_localNumberClasses,
            (SInt16*)m_classListPtr,
            (SInt16)1,
            (SInt16)gProjectInfoPtr->numberStatisticsClasses,
            IDC_ClassCombo,
            (int*)&m_classSelection);
	
}		// end "OnSelendokClassCombo"


void CMStatImageDialog::OnSelendokChannelCombo(wxCommandEvent& event)
{                                                           
	HandleChannelsMenu(IDC_ChannelCombo,
                     kNoTransformation,
                     (SInt16) gImageWindowInfoPtr->totalNumberChannels,
                     2,
                     TRUE);
                     //m_channelsAllAvailableFlag);
   
	
}		// end "OnSelendokChannelCombo"


void CMStatImageDialog::OnSelendokChannelComboDropDown(wxCommandEvent& event)
{
    wxComboBox* classcombo = (wxComboBox *) FindWindow(IDC_ChannelCombo);
    classcombo->SetSelection(-1); 
	    
}		// end "OnSelendokChannelComboDropDown"


void CMStatImageDialog::OnClickUserSettingRadio(wxCommandEvent& event)
{
   ShowDialogItem (this, IDC_MinPrompt);
   ShowDialogItem (this, IDC_StatisticMin);
   ShowDialogItem (this, IDC_MaxPrompt);
   ShowDialogItem (this, IDC_StatisticMax);
   
}


void CMStatImageDialog::OnClickIndividualRadio(wxCommandEvent& event){
   HideDialogItem (this, IDC_MinPrompt);
   HideDialogItem (this, IDC_StatisticMin);
   HideDialogItem (this, IDC_MaxPrompt);
   HideDialogItem (this, IDC_StatisticMax);  
}


void CMStatImageDialog::OnClickOverallRadio(wxCommandEvent& event){
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

bool CMStatImageDialog::TransferDataFromWindow() 
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
   if(m_channelSelection < 0)
      m_channelSelection = m_channelSelection_Saved;
   
   m_classSelection = classescb->GetSelection();
   if(m_classSelection < 0)
      m_classSelection = m_classSelection_Saved;
   
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
   m_classCode = classesbut->GetValue();
   m_perClassCode = seleclassbut->GetValue();
   m_perFieldCode = selecfieldsbut->GetValue();
   m_areaCode = selecareabut->GetValue();
   m_overallMinMaxCode = overallbut->GetValue();
   m_individualMinMaxCode = idvbut->GetValue();
   m_userMinMaxCode = userbut->GetValue();
   
   wxString UserSettingMinString = mintext->GetValue();
   wxString UserSettingMaxString = maxtext->GetValue();
   
   m_userMinimum = wxAtof(UserSettingMinString);
   m_userMaximum = wxAtof(UserSettingMaxString);
   
   return (returnCode == 0);
   
}		// end "TransferDataFromWindow"

bool CMStatImageDialog::TransferDataToWindow() 
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
   
   classesbut->SetValue(m_classCode);
   seleclassbut->SetValue(m_perClassCode);
   selecfieldsbut->SetValue(m_perFieldCode);
   selecareabut->SetValue(m_areaCode);
   overallbut->SetValue(m_MinMaxCode == 1);
   idvbut->SetValue(m_MinMaxCode == 2);
   userbut->SetValue(m_MinMaxCode == 3);
   
   mintext->ChangeValue(wxString::Format(wxT("%.2lf"), m_userMinimum));
   maxtext->ChangeValue(wxString::Format(wxT("%.2lf"), m_userMaximum));
   
   return (returnCode == 0);
	
}		// end "TransferDataToWindow"
