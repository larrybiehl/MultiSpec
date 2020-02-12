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
//	File:						WFeatureSelectionDialog.cpp : implementation file
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
//								CMFeatureSelectionDialog & CMFeatureSelectionListDialog
//								classes.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WChannelsDialog.h"
#include "WFeatureSelectionDialog.h"
#include "WMultiSpec.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMFeatureSelectionDialog, CMDialog)
	//{{AFX_MSG_MAP (CMFeatureSelectionDialog)
	ON_BN_CLICKED (IDC_FeatureTransformation, OnFeatureTransformation)
	ON_BN_CLICKED (IDC_ListOptions, OnListOptions)
	ON_BN_CLICKED (IDC_StepProcedure, OnStepProcedure)

	ON_CBN_SELENDOK (IDC_ChannelCombo, OnSelendokChannelCombo)
	ON_CBN_SELENDOK (IDC_ClassCombo, OnSelendokClassCombo)
	ON_CBN_SELENDOK (IDC_ClassPairWeightsCombo, OnSelendokClassPairWeightsCombo)
	ON_CBN_SELENDOK (IDC_DistanceMeasureCombo, OnSelendokDistanceMeasureCombo)
	ON_CBN_SELENDOK (IDC_NumberChannelsCombo, OnSelendokNumberChannelsCombo)

	ON_EN_CHANGE(IDC_ChannelsPerGroup, OnChangeChannelsPerGroup)
	ON_EN_CHANGE(IDC_NumberBestToList, OnChangeNumberBestToList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMFeatureSelectionDialog::CMFeatureSelectionDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMFeatureSelectionDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMFeatureSelectionDialog)
	m_separabilityDistance = -1; 
	m_savedContiguousChannelsPerGroup = 1; 
	m_localCombinationsToList = 1;    
	m_textWindowFlag = FALSE; 
	m_diskFileFlag = FALSE;
	m_channelCombinationSelection = -1;          
	m_interClassWeightsSelection = -1;
	m_searchFlag = FALSE;
	//}}AFX_DATA_INIT 
	
	m_separabilitySpecsPtr = NULL; 
	m_localChannelCombinationsPtr = NULL;
	m_maxContiguousChannelsPerGroup = 1;
	m_localClassPairWeightsListPtr = NULL;
	m_localDefaultClassPairWeight = 0;
	
	m_initializedFlag = CMDialog::m_initializedFlag; 
	
	if (m_initializedFlag)
		m_initializedFlag = GetDialogLocalVectors (&m_localFeaturesPtr,
																	&m_localTransformFeaturesPtr,
																	&m_classListPtr,
																	NULL,
																	NULL,
																	&m_localSymbolsPtr,
																	&m_localChannelCombinationsPtr,
																	&m_localClassPairWeightsListPtr);
	
}	// end "CMFeatureSelectionDialog"



CMFeatureSelectionDialog::~CMFeatureSelectionDialog (void)

{  
	ReleaseDialogLocalVectors (m_localFeaturesPtr,  
											m_localTransformFeaturesPtr,       
											m_classListPtr,
											NULL,
											NULL,
											m_localSymbolsPtr,
											m_localChannelCombinationsPtr,
											m_localClassPairWeightsListPtr);
	
}	// end "~CMFeatureSelectionDialog"



void CMFeatureSelectionDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMFeatureSelectionDialog) 
	DDX_CBIndex (pDX, IDC_DistanceMeasureCombo, m_separabilityListSelection);
	DDX_CBIndex (pDX, IDC_NumberChannelsCombo, m_channelCombinationSelection); 
	DDX_Text (pDX, IDC_ChannelsPerGroup, m_savedContiguousChannelsPerGroup);
	DDV_MinMaxLong (pDX, m_savedContiguousChannelsPerGroup, 1, m_maxContiguousChannelsPerGroup);
	DDX_Text (pDX, IDC_NumberBestToList, m_localCombinationsToList); 
	DDV_MinMaxLong (pDX, m_localCombinationsToList, 1, SInt16_MAX);
	DDX_CBIndex (pDX, IDC_ClassPairWeightsCombo, m_interClassWeightsSelection);
	DDX_Check (pDX, IDC_TextWindow, m_textWindowFlag);
	DDX_Check (pDX, IDC_DiskFile, m_diskFileFlag);
	DDX_Check (pDX, IDC_StepProcedure, m_searchFlag);
	DDX_Check (pDX, IDC_FeatureTransformation, m_featureTransformationFlag);
	DDX_CBIndex (pDX, IDC_ChannelCombo, m_channelSelection);
	DDX_CBIndex (pDX, IDC_ClassCombo, m_classSelection);
	DDX_CBIndex (pDX, IDC_SymbolCombo, m_symbolSelection);       
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the feature selection
//							specification dialog box to the user and copy the
//							revised back to the feature selection specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/28/1998
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean CMFeatureSelectionDialog::DoDialog (
				SeparabilitySpecsPtr				separabilitySpecsPtr,
				UInt16*								allChanCombinationsPtr)

{                                                  
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE; 

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
	
	m_separabilitySpecsPtr = separabilitySpecsPtr;
	                               
	m_allChanCombinationsPtr = allChanCombinationsPtr;
	
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE;
								
		SeparabilityDialogOK (separabilitySpecsPtr,
										m_separabilityDistance,
										m_featureTransformationFlag,
										m_channelSelection,
										m_localActiveNumberFeatures,
										m_localActiveFeaturesPtr,
										m_channelCombinationSelection,
										m_localNumberChannelGroupCombinations,
										m_localChannelCombinationsPtr,
										m_savedContiguousChannelsPerGroup,
										m_searchFlag,
										m_localCombinationsToList,
										m_classSelection,
										m_localNumberClasses,
										m_classListPtr,
										m_interClassWeightsSelection+1,
										m_localClassPairWeightsListPtr,
										m_localDefaultClassPairWeight,
										m_symbolSelection,
										m_localSymbolsPtr,
										m_textWindowFlag,
										m_diskFileFlag);
					                                 
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog" 



void CMFeatureSelectionDialog::HandleChannelCombinationsMenu (
				UInt16								channelCombinationsMenuItemNumber)

{  
	Boolean								returnFlag;
	
	                                                         
	DDX_CBIndex (m_dialogFromPtr,
						channelCombinationsMenuItemNumber, 
						m_channelCombinationSelection);
	                                           
	if (m_channelCombinationSelection == kSubsetMenuItem)
		{
				// Subset of channels to be used.							
								                
		SetDLogControlHilite (NULL, IDOK, 255);  
		
		CMChannelCombinationsDlg*		channelCombinationsDialogPtr = NULL;
		
		TRY
			{                                
			channelCombinationsDialogPtr = new CMChannelCombinationsDlg ();
			                                                                     
			returnFlag = channelCombinationsDialogPtr->DoDialog (
											m_channelCombinationSelection,
											&m_localNumberChannelGroupCombinations,
											m_localChannelCombinationsPtr,
											m_separabilitySpecsPtr->numberAllChanCombinations,
											m_allChanCombinationsPtr,
											m_savedContiguousChannelsPerGroup,
											m_featureTransformationFlag);
		   
		   if (channelCombinationsDialogPtr != NULL)                           
				delete channelCombinationsDialogPtr;
				
			}	// end "TRY"
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			
			}
		
		END_CATCH_ALL 
		                          				
		if (m_localNumberChannelGroupCombinations == 
										m_separabilitySpecsPtr->numberAllChanCombinations)
			m_channelCombinationSelection = kAllMenuItem; 
			
		if (m_channelCombinationSelection == kAllMenuItem)
			DDX_CBIndex (m_dialogToPtr,
								channelCombinationsMenuItemNumber,
								m_channelCombinationSelection);
		                  						                                
		SetDLogControlHilite (NULL, IDOK, 0);
						
		}	// end "if (m_channelCombinationsSelection == kSubsetMenuItem)"          
	
}	// end "HandleChannelCombinationsMenu"  



void CMFeatureSelectionDialog::OnChangeChannelsPerGroup ()

{  
	SInt32								newContiguousChannelsPerGroup;
	
	
	DDX_Text (m_dialogFromPtr,
					IDC_ChannelsPerGroup, 
					newContiguousChannelsPerGroup);
					
	if (newContiguousChannelsPerGroup < 1 || 
						newContiguousChannelsPerGroup > m_maxContiguousChannelsPerGroup)
		{					                  
		DDX_Text (m_dialogToPtr,
						IDC_ChannelsPerGroup,
						m_savedContiguousChannelsPerGroup);
					
		DDV_MinMaxLong (m_dialogFromPtr,
								newContiguousChannelsPerGroup,
								1,
								m_maxContiguousChannelsPerGroup);
							
		}	// end "if (newContiguousChannelsPerGroup < 1 || ..."
		
	else	// newContiguousChannelsPerGroup in proper range
		{ 
		if (m_savedContiguousChannelsPerGroup != newContiguousChannelsPerGroup)
			{
			m_savedContiguousChannelsPerGroup = newContiguousChannelsPerGroup;
															            
			SeparabilityDialogUpdateChannelCombinationItems (
														this,
														m_separabilitySpecsPtr,
														TRUE,
														m_localActiveNumberFeatures,
														m_allChanCombinationsPtr,
														m_savedContiguousChannelsPerGroup,
														m_searchFlag,
														m_channelCombinationSelection,
														m_localChannelCombinationsPtr,
														&m_localNumberChannelGroupCombinations,
														&m_savedNumberChannelGroupCombinations,
														&m_localCombinationsToList, 
														&m_maxContiguousChannelsPerGroup);
														      
			SeparabilityDialogUpdateChannelFeatureGroupText (
														this, 
														(SInt16)m_savedContiguousChannelsPerGroup,
														m_featureTransformationFlag);
														
			}	// end "if (m_savedContiguousChannelsPerGroup != ..."
														
		}	// end "else newContiguousChannelsPerGroup in proper range"                                                          
	
}	// end "OnChangeChannelsPerGroup"



void CMFeatureSelectionDialog::OnChangeNumberBestToList ()

{                                                           
	SInt32								newCombinationsToList;
	
	
	DDX_Text (m_dialogFromPtr,
					IDC_NumberBestToList, 
					newCombinationsToList);
					
	if (newCombinationsToList < 1 || newCombinationsToList > SInt16_MAX)
		{					                  
		DDX_Text (m_dialogToPtr,
						IDC_NumberBestToList,
						m_localCombinationsToList);
					
		DDV_MinMaxLong (m_dialogFromPtr,
								newCombinationsToList,
								1,
								SInt16_MAX);
							
		}	// end "if (newCombinationsToList < 1 || ..."
		
	else	// newCombinationsToList in proper range
		m_localCombinationsToList = newCombinationsToList;
	
}	// end "OnChangeNumberBestToList"



void CMFeatureSelectionDialog::OnFeatureTransformation ()

{  
	SInt16								channelSelection;
	
	      
	DDX_Check (m_dialogFromPtr, 
					IDC_FeatureTransformation, 
					m_featureTransformationFlag);
					                                                    
	SeparabilityDialogCheckFeatureTransformation (
															this,
															(Boolean*)&m_featureTransformationFlag,
															&m_localActiveNumberFeatures,
															&m_localActiveFeaturesPtr,
															m_featureTransformAllowedFlag,
															IDC_FeatureTransformation, 
															IDC_ChannelPrompt,
															&channelSelection,
															m_localNumberFeatures,
															m_localFeaturesPtr, 
															m_localNumberTransformFeatures,
															m_localTransformFeaturesPtr);
	
	m_channelSelection = channelSelection;
																	
	DDX_CBIndex (m_dialogToPtr, IDC_ChannelCombo, m_channelSelection);
															            
	SeparabilityDialogUpdateChannelCombinationItems (
														this,
														m_separabilitySpecsPtr,
														TRUE,
														m_localActiveNumberFeatures,
														m_allChanCombinationsPtr,
														m_savedContiguousChannelsPerGroup,
														m_searchFlag,
														m_channelCombinationSelection,
														m_localChannelCombinationsPtr,
														&m_localNumberChannelGroupCombinations,
														&m_savedNumberChannelGroupCombinations,
														&m_localCombinationsToList, 
														&m_maxContiguousChannelsPerGroup);
														      
	SeparabilityDialogUpdateChannelFeatureGroupText (
														this, 
														(SInt16)m_savedContiguousChannelsPerGroup,
														m_featureTransformationFlag);
	
}	// end "OnFeatureTransformation"



BOOL CMFeatureSelectionDialog::OnInitDialog ()

{
	SInt16								channelCombinationSelection,
											channelSelection,
											classSelection,
											interClassWeightsSelection,
											symbolSelection;
	
	
	CDialog::OnInitDialog ();
	
			// Initialize dialog variables.

	SeparabilityDialogInitialize (this,
											m_separabilitySpecsPtr,
											m_localFeaturesPtr,
											m_localTransformFeaturesPtr,
											m_classListPtr,
											m_localSymbolsPtr,
											m_localChannelCombinationsPtr,
											&m_localClassPairWeightsListPtr,
											&m_separabilityDistance,
											&m_numberEigenvectors,
											(Boolean*)&m_featureTransformAllowedFlag,
											(Boolean*)&m_featureTransformationFlag,
											&channelSelection,
											&m_localActiveNumberFeatures,
											&channelCombinationSelection,
											&m_localNumberChannelGroupCombinations, 
											&m_savedNumberChannelGroupCombinations,
											&m_savedContiguousChannelsPerGroup,
											(Boolean*)&m_searchFlag,
											&m_maxContiguousChannelsPerGroup,
											&m_localCombinationsToList,
											&classSelection,
											&m_localNumberClasses,
											&m_numberClassCombinations,
											&interClassWeightsSelection,
											&m_localDefaultClassPairWeight,
											&symbolSelection,
											(Boolean*)&m_textWindowFlag,
											(Boolean*)&m_diskFileFlag); 
	
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
		
			// Get the feature selection algorithm list selection that matches the input
			// feature selection algorithm.
			                             
	m_separabilityListSelection = GetComboListSelection (IDC_DistanceMeasureCombo,
																			m_separabilityDistance);
		
	if (m_separabilityListSelection == -1)
		m_separabilityListSelection = 0; 
	                                                                   			
			//	Set the channels/features list item				
			                                             
	m_availableFeaturePtr = gProjectInfoPtr->channelsPtr;
	m_channelSelection = channelSelection;
	
			// Number channel combinations.
			
	m_channelCombinationSelection = channelCombinationSelection;
	
			// Current maximum number of channel combinations 

	LoadDItemRealValue (this,
								IDC_MaxSearchCombinations,
								(double)m_separabilitySpecsPtr->maxNumberFeatureCombinations,
								0);
	
			// Classes to use.         
	
	m_classSelection = classSelection;					
	::LoadDItemValue (this, IDC_NumberClassCombinations, m_numberClassCombinations);
	
			// Interclass weights to use.             
			// Adjust for 0 base index.
			// Unequal interclass weights are not available yet.																	
			                                                                  
	m_interClassWeightsSelection = interClassWeightsSelection - 1; 
	HideDialogItem (this, IDC_WeightsEqual);                                            
	
			// Symbols to use.             
			// Adjust for 0 base index.
			// User defined symbols are not available yet.																	
			                                                                  
	m_symbolSelection = symbolSelection - 1; 
	HideDialogItem (this, IDC_SymbolCombo);                                            
	((CComboBox*)GetDlgItem (IDC_SymbolCombo))->DeleteString (1);
	                 
	if (UpdateData (FALSE))
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item	
		                                       
	SelectDialogItemText (this, IDC_ChannelsPerGroup, 0, SInt16_MAX);
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"



void CMFeatureSelectionDialog::OnListOptions ()

{  
	 
	SetDLogControlHilite (NULL, IDOK, 255);  
		
	SeparabilityListDialog (&m_localCombinationsToList); 
	
	SetDLogControlHilite (NULL, IDOK, 0);
	  
	DDX_Text (m_dialogToPtr,
					IDC_NumberBestToList,
					m_localCombinationsToList);
	
}	// end "OnListOptions"

 

void CMFeatureSelectionDialog::OnOK ()

{  
	SInt16								index;
	
	                                    
	if (m_separabilitySpecsPtr->maxNumberFeatureCombinations <= 0)
		{
		index = IDS_FeatureSelection11;
				
		if (m_featureTransformationFlag)
			index = IDS_FeatureSelection12;
				
		if (m_savedContiguousChannelsPerGroup > 1)
			index = IDS_FeatureSelection13;
					                                    
		SetDLogControlHilite (NULL, IDOK, 255); 
				
		DisplayAlert (kErrorAlertID, 1, kFeatureSelectionStrID, index, 0, NULL);
				                                          
		SetDLogControlHilite (NULL, IDOK, 0); 
				
		}	// end "if (m_separabilitySpecsPtr->maxNumberFeatureCombinations <= 0)" 
	
	else	// everything is 'okay'
		CMDialog::OnOK ();
	
}	// end "OnOK"



void CMFeatureSelectionDialog::OnSelendokChannelCombo ()

{                                                                                  
	HandleChannelsMenu (IDC_ChannelCombo,
								m_featureTransformationFlag,
								(SInt16)gProjectInfoPtr->numberStatisticsChannels,
								2,
								TRUE);   
															            
	SeparabilityDialogUpdateChannelCombinationItems (
														this,
														m_separabilitySpecsPtr,
														TRUE,
														m_localActiveNumberFeatures,
														m_allChanCombinationsPtr,
														m_savedContiguousChannelsPerGroup,
														m_searchFlag,
														m_channelCombinationSelection,
														m_localChannelCombinationsPtr,
														&m_localNumberChannelGroupCombinations,
														&m_savedNumberChannelGroupCombinations,
														&m_localCombinationsToList,
														&m_maxContiguousChannelsPerGroup);
	
}	// end "OnSelendokChannelCombo"



void CMFeatureSelectionDialog::OnSelendokClassCombo ()

{                                                                                
	HandleClassesMenu (&m_localNumberClasses,
								(SInt16*)m_classListPtr,
								2,      
								(SInt16)gProjectInfoPtr->numberStatisticsClasses, 
								IDC_ClassCombo,
								&m_classSelection); 
								
	SeparabilityDialogUpdateNumberClassCombintations (this,
																		m_classSelection,
																		&m_localNumberClasses,
																		m_classListPtr);
	
}	// end "OnSelendokClassCombo"

  

void CMFeatureSelectionDialog::OnSelendokClassPairWeightsCombo ()

{                                                           
	HandleClassPairWeightsMenu (&m_localClassPairWeightsListPtr,
											IDC_ClassPairWeightsCombo,
											&m_interClassWeightsSelection,
											&m_localDefaultClassPairWeight);
	
}	// end "OnSelendokClassPairWeightsCombo"



void CMFeatureSelectionDialog::OnSelendokDistanceMeasureCombo ()

{  
	CComboBox* 							comboBoxPtr;
	
	SInt16								channelSelection,
											separabilityDistance,
											savedSeparabilityListSelection;
	                  
	Boolean								returnFlag = TRUE;
	
	
	savedSeparabilityListSelection = m_separabilityListSelection;
	                                    
	DDX_CBIndex (m_dialogFromPtr,
						IDC_DistanceMeasureCombo,
						m_separabilityListSelection);
					
			// Get the actual classification procedure code.
			
	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_DistanceMeasureCombo);
	separabilityDistance = 
					(SInt16)comboBoxPtr->GetItemData (m_separabilityListSelection);
		
	if (separabilityDistance != 0)
		{                                                                                                    
		m_separabilityDistance = separabilityDistance; 
					
		m_featureTransformAllowedFlag =
								SeparabilityDialogGetFeatureTransformAllowedFlag (
																				m_separabilityDistance,
																				m_numberEigenvectors);
																		
		SeparabilityDialogCheckFeatureTransformation (
															this,
															(Boolean*)&m_featureTransformationFlag,
															&m_localActiveNumberFeatures,
															&m_localActiveFeaturesPtr,
															m_featureTransformAllowedFlag,
															IDC_FeatureTransformation, 
															IDC_ChannelPrompt,
															&channelSelection,
															m_localNumberFeatures,
															m_localFeaturesPtr, 
															m_localNumberTransformFeatures,
															m_localTransformFeaturesPtr); 
			
		m_channelSelection = channelSelection;
																	
		DDX_CBIndex (m_dialogToPtr, IDC_ChannelCombo, m_channelSelection);
		
		if (m_featureTransformationFlag && !m_featureTransformAllowedFlag)
			{												            
			SeparabilityDialogUpdateChannelCombinationItems (
														this,
														m_separabilitySpecsPtr,
														TRUE,
														m_localActiveNumberFeatures,
														m_allChanCombinationsPtr,
														m_savedContiguousChannelsPerGroup,
														m_searchFlag,
														m_channelCombinationSelection,
														m_localChannelCombinationsPtr,
														&m_localNumberChannelGroupCombinations,
														&m_savedNumberChannelGroupCombinations,
														&m_localCombinationsToList, 
														&m_maxContiguousChannelsPerGroup);
														      
			SeparabilityDialogUpdateChannelFeatureGroupText (
														this, 
														(SInt16)m_savedContiguousChannelsPerGroup,
														m_featureTransformationFlag);
															
			}	// end "if (m_featureTransformFlag && !m_featureTransformAllowedFlag)"
						
		}	// end "if (separabilityDistance != 0)"
		
	else	// separabilityDistance == 0
		{                      
		m_separabilityListSelection = savedSeparabilityListSelection; 
		
		}	// end "else separabilityDistance == 0" 
	                                    
	DDX_CBIndex (m_dialogToPtr,
						IDC_DistanceMeasureCombo,
						m_separabilityListSelection);
						 
	m_separabilityDistance = 
						(SInt16)comboBoxPtr->GetItemData (m_separabilityListSelection);
	
}	// end "OnSelendokDistanceMeasureCombo"



void CMFeatureSelectionDialog::OnSelendokNumberChannelsCombo ()

{                                             
	HandleChannelCombinationsMenu (IDC_NumberChannelsCombo);
															            
	SeparabilityDialogUpdateChannelCombinationItems (
														this,
														m_separabilitySpecsPtr,
														FALSE,
														m_localActiveNumberFeatures,
														m_allChanCombinationsPtr,
														m_savedContiguousChannelsPerGroup,
														m_searchFlag,
														m_channelCombinationSelection,
														m_localChannelCombinationsPtr,
														&m_localNumberChannelGroupCombinations,
														&m_savedNumberChannelGroupCombinations,
														&m_localCombinationsToList, 
														&m_maxContiguousChannelsPerGroup);
	
}	// end "OnSelendokNumberChannelsCombo"
 		


void CMFeatureSelectionDialog::OnStepProcedure ()

{  
   DDX_Check (m_dialogFromPtr, IDC_StepProcedure, m_searchFlag);
															            
	SeparabilityDialogUpdateChannelCombinationItems (
														this,
														m_separabilitySpecsPtr,
														TRUE,
														m_localActiveNumberFeatures,
														m_allChanCombinationsPtr,
														m_savedContiguousChannelsPerGroup,
														m_searchFlag,
														m_channelCombinationSelection,
														m_localChannelCombinationsPtr,
														&m_localNumberChannelGroupCombinations,
														&m_savedNumberChannelGroupCombinations,
														&m_localCombinationsToList,
														&m_maxContiguousChannelsPerGroup);
	
}	// end "OnStepProcedure"



//------------------------------------------------------------------------------------
// CMFeatureSelectionListDialog

BEGIN_MESSAGE_MAP (CMFeatureSelectionListDialog, CMDialog)
	//{{AFX_MSG_MAP (CMFeatureSelectionListDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMFeatureSelectionListDialog::CMFeatureSelectionListDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMFeatureSelectionListDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMFeatureSelectionListDialog)
	m_sortChannelCombinations = -1;
	m_sortChannelCombinations = 0;
	m_thresholdedGroupTableFlag = FALSE;
	m_thresholdedClassPairTableFlag = FALSE;
	m_localDistancesLessThanToGroup = 0;
	m_localDistancesLessThanToList = 0;
	m_listClassPairDistancesFlag = FALSE;
	m_localMinDistanceRangeToList = 0;
	m_localMaxDistanceRangeToList = 0;
	m_localCombinationsToList = 1;
	m_separabilityTableFlag = FALSE;
	//}}AFX_DATA_INIT

	m_initializedFlag = CMDialog::m_initializedFlag;
	
}	// end "CMFeatureSelectionListDialog"



void CMFeatureSelectionListDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMFeatureSelectionListDialog)
	DDX_Radio (pDX, IDC_AverageClassDistance, m_sortChannelCombinations);
	DDX_Check (pDX, IDC_ClassGroupThresholdTable, m_thresholdedGroupTableFlag);
	DDX_Check (pDX, IDC_ClassPairThresholdTable, m_thresholdedClassPairTableFlag);
	DDX_Text2 (pDX, IDC_ClassGroupThreshold, m_localDistancesLessThanToGroup);
	DDV_MinMaxDouble (pDX, m_localDistancesLessThanToGroup, 0, 32767);
	DDX_Text2 (pDX, IDC_ClassPairThreshold, m_localDistancesLessThanToList);
	DDV_MinMaxDouble (pDX, m_localDistancesLessThanToList, 0, 32767);
	DDX_Check (pDX, IDC_ListClassPairDistances, m_listClassPairDistancesFlag);
	DDX_Text2 (pDX, IDC_MinDistanceRange, m_localMinDistanceRangeToList);
	DDV_MinMaxDouble (pDX, m_localMinDistanceRangeToList, 0, 32767);
	DDX_Text2 (pDX, IDC_MaxDistanceRange, m_localMaxDistanceRangeToList);
	DDV_MinMaxDouble (pDX, m_localMaxDistanceRangeToList, 0, 32767);
	DDX_Text (pDX, IDC_NumberBestCombinations, m_localCombinationsToList);
	DDV_MinMaxLong (pDX, m_localCombinationsToList, 1, 32767);
	DDX_Check (pDX, IDC_SeparabilityTable, m_separabilityTableFlag);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the feature
//							selection list dialog box to the user and copy the
//							revised back to the feature selection specification structure 
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
//	Coded By:			Larry L. Biehl			Date: 05/06/1998
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean CMFeatureSelectionListDialog::DoDialog (
				SeparabilitySpecsPtr				separabilitySpecsPtr,
				SInt32*								combinationsToListPtr)

{                                                  
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE; 

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
	
	m_separabilitySpecsPtr = separabilitySpecsPtr;
	m_combinationsToListPtr = combinationsToListPtr;
	
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE;  
		
				// Adjust sort channel combinations variable to be 1 based.
			
		m_sortChannelCombinations++;
				
		SeparabilityListDialogOK (m_separabilitySpecsPtr,
											m_combinationsToListPtr,
											m_separabilityTableFlag,
											m_localCombinationsToList,
											(float)m_localMinDistanceRangeToList,
											(float)m_localMaxDistanceRangeToList,
											(m_sortChannelCombinations == 1),
											(m_sortChannelCombinations == 2),
											(m_sortChannelCombinations == 0),
											m_thresholdedClassPairTableFlag,
											m_localDistancesLessThanToList,
											m_thresholdedGroupTableFlag,
											m_localDistancesLessThanToGroup,
											m_listClassPairDistancesFlag);
					                                 
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog" 



BOOL CMFeatureSelectionListDialog::OnInitDialog ()

{
	SInt16								sortChannelCombinations;
	
	
	CDialog::OnInitDialog ();
	
			// Initialize dialog variables.

	SeparabilityListDialogInitialize (this,
													m_separabilitySpecsPtr,
													*m_combinationsToListPtr,
													(Boolean*)&m_separabilityTableFlag,
													&m_localCombinationsToList,
													&m_localMinDistanceRangeToList,
													&m_localMaxDistanceRangeToList,
													&sortChannelCombinations,
													(Boolean*)&m_thresholdedClassPairTableFlag,
													&m_localDistancesLessThanToList,
													(Boolean*)&m_thresholdedGroupTableFlag,
													&m_localDistancesLessThanToGroup,
													(Boolean*)&m_listClassPairDistancesFlag);
		
			// Adjust sort channel combinations variable to be 0 based.
			
	m_sortChannelCombinations = sortChannelCombinations - 1;					
	                 
	if (UpdateData (FALSE))
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item	
		                                       
	SelectDialogItemText (this, IDC_NumberBestCombinations, 0, SInt16_MAX);
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"
