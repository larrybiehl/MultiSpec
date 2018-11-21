// LEditClassFieldDialog.cpp : implementation file
//
// Revised by Larry Biehl on 11/13/2018
//               
/* Template for debugging for MultiSpec Online on mygeohub.org.
	int numberChars = sprintf ((char*)&gTextString3,
								" LEditClassFieldDialog::xxx (countBytes, errCode) %d, %d%s", 
								countBytes,
								errCode,
								gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);	
*/
#include "SMultiSpec.h"

#include "LImageView.h"
#include "LEditClassFieldDialog.h"



extern void EditClassFieldDialogInitialize (
				DialogPtr							dialogPtr,
				SInt16								classFieldFlag,
				SInt16								classStorage,
				SInt16								currentField,
				UInt16*								selectedItemPtr);



CMEditClassFieldDlg::CMEditClassFieldDlg(wxWindow* pParent, wxWindowID id, const wxString& title)
: CMDialog(CMEditClassFieldDlg::IDD, pParent, title) 
{
   m_className = "Class 1";
   m_fieldName = "Field 1";
   m_fieldType = 0;

   m_classNameCStringPtr = NULL;
   m_fieldNameCStringPtr = NULL;

   try 
		{
				//	Get pointers to the string buffers.	
				
		m_classNameCStringPtr = m_classNamebuf;
		m_fieldNameCStringPtr = m_fieldNamebuf;
      
		}   
	catch (int e) 
		{
      m_initializedFlag = FALSE;
		}

   CreateControls();
   SetSizerAndFit(bSizer285);
   m_initializedFlag = TRUE;
	
}	// end "CMEditClassFieldDlg"


BEGIN_EVENT_TABLE(CMEditClassFieldDlg, CMDialog)
EVT_INIT_DIALOG(CMEditClassFieldDlg::OnInitDialog)
EVT_TEXT_MAXLEN(IDC_ClassName, CMEditClassFieldDlg::CheckLength)
EVT_TEXT_MAXLEN(IDC_FieldName, CMEditClassFieldDlg::CheckLength)
//{{AFX_MSG_MAP(CMEditClassFieldDlg)               
//}}AFX_MSG_MAP
END_EVENT_TABLE()


void CMEditClassFieldDlg::CheckLength (wxCommandEvent& event)
{
   DisplayAlert (kErrorAlertID,
						kStopAlert,
						kAlertStrID,
						IDS_Alert116,
						0,
						NULL);
			
}	// end "CheckLength"


void CMEditClassFieldDlg::CreateControls ()

{
   this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
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
	
	bSizer289->Add( bSizer292, 0, wxEXPAND);
	/*
	wxBoxSizer* bSizer293;
	bSizer293 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button84 = new wxButton (this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
	//bSizer293->Add( m_button84, 0, wxALIGN_BOTTOM|wxALIGN_RIGHT|wxALL, 5 );
	//bSizer293->Add( m_button84, 0, wxALL, 5 );
	bSizer293->Add (m_button84, wxSizerFlags(0).Border(wxALL, 5));
	
	m_button85 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	//bSizer293->Add( m_button85, 0, wxALIGN_BOTTOM|wxALIGN_RIGHT|wxALL, 5 );
	//bSizer293->Add (m_button85, 0, wxALL, 5);
	bSizer293->Add (m_button85, wxSizerFlags(0).Border(wxALL, 5));
	
	//bSizer289->Add( bSizer293, 0, wxALIGN_BOTTOM|wxALIGN_RIGHT|wxEXPAND, 5 );
	//bSizer289->Add ( bSizer293, 0, wxEXPAND, 5 );
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


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
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
//	Coded By:			Abdur Rahman Maud		Date: ??/??/2015
//	Revised By:			Larry L. Biehl			Date: 07/07/2017	

Boolean CMEditClassFieldDlg::DoDialog(
				SInt16								classFieldCode,
				SInt16								currentClass,
				SInt16								currentField,
				char*									titleStringPtr) 
{
   Boolean OKFlag = FALSE;

   SInt16 returnCode;
   UInt16 selectedItem;


			// Make sure intialization has been completed.

   if (!m_initializedFlag)
																							return (FALSE);

   m_classFieldCode = classFieldCode;
   m_currentField = currentField;

   m_classStorage = gProjectInfoPtr->storageClass[currentClass];

   m_titleStringPtr = titleStringPtr;
   
   SetTitle (wxString::FromUTF8(m_titleStringPtr));
   
   returnCode = ShowModal();

   if (returnCode == wxID_OK) 
		{
      OKFlag = TRUE;

      if (classFieldCode == 2) 
			{
					// Class Name
         CtoPstring ((UCharPtr)m_classNameCStringPtr,
							gProjectInfoPtr->classNamesPtr[m_classStorage].name);
			CtoPstring ((UCharPtr)m_classNameCStringPtr, gTextString);
			returnCode = CheckForDuplicateClassName (m_classStorage, (Str255*) gTextString);
         if (returnCode > 0) 
				{
            if (returnCode == 1)
               DisplayAlert(kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);

            else // returnCode == 2
               DupClassFieldNameAlert (1, gTextString);

            selectedItem = IDC_ClassName;
            OKFlag = FALSE;
				}	// end "if ( returnCode > 0 )"
				 
         else
				{ // returnCode == 0
						// change name in class list
            wxComboBox* classList = (wxComboBox*)gProjectWindow->GetFrame()->FindWindow(IDC_ClassList);
            wxString classname = wxString::FromUTF8 ((char*)m_classNameCStringPtr);
            classList->SetString (currentClass+1, classname);
				
				}
				
			}	// end "if (classFieldCode == 2)" 
      
      if (OKFlag && classFieldCode == 3) 
			{
         HPFieldIdentifiersPtr fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
			
					// Field Identifier
         CtoPstring ((UCharPtr)m_fieldNameCStringPtr,
							gProjectInfoPtr->fieldIdentPtr[m_currentField].name);
         CtoPstring ((UCharPtr)m_fieldNameCStringPtr, gTextString);
			
					// Training field or test field						
         SInt16 fieldType = kTrainingType;
         if (m_fieldType == 1)
            fieldType = kTestingType;
         fieldIdentPtr[currentField].fieldType = fieldType;
         returnCode = CheckForDuplicateFieldName (m_currentField, (Str255*) gTextString);
         if (returnCode > 0) 
				{
            if (returnCode == 1)
               DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);

            else // returnCode == 2
               DupClassFieldNameAlert (2, gTextString);

            selectedItem = IDC_FieldName;
            OKFlag = FALSE;
				
				}	// end "if ( returnCode > 0 )"      
				      
			}	// end "if (classFieldCode == 3)" 
			
		}	// end "if (returnCode == IDOK)"
   
	return OKFlag;
		  
}	// end "DoDialog"


void CMEditClassFieldDlg::OnInitDialog (wxInitDialogEvent& event) 
{
   UInt16 selectedItem;


   CMDialog::OnInitDialog (event);

   EditClassFieldDialogInitialize (this,
												m_classFieldCode,
												m_classStorage,
												m_currentField,
												&selectedItem);

	//SetWindowText (m_titleStringPtr);
   
	PositionDialogWindow();

			// Set default text selection 

   SelectDialogItemText (this, selectedItem, 0, SInt16_MAX); 

}	// end "OnInitDialog"
   

//void CMEditClassFieldDlg::OnOK(void) 
//{
//   Boolean OKFlag = TRUE;
//   SInt16 returnCode;
//   UInt16 selectedItem;
//
//
//   UpdateData(TRUE);
//
//   if (m_classFieldCode == 2) {
//      CtoPstring(m_classNameCStringPtr, (char*) gTextString);
//
//      // Check if proposed class name is a duplicate.	 If it is, 	
//      // allow user to change it again.									
//
//      returnCode = CheckForDuplicateClassName(m_classStorage, (Str255*) gTextString);
//      if (returnCode > 0) {
//         if (returnCode == 1)
//            DisplayAlert(kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
//
//         else // returnCode == 2
//            DupClassFieldNameAlert(1, (char*) gTextString);
//
//         selectedItem = IDC_ClassName;
//         OKFlag = FALSE;
//
//      } // end "if ( returnCode > 0 )"                                     
//
//   } // end "if (m_classFieldCode == 2)" 
//
//   if (OKFlag && m_classFieldCode == 3) {
//      CtoPstring(m_fieldNameCStringPtr, (char*) gTextString);
//
//      // Check if proposed field name is a duplicate.	 If it  	
//      // is, allow user to change it again.							
//
//      returnCode = CheckForDuplicateFieldName(m_currentField, (Str255*) gTextString);
//      if (returnCode > 0) {
//         if (returnCode == 1)
//            DisplayAlert(kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
//
//         else // returnCode == 2
//            DupClassFieldNameAlert(2, (char*) gTextString);
//
//         selectedItem = IDC_FieldName;
//         OKFlag = FALSE;
//
//      } // end "if ( returnCode > 0 )"                                        
//
//   } // end "if (OKFlag && ...)"
//
//   if (OKFlag)
//      CMDialog::OnOK();
//
//   else // !OKFlag                      
//      SelectDialogItemText(this, selectedItem, 0, SInt16_MAX);
//
//} // end "OnOK"


bool CMEditClassFieldDlg::TransferDataFromWindow()
{
   wxTextCtrl* classname = (wxTextCtrl*)FindWindow(IDC_ClassName);
   wxTextCtrl* fieldname = (wxTextCtrl*)FindWindow(IDC_FieldName);
   wxRadioButton* training = (wxRadioButton*)FindWindow(IDC_Training);
   
   m_className = classname->GetValue();
   m_fieldName = fieldname->GetValue();
   
   strcpy( m_classNamebuf, (const char*)m_className.mb_str(wxConvUTF8));
   strcpy( m_fieldNamebuf, (const char*)m_fieldName.mb_str(wxConvUTF8)); 
   m_classNameCStringPtr = m_classNamebuf;
   m_fieldNameCStringPtr = m_fieldNamebuf;
   
   if(training->GetValue() == true)
      m_fieldType = 0;
   else
      m_fieldType = 1;
   
   return true;
	
}	// end "TransferDataFromWindow"

// This actually not effective. Data transfer is done in OnInitDialog
bool CMEditClassFieldDlg::TransferDataToWindow()
{
   wxTextCtrl* classname = (wxTextCtrl*)FindWindow(IDC_ClassName);
   wxTextCtrl* fieldname = (wxTextCtrl*)FindWindow(IDC_FieldName);
   wxRadioButton* training = (wxRadioButton*)FindWindow(IDC_Training);
   wxRadioButton* testfield = (wxRadioButton*)FindWindow(IDC_TestField);   

   classname->SetValue(m_className);
   fieldname->SetValue(m_fieldName);
   
   if (m_fieldType == 0)
      training->SetValue(true);
   else
      testfield->SetValue(true);
   
   return true;
	
}	// end "TransferDataToWindow"
