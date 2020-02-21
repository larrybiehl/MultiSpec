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
//	File:						WBiPlotDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			10/25/2019
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMBiPlotDialog class.
//
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h"
#include	"SExternalGlobals.h"

#include	"WBiPlotDialog.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMBiPlotDialog, CMDialog)
	ON_BN_CLICKED (IDC_Area, OnImageArea)
	ON_BN_CLICKED (IDC_CreateNewGraphWindow, OnCreateNewWindowFlag)
	ON_BN_CLICKED (IDC_DisplayPixels, OnDisplayPixelAsSymbol)
	ON_BN_CLICKED (IDC_FeatureTransformation, OnFeatureTranformation)
	ON_BN_CLICKED (IDC_OutlineClassAsEllipse, OnOutlineClassAsEllipse)
	ON_BN_CLICKED (IDC_TestAreas, OnTestArea)
	ON_BN_CLICKED (IDC_ThresholdPixelCheck, OnThresholdPixelFlag)
	ON_BN_CLICKED (IDC_Training, OnTrainArea)
	ON_BN_CLICKED (IDEntireImage, ToEntireImage)
	ON_BN_CLICKED (IDSelectedImage, ToSelectedImage)

	ON_CBN_SELENDOK (IDC_ClassCombo, OnSelendokClassCombo)
	ON_CBN_SELENDOK (IDC_ClassPairWeightsCombo, OnSelendokClassWeightsCombo)

	ON_EN_CHANGE (IDC_ColumnEnd, CheckColumnEnd)
	ON_EN_CHANGE (IDC_ColumnStart, CheckColumnStart)
	ON_EN_CHANGE (IDC_Horizontal_channel, OnChangeHorizontalAxis)
	ON_EN_CHANGE (IDC_LineEnd, CheckLineEnd)
	ON_EN_CHANGE (IDC_LineStart, CheckLineStart)
	ON_EN_CHANGE (IDC_Vertical_channel, OnChangeVerticalAxis)
END_MESSAGE_MAP ()



CMBiPlotDialog::CMBiPlotDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMBiPlotDialog::IDD, pParent)

{
	m_classWeightsPtr = NULL;
	m_checkChannelStatisticsFlag = FALSE;
	m_checkClassesPopUpFlag = FALSE;
	m_createNewGraphicsWindowFlag = FALSE;

	m_checkFeatureTransformFlag = FALSE;
	m_featureTransformAllowedFlag = FALSE;
	m_featureTransformationFlag = FALSE;

	m_thresholdPercent = 0;
	m_saveThresholdPercent = 0.;
	m_maxChannelFeatureNum = 1;

	m_displayPixelCode = 0;
	m_outlineClassCode = 0;
	m_plotDataCode = 0;
	m_newXAxisFeature = 1;
	m_newYAxisFeature = 1;
	m_initializedFlag = CMDialog::m_initializedFlag;

	m_trainingAreaFlag = FALSE;
	m_imageAreaFlag = FALSE;
	m_testFlag = FALSE;

	if (gBiPlotDataSpecsPtr->projectFlag)
		m_initializedFlag = GetDialogLocalVectors (NULL,
																	NULL,
																	&m_classListPtr,
																	NULL,
																	&m_classWeightsPtr,
																	NULL,
																	NULL,
																	NULL);
	
}	// end "CMBiPlotDialog"



CMBiPlotDialog::~CMBiPlotDialog ()

{
	ReleaseDialogLocalVectors (NULL,
										NULL,
										m_classListPtr,
										NULL,
										m_classWeightsPtr,
										NULL,
										NULL,
										NULL);
	
}	// end "~CMBiPlotDialog"



void CMBiPlotDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CMDialog::DoDataExchange (pDX);

	DDX_Text (pDX, IDC_LineStart, m_LineStart);
	DDX_Text (pDX, IDC_LineEnd, m_LineEnd);
	DDX_Text (pDX, IDC_LineInterval, m_LineInterval);
	DDX_Text (pDX, IDC_ColumnStart, m_ColumnStart);
	DDX_Text (pDX, IDC_ColumnEnd, m_ColumnEnd);
	DDX_Text (pDX, IDC_ColumnInterval, m_ColumnInterval);
	DDX_Text (pDX, IDC_Horizontal_channel, m_newXAxisFeature);
	DDV_MinMaxUInt (pDX, m_newXAxisFeature, 1, m_maxChannelFeatureNum);
	DDX_Text (pDX, IDC_Vertical_channel, m_newYAxisFeature);
	DDV_MinMaxUInt (pDX, m_newYAxisFeature, 1, m_maxChannelFeatureNum);
	DDX_Text2 (pDX, IDC_ThresholdLevel, m_thresholdPercent);


	if (m_displayPixelCode & kDisplayPixels)
		m_displayPixelCode = 1;
	else
		m_displayPixelCode = 0;

	if (m_outlineClassCode & kOutlineClasses)
		m_outlineClassCode = 1;
	else
		m_outlineClassCode = 0;

	mb_displayPixelCode = m_displayPixelCode;
	mb_outlineClassCode = m_outlineClassCode;

	DDX_Check (pDX, IDC_ThresholdPixelCheck, m_thresholdFlag);
	DDX_Check (pDX, IDC_CreateNewGraphWindow, m_createNewGraphWindowFlag);
	DDX_Check (pDX, IDC_FeatureTransformation, m_featureTransformationFlag);
	DDX_Check (pDX, IDC_Training, m_trainingAreaFlag);

	DDX_Check (pDX, IDC_TestAreas, m_testFlag); 
	DDX_Check (pDX, IDC_Area, m_imageAreaFlag);
	DDX_Check (pDX, IDC_DisplayPixels, mb_displayPixelCode);
	DDX_Check (pDX, IDC_OutlineClassAsEllipse, mb_outlineClassCode);

	DDX_CBIndex (pDX, IDC_ClassCombo, m_classSelection);
	DDX_CBIndex (pDX, IDC_WeightsCombo, m_classWeightsSelection);

	m_displayPixelCode = (Boolean)mb_displayPixelCode;
	m_outlineClassCode = (Boolean)mb_outlineClassCode;

}	// end "DoDataExchange"



BOOL CMBiPlotDialog::DoDialog ()

{
	INT_PTR								returnCode;

	BOOL									continueFlag = FALSE;

	
			// Make sure intialization has been completed.
	
	if (!m_initializedFlag)
																				return (continueFlag);
	
	returnCode = DoModal ();

	if (returnCode == IDOK)
		{
		continueFlag = TRUE;

		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;

		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;

		BiPlotDataDialogOK (this,
									gBiPlotDataSpecsPtr,
									&m_dialogSelectArea,
									m_newXAxisFeature,
									m_newYAxisFeature,
									m_featureTransformationFlag,
									m_createNewGraphicsWindowFlag,
									m_plotDataCode,
									m_displayPixelCode,
									m_outlineClassCode,
									m_thresholdPercent,
									m_thresholdFlag,
									m_classSelection,
									(UInt16)m_localNumberClasses,
									m_classListPtr,
									m_symbolSelection,
									m_localSymbolsPtr,
									m_classWeightsSelection + 1,
									m_classWeightsPtr);
	
		}	// end "if (returnCode == IDOK)"

	return (continueFlag);

}	// end "DoDialog"



void CMBiPlotDialog::OnChangeVerticalAxis ()

{
	m_checkChannelStatisticsFlag = TRUE;
	
}	// end "OnChangeVerticalAxis"



void CMBiPlotDialog::OnChangeHorizontalAxis ()

{
	m_checkChannelStatisticsFlag = TRUE;
	
}	// end "OnChangeHorizontalAxis"



void CMBiPlotDialog::OnCreateNewWindowFlag ()

{
	DDX_Check (m_dialogFromPtr,
					IDC_CreateNewGraphWindow,
					m_createNewGraphicsWindowFlag);

}	// end "OnCreateNewWindowFlag"



void CMBiPlotDialog::OnDisplayPixelAsSymbol ()

{
	BOOL 									displayPixel;
	
	
	DDX_Check (m_dialogFromPtr, IDC_DisplayPixels, displayPixel);
	if (displayPixel == 0)
		m_displayPixelCode &= (kPixelsAsSymbols + kPixelsAsColor);
	
	else	// displayPixel != 0
		m_displayPixelCode |= kDisplayPixels;

}	// end "OnDisplayPixelAsSymbol"



void CMBiPlotDialog::OnFeatureTranformation ()

{
	DDX_Check (m_dialogFromPtr, IDC_FeatureTransformation, m_featureTransformationFlag);

	if (m_featureTransformationFlag)
		{
		MHideDialogItem (this, IDC_ChannelsPrompt);
		MShowDialogItem (this, IDD_FeatureSelection);

		}	// end "if (m_featureTransformationFlag)"
	
	else	// !m_featureTransformationFlag
		{
		MShowDialogItem (this, IDC_ChannelsPrompt);
		MHideDialogItem (this, IDD_FeatureSelection);
		
		}	// end "else !m_featureTransformationFlag"

}	// end "OnFeatureTranformation"



void CMBiPlotDialog::OnImageArea ()

{
	UInt16 								selectItem;
	

	DDX_Check (m_dialogFromPtr, IDC_Area, m_imageAreaFlag);

	if (m_imageAreaFlag)
		{
		ShowSomeAreaSelectionItems ();
		selectItem = IDC_LineStart;

		}	// end "if (m_areaFlag)"

	else	// !m_areaFlag
		{
		HideSomeAreaSelectionItems ();
		selectItem = IDC_LineInterval;

		}	// end "else !m_areaFlag"

	SelectDialogItemText (this, selectItem, 0, SInt16_MAX);

	if (m_imageAreaFlag == 0)
		m_plotDataCode -= kAreaType;

	else	// m_imageAreaFlag != 0
		m_plotDataCode += kAreaType;

}	// end "OnImageArea"



BOOL CMBiPlotDialog::OnInitDialog ()

{
	SInt32								maxChannelFeatureNum;

	SInt16								classSelection,
											newXAxisFeature,
											newYAxisFeature,
											symbolSelection = 0,
											weightsSelection = 0;
	

	CMDialog::OnInitDialog ();

			// Make sure that we have the bitmaps for the entire image buttons.

	VERIFY (toEntireButton.AutoLoad (IDEntireImage, this));
	VERIFY (toSelectedButton.AutoLoad (IDSelectedImage, this));

	BiPlotDataDialogInitialize (this,
											gBiPlotDataSpecsPtr,
											&m_dialogSelectArea,
											gImageWindowInfoPtr,
											gActiveImageWindow,
											m_classListPtr,
											m_localSymbolsPtr,
											&newXAxisFeature,
											&newYAxisFeature,
											(Boolean*)&m_featureTransformationFlag,
											(Boolean*)&m_featureTransformAllowedFlag,
											(Boolean*)&m_checkFeatureTransformFlag,
											&maxChannelFeatureNum,
											&m_plotDataCode,
											&m_displayPixelCode,
											&m_outlineClassCode,
											(Boolean*)&m_thresholdFlag,
											&m_saveThresholdPercent,
											&classSelection,
											&m_localNumberClasses,
											&symbolSelection,
											&weightsSelection,
											(Boolean*)&m_createNewGraphicsWindowFlag);

	m_newXAxisFeature = newXAxisFeature;
	m_newYAxisFeature = newYAxisFeature;
	m_maxChannelFeatureNum = maxChannelFeatureNum;

	m_LineStart = m_dialogSelectArea.lineStart;
	m_LineEnd = m_dialogSelectArea.lineEnd;
	m_LineInterval = m_dialogSelectArea.lineInterval;
	m_ColumnStart = m_dialogSelectArea.columnStart;
	m_ColumnEnd = m_dialogSelectArea.columnEnd;
	m_ColumnInterval = m_dialogSelectArea.columnInterval;

	m_thresholdPercent = m_saveThresholdPercent;
	
			// Classes to use.

	m_classSelection = classSelection;

			// Class weights.
			// Adjust for 0 base index.

	m_classWeightsSelection = weightsSelection - 1;

			// Symbols to use.
			// User defined symbols are not available yet.

	m_symbolSelection = symbolSelection;

			// Set check boxes for "pixels to plot".
	if (m_plotDataCode & kTrainingType)
		m_trainingAreaFlag = TRUE;

	if (m_plotDataCode & kTestingType)
		m_testFlag = TRUE;

	if (m_plotDataCode & kAreaType)
		m_imageAreaFlag = TRUE;

			// Hide some of the area selection boxes if needed.

	if (!(m_plotDataCode & kAreaType))
		HideSomeAreaSelectionItems ();

	if (UpdateData (FALSE))
		PositionDialogWindow ();

	SelectDialogItemText (this, IDC_Horizontal_channel, 0, SInt16_MAX);

	return FALSE;
	
}	// end "OnInitDialog"



void CMBiPlotDialog::OnOutlineClassAsEllipse ()

{
	BOOL 									outlineClass;
	
	
	DDX_Check (m_dialogFromPtr, IDC_OutlineClassAsEllipse, outlineClass);
	if (outlineClass == 0)
		m_outlineClassCode &= kClassesAsEllipses + kClassesAsBorders;

	else	// outlineClass != 0
		m_outlineClassCode |= kOutlineClasses;

	BiPlotDataDialogHideShowClassItems (this, m_plotDataCode, m_outlineClassCode);

}	// end "OnOutlineClassAsEllipse"



void CMBiPlotDialog::OnSelendokClassWeightsCombo ()

{
	HandleClassWeightsMenu ((SInt16)gBiPlotDataSpecsPtr->numberClasses,
									(SInt16*)gBiPlotDataSpecsPtr->classPtr,
									m_classWeightsPtr,
									gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
									IDC_WeightsCombo,
									&m_classWeightsSelection);

}	// end "OnSelendokClassPairWeightsCombo"



void CMBiPlotDialog::OnSelendokClassCombo ()

{
	HandleClassesMenu (&m_localNumberClasses,
								(SInt16*)m_classListPtr,
								1,
								(SInt16)gProjectInfoPtr->numberStatisticsClasses,
								IDC_ClassCombo,
								&m_classSelection);

}	// end "OnSelendokClassCombo"



void CMBiPlotDialog::OnTestArea ()

{

	DDX_Check (m_dialogFromPtr, IDC_TestAreas, m_testFlag);

	if (m_testFlag == 0)
		m_plotDataCode -= kTestingType;

	else	// m_testFlag != 0
		m_plotDataCode += kTestingType;

	BiPlotDataDialogHideShowClassItems (this, m_plotDataCode, m_outlineClassCode);

}	// end "OnTestArea"



void CMBiPlotDialog::OnThresholdPixelFlag ()

{
	DDX_Check (m_dialogFromPtr, IDC_ThresholdPixelCheck, m_thresholdFlag);

}	// end "OnThresholdPixelFlag"



void CMBiPlotDialog::OnTrainArea ()

{
	DDX_Check (m_dialogFromPtr, IDC_Training, m_trainingAreaFlag);

	if (m_trainingAreaFlag == 0)
		m_plotDataCode -= kTrainingType;

	else	// m_trainingAreaFlag != 0
		m_plotDataCode += kTrainingType;

	BiPlotDataDialogHideShowClassItems (this, m_plotDataCode, m_outlineClassCode);

}	// end "OnTrainArea
