// LEvaluateCovariancesDialog.cpp : implementation file
//             
// Revised by Larry Biehl on 11/16/2018
//
#include "SMultiSpec.h"
                      
#include "LEvaluateCovariancesDialog.h"

//#include	"SExternalGlobals.h"

extern void 		EvaluateCovariancesDialogOK (
							EvaluateCovarianceSpecsPtr		evaluateCovarianceSpecsPtr,
							Boolean								listOriginalMatrixFlag,
							Boolean								listInvertedMatrixFlag,
							Boolean								listOriginalXInvertedMatrixFlag,
							Boolean								listInvertedInvertedMatrixFlag,
							Boolean								featureTransformationFlag);

/////////////////////////////////////////////////////////////////////////////
// CMEvalCovarianceDialog dialog

CMEvalCovarianceDialog::CMEvalCovarianceDialog(wxWindow* pParent,
   wxWindowID id, const wxString& title/*=NULL*/)
:CMDialog(CMEvalCovarianceDialog::IDD, pParent, title)
{
   m_listOriginalMatrixFlag = FALSE;
	m_listInvertedMatrixFlag = FALSE;
	m_listInvertedInvertedMatrixFlag = FALSE;
	m_listOriginalXInvertedMatrixFlag = FALSE;
	m_featureTransformationFlag = FALSE;
   
   m_evaluateCovarianceSpecsPtr = NULL;
	m_featureTransformAllowedFlag = FALSE;
	m_numberEigenvectors = 0;
   
   m_initializedFlag = CMDialog::m_initializedFlag;
   
   	if (m_initializedFlag)
		{                          
				// Get pointer to memory for temporary storage of channel list.
				
		WindowInfoPtr projectWindowInfoPtr = (WindowInfoPtr)GetHandlePointer(
								gProjectInfoPtr->windowInfoHandle, kNoLock, kNoMoveHi);
		SInt16 totalNumberChannels = projectWindowInfoPtr->totalNumberChannels; 
			                                                                       
		m_localFeaturesPtr = (UInt16*)MNewPointer ( 
											(UInt32)totalNumberChannels * sizeof(UInt16) ); 
	                                                       
		m_initializedFlag = (m_localFeaturesPtr != NULL);
		
		}		// end "if (m_initializedFlag)" 
	
	if (m_initializedFlag)
		{
				// Get memory for the local class list vector.	
				
		m_classListPtr = (UInt16*)MNewPointer ( 
				(UInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof(UInt16) );
	                                                       
		m_initializedFlag = (m_classListPtr != NULL);
		
		}		// end "if (m_initializedFlag)" 
   
   CreateControls();
   //SetSizerAndFit(bSizer107);
     
} // end "CMEvalCovarianceDialog"


CMEvalCovarianceDialog::~CMEvalCovarianceDialog(void)
{  
   m_classListPtr = CheckAndDisposePtr (m_classListPtr);
	m_localFeaturesPtr = CheckAndDisposePtr (m_localFeaturesPtr); 
} // end "~CMEvalCovarianceDialog"


BEGIN_EVENT_TABLE(CMEvalCovarianceDialog, CMDialog)
EVT_INIT_DIALOG(CMEvalCovarianceDialog::OnInitDialog)
EVT_COMBOBOX(IDC_ChannelCombo, CMEvalCovarianceDialog::OnSelendokChannelCombo)
EVT_COMBOBOX(IDC_ClassCombo, CMEvalCovarianceDialog::OnSelendokClassCombo)
EVT_CHECKBOX(IDC_Transformation, CMEvalCovarianceDialog::OnTransformation)
EVT_COMBOBOX_DROPDOWN(IDC_ClassCombo, CMEvalCovarianceDialog::OnSelendokClassComboDropDown)
EVT_COMBOBOX_DROPDOWN(IDC_ChannelCombo, CMEvalCovarianceDialog::OnSelendokChannelComboDropDown)
END_EVENT_TABLE()

bool CMEvalCovarianceDialog::TransferDataToWindow() {
   
   wxCheckBox* listOriginalMatrixFlag = (wxCheckBox*) FindWindow(IDC_OrigMatrix);
   wxCheckBox* listInvertedMatrixFlag = (wxCheckBox*) FindWindow(IDC_InvertedMatrix);
   wxCheckBox* listInvertedInvertedMatrixFlag = (wxCheckBox*) FindWindow(IDC_InvertedMatrix);
   wxCheckBox* listOriginalXInvertedMatrixFlag = (wxCheckBox*) FindWindow(IDC_Identity);
   wxCheckBox* featureTransformationFlag = (wxCheckBox*) FindWindow(IDC_Transformation);
   
   wxComboBox* channelSelection = (wxComboBox*) FindWindow(IDC_ChannelCombo);
   wxComboBox* classSelection = (wxComboBox*) FindWindow(IDC_ClassCombo);
   
   listOriginalMatrixFlag->SetValue(m_listOriginalMatrixFlag);
   listInvertedMatrixFlag->SetValue(m_listInvertedMatrixFlag);
   listInvertedInvertedMatrixFlag->SetValue(m_listInvertedInvertedMatrixFlag);
   listOriginalXInvertedMatrixFlag->SetValue(m_listOriginalXInvertedMatrixFlag);
   featureTransformationFlag->SetValue(m_featureTransformationFlag);
   channelSelection->SetSelection(m_channelSelection);
   classSelection->SetSelection(m_classSelection);
   
   return TRUE;
}

bool CMEvalCovarianceDialog::TransferDataFromWindow() {
   
   wxCheckBox* listOriginalMatrixFlag = (wxCheckBox*) FindWindow(IDC_OrigMatrix);
   wxCheckBox* listInvertedMatrixFlag = (wxCheckBox*) FindWindow(IDC_InvertedMatrix);
   wxCheckBox* listInvertedInvertedMatrixFlag = (wxCheckBox*) FindWindow(IDC_InvertedMatrix);
   wxCheckBox* listOriginalXInvertedMatrixFlag = (wxCheckBox*) FindWindow(IDC_Identity);
   wxCheckBox* featureTransformationFlag = (wxCheckBox*) FindWindow(IDC_Transformation);
   
   wxComboBox* channelSelection = (wxComboBox*) FindWindow(IDC_ChannelCombo);
   wxComboBox* classSelection = (wxComboBox*) FindWindow(IDC_ClassCombo);
   
   m_listOriginalMatrixFlag = listOriginalMatrixFlag->GetValue();
   m_listInvertedMatrixFlag = listInvertedMatrixFlag->GetValue();
   m_listInvertedInvertedMatrixFlag = listInvertedInvertedMatrixFlag->GetValue();
   m_listOriginalXInvertedMatrixFlag = listOriginalXInvertedMatrixFlag->GetValue();
   m_featureTransformationFlag = featureTransformationFlag->GetValue();
 
   m_channelSelection = channelSelection->GetSelection();
   if(m_channelSelection < 0)
      m_channelSelection = m_channelSelection_Saved;
   
   m_classSelection = classSelection->GetSelection();
   if(m_classSelection < 0)
      m_classSelection = m_classSelection_Saved;
   
   return TRUE;
   //printf("From:channelSelection:%d\n", m_channelSelection );
   //printf("From:classSelection:%d\n", m_classSelection );
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
//	Coded By:			Larry L. Biehl			Date: 04/26/96
//	Revised By:			Larry L. Biehl			Date: 05/13/98	

Boolean 
CMEvalCovarianceDialog::DoDialog(
				EvaluateCovarianceSpecsPtr		evaluateCovarianceSpecsPtr){
   
   Boolean  continueFlag = FALSE; 
	SInt16   returnCode;	 
	UInt16	index;
   
   // Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
																			
	m_evaluateCovarianceSpecsPtr = evaluateCovarianceSpecsPtr;
   
	if(TransferDataFromWindow()){																			
	  returnCode = ShowModal();
	}
	if (returnCode == wxID_OK)
		{
		continueFlag = TRUE;
		
		gChannelSelection = m_channelSelection; 
		if (m_channelSelection == kSubsetMenuItem)	// Subset of channels or features 
			{
			evaluateCovarianceSpecsPtr->numberFeatures = m_localActiveNumberFeatures; 
			for ( index=0; index<(UInt16)m_localNumberFeatures; index++)
				evaluateCovarianceSpecsPtr->featurePtr[index] = m_localFeaturesPtr[index];
												
			}		// end "else m_channelSelection == kSubsetMenuItem"  
		
		gClassSelection = m_classSelection;
		if (m_classSelection == kSubsetMenuItem) 
			{                                          
			evaluateCovarianceSpecsPtr->numberClasses = m_localNumberClasses;               
			for (index=0; index<(UInt16)m_localNumberClasses; index++)
				evaluateCovarianceSpecsPtr->classPtr[index] = m_classListPtr[index]; 
			
			}		// end "else m_classSelection == kSubsetMenuItem"	
	
		EvaluateCovariancesDialogOK (
							evaluateCovarianceSpecsPtr,
							m_listOriginalMatrixFlag,
							m_listInvertedMatrixFlag,
							m_listOriginalXInvertedMatrixFlag,
							m_listInvertedInvertedMatrixFlag,
							m_featureTransformationFlag);
		
		}		// end "if (returnCode == IDOK)"
	
	return (continueFlag);
   
   
   
   
}

/////////////////////////////////////////////////////////////////////////////
// CMEvalCovarianceDialog message handlers

void  CMEvalCovarianceDialog::OnInitDialog(wxInitDialogEvent& event)
{
   Boolean					tempFlag; 
	
	UInt16					index;
   
   CMDialog::OnInitDialog(event); 
	
			// Set flag for listing the original covariance matrix.		
			
	m_listOriginalMatrixFlag = m_evaluateCovarianceSpecsPtr->listOriginalMatrixFlag;
	
			// Set flag for listing the inverted matrix.					
			
	m_listInvertedMatrixFlag = m_evaluateCovarianceSpecsPtr->listInvertedMatrixFlag;
	
			// Set flag for listing the original covariance matrix		
			//          multiplied by the inverted matrix, i.e. identity matrix.	
			
	m_listOriginalXInvertedMatrixFlag = 
									m_evaluateCovarianceSpecsPtr->listOriginalXInvertedMatrixFlag;
	
			// Set flag for listing the inverted-inverted covariance matrix.																	
			
	m_listInvertedInvertedMatrixFlag = 
									m_evaluateCovarianceSpecsPtr->listInvertedInvertedMatrixFlag;
		
			// Set the check box for the feature transformation option.				
			
	EigenvectorInfoExists (kProject, NULL, &m_numberEigenvectors);
	m_featureTransformAllowedFlag = (m_numberEigenvectors > 0);
	if (m_numberEigenvectors <= 0)
		{		
		m_featureTransformationFlag = FALSE;
		MHideDialogItem (this, IDC_Transformation);
		
		}		// end "if (numberEigenvectors <= 0)" 
		
	else		// numberEigenvectors > 0 
		m_featureTransformationFlag = m_evaluateCovarianceSpecsPtr->featureTransformationFlag;
									
	tempFlag = CheckFeatureTransformationDialog (
							this, 
							m_featureTransformAllowedFlag,
							IDC_Transformation, 
							IDC_ChannelCombo,
							(SInt16*)&m_featureTransformationFlag);
	                                                        							
			//	Channels to use													
			                                                                   
	m_channelSelection = m_evaluateCovarianceSpecsPtr->channelSet;
	m_localNumberFeatures = m_evaluateCovarianceSpecsPtr->numberChannels; 
	for (index=0; index<m_localNumberFeatures; index++)
		m_localFeaturesPtr[index] = m_evaluateCovarianceSpecsPtr->featurePtr[index];
	
			// Set feature parameters 
			
	m_localActiveNumberFeatures =	m_localNumberFeatures; 
	m_localActiveFeaturesPtr = m_localFeaturesPtr; 
	                                                                   			
			//	Set the channels/features list item				
			                                             
	m_availableFeaturePtr = gProjectInfoPtr->channelsPtr;
	                                                      								
			//	Classes to use													
			                                                                      
	m_classSelection = m_evaluateCovarianceSpecsPtr->classSet;
	m_localNumberClasses = m_evaluateCovarianceSpecsPtr->numberClasses;
	for (index=0; index<m_localNumberClasses; index++)
		m_classListPtr[index] = m_evaluateCovarianceSpecsPtr->classPtr[index];  
	                  
	if (TransferDataToWindow() )                   
		PositionDialogWindow ();
	
	//return TRUE;  // return TRUE  unless you set the focus to a control
}



void CMEvalCovarianceDialog::OnTransformation(wxCommandEvent& event)
{
	// TODO: Add your control notification handler code here
	
}  

  

void CMEvalCovarianceDialog::OnSelendokChannelCombo(wxCommandEvent& event)
{  
  
	HandleChannelsMenu(IDC_ChannelCombo, 
								m_featureTransformationFlag,
								(SInt16)gProjectInfoPtr->numberStatisticsChannels,
								2,
								TRUE);
	
}		// end "OnSelendokChannelCombo"  



void CMEvalCovarianceDialog::OnSelendokClassCombo(wxCommandEvent& event)
{                                                                                
	HandleClassesMenu( &m_localNumberClasses,
								(SInt16*)m_classListPtr,
								1,      
								(SInt16)gProjectInfoPtr->numberStatisticsClasses, 
								IDC_ClassCombo,
								&m_classSelection);
	
}		// end "OnSelendokClassCombo" 

//void
//CMEvalCovarianceDialog::OnSelendokClassComboDropDown(wxCommandEvent& event) {
////   wxComboBox* classcombo = (wxComboBox *) FindWindow(IDC_ClassCombo);
////   m_classSelection_Saved = classcombo->GetSelection();
////   classcombo->SetSelection(-1);
//   ResetComboBox(IDC_ClassCombo);
//}

//void
//CMEvalCovarianceDialog::OnSelendokChannelComboDropDown(wxCommandEvent& event) {
////   wxComboBox* channelcombo = (wxComboBox *) FindWindow(IDC_ChannelCombo);
////   m_channelSelection_Saved = channelcombo->GetSelection();
////   channelcombo->SetSelection(-1);
//   ResetComboBox(IDC_ChannelCombo);
//}


void CMEvalCovarianceDialog::CreateControls(){
   
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer107;
	bSizer107 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer108;
	bSizer108 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer175;
	bSizer175 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox14 = new wxCheckBox( this, IDC_OrigMatrix, wxT("List original covariance matrix"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox14, IDS_ToolTip243);
	bSizer175->Add( m_checkBox14, 0, wxALL, 5 );
	
	m_checkBox15 = new wxCheckBox( this, IDC_InvertedMatrix, wxT("List inverted covariance matrix"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox15, IDS_ToolTip243);
	bSizer175->Add( m_checkBox15, 0, wxALL, 5 );
	
	m_checkBox16 = new wxCheckBox( this, IDC_InvertedInverted, wxT("List covariance x inverted covariance matrices"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox16, IDS_ToolTip244);
	bSizer175->Add( m_checkBox16, 0, wxALL, 5 );
	
	m_checkBox17 = new wxCheckBox( this, IDC_Identity, wxT("List inverted-inverted covariance matrix"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox17, IDS_ToolTip245);
	bSizer175->Add( m_checkBox17, 0, wxALL, 5 );
	
	m_checkBox18 = new wxCheckBox( this, IDC_Transformation, wxT("Use feature transformation"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox18, IDS_ToolTip246);
	bSizer175->Add( m_checkBox18, 0, wxALL, 5 );

   wxBoxSizer* bSizer176 = new wxBoxSizer( wxHORIZONTAL );
//	wxGridSizer* gSizer10;
//	bSizer176 = new wxGridSizer( 0, 2, 0, 0 );
	
	m_staticText190 = new wxStaticText( this, wxID_ANY, wxT("Channels:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText190->Wrap( -1 );
    SetUpToolTip(m_staticText190, IDS_ToolTip52);
	bSizer176->Add( m_staticText190, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_comboBox26 = new wxComboBox( this, IDC_ChannelCombo, wxT("All"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY); 
	m_comboBox26->Append( wxT("All") );
	m_comboBox26->Append( wxT("Subset...") );
   SetUpToolTip(m_comboBox26, IDS_ToolTip52);
   bSizer176->Add( m_comboBox26, 0, wxALL|wxALIGN_LEFT, 5 );
	
   wxBoxSizer* bSizer177 = new wxBoxSizer( wxHORIZONTAL );
   
	m_staticText191 = new wxStaticText( this, wxID_ANY, wxT("Classes:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText191->Wrap( -1 );
   SetUpToolTip(m_staticText191, IDS_ToolTip103);
	bSizer177->Add( m_staticText191, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_comboBox27 = new wxComboBox( this, IDC_ClassCombo, wxT("All"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY); 
	m_comboBox27->Append( wxT("All") );
	m_comboBox27->Append( wxT("Subset...") );
   SetUpToolTip(m_comboBox27, IDS_ToolTip103);
   bSizer177->Add( m_comboBox27, 0, wxALL|wxALIGN_LEFT, 5 );
	
	
	bSizer175->Add( bSizer176, 0, wxEXPAND, 5 );
	bSizer175->Add( bSizer177, 0, wxEXPAND, 5 );
	
	bSizer108->Add( bSizer175, 0, wxEXPAND, 5 );
	
	
	bSizer107->Add( bSizer108, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );
	
	wxBoxSizer* bSizer109;
	bSizer109 = new wxBoxSizer( wxVERTICAL );
	
	
	bSizer107->Add( bSizer109, 0, wxEXPAND|wxLEFT|wxRIGHT, 12 );
	
	wxBoxSizer* bSizer110;
	bSizer110 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button20 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer110->Add( m_button20, 0, wxALL, 5 );
	
	m_button21 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer110->Add( m_button21, 0, wxALL, 5 );
	
	
	bSizer107->Add( bSizer110, 0, wxALIGN_RIGHT|wxLEFT|wxRIGHT, 12 );
	
	
	
	this->Layout();
	SetSizerAndFit(bSizer107);
	//this->Centre( wxBOTH );
   
   //this->SetSizer( bSizer107 );
}

