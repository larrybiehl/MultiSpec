//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LFieldsToThematicDialog.cpp : class implementation file
//	Class Definition:		LFieldsToThematicDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			10/07/2019
//
//	Language:				C++
//
//	System:					Linux & MacOS Operating Systems
//
//	Brief description:	This file contains functions related to the
//								CMFieldsToThematicDialog class.
//
// Following is template for debugging
/*
	int numberChars = sprintf ((char*)gTextString3,
									 " LFieldsToThematicDialog:: (): %s",
									 gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------
//
#include "SMultiSpec.h"

#include "LFieldsToThematicDialog.h" 
#include "LImage_dialog.cpp"



BEGIN_EVENT_TABLE (CMFieldsToThematicDialog, CMDialog)
	EVT_BUTTON (IDEntireImage, CMFieldsToThematicDialog::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMFieldsToThematicDialog::ToSelectedImage)

	EVT_COMBOBOX (IDC_ClassCombo, CMFieldsToThematicDialog::OnClassComboSelendok)
	EVT_COMBOBOX_CLOSEUP (IDC_ClassCombo, CMFieldsToThematicDialog::OnClassComboCloseUp)
	EVT_COMBOBOX_DROPDOWN (IDC_ClassCombo, CMFieldsToThematicDialog::OnClassComboDropDown)

	EVT_INIT_DIALOG (CMFieldsToThematicDialog::OnInitDialog)

	EVT_TEXT (IDC_ColumnEnd, CMFieldsToThematicDialog::CheckColumnEnd)
	EVT_TEXT (IDC_ColumnStart, CMFieldsToThematicDialog::CheckColumnStart)
	EVT_TEXT (IDC_ColumnInterval, CMFieldsToThematicDialog::CheckColumnInterval)
	EVT_TEXT (IDC_LineEnd, CMFieldsToThematicDialog::CheckLineEnd)
	EVT_TEXT (IDC_LineStart, CMFieldsToThematicDialog::CheckLineStart)
	EVT_TEXT (IDC_LineInterval, CMFieldsToThematicDialog::CheckLineInterval)
END_EVENT_TABLE ()



CMFieldsToThematicDialog::CMFieldsToThematicDialog (
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title)
		: CMDialog (CMFieldsToThematicDialog::IDD, pParent, title)

{
	m_trainingAreasFlag = FALSE;
	m_testingAreasFlag = FALSE;
	m_outputFormatCode = 2;
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	                        
	m_reformatOptionsPtr = NULL;
   
   CreateControls ();
	
}	// end "CMFieldsToThematicDialog"



void CMFieldsToThematicDialog::CreateControls ()

{
   //wxBitmap entireimi = wxBITMAP_PNG_FROM_DATA(entireim);
   //wxBitmap toentirei = wxBITMAP_PNG_FROM_DATA(toentire);
   //wxBitmap selectedi = wxBITMAP_PNG_FROM_DATA(selected);
   //wxBitmap bmp4i = wxBITMAP_PNG_FROM_DATA(bmp4);
	
   this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer292;
	bSizer292 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText282 = new wxStaticText (
			this,
			wxID_ANY,
			wxT("Set Specifications for Creating Thematic Image from Project \rTrain/Test Areas"),
			wxDefaultPosition,
			//wxDefaultSize,
			wxSize (400, -1),
			0);
	//m_staticText282->Wrap (-1);
	bSizer292->Add( m_staticText282, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer8;
	
   wxStaticBox* areaStaticBox = new wxStaticBox (this,
   																IDC_LineColFrame,
   																wxT("Thematic image area:"));
   sbSizer8 = new wxStaticBoxSizer(areaStaticBox, wxHORIZONTAL);
	
 	CreateLineColumnControls (sbSizer8);

	//bSizer292->Add (sbSizer8, 0, wxALL|wxEXPAND, 12 );
   bSizer292->Add (sbSizer8, wxSizerFlags(0).Expand().Border(wxALL, 12));
	
	wxBoxSizer* bSizer293;
	bSizer293 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText283 = new wxStaticText( this, wxID_ANY, wxT("Use:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText283->Wrap( -1 );
	bSizer293->Add( m_staticText283, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer307;
	bSizer307 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer308;
	bSizer308 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox66 = new wxCheckBox( this, IDC_TrainingAreas, wxT("Training fields/areas"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox66, IDS_ToolTip190);
	bSizer308->Add( m_checkBox66, 0, wxLEFT, 15 );
	
	m_checkBox67 = new wxCheckBox( this, IDC_TestingAreas, wxT("Testing fields/areas"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox67, IDS_ToolTip191);
	bSizer308->Add( m_checkBox67, 0, wxLEFT, 15 );
	
	
	bSizer307->Add( bSizer308, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer295;
	bSizer295 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText284 = new wxStaticText( this, IDC_ClassPrompt, wxT("Classes:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText284->Wrap( -1 );
	SetUpToolTip(m_staticText284, IDS_ToolTip103);
   bSizer295->Add( m_staticText284, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_comboBox54 = new wxComboBox( this, IDC_ClassCombo, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	m_comboBox54->Append( wxT("All") );
	m_comboBox54->Append( wxT("Subset...") );
   SetUpToolTip(m_comboBox54, IDS_ToolTip103);
	bSizer295->Add( m_comboBox54, 0, wxALIGN_CENTER|wxRIGHT, 25 );
	
	
	bSizer307->Add( bSizer295, 0, wxEXPAND|wxLEFT, 15 );
	
	
	bSizer293->Add( bSizer307, 0, wxEXPAND, 5 );
	
	
	bSizer292->Add( bSizer293, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );
	
	wxBoxSizer* bSizer294;
	bSizer294 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer305;
	bSizer305 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText285 = new wxStaticText( this, wxID_ANY, wxT("Output Format Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText285->Wrap( -1 );
   SetUpToolTip(m_staticText285, IDS_ToolTip192);
	bSizer305->Add( m_staticText285, 0, wxALL, 5 );
	
	m_comboBox55 = new wxComboBox( this, IDC_OutputFormatCombo, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	m_comboBox55->Append( wxT("No Header Info") );
	m_comboBox55->Append( wxT("ERDAS.GIS") );
	m_comboBox55->Append( wxT("GeoTIFF") );
   SetUpToolTip(m_comboBox55, IDS_ToolTip192);
	bSizer305->Add( m_comboBox55, 0, wxLEFT, 25 );
	
	
	bSizer294->Add( bSizer305, 1, wxEXPAND, 5 );

	bSizer292->Add( bSizer294, 0, wxALL|wxEXPAND, 12 );
	
	CreateStandardButtons (bSizer292);
	
	SetSizerAndFit( bSizer292 );
	Layout();
	
	Centre( wxBOTH );
	
}	// end "CreateControls"


//------------------------------------------------------------------------------------
//								 Copyright (2009-2019)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the project
//							areas to thematic image dialog box to the user and copy the
//							revised items back to the specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
//	Called By:
//
//	Coded By:			Larry L. Biehl			Date: ??/??/2013
//	Revised By:			Larry L. Biehl			Date: 05/04/2019

Boolean CMFieldsToThematicDialog::DoDialog(
				ReformatOptionsPtr				reformatOptionsPtr)

{
	DialogSelectArea					dialogSelectArea;
	
	SInt16								returnCode;
	
	Boolean								continueFlag = FALSE;

	
			// Make sure intialization has been completed.
	
	if (!m_initializedFlag)
																			return (FALSE);
	
	m_reformatOptionsPtr = reformatOptionsPtr;
	
	returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{
		continueFlag = TRUE;
		
		dialogSelectArea.lineStart = m_LineStart;
		dialogSelectArea.lineEnd = m_LineEnd;
		dialogSelectArea.lineInterval = m_LineInterval;
		
		dialogSelectArea.columnStart = m_ColumnStart;
		dialogSelectArea.columnEnd = m_ColumnEnd;
		dialogSelectArea.columnInterval = m_ColumnInterval;

		LoadAreasToThematicDialogOK (reformatOptionsPtr,
												&dialogSelectArea,
												m_trainingAreasFlag,
												m_testingAreasFlag,
												m_classSelection,
												m_localNumberClasses,
												m_outputFormatCode+1);
		
		}		// end "if (returnCode == IDOK)"
	
	return (continueFlag);
	
}	// end "DoDialog"



void CMFieldsToThematicDialog::OnInitDialog (
				wxInitDialogEvent& 					event)

{
	SInt16									classSelection,
												outputFormatCode;
	
	
	LoadAreasToThematicDialogInitialize (this,
														m_reformatOptionsPtr,
														&m_dialogSelectArea,
														(Boolean*)&m_trainingAreasFlag,
														(Boolean*)&m_testingAreasFlag,
														&classSelection,
														&m_classListPtr,
														&m_localNumberClasses,
														&outputFormatCode );
	
			// Selected area to classify.
	
	m_LineStart = m_reformatOptionsPtr->lineStart;
	m_LineEnd = m_reformatOptionsPtr->lineEnd;
	m_LineInterval = m_reformatOptionsPtr->lineInterval;
	m_ColumnStart = m_reformatOptionsPtr->columnStart;
	m_ColumnEnd = m_reformatOptionsPtr->columnEnd;
	m_ColumnInterval = m_reformatOptionsPtr->columnInterval;
	
			// Update the maximum number of lines and columns for later use.
	
	m_maxNumberLines = m_dialogSelectArea.imageWindowInfoPtr->maxNumberLines;
	m_maxNumberColumns = m_dialogSelectArea.imageWindowInfoPtr->maxNumberColumns;
	
			// Classes to use.
	
	m_classSelection = classSelection;

			// Output format to use.

	m_outputFormatCode = outputFormatCode - 1;
	
			// Determine if this is the entire area and set the
			// to entire image icon.
	
	SetEntireImageButtons (
						NULL,
						m_LineStart,
						m_LineEnd,
						m_ColumnStart,
						m_ColumnEnd);
	
	if (TransferDataToWindow())
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item
	
	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);
	
	return;

}	// end "OnInitDialog"



bool CMFieldsToThematicDialog::TransferDataFromWindow ()

{
   SInt16								returnCode = 0;
	
	
   wxTextCtrl* c_end = (wxTextCtrl*)FindWindow (IDC_ColumnEnd);
	wxTextCtrl* c_inter = (wxTextCtrl*)FindWindow (IDC_ColumnInterval);
	wxTextCtrl* c_start = (wxTextCtrl*)FindWindow (IDC_ColumnStart);
	wxTextCtrl* l_end = (wxTextCtrl*)FindWindow (IDC_LineEnd);
	wxTextCtrl* l_inter = (wxTextCtrl*)FindWindow (IDC_LineInterval);
	wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
   
   wxCheckBox* test = (wxCheckBox*)FindWindow(IDC_TestingAreas);
   wxCheckBox* train = (wxCheckBox*)FindWindow(IDC_TrainingAreas);
   
   wxComboBox* classcb = (wxComboBox*)FindWindow(IDC_ClassCombo);
   wxComboBox* outputcb = (wxComboBox*)FindWindow(IDC_OutputFormatCombo);
   
   m_LineStartString = l_start->GetValue ();
	m_LineEndString = l_end->GetValue ();
	m_LineIntervalString = l_inter->GetValue ();
	m_ColumnStartString = c_start->GetValue ();
	m_ColumnEndString = c_end->GetValue ();
	m_ColumnIntervalString = c_inter->GetValue ();
	m_LineStart = wxAtoi (m_LineStartString);
	m_LineEnd = wxAtoi (m_LineEndString);
	m_LineInterval = wxAtoi (m_LineIntervalString);
	m_ColumnStart = wxAtoi (m_ColumnStartString);
	m_ColumnEnd = wxAtoi (m_ColumnEndString);
	m_ColumnInterval = wxAtoi (m_ColumnIntervalString);
   
   m_classSelection = classcb->GetSelection();
   //if (m_classSelection < 0)
	//	m_classSelection = m_classSelection_Saved;
   
   m_trainingAreasFlag = train->GetValue ();
   m_testingAreasFlag = test->GetValue ();
   
   m_outputFormatCode = outputcb->GetSelection ();
      
   returnCode = VerifyLineColumnValues (IDC_LineStart,
														IDC_ColumnStart,
														true);
   
   return (returnCode == 0);
	
}	// end "TransferDataFromWindow"


   
bool CMFieldsToThematicDialog::TransferDataToWindow ()

{
   wxTextCtrl* c_end = (wxTextCtrl*) FindWindow (IDC_ColumnEnd);
	wxTextCtrl* c_inter = (wxTextCtrl*) FindWindow (IDC_ColumnInterval);
	wxTextCtrl* c_start = (wxTextCtrl*) FindWindow (IDC_ColumnStart);
	wxTextCtrl* l_end = (wxTextCtrl*) FindWindow (IDC_LineEnd);
	wxTextCtrl* l_inter = (wxTextCtrl*) FindWindow (IDC_LineInterval);
	wxTextCtrl* l_start = (wxTextCtrl*) FindWindow (IDC_LineStart);
   
   wxCheckBox* test = (wxCheckBox*) FindWindow (IDC_TestingAreas);
   wxCheckBox* train = (wxCheckBox*) FindWindow (IDC_TrainingAreas);
   
   wxComboBox* classcb = (wxComboBox*)FindWindow (IDC_ClassCombo);
   wxComboBox* outputcb = (wxComboBox*)FindWindow (IDC_OutputFormatCombo);
   
   c_end->ChangeValue (wxString::Format (wxT("%i"), m_ColumnEnd));
	c_inter->ChangeValue (wxString::Format (wxT("%i"), m_ColumnInterval));
	c_start->ChangeValue (wxString::Format (wxT("%i"), m_ColumnStart));
	l_end->ChangeValue (wxString::Format (wxT("%i"), m_LineEnd));
	l_inter->ChangeValue (wxString::Format (wxT("%i"), m_LineInterval));
	l_start->ChangeValue (wxString::Format (wxT("%i"), m_LineStart));
   
   classcb->SetSelection (m_classSelection);
   outputcb->SetSelection (m_outputFormatCode);
   
   test->SetValue (m_testingAreasFlag);
   train->SetValue (m_trainingAreasFlag);
   
   return true;
	
}	// end "TransferDataToWindow"
