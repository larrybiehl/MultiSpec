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
//	File:						xNewClassFieldDialog.cpp : class implementation file
//	Class Definition:		xNewClassFieldDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			11/21/2019
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMNewClassFieldDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include	"xNewClassFieldDialog.h"



BEGIN_EVENT_TABLE (CMNewClassFieldDlg, CMDialog)
	EVT_CHOICE (IDC_ClassList, CMNewClassFieldDlg::OnSelendokClassList)

	EVT_INIT_DIALOG (CMNewClassFieldDlg::OnInitDialog)

	EVT_RADIOBUTTON (IDC_TestField, CMNewClassFieldDlg::OnFieldType)
	EVT_RADIOBUTTON (IDC_Training, CMNewClassFieldDlg::OnFieldType)

	EVT_TEXT_MAXLEN (IDC_ClassName, CMNewClassFieldDlg::CheckLength)
	EVT_TEXT_MAXLEN (IDC_FieldName, CMNewClassFieldDlg::CheckLength)
END_EVENT_TABLE ()



CMNewClassFieldDlg::CMNewClassFieldDlg (
				wxWindow* 							parent,
				wxWindowID 							id,
				const wxString& 					title)
		: CMDialog (CMNewClassFieldDlg::IDD, parent, title)

{
	m_classList = 0;
	m_className = wxT("Class 1");
	m_fieldName = wxT("Field 1");
	m_fieldType = 0;
	
	m_classNameCStringPtr = NULL;
	m_fieldNameCStringPtr = NULL;  
	
	try
		{
		m_classNameCStringPtr = m_classNamebuf;
		m_fieldNameCStringPtr = m_fieldNamebuf;			
		}
	
	catch(int e)
		{
		m_initializedFlag = FALSE;			
		}
	
	m_initializedFlag = TRUE;
   CreateControls ();
   
}	// end "CMNewClassFieldDlg"



void CMNewClassFieldDlg::CreateControls (void)

{
   SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	bSizer285 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer286;
	bSizer286 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText265 = new wxStaticText (this,
													IDC_ClassListPrompt,
													wxT("Class:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText265->Wrap (-1);
   SetUpToolTip (m_staticText265, IDS_ToolTip301);
	bSizer286->Add (m_staticText265, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_classListCtrl = new wxChoice (this,
												IDC_ClassList,
												//wxT("Combo!"),
												wxDefaultPosition,
												wxSize (200, -1));
												//0,
												//NULL,
												//wxCB_READONLY);
   SetUpToolTip (m_classListCtrl, IDS_ToolTip301);
	bSizer286->Add (m_classListCtrl, 0, wxALL, 5);
	
	bSizer285->Add (bSizer286, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12);
	
	wxBoxSizer* bSizer287;
	bSizer287 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText266 = new wxStaticText (this,
													IDC_ClassNamePrompt,
													wxT("Enter Class Name:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText266->Wrap (-1);
   SetUpToolTip (m_staticText266, IDS_ToolTip302);
	bSizer287->Add (m_staticText266, 0, wxALL, 5);
	
	m_textCtrl102 = new wxTextCtrl (this,
												IDC_ClassName,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (250, -1),
												0);
   m_textCtrl102->SetMaxLength (31);
   SetUpToolTip (m_textCtrl102, IDS_ToolTip302);
	bSizer287->Add (m_textCtrl102, 0, wxALL, 5);
	
	m_staticText267 = new wxStaticText (this,
													IDC_ClassNumberPixels,
													wxT("Number train pixels in class: 1000000"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText267->Wrap (-1);
   SetUpToolTip (m_staticText267, IDS_ToolTip303);
	bSizer287->Add (m_staticText267, 0, wxLEFT, 25);
	
	bSizer285->Add (bSizer287, 0, wxEXPAND|wxLEFT|wxRIGHT, 12);
	
	wxBoxSizer* bSizer288;
	bSizer288 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText268 = new wxStaticText (this,
													IDC_FieldNamePrompt,
													wxT("Enter Field Identifier:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText268->Wrap (-1);
   SetUpToolTip (m_staticText268, IDS_ToolTip304);
	bSizer288->Add (m_staticText268, 0, wxALL, 5);
	
	m_textCtrl103 = new wxTextCtrl (this,
												IDC_FieldName,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (250, -1),
												0);
   m_textCtrl103->SetMaxLength (31);
   SetUpToolTip (m_textCtrl103, IDS_ToolTip304);
	bSizer288->Add (m_textCtrl103, 0, wxALL, 5);
	
	m_staticText269 = new wxStaticText (this,
													IDC_FieldNumberPixels,
													wxT("Number pixels: 1000000"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText269->Wrap (-1);
   SetUpToolTip (m_staticText269, IDS_ToolTip305);
	bSizer288->Add (m_staticText269, 0, wxLEFT, 25);
	
	bSizer285->Add (bSizer288, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12);
	
	wxBoxSizer* bSizer289;
	bSizer289 = new wxBoxSizer (wxHORIZONTAL);
	
	wxBoxSizer* bSizer292;
	bSizer292 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText270 = new wxStaticText (this,
													IDC_AreaTypeOutline,
													wxT("AreaType:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText270->Wrap (-1);
   SetUpToolTip (m_staticText270, IDS_ToolTip306);
	bSizer292->Add (m_staticText270, 0, wxALL, 5);
	
	m_radioBtn27 = new wxRadioButton (this,
													IDC_Training,
													wxT("Training Field"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_radioBtn27, IDS_ToolTip306);
	bSizer292->Add (m_radioBtn27, 0, wxALL, 5);
	
	m_radioBtn28 = new wxRadioButton (this,
													IDC_TestField,
													wxT("Test Field"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_radioBtn28, IDS_ToolTip306);
	bSizer292->Add (m_radioBtn28, 0, wxALL, 5);
	
	bSizer289->Add (bSizer292, 0, wxEXPAND);

	bSizer285->Add (bSizer289, 0, wxLEFT|wxTOP|wxRIGHT, 12);
	
	CreateStandardButtons (bSizer285);
	
	Layout ();
	Centre (wxBOTH);
   SetSizerAndFit (bSizer285);
	
}	// end "CreateControls"



Boolean CMNewClassFieldDlg::DoDialog (
				Boolean								newClassOnlyFlag, 
				char*									classNamePtr,
				char*									fieldNamePtr, 
				SInt16*								fieldTypePtr)

{
	SInt16								returnCode;
	Boolean								OKFlag = FALSE;
	
	
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
																			
	m_newClassOnlyFlag = newClassOnlyFlag; 
	m_classNamePtr = classNamePtr;
	m_fieldNamePtr = fieldNamePtr;
	m_fieldTypePtr = fieldTypePtr;
	
   m_classNameCStringPtr = m_classNamebuf;
	
   m_fieldNameCStringPtr = m_fieldNamebuf;

	returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{ 
		OKFlag = TRUE;

				// Training field or test field
														          
		if (m_fieldType == 0)
			*fieldTypePtr = kTrainingType;
							
		if (m_fieldType == 1)
			*fieldTypePtr = kTestingType;
			
		}	// end "if (returnCode == IDOK)"
		
	return (OKFlag);
		
}	// end "DoDialog"



void CMNewClassFieldDlg::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	UInt16								selectedItem;
	
	
			// Add current list of classes to the combo box.
			
	CreateClassNameComboBoxList (m_classListCtrl);
	   
	   	// Set the specified class.
	   	
	m_classList = gProjectInfoPtr->lastClassForAddFieldSelection - 1;
   m_classListCtrl->SetSelection (m_classList);
	
			// Initialize the dialog box parameters that are common between the
			// Mac and Windows versions.
	
	NewClassFieldDialogInitialize (this, 
												m_newClassOnlyFlag,
												&selectedItem,
												(UCharPtr)m_fieldNamePtr,
												*m_fieldTypePtr,
												&m_numberSelectionPixels); 
   PositionDialogWindow ();  
		
			// Set default text selection 
	                                                 
	SelectDialogItemText (this, selectedItem, 0, SInt16_MAX);
	
}	// end "OnInitDialog"


 
void CMNewClassFieldDlg::OnSelendokClassList (
				wxCommandEvent& 					event)

{                   
	wxChoice* classlist = (wxChoice*)FindWindow (IDC_ClassList);
   m_classList = classlist->GetSelection ();
   
	NewClassFieldDialogChangeClass (this, 
												(SInt16)(m_classList+1), 
												m_fieldType+1,
												m_numberSelectionPixels);
	
}	// end "OnSelendokClassList"



void CMNewClassFieldDlg::OnFieldType (
				wxCommandEvent& 					event)

{                    
	wxRadioButton* training = (wxRadioButton*)FindWindow (IDC_Training);
   if(training->GetValue () == true)
      m_fieldType = 0;
   else
      m_fieldType = 1;
   
	SetNumberClassTrainPixels (this, m_numberSelectionPixels, m_fieldType+1);
	
}	// end "OnFieldType"



void CMNewClassFieldDlg::CheckLength (
				wxCommandEvent& 					event)

{
   DisplayAlert (kErrorAlertID,
						kStopAlert,
						kAlertStrID,
						IDS_Alert116,
						0,
						NULL);
	
}	// end "CheckLength"



bool CMNewClassFieldDlg::TransferDataFromWindow ()

{
	SInt16								returnCode,
											selectedItem;
	
	Boolean								OKFlag;
	
	
   m_classList = m_classListCtrl->GetSelection ();
	
   wxTextCtrl* classname = (wxTextCtrl*)FindWindow (IDC_ClassName);
   m_className = classname->GetValue ();
	
   wxTextCtrl* fieldname = (wxTextCtrl*)FindWindow (IDC_FieldName);
   m_fieldName = fieldname->GetValue ();
	
   strcpy (m_classNamebuf, (const char*)m_className.mb_str (wxConvUTF8));
   strcpy (m_fieldNamebuf, (const char*)m_fieldName.mb_str (wxConvUTF8));
	
			// Verify that class name is not a duplicate
	
	OKFlag = TRUE;
	selectedItem = IDC_ClassName;
	CtoPstring ((UCharPtr)m_classNameCStringPtr, (UCharPtr)m_classNamePtr, 30);
	if (m_classList == 0)
		{
				// I.e. This is a new class.
		
		returnCode = CheckForDuplicateClassName (-1, (Str255*)m_classNamePtr);
		if (returnCode != 0)
			{
			if (returnCode < 0)
				DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
			
			else	// returnCode > 0
				DupClassFieldNameAlert (1, (UCharPtr)m_classNamePtr);
			
			OKFlag = FALSE;
			
			}	// end "if (returnCode != 0)"
		
		}	// end "if (m_classList == 0)"

	if (OKFlag)
		{
		CtoPstring ((UCharPtr)m_fieldNameCStringPtr, (UCharPtr)m_fieldNamePtr, 30);
		
				// Check if proposed field name is a duplicate.	 If it
				// is, allow user to change it again.
		
		returnCode = CheckForDuplicateFieldName (-1, (Str255*)m_fieldNamePtr);
		if (returnCode > 0)
			{
			if (returnCode == 1)
				DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_Alert44, 0, NULL);
			
			else	// returnCode == 2
				DupClassFieldNameAlert (2, (UCharPtr)m_fieldNamePtr);
			
			selectedItem = IDC_FieldName;
			OKFlag = FALSE;
			
			}	// end "if (returnCode > 0)"
		
		}	// end "if (OKFlag)"

	if (!OKFlag)
		SelectDialogItemText (this, selectedItem, 0, SInt16_MAX);
	
   wxRadioButton* training = (wxRadioButton*)FindWindow (IDC_Training);
   if (training->GetValue ())
      m_fieldType = 0;
	
   else
      m_fieldType = 1;
	
   return OKFlag;
	
}	// end "TransferDataFromWindow"



bool CMNewClassFieldDlg::TransferDataToWindow ()

{
   m_classListCtrl->SetSelection(m_classList);
	
   wxTextCtrl* classname = (wxTextCtrl*)FindWindow (IDC_ClassName);
   classname->SetValue(m_className);
	
   wxTextCtrl* fieldname = (wxTextCtrl*)FindWindow (IDC_FieldName);
   fieldname->SetValue(m_fieldName);
	
   wxRadioButton* training = (wxRadioButton*)FindWindow (IDC_Training);
   wxRadioButton* testfield = (wxRadioButton*)FindWindow (IDC_TestField);
   if (m_fieldType == 0)
      training->SetValue (true);
   else
      testfield->SetValue (true);
	
   return true;
	
}	// end "TransferDataToWindow"
