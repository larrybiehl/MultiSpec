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
//	File:						xClassGroupDialog.cpp : class implementation file
//	Class Definition:		xClassGroupDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/12/2018
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMClassGroupDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 
#include "xClassGroupDialog.h"
#include "SImageWindow_class.h"



CMClassGroupDlg::CMClassGroupDlg (
				wxWindow*							pParent /*=NULL*/)
		: CMOneColDlg (pParent)

{
			// This is a 1 base list.

	m_indexStart = 1;

	m_listType = kSelectItemsList;

	m_classGroupCode = 0;

	m_initializedFlag = CMOneColDlg::m_initializedFlag;

	OnInitDialog ();
	 
}	// end "CMClassGroupDlg"



CMClassGroupDlg::~CMClassGroupDlg () 

{

}	// end "~CMClassGroupDlg"



//------------------------------------------------------------------------------------
//                   Copyright 2009-2020 Purdue Research Foundation
//
//	Function name:		Boolean DoDialog
//
//	Software purpose:	The purpose of this routine is to present the display
//							specification dialog box to the user and copy the
//							revised back to the display specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			OnClassesGroupsComboSelendok in xDisplayThematicDialog.cpp
//
//	Coded By:			Abdur Rahman Maud		Date: ??/??/????
//	Revised By:			Larry L. Biehl			Date: 01/12/2018	

bool CMClassGroupDlg::DoDialog (
				UInt32*								numberOutputItemsPtr,
				SInt16*								listPtr,
				SInt16								minimumNumberItems,
				SInt16								numberInputItems,
				SInt16								classGroupCode) 

{
	Cell									cell;
	
	wxListBox*							listBoxPtr;
	
	UInt32								classIndex;
	
	SInt16								classNum, 
											index,
											returnCode;

	Boolean								OKFlag = false;


			// Make sure intialization has been completed.

	if (!m_initializedFlag)
																							return (false);

	if (classGroupCode != kClassDisplay && classGroupCode != kGroupDisplay)
																							return (false);

	m_numberOutputItems = (UInt16)*numberOutputItemsPtr;
	m_selectedItemsPtr = listPtr;
	m_numberInputVecItems = numberInputItems;
	m_minimumItemsRequired = minimumNumberItems;
	m_classGroupCode = classGroupCode;

	listBoxPtr = (wxListBox*)FindWindow (IDC_List1);
	index = 0;
	for (classIndex=0; classIndex<m_numberInputVecItems; classIndex++) 
		{
		cell.v = (SInt16)classIndex;
		classNum = (SInt16)(classIndex + 1);
		if (index < m_numberOutputItems && m_selectedItemsPtr[index] == classNum) 
			{
			LSetSelect (true, cell, listBoxPtr);
			index++;
			
			}	// end "if (index < *numberOutputItems && ...)" 
			
		}	// end "for (classIndex=0; classIndex<..." 

			// Set the number of selected items.										

	m_numberSelections = index;
	UpdateNumberOfSelections ();

			// Set default text selection to first edit text item 

	SelectDialogItemText (this, IDC_First, 0, SInt16_MAX);

	returnCode = ShowModal ();

	if (returnCode == wxID_OK) 
		{
				// Get items to use.	

		*numberOutputItemsPtr = m_numberSelections;

		OKFlag = true;

		}	// end "if (returnCode == IDOK)"

	return (OKFlag);

}	// end "DoDialog" 



bool CMClassGroupDlg::OnInitDialog () 

{
			//	Item 6: Load title of class list dialog.									
			//	Item 7: Load title for class list.
														
	m_classGroupCode++;
	if (m_classGroupCode == kClassDisplay) 
		{
		SetLabel (wxT("Select Spectral Classes"));
		LoadDItemString (IDC_ListTitle, (Str255*)"\0Class List:");

		}	// end "if (m_classGroupCode == kClassDisplay)" 

	if (m_classGroupCode == kGroupDisplay) 
		{
		SetLabel (wxT("Select Information Groups"));
		LoadDItemString (IDC_ListTitle, (Str255*)"\0Group List:");

		}	// end "if (m_classGroupCode == kGroupDisplay)" 

	if (CMOneColDlg::TransferDataToWindow ()) 
		{
		wxListBox*			listBoxPtr;

		listBoxPtr = (wxListBox*)FindWindow (IDC_List1);

		if (m_classGroupCode == kClassDisplay)
			LoadThematicClassNamesIntoList (listBoxPtr);

		if (m_classGroupCode == kGroupDisplay)
			LoadThematicGroupNamesIntoList (listBoxPtr);

		}	// end "if (CMOneColDlg::TransferDataToWindow ())"

	Fit ();
	return true; 

}	// end "OnInitDialog"


