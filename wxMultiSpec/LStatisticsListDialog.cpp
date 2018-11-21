// LStatisticsListDialog.cpp : implementation file
// CMListStatsDialog dialog
//
// Revised by Larry Biehl on 11/16/2018
//
#include "SMultiSpec.h"
#include "LStatisticsListDialog.h"

extern void ListStatsDialogInitialize(
        DialogPtr dialogPtr,
        SInt16 statsWindowMode,
        Boolean* listFieldFlagPtr,
        Boolean* listClassFlagPtr,
        Boolean* listMeansStdDevFlagPtr,
        Boolean* listCovarianceFlagPtr,
        Boolean* listCorrelationFlagPtr,
        Boolean* featureTransformationFlagPtr,
        SInt16* listMeanStdPrecisionPtr,
        SInt16* listCovCorPrecisionPtr);

extern void ListStatsDialogOK(
        Boolean listFieldFlag,
        Boolean listClassFlag,
        Boolean listMeansStdDevFlag,
        Boolean listCovarianceFlag,
        Boolean listCorrelationFlag,
        Boolean featureTransformationFlag,
        SInt16 listMeanStdPrecision,
        SInt16 listCovCorPrecision);


CMListStatsDialog::CMListStatsDialog(wxWindow* pParent,
   wxWindowID id, const wxString& title/*=NULL*/)
:CMDialog(CMListStatsDialog::IDD, pParent, title){

    m_listClassFlag = FALSE;
    m_listFieldFlag = FALSE;
    m_listMeansStdDevFlag = FALSE;
    m_listCovarianceFlag = FALSE;
    m_listCorrelationFlag = FALSE;
    m_featureTransformationFlag = FALSE;
    m_listMeanStdPrecision = 1;
    m_listCovCorPrecision = 2;
    
    CreateControls();

}

BEGIN_EVENT_TABLE(CMListStatsDialog, CMDialog)
EVT_INIT_DIALOG(CMListStatsDialog::OnInitDialog)
EVT_COMBOBOX(IDC_Classes, CMListStatsDialog::OnClasses)
EVT_COMBOBOX(IDC_Classes, CMListStatsDialog::OnFields)
END_EVENT_TABLE()


Boolean
CMListStatsDialog::DoDialog(
        SInt16 statsWindowMode) {
    SInt16 returnCode;

    Boolean continueFlag = FALSE;


    // Make sure intialization has been completed.

    if (!m_initializedFlag)
        return (FALSE);

    m_statsWindowMode = statsWindowMode;

    if(TransferDataFromWindow()){																			
	  returnCode = ShowModal();
	}

    if (returnCode == wxID_OK) {
        continueFlag = TRUE;

        ListStatsDialogOK(m_listFieldFlag,
                m_listClassFlag,
                m_listMeansStdDevFlag,
                m_listCovarianceFlag,
                m_listCorrelationFlag,
                m_featureTransformationFlag,
                m_listMeanStdPrecision,
                m_listCovCorPrecision);

    } // end "if (returnCode == IDOK)"

    return (continueFlag);

} // end "DoDialog"


/////////////////////////////////////////////////////////////////////////////
// CMListStatsDialog message handlers

void CMListStatsDialog::OnInitDialog(wxInitDialogEvent& event) {
    SInt16 listCovCorPrecision,
            listMeanStdPrecision;


    wxDialog::OnInitDialog(event);


    ListStatsDialogInitialize(this,
            m_statsWindowMode,
            (Boolean*) & m_listFieldFlag,
            (Boolean*) & m_listClassFlag,
            (Boolean*) & m_listMeansStdDevFlag,
            (Boolean*) & m_listCovarianceFlag,
            (Boolean*) & m_listCorrelationFlag,
            (Boolean*) & m_featureTransformationFlag,
            &listMeanStdPrecision,
            &listCovCorPrecision);

    m_listMeanStdPrecision = listMeanStdPrecision;
    m_listCovCorPrecision = listCovCorPrecision;

     if (TransferDataToWindow() )                   
	    PositionDialogWindow ();

    // Set default text selection to first edit text item

    SelectDialogItemText(this, IDC_MeanPrecision, 0, SInt16_MAX);

    //return FALSE; // return TRUE  unless you set the focus to a control

} // end "OnInitDialog"

//#ifndef multispec_lin

void
CMListStatsDialog::CheckListFieldClassSettings(void) {
    Boolean enableFlag = FALSE;


    if (m_listFieldFlag ||
            m_listClassFlag)
        enableFlag = TRUE;

    //GetDlgItem(IDOK)->EnableWindow(enableFlag);

} // end "CheckListFieldClassSettings"
//#endif
void CMListStatsDialog::OnClasses(wxCommandEvent& event) {
   wxComboBox* listClassFlag = (wxComboBox *) FindWindow(IDC_Classes);
   m_listClassFlag = listClassFlag->GetSelection();

    CheckListFieldClassSettings();

} // end "OnClasses"

void CMListStatsDialog::OnFields(wxCommandEvent& event) {
   wxComboBox* listFieldFlag = (wxComboBox *) FindWindow(IDC_Fields);
   m_listFieldFlag = listFieldFlag->GetSelection();

    CheckListFieldClassSettings();

} // end "OnFields"

//#endif

bool CMListStatsDialog::TransferDataToWindow() {
   wxCheckBox* listClassFlag = (wxCheckBox*) FindWindow(IDC_Classes);
   wxCheckBox* listFieldFlag = (wxCheckBox*) FindWindow(IDC_Fields);
   wxCheckBox* listMeansStdDevFlag = (wxCheckBox*) FindWindow(IDC_MeansStdDev);
   wxCheckBox* listCovarianceFlag = (wxCheckBox*) FindWindow(IDC_CovarianceMatrix);
   wxCheckBox* listCorrelationFlag = (wxCheckBox*) FindWindow(IDC_CorrelationMatrix);
   wxCheckBox* featureTransformationFlag = (wxCheckBox*) FindWindow(IDC_UseTransformation);
   
   wxTextCtrl* listMeanStdPrecision = (wxTextCtrl*) FindWindow(IDC_MeanPrecision);
   wxTextCtrl* listCovCorPrecision = (wxTextCtrl*) FindWindow(IDC_CovariancePrecision);
   
   listClassFlag->SetValue(m_listClassFlag);
   listFieldFlag->SetValue(m_listFieldFlag);
   listMeansStdDevFlag->SetValue(m_listMeansStdDevFlag);
   listCovarianceFlag->SetValue(m_listCovarianceFlag);
   listCorrelationFlag->SetValue(m_listCorrelationFlag);
   featureTransformationFlag->SetValue(m_featureTransformationFlag);
   
   listMeanStdPrecision->SetValue(wxString::Format(wxT("%d"), (int) m_listMeanStdPrecision));
   listCovCorPrecision->SetValue(wxString::Format(wxT("%d"), (int) m_listCovCorPrecision));
   
   return TRUE; 
}

bool CMListStatsDialog::TransferDataFromWindow() {
   
   wxCheckBox* listClassFlag = (wxCheckBox*) FindWindow(IDC_Classes);
   wxCheckBox* listFieldFlag = (wxCheckBox*) FindWindow(IDC_Fields);
   wxCheckBox* listMeansStdDevFlag = (wxCheckBox*) FindWindow(IDC_MeansStdDev);
   wxCheckBox* listCovarianceFlag = (wxCheckBox*) FindWindow(IDC_CovarianceMatrix);
   wxCheckBox* listCorrelationFlag = (wxCheckBox*) FindWindow(IDC_CorrelationMatrix);
   wxCheckBox* featureTransformationFlag = (wxCheckBox*) FindWindow(IDC_UseTransformation);
   
   wxTextCtrl* listMeanStdPrecision = (wxTextCtrl*) FindWindow(IDC_MeanPrecision);
   wxTextCtrl* listCovCorPrecision = (wxTextCtrl*) FindWindow(IDC_CovariancePrecision);
   
   m_listClassFlag = listClassFlag->GetValue();
   m_listFieldFlag = listFieldFlag->GetValue();
   m_listMeansStdDevFlag = listMeansStdDevFlag->GetValue();
   m_listCovarianceFlag = listCovarianceFlag->GetValue();
   m_listCorrelationFlag = listCorrelationFlag->GetValue();
   m_featureTransformationFlag = featureTransformationFlag->GetValue();
   
   m_listMeanStdPrecision = wxAtoi(listMeanStdPrecision->GetValue());
   m_listCovCorPrecision = wxAtoi(listCovCorPrecision->GetValue());
   
   return TRUE;
   
}

void CMListStatsDialog::CreateControls ()

{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer107;
	bSizer107 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer108;
	bSizer108 = new wxBoxSizer( wxHORIZONTAL );
	
	wxGridSizer* gSizer13;
	gSizer13 = new wxGridSizer( 0, 2, 0, 0 );
	
	wxBoxSizer* bSizer256;
	bSizer256 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText258 = new wxStaticText( this, wxID_ANY, wxT("List Statistics For:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText258->Wrap( -1 );
	bSizer256->Add( m_staticText258, 0, wxALL, 5 );
	
	m_checkBox44 = new wxCheckBox( this, IDC_Classes, wxT("Classes"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer256->Add( m_checkBox44, 0, wxALL, 5 );
	
	m_checkBox45 = new wxCheckBox( this, IDC_Fields, wxT("Fields"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer256->Add( m_checkBox45, 0, wxALL, 5 );
	
	
	gSizer13->Add( bSizer256, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer257;
	bSizer257 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText259 = new wxStaticText( this, wxID_ANY, wxT("Include:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText259->Wrap( -1 );
	bSizer257->Add( m_staticText259, 0, wxALL, 5 );
	
	m_checkBox46 = new wxCheckBox( this, IDC_MeansStdDev, wxT("Means & standard deviations"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer257->Add( m_checkBox46, 0, wxALL, 5 );
	
	m_checkBox47 = new wxCheckBox( this, IDC_CovarianceMatrix, wxT("Covariance matrix"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer257->Add( m_checkBox47, 0, wxALL, 5 );
	
	m_checkBox48 = new wxCheckBox( this, IDC_CorrelationMatrix, wxT("Correlation matrix"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer257->Add( m_checkBox48, 0, wxALL, 5 );
	
	
	gSizer13->Add( bSizer257, 0, wxEXPAND, 5 );
	
	
	bSizer108->Add( gSizer13, 0, wxFIXED_MINSIZE, 5 );
	
	
	bSizer107->Add( bSizer108, 0, wxFIXED_MINSIZE|wxLEFT|wxRIGHT|wxTOP, 12 );
	
	wxBoxSizer* bSizer260;
	bSizer260 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText264 = new wxStaticText( this, wxID_ANY, wxT("Other Options"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText264->Wrap( -1 );
	bSizer260->Add( m_staticText264, 0, 0, 5 );
	
	wxBoxSizer* bSizer271;
	bSizer271 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText265 = new wxStaticText( this, wxID_ANY, wxT("Mean and Std. Dev. precision (decimal digits):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText265->Wrap( -1 );
	fgSizer11->Add( m_staticText265, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrl125 = new wxTextCtrl( this, IDC_MeanPrecision, wxEmptyString, wxDefaultPosition, wxSize(50,-1), 0 );
   m_textCtrl125->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_meanPrecisionString));
	fgSizer11->Add( m_textCtrl125, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText266 = new wxStaticText( this, wxID_ANY, wxT("Covariance/Correlation precision (decimal digits):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText266->Wrap( -1 );
	fgSizer11->Add( m_staticText266, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrl126 = new wxTextCtrl( this, IDC_CovariancePrecision, wxEmptyString, wxDefaultPosition, wxSize(50,-1), 0 );
   m_textCtrl126->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_covPrecisionString));
	fgSizer11->Add( m_textCtrl126, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizer271->Add( fgSizer11, 0, 0, 5 );
	
	
	bSizer260->Add( bSizer271, 0, 0, 5 );
	
	wxBoxSizer* bSizer275;
	bSizer275 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox49 = new wxCheckBox( this, IDC_UseTransformation, wxT("Use transformation"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer275->Add( m_checkBox49, 0, wxALL, 5 );
	
	bSizer260->Add( bSizer275, 0, 0, 5 );
	
	bSizer107->Add( bSizer260, 0, wxLEFT|wxRIGHT|wxTOP, 12 );
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
	
	this->Centre (wxBOTH);
   
   SetSizerAndFit (bSizer107);
   
}
