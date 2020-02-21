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
//	File:						WFeatureExtractionDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/04/2018
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMFeatureExtractionDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WChannelsDialog.h" 
#include "WFeatureExtractionDialog.h"
#include "WProjectionPursuitDialog.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMFeatureExtractionDialog, CMDialog)
	//{{AFX_MSG_MAP (CMFeatureExtractionDialog)
	ON_BN_CLICKED (IDC_OptimizeClasses, OnOptimizeClasses)
	ON_BN_CLICKED (IDC_SpecialOptions, OnSpecialOptions)

	ON_CBN_SELENDOK (IDC_ChannelCombo, OnSelendokChannelCombo)
	ON_CBN_SELENDOK (IDC_ClassCombo, OnSelendokClassCombo)
	ON_CBN_SELENDOK (IDC_ClassPairWeightsCombo, OnSelendokClassPairWeightsCombo)
	ON_CBN_SELENDOK (IDC_FEAlgorithmCombo, OnDecisionBoundary)
	ON_CBN_SELENDOK (IDC_PreprocessingCombo, OnCbnSelendokPreprocessingcombo)
	ON_CBN_SELENDOK (IDC_WeightsCombo, OnSelendokClassWeightsCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMFeatureExtractionDialog::CMFeatureExtractionDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMFeatureExtractionDialog::IDD, pParent)

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
		m_initializedFlag = GetDialogLocalVectors (&m_localActiveFeaturesPtr,
																	NULL,
																	&m_classListPtr,
																	NULL,
																	&m_classWeightsPtr,
																	NULL,
																	NULL,
																	&m_localClassPairWeightsListPtr);
														
}	// end "CMFeatureExtractionDialog"



CMFeatureExtractionDialog::~CMFeatureExtractionDialog (void)

{  
	ReleaseDialogLocalVectors (m_localActiveFeaturesPtr,  
											NULL,       
											m_classListPtr,
											NULL,
											m_classWeightsPtr,
											NULL,
											NULL,
											m_localClassPairWeightsListPtr);
	
}	// end "CMFeatureExtractionDialog"



void CMFeatureExtractionDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMFeatureExtractionDialog)
	DDX_Text2 (pDX, IDC_InterclassThreshold, m_interclassThrehold);
	DDX_Text2 (pDX, IDC_WithinClassThreshold, m_withinClassThreshold);
	DDX_Check (pDX, IDC_OptimizeClasses, m_optimizeClassesFlag);
	DDX_Text (pDX, IDC_MinThresholdNumber, m_minThresholdNumber);
	DDX_Text (pDX, IDC_MaxPixels, m_maxPixelsPerClass);
	DDV_MinMaxLong (pDX, m_maxPixelsPerClass, 0, 32000);
	DDX_Check (pDX, IDC_SpecialOptions, m_specialOptionsFlag);
	DDX_Check (pDX, IDC_ListTransformationMatrix, m_listTransformationFlag);
	DDX_CBIndex (pDX, IDC_ClassPairWeightsCombo, m_interClassWeightsSelection);
	DDX_CBIndex (pDX, IDC_PreprocessingCombo, m_preprocessSelection);
	DDX_CBIndex (pDX, IDC_FEAlgorithmCombo, m_algorithmCode);
	DDX_CBIndex (pDX, IDC_ChannelCombo, m_channelSelection);
	DDX_CBIndex (pDX, IDC_ClassCombo, m_classSelection);
	DDX_CBIndex (pDX, IDC_WeightsCombo, m_weightsSelection);
	DDX_Text2 (pDX, IDC_PercentAccuracy, m_optimizeThreshold);
	DDV_MinMaxDouble (pDX, m_optimizeThreshold, 0., 100.);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"
 


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the feature
//							extraction specification dialog box to the user and copy the
//							revised back to the feature extraction specification structure
//							if the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/02/1999
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean CMFeatureExtractionDialog::DoDialog (
				FeatureExtractionSpecsPtr		featureExtractionSpecsPtr)

{                                                  
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE; 

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																							return (FALSE);
	
	m_featureExtractionSpecsPtr = featureExtractionSpecsPtr;  
	
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
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
												m_preprocessSelection+1);
														
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



BOOL CMFeatureExtractionDialog::OnInitDialog ()

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
	
	
	CDialog::OnInitDialog ();
	
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
													(Boolean*)&m_listTransformationFlag);
	                                                                   			
			//	Set the channels/features list item				
			                                             
	m_availableFeaturePtr = gProjectInfoPtr->channelsPtr;
	m_channelSelection = channelSelection;
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
	projectionPursuitSpecsPtr = &m_featureExtractionSpecsPtr->projectionPursuitParams;
	if (projectionPursuitSpecsPtr->finalNumberFeatures <= 0)                           
		((CComboBox*)GetDlgItem (IDC_PreprocessingCombo))->DeleteString (2);
	
			// Class weights.
			// Adjust for 0 base index.
	
	m_weightsSelection = weightsSelection - 1;
	HideDialogItem (this, IDC_WeightsEqual);   
	
			// Inter class weights.
			// Adjust for 0 base index.
	
	m_interClassWeightsSelection = interClassWeightsSelection - 1;
	HideDialogItem (this, IDC_InterClassWeightsEqual);
	
	
	
	m_interclassThrehold = (float)interclassThrehold;
	m_optimizeThreshold = (float)optimizeThreshold;
	m_withinClassThreshold = (float)withinClassThreshold;
	                 
	if (UpdateData (FALSE))                   
		PositionDialogWindow ();
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"



void CMFeatureExtractionDialog::OnOptimizeClasses ()

{         
	DDX_Check (m_dialogFromPtr,
					IDC_OptimizeClasses, 
					m_optimizeClassesFlag);  
					
	FeatureExtractionDialogOptimizeClass (this, m_optimizeClassesFlag);                                                     
	
}	// end "OnOptimizeClasses"



void CMFeatureExtractionDialog::OnSpecialOptions ()

{                                       
	DDX_Check (m_dialogFromPtr, IDC_SpecialOptions, m_specialOptionsFlag);
	
	FeatureExtractionDialogUpdateSpecialOptions (this,
																m_algorithmCode+1,
																m_specialOptionsFlag,
																m_optimizeClassesFlag);
	
}	// end "OnSpecialOptions"



void CMFeatureExtractionDialog::OnDecisionBoundary ()

{                                                 
	DDX_CBIndex (m_dialogFromPtr, IDC_FEAlgorithmCombo, m_algorithmCode);
					                     
	FeatureExtractionDialogAlgorithm (this, m_algorithmCode+1);                                                        
	
	FeatureExtractionDialogUpdateSpecialOptions (this,
																m_algorithmCode+1,
																m_specialOptionsFlag,
																m_optimizeClassesFlag);
														 
}	// end "OnDecisionBoundary"



void CMFeatureExtractionDialog::OnSelendokChannelCombo ()

{                                                                                                       
	HandleChannelsMenu (IDC_ChannelCombo,
								kNoTransformation,
								(SInt16)gProjectInfoPtr->numberStatisticsChannels,
								m_channelListType,
								TRUE);   
	
}	// end "OnSelendokChannelCombo"

  

void CMFeatureExtractionDialog::OnSelendokClassWeightsCombo ()

{                                                           
	HandleClassWeightsMenu ((SInt16)m_localNumberClasses, 
									(SInt16*)m_classListPtr,
									m_classWeightsPtr,
									gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
									IDC_WeightsCombo,
									&m_weightsSelection);
	
}	// end "OnSelendokClassWeightsCombo"

  

void CMFeatureExtractionDialog::OnSelendokClassPairWeightsCombo ()

{                                                           
	HandleClassPairWeightsMenu (&m_localClassPairWeightsListPtr,
											IDC_ClassPairWeightsCombo,
											&m_interClassWeightsSelection,
											&m_localDefaultClassPairWeight);
	
}	// end "OnSelendokClassPairWeightsCombo"



void CMFeatureExtractionDialog::OnCbnSelendokPreprocessingcombo ()

{   
	Boolean								okFlag;


	DDX_CBIndex (m_dialogFromPtr, IDC_PreprocessingCombo, m_preprocessSelection);

	if (m_preprocessSelection == 1)
		okFlag = ProjectionPursuitDialog ();

}	// end "OnCbnSelendokPreprocessingcombo"
