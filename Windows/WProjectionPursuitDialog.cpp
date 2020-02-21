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
//	File:						MProjectionPursuitDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/03/2018
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMProjectionPursuitDialog class.
//
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h"

#include "WProjectionPursuitDialog.h"



BEGIN_MESSAGE_MAP (CMProjectionPursuitDialog, CMDialog)
	ON_BN_CLICKED (IDC_FS_Algorithm, OnBnClickedAlgorithm)
	ON_BN_CLICKED (IDC_LastFinalGrouping, OnBnClickedInitialBandGrouping)
	ON_BN_CLICKED (IDC_NumericalOptimization, OnBnClickedNumericaloptimization)
	ON_BN_CLICKED (IDC_PP_Algorithm, OnBnClickedAlgorithm)
	ON_BN_CLICKED (IDC_TopDownBottomUpMethod, OnBnClickedBandGroupingMethod)
	ON_BN_CLICKED (IDC_TopDownMethod, OnBnClickedBandGroupingMethod)
	ON_BN_CLICKED (IDC_Uniform, OnBnClickedBandGroupingMethod)
	ON_BN_CLICKED (IDC_UniformGrouping, OnBnClickedInitialBandGrouping)
END_MESSAGE_MAP ()



CMProjectionPursuitDialog::CMProjectionPursuitDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMProjectionPursuitDialog::IDD, pParent)
		
{
	//{{AFX_DATA_INIT (CMProjectionPursuitDialog)
	m_algorithmCode = 0;
	m_numericalOptimizationFlag = FALSE;
	m_optimizeThreshold =  0;
	m_initialGroupingCode = 0;
	m_firstStageMethod = 1;
	m_topDownThreshold = 0;
	m_bottomUpThreshold = 0;
	m_bothOddChoicesNumberFeatures = 0;
	m_maximumNumberFeatures = 0;
	m_initialNumberFeatures = 0;
	//}}AFX_DATA_INIT
	
	m_initializedFlag = TRUE;
	
}	// end "CMProjectionPursuitDialog"



CMProjectionPursuitDialog::~CMProjectionPursuitDialog ()

{

}	// end "~CMProjectionPursuitDialog"



void CMProjectionPursuitDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMProjectionPursuitDialog)
	DDX_Radio (pDX, IDC_PP_Algorithm, m_algorithmCode);
	DDX_Check (pDX, IDC_NumericalOptimization, m_numericalOptimizationFlag);
	DDX_Text2 (pDX, IDC_OptimizationThreshold, m_optimizeThreshold);
	DDV_MinMaxDouble (pDX, m_optimizeThreshold, 0, 100);
	DDX_Radio (pDX, IDC_UniformGrouping, m_initialGroupingCode);
	DDX_Radio (pDX, IDC_Uniform, m_firstStageMethod);
	DDX_Text2 (pDX, IDC_TopDownThreshold, m_topDownThreshold);
	DDV_MinMaxDouble (pDX, m_topDownThreshold, 0, 100);
	DDX_Text2 (pDX, IDC_BottomUpThreshold, m_bottomUpThreshold);
	DDV_MinMaxDouble (pDX, m_bottomUpThreshold, 0, 100);
	DDX_Text (pDX, IDC_BothChoicesUpToValue, m_bothOddChoicesNumberFeatures);
	DDV_MinMaxLong (pDX, m_bothOddChoicesNumberFeatures, 0, gProjectInfoPtr->numberStatisticsChannels);
	DDX_Text (pDX, IDC_MaxNumberOutputFeatures, m_maximumNumberFeatures);
	DDV_MinMaxLong (pDX, m_maximumNumberFeatures, 1, gProjectInfoPtr->numberStatisticsChannels);
	DDX_Text (pDX, IDC_EditNumberFeature, m_initialNumberFeatures);
	DDV_MinMaxLong (pDX, m_initialNumberFeatures, 1, gProjectInfoPtr->numberStatisticsChannels);
	//}}AFX_DATA_MAP

}	// end "DoDataExchange"



Boolean CMProjectionPursuitDialog::DoDialog (
				ProjectionPursuitSpecsPtr		projectionPursuitSpecsPtr)

{  
	INT_PTR								returnCode;
	Boolean								continueFlag = FALSE;

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
																																								
	m_projectionPursuitSpecsPtr = projectionPursuitSpecsPtr;

	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE; 
		ProjectionPursuitDialogOK (m_projectionPursuitSpecsPtr,
												m_numericalOptimizationFlag,
												m_algorithmCode+1,
												m_initialGroupingCode+1,
												m_initialNumberFeatures,
												m_firstStageMethod+1,
												m_optimizeThreshold,
												m_topDownThreshold,
												m_bottomUpThreshold,
												m_maximumNumberFeatures,
												m_bothOddChoicesNumberFeatures);

		}	// end "if (returnCode == IDOK)"
			
	return (continueFlag);
		
}	// end "DoDialog"



void CMProjectionPursuitDialog::OnBnClickedAlgorithm ()

{
	DDX_Radio (m_dialogFromPtr, IDC_PP_Algorithm, m_algorithmCode);

	ProjectionPursuitDialogAlgorithm (
									this, m_algorithmCode+1, m_numericalOptimizationFlag);

	ProjectionPursuitDialogFSOptions (this, m_algorithmCode+1, m_firstStageMethod+1);

}	// end "OnBnClickedAlgorithm"



void CMProjectionPursuitDialog::OnBnClickedBandGroupingMethod ()

{
	DDX_Radio (m_dialogFromPtr, IDC_Uniform, m_firstStageMethod);

	ProjectionPursuitDialogFSMethod (this, m_firstStageMethod+1);

}	// end "OnBnClickedBandGroupingMethod"



void CMProjectionPursuitDialog::OnBnClickedInitialBandGrouping ()

{
	DDX_Radio (m_dialogFromPtr, IDC_UniformGrouping, m_initialGroupingCode);

	ProjectionPursuitDialogInitialGrouping (this, m_initialGroupingCode+1);

}	// end "OnBnClickedInitialBandGrouping"



void CMProjectionPursuitDialog::OnBnClickedNumericaloptimization ()

{
	DDX_Check (m_dialogFromPtr,
					IDC_NumericalOptimization, 
					m_numericalOptimizationFlag);

	ProjectionPursuitDialogOptimizeFlag (this, m_numericalOptimizationFlag);

}	// end "OnBnClickedNumericaloptimization"



BOOL CMProjectionPursuitDialog::OnInitDialog ()

{
	SInt16								algorithmCode,
											firstStageMethod,
											initialGroupingCode;

	Boolean								numericalOptimizationFlag;


	CDialog::OnInitDialog ();
	
			// Add extra initialization here
	
	ProjectionPursuitDialogInitialize (this,
											 		m_projectionPursuitSpecsPtr,
													&numericalOptimizationFlag,
													&algorithmCode,
													&initialGroupingCode,
													&m_initialNumberFeatures,
													&m_finalNumberFeatures,
													&firstStageMethod,
													&m_optimizeThreshold,
													&m_topDownThreshold,
													&m_bottomUpThreshold,
													&m_maximumNumberFeatures,
													&m_bothOddChoicesNumberFeatures);
	
	m_numericalOptimizationFlag = numericalOptimizationFlag;
	m_algorithmCode = algorithmCode - 1;
	m_initialGroupingCode = initialGroupingCode - 1;
	m_firstStageMethod = firstStageMethod - 1;

	if (UpdateData (FALSE))
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item
	
	SelectDialogItemText (this, IDC_MaxNumberOutputFeatures, 0, SInt16_MAX);
	
	return FALSE;  // return TRUE unless you set the focus to a control

}	// end "OnInitDialog"
