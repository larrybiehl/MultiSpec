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
//	File:						WNewClassFieldDialog.cpp : implementation file
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
//								CMNewClassFieldDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include	"WNewClassFieldDialog.h" 

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMNewClassFieldDlg, CMDialog)
	//{{AFX_MSG_MAP (CMNewClassFieldDlg)
	ON_BN_CLICKED (IDC_TestField, OnFieldType)
	ON_BN_CLICKED (IDC_Training, OnFieldType)

	ON_CBN_SELENDOK (IDC_ClassList, OnSelendokClassList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMNewClassFieldDlg::CMNewClassFieldDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMNewClassFieldDlg::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMNewClassFieldDlg)
	m_classList = 0;
	m_className = _T("Class 1");
	m_fieldName = _T("Field 1");
	m_fieldType = 0;
	//}}AFX_DATA_INIT 
	
	m_classNameCStringPtr = NULL;
	m_fieldNameCStringPtr = NULL;  
	
	TRY
		{																			
				//	Set the algebraic transform descriptions.	
			
		m_classNameCStringPtr = (TBYTE*)m_className.GetBufferSetLength (64);
		m_fieldNameCStringPtr = (TBYTE*)m_fieldName.GetBufferSetLength (64);
				
		}
			
	CATCH_ALL (e)
		{
		m_initializedFlag = FALSE;
		}
	
	END_CATCH_ALL
	
	m_initializedFlag = TRUE;
	
}	// end "CMNewClassFieldDlg"



void CMNewClassFieldDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	CMDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMNewClassFieldDlg)
	DDX_CBIndex (pDX, IDC_ClassList, m_classList);
	DDX_Text (pDX, IDC_ClassName, m_className);
	DDV_MaxChars (pDX, m_className, 31);
	DDX_Text (pDX, IDC_FieldName, m_fieldName);
	DDV_MaxChars (pDX, m_fieldName, 31);
	DDX_Radio (pDX, IDC_Training, m_fieldType);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the new class - field
//							dialog box to the user to allow the user to make changes.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/09/1996
//	Revised By:			Larry L. Biehl			Date: 03/16/2017	

Boolean CMNewClassFieldDlg::DoDialog (
				Boolean								newClassOnlyFlag,
				UCharPtr								classNamePtr,
				UCharPtr								fieldNamePtr,
				SInt16*								fieldTypePtr)

{       
	USES_CONVERSION;
	
	INT_PTR								returnCode;
	
	UInt8									string[256];
	Boolean								OKFlag = FALSE;
	

			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																					return (FALSE);
																			
	m_newClassOnlyFlag = newClassOnlyFlag; 
	m_classNamePtr = classNamePtr;
	m_fieldNamePtr = fieldNamePtr;
	m_fieldTypePtr = fieldTypePtr;
	            																			
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{ 
		OKFlag = TRUE;   
					
				// Class Name													
                    
		strcpy ((char*)string, T2A (m_classNameCStringPtr));
		CtoPstring (string, classNamePtr);
					
				// Field Identifier
				
		strcpy ((char*)string, T2A (m_fieldNameCStringPtr));
		CtoPstring (string, fieldNamePtr);
					
				// Training field or test field						
				          
		if (m_fieldType == 0)
			*fieldTypePtr = kTrainingType;
						
		if (m_fieldType == 1)
			*fieldTypePtr = kTestingType;
		
		}	// end "if (returnCode == IDOK)"
		
	return (OKFlag);
		
}	// end "DoDialog"



void CMNewClassFieldDlg::OnFieldType ()

{                    
	DDX_Radio (m_dialogFromPtr, IDC_Training, m_fieldType);
	
	SetNumberClassTrainPixels (this, m_numberSelectionPixels, m_fieldType+1);
	
}	// end "OnFieldType"



BOOL CMNewClassFieldDlg::OnInitDialog (void)

{                          
	CMDialog*							dialogPtr = this;
	
	UInt16								selectedItem;
	
	
	CDialog::OnInitDialog ();
	
			// Add current list of classes to the combo box.
			
	CreateClassNameComboBoxList ((CComboBox*)GetDlgItem (IDC_ClassList));
	   
	   	// Set the specified class.
	   	
	m_classList = gProjectInfoPtr->lastClassForAddFieldSelection-1;
	DDX_CBIndex (m_dialogToPtr, IDC_ClassList, m_classList);
	
			// Initialize the dialog box parameters that are common between the
			// Mac and Windows versions.
	
	NewClassFieldDialogInitialize (this, 
												m_newClassOnlyFlag,
												&selectedItem,
												m_fieldNamePtr,
												*m_fieldTypePtr,
												&m_numberSelectionPixels); 

	PositionDialogWindow ();  
		
			// Set default text selection 
	                                                 
	SelectDialogItemText (this, selectedItem, 0, SInt16_MAX);  			 
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"



void CMNewClassFieldDlg::OnOK (void)

{  
	USES_CONVERSION;

	UInt8									string [256];
	
	SInt16								returnCode;
	UInt16								selectedItem;
	
	Boolean 								OKFlag = TRUE;
	
	
	if (!UpdateData (TRUE))
																								return;
	
	strcpy ((char*)string, T2A (m_classNameCStringPtr));
	CtoPstring (string, m_classNamePtr);
					
			// Check if proposed class name is a duplicate.	 If it is, 	
			// allow user to change it again.									
	
	returnCode = CheckForDuplicateClassName (-1, (Str255*)m_classNamePtr);					
	if (returnCode > 0)
		{ 
		if (returnCode == 1)
			DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
						
		else	// returnCode == 2
			DupClassFieldNameAlert (1, m_classNamePtr); 
			
		selectedItem = IDC_ClassName;
		OKFlag = FALSE;
					
		}	// end "if (returnCode > 0)"
	
	if (OKFlag)
		{	
		strcpy ((char*)string, T2A (m_fieldNameCStringPtr));
		CtoPstring (string, m_fieldNamePtr);
								                              
				// Check if proposed field name is a duplicate.	 If it  	
				// is, allow user to change it again.							
		
		returnCode = CheckForDuplicateFieldName (-1, (Str255*)m_fieldNamePtr);						
		if (returnCode > 0)
			{                                     
			if (returnCode == 1)
				DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
						
			else    // returnCode == 2                                           
				DupClassFieldNameAlert (2, m_fieldNamePtr);
				  
			selectedItem = IDC_FieldName;
			OKFlag = FALSE;
								
			}	// end "if (returnCode > 0)"
			
		}	// end "if (OKFlag)"
	
	if (OKFlag)
		CMDialog::OnOK ();
		
	else	// !OKFlag
		SelectDialogItemText (this, selectedItem, 0, SInt16_MAX); 
		
}	// end "OnOK"
 
 
 
void CMNewClassFieldDlg::OnSelendokClassList (void)

{                   
	DDX_CBIndex (m_dialogFromPtr, IDC_ClassList, m_classList);
	
	NewClassFieldDialogChangeClass (this, 
												(SInt16)(m_classList+1), 
												m_fieldType+1,
												m_numberSelectionPixels);
	
}	// end "OnSelendokClassList"
