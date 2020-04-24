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
//	File:						WStatisticsImageDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			04/17/2020
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMStatImageDialog class.
//
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h"

#include "WMultiSpec.h"
#include	"WStatisticsImageDialog.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMStatImageDialog, CMDialog)
	ON_BN_CLICKED (IDC_ClassesRadio, OnClickClassesRadio)
	ON_BN_CLICKED (IDC_SelectedAreaRadio, OnClickAreaRadio)
	ON_BN_CLICKED (IDC_FeatureTransformation, OnFeatureTransformation)
	ON_BN_CLICKED (IDC_IndividualRadio, OnClickIndividualRadio)
	ON_BN_CLICKED (IDC_OverallRadio, OnClickOverallRadio)
	ON_BN_CLICKED (IDC_UserSettingRadio, OnClickUserSettingRadio)
	ON_BN_CLICKED (IDEntireImage, ToEntireImage)
	ON_BN_CLICKED (IDSelectedImage, ToSelectedImage)

	ON_CBN_SELENDOK (IDC_ChannelCombo, OnSelendokChannelCombo)
	ON_CBN_SELENDOK (IDC_ClassCombo, OnSelendokClassCombo)

	ON_EN_CHANGE(IDC_ColumnEnd, CheckColumnEnd)
	ON_EN_CHANGE(IDC_ColumnStart, CheckColumnStart)
	ON_EN_CHANGE(IDC_LineEnd, CheckLineEnd)
	ON_EN_CHANGE(IDC_LineStart, CheckLineStart)
END_MESSAGE_MAP ()



CMStatImageDialog::CMStatImageDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMStatImageDialog::IDD, pParent)

{
	UInt16**								classPtrPtr;
	

	m_classPtr = NULL;
	m_featurePtr = NULL;
	m_channelsPtr = NULL;

	m_channelSelection = 0;
	m_featureTransformationFlag = FALSE;

	m_classSelection = 0;

	m_perClassFieldCode = 0;

	m_classCode = 1;
	m_areaCode = 0;

	m_overallMinMaxCode = 0;
	m_individualMinMaxCode = 0;
	m_userMinMaxCode = 0;

	m_userMinimum = 0.0;
	m_userMaximum = 0.0;

	m_initializedFlag = CMDialog::m_initializedFlag;

	m_channelsAllAvailableFlag = FALSE;

	if (gStatisticsImageSpecsPtr->areaCode == kTrainingType)
		classPtrPtr = &m_classListPtr;
	else
		classPtrPtr = NULL;

	m_initializedFlag = GetDialogLocalVectors (&m_localFeaturesPtr,
																&m_localTransformFeaturesPtr,
																classPtrPtr,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL);

}	// end "CMStatImageDialog"



CMStatImageDialog::~CMStatImageDialog () 

{
	if (m_classListPtr != NULL)
		{
		ReleaseDialogLocalVectors (m_localFeaturesPtr,
			m_localTransformFeaturesPtr,
			m_classListPtr,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL);
		
		}	// end "if (m_classListPtr != NULL)"
	
}	// end "~CMStatImageDialog"



void CMStatImageDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);

	DDX_Text (pDX, IDC_LineStart, m_LineStart);
	DDX_Text (pDX, IDC_LineEnd, m_LineEnd);
	DDX_Text (pDX, IDC_LineInterval, m_LineInterval);
	DDX_Text (pDX, IDC_ColumnStart, m_ColumnStart);
	DDX_Text (pDX, IDC_ColumnEnd, m_ColumnEnd);
	DDX_Text (pDX, IDC_ColumnInterval, m_ColumnInterval);

	DDX_CBIndex (pDX, IDC_ChannelCombo, m_channelSelection);
	DDX_Check (pDX, IDC_FeatureTransformation, m_featureTransformationFlag);

	DDX_Radio (pDX, IDC_ClassesRadio, m_classCode); 
	//DDX_Radio (pDX, IDC_SelectedAreaRadio, m_areaCode);
	
	int	perClassFieldCode = 0;
	DDX_Radio (pDX, IDC_SelectedClassRadio, perClassFieldCode);

	int overallMinMaxCode = 0;
	DDX_Radio (pDX, IDC_OverallRadio, overallMinMaxCode);
		
	DDX_CBIndex (pDX, IDC_ClassCombo, m_classSelection);

	DDX_Text2 (pDX, IDC_StatisticMin, m_userMinimum);
	DDX_Text2 (pDX, IDC_StatisticMax, m_userMaximum);

	if (m_classCode == 0)
		m_areaCode = kTrainingType;

	else  // m_classCode != 0
		m_areaCode = kAreaType;

	if (perClassFieldCode == 0) 
		m_perClassFieldCode = 1;

	else 
		m_perClassFieldCode = 2;

	if (overallMinMaxCode == 0) 
		{
		m_overallMinMaxCode = 1;
		m_individualMinMaxCode = 0;
		m_userMinMaxCode = 0;
		
		}	// end "if (overallMinMaxCode == 0)"
	
	else if (overallMinMaxCode == 1)
		{
		m_overallMinMaxCode = 0;
		m_individualMinMaxCode = 1;
		m_userMinMaxCode = 0;
		
		}	// end "else if (overallMinMaxCode == 1)"
	
	else	// overallMinMaxCode != 0 && overallMinMaxCode != 1
		{
		m_overallMinMaxCode = 0;
		m_individualMinMaxCode = 0;
		m_userMinMaxCode = 1;
		
		}	// end "else overallMinMaxCode != 0 && overallMinMaxCode != 1";

			// Verify that the line and column values make sense
		
	VerifyLineColumnStartEndValues (pDX);

}	// end "DoDataExchange"



BOOL CMStatImageDialog::DoDialog ()

{
	INT_PTR								returnCode;

	BOOL									continueFlag = FALSE;
	

			// Make sure intialization has been completed.
	
	if (!m_initializedFlag)
																			return (continueFlag);

	returnCode = DoModal ();

	if (returnCode == IDOK)
		{
		DialogSelectArea		dialogSelectArea;


		continueFlag = TRUE;

		dialogSelectArea.lineStart = m_LineStart;
		dialogSelectArea.lineEnd = m_LineEnd;
		dialogSelectArea.lineInterval = m_LineInterval;
		dialogSelectArea.columnStart = m_ColumnStart;
		dialogSelectArea.columnEnd = m_ColumnEnd;
		dialogSelectArea.columnInterval = m_ColumnInterval;
		
 		StatisticsImageDialogOK (this,
											gStatisticsImageSpecsPtr,
											&dialogSelectArea,
											m_channelSelection,
											m_featureTransformationFlag,
											(SInt16*)m_localActiveFeaturesPtr,
											m_localActiveNumberFeatures,
											m_maximumNumberChannels,
											m_classSelection,
											m_localNumberClasses,
											(SInt16*)m_classListPtr,
											m_areaCode,
											m_perClassFieldCode,
											m_overallMinMaxCode,
											m_individualMinMaxCode,
											m_userMinMaxCode,
											m_userMinimum,
											m_userMaximum);

		}	// end "if (returnCode == IDOK)"

	return (continueFlag);
	
}	// end "DoDialog"



void CMStatImageDialog::OnClickAreaRadio ()

{
	m_classCode = 0;
	m_areaCode = kAreaType;

	HideDialogItem (this, IDC_ClassCombo);
	HideDialogItem (this, IDC_StatPrompt);
	HideDialogItem (this, IDC_SelectedClassRadio);
	HideDialogItem (this, IDC_SelectedFieldRadio);

	HideShowAreaItems (TRUE);

			// Determine if this is the entire area and set the to entire image icon.

	//m_dialogSelectArea.imageWindowInfoPtr =
	//	(WindowInfoPtr)GetHandleStatusAndPointer (m_targetWindowInfoHandle,
	//		&handleStatus);

	SetEntireImageButtons (NULL,
									m_LineStart,
									m_LineEnd,
									m_ColumnStart,
									m_ColumnEnd);

	//m_dialogSelectArea.imageWindowInfoPtr = NULL;

	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);

}	// end "OnClickAreaRadio"



void CMStatImageDialog::OnClickClassesRadio ()

{
	m_classCode = 1;
	m_areaCode = kTrainingType;

	ShowDialogItem (this, IDC_ClassCombo);
	ShowDialogItem (this, IDC_StatPrompt);
	ShowDialogItem (this, IDC_SelectedClassRadio);
	ShowDialogItem (this, IDC_SelectedFieldRadio);

	HideShowAreaItems (FALSE);

	SelectDialogItemText (this, IDC_StatisticMin, 0, SInt16_MAX);

}	// end "OnClickClassesRadio"



void CMStatImageDialog::OnClickIndividualRadio ()

{
	HideDialogItem (this, IDC_MinPrompt);
	HideDialogItem (this, IDC_StatisticMin);
	HideDialogItem (this, IDC_MaxPrompt);
	HideDialogItem (this, IDC_StatisticMax);
	
}	// end "OnClickIndividualRadio"



void CMStatImageDialog::OnClickOverallRadio ()

{
	HideDialogItem (this, IDC_MinPrompt);
	HideDialogItem (this, IDC_StatisticMin);
	HideDialogItem (this, IDC_MaxPrompt);
	HideDialogItem (this, IDC_StatisticMax);
	
}	// end "OnClickOverallRadio"



void CMStatImageDialog::OnClickUserSettingRadio ()

{
	ShowDialogItem (this, IDC_MinPrompt);
	ShowDialogItem (this, IDC_StatisticMin);
	ShowDialogItem (this, IDC_MaxPrompt);
	ShowDialogItem (this, IDC_StatisticMax);

}	// end "OnClickUserSettingRadio"



void CMStatImageDialog::OnFeatureTransformation ()

{
	DDX_Check (m_dialogFromPtr,
					IDC_FeatureTransformation,
					m_featureTransformationFlag);

	CheckFeatureTransformationDialog (this,
													m_featureTransformAllowedFlag,
													IDC_FeatureTransformation,
													IDC_ChannelPrompt,
													(SInt16*)&m_featureTransformationFlag);

	m_channelSelection = UpdateDialogFeatureParameters (
													m_featureTransformationFlag,
													&m_localActiveNumberFeatures,
													&m_localActiveFeaturesPtr,
													m_localNumberFeatures,
													m_localFeaturesPtr,
													gProjectInfoPtr->numberStatisticsChannels,
													m_localNumberTransformFeatures,
													m_localTransformFeaturesPtr,
													gTransformationMatrix.numberFeatures);

}	// end "OnFeatureTransformation" 



BOOL CMStatImageDialog::OnInitDialog ()

{
	SInt16								areaCode,
											channelSelection,
											selectItem;

	Boolean								featureTransformationFlag;
	

	CMDialog::OnInitDialog ();

			// Make sure that we have the bitmaps for the entire image buttons.

	VERIFY (toEntireButton.AutoLoad (IDEntireImage, this));
	VERIFY (toSelectedButton.AutoLoad (IDSelectedImage, this));

	StatisticsImageDialogInitialize (this,
												gStatisticsImageSpecsPtr,
												&m_dialogSelectArea,
												gImageWindowInfoPtr,
												gActiveImageWindow,
												&channelSelection,
												&m_localNumberFeatures,
												m_localFeaturesPtr,
												m_localTransformFeaturesPtr,
												&m_channelsAllAvailableFlag,
												(UInt16**)&m_channelsPtr,
												&m_maximumNumberChannels,
												(Boolean*)&m_classSelection,
												&m_localNumberClasses,
												(UInt16*)m_classListPtr,
												&m_MinMaxCode,
												&m_userMinimum,
												&m_userMaximum,
												&areaCode,
												&m_perClassFieldCode,
												&selectItem,
												&featureTransformationFlag,
												&m_featureTransformAllowedFlag);

	m_LineStart = m_dialogSelectArea.lineStart;
	m_LineEnd = m_dialogSelectArea.lineEnd;
	m_LineInterval = m_dialogSelectArea.lineInterval;
	m_ColumnStart = m_dialogSelectArea.columnStart;
	m_ColumnEnd = m_dialogSelectArea.columnEnd;
	m_ColumnInterval = m_dialogSelectArea.columnInterval;

			// Set feature parameters

	m_localActiveNumberFeatures = m_localNumberFeatures;
	m_localActiveFeaturesPtr = m_localFeaturesPtr;
	m_channelSelection = channelSelection;

	m_featureTransformationFlag = featureTransformationFlag;

	m_areaCode = areaCode;
	if (areaCode == kTrainingType)
		HideShowAreaItems (FALSE);

	return TRUE;
	
}	// end "OnInitDialog"



void CMStatImageDialog::OnSelendokChannelCombo ()

{
	HandleChannelsMenu (IDC_ChannelCombo,
								kNoTransformation,
								(SInt16)gImageWindowInfoPtr->totalNumberChannels,
								2,
								TRUE);


}	// end "OnSelendokChannelCombo"



void CMStatImageDialog::OnSelendokClassCombo ()

{
	HandleClassesMenu (&m_localNumberClasses,
								(SInt16*)m_classListPtr,
								(SInt16)1,
								(SInt16)gProjectInfoPtr->numberStatisticsClasses,
								IDC_ClassCombo,
								(int*)&m_classSelection);
	
}	// end "OnSelendokClassCombo"
