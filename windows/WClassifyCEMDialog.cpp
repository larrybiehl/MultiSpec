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
//	File:						WClassifyCEMDialog.cpp : implementation file
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
//								CMCEMClassifyDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WClassifyCEMDialog.h"
#include "WMultiSpec.h" 

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMCEMClassifyDialog, CMDialog)
	//{{AFX_MSG_MAP (CMCEMClassifyDialog)
	ON_BN_CLICKED (IDC_UseClasses, OnUseClasses)
	ON_BN_CLICKED (IDC_UseSelectedArea, OnUseSelectedArea)
	ON_BN_CLICKED (IDEntireImage, ToEntireImage)
	ON_BN_CLICKED (IDSelectedImage, ToSelectedImage)

	ON_CBN_SELENDOK (IDC_ClassCombo, OnSelendokClassCombo)

	ON_EN_CHANGE(IDC_ColumnEnd, CheckColumnEnd)
	ON_EN_CHANGE(IDC_ColumnStart, CheckColumnStart)
	ON_EN_CHANGE(IDC_LineEnd, CheckLineEnd)
	ON_EN_CHANGE(IDC_LineStart, CheckLineStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMCEMClassifyDialog::CMCEMClassifyDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMCEMClassifyDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT(CMCEMClassifyDialog)
	m_correlationMatrixCode = -1;
	m_trainingFieldsFlag = FALSE;
	m_testFieldsFlag = FALSE;
	//}}AFX_DATA_INIT   
	
	m_cemParametersPtr = NULL; 
	m_localClassAreaPtr = NULL; 

	m_dialogSelectArea.windowPtr = gProjectSelectionWindow;
	
	m_initializedFlag = CMDialog::m_initializedFlag; 
	
	if (m_initializedFlag)
		{
				// Get memory for the local class area list vector.	
				
		m_localClassAreaPtr = (SInt16*)MNewPointer (
				(UInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));
	                                                       
		m_initializedFlag = (m_localClassAreaPtr != NULL);
		
		}	// end "if (m_initializedFlag)"
	
}	// end "CMCEMClassifyDialog"



void CMCEMClassifyDialog::BackgroundCorrelationSettings (
				Boolean								areaCode)

{
	ShowHideDialogItem (this, IDC_ClassCombo, (areaCode == 1));
	HideShowAreaItems (areaCode == 2);
	
			// Determine if this is the entire area and set the
			// to entire image icon.
	
	if (areaCode == 2)
		{
		SetEntireImageButtons (
						NULL,
						m_LineStart,
						m_LineEnd,
						m_ColumnStart,
						m_ColumnEnd);
		
		SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);
		
		}	// end "if (areaCode == 2)"
	
}	// end "BackgroundCorrelationSettings"



void CMCEMClassifyDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMCEMClassifyDialog) 
	DDX_CBIndex (pDX, IDC_ClassCombo, m_classSelection);
	DDX_Radio (pDX, IDC_UseClasses, m_correlationMatrixCode);
	DDX_Check (pDX, IDC_Training, m_trainingFieldsFlag);
	DDX_Check (pDX, IDC_Test, m_testFieldsFlag);
	DDX_Text (pDX, IDC_LineEnd, m_LineEnd);
	DDV_MinMaxLong (pDX, m_LineEnd, 1, m_maxNumberLines);
	DDX_Text (pDX, IDC_LineInterval, m_LineInterval);
	DDV_MinMaxLong (pDX, m_LineInterval, 1, m_maxNumberLines);
	DDX_Text (pDX, IDC_LineStart, m_LineStart);                              
	DDV_MinMaxLong (pDX, m_LineStart, 1, m_maxNumberLines); 
	DDX_Text (pDX, IDC_ColumnEnd, m_ColumnEnd);
	DDV_MinMaxLong (pDX, m_ColumnEnd, 1, m_maxNumberColumns);
	DDX_Text (pDX, IDC_ColumnInterval, m_ColumnInterval);
	DDV_MinMaxLong (pDX, m_ColumnInterval, 1, m_maxNumberColumns);
	DDX_Text (pDX, IDC_ColumnStart, m_ColumnStart);
	DDV_MinMaxLong (pDX, m_ColumnStart, 1, m_maxNumberColumns);
	//}}AFX_DATA_MAP

				// Verify that the line and column values make sense
	
	if (m_correlationMatrixCode == 1)
		VerifyLineColumnStartEndValues (pDX);

}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the CEM
//							specification dialog box to the user and copy the
//							revised back to the classify specification structure if
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
//	Coded By:			Larry L. Biehl			Date: 04/09/1998
//	Revised By:			Larry L. Biehl			Date: 12/21/2016	

Boolean CMCEMClassifyDialog::DoDialog (
				CEMParametersPtr					cemParametersPtr,
				UInt16*								classifyProcedureEnteredCodePtr)

{  
	DialogSelectArea				dialogSelectArea;
	
	INT_PTR							returnCode;
	
	Boolean							continueFlag = FALSE;
								

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																					return (FALSE);
	
	m_cemParametersPtr = cemParametersPtr;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE;
		                  
		dialogSelectArea.lineStart = m_LineStart;
		dialogSelectArea.lineEnd = m_LineEnd;
		dialogSelectArea.lineInterval = m_LineInterval;
																
		dialogSelectArea.columnStart = m_ColumnStart;
		dialogSelectArea.columnEnd = m_ColumnEnd;
		dialogSelectArea.columnInterval = m_ColumnInterval;
		
		CEMClassifyDialogOK (cemParametersPtr,
									&dialogSelectArea,
									m_correlationMatrixClassPtr,
									m_localClassAreaPtr,
									(SInt16)m_localNumberClassAreas,
									m_classSelection,
									(m_correlationMatrixCode == 0),
									classifyProcedureEnteredCodePtr);
		
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



BOOL CMCEMClassifyDialog::OnInitDialog ()

{
	CDialog::OnInitDialog ();   
	
			// Make sure that we have the bitmaps for the entire image buttons.
		
	VERIFY (toEntireButton.AutoLoad (IDEntireImage, this));
	VERIFY (toSelectedButton.AutoLoad (IDSelectedImage, this)); 
	
			// Indicate whether the class areas are to be used to compute the
			// correlation matrix.
	
	if (m_cemParametersPtr->correlationMatrixCode & kBothFieldTypes)
		m_correlationMatrixCode = 0;
		
			// Or the selected area. 
	
	if (m_cemParametersPtr->correlationMatrixCode & kAreaType)
		m_correlationMatrixCode = 1;

	ShowHideDialogItem (this, 
								IDC_UseClasses, 
								(gProjectInfoPtr->statisticsCode == kMeanCovariance));
				
	ShowHideDialogItem (this, 
								IDC_ClassCombo, 
								(m_cemParametersPtr->correlationMatrixCode & kBothFieldTypes));
	
			// Use all or subset of class train fields to compute the correlation matrix							
			//	Make all classes the default
			                  
	m_classSelection = m_cemParametersPtr->correlationMatrixClassAreaSet;
										
			// Note that the handle is already locked.
			
	m_correlationMatrixClassPtr = (SInt16*)GetHandlePointer (
										m_cemParametersPtr->correlationMatrixClassHandle);
								
	BlockMoveData (m_correlationMatrixClassPtr, 
						m_localClassAreaPtr, 
						gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16));
						
	m_localNumberClassAreas = m_cemParametersPtr->numbercorrelationMatrixClasses;
										
			// Hide these options for now. Will always use the training field.
	
	HideDialogItem (this, IDC_Training);                              
	HideDialogItem (this, IDC_Test); 
                                  															
			//	Selected area to use to compute the correlation matrix.
                  
	m_LineStart = m_cemParametersPtr->lineStart;
	m_LineEnd = m_cemParametersPtr->lineEnd;
	m_LineInterval = m_cemParametersPtr->lineInterval;
	m_ColumnStart = m_cemParametersPtr->columnStart;
	m_ColumnEnd = m_cemParametersPtr->columnEnd;
	m_ColumnInterval = m_cemParametersPtr->columnInterval; 
	                                                                
   BackgroundCorrelationSettings (m_correlationMatrixCode + 1); 
	                 
	if (UpdateData (FALSE))
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item	
		                                       
	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);         
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"



void CMCEMClassifyDialog::OnSelendokClassCombo ()

{
	HandleClassesMenu (&m_localNumberClassAreas,
								(SInt16*)m_correlationMatrixClassPtr,
								1,
								(SInt16)gProjectInfoPtr->numberStatisticsClasses,
								IDC_ClassCombo,
								&m_classSelection);
	
}	// end "OnSelendokClassCombo"

    
                                     
void CMCEMClassifyDialog::OnUseClasses ()

{                                                                                                     
   BackgroundCorrelationSettings (1); 
	
}	// end "OnUseClasses"


 
void CMCEMClassifyDialog::OnUseSelectedArea ()

{                                                                               
   BackgroundCorrelationSettings (2);             
	
}	// end "OnUseSelectedArea"
