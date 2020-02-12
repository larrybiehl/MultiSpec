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
//	File:						WClassesDialog.cpp : implementation file
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
//								CMClassesDlg class.
//
//------------------------------------------------------------------------------------
                   
#include "SMultiSpec.h"
#include "SImageWindow_class.h"

#include "WClassesDialog.h"  
#include "WOneColumnDialog.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMClassesDlg, CMOneColDlg)
	//{{AFX_MSG_MAP (CMClassesDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMClassesDlg::CMClassesDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMOneColDlg (pParent)
		
{
	//{{AFX_DATA_INIT(CMClassesDlg)
	//}}AFX_DATA_INIT  
	 
	m_listType = kSelectItemsList;          
	                        
	m_initializedFlag = CMOneColDlg::m_initializedFlag;
	
}	// end "CMClassesDlg"



void CMClassesDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	CMOneColDlg::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMClassesDlg) 
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



Boolean CMClassesDlg::DoDialog (
				UInt32*								numberOutputClassesPtr,
				SInt16* 								classListPtr,
				SInt16								minimumNumberClasses,
				SInt16								numberInputClasses)

{  
	INT_PTR								returnCode;
	
	Boolean								OKFlag = FALSE;
	
	
	
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																					return (FALSE);
	
			// This is a 1 base list.
			
	m_indexStart = 1; 
	
	m_numberOutputItems = (SInt16)*numberOutputClassesPtr;     
	m_selectedItemsPtr = classListPtr;         
	m_numberInputVecItems = numberInputClasses;
	m_minimumItemsRequired = minimumNumberClasses;                     
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{                                                  
				// Get items to use.	
				      
		*numberOutputClassesPtr = m_numberSelections; 
			
		OKFlag = TRUE; 
		
		}	// end "if (returnCode == IDOK)"
		
	return (OKFlag);
		
}	// end "DoDialog"



//-----------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		BOOL CMClassesDlg::OnInitDialog
//
//	Software purpose:	The purpose of this routine is to initialize the class 
//							dialog box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	TRUE, if focus was not set
//							FALSE, if focus was set 		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: ??/??/1995?
//	Revised By:			Larry L. Biehl			Date: 04/30/1998

BOOL CMClassesDlg::OnInitDialog ()

{   								
	CMOneColDlg::OnInitDialog ();  
	
	SetWindowText ((LPCTSTR)_T("Select Classes"));
	
	LoadDItemString (IDC_ListTitle, (Str255*)"\0Class List:");  

	if (UpdateData (FALSE))
		{ 
		PositionDialogWindow (); 
		
		CListBox* 				listBoxPtr;
		
									
									
		listBoxPtr = (CListBox*)GetDlgItem (IDC_List1);
		
		LoadClassNamesIntoList (listBoxPtr);  

		m_numberSelections = (UInt16)SetClassListSelections (
																		listBoxPtr,
																		(SInt32)m_numberInputVecItems,
																		(SInt32)m_numberOutputItems,
																		(UInt16*)m_selectedItemsPtr); 
																		
				// Set the number of selected items.										
					
		UpdateNumberOfSelections (); 
		
				// Set default text selection to first edit text item 
			                                       
		SelectDialogItemText (this, IDC_First, 0, SInt16_MAX);      
		
		return FALSE;  // return TRUE  unless you set the focus to a control
		
		}	// end "if (UpdateData (FALSE))"
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"
