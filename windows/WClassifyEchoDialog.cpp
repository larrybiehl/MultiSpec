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
//	File:						WClassifyEchoDialog.cpp : implementation file
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
//								CMEchoClassifyDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
                      
#include "WClassifyEchoDialog.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMEchoClassifyDialog, CMDialog)
	//{{AFX_MSG_MAP (CMEchoClassifyDialog)
	ON_BN_CLICKED (IDC_LogLike, OnLogLike)
	ON_BN_CLICKED (IDC_Percent, OnPercent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMEchoClassifyDialog::CMEchoClassifyDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMEchoClassifyDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMEchoClassifyDialog)
	m_cellWidth = 2;
	m_annexationThreshold = 0;
	m_homogeneityThreshold = 0;
	m_combineLikeFieldsFlag = FALSE;
	m_createHomogeneousFilesFlag = FALSE;
	m_mixCellsFlag = FALSE;
	m_homogeneousThresholdType = 0;
	m_echoAlgorithmProcedure = -1;
	//}}AFX_DATA_INIT
	
	m_minPhase1 = 0.;
	m_maxPhase1 = 100.;
	
	m_initializedFlag = CMDialog::m_initializedFlag; 
	
}	// end "CMEchoClassifyDialog"



CMEchoClassifyDialog::~CMEchoClassifyDialog (void)

{              
	
}	// end "~CMEchoClassifyDialog"



void CMEchoClassifyDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMEchoClassifyDialog)
	DDX_Text (pDX, IDC_CellWidth, m_cellWidth);
	DDV_MinMaxLong (pDX, m_cellWidth, 1, 100);
	DDX_Text2 (pDX, IDC_AnnexationThreshold, m_annexationThreshold);
	DDV_MinMaxDouble (pDX, m_annexationThreshold, 0., 100.);
	DDX_Text2 (pDX, IDC_HomogeneityThreshold, m_homogeneityThreshold);
	DDV_MinMaxDouble (pDX, m_homogeneityThreshold, m_minPhase1, m_maxPhase1);
	DDX_Check (pDX, IDC_CombineLikeFields, m_combineLikeFieldsFlag);
	DDX_Check (pDX, IDC_CreateHomogeneousFiles, m_createHomogeneousFilesFlag);
	DDX_Check (pDX, IDC_MixCells, m_mixCellsFlag);
	DDX_Radio (pDX, IDC_Percent, m_homogeneousThresholdType);
	DDX_CBIndex (pDX, IDC_EchoAlgorithm, m_echoAlgorithmProcedure);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the echo
//							classification specification dialog box to the user and copy the
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
//	Coded By:			Larry L. Biehl			Date: 04/18/1996
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

SInt16 CMEchoClassifyDialog::DoDialog (
				EchoClassifierVarPtr				echoClassifierVarPtr)

{  
	INT_PTR								returnCode;
	Boolean								continueFlag = FALSE;
	

			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																					return (FALSE);
																			
	m_echoClassifierVarPtr = echoClassifierVarPtr;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE; 
	
				// Cell Width.													
						
		m_echoClassifierVarPtr->cell_width = (SInt16)m_cellWidth;
												
		m_echoClassifierVarPtr->cell_size = 
				m_echoClassifierVarPtr->cell_width * m_echoClassifierVarPtr->cell_width;
					
				// Homogeneity (phase 1) threshold.						
						
		m_echoClassifierVarPtr->homogeneityThreshold = m_homogeneityThreshold;
				
				// Homogeniety (phase 1) Threshold specified in 		
				// percent probability correct.										
				
		m_echoClassifierVarPtr->fixed_homogeneityThreshold_option = 
																		m_homogeneousThresholdType;
					
				// Annexation threshold.									
						
		m_echoClassifierVarPtr->annexationThreshold = m_annexationThreshold;
	
				// Flag indicating whether to merge like fields.	
				
		m_echoClassifierVarPtr->combineLikeFieldsFlag = 
																	m_combineLikeFieldsFlag; 
	
				// Flag indicating whether to create image files with the	
				// homogeneous fields and classes.									
																
		m_echoClassifierVarPtr->createHomogeneousFilesFlag =  
																	m_createHomogeneousFilesFlag;
	
				// Flag indicating whether to allow merging of 					
				// cells and fields which are of different classes.			
				
		m_echoClassifierVarPtr->mixCellsFlag = m_mixCellsFlag;
		
				// Set classification algorithm to use for echo. 
		
		m_echoClassifierVarPtr->algorithmCode = m_echoAlgorithmProcedure + 1;
		
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



BOOL CMEchoClassifyDialog::OnInitDialog ()

{
	CMDialog::OnInitDialog ();
	
			// Cell Width.							
	
	m_cellWidth = m_echoClassifierVarPtr->cell_width;
	
			// Homogeniety Threshold (phase 1).
			// Threshold specified in percent		
			// probability correct. (> 0.0 && < 100.0)
			// or in -Log Likelihood.	 (> 0.0 && < 1000000.0)													
                                 
	m_homogeneityThreshold = m_echoClassifierVarPtr->homogeneityThreshold;
	                        
	m_homogeneousThresholdType = 0;
   if (m_echoClassifierVarPtr->fixed_homogeneityThreshold_option)
		m_homogeneousThresholdType = 1;
						 
	m_minPhase1 = 0; 
	m_savedPhase1PercentThreshold = 2.;
	m_savedPhase1LogLikelihoodThreshold = 95.;
	
	if (m_homogeneousThresholdType == 1)
		{
		m_maxPhase1 = 1000000.;
		m_savedPhase1LogLikelihoodThreshold = m_homogeneityThreshold;
											
		}	// end "if (m_homogeneousThresholdType == 1)"
		
	else	// m_homogeneousThresholdType == 0
		{
		m_maxPhase1 = 100.;
		m_savedPhase1PercentThreshold = m_homogeneityThreshold;
											
		}	// end "else m_homogeneousThresholdType == 0"
	
			// Annexation Threshold. (>= 0.0 && < 10.0)						
		
	m_annexationThreshold = m_echoClassifierVarPtr->annexationThreshold;
	
			// Flag indicating whether to merge like fields.				
	
	m_combineLikeFieldsFlag = m_echoClassifierVarPtr->combineLikeFieldsFlag;
	
			// Flag indicating whether to create image files with the				
			// homogeneous fields and classes.												
	
	m_createHomogeneousFilesFlag = 
								m_echoClassifierVarPtr->createHomogeneousFilesFlag;
	
			// Flag indicating whether to allow merging of cells and 				
			// fields which are of different classes.										
	                               
	m_mixCellsFlag = m_echoClassifierVarPtr->mixCellsFlag;
	MHideDialogItem (this, IDC_MixCells);
	
			// Classification procedure
			
	m_echoAlgorithmProcedure = m_echoClassifierVarPtr->algorithmCode - 1;
	                  
	if (UpdateData (FALSE))
		PositionDialogWindow ();
		                                       
	SelectDialogItemText (this, IDC_CellWidth, 0, SInt16_MAX);
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"



void CMEchoClassifyDialog::OnLogLike ()

{
	if (m_homogeneousThresholdType == 0)
		{
		DDX_Radio (m_dialogFromPtr, IDC_Percent, m_homogeneousThresholdType);
			
		DDX_Text2 (m_dialogFromPtr, IDC_HomogeneityThreshold, m_homogeneityThreshold);
			
		m_maxPhase1 = 1000000.;
		m_savedPhase1PercentThreshold = m_homogeneityThreshold;
		m_homogeneityThreshold = m_savedPhase1LogLikelihoodThreshold;
			
		DDX_Text2 (m_dialogToPtr, IDC_HomogeneityThreshold, m_homogeneityThreshold);
			
		}	// end "if (m_homogeneousThresholdType == 0)"
	
}	// end "OnLogLike"



void CMEchoClassifyDialog::OnPercent ()

{ 			
	if (m_homogeneousThresholdType == 1)
		{                
		DDX_Radio (m_dialogFromPtr, IDC_Percent, m_homogeneousThresholdType);
		                                                          
		DDX_Text2 (m_dialogFromPtr, IDC_HomogeneityThreshold, m_homogeneityThreshold);
		                                
		m_maxPhase1 = 100.;
		m_savedPhase1LogLikelihoodThreshold = m_homogeneityThreshold;
		m_homogeneityThreshold = m_savedPhase1PercentThreshold;  
		                                                          
		DDX_Text2 (m_dialogToPtr, IDC_HomogeneityThreshold, m_homogeneityThreshold);
										
		}	// end "if (m_homogeneousThresholdType == 1)"
	
}	// end "OnPercent"
