//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LStatisticsHistogramDialog.cpp : class implementation file
//	Class Definition:		LStatisticsHistogramDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			01/18/2019
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
#include "LStatisticsHistogramDialog.h"


extern void 		StatHistogramDialogInitialize (
							DialogPtr							dialogPtr,
							FileInfoPtr							fileInfoPtr,
							SInt16								statsWindowMode,
							UInt16*								localFeaturesPtr, 
							UInt16*								localTransformFeaturesPtr,
							Boolean*								histogramClassFlagPtr,
							SInt32*								lineIntervalPtr,
							SInt32*								columnIntervalPtr,
							Boolean*								featureTransformAllowedFlagPtr,
							Boolean*								featureTransformationFlagPtr,
							SInt16*								channelSelectionPtr,
							UInt16*								localNumberChannelsPtr,
							SInt16*								histogramOutputCodePtr,
							Boolean*								includeEmptyBinsFlagPtr,
							Boolean*								blankValuesFlagPtr,
							SInt16*								groupCodePtr,
							SInt16*								columnMatrixCodePtr,
							Boolean*								overlayDFAllowedFlagPtr,
							Boolean*								overlayDensityFunctionFlagPtr);

extern void 		StatHistogramDialogOK (
							Boolean								histogramClassFlag,
							SInt32								lineInterval,
							SInt32								columnInterval,
							Boolean								featureTransformationFlag,
							SInt16								channelSelection,
							UInt16								localNumberFeatures,
							UInt16*								localFeaturesPtr,
							Boolean								listDataFlag,
							Boolean								includeEmptyBinsFlag,
							Boolean								blankValuesFlag,
							Boolean								plotHistogramFlag,
							Boolean								groupChannelsFlag,
							Boolean								groupFieldsClassesFlag,
							SInt16								columnMatrixCode,
							Boolean								overlayDensityFunctionFlag);

extern void 		StatHistogramDialogClassesParameters (
							DialogPtr							dialogPtr,
							Boolean								listFlag,
							Boolean								overlayDFAllowedFlag,
							Boolean								overlayDensityFunctionFlag);
							
extern void 		StatHistogramDialogFieldsParameters (
							DialogPtr							dialogPtr);
							
extern void 		StatHistogramDialogListParameters (
							DialogPtr							dialogPtr,
							Boolean								histogramClassFlag,
							SInt16								columnMatrixCode);
							
extern void 		StatHistogramDialogPlotParameters (
							DialogPtr							dialogPtr,
							Boolean								histogramClassFlag,
							Boolean								overlayDensityFunctionFlag);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMStatHistogramSpecsDlg dialog

CMStatHistogramSpecsDlg::CMStatHistogramSpecsDlg(wxWindow* pParent,
   wxWindowID id, const wxString& title/*=NULL*/)
:CMDialog(CMStatHistogramSpecsDlg::IDD, pParent, title){
   m_featureTransformationFlag = FALSE;
	m_includeEmptyBinsFlag = FALSE;
	m_blankValuesFlag = FALSE;
	m_overlayDensityFunctionFlag = FALSE;
	m_lineInterval = 1;
	m_columnInterval = 1;
   
	m_histogramClassFieldCode = 0;
   m_histogramClassCode = 0;
   m_histogramFieldCode = 0;
   
	m_listPlotCode = 0;
   m_listCode = 0;
   m_plotCode = 0;
   
	m_channelsClassesCode = 0;
   m_channelsCode = 0;
   m_classesCode = 0;
   
	m_matrixColumnCode = 0;
   m_matrixCode = 0;
   m_columnCode = 0;
    
	//m_channelSelection = -1;
   
   m_initializedFlag = CMDialog::m_initializedFlag;
   
   if (m_initializedFlag)
		m_initializedFlag = GetDialogLocalVectors (
														&m_localFeaturesPtr,
														&m_localTransformFeaturesPtr,     
														NULL,
														NULL, 
														NULL,
														NULL,
														NULL,
														NULL);
   
   CreateControls();
}

CMStatHistogramSpecsDlg::~CMStatHistogramSpecsDlg(void)
{
   ReleaseDialogLocalVectors (m_localFeaturesPtr,
											m_localTransformFeaturesPtr,        
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL); 
} // end "~CMStatHistogramSpecsDlg"

BEGIN_EVENT_TABLE(CMStatHistogramSpecsDlg, CMDialog)
EVT_INIT_DIALOG(CMStatHistogramSpecsDlg::OnInitDialog)
EVT_TEXT(IDC_LineInterval, CMStatHistogramSpecsDlg::CheckLineInterval)
EVT_TEXT(IDC_ColumnInterval, CMStatHistogramSpecsDlg::CheckColumnInterval)
EVT_RADIOBUTTON(IDC_ClassesRadio, CMStatHistogramSpecsDlg::OnClassesRadio)
EVT_RADIOBUTTON(IDC_FieldsRadio, CMStatHistogramSpecsDlg::OnFieldsRadio)
EVT_RADIOBUTTON(IDC_ListRadio, CMStatHistogramSpecsDlg::OnListRadio)
EVT_RADIOBUTTON(IDC_PlotRadio, CMStatHistogramSpecsDlg::OnPlotRadio)
EVT_COMBOBOX(IDC_ChannelCombo, CMStatHistogramSpecsDlg::OnSelendokChannelCombo)
EVT_CHECKBOX(IDC_UseFeatureTransformation, CMStatHistogramSpecsDlg::OnFeatureTransformation)
EVT_COMBOBOX_DROPDOWN(IDC_ChannelCombo, CMStatHistogramSpecsDlg::OnSelendokChannelComboDropDown)
END_EVENT_TABLE()



void CMStatHistogramSpecsDlg::CreateControls(){
   
   this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer107;
	bSizer107 = new wxBoxSizer (wxVERTICAL);
	
	wxGridSizer* gSizer23;
	gSizer23 = new wxGridSizer( 0, 2, 0, 0 );
	
	wxBoxSizer* bSizer287;
	bSizer287 = new wxBoxSizer( wxVERTICAL );
	
	
	m_radioBtn33 = new wxRadioButton( this, IDC_ClassesRadio, wxT("Classes"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	bSizer287->Add( m_radioBtn33, 0, wxLEFT, 5 );
	
	m_radioBtn34 = new wxRadioButton( this, IDC_FieldsRadio, wxT("Fields"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer287->Add( m_radioBtn34, 0, wxLEFT, 5 );
	
	m_checkBox66 = new wxCheckBox( this, IDC_UseFeatureTransformation, wxT("Use feature transformation"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer287->Add( m_checkBox66, 0, wxALIGN_CENTER|wxALL, 5 );
	
	wxFlexGridSizer* fgSizer16;
	fgSizer16 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer16->SetFlexibleDirection( wxBOTH );
	fgSizer16->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText290 = new wxStaticText( this, IDC_ChannelsPrompt, wxT("Channels:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText290->Wrap( -1 );
	fgSizer16->Add( m_staticText290, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_comboBox36 = new wxComboBox( this, IDC_ChannelCombo, wxT("All Available"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	m_comboBox36->Append( wxT("All Available") );
	m_comboBox36->Append( wxT("Subset...") );
   fgSizer16->Add( m_comboBox36, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	
	bSizer287->Add( fgSizer16, 1, wxEXPAND, 5 );
	
	
	gSizer23->Add( bSizer287, 1, wxEXPAND|wxLEFT|wxTOP, 12 );
	
	wxFlexGridSizer* fgSizer15;
	fgSizer15 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer15->SetFlexibleDirection( wxBOTH );
	fgSizer15->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText287 = new wxStaticText( this, wxID_ANY, wxT("Line interval"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText287->Wrap( -1 );
	fgSizer15->Add( m_staticText287, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrl142 = new wxTextCtrl( this, IDC_LineInterval, wxEmptyString, wxDefaultPosition, wxSize(70,-1), 0 );
   m_textCtrl142->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_LineIntervalString));
	fgSizer15->Add( m_textCtrl142, 0, wxALL, 5 );
	
	m_staticText288 = new wxStaticText( this, wxID_ANY, wxT("Column interval"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText288->Wrap( -1 );
	fgSizer15->Add( m_staticText288, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrl143 = new wxTextCtrl( this, IDC_ColumnInterval, wxEmptyString, wxDefaultPosition, wxSize(70,-1), 0 );
   m_textCtrl143->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_LineIntervalString));
	fgSizer15->Add( m_textCtrl143, 0, wxALL, 5 );
	
	
	gSizer23->Add( fgSizer15, 1, wxEXPAND|wxLEFT|wxTOP, 12 );
	
	
	//bSizer107->Add( gSizer23, 0, wxALIGN_RIGHT|wxEXPAND, 12 );
   //bSizer107->Add( gSizer23, 0, wxEXPAND, 12 );
   bSizer107->Add (gSizer23, wxSizerFlags(0).Align(wxALIGN_RIGHT));
	
	wxGridSizer* gSizer29;
	gSizer29 = new wxGridSizer (0, 2, 0, 0 );
	
	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer21->SetFlexibleDirection( wxBOTH );
	fgSizer21->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* bSizer293;
	bSizer293 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText300 = new wxStaticText( this, wxID_ANY, wxT("Output"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText300->Wrap( -1 );
	bSizer293->Add( m_staticText300, 0, wxALL, 5 );
	
	m_radioBtn47 = new wxRadioButton( this, IDC_ListRadio, wxT("List histogram values"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	bSizer293->Add( m_radioBtn47, 0, 0, 5 );
	
	m_checkBox73 = new wxCheckBox( this, IDC_IncludeEmptyCheck, wxT("Include empty bins"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer293->Add( m_checkBox73, 0, wxLEFT, 30 );
	
	m_checkBox74 = new wxCheckBox( this, IDC_BlankCheck, wxT("Blank values outside of range"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer293->Add( m_checkBox74, 0, wxLEFT, 30 );
	
	m_radioBtn52 = new wxRadioButton( this, IDC_PlotRadio, wxT("Plot histograms"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer293->Add( m_radioBtn52, 0, 0, 5 );
	
	m_checkBox75 = new wxCheckBox( this, IDC_OverlayCheck, wxT("Overlay density functions"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer293->Add( m_checkBox75, 0, wxLEFT, 30 );
	
	
	fgSizer21->Add( bSizer293, 1, wxEXPAND, 5 );
	
	
	gSizer29->Add( fgSizer21, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer23;
	fgSizer23 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer23->SetFlexibleDirection( wxBOTH );
	fgSizer23->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* bSizer294;
	bSizer294 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText301 = new wxStaticText( this, wxID_ANY, wxT("Output Options"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText301->Wrap( -1 );
	bSizer294->Add( m_staticText301, 0, wxLEFT, 40 );
	
	m_staticText302 = new wxStaticText( this, wxID_ANY, wxT("Group by:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText302->Wrap( -1 );
	bSizer294->Add( m_staticText302, 0, wxLEFT, 50 );
	
	m_radioBtn48 = new wxRadioButton( this, IDC_ChannelsRadio, wxT("Channels:"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	bSizer294->Add( m_radioBtn48, 0, wxLEFT, 60 );
	
	m_radioBtn49 = new wxRadioButton( this, IDC_Classes, wxT("Fields/classes"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer294->Add( m_radioBtn49, 0, wxLEFT, 60 );
	
	m_staticText303 = new wxStaticText( this, wxID_ANY, wxT("List Format:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText303->Wrap( -1 );
	bSizer294->Add( m_staticText303, 0, wxLEFT, 40 );
	
	m_radioBtn50 = new wxRadioButton( this, IDC_MatrixRadio, wxT("Matrix"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	bSizer294->Add( m_radioBtn50, 0, wxLEFT, 50 );
	
	m_radioBtn51 = new wxRadioButton( this, IDC_ColumnRadio, wxT("Single Column"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer294->Add( m_radioBtn51, 0, wxLEFT, 50 );
	
	fgSizer23->Add( bSizer294, 1, wxEXPAND, 5 );
	
	gSizer29->Add( fgSizer23, 1, wxEXPAND, 5 );
	
	bSizer107->Add( gSizer29, 1, wxEXPAND|wxLEFT, 12 );
	/*
	wxBoxSizer* bSizer110;
	bSizer110 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button20 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer110->Add( m_button20, 0, wxALL, 5 );
	
	m_button21 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer110->Add( m_button21, 0, wxALL, 5 );
	
	bSizer107->Add( bSizer110, 0, wxALIGN_RIGHT|wxBOTTOM|wxRIGHT, 12 );
	*/
	CreateStandardButtons (bSizer107);
   
   SetSizerAndFit (bSizer107);
	
}	// end "CreateControls"



bool CMStatHistogramSpecsDlg::TransferDataToWindow()

{
   wxCheckBox* featureTransformationFlag = (wxCheckBox*) FindWindow(IDC_UseFeatureTransformation);
   wxCheckBox* includeEmptyBinsFlag = (wxCheckBox*) FindWindow(IDC_IncludeEmptyCheck);
   wxCheckBox* blankValuesFlag = (wxCheckBox*) FindWindow(IDC_BlankCheck);
   wxCheckBox* overlayDensityFunctionFlag = (wxCheckBox*) FindWindow(IDC_OverlayCheck);
  
   wxTextCtrl* lineInterval = (wxTextCtrl*) FindWindow(IDC_LineInterval);
   wxTextCtrl* columnInterval = (wxTextCtrl*) FindWindow(IDC_ColumnInterval);
   
   wxRadioButton *histogramClassCode = (wxRadioButton*)FindWindow(IDC_ClassesRadio);
   wxRadioButton *histogramFieldCode = (wxRadioButton*)FindWindow(IDC_FieldsRadio);
   wxRadioButton *listCode = (wxRadioButton*)FindWindow(IDC_ListRadio);
   wxRadioButton *plotCode = (wxRadioButton*)FindWindow(IDC_PlotRadio);
   wxRadioButton *channelsCode = (wxRadioButton*)FindWindow(IDC_ChannelsRadio);
   wxRadioButton *classesCode = (wxRadioButton*)FindWindow(IDC_Classes);
   wxRadioButton *matrixCode  = (wxRadioButton*)FindWindow(IDC_MatrixRadio);
   wxRadioButton *columnCode  = (wxRadioButton*)FindWindow(IDC_ColumnRadio);
   
   wxComboBox* channelSelection = (wxComboBox*) FindWindow(IDC_ChannelCombo);
   
   
   
   featureTransformationFlag->SetValue(m_featureTransformationFlag);
   includeEmptyBinsFlag->SetValue(m_includeEmptyBinsFlag);
   blankValuesFlag->SetValue(m_blankValuesFlag);
   overlayDensityFunctionFlag->SetValue(m_overlayDensityFunctionFlag);
   
    
   lineInterval->SetValue(wxString::Format(wxT("%d"), (int) m_lineInterval));
   columnInterval->SetValue(wxString::Format(wxT("%d"), (int) m_columnInterval));
   
   histogramClassCode->SetValue(m_histogramClassCode); 
   histogramFieldCode->SetValue(m_histogramFieldCode); 

   listCode->SetValue(m_listCode);

   plotCode->SetValue(m_plotCode);
	//plotCode->Enable(false);
   
   matrixCode->SetValue(m_matrixCode);
   columnCode->SetValue(m_columnCode);
   
   channelsCode->SetValue(m_channelsCode);
   classesCode->SetValue(m_classesCode);
   
   
   channelSelection->SetSelection(m_channelSelection);
	
   return TRUE; 
}



bool CMStatHistogramSpecsDlg::TransferDataFromWindow() {
   
   wxCheckBox* featureTransformationFlag = (wxCheckBox*) FindWindow(IDC_UseFeatureTransformation);
   wxCheckBox* includeEmptyBinsFlag = (wxCheckBox*) FindWindow(IDC_IncludeEmptyCheck);
   wxCheckBox* blankValuesFlag = (wxCheckBox*) FindWindow(IDC_BlankCheck);
   wxCheckBox* overlayDensityFunctionFlag = (wxCheckBox*) FindWindow(IDC_OverlayCheck);
   
   wxTextCtrl* lineInterval = (wxTextCtrl*) FindWindow(IDC_LineInterval);
   wxTextCtrl* columnInterval = (wxTextCtrl*) FindWindow(IDC_ColumnInterval);
   
   wxRadioButton *histogramClassCode = (wxRadioButton*)FindWindow(IDC_ClassesRadio);
   wxRadioButton *histogramFieldCode = (wxRadioButton*)FindWindow(IDC_FieldsRadio);
   wxRadioButton *listCode = (wxRadioButton*)FindWindow(IDC_ListRadio);
   wxRadioButton *plotCode = (wxRadioButton*)FindWindow(IDC_PlotRadio);
   wxRadioButton *channelsCode = (wxRadioButton*)FindWindow(IDC_ChannelsRadio);
   wxRadioButton *classesCode = (wxRadioButton*)FindWindow(IDC_Classes);
   wxRadioButton *matrixCode  = (wxRadioButton*)FindWindow(IDC_MatrixRadio);
   wxRadioButton *columnCode  = (wxRadioButton*)FindWindow(IDC_ColumnRadio);
   
   wxComboBox* channelSelection = (wxComboBox*) FindWindow(IDC_ChannelCombo);
    
   m_featureTransformationFlag = featureTransformationFlag->GetValue();
   m_includeEmptyBinsFlag = includeEmptyBinsFlag->GetValue();
   m_blankValuesFlag = blankValuesFlag->GetValue();
   m_overlayDensityFunctionFlag = overlayDensityFunctionFlag->GetValue();
   
   m_lineInterval = wxAtoi(lineInterval->GetValue());
   m_columnInterval = wxAtoi(columnInterval->GetValue());
   
   m_histogramClassCode = histogramClassCode->GetValue();
   m_histogramFieldCode = histogramFieldCode->GetValue();
   m_listCode = listCode->GetValue();
   m_plotCode = plotCode->GetValue();
   m_channelsCode = channelsCode->GetValue();
   m_classesCode = classesCode->GetValue();
   m_matrixCode = matrixCode->GetValue();
   m_columnCode = columnCode->GetValue();
   
   m_channelSelection = channelSelection->GetSelection();
   
   if(m_histogramClassCode == 0) m_histogramClassFieldCode = 1;
   else m_histogramClassFieldCode = 0;
   
   if(m_plotCode == 1) m_listPlotCode = 1;
   else m_listPlotCode = 0;
   
   if(m_classesCode == 1) m_channelsClassesCode = 1;
   else m_channelsClassesCode = 0;
   
   if(m_matrixCode == 1) m_matrixColumnCode = 0;
   else m_matrixColumnCode = 1;
   

   if(m_channelSelection < 0)
      m_channelSelection = m_channelSelection_Saved;

   //m_channelSelection = wxAtoi(channelSelection->GetValue());
   
   
   return TRUE;
   
}


//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the display
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
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/26/2002
//	Revised By:			Larry L. Biehl			Date: 09/26/2002	

Boolean 
CMStatHistogramSpecsDlg::DoDialog(
				StatHistogramSpecsPtr			statHistogramSpecsPtr,
				FileInfoPtr							fileInfoPtr)

{  
	SInt16								returnCode; 

	Boolean								continueFlag = FALSE;
	
	
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
      return(FALSE);
																			
	m_statHistogramSpecsPtr = statHistogramSpecsPtr;
	m_fileInfoPtr = fileInfoPtr;	
   
																			
//	if(TransferDataFromWindow()){	

	  returnCode = ShowModal();
//	}

  
	if (returnCode == wxID_OK)
		{ 
		StatHistogramDialogOK ((m_histogramClassFieldCode==0),
										m_lineInterval,
										m_columnInterval,
										m_featureTransformationFlag,
										m_channelSelection,
										m_localActiveNumberFeatures,
										m_localActiveFeaturesPtr,
										(m_listPlotCode==0),
										m_includeEmptyBinsFlag,
										m_blankValuesFlag,
										(m_listPlotCode==1),
										(m_channelsClassesCode==0),
										(m_channelsClassesCode==1),
										m_matrixColumnCode+1,
										m_overlayDensityFunctionFlag);


		continueFlag = TRUE;
		
		}		// end "if (returnCode == IDOK)"
	
	return (continueFlag);
		
}		// end "DoDialog"



void CMStatHistogramSpecsDlg::OnInitDialog (
				wxInitDialogEvent& event)

{
   SInt16				channelSelection,
							groupCode,
							matrixColumnCode,
							outputCode;
	
	Boolean				histogramClassFlag;
   
	
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
												(Boolean*)&m_featureTransformationFlag,
												&channelSelection,
												&m_localActiveNumberFeatures,
												&outputCode,
												(Boolean*)&m_includeEmptyBinsFlag,
												(Boolean*)&m_blankValuesFlag,
												&groupCode,
												&matrixColumnCode,
												&m_overlayDFAllowedFlag,
												(Boolean*)&m_overlayDensityFunctionFlag); 
	
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
	
	m_histogramClassFieldCode = 1;
	if (histogramClassFlag)
		m_histogramClassFieldCode = 0;
   
   if (m_histogramClassFieldCode == 0)
   	{
      m_histogramClassCode = 1;
      m_histogramFieldCode = 0;
   	}
	
	else
		{
      m_histogramClassCode = 0;
      m_histogramFieldCode = 1;
   	}

	m_listPlotCode = outputCode - 1;
   
	m_channelsClassesCode = groupCode - 1;

	m_savedMatrixColumnCode = matrixColumnCode - 1;
	m_matrixColumnCode = m_savedMatrixColumnCode;
   
   if(m_channelsClassesCode == 0){
      m_classesCode = 0;
      m_channelsCode = 1;
   }else{
      m_classesCode = 1;
      m_channelsCode = 0;
   }
   
   if (m_matrixColumnCode == 0)
   	{
      m_matrixCode = 1;
      m_columnCode = 0;
		
   	}
	
	else
		{
      m_matrixCode = 0;
      m_columnCode = 1;
		
   	}
   
	if (m_listPlotCode == 1)
		{
		m_matrixColumnCode = -1;
      m_listCode = 0;
      m_plotCode = 1;
      m_matrixCode = 0;
      m_columnCode = 0;

		}
	
	else
		{
      m_listCode = 1;
      m_plotCode = 0;
      m_matrixCode = 0;
      m_columnCode = 0;
     
		}

	if (TransferDataToWindow ())
		PositionDialogWindow ();
	
	SelectDialogItemText (this, IDC_LineInterval, 0, SInt16_MAX);
   
}	// end "OnInitDialog"



void CMStatHistogramSpecsDlg::OnClassesRadio(wxCommandEvent& event) 
{
	//DDX_Radio(m_dialogFromPtr, IDC_ListRadio, m_listPlotCode);
   wxRadioButton* listPlotCode = (wxRadioButton *) FindWindow(IDC_ListRadio);
   m_listPlotCode = listPlotCode->GetValue();
   
	StatHistogramDialogClassesParameters (this,
														(m_listPlotCode==0),
														m_overlayDFAllowedFlag,
														m_overlayDensityFunctionFlag);
	
}		// end "OnClassesRadio"

void CMStatHistogramSpecsDlg::OnFieldsRadio(wxCommandEvent& event) 
{
   
	StatHistogramDialogFieldsParameters (this);
	
}		// end "OnFieldsRadio"

void CMStatHistogramSpecsDlg::OnListRadio(wxCommandEvent& event) 
{
	//DDX_Radio(m_dialogFromPtr, IDC_ClassesRadio, m_histogramClassFieldCode);
   wxRadioButton* histogramClassFieldCode = (wxRadioButton *) FindWindow(IDC_ClassesRadio);
   m_histogramClassFieldCode = histogramClassFieldCode->GetValue();
	StatHistogramDialogListParameters (this,
													(m_histogramClassFieldCode==0),
													m_savedMatrixColumnCode+1);
	
}		// end "OnListRadio"

void CMStatHistogramSpecsDlg::OnPlotRadio(wxCommandEvent& event) 
{
	//DDX_Radio(m_dialogFromPtr, IDC_ClassesRadio, m_histogramClassFieldCode);
   wxRadioButton* histogramClassFieldCode = (wxRadioButton *) FindWindow(IDC_ClassesRadio);
   m_histogramClassFieldCode = histogramClassFieldCode->GetValue();

	//DDX_Radio(m_dialogFromPtr, IDC_MatrixRadio, m_matrixColumnCode);
   wxRadioButton* matrixColumnCode = (wxRadioButton *) FindWindow(IDC_MatrixRadio);
   m_matrixColumnCode = matrixColumnCode->GetValue();
   
	//if (m_matrixColumnCode >= 0)
	//	m_savedMatrixColumnCode = m_matrixColumnCode;
   
	
   
	StatHistogramDialogPlotParameters (this,
													(m_histogramClassFieldCode==0),
													m_overlayDensityFunctionFlag);
	
}		// end "OnPlotRadio"

void CMStatHistogramSpecsDlg::OnFeatureTransformation(wxCommandEvent& event)
{                                
	//DDX_Check(m_dialogFromPtr, 
	//				IDC_UseFeatureTransformation, 
	//				m_featureTransformationFlag);
   
   wxCheckBox* featureTransformationFlag = (wxCheckBox *) FindWindow(IDC_UseFeatureTransformation);
   m_featureTransformationFlag = featureTransformationFlag->GetValue();
	
	CheckFeatureTransformationDialog ( this,
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
	
}		// end "OnFeatureTransformation"

void CMStatHistogramSpecsDlg::OnSelendokChannelCombo(wxCommandEvent& event)
{
   
   HandleChannelsMenu(IDC_ChannelCombo, 
							m_featureTransformationFlag,
							(SInt16)gProjectInfoPtr->numberStatisticsChannels,
							2,
							TRUE);
   
	
}		// end "OnSelendokChannelCombo"
