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
//	File:						WStatisticsListDialog.cpp : implementation file
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
//								CMListStatsDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WStatisticsListDialog.h"



BEGIN_MESSAGE_MAP (CMListStatsDialog, CMDialog)
	//{{AFX_MSG_MAP (CMListStatsDialog)
	ON_BN_CLICKED (IDC_Classes, OnClasses)
	ON_BN_CLICKED (IDC_Fields, OnFields)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMListStatsDialog::CMListStatsDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMListStatsDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMListStatsDialog)
	m_listClassFlag = FALSE;
	m_listFieldFlag = FALSE;
	m_listMeansStdDevFlag = FALSE;
	m_listCovarianceFlag = FALSE;
	m_listCorrelationFlag = FALSE;
	m_featureTransformationFlag = FALSE;
	m_listMeanStdPrecision = 1;
	m_listCovCorPrecision = 2;
	//}}AFX_DATA_INIT

	m_initializedFlag = CMDialog::m_initializedFlag;
	
}	// end "CMListStatsDialog"



void CMListStatsDialog::CheckListFieldClassSettings (void)

{  
	Boolean								enableFlag = FALSE;
	
	                                                                                     
	if (m_listFieldFlag || m_listClassFlag)
		enableFlag = TRUE;
	
	GetDlgItem (IDOK)->EnableWindow (enableFlag);
	
}	// end "CheckListFieldClassSettings"



void CMListStatsDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMListStatsDialog)
	DDX_Check (pDX, IDC_Classes, m_listClassFlag);
	DDX_Check (pDX, IDC_Fields, m_listFieldFlag);
	DDX_Check (pDX, IDC_MeansStdDev, m_listMeansStdDevFlag);
	DDX_Check (pDX, IDC_CovarianceMatrix, m_listCovarianceFlag);
	DDX_Check (pDX, IDC_CorrelationMatrix, m_listCorrelationFlag);
	DDX_Check (pDX, IDC_UseTransformation, m_featureTransformationFlag);
	DDX_Text (pDX, IDC_MeanPrecision, m_listMeanStdPrecision);
	DDV_MinMaxUInt (pDX, m_listMeanStdPrecision, 1, 10);
	DDX_Text (pDX, IDC_CovariancePrecision, m_listCovCorPrecision);
	DDV_MinMaxUInt (pDX, m_listCovCorPrecision, 1, 10);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the List Statistics
//							specification dialog box to the user to allow the user to
//							make changes.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/10/1998
//	Revised By:			Larry L. Biehl			Date: 04/16/1998

Boolean CMListStatsDialog::DoDialog (
			SInt16									statsWindowMode)

{
	SInt64								returnCode;
	
	Boolean								continueFlag = FALSE;

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
	
	m_statsWindowMode = statsWindowMode;
																			
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE; 
				
		ListStatsDialogOK (m_listFieldFlag,
									m_listClassFlag,
									m_listMeansStdDevFlag,
									m_listCovarianceFlag,
									m_listCorrelationFlag,
									m_featureTransformationFlag,
									m_listMeanStdPrecision,
									m_listCovCorPrecision);
		
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"

  

void CMListStatsDialog::OnClasses ()

{                                                                   
	DDX_Check (m_dialogFromPtr,
					IDC_Classes, 
					m_listClassFlag); 
					
	CheckListFieldClassSettings ();
	
}	// end "OnClasses"



void CMListStatsDialog::OnFields ()

{                                                               
	DDX_Check (m_dialogFromPtr, IDC_Fields, m_listFieldFlag);
					                               
	CheckListFieldClassSettings ();
	
}	// end "OnFields"



BOOL CMListStatsDialog::OnInitDialog ()

{
	SInt16								listCovCorPrecision,
											listMeanStdPrecision;
											
											
	CDialog::OnInitDialog ();
	
	ListStatsDialogInitialize (this,
										m_statsWindowMode,
										(Boolean*)&m_listFieldFlag,
										(Boolean*)&m_listClassFlag,
										(Boolean*)&m_listMeansStdDevFlag,
										(Boolean*)&m_listCovarianceFlag,
										(Boolean*)&m_listCorrelationFlag,
										(Boolean*)&m_featureTransformationFlag,
										&listMeanStdPrecision,
										&listCovCorPrecision);
											
	m_listMeanStdPrecision = listMeanStdPrecision; 
	m_listCovCorPrecision = listCovCorPrecision;
	                 
	if (UpdateData (FALSE))
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item	
		                                       
	SelectDialogItemText (this, IDC_MeanPrecision, 0, SInt16_MAX);  			
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"
