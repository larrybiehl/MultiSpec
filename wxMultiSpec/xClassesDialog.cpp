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
//	File:						xClassesDialog.cpp : class implementation file
//	Class Definition:		xClassesDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/13/2019
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the CMClassesDlg
//								class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 

#include "SImageWindow_class.h"

#include "xClassesDialog.h"  
#include "xOneColumnDialog.h"



BEGIN_EVENT_TABLE (CMClassesDlg, CMOneColDlg)
	EVT_INIT_DIALOG (CMClassesDlg::OnInitDialog)
END_EVENT_TABLE ()



CMClassesDlg::CMClassesDlg (
				wxWindow*							pParent /*=NULL*/)
		: CMOneColDlg (pParent)

{
   m_listType = kSelectItemsList;
   m_initializedFlag = CMOneColDlg::m_initializedFlag;

}	// end "CMClassesDlg"



//------------------------------------------------------------------------------------
//                   Copyright 2009-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the classes
//							specification dialog box to the user and copy the
//							revised back to the classes specification structure if
//							the user selected OK.
//
//	Parameters in:		
//
//	Parameters out:	
//
//	Value Returned:			
// 
//	Called By:			
//
//	Coded By:			Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:			Larry L. Biehl			Date: 01/12/2018	

Boolean CMClassesDlg::DoDialog(
				UInt32*								numberOutputClassesPtr,
				SInt16*								classListPtr,
				SInt16								minimumNumberClasses,
				SInt16								numberInputClasses)

{
   SInt16								returnCode;
   Boolean								OKFlag = false;
	
	
			// Make sure intialization has been completed.		
								                         
   if (!m_initializedFlag)
																							return (FALSE);

			// This is a 1 base list.

   m_indexStart = 1;

   m_numberOutputItems = (SInt16) * numberOutputClassesPtr;
   m_selectedItemsPtr = classListPtr;
   m_numberInputVecItems = numberInputClasses;
   m_minimumItemsRequired = minimumNumberClasses;

   returnCode = ShowModal();

   if (returnCode == wxID_OK) 
		{
				// Get items to use.		
							      
      *numberOutputClassesPtr = m_numberSelections;
      OKFlag = true;
		
		}	// end "if (returnCode == IDOK)"

	return (OKFlag);

}	// end "DoDialog" 



//------------------------------------------------------------------------------------
//                   Copyright 2009-2020 Purdue Research Foundation
//
//	Function name:		OnInitDialog
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
//	Coded By:			Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:			Larry L. Biehl			Date: 01/12/2018	

void CMClassesDlg::OnInitDialog (
				wxInitDialogEvent&				event)

{
   CMOneColDlg::OnInitDialog ();

   SetLabel (wxT("Select Classes"));
   LoadDItemString (IDC_ListTitle, (Str255*)"\0Class List:");

   if (TransferDataToWindow ()) 
		{
      PositionDialogWindow ();
      wxListBox* listBoxPtr;
      listBoxPtr = (wxListBox*)FindWindow (IDC_List1);
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

		}	// end "if (TransferDataToWindow ())"	

}	// end "OnInitDialog"
