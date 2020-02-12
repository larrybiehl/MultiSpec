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
//	File:						xEditClassGroupDialog.cpp : class implementation file
//	Class Definition:		xEditClassGroupDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/16/2018
//
//	Language:				C++
//
//	System:					Linux & MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMEditClassGroupDlg class.
//
// Following is template for debugging
/*
	int numberChars = sprintf ((char*)gTextString3,
									 " LEditClassGroupDlg:: (): %s",
									 gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "xEditClassGroupDialog.h"



BEGIN_EVENT_TABLE (CMEditClassGroupDlg, CMDialog)
	EVT_INIT_DIALOG (CMEditClassGroupDlg::OnInitDialog)
END_EVENT_TABLE ()



CMEditClassGroupDlg::CMEditClassGroupDlg (
				wxWindow* 							pParent /*=NULL*/)
		: CMDialog (CMEditClassGroupDlg::IDD, pParent, wxT(""))

{
	m_classGroupName = "Class 1";

   m_classGroupCStringPtr = NULL;
   m_nameHandle = NULL;
   m_classGroupIndex = 0;
   m_newEditCode = 0;
   m_numberClassesGroups = 0;
   m_noChangeFlag = TRUE;

			//	Set the class or group name string.	

	m_classGroupCStringPtr = m_clgrpnamebuf;

   m_initializedFlag = FALSE;

   CreateControls();
   m_initializedFlag = TRUE;

}	// end "CMEditClassGroupDlg"



void CMEditClassGroupDlg::CreateControls ()

{
	SetSizeHints (wxDefaultSize, wxDefaultSize);
	
   wxBoxSizer* bSizer134 = new wxBoxSizer (wxVERTICAL);
	
   wxBoxSizer* bSizer135 = new wxBoxSizer (wxHORIZONTAL);
	
   m_textCtrl71 = new wxTextCtrl (this,
												IDC_ClassGroupName,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (300, -1),
												0);
	bSizer135->Add (m_textCtrl71, wxSizerFlags(0).Expand());
	
   bSizer135->Add (50, 0, wxSizerFlags(1).Expand());
	
	bSizer134->Add (bSizer135,
							wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT|wxBOTTOM,12));

	CreateStandardButtons (bSizer134);
	
   SetSizer (bSizer134);
	Fit ();
   Layout ();
	
}	// end "CreateControls"



void CMEditClassGroupDlg::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	char 									textString[64];
   char* 								namePtr;

   Handle 								nameHandle;


   wxDialog::OnInitDialog (event);

			// Load the current default class or group name  

   if (m_newEditCode == kNewGroup) 
		{
      FileInfoPtr fileInfoPtr = (FileInfoPtr) GetHandlePointer (m_fileInfoHandle);
      nameHandle = fileInfoPtr->classDescriptionH;

		}	// end "if (m_newEditCode == kNewGroup)"

   else	// m_newEditCode != kNewGroup
		{
      nameHandle = m_nameHandle;

		}	// end "else m_newEditCode != kNewGroup"

   namePtr = (char*)GetHandlePointer (nameHandle, kLock);
   namePtr = &namePtr[m_classGroupIndex * 32];
   PtoCstring (namePtr, textString);

   if (m_newEditCode == kNewGroup) 
		{
      SetLabel (wxT("Enter New Thematic Group Name"));
      LoadDItemString (IDC_ClassGroupName, textString);
      CheckAndUnlockHandle (nameHandle);

		}	// end "if (m_newEditCode == kNewGroup)"

   else if (m_newEditCode == kEditGroup) 
		{
      SetLabel (wxT("Edit Thematic Group Name"));
      LoadDItemString (IDC_ClassGroupName, textString);

		}	// end "else if (m_newEditCode == kEditGroup)" 

   else // m_newEditCode == kEditClass
		{
      SetLabel (wxT("Edit Thematic Class Name"));
      LoadDItemString (IDC_ClassGroupName, textString);
 
		}	// end "else m_newEditCode == kEditGroup" 

   PositionDialogWindow ();

			// Set default text selection 

   SelectDialogItemText (this, IDC_ClassGroupName, 0, SInt16_MAX);

}	// end "OnInitDialog"



//------------------------------------------------------------------------------------
//                   Copyright 2009-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present a dialog
//							box to the user that can be used to edit the
//							class or group name.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			 
//
//	Coded By:			Abdur Maud				Date: ??/??/2015
//	Revised By:			Larry L. Biehl			Date: 06/07/2017	

Boolean CMEditClassGroupDlg::DoDialog (
				CMLegendList*						legendListCPtr,
				SInt16								selectedCell,
				SInt16								newEditCode,
				SInt16								classGroupIndex,
				Handle								fileInfoHandle,
				Handle								nameHandle)

{
	Str255								textString;

   SInt16								returnCode;

   Boolean								OKFlag = FALSE;


			// Make sure initialization has been completed.

   if (!m_initializedFlag)
																							return (FALSE);

   m_classGroupIndex = classGroupIndex;
   m_newEditCode = newEditCode;
   m_fileInfoHandle = fileInfoHandle;
   m_nameHandle = nameHandle;

   returnCode = ShowModal ();

   if (returnCode == wxID_OK) 
		{
      OKFlag = TRUE;
      strncpy (m_classGroupCStringPtr,
					(const char*)m_classGroupName.mb_str(wxConvUTF8),
					1023);
		
				// Force to be no more than 31 characters in length
		
		m_classGroupCStringPtr[31] = 0;
		
      CtoPstring ((UCharPtr)m_classGroupCStringPtr, textString);

      EditGroupClassDialogOK (legendListCPtr,
										  m_newEditCode,
										  m_nameHandle,
										  m_fileInfoHandle,
										  (char*)&textString,
										  selectedCell,
										  m_numberClassesGroups,
										  m_classGroupIndex,
										  m_noChangeFlag);

		}	// end "if (returnCode == IDOK)" 

	return (OKFlag);

}	// end "DoDialog"



Boolean CMEditClassGroupDlg::OnOK () 
{
	Str255								textString;
   Boolean								OKFlag = TRUE;


	strncpy (m_classGroupCStringPtr,
				(const char*)m_classGroupName.mb_str(wxConvUTF8),
				1023);
   CtoPstring ((UCharPtr)m_classGroupCStringPtr, textString);
   m_numberClassesGroups = CheckForDuplicateName (m_newEditCode,
																  m_nameHandle,
																  m_fileInfoHandle,
																  textString,
																  &m_classGroupIndex,
																  &m_noChangeFlag);
																  
   if (m_numberClassesGroups == 0) 
		{
      SInt16 classGroupCode = 1;
      if (m_newEditCode != kEditClass)
         classGroupCode = 3;

      DupClassFieldNameAlert (classGroupCode, textString);
      OKFlag = FALSE;

		}	// end "if (m_numberClassesGroups == 0)"  

	if (!OKFlag)
		SelectDialogItemText (this, IDC_ClassGroupName, 0, SInt16_MAX);
		
	return (OKFlag);

}	// end "OnOK"



bool CMEditClassGroupDlg::TransferDataFromWindow ()

{
	Boolean				returnFlag;
	
	
   wxTextCtrl* tctrl = (wxTextCtrl*)FindWindow (IDC_ClassGroupName);
   m_classGroupName = tctrl->GetValue ();
	
	returnFlag = OnOK ();

	return returnFlag;
	
}	// end "TransferDataFromWindow"



bool CMEditClassGroupDlg::TransferDataToWindow ()

{
   wxTextCtrl* tctrl = (wxTextCtrl*)FindWindow (IDC_ClassGroupName);
   tctrl->Clear ();
   tctrl->SetValue (m_classGroupName);
   
   return true;
	
}	// end "TransferDataToWindow"
