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
//	File:						xEvaluateCovariancesDialog.cpp : class implementation file
//	Class Definition:		xEvaluateCovariancesDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/11/2019
//
//	Language:				C++
//
//	System:					Linux & MacOS Operating Systems
//
//	Brief description:	This file contains functions related to the
//								CMEvalCovarianceDialog class.
//
// Following is template for debugging
/*
	int numberChars = sprintf ((char*)gTextString3,
									 " xEvaluateCovariancesDialog:: (): %s",
									 gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
                      
#include "xEvaluateCovariancesDialog.h"



BEGIN_EVENT_TABLE (CMEvalCovarianceDialog, CMDialog)
	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_ChannelCombo, CMEvalCovarianceDialog::OnChannelComboSelendok)
		EVT_COMBOBOX (IDC_ClassCombo, CMEvalCovarianceDialog::OnClassComboSelendok)
	#endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_ChannelCombo, CMEvalCovarianceDialog::OnChannelComboSelendok)
		EVT_CHOICE (IDC_ClassCombo, CMEvalCovarianceDialog::OnClassComboSelendok)
	#endif

	EVT_COMBOBOX_CLOSEUP (IDC_ClassCombo, CMEvalCovarianceDialog::OnClassComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_ChannelCombo, CMEvalCovarianceDialog::OnChannelComboCloseUp)

	EVT_COMBOBOX_DROPDOWN (IDC_ClassCombo, CMEvalCovarianceDialog::OnClassComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_ChannelCombo, CMEvalCovarianceDialog::OnChannelComboDropDown)
	
	EVT_INIT_DIALOG (CMEvalCovarianceDialog::OnInitDialog)
END_EVENT_TABLE ()



CMEvalCovarianceDialog::CMEvalCovarianceDialog (
				wxWindow* 							pParent,
   			wxWindowID 							id,
   			const wxString& 					title/*=NULL*/)
		:CMDialog (CMEvalCovarianceDialog::IDD, pParent, title)

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
				
		WindowInfoPtr projectWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																gProjectInfoPtr->windowInfoHandle);
		SInt16 totalNumberChannels = projectWindowInfoPtr->totalNumberChannels; 
			                                                                       
		m_localFeaturesPtr = (UInt16*)MNewPointer (
											(UInt32)totalNumberChannels * sizeof (UInt16));
	                                                       
		m_initializedFlag = (m_localFeaturesPtr != NULL);
		
		}	// end "if (m_initializedFlag)" 
	
	if (m_initializedFlag)
		{
				// Get memory for the local class list vector.	
				
		m_classListPtr = (UInt16*)MNewPointer (
				(UInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));
	                                                       
		m_initializedFlag = (m_classListPtr != NULL);
		
		}	// end "if (m_initializedFlag)"
   
   CreateControls ();
     
}	// end "CMEvalCovarianceDialog"



CMEvalCovarianceDialog::~CMEvalCovarianceDialog (void)

{  
   m_classListPtr = CheckAndDisposePtr (m_classListPtr);
	m_localFeaturesPtr = CheckAndDisposePtr (m_localFeaturesPtr);
	
}	// end "~CMEvalCovarianceDialog"



void CMEvalCovarianceDialog::CreateControls ()

{
	SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxBoxSizer* bSizer107;
	bSizer107 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer108;
	bSizer108 = new wxBoxSizer (wxHORIZONTAL);
	
	wxBoxSizer* bSizer175;
	bSizer175 = new wxBoxSizer (wxVERTICAL);
	
	m_checkBox14 = new wxCheckBox (this,
												IDC_OrigMatrix,
												wxT("List original covariance matrix"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox14, IDS_ToolTip243);
	bSizer175->Add (m_checkBox14, 0, wxALL, 5);
	
	m_checkBox15 = new wxCheckBox (this,
												IDC_InvertedMatrix,
												wxT("List inverted covariance matrix"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox15, IDS_ToolTip243);
	bSizer175->Add (m_checkBox15, 0, wxALL, 5);
	
	m_checkBox16 = new wxCheckBox (
										this,
										IDC_InvertedInverted,
										wxT("List covariance x inverted covariance matrices"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
   SetUpToolTip (m_checkBox16, IDS_ToolTip244);
	bSizer175->Add (m_checkBox16, 0, wxALL, 5);
	
	m_checkBox17 = new wxCheckBox (this,
												IDC_Identity,
												wxT("List inverted-inverted covariance matrix"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox17, IDS_ToolTip245);
	bSizer175->Add (m_checkBox17, 0, wxALL, 5);
	
	m_checkBox18 = new wxCheckBox (this,
												IDC_Transformation,
												wxT("Use feature transformation"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox18, IDS_ToolTip246);
	bSizer175->Add (m_checkBox18, 0, wxALL, 5);

   wxBoxSizer* bSizer176 = new wxBoxSizer (wxHORIZONTAL);
//	wxGridSizer* gSizer10;
//	bSizer176 = new wxGridSizer (0, 2, 0, 0);
	
	m_staticText190 = new wxStaticText (this,
													wxID_ANY,
													wxT("Channels:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText190->Wrap (-1);
    SetUpToolTip (m_staticText190, IDS_ToolTip52);
	bSizer176->Add (m_staticText190, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

   GetAllSubsetMenuControl (kChannelsMenu,
										this,
										IDC_ChannelCombo,
										120,
										IDS_ToolTip52);
   bSizer176->Add (m_channelsCtrl, 0, wxALL|wxALIGN_LEFT, 5);
	
   wxBoxSizer* bSizer177 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText191 = new wxStaticText (this,
													wxID_ANY,
													wxT("Classes:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText191->Wrap (-1);
   SetUpToolTip (m_staticText191, IDS_ToolTip103);
	bSizer177->Add (m_staticText191, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

	GetAllSubsetMenuControl (kClassesMenu,
										this,
										IDC_ClassCombo,
										100,
										IDS_ToolTip103);
   bSizer177->Add (m_classesCtrl, 0, wxALL|wxALIGN_LEFT, 5);
	
	
	bSizer175->Add (bSizer176, 0, wxEXPAND, 5);
	bSizer175->Add (bSizer177, 0, wxEXPAND, 5);
	
	bSizer108->Add (bSizer175, 0, wxEXPAND, 5);
	
	
	bSizer107->Add (bSizer108, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12);
	
	wxBoxSizer* bSizer109;
	bSizer109 = new wxBoxSizer (wxVERTICAL);
	
	
	bSizer107->Add (bSizer109, 0, wxEXPAND|wxLEFT|wxRIGHT, 12);

	CreateStandardButtons (bSizer107);
	
	Layout ();
	SetSizerAndFit (bSizer107);
	
}	// end "CreateControls"



Boolean CMEvalCovarianceDialog::DoDialog (
				EvaluateCovarianceSpecsPtr		evaluateCovarianceSpecsPtr)

{
	SInt16   							returnCode = -1;
	UInt16								index;
	Boolean  							continueFlag = FALSE;
   
   		// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																					return (FALSE);
																			
	m_evaluateCovarianceSpecsPtr = evaluateCovarianceSpecsPtr;
   
	if (TransferDataFromWindow ())
		returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{
		continueFlag = TRUE;
		
		gChannelSelection = m_channelSelection; 
		if (m_channelSelection == kSubsetMenuItem)	// Subset of channels or features 
			{
			evaluateCovarianceSpecsPtr->numberFeatures = m_localActiveNumberFeatures; 
			for (index=0; index<(UInt16)m_localNumberFeatures; index++)
				evaluateCovarianceSpecsPtr->featurePtr[index] = m_localFeaturesPtr[index];
												
			}	// end "else m_channelSelection == kSubsetMenuItem"
		
		gClassSelection = m_classSelection;
		if (m_classSelection == kSubsetMenuItem) 
			{                                          
			evaluateCovarianceSpecsPtr->numberClasses = m_localNumberClasses;               
			for (index=0; index<(UInt16)m_localNumberClasses; index++)
				evaluateCovarianceSpecsPtr->classPtr[index] = m_classListPtr[index]; 
			
			}	// end "else m_classSelection == kSubsetMenuItem"
	
		EvaluateCovariancesDialogOK (evaluateCovarianceSpecsPtr,
												m_listOriginalMatrixFlag,
												m_listInvertedMatrixFlag,
												m_listOriginalXInvertedMatrixFlag,
												m_listInvertedInvertedMatrixFlag,
												m_featureTransformationFlag);
		
		}	// end "if (returnCode == IDOK)"
	
	return (continueFlag);
	
}	// end "DoDialog"



void  CMEvalCovarianceDialog::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	UInt16								index;
	Boolean								tempFlag;
	
   
   CMDialog::OnInitDialog (event);
	
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
		
		}	// end "if (numberEigenvectors <= 0)"
		
	else	// numberEigenvectors > 0
		m_featureTransformationFlag =
									m_evaluateCovarianceSpecsPtr->featureTransformationFlag;
									
	tempFlag = CheckFeatureTransformationDialog (this,
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
	                  
	if (TransferDataToWindow ())
		PositionDialogWindow ();
	
}	// end "OnInitDialog"

  

void CMEvalCovarianceDialog::OnChannelComboSelendok (
				wxCommandEvent& 					event)

{
	HandleChannelsMenu (IDC_ChannelCombo,
								m_featureTransformationFlag,
								(SInt16)gProjectInfoPtr->numberStatisticsChannels,
								2,
								TRUE);
	
}	// end "OnChannelComboSelendok"



void CMEvalCovarianceDialog::OnClassComboSelendok (
				wxCommandEvent& 					event)

{                                                                                
	HandleClassesMenu (&m_localNumberClasses,
								(SInt16*)m_classListPtr,
								1,      
								(SInt16)gProjectInfoPtr->numberStatisticsClasses, 
								IDC_ClassCombo,
								&m_classSelection);
	
}	// end "OnClassComboSelendok"



bool CMEvalCovarianceDialog::TransferDataFromWindow ()

{
   wxCheckBox* listOriginalMatrixFlag = (wxCheckBox*)FindWindow (IDC_OrigMatrix);
   m_listOriginalMatrixFlag = listOriginalMatrixFlag->GetValue ();
	
   wxCheckBox* listInvertedMatrixFlag = (wxCheckBox*)FindWindow (IDC_InvertedMatrix);
   m_listInvertedMatrixFlag = listInvertedMatrixFlag->GetValue ();
	
   wxCheckBox* listInvertedInvertedMatrixFlag =
   												(wxCheckBox*)FindWindow (IDC_InvertedMatrix);
   m_listInvertedInvertedMatrixFlag = listInvertedInvertedMatrixFlag->GetValue ();
	
   wxCheckBox* listOriginalXInvertedMatrixFlag =
   														(wxCheckBox*)FindWindow (IDC_Identity);
   m_listOriginalXInvertedMatrixFlag = listOriginalXInvertedMatrixFlag->GetValue ();
	
   wxCheckBox* featureTransformationFlag =
   												(wxCheckBox*)FindWindow (IDC_Transformation);
	m_featureTransformationFlag = featureTransformationFlag->GetValue ();
	
   m_channelSelection = m_channelsCtrl->GetSelection ();
	
   m_classSelection = m_classesCtrl->GetSelection ();
	
   return TRUE;
	
}	// end "TransferDataFromWindow"



bool CMEvalCovarianceDialog::TransferDataToWindow ()

{
   wxCheckBox* listOriginalMatrixFlag = (wxCheckBox*)FindWindow (IDC_OrigMatrix);
   listOriginalMatrixFlag->SetValue (m_listOriginalMatrixFlag);
	
   wxCheckBox* listInvertedMatrixFlag = (wxCheckBox*)FindWindow (IDC_InvertedMatrix);
   listInvertedMatrixFlag->SetValue (m_listInvertedMatrixFlag);
	
   wxCheckBox* listInvertedInvertedMatrixFlag =
   												(wxCheckBox*)FindWindow (IDC_InvertedMatrix);
   listInvertedInvertedMatrixFlag->SetValue (m_listInvertedInvertedMatrixFlag);
	
   wxCheckBox* listOriginalXInvertedMatrixFlag =
   												(wxCheckBox*)FindWindow (IDC_Identity);
   listOriginalXInvertedMatrixFlag->SetValue (m_listOriginalXInvertedMatrixFlag);
	
   wxCheckBox* featureTransformationFlag =
   												(wxCheckBox*)FindWindow (IDC_Transformation);
	featureTransformationFlag->SetValue (m_featureTransformationFlag);
	
   m_channelsCtrl->SetSelection (m_channelSelection);
	
   m_classesCtrl->SetSelection (m_classSelection);
	
   return TRUE;
	
}	// end "TransferDataToWindow"

