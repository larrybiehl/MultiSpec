// LEnhanceStatisticsDialog.cpp : implementation file
//
// Revised by Larry Biehl on 11/07/2019
/*
	Template for debug output to text window.
	
	int numberChars = sprintf ((char*)&gTextString3,
												" LEStaDlg::TransferDataToWindow: (m_hardThresholdCode, m_softThresholdCode): %d, %d%s", 
												m_hardThresholdCode,
												m_softThresholdCode,
												gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);	
*/	

#include "SMultiSpec.h"

#include "LEnhanceStatisticsDialog.h"
#include "LImage_dialog.cpp"

//#include	"SExternalGlobals.h"

		// Enhance Statistics constants												
	
#define	kUseNoThreshold					1
#define	kUseChiChiThreshold				2
#define	kUsePercentThreshold				3
#define	kUseFarthestPixelThreshold		4

extern void 		StatisticsEnhanceDialogInitialize (
							DialogPtr							dialogPtr,
							StatEnhanceSpecsPtr				statEnhanceSpecsPtr,
							DialogSelectAreaPtr				dialogSelectAreaPtr,
							UInt16*								localClassPtr,
							SInt16*								classSelectionPtr,
							UInt32*								localNumberClassesPtr,
							Boolean*								useEnhancedStatisticsFlagPtr,
							SInt16*								classWeightSetPtr,
							SInt16*								weightsSelectionPtr,
							SInt16*								softThresholdSelectionPtr,
							double*								softChiChiThresholdPtr,
							double*								softPercentThresholdPtr,
							SInt16*								hardThresholdSelectionPtr,
							double*								hardChiChiThresholdPtr,
							double*								hardPercentThresholdPtr,
							Boolean*								weightLabeledFlagPtr,
							double*								labelWeightPtr,
							SInt32*								iterationMaxPtr,
							SInt32*								iterationStopLengthPtr,
							double*								logLikeStopPercentPtr);
	
extern void 		StatisticsEnhanceDialogOK (
							StatEnhanceSpecsPtr				statEnhanceSpecsPtr,
							DialogSelectAreaPtr				dialogSelectAreaPtr,
							UInt16*								localClassPtr,
							SInt16								classSelection,
							UInt32								localNumberClasses,
							Boolean								useEnhancedStatisticsFlag,
							SInt16								weightsSelection,
							float*								classWeightsPtr,
							SInt16								softThresholdSelection,
							double								softChiChiThreshold,
							double								softPercentThreshold,
							SInt16								hardThresholdSelection,
							double								hardChiChiThreshold,
							double								hardPercentThreshold,
							Boolean								weightLabeledFlag,
							double								labelWeight,
							SInt32								iterationMax,
							SInt32								iterationStopLength,
							double								logLikeStopPercent);
	
extern void			StatisticsEnhanceDialogUpdateChiSquaredValue (
							DialogPtr							dialogPtr,
							SInt16								thresholdCode,
							SInt16								chiChiThresholdItemNumber,
							double								thresholdPercent,
							double*								chiChiThresholdPtr);
	
extern void 		StatisticsEnhanceDialogUpdateThresholdItems (
							DialogPtr							dialogPtr,
							SInt16								thresholdCode,
							SInt16								chiChiThresholdItemNumber,
							SInt16								percentThresholdPromptItemNumber,
							SInt16								percentThresholdItemNumber,
							double								thresholdPercent,
							double*								chiChiThresholdPtr);
		
extern void 		UpdateNumberLabeledSamples (
							DialogPtr							dialogPtr,
							SInt16								classSelection,
							SInt16*								classPtr,
							UInt32								numberClasses);
									
extern void			UpdateNumberUnlabeledSamples (
							DialogSelectAreaPtr				dialogSelectAreaPtr,
							DialogPtr							dialogPtr);

/////////////////////////////////////////////////////////////////////////////
// CMEnhanceStatisticsDialog dialog


CMEnhanceStatisticsDialog::CMEnhanceStatisticsDialog(wxWindow* pParent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
: CMDialog(CMEnhanceStatisticsDialog::IDD, pParent, title)
{
   m_logLikeStopPercent = 1;
	m_iterationStopLength = 1;
	m_iterationMax = 1;
	m_labelWeight = 1;
	m_useEnhancedStatisticsFlag = FALSE;
	m_weightLabeledFlag = FALSE;
	m_weightsSelection = 0;
	m_softChiChiThreshold = 0.0;
	m_hardChiChiThreshold = 0.0;
	m_softPercentThreshold = 0.0;
	m_hardPercentThreshold = 0.0;
	m_softThresholdCode = -1;
	m_hardThresholdCode = -1;
   saveSoftThresholdSelection = -1;
   
   m_classWeightSet = 0;

	m_updatingThresholdItemsFlag = TRUE;
	
	m_initializedFlag = CMDialog::m_initializedFlag; 
   
	if (m_initializedFlag)
		m_initializedFlag = GetDialogLocalVectors (
														NULL,
														NULL,     
														&m_classListPtr,
														NULL, 
														&m_classWeightsPtr,
														NULL,
														NULL,
														NULL);
   
   CreateControls();
   
} // end "CMEnhanceStatisticsDialog"


CMEnhanceStatisticsDialog::~CMEnhanceStatisticsDialog(void)
{  
   ReleaseDialogLocalVectors (NULL,
											NULL,        
											m_classListPtr,
											NULL,
											m_classWeightsPtr,
											NULL,
											NULL,
											NULL);

};		// end "~CMEnhanceStatisticsDialog"

BEGIN_EVENT_TABLE(CMEnhanceStatisticsDialog, CMDialog)
EVT_INIT_DIALOG(CMEnhanceStatisticsDialog::OnInitDialog)
EVT_CHECKBOX(IDC_WeightLabeledSamples, CMEnhanceStatisticsDialog::OnWeightLabeledSamples)
EVT_CHECKBOX(IDC_UseEnhancedStats, CMEnhanceStatisticsDialog::OnUseEnhancedStats)

EVT_COMBOBOX(IDC_HardThresholdCombo, CMEnhanceStatisticsDialog::OnSelchangeHardThresholdCombo)
EVT_COMBOBOX(IDC_SoftThresholdCombo, CMEnhanceStatisticsDialog::OnSelchangeSoftThresholdCombo)
EVT_COMBOBOX(IDC_ClassCombo, CMEnhanceStatisticsDialog::OnSelendokStatClassCombo)
EVT_COMBOBOX(IDC_WeightCombo, CMEnhanceStatisticsDialog::OnSelendokClassWeightsCombo)

EVT_COMBOBOX_DROPDOWN(IDC_ClassCombo, CMEnhanceStatisticsDialog::OnSelendokClassComboDropDown)
EVT_COMBOBOX_DROPDOWN(IDC_WeightCombo, CMEnhanceStatisticsDialog::OnSelendokClassWeightsComboDropDown)

EVT_TEXT(IDC_ColumnEnd, CMEnhanceStatisticsDialog::CheckStatColumnEnd)
EVT_TEXT(IDC_ColumnStart, CMEnhanceStatisticsDialog::CheckStatColumnStart)
EVT_TEXT(IDC_LineEnd, CMEnhanceStatisticsDialog::CheckStatLineEnd)
EVT_TEXT(IDC_LineStart, CMEnhanceStatisticsDialog::CheckStatLineStart)
EVT_TEXT(IDC_LineInterval, CMEnhanceStatisticsDialog::CheckStatLineInterval)
EVT_TEXT(IDC_ColumnInterval, CMEnhanceStatisticsDialog::CheckStatColumnInterval)

EVT_TEXT(IDC_HardChiChiThreshold, CMEnhanceStatisticsDialog::OnChangeHardChiChiThreshold)

EVT_TEXT(IDC_HardPercentThreshold, CMEnhanceStatisticsDialog::OnChangeHardPercentThreshold)
EVT_TEXT(IDC_SoftPercentThreshold, CMEnhanceStatisticsDialog::OnChangeSoftPercentThreshold)
EVT_TEXT(IDC_SoftChiChiThreshold, CMEnhanceStatisticsDialog::OnChangeSoftChiChiThreshold)


EVT_BUTTON(IDEntireImage, CMEnhanceStatisticsDialog::ToStatEntireImage)
EVT_BUTTON(IDSelectedImage, CMEnhanceStatisticsDialog::ToStatSelectedImage)

END_EVENT_TABLE()

/////////////////////////////////////////////////////////////////////////////
// CMEnhanceStatisticsDialog message handlers


void  CMEnhanceStatisticsDialog::CheckStatColumnEnd(wxCommandEvent& event)

{
	if (!m_settingSelectedEntireButton)
	{
		CMDialog::CheckColumnEnd(event);
		
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
		
	}		// end "if (!m_settingSelectedEntireButton)"
	
}		// end "CheckColumnEnd"


void CMEnhanceStatisticsDialog::CheckStatColumnInterval(wxCommandEvent& event)
{
	CMDialog::CheckColumnInterval(event);
	
	m_dialogSelectArea.columnInterval = m_ColumnInterval;
	UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
	
}		// end "CheckStatColumnInterval"


void  CMEnhanceStatisticsDialog::CheckStatColumnStart(wxCommandEvent& event)

{
	if (!m_settingSelectedEntireButton)
	{
		CMDialog::CheckColumnStart(event);
		
		m_dialogSelectArea.columnStart = m_ColumnStart;
		UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
		
	}		// end "if (!m_settingSelectedEntireButton)"
	
}		// end "CheckStatColumnStart"


void CMEnhanceStatisticsDialog::CheckStatLineEnd(wxCommandEvent& event)
{
	if (!m_settingSelectedEntireButton)
	{
		CMDialog::CheckLineEnd(event);
		
		m_dialogSelectArea.lineEnd = m_LineEnd;
		UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
		
	}		// end "if (!m_settingSelectedEntireButton)"
	
}		// end "CheckStatLineEnd"


void CMEnhanceStatisticsDialog::CheckStatLineInterval(wxCommandEvent& event)
{
	CMDialog::CheckLineInterval(event);
	
	m_dialogSelectArea.lineInterval = m_LineInterval;
	UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
	
}		// end "CheckStatLineInterval"


void CMEnhanceStatisticsDialog::CheckStatLineStart(wxCommandEvent& event)
{
	if (!m_settingSelectedEntireButton)
	{
		CMDialog::CheckLineStart(event);
		
		m_dialogSelectArea.lineStart = m_LineStart;
		UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
		
	}		// end "if (!m_settingSelectedEntireButton)"
	
}		// end "CheckLineStart"


void CMEnhanceStatisticsDialog::CreateControls()
{
	/*
	wxBitmap entireimi = wxBITMAP_PNG_FROM_DATA(entireim);
	wxBitmap toentirei = wxBITMAP_PNG_FROM_DATA(toentire);
	wxBitmap selectedi = wxBITMAP_PNG_FROM_DATA(selected);
	wxBitmap bmp4i = wxBITMAP_PNG_FROM_DATA(bmp4);
	*/
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	bSizer119 = new wxBoxSizer (wxVERTICAL);
	
	fgSizer15 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer15->SetFlexibleDirection (wxHORIZONTAL);
	fgSizer15->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	wxBoxSizer* bSizerL15;
	bSizerL15 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer247;
	bSizer247 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText249 = new wxStaticText( this, wxID_ANY, wxT("Unlabeled samples:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer247->Add (m_staticText249, 0, wxALIGN_CENTER_VERTICAL, 0);
	
	m_textCtrl170 = new wxStaticText( this, IDC_UnlabeledSamples, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_textCtrl170, IDS_ToolTip121);  
	bSizer247->Add (m_textCtrl170, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizerL15->Add( bSizer247, 0, wxEXPAND|wxBOTTOM, 5);
	
	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer (new wxStaticBox (this,
																		IDC_LineColFrame,
																		wxT("Unlabeled Samples Area"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL),
												wxHORIZONTAL);
	
	CreateLineColumnControls (sbSizer8);
	/*
	wxBoxSizer* bSizer791;
	bSizer791 = new wxBoxSizer(wxHORIZONTAL);
	
	m_bpButton4 = new wxBitmapButton(sbSizer8->GetStaticBox(), IDEntireImage, entireimi, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	m_bpButton4->SetBitmapDisabled(toentirei);
   SetUpToolTip(m_bpButton4, IDS_ToolTip40);
	//bSizer791->Add(m_bpButton4, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxALL, 5);
	bSizer791->Add(m_bpButton4, 0, wxALIGN_CENTER | wxALL, 5);
	m_bpButton5 = new wxBitmapButton(sbSizer8->GetStaticBox(), IDSelectedImage, selectedi, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	m_bpButton5->SetBitmapDisabled(bmp4i);
	//bSizer791->Add(m_bpButton5, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxALL, 5);
	bSizer791->Add(m_bpButton5, 0, wxALIGN_CENTER | wxALL, 5);
	sbSizer8->Add(bSizer791, 0, wxEXPAND, 5);
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer(3, 4, 0, 0);
	
	wxStaticText* m_staticText63;
	m_staticText63 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60,-1), 0);
	m_staticText63->Wrap(-1);
	gSizer1->Add(m_staticText63, 0, wxALL, 5);
	
	wxStaticText* m_staticText64;
	m_staticText64 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Start"), wxDefaultPosition, wxSize(65,-1), 0);
	m_staticText64->Wrap(-1);
	gSizer1->Add(m_staticText64, 0, wxALIGN_CENTER | wxALL, 5);
	
	wxStaticText* m_staticText65;
	m_staticText65 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("End"), wxDefaultPosition, wxSize(65,-1), 0);
	m_staticText65->Wrap(-1);
	gSizer1->Add(m_staticText65, 0, wxALIGN_CENTER | wxALL, 5);
	
	wxStaticText* m_staticText66;
	m_staticText66 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Interval"), wxDefaultPosition, wxSize(65,-1), 0);
	m_staticText66->Wrap(-1);
	gSizer1->Add(m_staticText66, 0, wxALIGN_CENTER | wxALL, 5);
	
	wxStaticText* m_staticText60;
	m_staticText60 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("  Line"), wxDefaultPosition, wxSize(60,-1), 0);
	m_staticText60->Wrap(-1);
	gSizer1->Add(m_staticText60, 0, wxALIGN_CENTER | wxALL, 5);
	
	wxTextCtrl* m_linestartctrl;
	m_linestartctrl = new wxTextCtrl(sbSizer8->GetStaticBox(), IDC_LineStart, wxEmptyString, wxDefaultPosition, wxSize(65,-1), 0);
	m_linestartctrl->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_LineStartString));
	
	gSizer1->Add(m_linestartctrl, 0, wxALIGN_CENTER | wxALL, 5);
	
	wxTextCtrl* m_lineendctrl;
	m_lineendctrl = new wxTextCtrl(sbSizer8->GetStaticBox(), IDC_LineEnd, wxEmptyString, wxDefaultPosition, wxSize(65,-1), 0);
	m_lineendctrl->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_LineEndString));
	
	gSizer1->Add(m_lineendctrl, 0, wxALIGN_CENTER | wxALL, 5);
	
	wxTextCtrl* m_lineintctrl;
	m_lineintctrl = new wxTextCtrl(sbSizer8->GetStaticBox(), IDC_LineInterval, wxEmptyString, wxDefaultPosition, wxSize(65,-1), 0);
	m_lineintctrl->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_LineIntervalString));
	
	gSizer1->Add(m_lineintctrl, 0, wxALIGN_CENTER | wxALL, 5);
	
	wxStaticText* m_staticText62;
	m_staticText62 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Column"), wxDefaultPosition, wxSize(60,-1), 0);
	m_staticText62->Wrap(-1);
	gSizer1->Add(m_staticText62, 0, wxALIGN_CENTER | wxALL, 5);
	
	wxTextCtrl* m_colstartctrl;
	m_colstartctrl = new wxTextCtrl(sbSizer8->GetStaticBox(), IDC_ColumnStart, wxEmptyString, wxDefaultPosition, wxSize(65,-1), 0);
	m_colstartctrl->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_ColumnStartString));
	
	gSizer1->Add(m_colstartctrl, 0, wxALIGN_CENTER | wxALL, 5);
	
	wxTextCtrl* m_colendctrl;
	m_colendctrl = new wxTextCtrl(sbSizer8->GetStaticBox(), IDC_ColumnEnd, wxEmptyString, wxDefaultPosition, wxSize(65,-1), 0);
	m_colendctrl->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_ColumnEndString));
	
	gSizer1->Add(m_colendctrl, 0, wxALIGN_CENTER | wxALL, 5);
	
	wxTextCtrl* m_colintctrl;
	m_colintctrl = new wxTextCtrl(sbSizer8->GetStaticBox(), IDC_ColumnInterval, wxEmptyString, wxDefaultPosition, wxSize(65,-1), 0);
	m_colintctrl->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_ColumnIntervalString));
	
   SetUpToolTip(m_linestartctrl, IDS_ToolTip19);
   SetUpToolTip(m_lineendctrl, IDS_ToolTip20);
   SetUpToolTip(m_lineintctrl, IDS_ToolTip21);
   SetUpToolTip(m_colstartctrl, IDS_ToolTip22);
   SetUpToolTip(m_colendctrl, IDS_ToolTip23);
   SetUpToolTip(m_colintctrl, IDS_ToolTip24);
   
	gSizer1->Add(m_colintctrl, 0, wxALIGN_CENTER | wxALL, 5);
	
	sbSizer8->Add(gSizer1, 1, wxEXPAND, 5);
	*/
	bSizerL15->Add (sbSizer8, wxSizerFlags(1).Expand().Border(wxTOP|wxBOTTOM,5));
	
	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer (new wxStaticBox (this,
																		wxID_ANY,
																		wxT("Unlabeled Samples Options"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL),
													wxVERTICAL);
	
	m_checkBox58 = new wxCheckBox( sbSizer9->GetStaticBox(), IDC_UseEnhancedStats, wxT("Use enhanced statistics for initial conditions             "), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox58, IDS_ToolTip122);  
	sbSizer9->Add (m_checkBox58, wxSizerFlags(0).Expand().Border(wxLEFT,5));
	
	wxBoxSizer* bSizer3001;
	bSizer3001 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText3071 = new wxStaticText( sbSizer9->GetStaticBox(), wxID_ANY, wxT("Outlier thresholds"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3001->Add( m_staticText3071, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText3081 = new wxStaticText( sbSizer9->GetStaticBox(), wxID_ANY, wxT("Chi*Chi"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3001->Add( m_staticText3081, 0, wxLEFT, 80 );
	
	sbSizer9->Add (bSizer3001, wxSizerFlags(0).Expand().Border(wxTOP|wxBOTTOM,5));
	
	wxBoxSizer* bSizer30011;
	bSizer30011 = new wxBoxSizer( wxHORIZONTAL );
	m_staticText30711 = new wxStaticText( sbSizer9->GetStaticBox(), wxID_ANY, wxT("Soft:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer30011->Add( m_staticText30711, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_bcomboBox1 = new wxComboBox( sbSizer9->GetStaticBox(), IDC_SoftThresholdCombo, wxT("Labeled pixel"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	m_bcomboBox1->Append("None");
	m_bcomboBox1->Append("Chi*Chi");
	m_bcomboBox1->Append("Percent");
	m_bcomboBox1->Append("Labeled pixel");
   SetUpToolTip(m_bcomboBox1, IDS_ToolTip123);  
	bSizer30011->Add( m_bcomboBox1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	
	m_textCtrl184 = new wxTextCtrl( sbSizer9->GetStaticBox(), IDC_SoftChiChiThreshold, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   m_textCtrl184->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_softChiThresString));
   SetUpToolTip(m_textCtrl184, IDS_ToolTip124);  
	bSizer30011->Add( m_textCtrl184, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	sbSizer9->Add (bSizer30011, wxSizerFlags(0).Expand().Border(wxTOP|wxBOTTOM,5));
	
	wxBoxSizer* bSizer30012;
	bSizer30012 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText30712 = new wxStaticText( sbSizer9->GetStaticBox(), IDC_SoftPercentThresholdPrompt, wxT("Percent:"), wxDefaultPosition, wxDefaultSize, 0 );
	SetUpToolTip(m_staticText30712, IDS_ToolTip125);  
   bSizer30012->Add (m_staticText30712, wxSizerFlags(0).ReserveSpaceEvenIfHidden().Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT,40));
	
	m_textCtrl185 = new wxTextCtrl( sbSizer9->GetStaticBox(), IDC_SoftPercentThreshold, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   m_textCtrl185->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_softpercentThresString));
   SetUpToolTip(m_textCtrl185, IDS_ToolTip125);  
	bSizer30012->Add (m_textCtrl185, wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxLEFT,5));

	sbSizer9->Add (bSizer30012, wxSizerFlags(0).Expand().ReserveSpaceEvenIfHidden().Border(wxTOP|wxBOTTOM,5));
	
	wxBoxSizer* bSizer300121;
	bSizer300121 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText307121 = new wxStaticText( sbSizer9->GetStaticBox(), wxID_ANY, wxT("Hard:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer300121->Add( m_staticText307121, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 20 );
	
	m_comboBox32 = new wxComboBox (sbSizer9->GetStaticBox(), IDC_HardThresholdCombo, wxT("Percent"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	m_comboBox32->Append("None");
	m_comboBox32->Append("Chi*Chi");
	m_comboBox32->Append("Percent");
   SetUpToolTip(m_comboBox32, IDS_ToolTip126);  
	bSizer300121->Add( m_comboBox32, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrl1851 = new wxTextCtrl( sbSizer9->GetStaticBox(), IDC_HardChiChiThreshold, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   m_textCtrl1851->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_hardchiThresString));
   SetUpToolTip(m_textCtrl1851, IDS_ToolTip127);  
	bSizer300121->Add( m_textCtrl1851, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	sbSizer9->Add (bSizer300121, wxSizerFlags(0).Expand().Border(wxTOP|wxBOTTOM,5));
	
	wxBoxSizer* bSizer3001211;
	bSizer3001211 = new wxBoxSizer(wxHORIZONTAL);
	
	m_staticText3071211 = new wxStaticText( sbSizer9->GetStaticBox(), IDC_HardPercentThresholdPrompt, wxT("Percent:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3001211->Add (m_staticText3071211, wxSizerFlags(0).ReserveSpaceEvenIfHidden().Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT,40));
	
	m_textCtrl18511 = new wxTextCtrl( sbSizer9->GetStaticBox(), IDC_HardPercentThreshold, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_textCtrl18511, IDS_ToolTip128);  
   m_textCtrl18511->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_hardpercentThresString));
	bSizer3001211->Add (m_textCtrl18511, wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxLEFT,5));

	sbSizer9->Add (bSizer3001211, wxSizerFlags(0).Expand().ReserveSpaceEvenIfHidden().Border(wxTOP|wxBOTTOM,5));
	
	bSizerL15->Add (sbSizer9, wxSizerFlags(0).Border(wxTOP,5));
	
	fgSizer15->Add (bSizerL15, wxSizerFlags(0));
	
	wxBoxSizer* bSizerR15;
	bSizerR15 = new wxBoxSizer(wxVERTICAL);
	
	wxBoxSizer* bSizer248;
	bSizer248 = new wxBoxSizer( wxHORIZONTAL );
	m_staticText250 = new wxStaticText (this, wxID_ANY, wxT("Labeled samples:"), wxDefaultPosition, wxDefaultSize, 0);
	SetUpToolTip(m_staticText250, IDS_ToolTip129);  
   bSizer248->Add (m_staticText250, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL));
	
	m_textCtrl171 = new wxStaticText (this, IDC_LabeledSamples, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_textCtrl171, IDS_ToolTip129);  
	bSizer248->Add (m_textCtrl171, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT,5));
	
	bSizerR15->Add (bSizer248, wxSizerFlags(0));
	
	wxBoxSizer* bSizer290;
	bSizer290 = new wxBoxSizer( wxHORIZONTAL );
	m_staticText326 = new wxStaticText( this, wxID_ANY, wxT("Classes:"), wxDefaultPosition, wxDefaultSize, 0 );
	SetUpToolTip(m_staticText326, IDS_ToolTip103);  
   bSizer290->Add (m_staticText326, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT,10));
	
	m_comboBox23 = new wxComboBox( this, IDC_ClassCombo, wxT("All"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	m_comboBox23->Append("All");
	m_comboBox23->Append("Subset...");
   SetUpToolTip(m_comboBox23, IDS_ToolTip103);  
	bSizer290->Add (m_comboBox23, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT,5));
	
	bSizerR15->Add (bSizer290, wxSizerFlags(1).Expand());
	
	wxBoxSizer* bSizer291;
	bSizer291 = new wxBoxSizer (wxHORIZONTAL);
	m_staticText327 = new wxStaticText( this, wxID_ANY, wxT("Class weights:"), wxDefaultPosition, wxDefaultSize, 0 );
	SetUpToolTip(m_staticText327, IDS_ToolTip130);
   bSizer291->Add (m_staticText327, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT,10));
	
	m_comboBox24 = new wxComboBox( this, IDC_WeightCombo, wxT("Equal"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	m_comboBox24->Append("Equal");
	m_comboBox24->Append("Unequal...");
   SetUpToolTip(m_comboBox24, IDS_ToolTip130);  
	bSizer291->Add (m_comboBox24, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT,5));
	
	bSizerR15->Add (bSizer291, wxSizerFlags(1).Expand());
	
	wxBoxSizer* bSizer292;
	bSizer292 = new wxBoxSizer( wxHORIZONTAL );
	m_checkBox59 = new wxCheckBox( this, IDC_WeightLabeledSamples, wxT("Weight labeled samples "), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox59, IDS_ToolTip131);  
	bSizer292->Add (m_checkBox59, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT,10));
	
	bSizerR15->Add (bSizer292, wxSizerFlags(1).Expand());
	
	wxBoxSizer* bSizer168;
	bSizer168 = new wxBoxSizer(wxHORIZONTAL);
	m_staticText188 = new wxStaticText( this, IDC_WeightingPrompt, wxT("weighting:"), wxDefaultPosition, wxDefaultSize, 0 );
	SetUpToolTip(m_staticText188, IDS_ToolTip132);  
   bSizer168->Add (m_staticText188, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).ReserveSpaceEvenIfHidden().Border(wxLEFT,30));
	
	m_textCtrl204 = new wxTextCtrl( this, IDC_Weighting, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   m_textCtrl204->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_weightString));
   SetUpToolTip(m_textCtrl204, IDS_ToolTip132);  
	bSizer168->Add (m_textCtrl204, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).ReserveSpaceEvenIfHidden().Border(wxLEFT,5));
	
	bSizerR15->Add (bSizer168, wxSizerFlags(1).Expand().Border(wxBOTTOM,5));
	
	wxBoxSizer* bSizer259;
	bSizer259 = new wxBoxSizer( wxVERTICAL );
	m_staticText278 = new wxStaticText( this, wxID_ANY, wxT("Iteration stop conditions"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer259->Add (m_staticText278, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,5));
	
	wxFlexGridSizer* fgSizer29;
	fgSizer29 = new wxFlexGridSizer (0, 2, 0, 0);
	fgSizer29->SetFlexibleDirection (wxHORIZONTAL);
	fgSizer29->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	m_staticText279 = new wxStaticText( this, wxID_ANY, wxT("Maximum number:"), wxDefaultPosition, wxDefaultSize, 0);
	SetUpToolTip(m_staticText279, IDS_ToolTip133);  
   fgSizer29->Add (m_staticText279, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT,20));
	
	m_textCtrl180 = new wxTextCtrl( this, IDC_MaximumNumber, wxEmptyString, wxDefaultPosition, wxSize(80,-1), 0);
   m_textCtrl180->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_maxNumberString));
   SetUpToolTip(m_textCtrl180, IDS_ToolTip133);  
	fgSizer29->Add (m_textCtrl180, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT|wxTOP|wxBOTTOM,5));
	
	m_staticText280 = new wxStaticText( this, wxID_ANY, wxT("Iteration length:"), wxDefaultPosition, wxDefaultSize, 0);
	SetUpToolTip(m_staticText280, IDS_ToolTip134);
   fgSizer29->Add (m_staticText280, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT,20));
	
	m_textCtrl181 = new wxTextCtrl( this, IDC_IterationLength, wxEmptyString, wxDefaultPosition, wxSize(80,-1), 0);
   m_textCtrl181->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_iterlengthString));
   SetUpToolTip(m_textCtrl181, IDS_ToolTip134);
	fgSizer29->Add (m_textCtrl181, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT|wxTOP|wxBOTTOM,5));
	
	m_staticText281 = new wxStaticText( this, wxID_ANY, wxT("Log like change(%):"), wxDefaultPosition, wxDefaultSize, 0);
	SetUpToolTip(m_staticText281, IDS_ToolTip135);
   fgSizer29->Add (m_staticText281, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT,20));
	
	m_textCtrl182 = new wxTextCtrl( this, IDC_LogLikeChange, wxEmptyString, wxDefaultPosition, wxSize(80,-1), 0);
   m_textCtrl182->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_loglikeString));
   SetUpToolTip(m_textCtrl182, IDS_ToolTip135);
	fgSizer29->Add (m_textCtrl182, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT|wxTOP|wxBOTTOM,5));
	
	bSizer259->Add (fgSizer29, wxSizerFlags(0).Border(wxBOTTOM,5));
	
	bSizerR15->Add (bSizer259, wxSizerFlags(0).Expand());
	/*
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxHORIZONTAL);
	
	m_sdbSizer6Cancel = new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer6->Add (m_sdbSizer6Cancel, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxRIGHT,5));
	
	m_sdbSizer6OK = new wxButton(this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer6->Add (m_sdbSizer6OK, wxSizerFlags(0).Border(wxLEFT|wxTOP,5));
	
	bSizerR15->Add (bSizer6, wxSizerFlags(1).Right().Border(wxTOP,6));
	*/
	fgSizer15->Add (bSizerR15, wxSizerFlags(1).Border(wxLEFT,15));
	
	bSizer119->Add (fgSizer15, wxSizerFlags(1).Border(wxLEFT|wxTOP|wxBOTTOM|wxRIGHT,12));
	
	wxSizer* standardButtonSizer = CreateButtonSizer (wxOK | wxCANCEL);
	bSizer119->Add (standardButtonSizer, wxSizerFlags(0).Right());
	
	this->SetSizer (bSizer119);
	//this->Layout();
	
}		// end "CreateControls"


void CMEnhanceStatisticsDialog::OnChangeHardChiChiThreshold(wxCommandEvent& event)
{
	if (!m_updatingThresholdItemsFlag)
		{
		m_updatingThresholdItemsFlag = TRUE;
		
		if (m_hardThresholdCode+1 != kUseChiChiThreshold)
			{
			m_hardThresholdCode = kUseChiChiThreshold - 1;
			
			StatisticsEnhanceDialogUpdateThresholdItems (
																		this,
																		m_hardThresholdCode + 1,
																		IDC_HardChiChiThreshold,
																		IDC_HardPercentThresholdPrompt,
																		IDC_HardPercentThreshold,
																		m_hardPercentThreshold,
																		&m_hardChiChiThreshold);
			
			wxComboBox* hardThresholdCombo = (wxComboBox *) FindWindow (IDC_HardThresholdCombo);
			hardThresholdCombo->SetSelection(m_hardThresholdCode);
			
			}		// end "if (m_usePercentThresholdFlag && ..."
		
		m_updatingThresholdItemsFlag = FALSE;
		
		}		// end "if (!m_updatingThresholdItemsFlag)"
	
}		// end "OnChangeHardChiChiThreshold"


void CMEnhanceStatisticsDialog::OnChangeHardPercentThreshold(wxCommandEvent& event)
{
	if (!m_updatingThresholdItemsFlag)
		{
		m_updatingThresholdItemsFlag = TRUE;
		
		wxTextCtrl* hardPercentThreshold = (wxTextCtrl *) FindWindow(IDC_HardPercentThreshold);
		hardPercentThreshold->GetValue().ToDouble(&m_hardPercentThreshold);
		
		StatisticsEnhanceDialogUpdateChiSquaredValue (
								 this,
								 (SInt16)(m_hardThresholdCode+1),
								 IDC_HardChiChiThreshold,
								 m_hardPercentThreshold,
								 &m_hardChiChiThreshold);
		
		m_updatingThresholdItemsFlag = FALSE;
		
		} // end "if (!m_updatingThresholdItemsFlag)"
	
} // end "OnChangeHardPercentThreshold"


void CMEnhanceStatisticsDialog::OnChangeSoftChiChiThreshold(wxCommandEvent& event)
{
	if (!m_updatingThresholdItemsFlag)
		{
		m_updatingThresholdItemsFlag = TRUE;
		
		if (m_softThresholdCode+1 != kUseChiChiThreshold)
			{
			m_softThresholdCode = kUseChiChiThreshold - 1;
			
			StatisticsEnhanceDialogUpdateThresholdItems (
																		this,
																		m_softThresholdCode + 1,
																		IDC_SoftChiChiThreshold,
																		IDC_SoftPercentThresholdPrompt,
																		IDC_SoftPercentThreshold,
																		m_softPercentThreshold,
																		&m_softChiChiThreshold);
			
			wxComboBox* softThresholdCombo = (wxComboBox *) FindWindow (IDC_SoftThresholdCombo);
			softThresholdCombo->SetSelection (m_softThresholdCode);
			
			}		// end "if (m_usePercentThresholdFlag && ..."
		
		m_updatingThresholdItemsFlag = FALSE;
		
		}		// end "if (!m_updatingThresholdItemsFlag)"
	
}		// end "OnChangeSoftChiChiThreshold"


void CMEnhanceStatisticsDialog::OnChangeSoftPercentThreshold(wxCommandEvent& event)
{
	if (!m_updatingThresholdItemsFlag)
		{
		m_updatingThresholdItemsFlag = TRUE;
		wxTextCtrl* softPercentThreshold = (wxTextCtrl *) FindWindow(IDC_SoftPercentThreshold);
		softPercentThreshold->GetValue().ToDouble(&m_softPercentThreshold);
		
		StatisticsEnhanceDialogUpdateChiSquaredValue (
																	 this,
																	 m_softThresholdCode+1,
																	 IDC_SoftChiChiThreshold,
																	 m_softPercentThreshold,
																	 &m_softChiChiThreshold);
		
		m_updatingThresholdItemsFlag = FALSE;
		
		} // end "if (!m_updatingThresholdItemsFlag)"
	
} // end "OnChangeSoftPercentThreshold"


SInt16 CMEnhanceStatisticsDialog::DoDialog(
				StatEnhanceSpecsPtr				statEnhanceSpecsPtr)
{
	SInt16								returnCode;
	
	Boolean								continueFlag = FALSE;
	
	
	
			// Make sure intialization has been completed.
	
	if (!m_initializedFlag)
		return(FALSE);
	
	m_statEnhanceSpecsPtr = statEnhanceSpecsPtr;
	
	returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{
		continueFlag = TRUE;
		
		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;
		
		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;
		
		StatisticsEnhanceDialogOK (m_statEnhanceSpecsPtr,
											&m_dialogSelectArea,
											m_classListPtr,
											m_classSelection,
											(SInt16)m_localNumberClasses,
											m_useEnhancedStatisticsFlag,
											m_weightsSelection+1,
											m_classWeightsPtr,
											m_softThresholdCode+1,
											m_softChiChiThreshold,
											m_softPercentThreshold,
											m_hardThresholdCode+1,
											m_hardChiChiThreshold,
											m_hardPercentThreshold,
											m_weightLabeledFlag,
											m_labelWeight,
											m_iterationMax,
											m_iterationStopLength,
											m_logLikeStopPercent);
		}
	
	return continueFlag;
	
}		// end "DoDialog"


void CMEnhanceStatisticsDialog::OnInitDialog(wxInitDialogEvent& event)
{
	SInt16      classSelection,
					hardThresholdSelection,
					softThresholdSelection,
					weightsSelection;
   
   
   wxDialog::OnInitDialog(event);
   
   		// Initialize dialog variables.
   
	m_updatingThresholdItemsFlag = TRUE;
	StatisticsEnhanceDialogInitialize (this,
													m_statEnhanceSpecsPtr,
													&m_dialogSelectArea,
													m_classListPtr,
													&classSelection,
													&m_localNumberClasses,
													(Boolean*)&m_useEnhancedStatisticsFlag,
													&m_classWeightSet,
													&weightsSelection,
													&softThresholdSelection,
													&m_softChiChiThreshold,
													&m_softPercentThreshold,
													&hardThresholdSelection,
													&m_hardChiChiThreshold,
													&m_hardPercentThreshold,
													(Boolean*)&m_weightLabeledFlag,
													&m_labelWeight,
													&m_iterationMax,
													&m_iterationStopLength,
													&m_logLikeStopPercent);
   
			// Selected area to use for non-labeled samples.
                  
	m_LineStart = m_dialogSelectArea.lineStart;
	m_LineEnd = m_dialogSelectArea.lineEnd;
	m_LineInterval = m_dialogSelectArea.lineInterval;
	m_ColumnStart = m_dialogSelectArea.columnStart;
	m_ColumnEnd = m_dialogSelectArea.columnEnd;
	m_ColumnInterval = m_dialogSelectArea.columnInterval; 	
						
			// Classes to use.
			
	m_classSelection = classSelection;
	
			// Class weights. Allow for 0 base, not 1 base as for Mac version				 
	
	m_weightsSelection = weightsSelection - 1;

			// Threshold codes.
   
	m_hardThresholdCode = hardThresholdSelection - 1;
   m_softThresholdCode = softThresholdSelection - 1;
	
	if (TransferDataToWindow())
		{
				// Now allow changes from the user for threshold items.
				
		m_updatingThresholdItemsFlag = FALSE;
      
				// Set default text selection to first edit text item

		SelectDialogItemText(this, IDC_LineStart, 0, SInt16_MAX);
			
		//this->Layout();
		SetSizerAndFit(bSizer119);
		this->Fit();
		PositionDialogWindow();
		}

}		// end "OnInitDialog"


void  CMEnhanceStatisticsDialog::OnSelchangeHardThresholdCombo(wxCommandEvent& event)
{
   wxComboBox* hardThresholdCode = (wxComboBox *) FindWindow(IDC_HardThresholdCombo);
   m_hardThresholdCode = hardThresholdCode->GetSelection();

	m_updatingThresholdItemsFlag = TRUE;

	StatisticsEnhanceDialogUpdateThresholdItems (
													this,
													(SInt16)(m_hardThresholdCode+1),
													IDC_HardChiChiThreshold,
													IDC_HardPercentThresholdPrompt,
													IDC_HardPercentThreshold,
													m_hardPercentThreshold,
													&m_hardChiChiThreshold);
   
	m_updatingThresholdItemsFlag = FALSE;
	
}		// end "OnSelchangeHardThresholdCombo"


void  CMEnhanceStatisticsDialog::OnSelchangeSoftThresholdCombo(wxCommandEvent& event)
{
   wxComboBox* softThresholdCode = (wxComboBox *) FindWindow (IDC_SoftThresholdCombo);
   m_softThresholdCode = softThresholdCode->GetSelection();
   
   m_updatingThresholdItemsFlag = TRUE;
   
	StatisticsEnhanceDialogUpdateThresholdItems (
													this,
													(SInt16)(m_softThresholdCode+1),
													IDC_SoftChiChiThreshold,
													IDC_SoftPercentThresholdPrompt,
													IDC_SoftPercentThreshold,
													m_softPercentThreshold,
													&m_softChiChiThreshold);
   
   wxTextCtrl* softChiChiThreshold = (wxTextCtrl *) FindWindow(IDC_SoftChiChiThreshold);
   softChiChiThreshold->GetValue().ToDouble(&m_softChiChiThreshold);
   
   m_updatingThresholdItemsFlag = FALSE;   
   
} // end "OnSelchangeSoftThresholdCombo"


void CMEnhanceStatisticsDialog::OnSelendokClassWeightsCombo(wxCommandEvent& event)
{                                                           
	HandleClassWeightsMenu ((SInt16)m_localNumberClasses, 
									(SInt16*)m_classListPtr,
									m_classWeightsPtr,
									gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
									IDC_WeightCombo,
									&m_weightsSelection);
	
}		// end "OnSelendokClassWeightsCombo"


//void CMEnhanceStatisticsDialog::OnSelendokClassWeightsDropDown(wxCommandEvent& event) 
//{
//	wxComboBox* classWeightsCombo = (wxComboBox *) FindWindow(IDC_WeightCombo);
//	classWeightsCombo->SetSelection(0);
//	
//} // end "OnSelendokClassWeightsDropDown"


void CMEnhanceStatisticsDialog::OnSelendokStatClassCombo(wxCommandEvent& event)
{
	SInt16			classSelection;
	
	CMDialog::OnSelendokClassCombo(event);
	
	if (m_classSelection == kAllMenuItem)
		{
				// Make sure that the class settings are setup for all
				// classes in case they will be used by the symbols
				// or weights popup menu.
				
		classSelection = m_classSelection;
										
		LoadTrainClassVector (&m_localNumberClasses,
										&classSelection, 
										(SInt16*)m_classListPtr);
										
		m_classSelection = classSelection;
										
		}		// end "if (gClassSelection == kAllMenuItem)"
	
	UpdateNumberLabeledSamples (this,
											m_classSelection,
											(SInt16*)m_classListPtr,
											m_localNumberClasses);
	
}		// end "OnSelendokStatClassCombo"

/*
void CMEnhanceStatisticsDialog::OnSelendokStatClassDropDown(wxCommandEvent& event) 
{
	wxComboBox* classcombo = (wxComboBox *) FindWindow(IDC_ClassCombo);
	classcombo->SetSelection(-1);
	
} // end "OnSelendokStatClassDropDown"
*/

void CMEnhanceStatisticsDialog::OnUseEnhancedStats(wxCommandEvent& event)
{	
	wxCheckBox* useEnhancedStatisticsFlag = (wxCheckBox *) FindWindow(IDC_UseEnhancedStats);
	
	m_useEnhancedStatisticsFlag = useEnhancedStatisticsFlag->GetValue();
	
	m_weightsSelection = UpdateDialogClassWeightsInfo (
																		(SInt16)m_weightsSelection,
																		m_useEnhancedStatisticsFlag,
																		&m_classWeightSet,
																		FALSE);
	
	wxComboBox* weightsSelection = (wxComboBox *) FindWindow(IDC_WeightCombo);
	m_weightsSelection = weightsSelection->GetSelection();
		
}		// end "OnUseEnhancedStats"


void CMEnhanceStatisticsDialog::OnWeightLabeledSamples(wxCommandEvent& event)
{
	
	wxCheckBox* weightLabeledFlag = (wxCheckBox *) FindWindow(IDC_WeightLabeledSamples);
	m_weightLabeledFlag = weightLabeledFlag->GetValue();
	
	ShowHideDialogItem (this, IDC_WeightingPrompt, m_weightLabeledFlag);
	ShowHideDialogItem (this, IDC_Weighting, m_weightLabeledFlag);
	
}		// end "OnWeightLabeledSamples"


void CMEnhanceStatisticsDialog::ToStatEntireImage(wxCommandEvent& event)

{
	CMDialog::ToEntireImage(event);
	
	m_dialogSelectArea.lineStart = m_LineStart;
	m_dialogSelectArea.lineEnd = m_LineEnd;
	m_dialogSelectArea.columnStart = m_ColumnStart;
	m_dialogSelectArea.columnEnd = m_ColumnEnd;
	UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
	
}		// end "ToEntireImage"


void  CMEnhanceStatisticsDialog::ToStatSelectedImage(wxCommandEvent& event)

{
	CMDialog::ToSelectedImage(event);
	
	m_dialogSelectArea.lineStart = m_LineStart;
	m_dialogSelectArea.lineEnd = m_LineEnd;
	m_dialogSelectArea.columnStart = m_ColumnStart;
	m_dialogSelectArea.columnEnd = m_ColumnEnd;
	UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
	
}		// end "ToSelectedImage"


bool CMEnhanceStatisticsDialog::TransferDataFromWindow()
{
	SInt16			returnCode = 0;
//	returnCode = TRUE;
	
	wxTextCtrl* lineStart = (wxTextCtrl*) FindWindow(IDC_LineStart);
	wxTextCtrl* lineEnd = (wxTextCtrl*) FindWindow(IDC_LineEnd);
	wxTextCtrl* columnStart = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
	wxTextCtrl* columnEnd = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
	wxTextCtrl* lineInterval = (wxTextCtrl*) FindWindow(IDC_LineInterval);
	wxTextCtrl* columnInterval = (wxTextCtrl*) FindWindow(IDC_ColumnInterval);
	wxTextCtrl* softChiChiThreshold = (wxTextCtrl*) FindWindow(IDC_SoftChiChiThreshold);
	wxTextCtrl* softPercentThreshold = (wxTextCtrl*) FindWindow(IDC_SoftPercentThreshold);
	wxTextCtrl* hardChiChiThreshold = (wxTextCtrl*) FindWindow(IDC_HardChiChiThreshold);
	wxTextCtrl* hardPercentThreshold = (wxTextCtrl*) FindWindow(IDC_HardPercentThreshold);
	wxTextCtrl* labelWeight = (wxTextCtrl*) FindWindow(IDC_Weighting);
	wxTextCtrl* iterationMax = (wxTextCtrl*) FindWindow(IDC_MaximumNumber);
	wxTextCtrl* iterationStopLength = (wxTextCtrl*) FindWindow(IDC_IterationLength);
	wxTextCtrl* logLikeStopPercent = (wxTextCtrl*) FindWindow(IDC_LogLikeChange);
	
	wxCheckBox* useEnhancedStatisticsFlag = (wxCheckBox*) FindWindow(IDC_UseEnhancedStats);
	wxCheckBox* weightLabeledFlag = (wxCheckBox*) FindWindow(IDC_WeightLabeledSamples);
	
	wxComboBox* classSelection = (wxComboBox*) FindWindow(IDC_ClassCombo);
	wxComboBox* weightSelection = (wxComboBox*) FindWindow(IDC_WeightCombo);
	wxComboBox* softThresholdCode = (wxComboBox*) FindWindow(IDC_SoftThresholdCombo);
	wxComboBox* hardThresholdCode = (wxComboBox*) FindWindow(IDC_HardThresholdCombo);
	
	wxString m_softChiChiThresholdString;
	wxString m_softPercentThresholdString;
	wxString m_hardChiChiThresholdString;
	wxString m_hardPercentThresholdString;
	wxString m_labelWeightString;
	wxString m_iterationMaxString;
	wxString m_iterationStopLengthString;
	wxString m_logLikeStopPercentString;
	
	m_LineStartString = lineStart->GetValue();
	m_LineStart = wxAtoi(m_LineStartString);
	
	m_LineEndString = lineEnd->GetValue();
	m_LineEnd = wxAtoi(m_LineEndString);
	
	m_LineIntervalString = lineInterval->GetValue();
	m_LineInterval = wxAtoi(m_LineIntervalString);
	
	m_ColumnStartString = columnStart->GetValue();
	m_ColumnStart = wxAtoi(m_ColumnStartString);
	
	m_ColumnEndString = columnEnd->GetValue();
	m_ColumnEnd = wxAtoi(m_ColumnEndString);
	
	m_ColumnIntervalString = columnInterval->GetValue();
	m_ColumnInterval = wxAtoi(m_ColumnIntervalString);
	
	m_useEnhancedStatisticsFlag = useEnhancedStatisticsFlag->GetValue();
	
	m_softThresholdCode = softThresholdCode->GetSelection();
	m_softChiChiThresholdString = softChiChiThreshold->GetValue();
	if (m_softThresholdCode == 1)
		m_softChiChiThreshold = wxAtof(m_softChiChiThresholdString);
	
	m_softPercentThresholdString = softPercentThreshold->GetValue();
	m_softPercentThreshold = wxAtof(m_softPercentThresholdString);
	
	m_hardThresholdCode = hardThresholdCode->GetSelection();
	m_hardChiChiThresholdString = hardChiChiThreshold->GetValue();
	if (m_hardThresholdCode == 1)
		m_hardChiChiThreshold = wxAtof(m_hardChiChiThresholdString);
		
	m_hardPercentThresholdString = hardPercentThreshold->GetValue();
	m_hardPercentThreshold = wxAtof(m_hardPercentThresholdString);
	
	m_classSelection = classSelection->GetSelection();
	m_weightsSelection = weightSelection->GetSelection();
	
   if(m_classSelection < 0)
      m_classSelection = m_classSelection_Saved;
   if(m_weightsSelection < 0)
      m_weightsSelection = m_weightSelection_Saved;
   
	m_weightLabeledFlag = weightLabeledFlag->GetValue();
	m_labelWeightString = labelWeight->GetValue();
	m_labelWeight = wxAtof(m_labelWeightString);
	
	m_iterationMaxString = iterationMax->GetValue();
	m_iterationMax = wxAtoi(m_iterationMaxString);
	
	m_iterationStopLengthString = iterationStopLength->GetValue();
	m_iterationStopLength = wxAtoi(m_iterationStopLengthString);
	
	m_logLikeStopPercentString = logLikeStopPercent->GetValue();
	m_logLikeStopPercent = wxAtof(m_logLikeStopPercentString);
	
   returnCode = CheckDialogRealValue (this,
													IDC_LogLikeChange,
													1.e-006, 
                                       10.,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);

   if(returnCode == 0)
         returnCode = CheckDialogRealValue (this,
													IDC_IterationLength,
													1, 
                                       200,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
    if(returnCode == 0)
         returnCode = CheckDialogRealValue (this,
													IDC_Weighting,
													1.e-005, 
                                       1000.,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);     
     if(returnCode == 0)
         returnCode = CheckDialogRealValue (this,
													IDC_SoftPercentThreshold,
													0., 
                                       100.,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);   
     if(returnCode == 0)
         returnCode = CheckDialogRealValue (this,
													IDC_HardPercentThreshold,
													0., 
                                       100.,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);  
 
      if(returnCode == 0)
         returnCode = VerifyLineColumnValues (IDC_LineStart,
														IDC_ColumnStart,
														true);
													
	return (returnCode == 0);
	
}		// end "TransferDataFromWindow"


bool CMEnhanceStatisticsDialog::TransferDataToWindow()
{
	SInt16			returnCode;
	returnCode = TRUE;
	
	wxTextCtrl* lineStart = (wxTextCtrl*) FindWindow(IDC_LineStart);
	wxTextCtrl* lineEnd = (wxTextCtrl*) FindWindow(IDC_LineEnd);
	wxTextCtrl* columnStart = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
	wxTextCtrl* columnEnd = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
	wxTextCtrl* lineInterval = (wxTextCtrl*) FindWindow(IDC_LineInterval);
	wxTextCtrl* columnInterval = (wxTextCtrl*) FindWindow(IDC_ColumnInterval);
	wxTextCtrl* softChiChiThreshold = (wxTextCtrl*) FindWindow(IDC_SoftChiChiThreshold);
	wxTextCtrl* softPercentThreshold = (wxTextCtrl*) FindWindow(IDC_SoftPercentThreshold);
	wxTextCtrl* hardChiChiThreshold = (wxTextCtrl*) FindWindow(IDC_HardChiChiThreshold);
	wxTextCtrl* hardPercentThreshold = (wxTextCtrl*) FindWindow(IDC_HardPercentThreshold);
	wxTextCtrl* labelWeight = (wxTextCtrl*) FindWindow(IDC_Weighting);
	wxTextCtrl* iterationMax = (wxTextCtrl*) FindWindow(IDC_MaximumNumber);
	wxTextCtrl* iterationStopLength = (wxTextCtrl*) FindWindow(IDC_IterationLength);
	wxTextCtrl* logLikeStopPercent = (wxTextCtrl*) FindWindow(IDC_LogLikeChange);
	
	wxCheckBox* useEnhancedStatisticsFlag = (wxCheckBox*) FindWindow(IDC_UseEnhancedStats);
	wxCheckBox* weightLabeledFlag = (wxCheckBox*) FindWindow(IDC_WeightLabeledSamples);
	
	wxComboBox* classSelection = (wxComboBox*) FindWindow(IDC_ClassCombo);
	wxComboBox* weightSelection = (wxComboBox*) FindWindow(IDC_WeightCombo);
	wxComboBox* softThresholdCode = (wxComboBox*) FindWindow(IDC_SoftThresholdCombo);
	wxComboBox* hardThresholdCode = (wxComboBox*) FindWindow(IDC_HardThresholdCombo);
	
	lineStart->ChangeValue(wxString::Format(wxT("%d"), m_LineStart ));
	lineEnd->ChangeValue(wxString::Format(wxT("%d"), m_LineEnd ));
	columnStart->ChangeValue(wxString::Format(wxT("%d"), m_ColumnStart ));
	columnEnd->ChangeValue(wxString::Format(wxT("%d"), m_ColumnEnd ));
	lineInterval->ChangeValue(wxString::Format(wxT("%d"), m_LineInterval ));
	columnInterval->ChangeValue(wxString::Format(wxT("%d"), m_ColumnInterval ));
	
	softChiChiThreshold->SetValue(wxString::Format(wxT("%.4f"), m_softChiChiThreshold));
	softPercentThreshold->SetValue(wxString::Format(wxT("%.g"), m_softPercentThreshold ));
	hardChiChiThreshold->SetValue(wxString::Format(wxT("%.4f"), m_hardChiChiThreshold ));
	hardPercentThreshold->SetValue(wxString::Format(wxT("%.g"), m_hardPercentThreshold ));
	labelWeight->SetValue(wxString::Format(wxT("%.2f"), m_labelWeight ));
	iterationMax->SetValue(wxString::Format(wxT("%d"), m_iterationMax));
	iterationStopLength->SetValue(wxString::Format(wxT("%d"), m_iterationStopLength ));
	logLikeStopPercent->SetValue(wxString::Format(wxT("%.g"), m_logLikeStopPercent ));
	
	classSelection->SetSelection(m_classSelection);
	weightSelection->SetSelection(m_weightsSelection);
	softThresholdCode->SetSelection(m_softThresholdCode);
	hardThresholdCode->SetSelection(m_hardThresholdCode);
	
	useEnhancedStatisticsFlag->SetValue(m_useEnhancedStatisticsFlag);
	weightLabeledFlag->SetValue(m_weightLabeledFlag);
	
	return (returnCode);
	
}		// end "TransferDataToWindow"
