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
//	File:						xStatisticsHistogramDialog.cpp : class implementation file
//	Class Definition:		xStatisticsHistogramDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			02/24/2020
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMStatHistogramSpecsDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "xStatisticsHistogramDialog.h"



BEGIN_EVENT_TABLE (CMStatHistogramSpecsDlg, CMDialog)
	EVT_CHECKBOX (IDC_UseFeatureTransformation, CMStatHistogramSpecsDlg::OnFeatureTransformation)

	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_ChannelCombo, CMStatHistogramSpecsDlg::OnChannelComboSelendok)
	#endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_ChannelCombo, CMStatHistogramSpecsDlg::OnChannelComboSelendok)
	#endif

	EVT_COMBOBOX_CLOSEUP (IDC_ChannelCombo, CMStatHistogramSpecsDlg::OnChannelComboCloseUp)
	EVT_COMBOBOX_DROPDOWN (IDC_ChannelCombo, CMStatHistogramSpecsDlg::OnChannelComboDropDown)

	EVT_INIT_DIALOG (CMStatHistogramSpecsDlg::OnInitDialog)

	EVT_RADIOBUTTON (IDC_ClassesRadio, CMStatHistogramSpecsDlg::OnClassesRadio)
	EVT_RADIOBUTTON (IDC_ColumnRadio, CMStatHistogramSpecsDlg::OnListColumnFormatRadio)
	EVT_RADIOBUTTON (IDC_FieldsRadio, CMStatHistogramSpecsDlg::OnFieldsRadio)
	EVT_RADIOBUTTON (IDC_ListRadio, CMStatHistogramSpecsDlg::OnListRadio)
	EVT_RADIOBUTTON (IDC_MatrixRadio, CMStatHistogramSpecsDlg::OnListMatrixFormatRadio)
	EVT_RADIOBUTTON (IDC_PlotRadio, CMStatHistogramSpecsDlg::OnPlotRadio)

	EVT_TEXT (IDC_ColumnInterval, CMStatHistogramSpecsDlg::CheckColumnInterval)
	EVT_TEXT (IDC_LineInterval, CMStatHistogramSpecsDlg::CheckLineInterval)
END_EVENT_TABLE ()



CMStatHistogramSpecsDlg::CMStatHistogramSpecsDlg (
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title/*=NULL*/)
	: CMDialog (CMStatHistogramSpecsDlg::IDD, pParent, title)

{
   m_featureTransformationFlag = FALSE;
	m_includeEmptyBinsFlag = FALSE;
	m_blankValuesFlag = FALSE;
	m_overlayDensityFunctionFlag = FALSE;
	m_lineInterval = 1;
	m_columnInterval = 1;
   
	m_histogramClassFieldCode = kHistogramClass;
	
	m_listPlotCode = 0;
	
	m_groupByChannelsClassesCode = 0;
	
	m_matrixColumnCode = kListMatrixFormat;
	
   m_initializedFlag = CMDialog::m_initializedFlag;
   
   if (m_initializedFlag)
		m_initializedFlag = GetDialogLocalVectors (&m_localFeaturesPtr,
																	&m_localTransformFeaturesPtr,
																	NULL,
																	NULL,
																	NULL,
																	NULL,
																	NULL,
																	NULL);
   
   CreateControls ();
	
}	// end "CMStatHistogramSpecsDlg"



CMStatHistogramSpecsDlg::~CMStatHistogramSpecsDlg (void)

{
   ReleaseDialogLocalVectors (m_localFeaturesPtr,
											m_localTransformFeaturesPtr,        
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	
}	// end "~CMStatHistogramSpecsDlg"



void CMStatHistogramSpecsDlg::CreateControls ()

{
   SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxBoxSizer* bSizer107;
	bSizer107 = new wxBoxSizer (wxVERTICAL);
	
	wxGridSizer* gSizer23;
	gSizer23 = new wxGridSizer (0, 2, 0, 0);
	
	wxBoxSizer* bSizer287;
	bSizer287 = new wxBoxSizer (wxVERTICAL);
	
	
	m_classRadioCtrl = new wxRadioButton (this,
														IDC_ClassesRadio,
														wxT("Classes"),
														wxDefaultPosition,
														wxDefaultSize,
														wxRB_GROUP);
	bSizer287->Add (m_classRadioCtrl, 0, wxLEFT, 5);
	
	m_fieldRadioCtrl = new wxRadioButton (this,
														IDC_FieldsRadio,
														wxT("Fields"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	bSizer287->Add (m_fieldRadioCtrl, 0, wxLEFT, 5);
	
	m_checkBox66 = new wxCheckBox (this,
												IDC_UseFeatureTransformation,
												wxT("Use feature transformation"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer287->Add (m_checkBox66, 0, wxALIGN_CENTER|wxALL, 5);
	
	wxFlexGridSizer* fgSizer16;
	fgSizer16 = new wxFlexGridSizer (0, 2, 0, 0);
	fgSizer16->SetFlexibleDirection (wxBOTH);
	fgSizer16->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	m_staticText290 = new wxStaticText (this,
													IDC_ChannelsPrompt,
													wxT("Channels:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText290->Wrap (-1);
	fgSizer16->Add (m_staticText290, 0, wxALIGN_CENTER|wxALL, 5);

   GetAllSubsetMenuControl (kChannelsAvailableMenu,
										this,
										IDC_ChannelCombo,
										130,
										IDS_ToolTip52);
   fgSizer16->Add (m_channelsCtrl, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
	
	bSizer287->Add (fgSizer16, 1, wxEXPAND, 5);
	
	gSizer23->Add (bSizer287, 1, wxEXPAND|wxLEFT|wxTOP, 12);
	
	wxFlexGridSizer* fgSizer15;
	fgSizer15 = new wxFlexGridSizer (0, 2, 0, 0);
	fgSizer15->SetFlexibleDirection (wxBOTH);
	fgSizer15->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	m_staticText287 = new wxStaticText (this,
													wxID_ANY,
													wxT("Line interval"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText287->Wrap (-1);
	fgSizer15->Add (m_staticText287, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	m_textCtrl142 = new wxTextCtrl (this,
												IDC_LineInterval,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (70, -1),
												0);
   m_textCtrl142->SetValidator (
   								wxTextValidator (wxFILTER_DIGITS, &m_LineIntervalString));
	fgSizer15->Add (m_textCtrl142, 0, wxALL, 5);
	
	m_staticText288 = new wxStaticText (this,
													wxID_ANY,
													wxT("Column interval"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText288->Wrap (-1);
	fgSizer15->Add (m_staticText288, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	m_textCtrl143 = new wxTextCtrl (this,
												IDC_ColumnInterval,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (70, -1),
												0);
   m_textCtrl143->SetValidator (
   								wxTextValidator (wxFILTER_DIGITS, &m_LineIntervalString));
	fgSizer15->Add (m_textCtrl143, 0, wxALL, 5);
	
	
	gSizer23->Add (fgSizer15, 1, wxEXPAND|wxLEFT|wxTOP, 12);
	
   bSizer107->Add (gSizer23, wxSizerFlags(0).Align(wxALIGN_RIGHT));
	
	wxGridSizer* gSizer29;
	gSizer29 = new wxGridSizer (0, 2, 0, 0);
	
	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer (0, 2, 0, 0);
	fgSizer21->SetFlexibleDirection (wxBOTH);
	fgSizer21->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	wxBoxSizer* bSizer293;
	bSizer293 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText300 = new wxStaticText (this,
													wxID_ANY,
													wxT("Output"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText300->Wrap (-1);
	bSizer293->Add (m_staticText300, 0, wxALL, 5);
	
	m_listRadioCtrl = new wxRadioButton (this,
														IDC_ListRadio,
														wxT("List histogram values"),
														wxDefaultPosition,
														wxDefaultSize,
														wxRB_GROUP);
	bSizer293->Add (m_listRadioCtrl, 0, 0, 5);
	
	m_checkBox73 = new wxCheckBox (this,
												IDC_IncludeEmptyCheck,
												wxT("Include empty bins"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer293->Add (m_checkBox73, 0, wxLEFT, 30);
	
	m_checkBox74 = new wxCheckBox (this,
												IDC_BlankCheck,
												wxT("Blank values outside of range"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer293->Add (m_checkBox74, 0, wxLEFT, 30);
	
	m_plotRadioCtrl = new wxRadioButton (this,
														IDC_PlotRadio,
														wxT("Plot histograms"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	bSizer293->Add (m_plotRadioCtrl, 0, 0, 5);
	
	m_checkBox75 = new wxCheckBox (this,
												IDC_OverlayCheck,
												 wxT("Overlay density functions"),
												 wxDefaultPosition,
												 wxDefaultSize,
												 0);
	bSizer293->Add (m_checkBox75, 0, wxLEFT, 30);
	
	fgSizer21->Add (bSizer293, 1, wxEXPAND, 5);
	
	gSizer29->Add (fgSizer21, 1, wxEXPAND, 5);
	
	wxFlexGridSizer* fgSizer23;
	fgSizer23 = new wxFlexGridSizer (0, 2, 0, 0);
	fgSizer23->SetFlexibleDirection (wxBOTH);
	fgSizer23->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	wxBoxSizer* bSizer294;
	bSizer294 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText301 = new wxStaticText (this,
													wxID_ANY,
													wxT("Output Options"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText301->Wrap (-1);
	bSizer294->Add (m_staticText301, 0, wxLEFT, 40);
	
	m_staticText302 = new wxStaticText (this,
													wxID_ANY,
													wxT("Group by:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText302->Wrap (-1);
	bSizer294->Add (m_staticText302, 0, wxLEFT, 50);
	
	m_groupChannelsCtrl = new wxRadioButton (this,
															IDC_ChannelsRadio,
															wxT("Channels:"),
															wxDefaultPosition,
															wxDefaultSize,
															wxRB_GROUP);
	bSizer294->Add (m_groupChannelsCtrl, 0, wxLEFT, 60);
	
	m_groupFieldsClassesCtrl = new wxRadioButton (this,
																	IDC_Classes,
																	wxT("Fields/classes"),
																	wxDefaultPosition,
																	wxDefaultSize,
																	0);
	bSizer294->Add (m_groupFieldsClassesCtrl, 0, wxLEFT, 60);
	
	m_staticText303 = new wxStaticText (this,
													wxID_ANY,
													wxT("List Format:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText303->Wrap (-1);
	bSizer294->Add (m_staticText303, 0, wxLEFT, 40);
	
	m_matrixRadioCtrl = new wxRadioButton (this,
														IDC_MatrixRadio,
														wxT("Matrix"),
														wxDefaultPosition,
														wxDefaultSize,
														wxRB_GROUP);
	bSizer294->Add (m_matrixRadioCtrl, 0, wxLEFT, 50);
	
	m_singleColumnRadioCtrl = new wxRadioButton (this,
																IDC_ColumnRadio,
																wxT("Single Column"),
																wxDefaultPosition,
																wxDefaultSize,
																0);
	bSizer294->Add (m_singleColumnRadioCtrl, 0, wxLEFT, 50);
	
	fgSizer23->Add (bSizer294, 1, wxEXPAND, 5);
	
	gSizer29->Add (fgSizer23, 1, wxEXPAND, 5);
	
	bSizer107->Add (gSizer29, 1, wxEXPAND|wxLEFT, 12);

	CreateStandardButtons (bSizer107);
   
   SetSizerAndFit (bSizer107);
	
}	// end "CreateControls"



Boolean CMStatHistogramSpecsDlg::DoDialog (
				StatHistogramSpecsPtr			statHistogramSpecsPtr,
				FileInfoPtr							fileInfoPtr)

{  
	SInt16								returnCode;

	Boolean								continueFlag = FALSE;
	
	
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
      																				return (FALSE);
																			
	m_statHistogramSpecsPtr = statHistogramSpecsPtr;
	m_fileInfoPtr = fileInfoPtr;	

	returnCode = ShowModal ();//	}
	
	if (returnCode == wxID_OK)
		{ 
		StatHistogramDialogOK ((m_histogramClassFieldCode==kHistogramClass),
										m_lineInterval,
										m_columnInterval,
										m_featureTransformationFlag,
										m_channelSelection,
										m_localActiveNumberFeatures,
										m_localActiveFeaturesPtr,
										(m_listPlotCode==kListData),
										m_includeEmptyBinsFlag,
										m_blankValuesFlag,
										(m_listPlotCode==kPlotData),
										(m_groupByChannelsClassesCode==kGroupByChannels),
										(m_groupByChannelsClassesCode==kGroupByFieldsClasses),
										m_matrixColumnCode,
										m_overlayDensityFunctionFlag);

		continueFlag = TRUE;
		
		}	// end "if (returnCode == IDOK)"
	
	return (continueFlag);
		
}	// end "DoDialog"



void CMStatHistogramSpecsDlg::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	SInt16								channelSelection,
											groupCode,
											listPlotCode,
											matrixColumnCode;
	
	Boolean								histogramClassFlag;
   
	
   wxDialog::OnInitDialog (event);
	
	StatHistogramDialogInitialize (this,
												m_fileInfoPtr,
												gProjectInfoPtr->statsWindowMode,
												m_localFeaturesPtr, 
												m_localTransformFeaturesPtr,
												&histogramClassFlag,
												&m_lineInterval,
												&m_columnInterval,
												&m_featureTransformAllowedFlag,
												&m_featureTransformationFlag,
												&channelSelection,
												&m_localActiveNumberFeatures,
												&listPlotCode,
												&m_includeEmptyBinsFlag,
												&m_blankValuesFlag,
												&groupCode,
												&matrixColumnCode,
												&m_overlayDFAllowedFlag,
												&m_overlayDensityFunctionFlag);
	
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
	
	m_channelSelection = channelSelection;
	
	m_histogramClassFieldCode = kHistogramField;
	if (histogramClassFlag)
		m_histogramClassFieldCode = kHistogramClass;

	m_listPlotCode = listPlotCode;
   
	m_groupByChannelsClassesCode = groupCode;

	m_matrixColumnCode = matrixColumnCode;

	if (TransferDataToWindow ())
		PositionDialogWindow ();
	
	SelectDialogItemText (this, IDC_LineInterval, 0, SInt16_MAX);
   
}	// end "OnInitDialog"



void CMStatHistogramSpecsDlg::OnClassesRadio (
				wxCommandEvent& 					event)

{
   int listRadioValue = m_listRadioCtrl->GetValue ();
   
	StatHistogramDialogClassesParameters (this,
														(listRadioValue==1),
														m_overlayDFAllowedFlag,
														m_overlayDensityFunctionFlag);
	
}	// end "OnClassesRadio"



void CMStatHistogramSpecsDlg::OnFieldsRadio (
				wxCommandEvent& 					event)

{
	StatHistogramDialogFieldsParameters (this);
	
}	// end "OnFieldsRadio"



void CMStatHistogramSpecsDlg::OnListColumnFormatRadio (
				wxCommandEvent& 					event)

{
   m_matrixColumnCode = m_singleColumnRadioCtrl->GetValue ();

   if (m_matrixColumnCode == 1)
   	m_matrixColumnCode = kListSingleColumnFormat;
	else
   	m_matrixColumnCode = kListMatrixFormat;
	
}	// end "OnListColumnFormatRadio"



void CMStatHistogramSpecsDlg::OnListMatrixFormatRadio (
				wxCommandEvent& 					event)

{
   m_matrixColumnCode = m_matrixRadioCtrl->GetValue ();

   if (m_matrixColumnCode == 1)
   	m_matrixColumnCode = kListMatrixFormat;
	else
   	m_matrixColumnCode = kListSingleColumnFormat;
	
}	// end "OnListMatrixFormatRadio"



void CMStatHistogramSpecsDlg::OnListRadio (
				wxCommandEvent& 					event)

{
   int histogramClassCode = m_classRadioCtrl->GetValue ();

	StatHistogramDialogListParameters (this,
													(histogramClassCode==1),
													m_matrixColumnCode);
	
}	// end "OnListRadio"



void CMStatHistogramSpecsDlg::OnPlotRadio (
				wxCommandEvent& 					event)

{
   int histogramClassCode = m_classRadioCtrl->GetValue ();
   
	StatHistogramDialogPlotParameters (this,
													(histogramClassCode==1),
													m_overlayDensityFunctionFlag);
	
}	// end "OnPlotRadio"



void CMStatHistogramSpecsDlg::OnFeatureTransformation (
				wxCommandEvent& 					event)

{                                
   wxCheckBox* featureTransformationFlag =
   									(wxCheckBox*)FindWindow (IDC_UseFeatureTransformation);
   m_featureTransformationFlag = featureTransformationFlag->GetValue ();
	
	CheckFeatureTransformationDialog (this,
													m_featureTransformAllowedFlag, 
													IDC_UseFeatureTransformation, 
													IDC_ChannelsPrompt, 
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



void CMStatHistogramSpecsDlg::OnChannelComboSelendok (
				wxCommandEvent& 					event)

{
   HandleChannelsMenu (IDC_ChannelCombo,
								m_featureTransformationFlag,
								(SInt16)gProjectInfoPtr->numberStatisticsChannels,
								2,
								TRUE);
	
}	// end "OnChannelComboSelendok"



bool CMStatHistogramSpecsDlg::TransferDataFromWindow ()

{
	wxCheckBox* featureTransformationFlag =
    								(wxCheckBox*)FindWindow (IDC_UseFeatureTransformation);
   m_featureTransformationFlag = featureTransformationFlag->GetValue ();
	
   wxCheckBox* includeEmptyBinsFlag = (wxCheckBox*)FindWindow (IDC_IncludeEmptyCheck);
   m_includeEmptyBinsFlag = includeEmptyBinsFlag->GetValue ();
	
   wxCheckBox* blankValuesFlag = (wxCheckBox*)FindWindow (IDC_BlankCheck);
   m_blankValuesFlag = blankValuesFlag->GetValue ();
	
   wxCheckBox* overlayDensityFunctionFlag = (wxCheckBox*)FindWindow (IDC_OverlayCheck);
	m_overlayDensityFunctionFlag = overlayDensityFunctionFlag->GetValue ();
	
   wxTextCtrl* lineInterval = (wxTextCtrl*)FindWindow (IDC_LineInterval);
   m_lineInterval = wxAtoi (lineInterval->GetValue ());
	
   wxTextCtrl* columnInterval = (wxTextCtrl*)FindWindow (IDC_ColumnInterval);
	m_columnInterval = wxAtoi (columnInterval->GetValue ());
	
   if (m_classRadioCtrl->GetValue ())
   	m_histogramClassFieldCode = kHistogramClass;
   else
   	m_histogramClassFieldCode = kHistogramField;
	
   if (m_listRadioCtrl->GetValue ())
   	m_listPlotCode = kListData;
   else
   	m_listPlotCode = kPlotData;
	
	if (m_groupChannelsCtrl->GetValue ())
   	m_groupByChannelsClassesCode = kGroupByChannels;
   else
   	m_groupByChannelsClassesCode = kGroupByFieldsClasses;
	
	if (m_matrixRadioCtrl->GetValue ())
   	m_matrixColumnCode = kListMatrixFormat;
   else
   	m_matrixColumnCode = kListSingleColumnFormat;
	
   m_channelSelection = m_channelsCtrl->GetSelection ();

	return TRUE;
	
}	// end "TransferDataFromWindow"



bool CMStatHistogramSpecsDlg::TransferDataToWindow ()

{
   wxCheckBox* featureTransformationFlag =
   									(wxCheckBox*)FindWindow (IDC_UseFeatureTransformation);
   featureTransformationFlag->SetValue (m_featureTransformationFlag);
	
   wxCheckBox* includeEmptyBinsFlag = (wxCheckBox*)FindWindow (IDC_IncludeEmptyCheck);
   includeEmptyBinsFlag->SetValue (m_includeEmptyBinsFlag);
	
   wxCheckBox* blankValuesFlag = (wxCheckBox*)FindWindow (IDC_BlankCheck);
   blankValuesFlag->SetValue (m_blankValuesFlag);
	
   wxCheckBox* overlayDensityFunctionFlag = (wxCheckBox*)FindWindow (IDC_OverlayCheck);
	overlayDensityFunctionFlag->SetValue (m_overlayDensityFunctionFlag);
	
   wxTextCtrl* lineInterval = (wxTextCtrl*)FindWindow (IDC_LineInterval);
   lineInterval->SetValue (wxString::Format (wxT("%d"), (int) m_lineInterval));
	
   wxTextCtrl* columnInterval = (wxTextCtrl*)FindWindow (IDC_ColumnInterval);
	columnInterval->SetValue (wxString::Format (wxT("%d"), (int) m_columnInterval));
	
   m_classRadioCtrl->SetValue (m_histogramClassFieldCode == kHistogramClass);
	
   m_fieldRadioCtrl->SetValue (m_histogramClassFieldCode == kHistogramField);

   m_listRadioCtrl->SetValue (m_listPlotCode==kListData);

   m_plotRadioCtrl->SetValue (m_listPlotCode==kPlotData);
	
   m_matrixRadioCtrl->SetValue (m_matrixColumnCode == kListMatrixFormat);
	
	m_singleColumnRadioCtrl->SetValue (m_matrixColumnCode == kListSingleColumnFormat);
	
   m_groupChannelsCtrl->SetValue (m_groupByChannelsClassesCode==kGroupByChannels);
	
   m_groupFieldsClassesCtrl->SetValue (m_groupByChannelsClassesCode==kGroupByFieldsClasses);
	
   m_channelsCtrl->SetSelection (m_channelSelection);
	
   return TRUE;
	
}	// end "TransferDataToWindow"
