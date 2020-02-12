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
//	File:						xClassifyCEMDialog.cpp : class implementation file
//	Class Definition:		xClassifyCEMDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/21/2019
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMCEMClassifyDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xMultiSpec.h"  
#include "xClassifyCEMDialog.h"
#include "xDialog_images.cpp"



BEGIN_EVENT_TABLE (CMCEMClassifyDialog, CMDialog)
	EVT_BUTTON (IDEntireImage, CMCEMClassifyDialog::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMCEMClassifyDialog::ToSelectedImage)

	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_ClassCombo, CMCEMClassifyDialog::OnClassComboSelendok)
	#endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_ClassCombo, CMCEMClassifyDialog::OnClassComboSelendok)
	#endif

	EVT_COMBOBOX_CLOSEUP (IDC_ClassCombo, CMCEMClassifyDialog::OnClassComboCloseUp)
	EVT_COMBOBOX_DROPDOWN (IDC_ClassCombo, CMCEMClassifyDialog::OnClassComboDropDown)

	EVT_INIT_DIALOG (CMCEMClassifyDialog::OnInitDialog)

	EVT_RADIOBUTTON (IDC_UseSelectedArea, CMCEMClassifyDialog::OnUseSelectedArea)
	EVT_RADIOBUTTON (IDC_UseClasses, CMCEMClassifyDialog::OnUseClasses)

	EVT_TEXT (IDC_ColumnEnd, CMCEMClassifyDialog::CheckColumnEnd)
	EVT_TEXT (IDC_ColumnStart, CMCEMClassifyDialog::CheckColumnStart)
	EVT_TEXT (IDC_LineEnd, CMCEMClassifyDialog::CheckLineEnd)
	EVT_TEXT (IDC_LineStart, CMCEMClassifyDialog::CheckLineStart)
END_EVENT_TABLE ()



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
   SetSizeHints (wxDefaultSize, wxDefaultSize);

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

	GetAllSubsetMenuControl (kClassesMenu,
										this,
										IDC_ClassCombo,
										100,
										IDS_ToolTip103);
   bSizer158->Add (m_classesCtrl, 0, wxALL, 5);

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

	CreateLineColumnControls (sbSizer8);

   bSizer161->Add (sbSizer8, 0, wxEXPAND, 5);

   bSizer157->Add (bSizer161, 0, wxEXPAND|wxLEFT|wxRIGHT, 30);

   bSizer156->Add (bSizer157, 0, wxEXPAND, 5);

   CreateStandardButtons (bSizer156);

   SetSizer (bSizer156);
   Layout ();
	Centre (wxBOTH);

}	// end "CreateControls"



//------------------------------------------------------------------------------------
//                   Copyright 2009-2020 Purdue Research Foundation
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
//	Called By:			CEMClassifyDialog in SClassifyDialogs.cpp
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



void CMCEMClassifyDialog::OnClassComboSelendok (
				wxCommandEvent&					event)

{
   HandleClassesMenu (&m_localNumberClassAreas,
                        (SInt16*)m_correlationMatrixClassPtr,
                        1,
                        (SInt16)gProjectInfoPtr->numberStatisticsClasses,
                        IDC_ClassCombo,
                        &m_classSelection);

}	// end "OnClassComboSelendok"



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
   SInt16 								returnCode = 0;
	
	
	TransferLinesColumnsFromWindow ();

   m_classSelection = m_classesCtrl->GetSelection ();
	
   wxRadioButton* useclass = (wxRadioButton*)FindWindow (IDC_UseClasses);
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
   m_classesCtrl->SetSelection (m_classSelection);
	
   wxRadioButton* useclass = (wxRadioButton*)FindWindow (IDC_UseClasses);
   wxRadioButton* useselarea = (wxRadioButton*)FindWindow (IDC_UseSelectedArea);
   if (m_correlationMatrixCode == 0)
		useclass->SetValue (true);
		
   else	// m_correlationMatrixCode != 0
		useselarea->SetValue (true);
	
	TransferLinesColumnsToWindow ();

   return true;
	
}	// end "TransferDataToWindow"
