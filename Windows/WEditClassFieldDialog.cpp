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
//	File:						WEditClassFieldDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			07/13/2018
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMEditClassFieldDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
 
#include "WEditClassFieldDialog.h"
#include "WImageView.h"   
#include "WStatisticsView.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMEditClassFieldDlg, CMDialog)
	//{{AFX_MSG_MAP (CMEditClassFieldDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMEditClassFieldDlg::CMEditClassFieldDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog	(CMEditClassFieldDlg::IDD, pParent)

{
	//{{AFX_DATA_INIT(CMEditClassFieldDlg)
	m_className = "Class 1";
	m_fieldName = "Field 1";
	m_fieldType = 0;
	//}}AFX_DATA_INIT 
	
	m_classNameCStringPtr = NULL;
	m_fieldNameCStringPtr = NULL;  
	
	TRY
		{																			
				//	Get pointers to the string buffers.	
			
		m_classNameCStringPtr = (TBYTE*)m_className.GetBufferSetLength (64);
		m_fieldNameCStringPtr = (TBYTE*)m_fieldName.GetBufferSetLength (64);
				
		}
			
	CATCH_ALL (e)
		{
		m_initializedFlag = FALSE;
		}
	
	END_CATCH_ALL
	
	m_initializedFlag = TRUE;
	
}	// end "CMEditClassFieldDlg"



void CMEditClassFieldDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	CMDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMEditClassFieldDlg)        
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
//	Software purpose:	The purpose of this routine is to present the edit class/fields
//							dialog box to the user so that the user make changes in the
//							class or field name and/or train/test field designation.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/19/1996
//	Revised By:			Larry L. Biehl			Date: 02/19/1996

Boolean CMEditClassFieldDlg::DoDialog (
				SInt16								classFieldCode, 
				SInt16								currentClass,
				SInt16								currentField,
				char*									titleStringPtr)

{    
	USES_CONVERSION;

	UInt8									string[256];
	
	INT_PTR								returnCode;

	Boolean								OKFlag = FALSE;
	
	
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
																			
	m_classFieldCode = classFieldCode;
	m_currentField = currentField;
	
	m_classStorage = gProjectInfoPtr->storageClass[currentClass];
	
	m_titleStringPtr = titleStringPtr;
	            																			
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{ 
		OKFlag = TRUE;   
		
		if (classFieldCode == 2)
			{ 
					// Class Name
					                  
			strcpy ((char*)string, T2A (m_classNameCStringPtr)); 
			CtoPstring (string, 
							gProjectInfoPtr->classNamesPtr[m_classStorage].name);
			
			CComboBox*	classListComboBoxPtr = 
										(CComboBox*)gProjectWindow->GetDlgItem (IDC_ClassList);
			classListComboBoxPtr->DeleteString (currentClass+1);
			classListComboBoxPtr->InsertString (currentClass+1,
															m_classNameCStringPtr);
						
			}	// end "if (classFieldCode == 2)"
					             
		if (classFieldCode == 3)
			{                      
			HPFieldIdentifiersPtr fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
			
					// Field Identifier
					  
			strcpy ((char*)string, T2A (m_fieldNameCStringPtr)); 
			CtoPstring (string, gProjectInfoPtr->fieldIdentPtr[m_currentField].name);
					
					// Training field or test field						
					                
			SInt16 fieldType = kTrainingType; 
			if (m_fieldType == 1)
				fieldType = kTestingType;
				
			fieldIdentPtr[currentField].fieldType = fieldType;
				
			}	// end "if (classFieldCode == 3)"
		
		}	// end "if (returnCode == IDOK)"
		
	return (OKFlag);
		
}	// end "DoDialog"



BOOL CMEditClassFieldDlg::OnInitDialog (void)

{                                         
	USES_CONVERSION;

	UInt16								selectedItem;
	
	
	CDialog::OnInitDialog ();

	EditClassFieldDialogInitialize (this, 
												m_classFieldCode,
												m_classStorage,
												m_currentField,
												&selectedItem);
												
	SetWindowText (A2T((LPCSTR)m_titleStringPtr)); 
	
	PositionDialogWindow ();  
		
			// Set default text selection 
	                                     
	SelectDialogItemText (this, selectedItem, 0, SInt16_MAX);  			
	
	return FALSE;  // return TRUE  unless you set the focus to a control 
	
}	// end "OnInitDialog"



void CMEditClassFieldDlg::OnOK (void)

{  
	USES_CONVERSION;

	UInt8									string[256];
	
	SInt16								returnCode;
	UInt16								selectedItem;
	
	Boolean 								OKFlag = TRUE;


	UpdateData (TRUE);
	
	if (m_classFieldCode == 2)
		{              
		strcpy ((char*)string, T2A (m_classNameCStringPtr));
		CtoPstring (string, gTextString);
						
				// Check if proposed class name is a duplicate.	 If it is, 	
				// allow user to change it again.									
							                                                         
		returnCode = CheckForDuplicateClassName (m_classStorage, (Str255*)gTextString);
		if (returnCode > 0)
			{ 			 
			if (returnCode == 1)
				DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
						
			else	// returnCode == 2
				DupClassFieldNameAlert (1, (UCharPtr)gTextString);
							                                   
			selectedItem = IDC_ClassName;
			OKFlag = FALSE;
			
			}	// end "if (returnCode > 0)"
			
		}	// end "if (m_classFieldCode == 2)"
	
	if (OKFlag && m_classFieldCode == 3)
		{	    
		strcpy ((char*)string, T2A (m_fieldNameCStringPtr));
		CtoPstring (string, gTextString);
								                              
				// Check if proposed field name is a duplicate.	 If it  	
				// is, allow user to change it again.							
								                                
		returnCode = CheckForDuplicateFieldName (m_currentField, (Str255*)gTextString);
		if (returnCode > 0)
			{                                        
			if (returnCode == 1)
				DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
						
			else    // returnCode == 2
				DupClassFieldNameAlert (2, gTextString);
				     
			selectedItem = IDC_FieldName;
			OKFlag = FALSE;
			
			}	// end "if (returnCode > 0)"
			
		}	// end "if (OKFlag && ...)"
	
	if (OKFlag)
		CMDialog::OnOK ();
		
	else	// !OKFlag
		SelectDialogItemText (this, selectedItem, 0, SInt16_MAX); 
		
}	// end "OnOK"
