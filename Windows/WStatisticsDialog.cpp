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
//	File:						WStatisticsDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			08/16/2019
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMStatisticsDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include	"WChannelsDialog.h"
#include "WStatisticsDialog.h"



BEGIN_MESSAGE_MAP (CMStatisticsDialog, CMDialog)
	//{{AFX_MSG_MAP (CMStatisticsDialog)
	ON_BN_CLICKED (IDC_ShowClassNames, OnShowClassNames)
	ON_BN_CLICKED (IDC_ShowFieldNames, OnShowFieldNames)
	ON_BN_CLICKED (IDC_ShowTrainTestLabel, OnShowTrainTestLabel)
	ON_BN_CLICKED (IDC_StatisticsOptions, OnStatisticsOptions)
	ON_BN_CLICKED (IDC_TestFields, OnTestFields)
	ON_BN_CLICKED (IDC_TrainingFields, OnTrainingFields)

	ON_CBN_DROPDOWN (IDC_Channels, OnDropdownChannels)
	ON_CBN_DROPDOWN (IDC_ProjectChanges, OnDropdownProjectChanges)
	ON_CBN_DROPDOWN (IDC_TestMaskPopUp, OnDropdownTestMaskCOMBO)
	ON_CBN_DROPDOWN (IDC_TrainMaskPopUp, OnDropdownTrainMaskCOMBO)

	ON_CBN_SELENDOK (IDC_Channels, OnSelendokChannels)
	ON_CBN_SELENDOK (IDC_ProjectChanges, OnSelendokProjectChanges)
	ON_CBN_SELENDOK (IDC_TestMaskPopUp, OnSelendokTestMaskCOMBO)
	ON_CBN_SELENDOK (IDC_TrainMaskPopUp, OnSelendokTrainMaskCOMBO)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMStatisticsDialog::CMStatisticsDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMStatisticsDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT(CMStatisticsDialog)
	m_showFieldNames = FALSE;
	m_showTestFields = FALSE;
	m_showTrainingFields = FALSE;
	m_projectCommands = 0;
	m_showClassNames = FALSE;
	m_showTrainTestText = FALSE;
	m_outlineColorSelection = -1;
	m_testMaskCombo = -1;
	m_trainMaskCombo = -1;
	//}}AFX_DATA_INIT
	
	m_totalNumberChannels = 0; 
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
}	// end "CMStatisticsDialog"



void CMStatisticsDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMStatisticsDialog)                      
	DDX_Check (pDX, IDC_ShowFieldNames, m_showFieldNames);
	DDX_Check (pDX, IDC_TestFields, m_showTestFields);
	DDX_Check (pDX, IDC_TrainingFields, m_showTrainingFields);
	DDX_CBIndex (pDX, IDC_ProjectChanges, m_projectCommands);
	DDX_Check (pDX, IDC_ShowClassNames, m_showClassNames);
	DDX_Check (pDX, IDC_ShowTrainTestLabel, m_showTrainTestText);
	DDX_CBIndex (pDX, IDC_ColorCombo, m_outlineColorSelection);
	DDX_CBIndex (pDX, IDC_Channels, m_channelSelection);
	DDX_CBIndex (pDX, IDC_TestMaskPopUp, m_testMaskCombo);
	DDX_CBIndex (pDX, IDC_TrainMaskPopUp, m_trainMaskCombo);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the statistics
//							specification dialog box to the user and copy the
//							revised back to the statistics specification structure if
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
//	Coded By:			Larry L. Biehl			Date: 02/13/1996
//	Revised By:			Larry L. Biehl			Date: 02/26/1999

SInt16 CMStatisticsDialog::DoDialog (
				SInt16*								featurePtr,
				SInt16								totalNumberChannels,
				Handle*								trainMaskFileInfoHandlePtr,
				Handle*								testMaskFileInfoHandlePtr)

{  
	INT_PTR								returnCode;
	
	SInt16								statisticsRequest;

	Boolean								continueFlag = FALSE;
	

			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
																			
	m_localFeaturesPtr = (UInt16*)featurePtr;  
	m_totalNumberChannels = totalNumberChannels;
	m_trainMaskFileInfoHandle = *trainMaskFileInfoHandlePtr;
	m_testMaskFileInfoHandle = *testMaskFileInfoHandlePtr;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{                 
		statisticsRequest = 1; 
				
		StatisticsDialogOK (m_channelSelection,
									m_totalNumberChannels,
									m_localActiveFeaturesPtr, 
									m_localActiveNumberFeatures, 
									m_showTrainingFields,
									m_showTestFields,
									m_showClassNames, 
									m_showFieldNames,
									m_showTrainTestText,  
									m_outlineColorSelection + 1,
									m_localStatCode,       
									m_keepClassStatsFlag,
									m_variance,
									m_minLogDeterminant,
									m_useCommonCovarianceInLOOCFlag); 
									
		continueFlag = TRUE;
		
		}	// end "if (returnCode == IDOK)"
		
	CheckAndUnlockHandle (gProjectInfoPtr->windowInfoHandle); 
	
	*trainMaskFileInfoHandlePtr = m_trainMaskFileInfoHandle;
	*testMaskFileInfoHandlePtr = m_testMaskFileInfoHandle;
		
	return (continueFlag);
		
}	// end "DoDialog"



void CMStatisticsDialog::OnDropdownChannels ()

{
	UpdateAllSubsetList (IDC_Channels);
	
}	// end "OnDropdownChannels"



void CMStatisticsDialog::OnDropdownProjectChanges ()

{
	if (gProjectInfoPtr->statsLoaded || gProjectInfoPtr->pixelDataLoadedFlag)
		{
				// Make sure that the clear statistics command is in the list.
		
		if (((CComboBox*)GetDlgItem (IDC_ProjectChanges))->GetCount () == 1)
			 ((CComboBox*)GetDlgItem (IDC_ProjectChanges))->
													AddString ((LPCTSTR)_T("Clear Statistics..."));
		
		}	// end "if (gProjectInfoPtr->statsLoaded || ..."
	
	else	// !gProjectInfoPtr->statsLoaded && ...
		{
				// Make sure that the clear statistics command is not in the list.
		
		if (((CComboBox*)GetDlgItem (IDC_ProjectChanges))->GetCount () == 2)
			 ((CComboBox*)GetDlgItem (IDC_ProjectChanges))->DeleteString (1);
		
		}	// end "else !gProjectInfoPtr->statsLoaded && ..."
	
}	// end "OnDropdownProjectChanges"



void CMStatisticsDialog::OnDropdownTestMaskCOMBO ()

{
			// Add your control notification handler code here
	
}	// end "OnDropdownTestMaskCOMBO"



void CMStatisticsDialog::OnDropdownTrainMaskCOMBO ()

{
			// Add your control notification handler code here
	
}	// end "OnDropdownTrainMaskCOMBO"



BOOL CMStatisticsDialog::OnInitDialog ()

{  
	SInt16								localChannelSelection,
											localLabelFieldCode,
											localOutlineColorSelection,
											localOutlineFieldType,
											localTestMaskCombo,
											localTrainMaskCombo;
	
	
	CDialog::OnInitDialog ();
	
			// Initialize dialog variables.
			
	StatisticsDialogInitialize (this,
											m_totalNumberChannels,
											&m_localStatCode,
											&m_keepClassStatsFlag,
											&m_useCommonCovarianceInLOOCFlag,
											&m_variance,
											&m_minLogDeterminant,
											&localChannelSelection,
											&m_channelListType,
											m_localFeaturesPtr,
											&m_localNumberFeatures,
											&localOutlineFieldType,
											&localLabelFieldCode,
											&localOutlineColorSelection,
											&localTrainMaskCombo,
											&localTestMaskCombo,
											&m_maxNumberTrainLayers,
											&m_maxNumberTestLayers);
	
			// Set feature parameters 
			
	m_localActiveNumberFeatures = m_localNumberFeatures; 
	m_localActiveFeaturesPtr = m_localFeaturesPtr;
	m_channelSelection = localChannelSelection;
	
			// Outline training areas.															
	
	m_showTrainingFields = (localOutlineFieldType & 0x0001);
	
			// Outline test areas.																
			
	m_showTestFields = (localOutlineFieldType & 0x0002);
	
			// Put labels with the area outlines.              
	
	m_showClassNames = (localLabelFieldCode & 0x0001);

	m_showFieldNames = FALSE;
	if (localLabelFieldCode & 0x0002)
		m_showFieldNames = TRUE;

	m_showTrainTestText = FALSE;
	if (localLabelFieldCode & 0x0004)
		m_showTrainTestText = TRUE;
	
	m_outlineColorSelection = localOutlineColorSelection;
	
			// Make the train and test mask combo variables to be 0 based.
	
	m_trainMaskCombo = localTrainMaskCombo - 1;	
	m_testMaskCombo = localTestMaskCombo - 1; 
	            
			// Make the outline color combo variable to be 0 based.
			
	m_outlineColorSelection--;
		
			// Update the dialog box parameters and then center the dialog.
	                 
	if (UpdateData (FALSE))
		PositionDialogWindow (); 
	
	SetOutlineAreaOptions ();

	if (localTrainMaskCombo == 3)
		SelectDialogItemText (this, IDC_TrainMaskLayer, 0, SInt16_MAX);
	else if (localTestMaskCombo == 3)
		SelectDialogItemText (this, IDC_TestMaskLayer, 0, SInt16_MAX);
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"



void CMStatisticsDialog::OnOK ()

{
	SInt16								itemHit = 1;

	UInt16								testLayerNumber,
											trainLayerNumber;
	
	
	if (UpdateData (TRUE))
		{
				// User selected OK for information.

				// Check channels used for train and test mask
			
		trainLayerNumber = 1;
		if (itemHit == 1 && m_trainMaskCombo == 2 && m_maxNumberTrainLayers > 1)
			{
			itemHit = CheckMaxValue (this,
												IDC_TrainMaskLayer,
												1,
												m_maxNumberTrainLayers,
												kDisplayRangeAlert);

			trainLayerNumber = (UInt16)GetDItemValue (this, IDC_TrainMaskLayer);

			}	// end "if (itemHit == 1 && gMaskTrainImageSelection == 3 && ..."


		testLayerNumber = 1;
		if (itemHit == 1 && m_testMaskCombo == 2 && m_maxNumberTestLayers > 1)
			{
			itemHit = CheckMaxValue (this,
												IDC_TestMaskLayer,
												1,
												m_maxNumberTestLayers,
												kDisplayRangeAlert);

			testLayerNumber = (UInt16)GetDItemValue (this, IDC_TestMaskLayer);

			}	// end "if (itemHit == 1 && gMaskTestImageSelection == 3 && ..."
		
				// Load the mask data into memory if a mask file was selected.
		
		if (itemHit == 1)
			itemHit = StatisticsDialogMaskCheck (m_trainMaskFileInfoHandle,
																m_testMaskFileInfoHandle,
																m_trainMaskCombo + 1,
																m_testMaskCombo + 1,
																trainLayerNumber,
																testLayerNumber);
		
		if (itemHit == 1)
			CMDialog::OnOK ();
		
		}	// end "if (UpdateData (TRUE))"
	
}	// end "OnOK"



void CMStatisticsDialog::OnSelendokChannels ()

{                                                                
	HandleChannelsMenu (IDC_Channels, 
								kNoTransformation,
								m_totalNumberChannels,
								kImageChannelType,
								TRUE);
	
}	// end "OnSelendokChannels"



void CMStatisticsDialog::OnSelendokProjectChanges ()

{                                                           
	DDX_CBIndex (m_dialogFromPtr, IDC_ProjectChanges, m_projectCommands);
	if (m_projectCommands == 1)
		{
		SInt16 returnCode = m_channelListType;
		if (ProjectMenuClearStatistics ())
			returnCode = 1; 
							
				// If project stats were cleared, then reload the local	
				// feature vector to include all channels.					
							
		if (m_channelListType == kSelectedItemsListOnly && returnCode == 1)
			{
			SInt16		index;
			
			m_channelListType = 1;
			m_channelSelection = kAllMenuItem;
			for (index=0;
					index<gProjectInfoPtr->numberStatisticsChannels;
					index++)
				m_localFeaturesPtr[index] = index;
			m_localActiveNumberFeatures = 
									gProjectInfoPtr->numberStatisticsChannels; 
			
			UpdateAllSubsetList (IDC_Channels);
				
			DDX_CBIndex (m_dialogToPtr, IDC_Channels, m_channelSelection);
						
			}	// end "if (m_channelListType == ..."
		
		m_projectCommands = 0;	
		DDX_CBIndex (m_dialogToPtr, IDC_ProjectChanges, m_projectCommands);
		
		}	// end "if (m_projectCommands == 1)"
	
}	// end "OnSelendokProjectChanges"



void CMStatisticsDialog::OnSelendokTestMaskCOMBO ()

{
	SInt16								itemHit,
											savedTestMaskCombo;
	
	
	savedTestMaskCombo = m_testMaskCombo;
	
	DDX_CBIndex (m_dialogFromPtr,
						IDC_TestMaskPopUp,
						m_testMaskCombo);
	
	if (m_testMaskCombo >= 0)
		{
		itemHit = StatisticsDialogSelectMaskItem (&m_testMaskFileInfoHandle,
																this,
																NULL,
																m_testMaskCombo+1,
																savedTestMaskCombo+1,
																IDS_FileIO96,
																IDC_TestMaskPopUp,
																IDC_TestMaskLayer,
																&m_maxNumberTestLayers);
			
		m_testMaskCombo = itemHit - 1;
		
		DDX_CBIndex (m_dialogToPtr,
						IDC_TestMaskPopUp,
						m_testMaskCombo);
		
		}	// end "if (m_testMaskCombo >= 0)"
	
}	// end "OnSelendokTestMaskCOMBO"



void CMStatisticsDialog::OnSelendokTrainMaskCOMBO ()

{
	SInt16								itemHit,
											savedTrainMaskCombo;
	
	
	savedTrainMaskCombo = m_trainMaskCombo;
	
	DDX_CBIndex (m_dialogFromPtr, IDC_TrainMaskPopUp, m_trainMaskCombo);
	
	if (m_trainMaskCombo >= 0)
		{
		itemHit = StatisticsDialogSelectMaskItem (&m_trainMaskFileInfoHandle,
																this,
																NULL,
																m_trainMaskCombo+1,
																savedTrainMaskCombo+1,
																IDS_FileIO95,
																IDC_TrainMaskPopUp,
																IDC_TrainMaskLayer,
																&m_maxNumberTrainLayers);
			
		m_trainMaskCombo = itemHit - 1;
			
		DDX_CBIndex (m_dialogToPtr, IDC_TrainMaskPopUp, m_trainMaskCombo);
		
		}	// end "if (m_trainMaskCombo >= 0)"
	
}	// end "OnSelendokTrainMaskCOMBO"



void CMStatisticsDialog::OnShowClassNames ()

{
	DDX_Check (m_dialogFromPtr, IDC_ShowClassNames, m_showClassNames);
	
}	// end "OnShowClassNames"



void CMStatisticsDialog::OnShowFieldNames ()

{
	DDX_Check (m_dialogFromPtr, IDC_ShowFieldNames, m_showFieldNames);
	
}	// end "OnShowFieldNames"



void CMStatisticsDialog::OnShowTrainTestLabel ()

{
	DDX_Check (m_dialogFromPtr, IDC_ShowTrainTestLabel, m_showTrainTestText);
	
}	// end "OnShowTrainTestLabel"

 

void CMStatisticsDialog::OnStatisticsOptions ()

{                          
	SetDLogControlHilite (NULL, IDOK, 255); 
	
	StatisticsOptionsDialog (&m_localStatCode,
										&m_keepClassStatsFlag,
										&m_variance,
										&m_minLogDeterminant,
										&m_useCommonCovarianceInLOOCFlag);
									
	SetDLogControlHilite (NULL, IDOK, 0); 
	
}	// end "OnStatisticsOptions"



void CMStatisticsDialog::OnTestFields ()

{
	DDX_Check (m_dialogFromPtr, IDC_TestFields, m_showTestFields);
	
	SetOutlineAreaOptions ();
	
}	// end "OnTestFields"



void CMStatisticsDialog::OnTrainingFields ()

{                                                                            
	DDX_Check (m_dialogFromPtr, IDC_TrainingFields, m_showTrainingFields);
	
	SetOutlineAreaOptions ();
	
}	// end "OnTrainingFields"



void CMStatisticsDialog::SetOutlineAreaOptions (void)

{
	if (m_showTrainingFields || m_showTestFields)
		{
		DDX_Check (m_dialogToPtr, IDC_ShowClassNames, m_showClassNames);
		DDX_Check (m_dialogToPtr, IDC_ShowFieldNames, m_showFieldNames);
		DDX_Check (m_dialogToPtr, IDC_ShowTrainTestLabel, m_showTrainTestText);
		
		SetDLogControlHilite (this, IDC_ShowClassNames, 0);
		SetDLogControlHilite (this, IDC_ShowFieldNames, 0);
		SetDLogControlHilite (this, IDC_ShowTrainTestLabel, 0);
		
		ShowDialogItem (this, IDC_ColorPrompt);
		ShowDialogItem (this, IDC_ColorCombo);
		
		}	// end "if (m_showTrainingFields || m_showTestFields)"
	
	else    // !m_showTrainingFields && !m_showTestFields
		{
		BOOL	falseSetting = FALSE;
		
		DDX_Check (m_dialogToPtr, IDC_ShowClassNames, falseSetting);
		DDX_Check (m_dialogToPtr, IDC_ShowFieldNames, falseSetting);
		DDX_Check (m_dialogToPtr, IDC_ShowTrainTestLabel, falseSetting);
			
		SetDLogControlHilite (this, IDC_ShowClassNames, 255);
		SetDLogControlHilite (this, IDC_ShowFieldNames, 255);
		SetDLogControlHilite (this, IDC_ShowTrainTestLabel, 255);
			
		HideDialogItem (this, IDC_ColorPrompt);
		HideDialogItem (this, IDC_ColorCombo);
		
		}	// end "else !m_showTrainingFields && !m_showTestFields"
	
}	// end "SetOutlineAreaOptions"
