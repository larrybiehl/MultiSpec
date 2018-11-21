// LReformatTransformDialog.cpp : implementation file
//                   
// Revised by Larry Biehl on 11/16/2018
//
#include "SMultiSpec.h" 
#include "LReformatTransformDialog.h"
#include "LChannelsDialog.h"   

//#include  "SExternalGlobals.h"

extern Boolean GetDefaultBandRatio(
        WindowInfoPtr windowInfoPtr,
        FileInfoPtr fileInfoPtr,
        ReformatOptionsPtr reformatOptionsPtr);



CMReformatTransformDlg::CMReformatTransformDlg(wxWindow* pParent, wxWindowID id, const wxString& title /*=NULL*/)
: CMDialog(CMReformatTransformDlg::IDD, pParent, title) {
    //{{AFX_DATA_INIT(CMReformatTransform)
    m_adjustDivisor = 0;
    m_adjustFactor = 0;
    m_adjustOffset = 0;
    m_transformFactor = 0;
    m_transformOffset = 0;
    m_denominatorString = "";
    m_numeratorString = "";
    m_scaleFactor = 0;
    m_channelSelection = -1;
    m_minimumNumberBits = 1;
    m_minSelectedNumberBits = 8;
    m_transformCode = -1;
    m_functionFactor = 1;
    m_kthSmallestElement = 1;
    m_functionCode = -1;
    m_adjustSelectedChannelsFactor = -1.;
    m_adjustSelectedChannel = 1;
    //}}AFX_DATA_INIT 	

    m_maxAdjustOffset = SInt16_MAX;
    m_maxAdjustFactor = SInt16_MAX;
    m_maxAdjustDivisor = SInt16_MAX;

    m_maxValue = SInt16_MAX;
    m_minValue = SInt16_MAX;

    m_denominatorStringPtr = NULL;
    m_numeratorStringPtr = NULL;

    m_initializedFlag = CMDialog::m_initializedFlag;

    if (m_initializedFlag) {
        try {
            //	Set the algebraic transform descriptions.	
            wxString m_denomBufString(wxStringBuffer(m_denominatorString, 256));
            wxString m_numermBufString(wxStringBuffer(m_numeratorString, 256));
            m_denominatorStringPtr = strdup(m_denomBufString);
            m_numeratorStringPtr = strdup(m_numermBufString);
           
        } catch (int e) {
            m_initializedFlag = FALSE;

        }
        //END_CATCH_ALL
    } // end "if (m_initializedFlag)"

    CreateControls();
    SetSizerAndFit(bSizer84);
} // end "CMReformatTransformDlg"

CMReformatTransformDlg::~CMReformatTransformDlg() {
    //    if (m_denominatorStringPtr != NULL)
    //        m_denominatorString.ReleaseBuffer();
    //
    //    if (m_denominatorStringPtr != NULL)
    //        m_denominatorString.ReleaseBuffer();

} // end "~CMReformatTransformDlg"


BEGIN_EVENT_TABLE(CMReformatTransformDlg, CMDialog)
EVT_INIT_DIALOG(CMReformatTransformDlg::OnInitDialog)
EVT_RADIOBUTTON(IDC_AdjustSelectedChannels, CMReformatTransformDlg::OnAdjustSelectedChannels)
EVT_RADIOBUTTON(IDC_RT_AlgebraicTransformation, CMReformatTransformDlg::OnRTAlgebraicTransformation)
EVT_RADIOBUTTON(IDC_RT_Eigenvectors, CMReformatTransformDlg::OnRTEigenvectors)
EVT_RADIOBUTTON(IDC_RT_FunctionOfChannels, CMReformatTransformDlg::OnRTFunctionOfChannels)
EVT_RADIOBUTTON(IDC_RT_NoTransformation, CMReformatTransformDlg::OnRTNoTransformation)
EVT_COMBOBOX(IDC_EV_Eigenvectors, CMReformatTransformDlg::OnSelendokEVEigenvectors)
EVT_COMBOBOX(IDC_ReformatFunctions, CMReformatTransformDlg::OnSelendokReformatFunctions)
EVT_RADIOBUTTON(IDC_AdjustSelectedChannelsbyChannel, CMReformatTransformDlg::OnAdjustSelectedChannelsByChannel)
//EVT_CHAR_HOOK(CMReformatTransformDlg::OnButtonPress)
END_EVENT_TABLE()


void CMReformatTransformDlg::CreateControls()
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	
	//wxBoxSizer* bSizer84;
	bSizer84 = new wxBoxSizer(wxVERTICAL);
	
	wxBoxSizer* bSizer85;
	bSizer85 = new wxBoxSizer(wxVERTICAL);
	
	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer(wxHORIZONTAL);
	
	m_radioBtn5 = new wxRadioButton(this, IDC_AdjustSelectedChannels, wxT("Adjust Selected Channels"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_radioBtn5, IDS_ToolTip249);
	//bSizer91->Add (m_radioBtn5, 0, wxALL, 5);
	bSizer91->Add (m_radioBtn5, wxSizerFlags(0).Border(wxBOTTOM, 5));
	
	
	bSizer85->Add(bSizer91, 0, wxALL | wxEXPAND, 5);
	
	wxBoxSizer* bSizer92;
	bSizer92 = new wxBoxSizer(wxHORIZONTAL);
	
	
	bSizer92->Add(25, 0, 0, wxEXPAND, 5);
	
	m_staticText127 = new wxStaticText(this, IDC_RT_AC_Equal, wxT("="), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText127->Wrap(-1);
	bSizer92->Add(m_staticText127, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_textCtrl48 = new wxTextCtrl(this, IDC_RT_AC_Offset, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_textCtrl48, IDS_ToolTip250);
	bSizer92->Add(m_textCtrl48, 0, wxALL, 5);
	
	m_staticText128 = new wxStaticText(this, IDC_RT_AC_Plus1, wxT("+"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText128->Wrap(-1);
	bSizer92->Add(m_staticText128, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_textCtrl49 = new wxTextCtrl(this, IDC_RT_AC_Factor, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_textCtrl49, IDS_ToolTip251);
	bSizer92->Add(m_textCtrl49, 0, wxALL, 5);
	
	m_staticText129 = new wxStaticText(this, IDC_RT_AC_OldValue, wxT("* OldValue /"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText129->Wrap(-1);
	bSizer92->Add(m_staticText129, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_textCtrl50 = new wxTextCtrl(this, IDC_RT_AC_Divisor, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_textCtrl50, IDS_ToolTip251);
	bSizer92->Add(m_textCtrl50, 0, wxALL, 5);
	
	
	bSizer85->Add(bSizer92, 0, wxEXPAND, 5);
	
	
	//bSizer84->Add(bSizer85, 0, wxEXPAND, 5);
	bSizer84->Add(bSizer85, wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT, 12));
	
	wxBoxSizer* bSizer86;
	bSizer86 = new wxBoxSizer(wxVERTICAL);
	
	wxBoxSizer* bSizer93;
	bSizer93 = new wxBoxSizer(wxHORIZONTAL);
	
	m_radioBtn6 = new wxRadioButton(this, IDC_AdjustSelectedChannelsbyChannel, wxT("Adjust Selected Channels by Selected Channel"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_radioBtn6, IDS_ToolTip252);
	//bSizer93->Add (m_radioBtn6, 0, wxALL, 5);
	bSizer93->Add (m_radioBtn6, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 5));
	
	
	bSizer86->Add(bSizer93, 0, wxALL | wxEXPAND, 5);
	
	wxBoxSizer* bSizer94;
	bSizer94 = new wxBoxSizer(wxHORIZONTAL);
	
	
	bSizer94->Add(25, 0, 0, wxEXPAND, 5);
	
	m_staticText130 = new wxStaticText(this, IDC_RT_ACbyC_OldValue, wxT("=  OldValue + "), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText130->Wrap(-1);
	bSizer94->Add(m_staticText130, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_textCtrl51 = new wxTextCtrl(this, IDC_RT_ACbyC_Factor, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	bSizer94->Add(m_textCtrl51, 0, wxALL, 5);
	
	m_staticText131 = new wxStaticText(this, IDC_RT_ACbyC_ChannelPrompt, wxT("* Channel:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText131->Wrap(-1);
	bSizer94->Add(m_staticText131, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_textCtrl52 = new wxTextCtrl(this, IDC_RT_ACbyC_ChannelNumber, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	bSizer94->Add(m_textCtrl52, 0, wxALL, 5);
	
	
	bSizer86->Add(bSizer94, 0, wxEXPAND, 5);
	
	
	//bSizer84->Add(bSizer86, 0, wxEXPAND, 5);
	bSizer84->Add(bSizer86, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	wxBoxSizer* bSizer87;
	bSizer87 = new wxBoxSizer(wxVERTICAL);
	
	
	bSizer103 = new wxBoxSizer(wxHORIZONTAL);
	
	m_radioBtn7 = new wxRadioButton(this, IDC_RT_Eigenvectors, wxT("New Channels from"), wxDefaultPosition, wxDefaultSize, 0);
    SetUpToolTip(m_radioBtn7, IDS_ToolTip253);
	//bSizer103->Add (m_radioBtn7, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer103->Add (m_radioBtn7, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxTOP|wxBOTTOM, 5));
	
	m_staticText145 = new wxStaticText(this, IDC_RT_EV_PCEigenvectors, wxT("PC Eigenvectors"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText145->Wrap(-1);
   SetUpToolTip(m_staticText145, IDS_ToolTip254);
	bSizer103->Add(m_staticText145, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_staticText134 = new wxStaticText(this, IDC_RT_EV_FEEigenvectors, wxT("FE Eigenvectors"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText134->Wrap(-1);
   SetUpToolTip(m_staticText134, IDS_ToolTip255);
	bSizer103->Add(m_staticText134, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_comboBox14 = new wxComboBox(this, IDC_EV_Eigenvectors, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	m_comboBox14->Append(wxT("All"));
	m_comboBox14->Append(wxT("Subset..."));
   SetUpToolTip(m_comboBox14, IDS_ToolTip256);
	bSizer103->Add(m_comboBox14, 0, wxALL, 5);
	
	
	bSizer87->Add(bSizer103, 1, wxALL | wxEXPAND, 5);
	
	wxBoxSizer* bSizer104;
	bSizer104 = new wxBoxSizer(wxHORIZONTAL);
	
	
	bSizer104->Add(25, 0, 0, wxEXPAND, 5);
	
	wxBoxSizer* bSizer106;
	bSizer106 = new wxBoxSizer(wxVERTICAL);
	
	m_staticText135 = new wxStaticText(this, IDC_RT_EV_NumberBits, wxT("Recommended minimum number of bits - All PC Components:\r                                                                 Selected Components:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText135->Wrap(-1);
	bSizer106->Add(m_staticText135, 0, wxALIGN_RIGHT | wxALL, 5);
	
	
	bSizer104->Add(bSizer106, 0, 0, 5);
	
	wxBoxSizer* bSizer105;
	bSizer105 = new wxBoxSizer(wxVERTICAL);
	
	m_staticText136 = new wxStaticText(this, IDC_RT_EV_AllComponents, wxT("16"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText136->Wrap(-1);
	bSizer105->Add(m_staticText136, 0, wxTOP, 5);
	
	m_staticText137 = new wxStaticText(this, IDC_RT_EV_SelComponents, wxT("16"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText137->Wrap(-1);
	bSizer105->Add(m_staticText137, 0, 0, 5);
	
	
	bSizer104->Add(bSizer105, 0, wxEXPAND, 5);
	
	
	bSizer87->Add(bSizer104, 1, wxEXPAND, 5);
	
	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer(wxHORIZONTAL);
	
	
	bSizer111->Add(25, 0, 0, wxEXPAND, 5);
	
	m_staticText140 = new wxStaticText(this, IDC_ScaleFactorPrompt, wxT("Scale Factor:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText140->Wrap(-1);
	bSizer111->Add(m_staticText140, 0, wxALIGN_CENTER | wxLEFT, 5);
	
	m_textCtrl56 = new wxTextCtrl(this, IDC_EV_ScaleFactor, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	bSizer111->Add(m_textCtrl56, 0, wxALIGN_CENTER | wxLEFT, 5);
	
	
	bSizer87->Add(bSizer111, 1, wxEXPAND, 5);
	
	
	//bSizer84->Add(bSizer87, 0, wxEXPAND, 5);
	bSizer84->Add(bSizer87, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	wxBoxSizer* bSizer88;
	bSizer88 = new wxBoxSizer(wxVERTICAL);
	
	wxBoxSizer* bSizer96;
	bSizer96 = new wxBoxSizer(wxHORIZONTAL);
	
	m_radioBtn8 = new wxRadioButton(this, IDC_RT_AlgebraicTransformation, wxT("New Channel from General Algebraic Transformation"), wxDefaultPosition, wxDefaultSize, 0);
	//bSizer96->Add (m_radioBtn8, 0, wxALL, 5);
   SetUpToolTip(m_radioBtn8, IDS_ToolTip257);
	bSizer96->Add (m_radioBtn8, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 5));
	
	
	bSizer88->Add(bSizer96, 0, wxALL | wxEXPAND, 5);
	
	wxBoxSizer* bSizer97;
	bSizer97 = new wxBoxSizer(wxHORIZONTAL);
	
	
	bSizer97->Add(25, 0, 0, wxEXPAND, 5);
	
	m_staticText141 = new wxStaticText(this, IDC_RT_AT_Equal, wxT("="), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText141->Wrap(-1);
	bSizer97->Add(m_staticText141, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	
	m_textCtrl57 = new wxTextCtrl(this, IDC_RT_AT_Offset, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_textCtrl57, IDS_ToolTip250);
	bSizer97->Add(m_textCtrl57, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	
	m_staticText142 = new wxStaticText(this, IDC_RT_AT_Plus, wxT("+"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText142->Wrap(-1);
	bSizer97->Add(m_staticText142, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	
	wxBoxSizer* bSizer112;
	bSizer112 = new wxBoxSizer(wxVERTICAL);
	
	m_textCtrl58 = new wxTextCtrl(this, IDC_RT_AT_Numerator, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0);
   SetUpToolTip(m_textCtrl58, IDS_ToolTip259);
	bSizer112->Add(m_textCtrl58, 0, wxALIGN_CENTER, 5);
	
	m_staticText144 = new wxStaticText(this, IDC_RT_AT_Line, wxT("_____________________________"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText144->Wrap(-1);
	bSizer112->Add(m_staticText144, 0, wxALIGN_CENTER | wxBOTTOM, 10);
	
	m_textCtrl59 = new wxTextCtrl(this, IDC_RT_AT_Denominator, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0);
   SetUpToolTip(m_textCtrl59, IDS_ToolTip260);
	bSizer112->Add(m_textCtrl59, 0, wxALIGN_CENTER, 5);
	
	
	bSizer97->Add(bSizer112, 0, wxEXPAND, 5);
	
	m_staticText143 = new wxStaticText(this, IDC_RT_AT_Multiply, wxT("*"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText143->Wrap(-1);
	bSizer97->Add(m_staticText143, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_textCtrl60 = new wxTextCtrl(this, IDC_RT_AT_Factor, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_textCtrl60, IDS_ToolTip258);
	bSizer97->Add(m_textCtrl60, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	
	
	bSizer88->Add(bSizer97, 0, wxEXPAND, 5);
	
	
	//bSizer84->Add(bSizer88, 0, wxEXPAND, 5);
	bSizer84->Add(bSizer88, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	wxBoxSizer* bSizer89;
	bSizer89 = new wxBoxSizer(wxVERTICAL);
	
	wxBoxSizer* bSizer98;
	bSizer98 = new wxBoxSizer(wxHORIZONTAL);
	
	m_radioBtn9 = new wxRadioButton(this, IDC_RT_FunctionOfChannels, wxT("New Channel from Function"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_radioBtn9, IDS_ToolTip261);
	//bSizer98->Add (m_radioBtn9, 0, wxALL, 5);
	bSizer98->Add (m_radioBtn9, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 5));
	
	
	bSizer89->Add(bSizer98, 0, wxALL | wxEXPAND, 5);
	
	wxBoxSizer* bSizer99;
	bSizer99 = new wxBoxSizer(wxHORIZONTAL);
	
	
	bSizer99->Add(25, 0, 0, wxEXPAND, 5);
	
	m_comboBox13 = new wxComboBox(this, IDC_ReformatFunctions, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	m_comboBox13->Append(wxT("Minimum"));
	m_comboBox13->Append(wxT("Maximum"));
	m_comboBox13->Append(wxT("Sum"));
	m_comboBox13->Append(wxT("Average"));
	m_comboBox13->Append(wxT("Median"));
	m_comboBox13->Append(wxT("Kth Smallest Value"));
	bSizer99->Add(m_comboBox13, 0, wxALL, 5);
	
	m_staticText132 = new wxStaticText(this, IDC_FunctionFactorPrompt, wxT("of selected channels *"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText132->Wrap(-1);
	bSizer99->Add(m_staticText132, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_textCtrl53 = new wxTextCtrl(this, IDC_FunctionFactor, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	bSizer99->Add(m_textCtrl53, 0, wxALL, 5);
	
	
	bSizer89->Add(bSizer99, 0, wxEXPAND, 5);
	
	wxBoxSizer* bSizer102;
	bSizer102 = new wxBoxSizer(wxHORIZONTAL);
	
	
	bSizer102->Add(60, 0, 0, wxEXPAND, 5);
	
	m_staticText133 = new wxStaticText(this, IDC_kthSmallestElementPrompt, wxT("kth smallest value:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText133->Wrap(-1);
	bSizer102->Add(m_staticText133, 0, wxALIGN_CENTER | wxALL, 5);
	
	m_textCtrl54 = new wxTextCtrl(this, IDC_kthSmallestElement, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	bSizer102->Add(m_textCtrl54, 0, wxALL, 5);
	
	
	bSizer89->Add(bSizer102, 1, wxEXPAND, 5);
	
	
	//bSizer84->Add(bSizer89, 0, wxEXPAND, 5);
	bSizer84->Add(bSizer89, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	wxBoxSizer* bSizer90;
	bSizer90 = new wxBoxSizer(wxVERTICAL);
	
	wxBoxSizer* bSizer100;
	bSizer100 = new wxBoxSizer(wxHORIZONTAL);
	
	m_radioBtn10 = new wxRadioButton(this, IDC_RT_NoTransformation, wxT("No Transformation to be Done"), wxDefaultPosition, wxDefaultSize, 0);
	//bSizer100->Add (m_radioBtn10, 0, wxALL, 5);
	bSizer100->Add (m_radioBtn10, wxSizerFlags(0).Border(wxTOP, 5));
	
	
	bSizer90->Add(bSizer100, 1, wxALL | wxEXPAND, 5);
	
	
	//bSizer84->Add(bSizer90, 0, wxEXPAND, 5);
	bSizer84->Add(bSizer90, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	/*
	wxBoxSizer* bSizer95;
	bSizer95 = new wxBoxSizer(wxHORIZONTAL);
	m_button18 = new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
	//bSizer95->Add (m_button18, 0, wxALL, 5);
	bSizer95->Add (m_button18, wxSizerFlags(0).Border(wxRIGHT,6));
	m_button19 = new wxButton(this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
	//bSizer95->Add(m_button19, 0, wxALL, 5);
	bSizer95->Add (m_button19, wxSizerFlags(0));
	
	//bSizer84->Add (bSizer95, 0, wxALIGN_RIGHT, 5);
	bSizer84->Add (bSizer95, wxSizerFlags(0).Right().Border(wxLEFT|wxBOTTOM|wxRIGHT, 12));
	*/
	CreateStandardButtons (bSizer84);
	
	this->SetSizer(bSizer84);
	this->Layout();
	
	this->Centre(wxBOTH);
}

	
Boolean CMReformatTransformDlg::DoDialog(
        UInt16* recommendNumberOfBitsPtr,
        SInt16 bandInterleaveSelection)
{
    Boolean OKFlag = FALSE;

    SInt16 returnCode;

    UInt16 item;


    // Make sure intialization has been completed.

    if (!m_initializedFlag)
        return (FALSE);

    m_bandInterleaveSelection = bandInterleaveSelection;

    returnCode = ShowModal();

    if (returnCode == wxID_OK) {
        // Get transform data code.									

        gReformatOptionsPtr->transformDataCode = m_transformCode;

        // Number to offset data values by.						

        gReformatOptionsPtr->adjustOffset = m_adjustOffset;
        
        // Number to multiply data values by.					
        
        gReformatOptionsPtr->adjustFactor = m_adjustFactor;
        
        // Number to divide data values by.						

        gReformatOptionsPtr->adjustDivisor = m_adjustDivisor;

        // Output data can be signed flag.									
        // Only valid for transform code of 1.								

        //		if (gReformatOptionsPtr->transformDataCode == kAdjustChannel)		
        //			gReformatOptionsPtr->signedOutputDataFlag = m_signedOutputDataFlag;

        //		else		// gReformatOptionsPtr->transformDataCode != kAdjustChannel 	
        //			gReformatOptionsPtr->signedOutputDataFlag = FALSE;

        // Get the list of principal component channels to create	
        // if transform code 'kCreatePCImage' has been selected.							

        if (gReformatOptionsPtr->transformDataCode == kCreatePCImage) {
            gReformatOptionsPtr->pcScaleFactor = m_scaleFactor;
            gReformatOptionsPtr->numberPCChannels = m_localActiveNumberFeatures;

            if (m_channelSelection == kAllMenuItem) {
                gReformatOptionsPtr->numberPCChannels = m_numberEigenvectors;
                for (item = 0; item < m_numberEigenvectors; item++)
                    gReformatOptionsPtr->pcChannelPtr[item] = item;

            } // end "if (m_channelSelection == kAllMenuItem)" 

        } // end "if (...->transformDataCode == kCreatePCImage)" 

        // Decode the numerator and denominator if transform code	
        // 2 has been selected.													

        if (gReformatOptionsPtr->transformDataCode == kTransformChannels) {
            item = 0;

            // Transform offset.										

            gReformatOptionsPtr->transformOffset = m_transformOffset;

            // Decode the denominator.
            // Decode the numerator.
            //		These have already been decoded in "DoDataExchange".

            // Estimate recommendation for minimum number bits needed.

            if (gReformatOptionsPtr->numberDenominatorTerms > 0) {
                if (m_transformFactor <= 10)
                    // This implies 32-bit real
                    m_minSelectedNumberBits = 32;

                else if (m_transformFactor <= 100)
                    m_minSelectedNumberBits = 8;

                else
                    m_minSelectedNumberBits = 16;

            } // end "if (...->numberDenominatorTerms > 0)"

            // Transform factor.										

            gReformatOptionsPtr->transformFactor = m_transformFactor;

            gReformatOptionsPtr->transformChannelPtrSetFlag = FALSE;

        } // end "if (...->transformDataCode == kTransformChannels)"													

        if (gReformatOptionsPtr->transformDataCode == kFunctionOfChannels) {
            // Get info for function of channels.										

            gReformatOptionsPtr->functionCode = m_functionCode + 1;
            gReformatOptionsPtr->functionFactor = m_functionFactor;
            gReformatOptionsPtr->kthSmallestValue = m_kthSmallestElement;
            
            printf("factor:%lf, kthsmall:%d, functionCode:%d\n",m_functionFactor, m_kthSmallestElement, m_functionCode);
        } // end "if (...->transformDataCode == kFunctionOfChannels)" 												

        if (gReformatOptionsPtr->transformDataCode == kAdjustChannelsByChannel) {
            // Get info for adjusting selected channels by a channel										

            gReformatOptionsPtr->transformAdjustSelectedChannelsFactor = m_adjustSelectedChannelsFactor;
            gReformatOptionsPtr->transformAdjustSelectedChannel = m_adjustSelectedChannel;
            
        } // end "if (...->transformDataCode == kFunctionOfChannels)" 

        gReformatOptionsPtr->eigenSource = m_eigenSource;
        
        *recommendNumberOfBitsPtr = m_minSelectedNumberBits;

        OKFlag = TRUE;

    } // end "if (returnCode == IDOK)"

    return (OKFlag);

} // end "DoDialog"


void CMReformatTransformDlg::OnAdjustSelectedChannels(wxCommandEvent& event)
{
	DialogPtr dialogPtr = this;
	
	
	ShowHideAdjustChannelItems(dialogPtr, kShow);
	ShowHideAdjustSelectedChannelsByChannelItems(dialogPtr, kHide);
	ShowHidePCTransformItems(dialogPtr, kHide, FALSE);
	ShowHideAlgebraicTransformItems(dialogPtr, kHide);
	ShowHideFunctionChannelsItems(dialogPtr, kHide, 0);
	
	// Set default text selection to first edit text item
	
	SelectDialogItemText(this, IDC_RT_AC_Offset, 0, SInt16_MAX);
	m_transformCode = kAdjustChannel;
	//this->Layout();
	//SetSizerAndFit(bSizer84);
	//this->Fit();
} // end "OnAdjustSelectedChannels"


void CMReformatTransformDlg::OnAdjustSelectedChannelsByChannel(wxCommandEvent& event)
{
	DialogPtr dialogPtr = this;
	
	
	ShowHideAdjustChannelItems(dialogPtr, kHide);
	ShowHideAdjustSelectedChannelsByChannelItems(dialogPtr, kShow);
	ShowHidePCTransformItems(dialogPtr, kHide, TRUE);
	ShowHideAlgebraicTransformItems(dialogPtr, kHide);
	ShowHideFunctionChannelsItems(dialogPtr, kHide, 0);
	
	// Set default text selection to first edit text item
	
	SelectDialogItemText(this, IDC_RT_ACbyC_Factor, 0, SInt16_MAX);
	m_transformCode = kAdjustChannelsByChannel;
	//SetSizerAndFit(bSizer84);
	//this->Layout();
	//this->Fit();
} // end "OnBnClickedAdjustSelectedChannelsByChannel"

/*
//Press keyboard "Enter"  -> wxID_OK
void CMReformatTransformDlg::OnButtonPress(wxKeyEvent& event)
{
	if (event.GetKeyCode() == WXK_RETURN) {
		if(Validate() && TransferDataFromWindow())
			this->EndModal(wxID_OK);
	} else {
		event.Skip();
	}
}
*/

void CMReformatTransformDlg::OnInitDialog(wxInitDialogEvent& event)
{
    CMDialog* dialogPtr = this;

    UInt16 selectedItem;


//    CMDialog::OnInitDialog(event);
    m_maxChannel = gImageWindowInfoPtr->totalNumberChannels;

    // Adjust data radio button.											
    // Hide related items until the box is checked.

    selectedItem = 0;

    m_transformCode = gReformatOptionsPtr->transformDataCode;

    // Adjust Selected Channels

    if (m_transformCode != kAdjustChannel)
        ShowHideAdjustChannelItems(dialogPtr, kHide);

    else // m_transformCode == kAdjustChannel 
        selectedItem = IDC_RT_AC_Offset;

    m_adjustOffset = gReformatOptionsPtr->adjustOffset;
   
    m_adjustFactor = gReformatOptionsPtr->adjustFactor;
    m_adjustDivisor = gReformatOptionsPtr->adjustDivisor;
    
    // Adjust Selected Channels by a Selected Channel

    if (m_transformCode != kAdjustChannelsByChannel)
        ShowHideAdjustSelectedChannelsByChannelItems(dialogPtr, kHide);

    else // m_transformCode == kAdjustChannel 
        selectedItem = IDC_RT_ACbyC_Factor;

    m_adjustSelectedChannelsFactor = gReformatOptionsPtr->transformAdjustSelectedChannelsFactor;
    m_adjustSelectedChannel = gReformatOptionsPtr->transformAdjustSelectedChannel;

    // Use transformation radio button.		

    EigenvectorInfoExists(kNoProject, &m_eigenSource, &m_numberEigenvectors);    
     
    m_scaleFactor = gReformatOptionsPtr->pcScaleFactor;
    if (m_numberEigenvectors > 0 && m_bandInterleaveSelection != kBSQ) {
        if (m_eigenSource == 1)
            MHideDialogItem(dialogPtr, IDC_RT_EV_FEEigenvectors);

        if (m_eigenSource == 2)
            MHideDialogItem(dialogPtr, IDC_RT_EV_PCEigenvectors);

        if (m_transformCode != kCreatePCImage)
            ShowHidePCTransformItems(dialogPtr, kHide, FALSE);

        else // m_transformCode == kCreatePCImage 
            selectedItem = IDC_EV_ScaleFactor;

        //		if (m_eigenSource == 2)
        //			m_scaleFactor = 10;

        m_minimumNumberBits = DeterminePCMinimumNumberBits(
                m_scaleFactor, m_eigenSource, 0);

        m_minSelectedNumberBits = DeterminePCMinimumNumberBits(
                m_scaleFactor, m_eigenSource, gReformatOptionsPtr->pcChannelPtr[0]);

    }// end "if ( m_numberEigenvectors > 0 && ...)" 

    else // m_numberEigenvectors <= 0 || m_bandInterleaveSelection == kBSQ
        ShowHidePCTransformItems(dialogPtr, kHide, TRUE);

    m_channelSelection = kAllMenuItem;
    m_localNumberFeatures = gReformatOptionsPtr->numberPCChannels;

    if (m_localNumberFeatures < m_numberEigenvectors)
        m_channelSelection = kSubsetMenuItem;

    m_localFeaturesPtr = (UInt16*) gReformatOptionsPtr->pcChannelPtr;

    // Set feature parameters 

    m_localActiveNumberFeatures = m_localNumberFeatures;
    m_localActiveFeaturesPtr = m_localFeaturesPtr;

    // Use general algebraic transformation radio button.					

    if (gImageFileInfoPtr->thematicType)
        MHideDialogItem(dialogPtr, IDC_RT_AlgebraicTransformation);

    if (m_transformCode != kTransformChannels)
        ShowHideAlgebraicTransformItems(dialogPtr, kHide);

    else // m_transformCode == kTransformChannels 
        selectedItem = IDC_RT_AT_Offset;

    if (!GetDefaultBandRatio(gImageWindowInfoPtr, gImageFileInfoPtr, gReformatOptionsPtr)) {
        gReformatOptionsPtr->transformFactor = 1.;
        gReformatOptionsPtr->transformOffset = 0.;

    } // end "if (!GetDefaultBandRatio (gImageWindowInfoPtr, ..."

    m_transformOffset = gReformatOptionsPtr->transformOffset;

    strncpy(m_numeratorStringPtr,
            (CharPtr) & gReformatOptionsPtr->numeratorString[1],
            gReformatOptionsPtr->numeratorString[0]);
    m_numeratorStringPtr[gReformatOptionsPtr->numeratorString[0]] = 0;

    strncpy(m_denominatorStringPtr,
            (CharPtr) & gReformatOptionsPtr->denominatorString[1],
            gReformatOptionsPtr->denominatorString[0]);
    m_denominatorStringPtr[gReformatOptionsPtr->denominatorString[0]] = 0;

    m_transformFactor = gReformatOptionsPtr->transformFactor;

    // Function of channels items

    m_functionCode = gReformatOptionsPtr->functionCode - 1;
    m_functionFactor = gReformatOptionsPtr->functionFactor;
    m_kthSmallestElement = gReformatOptionsPtr->kthSmallestValue;

    ShowHideFunctionChannelsItems(dialogPtr,
            m_transformCode == kFunctionOfChannels,
            gReformatOptionsPtr->functionCode);

    if (m_transformCode == kFunctionOfChannels) {
        selectedItem = IDC_FunctionFactor;
        if (m_functionCode == kFunctionKthSmallestElement - 1)
            selectedItem = IDC_kthSmallestElement;

    } // end "if (m_transformCode == kFunctionOfChannels)"

    // Make certain that all radio buttons are showing for now.

    MShowDialogItem(dialogPtr, IDC_AdjustSelectedChannelsbyChannel);
    MShowDialogItem(dialogPtr, IDC_RT_Eigenvectors);
    MShowDialogItem(dialogPtr, IDC_RT_AlgebraicTransformation);
    MShowDialogItem(dialogPtr, IDC_RT_FunctionOfChannels);

    //if (UpdateData(FALSE))
    if (TransferDataToWindow())
        PositionDialogWindow();

    // Update the radio buttons.

    if (m_numberEigenvectors <= 0 || m_bandInterleaveSelection == kBSQ)
        MHideDialogItem(dialogPtr, IDC_RT_Eigenvectors);

    if (gImageFileInfoPtr->thematicType)
        MHideDialogItem(dialogPtr, IDC_RT_AlgebraicTransformation);

    // Set default text selection to first edit text item 

    if (selectedItem > 0)
        SelectDialogItemText(this, selectedItem, 0, SInt16_MAX);

    //return FALSE; // return TRUE  unless you set the focus to a control

    //SetSizerAndFit(bSizer84);
    this->Fit();
    bSizer103->Layout();
} // end "OnInitDialog"


void CMReformatTransformDlg::OnRTAlgebraicTransformation(wxCommandEvent& event)
{
    DialogPtr dialogPtr = this;


    ShowHideAdjustChannelItems(dialogPtr, kHide);
    ShowHideAdjustSelectedChannelsByChannelItems(dialogPtr, kHide);
    ShowHidePCTransformItems(dialogPtr, kHide, FALSE);
    ShowHideAlgebraicTransformItems(dialogPtr, kShow);
    ShowHideFunctionChannelsItems(dialogPtr, kHide, 0);

    // Set default text selection to first edit text item 

    SelectDialogItemText(this, IDC_RT_AT_Offset, 0, SInt16_MAX);
    m_transformCode = kTransformChannels;
    //SetSizerAndFit(bSizer84);
    //this->Layout();
    //this->Fit();
} // end "OnRTAlgebraicTransformation"


void CMReformatTransformDlg::OnRTEigenvectors(wxCommandEvent& event)
{
    DialogPtr dialogPtr = this;


    ShowHideAdjustChannelItems(dialogPtr, kHide);
    ShowHideAdjustSelectedChannelsByChannelItems(dialogPtr, kHide);
    ShowHidePCTransformItems(dialogPtr, kShow, TRUE);
    ShowHideAlgebraicTransformItems(dialogPtr, kHide);
    ShowHideFunctionChannelsItems(dialogPtr, kHide, 0);

    // Set default text selection to first edit text item 

    SelectDialogItemText(this, IDC_EV_ScaleFactor, 0, SInt16_MAX);
    m_transformCode = kCreatePCImage;
    //SetSizerAndFit(bSizer84);
    //this->Layout();
    //this->Fit();
} // end "OnRTEigenvectors"


void CMReformatTransformDlg::OnRTNoTransformation(wxCommandEvent& event)
{
    DialogPtr dialogPtr = this;


    ShowHideAdjustChannelItems(dialogPtr, kHide);
    ShowHideAdjustSelectedChannelsByChannelItems(dialogPtr, kHide);
    ShowHidePCTransformItems(dialogPtr, kHide, FALSE);
    ShowHideAlgebraicTransformItems(dialogPtr, kHide);
    ShowHideFunctionChannelsItems(dialogPtr, kHide, 0);
    m_transformCode = kNoTransform;
    //SetSizerAndFit(bSizer84);
    //this->Layout();
    //this->Fit();
} // end "OnRTNoTransformation"


void CMReformatTransformDlg::OnRTFunctionOfChannels(wxCommandEvent& event)
{
    DialogPtr dialogPtr = this;


    ShowHideAdjustChannelItems(dialogPtr, kHide);
    ShowHideAdjustSelectedChannelsByChannelItems(dialogPtr, kHide);
    ShowHidePCTransformItems(dialogPtr, kHide, TRUE);
    ShowHideAlgebraicTransformItems(dialogPtr, kHide);
    ShowHideFunctionChannelsItems(dialogPtr, kShow, m_functionCode + 1);

    // Set default text selection to first edit text item 

    SelectDialogItemText(this, IDC_FunctionFactor, 0, SInt16_MAX);
    m_transformCode = kFunctionOfChannels;
    //SetSizerAndFit(bSizer84);
    //this->Layout();
    //this->Fit();
} // end "OnRTFunctionOfChannels"


void CMReformatTransformDlg::OnSelendokEVEigenvectors(wxCommandEvent& event)
{
	HandleChannelsMenu(IDC_EV_Eigenvectors,
							 kUseTransformation,
							 (SInt16) gReformatOptionsPtr->numberPCChannels,
							 2,
							 TRUE);
	m_transformCode = kCreatePCImage;
	//SetSizerAndFit(bSizer84);
	//this->Layout();
	//this->Fit();
} // end "OnSelendokEVEigenvectors"


void CMReformatTransformDlg::OnSelendokReformatFunctions(wxCommandEvent& event)
{
    Boolean showFlag = FALSE;


    // DDX_CBIndex(m_dialogFromPtr, IDC_ReformatFunctions, m_functionCode);
    wxComboBox* functionCode = (wxComboBox*) FindWindow(IDC_ReformatFunctions);
    m_functionCode = functionCode->GetSelection();

    if (m_functionCode == kFunctionKthSmallestElement - 1)
        showFlag = TRUE;

    ShowHideDialogItem(this, IDC_kthSmallestElementPrompt, showFlag);
    ShowHideDialogItem(this, IDC_kthSmallestElement, showFlag);
    m_transformCode = kFunctionOfChannels;
    //SetSizerAndFit(bSizer84);
    //this->Layout();
    //this->Fit();
} // end "OnSelendokReformatFunctions"


void CMReformatTransformDlg::ShowHideAdjustChannelItems(
																		  DialogPtr dialogPtr,
																		  Boolean showFlag)
{
	ShowHideDialogItem(dialogPtr, IDC_RT_AC_Equal, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_AC_Offset, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_AC_Plus1, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_AC_Factor, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_AC_OldValue, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_AC_Divisor, showFlag);
	
} // end "ShowHideAdjustChannelItems"


void CMReformatTransformDlg::ShowHideAdjustSelectedChannelsByChannelItems(
																								  DialogPtr dialogPtr,
																								  Boolean showFlag) {
	ShowHideDialogItem(dialogPtr, IDC_RT_ACbyC_OldValue, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_ACbyC_Factor, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_ACbyC_ChannelPrompt, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_ACbyC_ChannelNumber, showFlag);
	
} // end "ShowHideAdjustSelectedChannelsByChannelItems"


void CMReformatTransformDlg::ShowHideAlgebraicTransformItems(
																				 DialogPtr dialogPtr,
																				 Boolean showFlag)
{
	ShowHideDialogItem(dialogPtr, IDC_RT_AT_Equal, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_AT_Offset, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_AT_Plus, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_AT_Numerator, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_AT_Line, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_AT_Denominator, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_AT_Multiply, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_AT_Factor, showFlag);
	
} // end "ShowHideAlgebraicTransformItems"


void CMReformatTransformDlg::ShowHideFunctionChannelsItems(
																			  DialogPtr dialogPtr,
																			  Boolean showFlag,
																			  UInt16 functionChannelCode)
{
	ShowHideDialogItem(dialogPtr, IDC_ReformatFunctions, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_FunctionFactorPrompt, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_FunctionFactor, showFlag);
	
	if (functionChannelCode != kFunctionKthSmallestElement)
		showFlag = FALSE;
	
	ShowHideDialogItem(dialogPtr, IDC_kthSmallestElementPrompt, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_kthSmallestElement, showFlag);
	
} // end "ShowHideFunctionChannelsItems"


void CMReformatTransformDlg::ShowHidePCTransformItems(
																		DialogPtr dialogPtr,
																		Boolean showFlag,
																		Boolean pcButtonFlag) {
	/*
   if (pcButtonFlag) {
		//ShowHideDialogItem(dialogPtr, IDC_RT_Eigenvectors, showFlag);
		ShowHideDialogItem(dialogPtr, IDC_RT_EV_FEEigenvectors, showFlag);
		ShowHideDialogItem(dialogPtr, IDC_RT_EV_PCEigenvectors, showFlag);
		
	} // end "if (pcButtonFlag)"
	*/
	ShowHideDialogItem(dialogPtr, IDC_EV_Eigenvectors, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_EV_NumberBits, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_EV_AllComponents, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_RT_EV_SelComponents, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_ScaleFactorPrompt, showFlag);
	ShowHideDialogItem(dialogPtr, IDC_EV_ScaleFactor, showFlag);
   bSizer103->Layout();
	
} // end "ShowHidePCTransformItems"


bool CMReformatTransformDlg::TransferDataFromWindow() {
	
	SInt16 item, numberTerms;
	UInt16 stringID;
	Boolean algebraicItemFlag, eigenvectorItemFlag;
	
	wxTextCtrl* adjustDivisor = (wxTextCtrl*) FindWindow(IDC_RT_AC_Divisor);
	wxTextCtrl* adjustFactor = (wxTextCtrl*) FindWindow(IDC_RT_AC_Factor);
	wxTextCtrl* adjustOffset = (wxTextCtrl*) FindWindow(IDC_RT_AC_Offset);
	wxTextCtrl* transformFactor = (wxTextCtrl*) FindWindow(IDC_RT_AT_Factor);
	wxTextCtrl* transformOffset = (wxTextCtrl*) FindWindow(IDC_RT_AT_Offset);
	wxTextCtrl* denominatorString = (wxTextCtrl*) FindWindow(IDC_RT_AT_Denominator);
	wxTextCtrl* numeratorString = (wxTextCtrl*) FindWindow(IDC_RT_AT_Numerator);
	wxTextCtrl* scaleFactor = (wxTextCtrl*) FindWindow(IDC_EV_ScaleFactor);
	wxTextCtrl* functionFactor = (wxTextCtrl*) FindWindow(IDC_FunctionFactor);
	wxTextCtrl* kthSmallestElement = (wxTextCtrl*) FindWindow(IDC_kthSmallestElement);
	wxTextCtrl* adjustSelectedChannelsFactor = (wxTextCtrl*) FindWindow(IDC_RT_ACbyC_Factor);
	wxTextCtrl* adjustSelectedChannel = (wxTextCtrl*) FindWindow(IDC_RT_ACbyC_ChannelNumber);
	
	wxComboBox* channelSelection = (wxComboBox *) FindWindow(IDC_EV_Eigenvectors);
	wxComboBox* functionCode = (wxComboBox *) FindWindow(IDC_ReformatFunctions);
	wxStaticText* minimumNumberBits = (wxStaticText *) FindWindow(IDC_RT_EV_AllComponents);
	wxStaticText* minSelectedNumberBits = (wxStaticText *) FindWindow(IDC_RT_EV_SelComponents);
	
	wxRadioButton* notransform = (wxRadioButton *) FindWindow(IDC_RT_NoTransformation);
	wxRadioButton* adjustSelChan = (wxRadioButton *) FindWindow(IDC_AdjustSelectedChannels);
	wxRadioButton* adjustSelChanBySelChan = (wxRadioButton *) FindWindow(IDC_AdjustSelectedChannelsbyChannel);
	wxRadioButton* newChanFromEV = (wxRadioButton *) FindWindow(IDC_RT_Eigenvectors);
	wxRadioButton* newChanFromFunc = (wxRadioButton *) FindWindow(IDC_RT_FunctionOfChannels);
	wxRadioButton* NewChanFromGAlgebra = (wxRadioButton *) FindWindow(IDC_RT_AlgebraicTransformation);
	
	/*To gain renew values from users' inputs*/
	m_adjustDivisor = wxAtof(adjustDivisor->GetValue());
	m_adjustFactor = wxAtof(adjustFactor->GetValue());
	m_adjustOffset = wxAtof(adjustOffset->GetValue());
	m_transformFactor = wxAtof(transformFactor->GetValue());
	m_transformOffset = wxAtof(transformOffset->GetValue());
	
	m_denominatorString = denominatorString->GetValue();
	m_numeratorString = numeratorString->GetValue();
	
	m_scaleFactor = wxAtof(scaleFactor->GetValue());
	m_functionFactor = wxAtof(functionFactor->GetValue());
	m_kthSmallestElement = wxAtoi(kthSmallestElement->GetValue());
	m_adjustSelectedChannelsFactor = wxAtof(adjustSelectedChannelsFactor->GetValue());
	
	m_adjustSelectedChannel = wxAtoi(adjustSelectedChannel->GetValue());
	m_channelSelection = channelSelection->GetSelection();
	m_functionCode = functionCode->GetSelection();
	
	// the value of kFunctionOfChannels = 4
	if(m_transformCode == kTransformChannels)
	{
		item = 0;
		
		CtoPstring ((UCharPtr)m_denominatorStringPtr,
						(UCharPtr)gReformatOptionsPtr->denominatorString);
		
		numberTerms = DecodeAlgebraicFormula (
														  (unsigned char*)gReformatOptionsPtr->denominatorString,
														  gReformatOptionsPtr->coefficientsPtr,
														  gReformatOptionsPtr->transformChannelPtr,
														  gReformatOptionsPtr->transformOperatorPtr);
		if (numberTerms >= 0)
			gReformatOptionsPtr->numberDenominatorTerms = numberTerms;
		else		// numberTerms < 0)
			item = IDC_RT_AT_Denominator;
		// Decode the numerator
		if(item == 0)
		{
			CtoPstring ((UCharPtr)m_numeratorStringPtr,
							(UCharPtr)gReformatOptionsPtr->numeratorString);
			numberTerms = DecodeAlgebraicFormula (
															  (unsigned char*)gReformatOptionsPtr->numeratorString,
															  &gReformatOptionsPtr->coefficientsPtr[
																												 gReformatOptionsPtr->numberDenominatorTerms],
															  &gReformatOptionsPtr->transformChannelPtr[
																													  gReformatOptionsPtr->numberDenominatorTerms],
															  &gReformatOptionsPtr->transformOperatorPtr[
																														gReformatOptionsPtr->numberDenominatorTerms]);
			
			if (numberTerms >= 0)
				gReformatOptionsPtr->numberNumeratorTerms = numberTerms;
			else		// numberTerms < 0)
				item = IDC_RT_AT_Numerator;
		}
		if(item > 0)
		{
			stringID = IDS_Alert118 + abs(numberTerms) - 1;
			
			DisplayAlert (kErrorAlert2ID,
							  kStopAlert,
							  kAlertStrID,
							  stringID,
							  0,
							  NIL);
			
		}
	}
	
	return true;
}

bool CMReformatTransformDlg::TransferDataToWindow() {
	wxTextCtrl* adjustDivisor = (wxTextCtrl*) FindWindow(IDC_RT_AC_Divisor);
	wxTextCtrl* adjustFactor = (wxTextCtrl*) FindWindow(IDC_RT_AC_Factor);
	wxTextCtrl* adjustOffset = (wxTextCtrl*) FindWindow(IDC_RT_AC_Offset);
	wxTextCtrl* transformFactor = (wxTextCtrl*) FindWindow(IDC_RT_AT_Factor);
	wxTextCtrl* transformOffset = (wxTextCtrl*) FindWindow(IDC_RT_AT_Offset);
	wxTextCtrl* denominatorString = (wxTextCtrl*) FindWindow(IDC_RT_AT_Denominator);
	wxTextCtrl* numeratorString = (wxTextCtrl*) FindWindow(IDC_RT_AT_Numerator);
	wxTextCtrl* scaleFactor = (wxTextCtrl*) FindWindow(IDC_EV_ScaleFactor);
	wxTextCtrl* functionFactor = (wxTextCtrl*) FindWindow(IDC_FunctionFactor);
	wxTextCtrl* kthSmallestElement = (wxTextCtrl*) FindWindow(IDC_kthSmallestElement);
	wxTextCtrl* adjustSelectedChannelsFactor = (wxTextCtrl*) FindWindow(IDC_RT_ACbyC_Factor);
	wxTextCtrl* adjustSelectedChannel = (wxTextCtrl*) FindWindow(IDC_RT_ACbyC_ChannelNumber);
	wxComboBox* channelSelection = (wxComboBox*) FindWindow(IDC_EV_Eigenvectors);
	wxComboBox* functionCode = (wxComboBox*) FindWindow(IDC_ReformatFunctions);
	wxStaticText* minimumNumberBits = (wxStaticText*) FindWindow(IDC_RT_EV_AllComponents);
	wxStaticText* minSelectedNumberBits = (wxStaticText*) FindWindow(IDC_RT_EV_SelComponents);
	
	wxRadioButton* notransform = (wxRadioButton *) FindWindow(IDC_RT_NoTransformation);
	wxRadioButton* adjustSelChan = (wxRadioButton *) FindWindow(IDC_AdjustSelectedChannels);
	wxRadioButton* adjustSelChanBySelChan = (wxRadioButton *) FindWindow(IDC_AdjustSelectedChannelsbyChannel);
	wxRadioButton* newChanFromEV = (wxRadioButton *) FindWindow(IDC_RT_Eigenvectors);
	wxRadioButton* newChanFromFunc = (wxRadioButton *) FindWindow(IDC_RT_FunctionOfChannels);
	wxRadioButton* NewChanFromGAlgebra = (wxRadioButton *) FindWindow(IDC_RT_AlgebraicTransformation);
 
	
	switch (m_transformCode) {
		case kNoTransform:
			notransform->SetValue(true);
			break;
		case kAdjustChannel:
			adjustSelChan->SetValue(true);
			break;
		case kCreatePCImage:
			newChanFromEV->SetValue(true);
			break;
		case kTransformChannels:
			NewChanFromGAlgebra->SetValue(true);
			break;
		case kFunctionOfChannels:
			newChanFromFunc->SetValue(true);
			break;
		case kAdjustChannelsByChannel:
			adjustSelChanBySelChan->SetValue(true);
			break;
		default:
			notransform->SetValue(true);
			
			break;
	}
	
	adjustDivisor->ChangeValue(wxString::Format(wxT("%.1f"), m_adjustDivisor));
	adjustFactor->ChangeValue(wxString::Format(wxT("%.1f"), m_adjustFactor));
	adjustOffset->ChangeValue(wxString::Format(wxT("%.1f"), m_adjustOffset));
	
	transformFactor->ChangeValue(wxString::Format(wxT("%.1f"), m_transformFactor));
	transformOffset->ChangeValue(wxString::Format(wxT("%.1f"), m_transformOffset));
	denominatorString->ChangeValue(wxString::Format(wxT("%s"), m_denominatorStringPtr));
	numeratorString->ChangeValue(wxString::Format(wxT("%s"), m_numeratorStringPtr));
	scaleFactor->ChangeValue(wxString::Format(wxT("%.1f"), m_scaleFactor));
	functionFactor->ChangeValue(wxString::Format(wxT("%.1f"), m_functionFactor));
	kthSmallestElement->ChangeValue(wxString::Format(wxT("%li"), m_kthSmallestElement));
	adjustSelectedChannelsFactor->ChangeValue(wxString::Format(wxT("%.1f"), m_adjustSelectedChannelsFactor));
	adjustSelectedChannel->ChangeValue(wxString::Format(wxT("%li"), m_adjustSelectedChannel));
	
	channelSelection->SetSelection(m_channelSelection);
	functionCode->SetSelection(m_functionCode);
	
	minimumNumberBits->SetLabel(wxString::Format(wxT("%li"), m_minimumNumberBits));
	minSelectedNumberBits->SetLabel(wxString::Format(wxT("%li"), m_minSelectedNumberBits));
	
	return true;
}
