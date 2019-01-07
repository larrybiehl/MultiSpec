//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LNewClassFieldDialog.cpp : class implementation file
//	Class Definition:		LNewClassFieldDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			11/13/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMNewClassFieldDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include	"LNewClassFieldDialog.h"   


extern void 		NewClassFieldDialogChangeClass (
							DialogPtr							dialogPtr, 
							SInt16								newCurrentClass,
							SInt16								fieldType,
							SInt64								numberSelectionPixels);

extern void 		NewClassFieldDialogInitialize (
							DialogPtr							dialogPtr, 
							Boolean								newClassOnlyFlag,
							UInt16*								selectedItemPtr,
							UCharPtr								fieldNamePtr,
							SInt16								fieldType,
							SInt64*								numberSelectionPixelsPtr);



CMNewClassFieldDlg::CMNewClassFieldDlg(wxWindow* parent, wxWindowID id, const wxString& title)
	: CMDialog(CMNewClassFieldDlg::IDD, parent, title)
{
	//{{AFX_DATA_INIT(CMNewClassFieldDlg)
	m_classList = 0;
	m_className = wxT("Class 1");
	m_fieldName = wxT("Field 1");
	m_fieldType = 0;
	//}}AFX_DATA_INIT 
	
	m_classNameCStringPtr = NULL;
	m_fieldNameCStringPtr = NULL;  
	
	try{																			
				//	Set the algebraic transform descriptions.				
//		m_classNameCStringPtr = m_className.GetBufferSetLength(64);
//		m_fieldNameCStringPtr = m_fieldName.GetBufferSetLength(64);		
		m_classNameCStringPtr = m_classNamebuf;
		m_fieldNameCStringPtr = m_fieldNamebuf;			
	}			
	catch(int e){
		m_initializedFlag = FALSE;			
	}	
	
	m_initializedFlag = TRUE;
   CreateControls();
   SetSizerAndFit(bSizer285);
   
}



BEGIN_EVENT_TABLE(CMNewClassFieldDlg, CMDialog)
EVT_INIT_DIALOG(CMNewClassFieldDlg::OnInitDialog)
EVT_COMBOBOX(IDC_ClassList, CMNewClassFieldDlg::OnSelendokClassList)
EVT_RADIOBUTTON(IDC_TestField, CMNewClassFieldDlg::OnFieldType)
EVT_RADIOBUTTON(IDC_Training, CMNewClassFieldDlg::OnFieldType)
//EVT_TEXT(IDC_ClassName, CMNewClassFieldDlg::OnEditClassName)
//EVT_TEXT(IDC_FieldName, CMNewClassFieldDlg::OnEditFieldName)
EVT_TEXT_MAXLEN(IDC_ClassName, CMNewClassFieldDlg::CheckLength)
EVT_TEXT_MAXLEN(IDC_FieldName, CMNewClassFieldDlg::CheckLength)
EVT_CHAR_HOOK(CMNewClassFieldDlg::OnCharHook)
END_EVENT_TABLE()



void CMNewClassFieldDlg::CreateControls ()

{
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
//	wxBoxSizer* bSizer285;
	bSizer285 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer286;
	bSizer286 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText265 = new wxStaticText( this, IDC_ClassListPrompt, wxT("Class:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText265->Wrap( -1 );
   SetUpToolTip(m_staticText265, IDS_ToolTip301);
	bSizer286->Add( m_staticText265, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_comboBox56 = new wxComboBox( this, IDC_ClassList, wxT("Combo!"), wxDefaultPosition, wxSize( 200,-1 ), 0, NULL, 0 );
   SetUpToolTip(m_comboBox56, IDS_ToolTip301);
	bSizer286->Add( m_comboBox56, 0, wxALL, 5 );
	
	bSizer285->Add( bSizer286, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );
	
	wxBoxSizer* bSizer287;
	bSizer287 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText266 = new wxStaticText( this, IDC_ClassNamePrompt, wxT("Enter Class Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText266->Wrap( -1 );
   SetUpToolTip(m_staticText266, IDS_ToolTip302);
	bSizer287->Add( m_staticText266, 0, wxALL, 5 );
	
	m_textCtrl102 = new wxTextCtrl( this, IDC_ClassName, wxEmptyString, wxDefaultPosition, wxSize( 250,-1 ), 0 );
   m_textCtrl102->SetMaxLength(31);
   SetUpToolTip(m_textCtrl102, IDS_ToolTip302);
	bSizer287->Add( m_textCtrl102, 0, wxALL, 5 );
	
	m_staticText267 = new wxStaticText( this, IDC_ClassNumberPixels, wxT("Number train pixels in class: 1000000"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText267->Wrap( -1 );
   SetUpToolTip(m_staticText267, IDS_ToolTip303);
	bSizer287->Add( m_staticText267, 0, wxLEFT, 25 );
	
	bSizer285->Add( bSizer287, 0, wxEXPAND|wxLEFT|wxRIGHT, 12 );
	
	wxBoxSizer* bSizer288;
	bSizer288 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText268 = new wxStaticText( this, IDC_FieldNamePrompt, wxT("Enter Field Identifier:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText268->Wrap( -1 );
   SetUpToolTip(m_staticText268, IDS_ToolTip304);
	bSizer288->Add( m_staticText268, 0, wxALL, 5 );
	
	m_textCtrl103 = new wxTextCtrl( this, IDC_FieldName, wxEmptyString, wxDefaultPosition, wxSize( 250,-1 ), 0 );
   m_textCtrl103->SetMaxLength(31);
   SetUpToolTip(m_textCtrl103, IDS_ToolTip304);
	bSizer288->Add( m_textCtrl103, 0, wxALL, 5 );
	
	m_staticText269 = new wxStaticText( this, IDC_FieldNumberPixels, wxT("Number pixels: 1000000"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText269->Wrap( -1 );
   SetUpToolTip(m_staticText269, IDS_ToolTip305);
	bSizer288->Add( m_staticText269, 0, wxLEFT, 25 );
	
	bSizer285->Add( bSizer288, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );
	
	wxBoxSizer* bSizer289;
	bSizer289 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer292;
	bSizer292 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText270 = new wxStaticText( this, IDC_AreaTypeOutline, wxT("AreaType:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText270->Wrap( -1 );
   SetUpToolTip(m_staticText270, IDS_ToolTip306);
	bSizer292->Add( m_staticText270, 0, wxALL, 5 );
	
	m_radioBtn27 = new wxRadioButton( this, IDC_Training, wxT("Training Field"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_radioBtn27, IDS_ToolTip306);
	bSizer292->Add( m_radioBtn27, 0, wxALL, 5 );
	
	m_radioBtn28 = new wxRadioButton( this, IDC_TestField, wxT("Test Field"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_radioBtn28, IDS_ToolTip306);
	bSizer292->Add( m_radioBtn28, 0, wxALL, 5 );
	
	bSizer289->Add( bSizer292, 0, wxEXPAND, 5 );
	/*
	wxBoxSizer* bSizer293 = new wxBoxSizer (wxHORIZONTAL);
	
	m_button84 = new wxButton (this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
	//bSizer293->Add (m_button84, 0, wxALIGN_BOTTOM|wxALL, 5);
	bSizer293->Add (m_button84, wxSizerFlags(0).Border(wxALL, 5));
	
	m_button85 = new wxButton (this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
	//bSizer293->Add (m_button85, 0, wxALIGN_BOTTOM|wxALL, 5);
	bSizer293->Add (m_button85, wxSizerFlags(0).Border(wxALL, 5));
	
	//bSizer289->Add (bSizer293, 0, wxALIGN_BOTTOM|wxEXPAND, 5 );
   bSizer289->Add (bSizer293, wxSizerFlags(0).Align(wxALIGN_BOTTOM));
	*/
	bSizer285->Add (bSizer289, 0, wxLEFT|wxTOP|wxRIGHT, 12);
	
	//wxSizer* standardButtonSizer = CreateButtonSizer (wxOK | wxCANCEL);
	//bSizer285->Add (standardButtonSizer, wxSizerFlags(0).Right());
	CreateStandardButtons (bSizer285);
	
	this->SetSizer (bSizer285);
	this->Layout ();
	
	this->Centre (wxBOTH);
	
}	// end "CreateControls"


//Press keyboard "Enter"  -> wxID_OK
void CMNewClassFieldDlg::OnCharHook (wxKeyEvent& event)

{
	if (event.GetKeyCode() == WXK_RETURN)
    	{
		if (Validate() && TransferDataFromWindow ())
			this->EndModal (wxID_OK);
    	}
	
	else
		{
		event.Skip ();
		
    	}
	
}	// end "OnCharHook"



//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the reformat
//							options dialog box to the user so that the user can
//							selection which reformat function is to be run.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/09/96
//	Revised By:			Larry L. Biehl			Date: 06/16/2017	

Boolean CMNewClassFieldDlg::DoDialog (
				Boolean								newClassOnlyFlag, 
				char*									classNamePtr,
				char*									fieldNamePtr, 
				SInt16*								fieldTypePtr)

{                              
	Boolean								OKFlag = FALSE;
	
	SInt16								returnCode;
	UInt16								selectedItem;
	
	
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
																			
	m_newClassOnlyFlag = newClassOnlyFlag; 
	m_classNamePtr = classNamePtr;
	m_fieldNamePtr = fieldNamePtr;
	m_fieldTypePtr = fieldTypePtr;
   
	//strcpy (m_classNamebuf, (const char*)m_className.mb_str(wxConvUTF8)); 
   m_classNameCStringPtr = m_classNamebuf;

	//strcpy (m_fieldNamebuf, (const char*)m_fieldName.mb_str(wxConvUTF8)); 
   m_fieldNameCStringPtr = m_fieldNamebuf;

	returnCode = ShowModal();
	
	if (returnCode == wxID_OK)
		{ 
		OKFlag = TRUE;   					
				// Class Name		

      CtoPstring ((UCharPtr)m_classNameCStringPtr, (UCharPtr)m_classNamePtr);
      returnCode = CheckForDuplicateClassName (-1, (Str255*)m_classNamePtr);					
      if (returnCode > 0)
			{ 
			if ( returnCode == 1 )
				DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);						
			else		// returnCode == 2                   
				DupClassFieldNameAlert (1, (UCharPtr)m_classNamePtr); 
				
			selectedItem = IDC_ClassName;
			OKFlag = FALSE;		
						
			}	// end "if (returnCode > 0)" 
	
		if (OKFlag)
			{	
			CtoPstring ((UCharPtr)m_fieldNameCStringPtr, (UCharPtr)m_fieldNamePtr);	
																				
					// Check if proposed field name is a duplicate.	 If it  	
					// is, allow user to change it again.						
					
			returnCode = CheckForDuplicateFieldName (-1, (Str255*)m_fieldNamePtr);						
			if (returnCode > 0)
				{                                     
				if ( returnCode == 1 )
					DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);						
				else		// returnCode == 2                                           
					DupClassFieldNameAlert (2, (UCharPtr)m_fieldNamePtr);
					  
				selectedItem = IDC_FieldName;
				OKFlag = FALSE;
												
				}	// end "if ( returnCode > 0 )"
				
			}	// end "if (OKFlag)"  
			
		//CtoPstring (m_classNameCStringPtr, (char*)classNamePtr); 					
					// Field Identifier				
		//CtoPstring (m_fieldNameCStringPtr, (char*)fieldNamePtr);
						
					// Training field or test field	
														          
		if (m_fieldType == 0)
			*fieldTypePtr = kTrainingType;
							
		if (m_fieldType == 1)
			*fieldTypePtr = kTestingType;
			
		}	// end "if (returnCode == IDOK)"
			
	if (!OKFlag)                     
		SelectDialogItemText (this, selectedItem, 0, SInt16_MAX); 
		
	return (OKFlag);
		
}		// end "DoDialog"



void CMNewClassFieldDlg::OnInitDialog(wxInitDialogEvent& event)

{                          
	CMDialog*				dialogPtr = this; 
	
	UInt16					selectedItem;
	
	
//	CDialog::OnInitDialog();
	
			// Add current list of classes to the combo box.
			
	CreateClassNameComboBoxList( (wxComboBox*)FindWindow(IDC_ClassList) );
	   
	   	// Set the specified class.
	   	
	m_classList = gProjectInfoPtr->lastClassForAddFieldSelection-1;
//	DDX_CBIndex(m_dialogToPtr, IDC_ClassList, m_classList);
   wxComboBox* classlist = (wxComboBox*) FindWindow(IDC_ClassList);
   classlist->SetSelection(m_classList);
	
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
	
//	return FALSE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"



//void CMNewClassFieldDlg::OnOK(void)
//
//{  
//	Boolean 		OKFlag = TRUE;
//	SInt16		returnCode;
//	UInt16		selectedItem;
//	
//	
//	if ( !UpdateData(TRUE) )
//																							return; 
//	                                   
//	CtoPstring (m_classNameCStringPtr, (char*)m_classNamePtr);
//					
//			// Check if proposed class name is a duplicate.	 If it is, 	
//			// allow user to change it again.									
//	
//	returnCode = CheckForDuplicateClassName (-1, (Str255*)m_classNamePtr);					
//	if ( returnCode > 0 )
//		{ 
//		if ( returnCode == 1 )
//			DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
//						
//		else		// returnCode == 2                   
//			DupClassFieldNameAlert (1, m_classNamePtr); 
//			
//		selectedItem = IDC_ClassName;
//		OKFlag = FALSE;
//					
//		}		// end "if ( returnCode > 0 )" 
//	
//	if (OKFlag)
//		{	
//		CtoPstring (m_fieldNameCStringPtr, (char*)m_fieldNamePtr);
//								                              
//				// Check if proposed field name is a duplicate.	 If it  	
//				// is, allow user to change it again.							
//		
//		returnCode = CheckForDuplicateFieldName (-1, (Str255*)m_fieldNamePtr);						
//		if ( returnCode > 0 )
//			{                                     
//			if ( returnCode == 1 )
//				DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
//						
//			else		// returnCode == 2                                           
//				DupClassFieldNameAlert (2, m_fieldNamePtr);
//				  
//			selectedItem = IDC_FieldName;
//			OKFlag = FALSE;
//								
//			}		// end "if ( returnCode > 0 )"
//			
//		}		// end "if (OKFlag)"  
//	
//	if (OKFlag)
//		CMDialog::OnOK();
//		
//	else		// !OKFlag                      
//		SelectDialogItemText (this, selectedItem, 0, SInt16_MAX); 
//		
//}		// end "OnOK"
// 
 
 
void CMNewClassFieldDlg::OnSelendokClassList(wxCommandEvent& event)

{                   
//	DDX_CBIndex(m_dialogFromPtr, IDC_ClassList, m_classList);
	wxComboBox* classlist = (wxComboBox*) FindWindow(IDC_ClassList);
   m_classList = classlist->GetSelection();
   
	NewClassFieldDialogChangeClass (this, 
												(SInt16)(m_classList+1), 
												m_fieldType+1,
												m_numberSelectionPixels);
	
}		// end "OnSelendokClassList" 



void CMNewClassFieldDlg::OnFieldType(wxCommandEvent& event)
{                    
//	DDX_Radio(m_dialogFromPtr, IDC_Training, m_fieldType);
	wxRadioButton* training = (wxRadioButton*) FindWindow(IDC_Training);
   if(training->GetValue() == true)
      m_fieldType = 0;
   else
      m_fieldType = 1;
   
	SetNumberClassTrainPixels (this, m_numberSelectionPixels, m_fieldType+1);
	
}		// end "OnFieldType"



void CMNewClassFieldDlg::CheckLength(wxCommandEvent& event){
   DisplayAlert(kErrorAlertID,
         kStopAlert,
         kAlertStrID,
         IDS_Alert116,
         0,
         NULL);
}



bool CMNewClassFieldDlg::TransferDataFromWindow(){
   wxComboBox* classlist = (wxComboBox*) FindWindow(IDC_ClassList);
   wxTextCtrl* classname = (wxTextCtrl*) FindWindow(IDC_ClassName);
   wxTextCtrl* fieldname = (wxTextCtrl*) FindWindow(IDC_FieldName);
   wxRadioButton* training = (wxRadioButton*) FindWindow(IDC_Training);
	
   m_classList = classlist->GetSelection();
   m_className = classname->GetValue();
   m_fieldName = fieldname->GetValue();
	
   strcpy( m_classNamebuf, (const char*)m_className.mb_str(wxConvUTF8) );
   strcpy( m_fieldNamebuf, (const char*)m_fieldName.mb_str(wxConvUTF8) );
	
   if(training->GetValue() == true)
      m_fieldType = 0;
   else
      m_fieldType = 1;
	
   return true;
}



bool CMNewClassFieldDlg::TransferDataToWindow(){
   wxComboBox* classlist = (wxComboBox*) FindWindow(IDC_ClassList);
   wxTextCtrl* classname = (wxTextCtrl*) FindWindow(IDC_ClassName);
   wxTextCtrl* fieldname = (wxTextCtrl*) FindWindow(IDC_FieldName);
   wxRadioButton* training = (wxRadioButton*) FindWindow(IDC_Training);
   wxRadioButton* testfield = (wxRadioButton*) FindWindow(IDC_TestField);
	
   classlist->SetSelection(m_classList);
   classname->SetValue(m_className);
   fieldname->SetValue(m_fieldName);
	
   if (m_fieldType == 0)
      training->SetValue(true);
   else
      testfield->SetValue(true);
	
   return true;
	
}	// end "TransferDataToWindow"
