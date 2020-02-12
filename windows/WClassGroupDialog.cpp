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
//	File:						WClassGroupDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/21/2017
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMBiPlotDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SImageWindow_class.h"

#include "WClassGroupDialog.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMClassGroupDlg, CMOneColDlg)
	//{{AFX_MSG_MAP (CMClassGroupDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMClassGroupDlg::CMClassGroupDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMOneColDlg (pParent)
		
{
	//{{AFX_DATA_INIT(CMClassGroupDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT 

			// This is a 1 base list.
						
	m_indexStart = 1; 
	
	m_listType = kSelectItemsList;
	
	m_classGroupCode = 0; 
	                        
	m_initializedFlag = CMOneColDlg::m_initializedFlag;
	
}	// end "CMClassGroupDlg"



void CMClassGroupDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	CMOneColDlg::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMClassGroupDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



Boolean CMClassGroupDlg::DoDialog (
				UInt32*								numberOutputItemsPtr,
				SInt16* 								listPtr,
				SInt16								minimumNumberItems,
				SInt16								numberInputItems,
				SInt16								classGroupCode)

{  
	INT_PTR								returnCode;
	 
	Boolean								OKFlag = FALSE;
	

			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																					return (FALSE);
																			
	if (classGroupCode != kClassDisplay &&
									classGroupCode != kGroupDisplay)
																					return (FALSE);

	m_numberOutputItems = (UInt16)*numberOutputItemsPtr;     
	m_selectedItemsPtr = listPtr;         
	m_numberInputVecItems = numberInputItems;
	m_minimumItemsRequired = minimumNumberItems;
	m_classGroupCode = classGroupCode;                     
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{                                                  
				// Get items to use.	
				      
		*numberOutputItemsPtr = m_numberSelections; 
			
		OKFlag = TRUE; 
		
		}	// end "if (returnCode == IDOK)"
		
	return (OKFlag);
		
}	// end "DoDialog"



BOOL CMClassGroupDlg::OnInitDialog ()

{
	CMOneColDlg::OnInitDialog (); 
	
			//	Item 6: Load title of class list dialog.									
			//	Item 7: Load title for class list.											
	
	if (m_classGroupCode == kClassDisplay)
		{                                                                    
		SetWindowText ((LPCTSTR)_T("Select Spectral Classes"));
		LoadDItemString (IDC_ListTitle, (Str255*)"\0Class List:");
		
		}	// end "if (m_classGroupCode == kClassDisplay)" 
		
	if (m_classGroupCode == kGroupDisplay)
		{
		SetWindowText ((LPCTSTR)_T("Select Information Groups"));
		LoadDItemString (IDC_ListTitle, (Str255*)"\0Group List:");
		
		}	// end "if (m_classGroupCode == kGroupDisplay)"

	if (UpdateData (FALSE))
		{ 
		PositionDialogWindow ();  
		
		Cell						cell;
		CListBox* 				listBoxPtr;

		UInt32					classIndex;
		
		SInt16					classNum,
									index;    
									
									
		listBoxPtr = (CListBox*)GetDlgItem (IDC_List1);
		
		if (m_classGroupCode == kClassDisplay)
			LoadThematicClassNamesIntoList (listBoxPtr);
		
		if (m_classGroupCode == kGroupDisplay)
			LoadThematicGroupNamesIntoList (listBoxPtr);
			
		cell.h = 0;
		index = 0;                                               
		for (classIndex=0; classIndex<m_numberInputVecItems; classIndex++)
			{
			cell.v = (SInt16)classIndex;
			classNum = (SInt16)(classIndex + 1);
			if (index < m_numberOutputItems && 
											m_selectedItemsPtr[index] == classNum)
				{
				LSetSelect (TRUE, cell, listBoxPtr);
				index++;
						
				}	// end "if (index < *numberOutputItems && ...)"
					
			}	// end "for (classIndex=0; classIndex<..."
					
				// Set the number of selected items.										
					
		m_numberSelections = index;
		UpdateNumberOfSelections (); 
		
				// Set default text selection to first edit text item 
			                                       
		SelectDialogItemText (this, IDC_First, 0, SInt16_MAX);      
		
		return FALSE;  // return TRUE  unless you set the focus to a control
		
		}	// end "if (UpdateData (FALSE))"
	
	return TRUE;  // return TRUE  unless you set the focus to a control  
	
}	// end "OnInitDialog"
