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
//	File:						WFieldsToThematicDialog.cpp : implementation file
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
//								CMFieldsToThematicDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WFieldsToThematicDialog.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMFieldsToThematicDialog, CMDialog)
	//{{AFX_MSG_MAP (CMFieldsToThematicDialog)
	ON_BN_CLICKED (IDEntireImage, ToEntireImage)
	ON_BN_CLICKED (IDSelectedImage, ToSelectedImage)

	ON_CBN_SELENDOK (IDC_ClassCombo, OnSelendokClassCombo)

	ON_EN_CHANGE (IDC_ColumnEnd, CheckColumnEnd)
	ON_EN_CHANGE (IDC_ColumnStart, CheckColumnStart)
	ON_EN_CHANGE (IDC_LineEnd, CheckLineEnd)
	ON_EN_CHANGE (IDC_LineStart, CheckLineStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMFieldsToThematicDialog::CMFieldsToThematicDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMFieldsToThematicDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMFieldsToThematicDialog)
	m_trainingAreasFlag = FALSE;
	m_testingAreasFlag = FALSE;
	m_outputFormatCode = 2;
	//}}AFX_DATA_INIT 
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	                        
	m_reformatOptionsPtr = NULL;
	
}	// end "CMFieldsToThematicDialog"



void CMFieldsToThematicDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMFieldsToThematicDialog)
	DDX_Check (pDX, IDC_TrainingAreas, m_trainingAreasFlag);
	DDX_Check (pDX, IDC_TestingAreas, m_testingAreasFlag);
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
	DDX_CBIndex (pDX, IDC_ClassCombo, m_classSelection);
	DDX_CBIndex (pDX, IDC_OutputFormatCombo, m_outputFormatCode);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the project
//							areas to thematic image dialog box to the user and copy the
//							revised items back to the specification structure if
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
//	Coded By:			Larry L. Biehl			Date: 07/29/1998
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean CMFieldsToThematicDialog::DoDialog (
				ReformatOptionsPtr				reformatOptionsPtr)

{  
	DialogSelectArea					dialogSelectArea;     
	
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE;						

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
																			
	m_reformatOptionsPtr = reformatOptionsPtr;
																					
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

		LoadAreasToThematicDialogOK (reformatOptionsPtr,
												&dialogSelectArea,
												m_trainingAreasFlag,
												m_testingAreasFlag,
												m_classSelection,
												m_localNumberClasses,
												m_outputFormatCode+1);
		
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



BOOL CMFieldsToThematicDialog::OnInitDialog ()

{                        
	SInt16								classSelection,
											outputFormatCode;
	
	
	CDialog::OnInitDialog ();
	
			// Make sure that we have the bitmaps for the entire image buttons.
		
	VERIFY (toEntireButton.AutoLoad (IDEntireImage, this));
	VERIFY (toSelectedButton.AutoLoad (IDSelectedImage, this));

	LoadAreasToThematicDialogInitialize (this,
														m_reformatOptionsPtr,
														&m_dialogSelectArea,
														(Boolean*)&m_trainingAreasFlag,
														(Boolean*)&m_testingAreasFlag,
														&classSelection,
														&m_classListPtr,
														&m_localNumberClasses,
														&outputFormatCode);
				
			// Selected area to classify.
                  
	m_LineStart = m_reformatOptionsPtr->lineStart;
	m_LineEnd = m_reformatOptionsPtr->lineEnd;
	m_LineInterval = m_reformatOptionsPtr->lineInterval;
	m_ColumnStart = m_reformatOptionsPtr->columnStart;
	m_ColumnEnd = m_reformatOptionsPtr->columnEnd;
	m_ColumnInterval = m_reformatOptionsPtr->columnInterval;
	
			// Update the maximum number of lines and columns for later use.
	                                                            
	m_maxNumberLines = m_dialogSelectArea.imageWindowInfoPtr->maxNumberLines;
	m_maxNumberColumns = m_dialogSelectArea.imageWindowInfoPtr->maxNumberColumns; 
	
			// Classes to use.
			
	m_classSelection = classSelection;	

			// Output format to use.

	m_outputFormatCode = outputFormatCode - 1;
		
			// Determine if this is the entire area and set the 
			// to entire image icon.
	                     
	SetEntireImageButtons (NULL,
									m_LineStart, 
									m_LineEnd, 
									m_ColumnStart, 
									m_ColumnEnd); 
	
	if (UpdateData (FALSE))
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item	
		                                       
	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX); 
	
	return FALSE;  // return TRUE  unless you set the focus to a control; 

}	// end "OnInitDialog"
