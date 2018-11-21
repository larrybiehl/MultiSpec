//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClassGroupDialog.cpp : class implementation file
//	Class Definition:		LClassGroupDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/12/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMClassGroupDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 
#include "LClassGroupDialog.h"
#include "CImageWindow.h"



CMClassGroupDlg::CMClassGroupDlg (
				wxWindow*							pParent /*=NULL*/) : CMOneColDlg (pParent) 

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
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
//	Called By:			Dialog in MDisMult.cpp
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
		Cell					cell;
		wxListBox*			listBoxPtr;

		UInt32				classIndex;

		SInt16				classNum, 
								index; 

		listBoxPtr = (wxListBox*)FindWindow (IDC_List1);

		if (m_classGroupCode == kClassDisplay)
			LoadThematicClassNamesIntoList (listBoxPtr);

		if (m_classGroupCode == kGroupDisplay)
			LoadThematicGroupNamesIntoList (listBoxPtr);
		/*
				// Move this section (to highlight the selected channel) to DoDialog
		
		cell.h = 0;
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
		*/

		}	// end "if (CMOneColDlg::TransferDataToWindow ())"

	this->Fit ();
	return true; 

}	// end "OnInitDialog"


