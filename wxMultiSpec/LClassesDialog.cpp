//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClassesDialog.cpp : class implementation file
//	Class Definition:		LClassesDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/12/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the CMClassesDlg
//								class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 

#include "CImageWindow.h"

#include "LClassesDialog.h"  
#include "LOneColumnDialog.h"



CMClassesDlg::CMClassesDlg (
				wxWindow*							pParent /*=NULL*/) : CMOneColDlg (pParent) 

{
   m_listType = kSelectItemsList;
   m_initializedFlag = CMOneColDlg::m_initializedFlag;

}	// end "CMClassesDlg"



BEGIN_EVENT_TABLE (CMClassesDlg, CMOneColDlg)
	EVT_INIT_DIALOG (CMClassesDlg::OnInitDialog)
END_EVENT_TABLE ()



//------------------------------------------------------------------------------------
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
