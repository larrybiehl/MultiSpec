// LFeatureExtractionDialog.cpp : implementation file
//     
// Revised by Larry Biehl on 11/13/2018
						//
#include "SMultiSpec.h"
                    
#include "LChannelsDialog.h"  
#include "LFeatureExtractionDialog.h"
#include "LProjectionPursuitDialog.h"

//#include	"SExternalGlobals.h" 

extern void 		FeatureExtractionDialogAlgorithm (
							DialogPtr							dialogPtr, 
							SInt16								algorithmCode);

extern void 		FeatureExtractionDialogInitialize (
							DialogPtr							dialogPtr,
							FeatureExtractionSpecsPtr		featureExtractionSpecsPtr,
							UInt16*								localFeaturesPtr,
							UInt16*								localClassPtr,
							SInt16**								localClassPairWeightsListPtr,
							SInt16*								algorithmCodePtr,
							SInt16*								weightsSelectionPtr,
							SInt16*								interClassWeightsSelectionPtr,
							SInt16*								defaultClassPairWeightPtr,
							Boolean*								specialOptionsFlagPtr,
							double*								outlierThreshold1Ptr,
							double*								outlierThreshold2Ptr,
							UInt32*								minimumThresholdNumberPtr,
							Boolean*								optimizeClassFlagPtr,
							double*								optimizeThresholdPtr,
							UInt32*								maxPixelsPerClassPtr,
							SInt16*								channelSelectionPtr,
							UInt16*								localNumberChannelsPtr,
							SInt16*								channelListTypePtr,
							SInt16*								classSelectionPtr,
							UInt32*								localNumberClassesPtr,
							SInt16*								preprocessSelectionPtr,
							Boolean*								listTransformationFlagPtr );
	
extern void 		FeatureExtractionDialogOK (
							FeatureExtractionSpecsPtr		featureExtractionSpecsPtr,
							SInt16								algorithmCode,
							SInt16								weightsSelection,
							float*								localClassWeightsPtr,
							SInt16								interClassWeightsSelection,
							SInt16*								localClassPairWeightsListPtr,
							SInt16								localDefaultClassPairWeight,
							Boolean								specialOptionsFlag,
							double								outlierThreshold1,
							double								outlierThreshold2,
							UInt32								minThresholdNumber,
							double								optimizeThreshold,
							Boolean								optimizeClassFlag,
							UInt32								maxPixelsPerClass,
							SInt16								channelSelection,
							UInt16								localNumberFeatures,
							UInt16*								localFeaturesPtr,
							SInt16								classSelection,
							UInt32								localNumberClasses,
							UInt16*								localClassPtr,
							Boolean								listTransformationFlag,
							SInt16								preprocessSelection );
	
extern void 		FeatureExtractionDialogOptimizeClass (
							DialogPtr							dialogPtr,
							Boolean								optimizeClassFlag);
	
extern void 		FeatureExtractionDialogUpdateSpecialOptions (
							DialogPtr							dialogPtr,
							SInt16								algorithmCode,
							Boolean								specialOptionsFlag,
							Boolean								optimizeClassFlag );

extern Boolean 	ProjectionPursuitDialog (void);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMFeatureExtractionDialog dialog


CMFeatureExtractionDialog::CMFeatureExtractionDialog(wxWindow* pParent, wxWindowID id, const wxString& title)
	: CMDialog(CMFeatureExtractionDialog::IDD, pParent,title)
{
	//{{AFX_DATA_INIT(CMFeatureExtractionDialog)
	m_interclassThrehold = (float)0.;
	m_withinClassThreshold = (float)0.;
	m_optimizeClassesFlag = FALSE;
	m_minThresholdNumber = 0;
	m_maxPixelsPerClass = 0;
	m_specialOptionsFlag = FALSE;
	m_listTransformationFlag = FALSE;
	m_interClassWeightsSelection = -1;
	m_preprocessSelection = -1;
	m_algorithmCode = -1;
	m_optimizeThreshold = (float)0.;
	//}}AFX_DATA_INIT
	
	m_localClassPairWeightsListPtr = NULL;
	m_localDefaultClassPairWeight = 0;
	
	m_initializedFlag = CMDialog::m_initializedFlag; 
	
	if (m_initializedFlag)
		m_initializedFlag = GetDialogLocalVectors (
														&m_localActiveFeaturesPtr,
														NULL,     
														&m_classListPtr,
														NULL, 
														&m_classWeightsPtr,
														NULL,
														NULL,
														&m_localClassPairWeightsListPtr);
   
   CreateControls();
	
}	// end "CMFeatureExtractionDialog"



CMFeatureExtractionDialog::~CMFeatureExtractionDialog(void)

{  
	ReleaseDialogLocalVectors (m_localActiveFeaturesPtr,  
											NULL,       
											m_classListPtr,
											NULL,
											m_classWeightsPtr,
											NULL,
											NULL,
											m_localClassPairWeightsListPtr);
	
}		// end "CMFeatureExtractionDialog"


 
BEGIN_EVENT_TABLE(CMFeatureExtractionDialog, CMDialog)
   EVT_INIT_DIALOG(CMFeatureExtractionDialog::OnInitDialog)
	EVT_CHECKBOX(IDC_OptimizeClasses, CMFeatureExtractionDialog::OnOptimizeClasses)
	EVT_CHECKBOX(IDC_SpecialOptions, CMFeatureExtractionDialog::OnSpecialOptions)
	EVT_COMBOBOX(IDC_ChannelCombo, CMFeatureExtractionDialog::OnSelendokChannelCombo)
	EVT_COMBOBOX(IDC_ClassCombo, CMFeatureExtractionDialog::OnSelendokClassCombo)      
	EVT_COMBOBOX(IDC_WeightsCombo, CMFeatureExtractionDialog::OnSelendokClassWeightsCombo)
	EVT_COMBOBOX(IDC_ClassPairWeightsCombo, CMFeatureExtractionDialog::OnSelendokClassPairWeightsCombo)
	EVT_COMBOBOX(IDC_FEAlgorithmCombo, CMFeatureExtractionDialog::OnDecisionBoundary)
   EVT_COMBOBOX_DROPDOWN(IDC_PreprocessingCombo, CMFeatureExtractionDialog::OnCbnSelendokPreprocessingDropDown)
	EVT_COMBOBOX(IDC_PreprocessingCombo, CMFeatureExtractionDialog::OnCbnSelendokPreprocessingcombo)
   EVT_COMBOBOX_DROPDOWN(IDC_ChannelCombo, CMFeatureExtractionDialog::OnSelendokChannelComboDropDown)
	EVT_COMBOBOX_DROPDOWN(IDC_ClassCombo, CMFeatureExtractionDialog::OnSelendokClassComboDropDown)      
	EVT_COMBOBOX_DROPDOWN(IDC_WeightsCombo, CMFeatureExtractionDialog::OnSelendokClassWeightsComboDropDown)
	EVT_COMBOBOX_DROPDOWN(IDC_ClassPairWeightsCombo, CMFeatureExtractionDialog::OnSelendokClassPairWeightsComboDropDown)
END_EVENT_TABLE()



void CMFeatureExtractionDialog::CreateControls ()

{
	this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxBoxSizer* bVSizerMain = new wxBoxSizer (wxVERTICAL);
	
	bSizer250 = new wxBoxSizer (wxHORIZONTAL);
	
	wxBoxSizer* bSizer251;
	bSizer251 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer253;
	bSizer253 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText244 = new wxStaticText( this, wxID_ANY, wxT("Preprocessing Transformation:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText244->Wrap( -1 );
	bSizer253->Add( m_staticText244, 0, wxALL, 5 );
	
	m_comboBox49 = new wxComboBox( this, IDC_PreprocessingCombo, wxT("None"), wxDefaultPosition, wxSize( 210,25 ), 0, NULL, wxCB_READONLY );
	m_comboBox49->Append( wxT("None") );
	m_comboBox49->Append( wxT("Compute new...") );
	m_comboBox49->Append( wxT("Use last") );
	m_comboBox49->SetSelection( 0 );
   SetUpToolTip(m_comboBox49, IDS_ToolTip137);
	bSizer253->Add( m_comboBox49, 0, wxLEFT, 15 );
	
	
	bSizer251->Add( bSizer253, 0, wxBOTTOM|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer254;
	bSizer254 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText245 = new wxStaticText( this, wxID_ANY, wxT("Feature Extraction"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText245->Wrap( -1 );
	bSizer254->Add( m_staticText245, 0, wxALL, 5 );
	
	m_comboBox50 = new wxComboBox( this, IDC_FEAlgorithmCombo, wxT("Combo!"), wxDefaultPosition, wxSize( 210,25 ), 0, NULL, wxCB_READONLY);
	m_comboBox50->Append( wxT("Nonparametric Weighting") );
	m_comboBox50->Append( wxT("Decision Boundary") );
	m_comboBox50->Append( wxT("Discriminant Analysis") );
	m_comboBox50->Append( wxT("Preprocessing only") );
   SetUpToolTip(m_comboBox50, IDS_ToolTip138);
	bSizer254->Add( m_comboBox50, 0, wxLEFT, 15 );
	
	
	bSizer251->Add( bSizer254, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer255;
	bSizer255 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText246 = new wxStaticText( this, IDC_ChannelPrompt, wxT("Channels:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText246->Wrap( -1 );
   SetUpToolTip(m_staticText246, IDS_ToolTip52);
	bSizer255->Add( m_staticText246, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_comboBox45 = new wxComboBox( this, IDC_ChannelCombo, wxT("All"), wxDefaultPosition, wxSize( 100,-1 ), 0, NULL, wxCB_READONLY );
	m_comboBox45->Append( wxT("All") );
	m_comboBox45->Append( wxT("Subset...") );
	m_comboBox45->SetSelection( 0 );
   SetUpToolTip(m_comboBox45, IDS_ToolTip52);
	bSizer255->Add( m_comboBox45, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer251->Add( bSizer255, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer256;
	bSizer256 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText247 = new wxStaticText( this, IDC_ClassPrompt, wxT("Classes:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText247->Wrap( -1 );
   SetUpToolTip(m_staticText247, IDS_ToolTip103);
	bSizer256->Add( m_staticText247, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_comboBox46 = new wxComboBox( this, IDC_ClassCombo, wxT("Subset..."), wxDefaultPosition, wxSize( 100,-1 ), 0, NULL, wxCB_READONLY );
	m_comboBox46->Append( wxT("All") );
	m_comboBox46->Append( wxT("Subset...") );
	m_comboBox46->SetSelection( 1 );
   SetUpToolTip(m_comboBox46, IDS_ToolTip103);
	bSizer256->Add( m_comboBox46, 0, wxALL, 5 );
	
	
	bSizer251->Add( bSizer256, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer257;
	bSizer257 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText248 = new wxStaticText( this, IDC_WeightsPrompt, wxT("Class Weights:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText248->Wrap( -1 );
   SetUpToolTip(m_staticText248, IDS_ToolTip130);
	bSizer257->Add( m_staticText248, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_comboBox47 = new wxComboBox( this, IDC_WeightsCombo, wxT("Unequal..."), wxDefaultPosition, wxSize( 90,-1 ), 0, NULL, wxCB_READONLY );
	m_comboBox47->Append( wxT("Equal") );
	m_comboBox47->Append( wxT("Unequal...") );
	m_comboBox47->SetSelection( 1 );
   SetUpToolTip(m_comboBox47, IDS_ToolTip130);
	bSizer257->Add( m_comboBox47, 0, wxALL, 5 );
	
	m_staticText250 = new wxStaticText( this, IDC_WeightsEqual, wxT("Equal"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_staticText250->Wrap( -1 );
	bSizer257->Add( m_staticText250, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer251->Add( bSizer257, 0, 0, 5 );
	
	wxBoxSizer* bSizer258;
	bSizer258 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText249 = new wxStaticText( this, IDC_ClassPairWeightsPrompt, wxT("Interclass Weights:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText249->Wrap( -1 );
   SetUpToolTip(m_staticText249, IDS_ToolTip139);
	bSizer258->Add( m_staticText249, 0, wxALIGN_CENTER|wxLEFT, 5 );
	
	m_comboBox48 = new wxComboBox( this, IDC_ClassPairWeightsCombo, wxT("Unequal..."), wxDefaultPosition, wxSize( 90,-1 ), 0, NULL, wxCB_READONLY );
	m_comboBox48->Append( wxT("Equal") );
	m_comboBox48->Append( wxT("Unequal...") );
	m_comboBox48->SetSelection( 1 );
   SetUpToolTip(m_comboBox48, IDS_ToolTip139);
	bSizer258->Add( m_comboBox48, 0, wxALL, 5 );
	
	m_staticText251 = new wxStaticText( this, IDC_InterClassWeightsEqual, wxT("Equal"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText251->Wrap( -1 );
	bSizer258->Add( m_staticText251, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer251->Add( bSizer258, 0, 0, 5 );
	
	m_checkBox54 = new wxCheckBox( this, IDC_ListTransformationMatrix, wxT("List Transformation Matrix"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox54, IDS_ToolTip140);
	bSizer251->Add( m_checkBox54, 0, wxALL, 5 );
	
	
	bSizer250->Add( bSizer251, 0, wxLEFT|wxTOP|wxBOTTOM|wxEXPAND, 12 );
	
	wxBoxSizer* bSizer252;
	bSizer252 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox55 = new wxCheckBox( this, IDC_SpecialOptions, wxT("Special options"), wxDefaultPosition, wxDefaultSize, 0 );
	SetUpToolTip(m_checkBox55, IDS_ToolTip141);
   bSizer252->Add( m_checkBox55, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer259;
	bSizer259 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer260;
	bSizer260 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText252 = new wxStaticText( this, IDC_WithinClassThresholdPrompt, wxT("Within class threshold: "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText252->Wrap( -1 );
   SetUpToolTip(m_staticText252, IDS_ToolTip142);
	bSizer260->Add( m_staticText252, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl104 = new wxTextCtrl( this, IDC_WithinClassThreshold, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), 0 );
   m_textCtrl104->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_wclassThresString));
   SetUpToolTip(m_textCtrl104, IDS_ToolTip142);
	//bSizer260->Add( m_textCtrl104, 0, wxALIGN_RIGHT, 5 );
	bSizer260->Add( m_textCtrl104, 0, wxALIGN_CENTER, 5 );
	
	bSizer259->Add( bSizer260, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	wxBoxSizer* bSizer261;
	bSizer261 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText253 = new wxStaticText( this, IDC_InterclassThresholdPrompt, wxT("Interclass threshold:     "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText253->Wrap( -1 );
   SetUpToolTip(m_staticText253, IDS_ToolTip143);
	bSizer261->Add( m_staticText253, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl105 = new wxTextCtrl( this, IDC_InterclassThreshold, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), 0 );
   m_textCtrl105->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_iclassThresString));
   SetUpToolTip(m_textCtrl105, IDS_ToolTip143);
	//bSizer261->Add( m_textCtrl105, 0, wxALIGN_RIGHT|wxEXPAND, 5 );
   bSizer261->Add( m_textCtrl105, 0, wxALIGN_CENTER, 5 );
	
	
	bSizer259->Add( bSizer261, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	wxBoxSizer* bSizer262;
	bSizer262 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText254 = new wxStaticText( this, IDC_MinThresholdNumberPrompt, wxT("Min. threshold number:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText254->Wrap( -1 );
   SetUpToolTip(m_staticText254, IDS_ToolTip144);
	bSizer262->Add( m_staticText254, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl106 = new wxTextCtrl( this, IDC_MinThresholdNumber, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), 0 );
   m_textCtrl106->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_minThrestString));
   SetUpToolTip(m_textCtrl106, IDS_ToolTip144);
	//bSizer262->Add( m_textCtrl106, 0, wxALIGN_RIGHT|wxEXPAND, 5 );
	bSizer262->Add( m_textCtrl106, 0, wxALIGN_CENTER, 5 );
	
	bSizer259->Add( bSizer262, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	
	bSizer252->Add( bSizer259, 0, wxEXPAND|wxLEFT, 10 );
	
	wxBoxSizer* bSizer263;
	bSizer263 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox56 = new wxCheckBox( this, IDC_OptimizeClasses, wxT("Optimize classes"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox56, IDS_ToolTip145);
	bSizer263->Add( m_checkBox56, 0, wxLEFT|wxTOP, 10 );
	
	wxBoxSizer* bSizer264;
	bSizer264 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText255 = new wxStaticText( this, IDC_PercentAccuracyPrompt, wxT("Percent of all\rfeature accuracy:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText255->Wrap( -1 );
   SetUpToolTip(m_staticText255, IDS_ToolTip146);
	bSizer264->Add( m_staticText255, 0, wxALIGN_CENTER|wxLEFT, 25 );
	
	m_textCtrl107 = new wxTextCtrl( this, IDC_PercentAccuracy, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), 0 );
   m_textCtrl107->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_percentString));
   SetUpToolTip(m_textCtrl107, IDS_ToolTip146);
	//bSizer264->Add( m_textCtrl107, 0, wxALIGN_CENTER|wxALIGN_RIGHT|wxALL, 5 );
	bSizer264->Add( m_textCtrl107, 0, wxALIGN_CENTER, 5 );
	
	bSizer263->Add( bSizer264, 0, wxALIGN_RIGHT, 5 );
	
	
	bSizer252->Add( bSizer263, 0, wxBOTTOM|wxEXPAND|wxLEFT, 10 );
	
	wxBoxSizer* bSizer265;
	bSizer265 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText256 = new wxStaticText( this, IDC_MaxPixelsPrompt, wxT("Maximum training\npixels per class:    "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText256->Wrap( -1 );
   SetUpToolTip(m_staticText256, IDS_ToolTip147);
	//bSizer265->Add( m_staticText256, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 5 );
	bSizer265->Add( m_staticText256, 0, wxALIGN_CENTER, 5 );
	m_textCtrl108 = new wxTextCtrl( this, IDC_MaxPixels, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), 0 );
   m_textCtrl108->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_maxpixeltString));
   SetUpToolTip(m_textCtrl108, IDS_ToolTip147);
	bSizer265->Add( m_textCtrl108, 0, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer252->Add( bSizer265, 0, wxALIGN_RIGHT|wxLEFT, 15 );
	/*
	wxBoxSizer* bSizer266;
	bSizer266 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button65 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer266->Add (m_button65, wxSizerFlags(0).Border(wxALL, 5));
	
	m_button66 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer266->Add (m_button66, wxSizerFlags(0).Border(wxALL, 5));
	
	bSizer252->Add (bSizer266, wxSizerFlags(1).Align(wxALIGN_RIGHT));
	*/
	bSizer250->Add (bSizer252, wxSizerFlags(1).Expand().Border(wxRIGHT|wxTOP, 12));
	
	bVSizerMain->Add (bSizer250, wxSizerFlags(0));
	
	//wxSizer* standardButtonSizer = CreateButtonSizer (wxOK | wxCANCEL);
	//bVSizerMain->Add (standardButtonSizer, wxSizerFlags(0).Right());
	CreateStandardButtons (bVSizerMain);
	
	//this->SetSizer (bVSizerMain);
	//this->Layout ();
   this->SetSizerAndFit (bVSizerMain);
	
	this->Centre (wxBOTH);
	
}	// end "CreateControls"



//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the classification
//							specification dialog box to the user and copy the
//							revised back to the classify specification structure if
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
//	Coded By:			Larry L. Biehl			Date: 03/02/1999
//	Revised By:			Larry L. Biehl			Date: 03/03/1999

Boolean CMFeatureExtractionDialog::DoDialog(
				FeatureExtractionSpecsPtr		featureExtractionSpecsPtr)

{                                                  
	SInt16								returnCode;
	
	Boolean								continueFlag = FALSE; 

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
	
	m_featureExtractionSpecsPtr = featureExtractionSpecsPtr;  
	
	returnCode = ShowModal();
	
	if (returnCode == wxID_OK)
		{
		continueFlag = TRUE;
		
		FeatureExtractionDialogOK (featureExtractionSpecsPtr,
												m_algorithmCode+1,
												m_weightsSelection+1,
												m_classWeightsPtr,
												m_interClassWeightsSelection+1,
												m_localClassPairWeightsListPtr,
												m_localDefaultClassPairWeight,
												m_specialOptionsFlag,
												(double)m_withinClassThreshold,
												(double)m_interclassThrehold,
												m_minThresholdNumber,
												(double)m_optimizeThreshold,
												m_optimizeClassesFlag,
												m_maxPixelsPerClass,
												m_channelSelection,
												m_localActiveNumberFeatures,
												m_localActiveFeaturesPtr,
												m_classSelection,
												m_localNumberClasses,
												m_classListPtr,
												m_listTransformationFlag,
												m_preprocessSelection+1 );
														
		}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



void CMFeatureExtractionDialog::OnDecisionBoundary (wxCommandEvent& event)

{
	wxComboBox* fEAlgorithmCombo = (wxComboBox*) FindWindow(IDC_FEAlgorithmCombo);
   m_algorithmCode = fEAlgorithmCombo->GetSelection();
	FeatureExtractionDialogAlgorithm (this, m_algorithmCode+1);
	
	FeatureExtractionDialogUpdateSpecialOptions (this,
																m_algorithmCode+1,
																m_specialOptionsFlag,
																m_optimizeClassesFlag);
	
}	// end "OnDecisionBoundary"



void CMFeatureExtractionDialog::OnInitDialog (wxInitDialogEvent& event)
{  
	double								interclassThrehold,
											optimizeThreshold,
											withinClassThreshold;

	ProjectionPursuitSpecsPtr		projectionPursuitSpecsPtr;
														
	SInt16								algorithmCode,
											channelSelection,
											classSelection,
											interClassWeightsSelection,
											preprocessSelection,
											weightsSelection;
	

//	CMDialog::OnInitDialog(event);
	
			// Initialize dialog variables.

	FeatureExtractionDialogInitialize (this,
														m_featureExtractionSpecsPtr,
														m_localActiveFeaturesPtr,
														m_classListPtr,
														&m_localClassPairWeightsListPtr,
														&algorithmCode,
														&weightsSelection,
														&interClassWeightsSelection,
														&m_localDefaultClassPairWeight,
														(Boolean*)&m_specialOptionsFlag, 
														&withinClassThreshold,
														&interclassThrehold,
														(UInt32*)&m_minThresholdNumber,
														(Boolean*)&m_optimizeClassesFlag,
														&optimizeThreshold,
														(UInt32*)&m_maxPixelsPerClass,
														&channelSelection,
														&m_localNumberFeatures,
														&m_channelListType,
														&classSelection,
														&m_localNumberClasses,
														&preprocessSelection,
														(Boolean*)&m_listTransformationFlag ); 
	                                                                   			
      
   
   //	Set the channels/features list item				
			                                             
	m_availableFeaturePtr = gProjectInfoPtr->channelsPtr;
	m_channelSelection = channelSelection;

   // Set feature parameters 
	m_localActiveNumberFeatures = m_localNumberFeatures;

   
			// Algorithm.
			// Adjust for 0 base index.
			
	m_algorithmCode = algorithmCode - 1;  
                      
	FeatureExtractionDialogAlgorithm (this, m_algorithmCode+1);							
	HideDialogItem (this, IDC_PreprocessingOnly);
	
			// Class selection.
			
	m_classSelection = classSelection;
	
			// Preprocessing selection.  
			// Adjust for 0 base index.
	
	m_preprocessSelection = preprocessSelection - 1;                            													
//	HideDialogItem (this, IDC_PreprocessingCombo);	
	projectionPursuitSpecsPtr = 
							&m_featureExtractionSpecsPtr->projectionPursuitParams;
	if (projectionPursuitSpecsPtr->finalNumberFeatures <= 0){                           
//		((CComboBox*)GetDlgItem(IDC_PreprocessingCombo))->DeleteString(2);
      wxComboBox* preprocessingCombo = (wxComboBox*) FindWindow(IDC_PreprocessingCombo);
      preprocessingCombo->Delete(2);
      
   }
	
			// Class weights.
			// Adjust for 0 base index.
	
	m_weightsSelection = weightsSelection - 1; 
//	HideDialogItem (this, IDC_WeightsCombo);   
	HideDialogItem (this, IDC_WeightsEqual);   
	
			// Inter class weights.
			// Adjust for 0 base index.
	
	m_interClassWeightsSelection = interClassWeightsSelection - 1;  
//	HideDialogItem (this, IDC_ClassPairWeightsCombo);
	HideDialogItem (this, IDC_InterClassWeightsEqual);
	
	
	
	m_interclassThrehold = (float)interclassThrehold;
	m_optimizeThreshold = (float)optimizeThreshold;
	m_withinClassThreshold = (float)withinClassThreshold;

	                 
	if (TransferDataToWindow())                   
		PositionDialogWindow ();
	
//   this->SetSizerAndFit(bSizer250);		
//   this->Layout();
//	return FALSE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"



void CMFeatureExtractionDialog::OnOptimizeClasses(wxCommandEvent& event)
{         
//	DDX_Check(m_dialogFromPtr, IDC_OptimizeClasses, m_optimizeClassesFlag);  
   wxCheckBox* optimizeClasses = (wxCheckBox*) FindWindow(IDC_OptimizeClasses);					
   m_optimizeClassesFlag = optimizeClasses->GetValue();
	FeatureExtractionDialogOptimizeClass (this, m_optimizeClassesFlag);                                                     
	
}		// end "OnOptimizeClasses" 



void CMFeatureExtractionDialog::OnSpecialOptions(wxCommandEvent& event) 

{                                       
//	DDX_Check(m_dialogFromPtr,IDC_SpecialOptions, m_specialOptionsFlag);                                                       
	 wxCheckBox* specialOptions = (wxCheckBox*) FindWindow(IDC_SpecialOptions);
    m_specialOptionsFlag = specialOptions->GetValue();
	FeatureExtractionDialogUpdateSpecialOptions (
														this,
														m_algorithmCode+1,
														m_specialOptionsFlag,
														m_optimizeClassesFlag);
//	if(m_specialOptionsFlag == true){
//      this->Layout();
////      this->Fit();
//   }
}		// end "OnSpecialOptions"


void CMFeatureExtractionDialog::OnSelendokChannelCombo(wxCommandEvent& event)
{                                                                                                       
	HandleChannelsMenu(IDC_ChannelCombo, 
								kNoTransformation,
								(SInt16)gProjectInfoPtr->numberStatisticsChannels,
								m_channelListType,
								TRUE);   
	
}		// end "OnSelendokChannelCombo"

void CMFeatureExtractionDialog::OnCbnSelendokPreprocessingDropDown(wxCommandEvent& event)
{                                                                                                       
	wxComboBox* preprocessingCombo = (wxComboBox*) FindWindow(IDC_PreprocessingCombo);
   m_preprocess_Saved = preprocessingCombo->GetSelection();
	preprocessingCombo->SetSelection(-1);
	
}		// end "OnSelendokChannelComboDN"

//void CMFeatureExtractionDialog::OnSelendokChannelComboDN(wxCommandEvent& event)
//{                                                                                                       
//	wxComboBox* channel = (wxComboBox*)FindWindow(IDC_ChannelCombo);
//	channel->SetSelection(0);
//	
//}		// end "OnSelendokChannelComboDN"
  

void CMFeatureExtractionDialog::OnSelendokClassWeightsCombo(wxCommandEvent& event)
{                                                           
	HandleClassWeightsMenu ((SInt16)m_localNumberClasses, 
									(SInt16*)m_classListPtr,
									m_classWeightsPtr,
									gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
									IDC_WeightsCombo,
									&m_weightsSelection);
	
}		// end "OnSelendokClassWeightsCombo"

void CMFeatureExtractionDialog::OnSelendokClassWeightsComboDN(wxCommandEvent& event)
{                                                                                                       
	wxComboBox* weights = (wxComboBox*)FindWindow(IDC_WeightsCombo);
	weights->SetSelection(0);	
}		// end "OnSelendokClassWeightsComboDN"

void CMFeatureExtractionDialog::OnSelendokClassPairWeightsCombo(wxCommandEvent& event)
{                                                           
	HandleClassPairWeightsMenu (
						&m_localClassPairWeightsListPtr,
						IDC_ClassPairWeightsCombo,
						&m_interClassWeightsSelection,
						&m_localDefaultClassPairWeight);
	
}		// end "OnSelendokClassPairWeightsCombo"

void CMFeatureExtractionDialog::OnSelendokClassPairWeightsComboDN(wxCommandEvent& event)
{                                                                                                       
	wxComboBox* classweights = (wxComboBox*)FindWindow(IDC_ClassPairWeightsCombo);
	classweights->SetSelection(0);	
}		// end "OnSelendokClassPairWeightsComboDN"

void CMFeatureExtractionDialog::OnCbnSelendokPreprocessingcombo(wxCommandEvent& event)
{   
	Boolean					okFlag;


//	DDX_CBIndex(m_dialogFromPtr, 	IDC_PreprocessingCombo, m_preprocessSelection);
   wxComboBox* preprocessingCombo = (wxComboBox*) FindWindow(IDC_PreprocessingCombo);
      m_preprocessSelection = preprocessingCombo->GetSelection();
	if (m_preprocessSelection == 1)
		okFlag = ProjectionPursuitDialog();

	if (okFlag)
	{

	}		// end "if (okFlag)"

}		// end "OnCbnSelendokPreprocessingcombo"



bool CMFeatureExtractionDialog::TransferDataFromWindow ()

{
	SInt16 returnCode = 0;
	
	
   wxTextCtrl* interclassThreshold = (wxTextCtrl*) FindWindow(IDC_InterclassThreshold);
   wxTextCtrl* withinClassThreshold = (wxTextCtrl*) FindWindow(IDC_WithinClassThreshold);
   wxTextCtrl* minThresholdNumber = (wxTextCtrl*) FindWindow(IDC_MinThresholdNumber);
   wxTextCtrl* maxPixels = (wxTextCtrl*) FindWindow(IDC_MaxPixels);
   wxTextCtrl* percentAccuracy = (wxTextCtrl*) FindWindow(IDC_PercentAccuracy);
	
   wxComboBox* classPairWeightsCombo = (wxComboBox*) FindWindow(IDC_ClassPairWeightsCombo);
   wxComboBox* preprocessingCombo = (wxComboBox*) FindWindow(IDC_PreprocessingCombo);
   wxComboBox* fEAlgorithmCombo = (wxComboBox*) FindWindow(IDC_FEAlgorithmCombo);
   wxComboBox* channelCombo = (wxComboBox*) FindWindow(IDC_ChannelCombo);
   wxComboBox* classCombo = (wxComboBox*) FindWindow(IDC_ClassCombo);
   wxComboBox* weightsCombo = (wxComboBox*) FindWindow(IDC_WeightsCombo);
	
   wxCheckBox* specialOptions = (wxCheckBox*) FindWindow(IDC_SpecialOptions);
   wxCheckBox* optimizeClasses = (wxCheckBox*) FindWindow(IDC_OptimizeClasses);
   wxCheckBox* listTransformationMatrix = (wxCheckBox*) FindWindow(IDC_ListTransformationMatrix);
	
   wxString m_interclassThreholdstring = interclassThreshold->GetValue();
      double interclassThresholdD;
      m_interclassThreholdstring.ToDouble(&interclassThresholdD);
      m_interclassThrehold = (float)interclassThresholdD;
   wxString withinClassThresholdstring = withinClassThreshold->GetValue();
      double withinClassThresholdD;
      withinClassThresholdstring.ToDouble(&withinClassThresholdD);
      m_withinClassThreshold = (float)withinClassThresholdD;
   wxString minThresholdNumberstring = minThresholdNumber->GetValue();
      m_minThresholdNumber = wxAtoi(minThresholdNumberstring);
   wxString maxPixelsstring = maxPixels->GetValue();
      m_maxPixelsPerClass = wxAtoi(maxPixelsstring);
   wxString percentAccuracystring = percentAccuracy->GetValue();
   double percentAccuracyD;
      percentAccuracystring.ToDouble(&percentAccuracyD);
   m_optimizeThreshold = (float)percentAccuracyD;
	
   m_interClassWeightsSelection = classPairWeightsCombo->GetSelection();
   m_preprocessSelection = preprocessingCombo->GetSelection();
   m_algorithmCode = fEAlgorithmCombo->GetSelection();
   m_channelSelection = channelCombo->GetSelection();
   m_classSelection = classCombo->GetSelection();
   m_weightsSelection = weightsCombo->GetSelection();
	
   m_specialOptionsFlag = specialOptions->GetValue();
   m_optimizeClassesFlag = optimizeClasses->GetValue();
   m_listTransformationFlag = listTransformationMatrix->GetValue();
	
   if (m_channelSelection < 0)
      m_channelSelection = m_channelSelection_Saved;
   if (m_classSelection < 0)
      m_classSelection = m_classSelection_Saved;
   if (m_interClassWeightsSelection < 0)
      m_interClassWeightsSelection = m_interClassWeightsSelection_Saved;
   if (m_weightsSelection < 0)
      m_weightsSelection = m_weightSelection_Saved;
   if(m_preprocessSelection < 0)
      m_preprocessSelection = m_preprocess_Saved;
	
   returnCode = CheckDialogRealValue (this,
													IDC_MaxPixels,
													1,
													32000,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
   if(returnCode == 0)
      returnCode = CheckDialogRealValue (this,
													IDC_PercentAccuracy,
													0.,
													100.,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
   if(returnCode == 0)
      returnCode = CheckDialogRealValue (this,
													IDC_WithinClassThreshold,
													0,
													1,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
   if(returnCode == 0)
      returnCode = CheckDialogRealValue (this,
													IDC_InterclassThreshold,
													0,
													1,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
   if(returnCode == 0)
      returnCode = CheckDialogRealValue (this,
													IDC_MinThresholdNumber,
													1,
													SInt16_MAX,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
   return (returnCode == 0);
	
}	// end "TransferDataFromWindow"



bool CMFeatureExtractionDialog::TransferDataToWindow ()

{
   wxTextCtrl* interclassThreshold = (wxTextCtrl*) FindWindow(IDC_InterclassThreshold);
   wxTextCtrl* withinClassThreshold = (wxTextCtrl*) FindWindow(IDC_WithinClassThreshold);
   wxTextCtrl* minThresholdNumber = (wxTextCtrl*) FindWindow(IDC_MinThresholdNumber);
   wxTextCtrl* maxPixels = (wxTextCtrl*) FindWindow(IDC_MaxPixels);
   wxTextCtrl* percentAccuracy = (wxTextCtrl*) FindWindow(IDC_PercentAccuracy);
	
   wxComboBox* classPairWeightsCombo = (wxComboBox*) FindWindow(IDC_ClassPairWeightsCombo);
   wxComboBox* preprocessingCombo = (wxComboBox*) FindWindow(IDC_PreprocessingCombo);
   wxComboBox* fEAlgorithmCombo = (wxComboBox*) FindWindow(IDC_FEAlgorithmCombo);
   wxComboBox* channelCombo = (wxComboBox*) FindWindow(IDC_ChannelCombo);
   wxComboBox* classCombo = (wxComboBox*) FindWindow(IDC_ClassCombo);
   wxComboBox* weightsCombo = (wxComboBox*) FindWindow(IDC_WeightsCombo);
	
   wxCheckBox* specialOptions = (wxCheckBox*) FindWindow(IDC_SpecialOptions);
   wxCheckBox* optimizeClasses = (wxCheckBox*) FindWindow(IDC_OptimizeClasses);
   wxCheckBox* listTransformationMatrix = (wxCheckBox*) FindWindow(IDC_ListTransformationMatrix);
	
   interclassThreshold->ChangeValue(wxString::Format(wxT("%.2f"), m_interclassThrehold));
   withinClassThreshold->ChangeValue(wxString::Format(wxT("%.2f"), m_withinClassThreshold));
   percentAccuracy->ChangeValue(wxString::Format(wxT("%.1f"), m_optimizeThreshold));
   minThresholdNumber->ChangeValue(wxString::Format(wxT("%i"), m_minThresholdNumber));
   maxPixels->ChangeValue(wxString::Format(wxT("%i"), m_maxPixelsPerClass));
	
   classPairWeightsCombo->SetSelection(m_interClassWeightsSelection);
   preprocessingCombo->SetSelection(m_preprocessSelection);
   fEAlgorithmCombo->SetSelection(m_algorithmCode);
   channelCombo->SetSelection(m_channelSelection);
   classCombo->SetSelection(m_classSelection);
   weightsCombo->SetSelection(m_weightsSelection);
	
   specialOptions->SetValue(m_specialOptionsFlag);
   optimizeClasses->SetValue(m_optimizeClassesFlag);
   listTransformationMatrix->SetValue(m_listTransformationFlag);
	
   return true;
	
}
