//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClassifyCEMDialog.cpp : class implementation file
//	Class Definition:		LClassifyCEMDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			10/19/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMCEMClassifyDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "LMultiSpec.h"  
#include "LClassifyCEMDialog.h"
#include "LImage_dialog.cpp"



CMCEMClassifyDialog::CMCEMClassifyDialog (
				wxWindow*							parent, 
				wxWindowID							id, 
				const wxString&					title)
		: CMDialog (id, parent, title)
				
{
   m_correlationMatrixCode = -1;
   m_trainingFieldsFlag = FALSE;
   m_testFieldsFlag = FALSE;

   m_cemParametersPtr = NULL;
   m_localClassAreaPtr = NULL;

   m_dialogSelectArea.windowPtr = gProjectSelectionWindow;

   m_initializedFlag = CMDialog::m_initializedFlag;

   if (m_initializedFlag)
		{
				// Get memory for the local class area list vector.

		m_localClassAreaPtr = (SInt16*)MNewPointer (
					(UInt32) gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));

		m_initializedFlag = (m_localClassAreaPtr != NULL);

		}	// end "if (m_initializedFlag)"

   CreateControls ();
   SetSizerAndFit (bSizer156);

}	// end "CMCEMClassifyDialog"



BEGIN_EVENT_TABLE (CMCEMClassifyDialog, CMDialog)
	EVT_INIT_DIALOG (CMCEMClassifyDialog::OnInitDialog)
	EVT_RADIOBUTTON (IDC_UseSelectedArea, CMCEMClassifyDialog::OnUseSelectedArea)
	EVT_RADIOBUTTON (IDC_UseClasses, CMCEMClassifyDialog::OnUseClasses)
	EVT_COMBOBOX (IDC_ClassCombo, CMCEMClassifyDialog::OnSelendokClassCombo)
	EVT_TEXT (IDC_ColumnEnd, CMCEMClassifyDialog::CheckColumnEnd)
	EVT_TEXT (IDC_ColumnStart, CMCEMClassifyDialog::CheckColumnStart)
	EVT_TEXT (IDC_LineEnd, CMCEMClassifyDialog::CheckLineEnd)
	EVT_TEXT (IDC_LineStart, CMCEMClassifyDialog::CheckLineStart)
	EVT_BUTTON (IDEntireImage, CMCEMClassifyDialog::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMCEMClassifyDialog::ToSelectedImage)
END_EVENT_TABLE ()



void CMCEMClassifyDialog::BackgroundCorrelationSettings (
				Boolean								areaCode)

{
	ShowHideDialogItem (this, IDC_ClassCombo, (areaCode == 1));
	HideShowAreaItems (areaCode == 2);

	// Determine if this is the entire area and set the
	// to entire image icon.

	if (areaCode == 2) 
		{
		SetEntireImageButtons (NULL,
										 m_LineStart,
										 m_LineEnd,
										 m_ColumnStart,
										 m_ColumnEnd);

		SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);

		}	// end "if (areaCode == 2)"

}	// end "BackgroundCorrelationSettings"



void CMCEMClassifyDialog::CreateControls ()

{
   wxBitmap entireimi = wxBITMAP_PNG_FROM_DATA (entireim);
   wxBitmap toentirei = wxBITMAP_PNG_FROM_DATA (toentire);
   wxBitmap selectedi = wxBITMAP_PNG_FROM_DATA (selected);
   wxBitmap bmp4i = wxBITMAP_PNG_FROM_DATA (bmp4);

   this->SetSizeHints (wxDefaultSize, wxDefaultSize);

   //wxBoxSizer* bSizer156;
   bSizer156 = new wxBoxSizer (wxVERTICAL);

   m_staticText175 = new wxStaticText (
									this, 
									wxID_ANY, 
									wxT("Area(s) to use for background correlation matrix:"), 
									wxDefaultPosition, 
									wxDefaultSize, 
									0);
   m_staticText175->Wrap (-1);
   bSizer156->Add (m_staticText175, 0, wxALL, 12);

   wxBoxSizer* bSizer157;
   bSizer157 = new wxBoxSizer (wxVERTICAL);

   wxBoxSizer* bSizer158;
   bSizer158 = new wxBoxSizer (wxHORIZONTAL);

   m_radioBtn15 = new wxRadioButton (this, 
													IDC_UseClasses, 
													wxT("Classes: "), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
   SetUpToolTip (m_radioBtn15, IDS_ToolTip325);
   bSizer158->Add (m_radioBtn15, 0, wxALIGN_CENTER|wxALL, 5);

   m_comboBox26 = new wxComboBox (this, 
												IDC_ClassCombo, 
												wxT("Combo!"), 
												wxDefaultPosition, 
												wxDefaultSize, 
												0, 
												NULL, 
												wxCB_READONLY);
   m_comboBox26->Append (wxT("All"));
   m_comboBox26->Append (wxT("Subset..."));
   SetUpToolTip (m_comboBox26, IDS_ToolTip103);
   bSizer158->Add (m_comboBox26, 0, wxALL, 5);


   bSizer157->Add (bSizer158, 0, wxEXPAND|wxLEFT|wxRIGHT, 30);

   wxBoxSizer* bSizer161;
   bSizer161 = new wxBoxSizer (wxVERTICAL);

   m_radioBtn16 = new wxRadioButton (this, 
													IDC_UseSelectedArea, 
													wxT("Selected Area:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
   SetUpToolTip (m_radioBtn16, IDS_ToolTip326);
   bSizer161->Add (m_radioBtn16, 0, wxALL, 5);

   wxStaticBoxSizer* sbSizer8;
   wxStaticBox* areaStaticBox = new wxStaticBox (
													this, 
													IDC_LineColFrame, 
													wxT("Area for background correlation matrix"));
   sbSizer8 = new wxStaticBoxSizer (areaStaticBox, wxHORIZONTAL);

   wxBoxSizer* bSizer791;
   bSizer791 = new wxBoxSizer (wxHORIZONTAL);


   m_bpButton4 = new wxBitmapButton (areaStaticBox, 
													IDEntireImage, 
													entireimi, 
													wxDefaultPosition, 
													wxDefaultSize, 
													wxBU_AUTODRAW);
   m_bpButton4->SetBitmapDisabled (toentirei);    
   //bSizer791->Add (m_bpButton4, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxALL, 5);
   bSizer791->Add (m_bpButton4, 0, wxALIGN_CENTER | wxALL, 5);
   m_bpButton5 = new wxBitmapButton (areaStaticBox, 
													IDSelectedImage, 
													selectedi, 
													wxDefaultPosition, 
													wxDefaultSize, 
													wxBU_AUTODRAW);    
   m_bpButton5->Hide ();
   m_bpButton5->SetBitmapDisabled (selectedi);
   //bSizer791->Add (m_bpButton5, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxALL, 5);
   bSizer791->Add (m_bpButton5, 0, wxALIGN_CENTER | wxALL, 5);

   sbSizer8->Add (bSizer791, 0, wxEXPAND, 5);

   wxBoxSizer* bSizer121;
   bSizer121 = new wxBoxSizer (wxVERTICAL);

   m_staticText177 = new wxStaticText (
							sbSizer8->GetStaticBox (), 
							IDC_StartEndInterval, 
							wxT("                      Start       \tEnd          Interval"), 
							wxDefaultPosition, 
							wxDefaultSize, 
							0);
   m_staticText177->Wrap (-1);
   bSizer121->Add (m_staticText177, 0, wxALL, 5);

   wxGridSizer* gSizer1;
   gSizer1 = new wxGridSizer (2, 4, 0, 0);

   m_staticText60 = new wxStaticText (sbSizer8->GetStaticBox (), 
													IDC_LinePrompt, 
													wxT("Lines"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
   m_staticText60->Wrap (-1);
   gSizer1->Add (m_staticText60, 0, wxALIGN_CENTER|wxALL, 2);

   m_linestartctrl = new wxTextCtrl (sbSizer8->GetStaticBox (), 
													IDC_LineStart, 
													wxEmptyString, 
													wxDefaultPosition, 
													wxSize (70,-1), 
													0);
   m_linestartctrl->SetMaxLength (0); 
   m_linestartctrl->SetValidator (
									wxTextValidator (wxFILTER_NUMERIC, &m_LineStartString));

   gSizer1->Add (m_linestartctrl, 0, wxALIGN_CENTER|wxALL, 2);

   m_lineendctrl = new wxTextCtrl (sbSizer8->GetStaticBox (), 
												IDC_LineEnd, 
												wxEmptyString, 
												wxDefaultPosition, 
												wxSize (70,-1), 
												0);
   m_lineendctrl->SetMaxLength (0); 
   m_lineendctrl->SetValidator (
									wxTextValidator (wxFILTER_NUMERIC, &m_LineEndString));

   gSizer1->Add (m_lineendctrl, 0, wxALIGN_CENTER|wxALL, 2);

   m_lineintctrl = new wxTextCtrl (sbSizer8->GetStaticBox (), 
												IDC_LineInterval, 
												wxEmptyString, 
												wxDefaultPosition, 
												wxSize (70,-1), 
												0);
   m_lineintctrl->SetMaxLength (0); 
   m_lineintctrl->SetValidator (
								wxTextValidator (wxFILTER_NUMERIC, &m_LineIntervalString));

   gSizer1->Add (m_lineintctrl, 0, wxALIGN_CENTER|wxALL, 2);

   m_staticText62 = new wxStaticText (sbSizer8->GetStaticBox (), 
													IDC_ColumnPrompt, 
													wxT("Columns"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
   m_staticText62->Wrap (-1);
   gSizer1->Add (m_staticText62, 0, wxALIGN_CENTER|wxALL, 2);

   m_colstartctrl = new wxTextCtrl (sbSizer8->GetStaticBox (), 
												IDC_ColumnStart, 
												wxEmptyString, 
												wxDefaultPosition, 
												wxSize (70,-1), 
												0);
   m_colstartctrl->SetMaxLength (0); 
   m_colstartctrl->SetValidator (
									wxTextValidator (wxFILTER_NUMERIC, &m_ColumnStartString));

   gSizer1->Add (m_colstartctrl, 0, wxALIGN_CENTER|wxALL, 2);

   m_colendctrl = new wxTextCtrl (sbSizer8->GetStaticBox (), 
												IDC_ColumnEnd, 
												wxEmptyString, 
												wxDefaultPosition, 
												wxSize (70,-1), 
												0);
   m_colendctrl->SetMaxLength (0); 
   m_colendctrl->SetValidator (
									wxTextValidator (wxFILTER_NUMERIC, &m_ColumnEndString));

   gSizer1->Add (m_colendctrl, 0, wxALIGN_CENTER|wxALL, 2);

   m_colintctrl = new wxTextCtrl (sbSizer8->GetStaticBox (), 
												IDC_ColumnInterval, 
												wxEmptyString, 
												wxDefaultPosition, 
												wxSize (70,-1), 
												0);
   m_colintctrl->SetMaxLength (0); 
   m_colintctrl->SetValidator (
								wxTextValidator (wxFILTER_NUMERIC, &m_ColumnIntervalString));

   gSizer1->Add (m_colintctrl, 0, wxALIGN_CENTER|wxALL, 2);

   SetUpToolTip (m_bpButton4, IDS_ToolTip40);  
   SetUpToolTip (m_linestartctrl, IDS_ToolTip19);
   SetUpToolTip (m_lineendctrl, IDS_ToolTip20);
   SetUpToolTip (m_lineintctrl, IDS_ToolTip21);
   SetUpToolTip (m_colstartctrl, IDS_ToolTip22);
   SetUpToolTip (m_colendctrl, IDS_ToolTip23);
   SetUpToolTip (m_colintctrl, IDS_ToolTip24);
   
   bSizer121->Add (gSizer1, 1, wxEXPAND, 5);


   sbSizer8->Add (bSizer121, 1, wxEXPAND, 5);


   bSizer161->Add (sbSizer8, 0, wxEXPAND, 5);


   bSizer157->Add (bSizer161, 0, wxEXPAND|wxLEFT|wxRIGHT, 30);


   bSizer156->Add (bSizer157, 0, wxEXPAND, 5);

   wxBoxSizer* bSizer164;
   bSizer164 = new wxBoxSizer (wxHORIZONTAL);

   m_button35 = new wxButton (this, 
										wxID_CANCEL, 
										wxT("Cancel"), 
										wxDefaultPosition, 
										wxDefaultSize, 
										0);
   bSizer164->Add (m_button35, 0, wxALL, 5);

   m_button36 = new wxButton (this, 
										wxID_OK, 
										wxT("OK"), 
										wxDefaultPosition, 
										wxDefaultSize, 
										0);
   bSizer164->Add (m_button36, 0, wxALL, 5);


   bSizer156->Add (bSizer164, 0, wxALIGN_RIGHT|wxALL, 12);


   this->SetSizer (bSizer156);
   this->Layout ();

   this->Centre (wxBOTH);

}	// end "CreateControls"



//------------------------------------------------------------------------------------
//								 Copyright (2009-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the CEM
//							specification dialog box to the user and copy the
//							revised back to the classify specification structure if
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
//	Coded By:			Wei-Kang Hsu			Date: 04/09/2015
//	Revised By:			Larry L. Biehl			Date: 05/13/2016	

Boolean CMCEMClassifyDialog::DoDialog (
				CEMParametersPtr					cemParametersPtr,
				UInt16*								classifyProcedureEnteredCodePtr)
				
{
	DialogSelectArea					dialogSelectArea;

	SInt16								returnCode;

	Boolean								continueFlag = FALSE;
	
	
			// Make sure intialization has been completed.

	if (!m_initializedFlag)
																							return (FALSE);

	m_cemParametersPtr = cemParametersPtr;

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

		CEMClassifyDialogOK (cemParametersPtr,
									 &dialogSelectArea,
									 m_correlationMatrixClassPtr,
									 m_localClassAreaPtr,
									 (SInt16) m_localNumberClassAreas,
									 m_classSelection,
									 (m_correlationMatrixCode == 0),
									 classifyProcedureEnteredCodePtr);

		}	// end "if (returnCode == IDOK)"

	return (continueFlag);

}	// end "DoDialog"



void CMCEMClassifyDialog::OnInitDialog (
				wxInitDialogEvent&				event)
				
{
	CMDialog::OnInitDialog (event);

			// Indicate whether the class areas are to be used to compute the
			// correlation matrix.

   if (m_cemParametersPtr->correlationMatrixCode & kBothFieldTypes)
		m_correlationMatrixCode = 0;

			// Or the selected area.

   if (m_cemParametersPtr->correlationMatrixCode & kAreaType)
		m_correlationMatrixCode = 1;

   ShowHideDialogItem (this,
								IDC_UseClasses,
								(gProjectInfoPtr->statisticsCode == kMeanCovariance));

   ShowHideDialogItem (this,
								IDC_ClassCombo,
								(m_cemParametersPtr->correlationMatrixCode & kBothFieldTypes));

			// Use all or subset of class train fields to compute the correlation matrix
			//	Make all classes the default

   m_classSelection = m_cemParametersPtr->correlationMatrixClassAreaSet;

			// Note that the handle is already locked.

   m_correlationMatrixClassPtr = (SInt16*)GetHandlePointer (
											m_cemParametersPtr->correlationMatrixClassHandle);

   BlockMoveData (m_correlationMatrixClassPtr,
						m_localClassAreaPtr,
						gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16));

   m_localNumberClassAreas = m_cemParametersPtr->numbercorrelationMatrixClasses;

			// Hide these options for now. Will always use the training field.

   HideDialogItem (this, IDC_Training);
   HideDialogItem (this, IDC_Test);

			//	Selected area to use to compute the correlation matrix.

   m_LineStart = m_cemParametersPtr->lineStart;
   m_LineEnd = m_cemParametersPtr->lineEnd;
   m_LineInterval = m_cemParametersPtr->lineInterval;
   m_ColumnStart = m_cemParametersPtr->columnStart;
   m_ColumnEnd = m_cemParametersPtr->columnEnd;
   m_ColumnInterval = m_cemParametersPtr->columnInterval;

   BackgroundCorrelationSettings (m_correlationMatrixCode + 1);

   if (TransferDataToWindow ())
		PositionDialogWindow ();

			// Set default text selection to first edit text item

   SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);

}	// end "OnInitDialog"



void CMCEMClassifyDialog::OnSelendokClassCombo (
				wxCommandEvent&					event)

{
   HandleClassesMenu (&m_localNumberClassAreas,
                        (SInt16*)m_correlationMatrixClassPtr,
                        1,
                        (SInt16)gProjectInfoPtr->numberStatisticsClasses,
                        IDC_ClassCombo,
                        &m_classSelection);

}	// end "OnSelendokClassCombo"



void CMCEMClassifyDialog::OnUseClasses (
				wxCommandEvent&					event)

{
    BackgroundCorrelationSettings (1);

}	// end "OnUseClasses"



void CMCEMClassifyDialog::OnUseSelectedArea (
				wxCommandEvent&					event)

{
    BackgroundCorrelationSettings (2);

}	// end "OnUseSelectedArea"



bool CMCEMClassifyDialog::TransferDataFromWindow ()

{
   SInt16 returnCode = 0;

   wxTextCtrl* c_end = (wxTextCtrl*)FindWindow (IDC_ColumnEnd);
   wxTextCtrl* c_inter = (wxTextCtrl*)FindWindow (IDC_ColumnInterval);
   wxTextCtrl* c_start = (wxTextCtrl*)FindWindow (IDC_ColumnStart);
   wxTextCtrl* l_end = (wxTextCtrl*)FindWindow (IDC_LineEnd);
   wxTextCtrl* l_inter = (wxTextCtrl*)FindWindow (IDC_LineInterval);
   wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
   wxComboBox* classc = (wxComboBox*)FindWindow (IDC_ClassCombo);
   wxRadioButton* useclass = (wxRadioButton*)FindWindow (IDC_UseClasses);
   
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
   
   m_classSelection = classc->GetSelection ();
   
   if (useclass->GetValue () == true)
      m_correlationMatrixCode = 0;
		
   else	// useclass->GetValue () == false
      {
      m_correlationMatrixCode = 1;
      returnCode = VerifyLineColumnValues (IDC_LineStart, IDC_ColumnStart, true);

      }	// end "else useclass->GetValue () == false"
   
   return (returnCode == 0);
	
}	// end "TransferDataFromWindow"



bool CMCEMClassifyDialog::TransferDataToWindow ()

{
   wxTextCtrl* c_end = (wxTextCtrl*)FindWindow (IDC_ColumnEnd);
   wxTextCtrl* c_inter = (wxTextCtrl*)FindWindow (IDC_ColumnInterval);
   wxTextCtrl* c_start = (wxTextCtrl*)FindWindow (IDC_ColumnStart);
   wxTextCtrl* l_end = (wxTextCtrl*)FindWindow (IDC_LineEnd);
   wxTextCtrl* l_inter = (wxTextCtrl*)FindWindow (IDC_LineInterval);
   wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
   wxComboBox* classc = (wxComboBox*)FindWindow (IDC_ClassCombo);
   wxRadioButton* useclass = (wxRadioButton*)FindWindow (IDC_UseClasses);
   wxRadioButton* useselarea = (wxRadioButton*)FindWindow (IDC_UseSelectedArea);

   classc->SetSelection (m_classSelection);
	
   if (m_correlationMatrixCode == 0)
		useclass->SetValue (true);
		
   else	// m_correlationMatrixCode != 0
		useselarea->SetValue (true);

   c_end->ChangeValue (wxString::Format (wxT("%i"), m_ColumnEnd));
   c_inter->ChangeValue (wxString::Format (wxT("%i"), m_ColumnInterval));
   c_start->ChangeValue (wxString::Format (wxT("%i"), m_ColumnStart));
   l_end->ChangeValue (wxString::Format (wxT("%i"), m_LineEnd));
   l_inter->ChangeValue (wxString::Format (wxT("%i"), m_LineInterval));
   l_start->ChangeValue (wxString::Format (wxT("%i"), m_LineStart));

   return true;
	
}	// end "TransferDataToWindow"
