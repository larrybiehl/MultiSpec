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
//	File:						WClusterDialog.cpp : implementation file
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
//								CMClusterDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WClusterDialog.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMClusterDialog, CMDialog)
	//{{AFX_MSG_MAP (CMClusterDialog)
	ON_BN_CLICKED (IDC_ClassifySelectedArea, OnClassifySelectedArea)
	ON_BN_CLICKED (IDC_ClassifyTrainingAreas, OnClassifySelectedArea)
	ON_BN_CLICKED (IDC_CreateClusterMaskCheckBox, OnCreateMaskFile)
	ON_BN_CLICKED (IDC_ImageOverlay, OnImageOverlay)
	ON_BN_CLICKED (IDC_ISODATAAlgorithm, OnISODATAAlgorithm)
	ON_BN_CLICKED (IDC_NoClassificationMap, OnClassifySelectedArea)
	ON_BN_CLICKED (IDC_SinglePassAlgorithm, OnSinglePassAlgorithm)
	ON_BN_CLICKED (IDEntireImage, ToEntireImage)
	ON_BN_CLICKED (IDSelectedImage, ToSelectedImage)

	ON_EN_CHANGE (IDC_ColumnEnd, CheckColumnEnd)
	ON_EN_CHANGE (IDC_ColumnStart, CheckColumnStart)
	ON_EN_CHANGE (IDC_LineEnd, CheckLineEnd)
	ON_EN_CHANGE (IDC_LineStart, CheckLineStart)

	ON_CBN_SELENDOK (IDC_ChannelCombo, OnSelendokChannelCombo)
	ON_CBN_SELENDOK (IDC_DiskCombo, OnSelendokMaskFileDiskCombo)
	ON_CBN_SELENDOK (IDC_ImageOverlayCombo, OnSelendokImageOverlayCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMClusterDialog::CMClusterDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMClusterDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT(CMClusterDialog)
	m_thresholdFlag = FALSE;
	m_textWindowFlag = FALSE;
	m_symbolSelection = -1;
	m_clusterProcedure = -1;
	m_probabilityThreshold = 0;
	m_classifyThreshold = 1;
	m_diskFileFlag = FALSE;
	m_classificationArea = -1;
	m_saveStatisticsSelection = -1;
	m_createClusterMaskFlag = FALSE;
	m_maskFileFormatCode = 0;
	m_createImageOverlayFlag = FALSE;
	m_selectImageOverlaySelection = 1;
	//}}AFX_DATA_INIT
	
	mMaxClassifyThreshold = gImageFileInfoPtr->maxUsableDataValue;
	
	m_clusterProcedureSetFlag = FALSE;

	m_characterLimitDialogDisplayedFlag = FALSE;

	m_transparencyValue = 0;
	
	m_initializedFlag = CMDialog::m_initializedFlag; 
	
	if (m_initializedFlag)
		{                          
				// Get pointer to memory for temporary storage of channel list. 
			                                                                       
		m_localFeaturesPtr = (UInt16*)MNewPointer (
					(UInt32)gImageWindowInfoPtr->totalNumberChannels * sizeof (UInt16));
	                                                       
		m_initializedFlag = (m_localFeaturesPtr != NULL);
		
		}	// end "if (m_initializedFlag)"
		
}	// end "CMClusterDialog"



CMClusterDialog::~CMClusterDialog (void)

{                                                              
	m_localFeaturesPtr = CheckAndDisposePtr (m_localFeaturesPtr); 
	
}	// end "~CMClusterDialog"



void CMClusterDialog::ClassifyAreaSetting (
				CDataExchange* 					pDX,
				int 									nIDC,
				int& 									value)

{
	BOOL									settingFlag;
	
	
	if (pDX->m_bSaveAndValidate)
		{
				// Reading from radio group. Get index ignoring the disabled
				// items.
		
		value = -1;
		
		DDX_Check (m_dialogFromPtr, IDC_NoClassificationMap, settingFlag);
		if (settingFlag)
			value = 0;
		
		DDX_Check (m_dialogFromPtr, IDC_ClassifyTrainingAreas, settingFlag);
		if (settingFlag)
			value = 1;
		
		DDX_Check (m_dialogFromPtr, IDC_ClassifySelectedArea, settingFlag);
		if (settingFlag)
			value = 2;
		
		}	// end "if (pDX->m_bSaveAndValidate)"
	
	else	// !pDX->m_bSaveAndValidate
		{
		int			localValue = value;
		
		DDX_Radio (pDX,
						IDC_NoClassificationMap,
						localValue);
		
		}	// end "else !pDX->m_bSaveAndValidate"
	
}	// end "ClassifyAreaSetting"



void CMClusterDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMClusterDialog)
	DDX_Check (pDX, IDC_UsePixelsThresholdFlag, m_thresholdFlag);
	DDX_Check (pDX, IDC_TextWindow, m_textWindowFlag);
	DDX_CBIndex (pDX, IDC_SymbolCombo, m_symbolSelection);
	DDX_Radio (pDX, IDC_SinglePassAlgorithm, m_clusterProcedure);
	DDX_Text2 (pDX, IDC_UsePixelsThresholdValue, m_probabilityThreshold);
	DDV_MinMaxDouble (pDX, m_probabilityThreshold, 0., 100.);
	DDX_Text2 (pDX, IDC_ClassifyThreshold, m_classifyThreshold);         
	DDX_Check (pDX, IDC_DiskFile, m_diskFileFlag);
	DDX_CBIndex (pDX, IDC_ClusterStatsCombo, m_saveStatisticsSelection);
	DDX_Text (pDX, IDC_LineEnd, m_LineEnd);             
	DDX_Text (pDX, IDC_LineInterval, m_LineInterval);         
	DDX_Text (pDX, IDC_LineStart, m_LineStart);            
	DDX_Text (pDX, IDC_ColumnEnd, m_ColumnEnd);             
	DDX_Text (pDX, IDC_ColumnInterval, m_ColumnInterval);        
	DDX_Text (pDX, IDC_ColumnStart, m_ColumnStart);            
	DDX_CBIndex (pDX, IDC_ChannelCombo, m_channelSelection);
	DDX_Check (pDX, IDC_CreateClusterMaskCheckBox, m_createClusterMaskFlag);
	DDX_CBIndex (pDX, IDC_DiskCombo, m_maskFileFormatCode);
	DDX_Check (pDX, IDC_ImageOverlay, m_createImageOverlayFlag);
	DDX_CBIndex (pDX, IDC_ImageOverlayCombo, m_selectImageOverlaySelection);
	//}}AFX_DATA_MAP
	
	ClassifyAreaSetting (pDX, IDC_NoClassificationMap, m_classificationArea);
	
	if (m_classificationArea != 0 && pDX->m_bSaveAndValidate)
		{                         
				// Verify that the line and column values make sense
		
		VerifyLineColumnStartEndValues (pDX);
		
		DDV_MinMaxLong (pDX, m_LineStart, 1, m_maxNumberLines);                  
		DDV_MinMaxLong (pDX, m_LineEnd, 1, m_maxNumberLines);
		DDV_MinMaxLong (pDX, m_LineInterval, 1, m_maxNumberLines); 
		
		DDV_MinMaxLong (pDX, m_ColumnStart, 1, m_maxNumberColumns);
		DDV_MinMaxLong (pDX, m_ColumnEnd, 1, m_maxNumberColumns); 
		DDV_MinMaxLong (pDX, m_ColumnInterval, 1, m_maxNumberColumns);
	
		DDV_MinMaxDouble (pDX, m_classifyThreshold, 0, mMaxClassifyThreshold);
		
		}	// end "if (m_classificationArea != 0 && ...)" 
	
	if (pDX->m_bSaveAndValidate && !m_characterLimitDialogDisplayedFlag)
		{
		m_characterLimitDialogDisplayedFlag = TRUE;

				// Check if listing the stats in the output text window is more than
				// the define limit. If so present message to the user and allow them
				// a chance to save to a disk file.
				
		if (CheckIfTextForTextWindowIsWithinLimits (
													NULL,
													m_localActiveNumberFeatures,
													gImageFileInfoPtr->maxDataValue,
													gImageFileInfoPtr->minDataValue,
													m_diskFileFlag) == 0)
			{
			m_characterLimitDialogDisplayedFlag = FALSE;
			pDX->Fail ();

			}	// end "if (CheckIfTextForTextWindowIsWithinLimits (..."

		}	// end "if (pDX->m_bSaveAndValidate && ..."
		
}	// end "DoDataExchange"


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the cluster
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
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 08/02/1996
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

SInt16 CMClusterDialog::DoDialog (
				Boolean								newProjectFlag)

{  
	INT_PTR						returnCode;
	Boolean						continueFlag = FALSE;
	
	
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
																			
	m_newProjectFlag = newProjectFlag;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE; 	
				
				// Cluster classification area													
				
		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;
																
		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;

		ClusterDialogOK (this,
								(SInt16)(m_clusterProcedure+1),
								(Boolean)(m_classificationArea==1),
								(Boolean)(m_classificationArea==2),
								&m_dialogSelectArea,
								(double)m_classifyThreshold,
								(SInt16)m_channelSelection,
								(SInt16)m_localActiveNumberFeatures,
								(SInt16*)m_localActiveFeaturesPtr,
								(SInt16)m_symbolSelection,
								(unsigned char*)m_localSymbolsPtr,
								(SInt16)m_saveStatisticsSelection+1,
								(Boolean)m_textWindowFlag,
								(Boolean)m_diskFileFlag,
								(Boolean)m_createClusterMaskFlag,
								(SInt16)m_maskFileFormatCode+1,
								(Boolean)m_createImageOverlayFlag,
								(SInt16)m_selectImageOverlaySelection+1,
								(Boolean)m_thresholdFlag,
								(double)m_probabilityThreshold,
								(SInt32)m_transparencyValue);

		}	// end "if (returnCode == IDOK)"
	
			// Set the variable containing the pointer to the overlay combo box
			// to NULL.
		
	gPopUpImageOverlayMenu = NULL;
		
	return (continueFlag);
		
}	// end "DoDialog"



void CMClusterDialog::OnAlgorithm (
				Boolean								returnFlag)

{
	if (returnFlag)
		{
		if (!m_clusterProcedureSetFlag)
			GetDlgItem (IDOK)->EnableWindow (TRUE);
		
		m_clusterProcedureSetFlag = TRUE;

		DDX_Radio (m_dialogFromPtr, IDC_SinglePassAlgorithm, m_clusterProcedure);
		
		}	// end "if (returnFlag)"

	else	// !returnFlag
		{
		if (!m_clusterProcedureSetFlag)
			m_clusterProcedure = -1;

		DDX_Radio (m_dialogToPtr, IDC_SinglePassAlgorithm, m_clusterProcedure);

		}	// end "else !returnFlag"
	
}	// end "OnAlgorithm"



void CMClusterDialog::OnClassifySelectedArea (void)

{
	ClassifyAreaSetting (m_dialogFromPtr,
								IDC_NoClassificationMap,
								m_classificationArea);
	
	UpdateClassificationItems (m_classificationArea);
	
}	// end "OnClassifySelectedArea"



void CMClusterDialog::OnCreateMaskFile (void)

{
	DDX_Check (m_dialogFromPtr, IDC_CreateClusterMaskCheckBox, m_createClusterMaskFlag);
	
	if (!m_createClusterMaskFlag)
		HideDialogItem (this, IDC_DiskCombo);
	
	else	// !m_createClusterMaskFlag)
		ShowDialogItem (this, IDC_DiskCombo);
	
}	// end "OnCreateMaskFile"



void CMClusterDialog::OnImageOverlay ()

{
	DDX_Check (m_dialogFromPtr, IDC_ImageOverlay, m_createImageOverlayFlag);

	if (m_createImageOverlayFlag)
		ShowDialogItem (this, IDC_ImageOverlayCombo);
	
	else	// m_createImageOverlayFlag
		HideDialogItem (this, IDC_ImageOverlayCombo);
	
}	// end "OnImageOverlay"



BOOL CMClusterDialog::OnInitDialog (void)

{  
	SInt16								channelSelection,
											classificationArea,
											clusterProcedure,
											maskFileFormatCode,
											saveStatisticsSelection,
											selectImageOverlaySelection,
											symbolSelection;
	
	Boolean								modeSetFlag;
	

	CMDialog::OnInitDialog ();
	
			// Make sure that we have the bitmaps for the entire image buttons.
		
	VERIFY (toEntireButton.AutoLoad (IDEntireImage, this));
	VERIFY (toSelectedButton.AutoLoad (IDSelectedImage, this));

	ClusterDialogInitialize (this,
										m_localFeaturesPtr,
										m_localSymbolsPtr,
										m_newProjectFlag,
										&clusterProcedure,
										&modeSetFlag,
										&classificationArea,
										&m_dialogSelectArea,
										&m_classifyThreshold,
										&channelSelection,
										&m_localActiveNumberFeatures,
										&symbolSelection,
										&saveStatisticsSelection,
										(Boolean*)&m_textWindowFlag,
										(Boolean*)&m_diskFileFlag,
										(Boolean*)&m_createClusterMaskFlag,
										&maskFileFormatCode,
										(Boolean*)&m_createImageOverlayFlag,
										&selectImageOverlaySelection,
										(Boolean*)&m_thresholdFlag,
										&m_probabilityThreshold,
										(SInt32*)&m_transparencyValue); 
	
			// Set feature/transform feature parameters  

	InitializeDialogFeatureParameters (kNoTransformation,
													m_localActiveNumberFeatures, 
													gImageFileInfoPtr->numberChannels, 
													0,
													m_localFeaturesPtr,
													NULL,
													&m_localNumberFeatures,
													NULL,
													&m_localActiveFeaturesPtr);
	
	m_clusterProcedure = clusterProcedure - 1;
	m_classificationArea = classificationArea;
	m_channelSelection = channelSelection;	
	m_symbolSelection = symbolSelection;	
	m_saveStatisticsSelection = saveStatisticsSelection - 1;							
	m_selectImageOverlaySelection = selectImageOverlaySelection - 1;
	m_channelListType = 1;  
	
			//	Cluster Procedure
			// Make the "Single Pass" the default cluster algorithm.					
			// Single Pass Cluster.												
	
	m_clusterProcedureSetFlag = TRUE;
	if (m_clusterProcedure == -1)
		{                                            
		GetDlgItem (IDOK)->EnableWindow (FALSE);
		m_clusterProcedureSetFlag = FALSE;
		
		}	// end "if (m_clusterProcedure == -1)" 
	 
			// Cluster Mask File output to disk file.
			//// Adjust for 0 base index.
	
	m_maskFileFormatCode = abs (maskFileFormatCode);
	m_maskFileFormatCode -= 1;
		
	if (!m_createClusterMaskFlag)
		MHideDialogItem (this, IDC_DiskCombo);
	
	m_LineStart = m_dialogSelectArea.lineStart;
	m_LineEnd = m_dialogSelectArea.lineEnd;
	m_LineInterval = m_dialogSelectArea.lineInterval;
	m_ColumnStart = m_dialogSelectArea.columnStart;
	m_ColumnEnd = m_dialogSelectArea.columnEnd;
	m_ColumnInterval = m_dialogSelectArea.columnInterval; 
		
	UpdateClassificationItems (m_classificationArea);     
	if (UpdateData (FALSE))
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item	
		                                       
	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX); 
	
	return FALSE;  // return TRUE  unless you set the focus to a control 
	
}	// end "OnInitDialog"

  
  
void CMClusterDialog::OnISODATAAlgorithm (void)

{  
	Boolean returnFlag = ISODATAClusterDialog (gImageFileInfoPtr, this);
	
	OnAlgorithm (returnFlag); 
	
}	// end "OnISODATAAlgorithm"



void CMClusterDialog::OnOK ()

{
	Boolean								okFlag = TRUE;
	
	
	if (UpdateData (TRUE))
		{
					// Check if the save statistics option will
					// require the current project to be saved.  Set item hit to
					// 0 if the user cancels from the save.
		
		if (m_saveStatisticsSelection == 1)		// To new project
			{
			if (gProjectInfoPtr->changedFlag)
				if (SaveProjectFile (2) != 0)
					okFlag = FALSE;
			
			}	// end "if (m_saveStatisticsSelection == 1)"
		
		else if (m_saveStatisticsSelection == 2)
			{
					// Add to project
			
			if (gImageFileInfoPtr->numberChannels !=
											gClusterSpecsPtr->numberChannels ||
							gProjectInfoPtr->numberStatisticsChannels !=
													(SInt16)gClusterSpecsPtr->numberChannels)
				{
				SInt16			itemHit;
				
				itemHit = DisplayAlert (kOKCancelAlertID,
												2, kAlertStrID,
												IDS_Alert11,
												0,
												NULL);
				if (itemHit != 1)
					okFlag = FALSE;
				
				}	// end "if (fileInfoPtr->numberChannels != ..."
			
			}	// end "else if (m_saveStatisticsSelection == 2)"
	
		if (okFlag)
			CMDialog::OnOK ();
		
		}	// end "if (UpdateData (TRUE))"
	
}	// end "OnOK"



void CMClusterDialog::OnSelendokChannelCombo (void)

{
	HandleChannelsMenu (IDC_ChannelCombo,
								kNoTransformation,
								(SInt16)gImageWindowInfoPtr->totalNumberChannels,
								2,
								m_clusterProcedureSetFlag);
	
}	// end "OnSelendokChannelCombo"

  

void CMClusterDialog::OnSelendokImageOverlayCombo ()

{                                                           
	DDX_CBIndex (m_dialogFromPtr, IDC_ImageOverlayCombo, m_selectImageOverlaySelection);
	
}	// end "OnSelendokImageOverlayCombo"

  

void CMClusterDialog::OnSelendokMaskFileDiskCombo ()

{                                
	DDX_CBIndex (m_dialogFromPtr, IDC_DiskCombo, m_maskFileFormatCode);
	m_maskFileFormatCode++; 
	
}	// end "OnSelendokImageOverlayCombo"



void CMClusterDialog::OnSinglePassAlgorithm (void)

{
	Boolean returnFlag = OnePassClusterDialog (gImageFileInfoPtr, this);
	
	OnAlgorithm (returnFlag);
	
}	// end "OnSinglePassAlgorithm"



void CMClusterDialog::UpdateClassificationItems (
				UInt16								classificationArea)

{
	if (classificationArea == 0)
		{
		HideShowAreaItems (kHide);
			
		MHideDialogItem (this, IDC_ClassifyThresholdPrompt);
		MHideDialogItem (this, IDC_ClassifyThreshold);
		
		}	// end "if (classificationArea == 0)"
	
	else if (classificationArea == 1)
		{
		HideShowAreaItems (kHide);
		
		MShowDialogItem (this, IDC_StartEndInterval);
		MShowDialogItem (this, IDC_LinePrompt);
		MShowDialogItem (this, IDC_ColumnPrompt);
		MShowDialogItem (this, IDC_LineInterval);
		MShowDialogItem (this, IDC_ColumnInterval);
		
		MShowDialogItem (this, IDC_ClassifyThresholdPrompt);
		MShowDialogItem (this, IDC_ClassifyThreshold);
		
		}	// end "else if (classificationArea == 1)"
	
	else	// (classificationArea == 2)
		{
		HideShowAreaItems (kShow);
	
		SetEntireImageButtons (NULL,
										m_LineStart,
										m_LineEnd,
										m_ColumnStart,
										m_ColumnEnd);
		
		MShowDialogItem (this, IDC_ClassifyThresholdPrompt);
		MShowDialogItem (this, IDC_ClassifyThreshold);
		
		}	// end "else if (classificationArea == 2)"
	
}	// end "UpdateClassificationItems"
