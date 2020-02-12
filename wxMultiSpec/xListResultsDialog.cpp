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
//	File:						xListResultsDialog.cpp : class implementation file
//	Class Definition:		xListResultsDialog.h
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
//								CMListResultsDialog class.
//
// Following is template for debugging
/*
		int numberChars = sprintf ((char*)gTextString3,
									 " xListResultsDialog:: (): %s",
									 gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "xListResultsDialog.h"
#include "xMultiSpec.h"

#include "xDialog_images.cpp"

extern ListResultsSpecsPtr gListResultsSpecsPtr;



BEGIN_EVENT_TABLE (CMListResultsDialog, CMDialog)
	EVT_BUTTON (IDEntireImage, CMListResultsDialog::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMListResultsDialog::ToSelectedImage)

	EVT_CHECKBOX (IDC_Class, CMListResultsDialog::OnClassSummary)
	EVT_CHECKBOX (IDC_Field, CMListResultsDialog::OnFieldSummary)
	EVT_CHECKBOX (IDC_Group, CMListResultsDialog::OnGroupSummary)
	EVT_CHECKBOX (IDC_Image, CMListResultsDialog::OnImage)
	EVT_CHECKBOX (IDC_Test, CMListResultsDialog::OnTest)
	EVT_CHECKBOX (IDC_ThresholdResults, CMListResultsDialog::OnThresholdResults)
	EVT_CHECKBOX (IDC_Training, CMListResultsDialog::OnTraining)

	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_ClassCombo, CMListResultsDialog::OnClassComboSelendok)
	#endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_ClassCombo, CMListResultsDialog::OnClassComboSelendok)
	#endif

	EVT_COMBOBOX_CLOSEUP (IDC_ClassCombo, CMListResultsDialog::OnClassComboCloseUp)
	EVT_COMBOBOX_DROPDOWN (IDC_ClassCombo, CMListResultsDialog::OnClassComboDropDown)

	EVT_INIT_DIALOG (CMListResultsDialog::OnInitDialog)

	EVT_RADIOBUTTON (IDC_Area, CMListResultsDialog::OnAreaUnits)
	EVT_RADIOBUTTON (IDC_Percent, CMListResultsDialog::OnPercentUnits)
	EVT_RADIOBUTTON (IDC_Samples, CMListResultsDialog::OnSampleUnits)

	EVT_TEXT (IDC_ColumnEnd, CMListResultsDialog::CheckColumnEnd)
	EVT_TEXT (IDC_ColumnStart, CMListResultsDialog::CheckColumnStart)
	EVT_TEXT (IDC_LineEnd, CMListResultsDialog::CheckLineEnd)
	EVT_TEXT (IDC_LineStart, CMListResultsDialog::CheckLineStart)
END_EVENT_TABLE ()



CMListResultsDialog::CMListResultsDialog (
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title)
		: CMDialog (CMListResultsDialog::IDD, pParent, title)

{
   m_trainingFlag = FALSE;
   m_testFlag = FALSE;
   m_imageFlag = FALSE;
   m_fieldSummaryFlag = FALSE;
   m_classSummaryFlag = FALSE;
   m_groupSummaryFlag = FALSE;
   m_conversionFactor = 0;
   m_thresholdResultsFlag = FALSE;
   m_thresholdPercent = 0;
   m_textWindowFlag = FALSE;
   m_diskFileFlag = FALSE;
   m_tableType = -1;
   m_tableUnits = -1;

   m_projectWindowFlag = (gProjectInfoPtr != NULL);
   m_initializedFlag = CMDialog::m_initializedFlag;

   if (m_initializedFlag)
   	{
      		// Get memory for the local class list vector.

      m_classListPtr = NULL;
      if (m_projectWindowFlag)
      	{
         m_classListPtr = (UInt16*)MNewPointer (
					(UInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));

         m_initializedFlag = (m_classListPtr != NULL);

      	}	// end "if (m_projectWindowFlag)"

   	}	// end "if (m_initializedFlag)"

   CreateControls ();
   SetSizerAndFit (bSizer188);
	
}	// end "CMListResultsDialog"



CMListResultsDialog::~CMListResultsDialog (void)

{
	m_classListPtr = CheckAndDisposePtr (m_classListPtr);

}	// end "~CMListResultsDialog"



void CMListResultsDialog::CheckListAreaItems ()

{
   m_localAreaCode = 0;

   if (m_trainingFlag)
      m_localAreaCode += 1;

   if (m_testFlag)
      m_localAreaCode += 2;

   if (m_imageFlag)
      m_localAreaCode += 4;

   SetSummarizeByItems ();

}	// end "CheckListAreaItems"



void CMListResultsDialog::CheckSummaryItems ()

{
   m_localSummaryCode = 0;

   if (m_fieldSummaryFlag)
      m_localSummaryCode += kFieldSummary;

   if (m_classSummaryFlag)
      m_localSummaryCode += kClassSummary;

   if (m_groupSummaryFlag)
      m_localSummaryCode += kGroupSummary;

   SetSummarizeByItems ();

}	// end "CheckSummaryItems"



void CMListResultsDialog::CreateControls ()

{
   SetSizeHints (wxDefaultSize, wxDefaultSize);

  	bSizer188 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer189;
	bSizer189 = new wxBoxSizer (wxHORIZONTAL);
	
	wxBoxSizer* bSizer191;
	bSizer191 = new wxBoxSizer (wxVERTICAL);
	
	wxStaticBoxSizer* sbSizer18;
	sbSizer18 = new wxStaticBoxSizer (
								new wxStaticBox (this,
														wxID_ANY,
														wxT("List results for following areas")),
								wxVERTICAL);
	
	wxBoxSizer* bSizer193;
	bSizer193 = new wxBoxSizer (wxHORIZONTAL);
	
	m_checkBox36 = new wxCheckBox (sbSizer18->GetStaticBox (),
												IDC_Training,
												wxT("Training"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox36, IDS_ToolTip263);
	bSizer193->Add (m_checkBox36, 0, wxLEFT, 5);

	GetAllSubsetMenuControl (kClassesMenu,
										sbSizer18->GetStaticBox (),
										IDC_ClassCombo,
										100,
										IDS_ToolTip103);
	bSizer193->Add (m_classesCtrl, 0, wxLEFT|wxRIGHT|wxTOP, 15);
	
	sbSizer18->Add (bSizer193, 0, wxEXPAND);
	
	m_checkBox37 = new wxCheckBox (sbSizer18->GetStaticBox (),
												IDC_Test, wxT("Test"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox37, IDS_ToolTip264);
	sbSizer18->Add (m_checkBox37, 0, wxLEFT|wxBOTTOM, 5);
	
	wxBoxSizer* bSizer117;
	bSizer117 = new wxBoxSizer (wxVERTICAL);
	
	m_checkBox38 = new wxCheckBox (sbSizer18->GetStaticBox (),
												IDC_Image, wxT("Image selection"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox38, IDS_ToolTip265);
	bSizer117->Add (m_checkBox38, 0, wxLEFT|wxTOP, 5);

	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer (new wxStaticBox (sbSizer18->GetStaticBox (),
																		IDC_LineColFrame,
																		//wxT("Area to List"),
																		wxEmptyString,
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL),
													wxHORIZONTAL);
	
	CreateLineColumnControls (sbSizer8);

	bSizer117->Add (sbSizer8, 0, wxEXPAND|wxTOP, 10);
	
	sbSizer18->Add (bSizer117, 1, wxEXPAND, 5);
	
	bSizer191->Add (sbSizer18, 0, wxEXPAND, 5);
	
	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer (new wxStaticBox (
															this,
															IDC_SummarizeOutline,
															wxT("Summarize by (train/test only):")),
												wxVERTICAL);
	
	m_checkBox39 = new wxCheckBox (sbSizer9->GetStaticBox (),
												IDC_Field, wxT("Field"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	m_checkBox39->SetValue (true);
   SetUpToolTip (m_checkBox39, IDS_ToolTip266);
	sbSizer9->Add (m_checkBox39, 0, wxLEFT|wxBOTTOM, 5);
	
	m_checkBox40 = new wxCheckBox (sbSizer9->GetStaticBox (),
												IDC_Class,
												wxT("Class"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	m_checkBox40->SetValue (true);
   SetUpToolTip (m_checkBox40, IDS_ToolTip267);
	sbSizer9->Add (m_checkBox40, 0, wxLEFT|wxTOP|wxBOTTOM, 5);
	
	m_checkBox41 = new wxCheckBox (sbSizer9->GetStaticBox (),
												IDC_Group,
												wxT("Group"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	m_checkBox41->SetValue (true);
	SetUpToolTip (m_checkBox41, IDS_ToolTip268);
   sbSizer9->Add (m_checkBox41, 0, wxLEFT|wxTOP, 5);
	
	bSizer191->Add (sbSizer9, 0, wxEXPAND, 5);
	
	bSizer189->Add (bSizer191, 0, wxALL|wxEXPAND, 12);
	
	wxBoxSizer* bSizer192;
	bSizer192 = new wxBoxSizer (wxVERTICAL);
	
	wxStaticBoxSizer* sbSizer10;
	sbSizer10 = new wxStaticBoxSizer (new wxStaticBox (this,
																		wxID_ANY,
																		wxT("Table type")),
													wxVERTICAL);
	
	m_radioBtn19 = new wxRadioButton (sbSizer10->GetStaticBox (),
													IDC_ClassTable,
													wxT("Class"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_radioBtn19, IDS_ToolTip269);
	sbSizer10->Add (m_radioBtn19, 0, wxLEFT|wxBOTTOM, 5);
	
	m_radioBtn20 = new wxRadioButton (sbSizer10->GetStaticBox (),
													IDC_GroupTable,
													wxT("Group"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_radioBtn20, IDS_ToolTip269);
	sbSizer10->Add (m_radioBtn20, 0, wxLEFT|wxTOP|wxBOTTOM, 5);
	
	
	bSizer192->Add (sbSizer10, 0, wxEXPAND, 5);
	
	wxStaticBoxSizer* sbSizer11;
	sbSizer11 = new wxStaticBoxSizer (new wxStaticBox (this,
																		wxID_ANY,
																		wxT("Table units")),
													wxVERTICAL);
	
	m_radioBtn21 = new wxRadioButton (sbSizer11->GetStaticBox (),
													IDC_Samples,
													wxT("Samples"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_radioBtn21, IDS_ToolTip270);
	sbSizer11->Add (m_radioBtn21, 0, wxLEFT|wxBOTTOM, 5);
	
	m_radioBtn22 = new wxRadioButton (sbSizer11->GetStaticBox (),
													IDC_Percent,
													wxT("Percent"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_radioBtn22, IDS_ToolTip270);
	sbSizer11->Add (m_radioBtn22, 0, wxLEFT|wxTOP|wxBOTTOM, 5);
	
	wxBoxSizer* bSizer198;
	bSizer198 = new wxBoxSizer (wxHORIZONTAL);
	
	m_radioBtn23 = new wxRadioButton (sbSizer11->GetStaticBox (),
													IDC_Area,
													wxT("Area"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_radioBtn23, IDS_ToolTip270);
	//bSizer198->Add (m_radioBtn23, 0, wxALIGN_CENTER|wxLEFT, 15);
	bSizer198->Add (m_radioBtn23, 0, wxLEFT|wxTOP, 5);
	
	m_staticText203 = new wxStaticText (sbSizer11->GetStaticBox (),
													IDC_ConversionText,
													wxT("conversion factor:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText203->Wrap (-1);
   SetUpToolTip (m_staticText203, IDS_ToolTip271);
	bSizer198->Add (m_staticText203, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_textCtrl106 = new wxTextCtrl (sbSizer11->GetStaticBox (),
												IDC_ConversionFactor,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_textCtrl106, IDS_ToolTip271);
	bSizer198->Add (m_textCtrl106, 0, wxALIGN_CENTER|wxBOTTOM|wxLEFT|wxRIGHT, 5);
	
	
	sbSizer11->Add (bSizer198, 0, wxEXPAND, 5);
	
	
	bSizer192->Add (sbSizer11, 0, wxEXPAND, 5);
	
	wxBoxSizer* bSizer199;
	bSizer199 = new wxBoxSizer (wxVERTICAL);
	
	m_checkBox42 = new wxCheckBox (this,
												IDC_ThresholdResults,
												wxT("Threshold results at"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox42, IDS_ToolTip272);
	bSizer199->Add (m_checkBox42, 0, wxALL, 5);
	
	wxBoxSizer* bSizer200;
	bSizer200 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText204 = new wxStaticText (this,
													IDC_ProbabilityPrompt,
													wxT("Probabilities <"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText204->Wrap (-1);
	bSizer200->Add (m_staticText204, 0, wxALIGN_CENTER|wxLEFT, 45);
	
	m_textCtrl107 = new wxTextCtrl (this,
												IDC_Probability,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (80, -1),
												0);
	bSizer200->Add (m_textCtrl107, 0, wxALIGN_CENTER|wxLEFT, 5);
	
	m_staticText205 = new wxStaticText (this,
													IDC_LRdegrees,
													wxT("o"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText205->Wrap (-1);
	bSizer200->Add (m_staticText205, 0, wxLEFT, 5);
	
	m_staticText206 = new wxStaticText (this,
													IDC_LRPercent,
													wxT("%"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText206->Wrap (-1);
	bSizer200->Add (m_staticText206, 0, wxALIGN_CENTER|wxALL|wxLEFT, 5);
	
	bSizer199->Add (bSizer200, 1, wxBOTTOM|wxEXPAND, 5);
	
	bSizer192->Add (bSizer199, 0, wxEXPAND, 5);
	
	wxStaticBoxSizer* sbSizer16;
	sbSizer16 = new wxStaticBoxSizer (new wxStaticBox (this,
																		wxID_ANY,
																		wxT("Write results to:")),
													wxVERTICAL);
	
	m_checkBox43 = new wxCheckBox (sbSizer16->GetStaticBox (),
												IDC_TextWindow,
												wxT("Text window"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	SetUpToolTip (m_checkBox43, IDS_ToolTip66);
   sbSizer16->Add (m_checkBox43, 0, wxLEFT|wxBOTTOM, 5);
	
	m_checkBox44 = new wxCheckBox (sbSizer16->GetStaticBox (),
												IDC_DiskFile,
												wxT("Disk file"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	SetUpToolTip (m_checkBox44, IDS_ToolTip67);
   sbSizer16->Add (m_checkBox44, 0, wxLEFT|wxTOP, 5);
	
	bSizer192->Add (sbSizer16, 0, wxALL|wxEXPAND, 5);
	
	bSizer189->Add (bSizer192, 0, wxALL|wxEXPAND, 12);
	
	bSizer188->Add (bSizer189, 0, wxEXPAND, 5);

	CreateStandardButtons (bSizer188);
	
	SetSizer (bSizer188);
	Layout ();
	
	Centre (wxBOTH);
	
}	// end "CreateControls"



Boolean CMListResultsDialog::DoDialog (void)

{
   Boolean 								continueFlag = FALSE;

   SInt16 								returnCode;


   		// Make sure intialization has been completed.

   if (!m_initializedFlag)
      																				return (FALSE);

   returnCode = ShowModal ();

   if (returnCode == wxID_OK)
   	{
      continueFlag = TRUE;

      		// Class areas

      if (m_groupSummaryFlag)
         m_classSelection = 0;

      gListResultsSpecsPtr->classAreaSet = m_classSelection;

      if (m_projectWindowFlag)
      	{
         SInt16* classAreaPtr = (SInt16*)GetHandlePointer (
															gListResultsSpecsPtr->classAreaHandle);

         if (m_classSelection == kAllMenuItem)
            LoadClassAreaVector (&gListResultsSpecsPtr->numberClassAreas,
            							classAreaPtr);

         else	// m_classSelection == kSubsetMenuItem
         	{
            UInt32 		index;

            gListResultsSpecsPtr->numberClassAreas = m_localNumberClasses;
            for (index = 0; index < m_localNumberClasses; index++)
               classAreaPtr[index] = m_classListPtr[index];

         	}	// end "else m_classSelection == kSubsetMenuItem"

      	}	// end "if (m_projectWindowFlag)"

      		// Areas to use to list results.

      gListResultsSpecsPtr->areaCode = 0;
      if (m_trainingFlag)
         gListResultsSpecsPtr->areaCode += kTrainingType;

      if (m_testFlag)
         gListResultsSpecsPtr->areaCode += kTestingType;

      if (m_imageFlag)
         gListResultsSpecsPtr->areaCode += kAreaType;

      		// Selected list results area.

      gListResultsSpecsPtr->lineStart = m_LineStart;
      gListResultsSpecsPtr->lineEnd = m_LineEnd;
      gListResultsSpecsPtr->lineInterval = m_LineInterval;
      gListResultsSpecsPtr->columnStart = m_ColumnStart;
      gListResultsSpecsPtr->columnEnd = m_ColumnEnd;
      gListResultsSpecsPtr->columnInterval = m_ColumnInterval;

      		// Summarize by.

      if (m_nonClusterFieldTypeCode)
      	{
         gListResultsSpecsPtr->summaryCode = 0;
         if (m_fieldSummaryFlag)
            gListResultsSpecsPtr->summaryCode += kFieldSummary;

         if (m_classSummaryFlag)
            gListResultsSpecsPtr->summaryCode += kClassSummary;

         if (m_groupSummaryFlag)
            gListResultsSpecsPtr->summaryCode += kGroupSummary;

      	}	// end "if (nonClusterFieldTypeCode)"

      		// Table type.

      gListResultsSpecsPtr->tableType = 0;
      if (m_tableType == 0)
         gListResultsSpecsPtr->tableType = 1;

      else if (m_tableType == 1)
         gListResultsSpecsPtr->tableType = 2;

      		// Table units.

      gListResultsSpecsPtr->unitsCode = 0;
      if (m_tableUnits == 0)
         gListResultsSpecsPtr->unitsCode += kSampleUnits;

      if (m_tableUnits == 1)
         gListResultsSpecsPtr->unitsCode += kPercentUnits;

      if (m_tableUnits == 2)
         gListResultsSpecsPtr->unitsCode += kAreaUnits;

      if (gListResultsSpecsPtr->unitsCode == kAreaUnits)
         gListResultsSpecsPtr->summaryUnitsCode = kAreaUnits;

      else	// ...->unitsCode != kAreaUnits
      	{
         if (GetDefaultAreaConversionFactor (
            								gImageFileInfoPtr->mapProjectionHandle,
            								gImageWindowInfoPtr->windowInfoHandle) == 0)
            gListResultsSpecsPtr->summaryUnitsCode = kPercentUnits;

			}	// end "else ...->unitsCode != kAreaUnits"

      		// Area conversion factor.

      if (m_conversionFactor > 0)
         gListResultsSpecsPtr->areaConversionFactor = m_conversionFactor;

      		// Threshold results flag.

      gListResultsSpecsPtr->thresholdFlag = m_thresholdResultsFlag;

      if (gListResultsSpecsPtr->thresholdFlag)
      	{
         		// Get probability threshold.

         gListResultsSpecsPtr->probabilityThreshold = m_thresholdPercent;

         		// Get the image data value which corresponds to the given
         		// threshold value.

         Handle fileInfoHandle = GetFileInfoHandle (
            							gListResultsSpecsPtr->probabilityWindowInfoHandle);

         gListResultsSpecsPtr->probabilityThresholdCode =
            				GetThresholdCode (m_thresholdPercent,
														fileInfoHandle,
														gListResultsSpecsPtr->thresholdTypeCode);

      	}	// end "if (gListResultsSpecsPtr->thresholdFlag)"

      		// Write list results to output window.
      		// Write list results to disk file.

      gListResultsSpecsPtr->outputStorageType = 0;

      if (m_textWindowFlag)
         gListResultsSpecsPtr->outputStorageType += 1;

      if (m_diskFileFlag)
         gListResultsSpecsPtr->outputStorageType += 2;

   	}	// end "if (returnCode == IDOK)"

   return (continueFlag);

}	// end "DoDialog"


/*
void CMListResultsDialog::HideShowProbabilityItems (
            Boolean								thresholdResultsFlag)
				
{  
   if (thresholdResultsFlag)
      {                                          
      MShowDialogItem (this, IDC_ProbabilityPrompt);
      MShowDialogItem (this, IDC_Probability);
	
      }	// end "if (thresholdResultsFlag)"
	
   else	// !thresholdResultsFlag
      {	                                        
      MHideDialogItem (this, IDC_ProbabilityPrompt);
      MHideDialogItem (this, IDC_Probability);
		
      }	// end "else !thresholdResultsFlag"
	
}	// end "HideShowProbabilityItems"
*/


void CMListResultsDialog::HideShowUnitsItems (
   			Boolean 								areaUnitsFlag)

{
   if (areaUnitsFlag)
   	{
      MShowDialogItem (this, IDC_ConversionText);
      MShowDialogItem (this, IDC_ConversionFactor);

   	}	// end "if (areaUnitsFlag)"

   else	// !areaUnitsFlag
   	{
      MHideDialogItem (this, IDC_ConversionText);
      MHideDialogItem (this, IDC_ConversionFactor);

   	}	// end "else !areaUnitsFlag"

}	// end "HideShowUnitsItems"



void CMListResultsDialog::OnAreaUnits (
				wxCommandEvent& 					event)

{
   HideShowUnitsItems (TRUE);

}	// end "OnAreaUnits"



void CMListResultsDialog::OnClassSummary (
				wxCommandEvent& 					event)

{
   wxCheckBox * classcb = (wxCheckBox*)FindWindow (IDC_Class);
   m_classSummaryFlag = classcb->GetValue ();
   CheckSummaryItems ();

}	// end "OnClassSummary"



void CMListResultsDialog::OnFieldSummary (
				wxCommandEvent& 					event)

{
   wxCheckBox* field = (wxCheckBox*)FindWindow (IDC_Field);
   m_fieldSummaryFlag = field->GetValue ();
   CheckSummaryItems ();

}	// end "OnFieldSummary"



void CMListResultsDialog::OnGroupSummary (
				wxCommandEvent& 					event)

{
   wxCheckBox* group = (wxCheckBox*)FindWindow (IDC_Group);
   m_groupSummaryFlag = group->GetValue ();
   CheckSummaryItems ();

}	// end "OnGroupSummary"


void CMListResultsDialog::OnImage (
				wxCommandEvent& 					event)

{
   wxCheckBox* image = (wxCheckBox*)FindWindow (IDC_Image);
   m_imageFlag = image->GetValue ();
   CheckListAreaItems ();

   HideShowAreaItems (m_imageFlag);

   if (m_imageFlag)
		{
      SetEntireImageButtons (NULL,
										m_LineStart,
										m_LineEnd,
										m_ColumnStart,
										m_ColumnEnd);

      		// Set default text selection to first edit text item

      SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);

   	}	// end "if (m_imageFlag)"

}	// end "OnImage"



void CMListResultsDialog::OnInitDialog (
				wxInitDialogEvent& 				event)

{
   m_nonClusterFieldTypeCode = 0;

   if (m_projectWindowFlag)
      m_nonClusterFieldTypeCode = DetermineFieldTypes ();

   m_nonClusterFieldTypeCode &= 0x0003;

   		// List areas to be used - training, test, and/or image selection.

   m_localAreaCode = gListResultsSpecsPtr->areaCode;

   if (m_projectWindowFlag)
   	{
      		// List results for training areas. Unhilite this control if there
      		// are no training areas for the project.

      if (!(m_nonClusterFieldTypeCode & kTrainingType))
         SetDLogControlHilite (this, IDC_Training, 255);

      else	// m_nonClusterFieldTypeCode & kTrainingType
         m_trainingFlag = (m_localAreaCode & kTrainingType);

      		// List results for test areas. Unhilite this control if there
      		// are no test areas for the project.

      if (!(m_nonClusterFieldTypeCode & kTestingType))
         SetDLogControlHilite (this, IDC_Test, 255);

      else	// m_nonClusterFieldTypeCode & kTestingType
         m_testFlag = (m_localAreaCode & kTestingType);

      if (!m_trainingFlag && !m_testFlag)
         MHideDialogItem (this, IDC_ClassCombo);

   	}	// end "if (m_projectWindowFlag)"

   else	// !m_projectWindowFlag
   	{
      SetDLogControlHilite (this, IDC_Training, 255);
      SetDLogControlHilite (this, IDC_Test, 255);
      MHideDialogItem (this, IDC_ClassCombo);

   	}	// end "else !m_projectWindowFlag"

   		// Class areas to use.

   m_classSelection = gListResultsSpecsPtr->classAreaSet;
   m_localNumberClasses = gListResultsSpecsPtr->numberClassAreas;

   if (m_projectWindowFlag)
		{
      UInt32 index;

      UInt16* classPtr = (UInt16*)GetHandlePointer (
															gListResultsSpecsPtr->classAreaHandle);

      for (index = 0; index < m_localNumberClasses; index++)
         m_classListPtr[index] = classPtr[index];

   	}	// end "if (m_projectWindowFlag)"

   		// If the train/test areas are to be summarized by groups then all
   		// classes will need to be used.

   if (gListResultsSpecsPtr->summaryCode & kGroupSummary)
      MHideDialogItem (this, IDC_ClassCombo);

   		// List results for selected area.

   m_imageFlag = (m_localAreaCode & kAreaType) || (m_nonClusterFieldTypeCode == 0);

   		//	Selected area for list results
   		// 	Initialize selected area structure.

   InitializeDialogSelectArea (&m_dialogSelectArea,
											gImageWindowInfoPtr,
											gActiveImageWindow,
											gListResultsSpecsPtr->columnStart,
											gListResultsSpecsPtr->columnEnd,
											gListResultsSpecsPtr->columnInterval,
											gListResultsSpecsPtr->lineStart,
											gListResultsSpecsPtr->lineEnd,
											gListResultsSpecsPtr->lineInterval,
											12,
											10,
											kAdjustToBaseImage);

   m_LineStart = gListResultsSpecsPtr->lineStart;
   m_LineEnd = gListResultsSpecsPtr->lineEnd;
   m_LineInterval = gListResultsSpecsPtr->lineInterval;
   m_ColumnStart = gListResultsSpecsPtr->columnStart;
   m_ColumnEnd = gListResultsSpecsPtr->columnEnd;
   m_ColumnInterval = gListResultsSpecsPtr->columnInterval;

   HideShowAreaItems (m_imageFlag);

   if (m_imageFlag)
      SetEntireImageButtons (NULL,
										m_LineStart,
										m_LineEnd,
										m_ColumnStart,
										m_ColumnEnd);

			// Summarize by field.
			// Summarize by class. 																
			// Summarize by group.

   m_localSummaryCode = gListResultsSpecsPtr->summaryCode;

   if (m_projectWindowFlag)
		{
      		// Verify that the group summary option is not available if there
      		// are not groups defined.

      if (gImageFileInfoPtr->numberGroups == 0)
         m_localSummaryCode &= (kFieldSummary + kClassSummary);

      if (m_nonClusterFieldTypeCode &&
         				(m_localAreaCode & (kTrainingType + kTestingType)))
			{
         m_fieldSummaryFlag = ((m_localSummaryCode & kFieldSummary) != 0);
         m_classSummaryFlag = ((m_localSummaryCode & kClassSummary) != 0);
         m_groupSummaryFlag = ((m_localSummaryCode & kGroupSummary) != 0);

      	}	// end "if (nonClusterFieldTypeCode && ...)"

      if (gImageFileInfoPtr->numberGroups == 0)
         SetDLogControlHilite (this, IDC_Group, 255);

      if (m_localAreaCode == kAreaType || m_nonClusterFieldTypeCode == 0)
			{
         SetDLogControlHilite (this, IDC_Field, 255);
         SetDLogControlHilite (this, IDC_Class, 255);
         SetDLogControlHilite (this, IDC_Group, 255);

      	}	// end "if (m_localAreaCode == ...)"

   	}	// end "if (projectWindowFlag)"

   else	// !projectWindowFlag
   	{
      MHideDialogItem (this, IDC_SummarizeOutline);
      MHideDialogItem (this, IDC_Field);
      MHideDialogItem (this, IDC_Class);
      MHideDialogItem (this, IDC_Group);

   	}	// end "else !projectWindowFlag"

   		// Class type table.
   		// Group type table.

   if (gListResultsSpecsPtr->tableType == kClassTable)
      m_tableType = 0;

   else if (gListResultsSpecsPtr->tableType == kGroupTable)
      m_tableType = 1;

   if (gImageFileInfoPtr->numberGroups == 0)
      SetDLogControlHilite (this, IDC_GroupTable, 255);

   		// Sample units.
   		// Percent units.
   		// Area units.

   if (gListResultsSpecsPtr->unitsCode & kSampleUnits)
      m_tableUnits = 0;

   else if (gListResultsSpecsPtr->unitsCode & kPercentUnits)
      m_tableUnits = 1;

   else if (gListResultsSpecsPtr->unitsCode & kAreaUnits)
      m_tableUnits = 2;

   HideShowUnitsItems (m_tableUnits == 2);

   		// Area conversion factor.

   m_saveAreaConversionFactor = gListResultsSpecsPtr->areaConversionFactor;
   m_conversionFactor = m_saveAreaConversionFactor;

   		// Threshold the results.
   		// Threshold percent.

   m_thresholdResultsFlag = gListResultsSpecsPtr->thresholdFlag;
   if (gListResultsSpecsPtr->probabilityWindowInfoHandle == NULL)
      SetDLogControlHilite (this, IDC_ThresholdResults, 255);

   m_saveThresholdPercent = gListResultsSpecsPtr->probabilityThreshold;
   m_thresholdPercent = m_saveThresholdPercent;

   ListResultsDialogSetThresholdItems (this,
      m_thresholdResultsFlag,
      gListResultsSpecsPtr->thresholdTypeCode);

   		// List results to output text window.
   		// List results to disk file.

   m_textWindowFlag = (gListResultsSpecsPtr->outputStorageType & 0x0001);
   m_diskFileFlag = (gListResultsSpecsPtr->outputStorageType & 0x0002);

   if (TransferDataToWindow ())
      PositionDialogWindow ();

   		// Set default text selection to first edit text item
	
 	if (m_imageFlag)
   	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);

}	// end "OnInitDialog"



void CMListResultsDialog::OnPercentUnits (
				wxCommandEvent& 					event)

{
   HideShowUnitsItems (FALSE);

}	// end "OnPercentUnits"



void CMListResultsDialog::OnSampleUnits (
				wxCommandEvent& 					event)

{
   HideShowUnitsItems (FALSE);

}	// end "OnSampleUnits"



void CMListResultsDialog::OnClassComboSelendok (
				wxCommandEvent& 					event)

{
   HandleClassesMenu (&m_localNumberClasses,
								(SInt16*)m_classListPtr,
								1,
								(SInt16) gProjectInfoPtr->numberStatisticsClasses,
								IDC_ClassCombo,
								&m_classSelection);

}	// end "OnClassComboSelendok"



void CMListResultsDialog::OnTest (
				wxCommandEvent& 					event)

{
   wxCheckBox* test = (wxCheckBox*)FindWindow (IDC_Test);
   m_testFlag = test->GetValue ();
   CheckListAreaItems ();

}	// end "OnTest"



void CMListResultsDialog::OnThresholdResults (
				wxCommandEvent& 					event)

{
   wxCheckBox* thres = (wxCheckBox*)FindWindow (IDC_ThresholdResults);
   m_thresholdResultsFlag = thres->GetValue ();

   ListResultsDialogSetThresholdItems (this,
      m_thresholdResultsFlag,
      gListResultsSpecsPtr->thresholdTypeCode);

   //	HideShowProbabilityItems (m_thresholdResultsFlag);

}	// end "OnThresholdResults"



void CMListResultsDialog::OnTraining (
				wxCommandEvent& 					event)

{
   wxCheckBox* train = (wxCheckBox*)FindWindow (IDC_Training);
   m_trainingFlag = train->GetValue ();

   CheckListAreaItems ();

}	// end "OnTraining"



void CMListResultsDialog::SetSummarizeByItems ()

{
   Boolean tempFlag = TRUE;


   if (m_projectWindowFlag)
		{
      tempFlag = (m_localSummaryCode != 0);

      if (m_localAreaCode == kAreaType || m_localAreaCode == 0)
			{
         m_fieldSummaryFlag = FALSE;
         m_classSummaryFlag = FALSE;
         m_groupSummaryFlag = FALSE;

         wxCheckBox* field = (wxCheckBox*)FindWindow (IDC_Field);
         field->SetValue (m_fieldSummaryFlag);
         wxCheckBox* classcb = (wxCheckBox*)FindWindow (IDC_Class);
         classcb->SetValue (m_classSummaryFlag);
         wxCheckBox* group = (wxCheckBox*)FindWindow (IDC_Group);
         group->SetValue (m_groupSummaryFlag);

         SetDLogControlHilite (this, IDC_Field, 255);
         SetDLogControlHilite (this, IDC_Class, 255);
         SetDLogControlHilite (this, IDC_Group, 255);

         MHideDialogItem (this, IDC_ClassCombo);

      	}	// end "if (m_localAreaCode == kAreaType || ...)"

      else	// localAreaCode != kAreaType && ...
      	{
         m_fieldSummaryFlag = ((m_localSummaryCode & kFieldSummary) != 0);
         m_classSummaryFlag = ((m_localSummaryCode & kClassSummary) != 0);
         m_groupSummaryFlag = ((m_localSummaryCode & kGroupSummary) != 0);

			wxCheckBox* field = (wxCheckBox*)FindWindow (IDC_Field);
         field->SetValue (m_fieldSummaryFlag);
         wxCheckBox* classcb = (wxCheckBox*)FindWindow (IDC_Class);
         classcb->SetValue (m_classSummaryFlag);
         wxCheckBox* group = (wxCheckBox*)FindWindow (IDC_Group);
         group->SetValue (m_groupSummaryFlag);

         SetDLogControlHilite (this, IDC_Field, 0);
         SetDLogControlHilite (this, IDC_Class, 0);

         if (gImageFileInfoPtr->numberGroups > 0)
            SetDLogControlHilite (this, IDC_Group, 0);

         if (m_groupSummaryFlag)
            MHideDialogItem (this, IDC_ClassCombo);

         else	// !m_groupSummaryFlag
            MShowDialogItem (this, IDC_ClassCombo);

      	}	// end "else m_localAreaCode != kAreaType && ..."

   	}	// end "if (m_projectWindowFlag)"

   wxButton* okbutton = (wxButton*)FindWindow (wxID_OK);
   okbutton->Enable ((m_localAreaCode != 0) && tempFlag);

}	// end "SetSummarizeByItems"



bool CMListResultsDialog::TransferDataFromWindow ()

{
   wxTextCtrl* conversion = (wxTextCtrl*)FindWindow (IDC_ConversionFactor);
   wxString conversion_double = conversion->GetValue ();
   conversion_double.ToDouble (&m_conversionFactor);
	
   wxTextCtrl* prob = (wxTextCtrl*)FindWindow (IDC_Probability);
	wxString prob_double = prob->GetValue ();
   prob_double.ToDouble (&m_thresholdPercent);
   
	wxCheckBox* train = (wxCheckBox*)FindWindow (IDC_Training);
   m_trainingFlag = train->GetValue ();
	
	wxCheckBox* test = (wxCheckBox*)FindWindow (IDC_Test);
   m_testFlag = test->GetValue ();
	
	wxCheckBox* image = (wxCheckBox*)FindWindow (IDC_Image);
   m_imageFlag = image->GetValue ();
	
	wxCheckBox* field = (wxCheckBox*)FindWindow (IDC_Field);
   m_fieldSummaryFlag = field->GetValue ();
	
	wxCheckBox* classcb = (wxCheckBox*)FindWindow (IDC_Class);
   m_classSummaryFlag = classcb->GetValue ();
	
	wxCheckBox* group = (wxCheckBox*)FindWindow (IDC_Group);
   m_groupSummaryFlag = group->GetValue ();
	
	wxCheckBox* thres = (wxCheckBox*)FindWindow (IDC_ThresholdResults);
   m_thresholdResultsFlag = thres->GetValue ();
	
	wxCheckBox* text = (wxCheckBox*)FindWindow (IDC_TextWindow);
   m_textWindowFlag  = text->GetValue ();
	
	wxCheckBox* disk = (wxCheckBox*)FindWindow (IDC_DiskFile);
	m_diskFileFlag = disk->GetValue ();

	wxRadioButton* sample = (wxRadioButton*)FindWindow (IDC_Samples);
   wxRadioButton* percent = (wxRadioButton*)FindWindow (IDC_Percent);
  	if (sample->GetValue ())
		m_tableUnits = 0;
	
	else if (percent->GetValue ())
		m_tableUnits = 1;
	
	else	// !sample->GetValue () && !percent->GetValue ()
		m_tableUnits = 2;

	wxRadioButton* classtable = (wxRadioButton*)FindWindow (IDC_ClassTable);
   if (classtable->GetValue ())
		m_tableType = 0;
	
	else	// !classtable->GetValue ()
		m_tableType = 1;
	
	TransferLinesColumnsFromWindow ();

   return true;
	
}	// end "TransferDataFromWindow"



bool CMListResultsDialog::TransferDataToWindow ()

{
   wxRadioButton* classtable = (wxRadioButton*)FindWindow (IDC_ClassTable);
   wxRadioButton* grouptable = (wxRadioButton*)FindWindow (IDC_GroupTable);
   if (m_tableType == 0)
		{
      classtable->SetValue (true);
      grouptable->SetValue (false);
		
   	}	// end "if (m_tableType == 0)"
	
	else	// m_tableType != 0
		{
		classtable->SetValue (false);
      grouptable->SetValue (true);
		
   	}	// end "else m_tableType != 0"
   
   wxRadioButton* sample = (wxRadioButton*)FindWindow (IDC_Samples);
   wxRadioButton* percent = (wxRadioButton*)FindWindow (IDC_Percent);
   wxRadioButton* area = (wxRadioButton*)FindWindow (IDC_Area);
   if (m_tableUnits == 0)
		{
      sample->SetValue (true);
      percent->SetValue (false);
      area->SetValue (false);
		
		}	// end "if (m_tableUnits == 0)"
	
	else if (m_tableUnits == 1)
		{
      sample->SetValue (false);
      percent->SetValue (true);
      area->SetValue (false);
		
		}	// end "else if (m_tableUnits == 1)"
	
	else	// m_tableUnits != 0 && m_tableUnits != 1
		{
      sample->SetValue (false);
      percent->SetValue (false);
      area->SetValue (true);
		
		}	// end "else m_tableUnits != 0 && m_tableUnits != 1"

   wxCheckBox* train = (wxCheckBox*)FindWindow (IDC_Training);
   train->SetValue (m_trainingFlag);
	
   wxCheckBox* test = (wxCheckBox*)FindWindow (IDC_Test);
   test->SetValue (m_testFlag);
	
   wxCheckBox* image = (wxCheckBox*)FindWindow (IDC_Image);
   image->SetValue (m_imageFlag);
	
   wxCheckBox* field = (wxCheckBox*)FindWindow (IDC_Field);
   field->SetValue (m_fieldSummaryFlag);
	
   wxCheckBox* classcb = (wxCheckBox*)FindWindow (IDC_Class);
   classcb->SetValue (m_classSummaryFlag);
	
   wxCheckBox* group = (wxCheckBox*)FindWindow (IDC_Group);
   group->SetValue (m_groupSummaryFlag);
	
   wxCheckBox* thres = (wxCheckBox*)FindWindow (IDC_ThresholdResults);
   thres->SetValue (m_thresholdResultsFlag);
	
   wxCheckBox* text = (wxCheckBox*)FindWindow (IDC_TextWindow);
   text->SetValue (m_textWindowFlag);
	
   wxCheckBox* disk = (wxCheckBox*)FindWindow (IDC_DiskFile);
	disk->SetValue (m_diskFileFlag);

   m_classesCtrl->SetSelection (m_classSelection);

   wxTextCtrl* conversion = (wxTextCtrl*)FindWindow (IDC_ConversionFactor);
   conversion->ChangeValue (wxString::Format (wxT("%.9f"), m_conversionFactor));
	
   wxTextCtrl* prob = (wxTextCtrl*)FindWindow (IDC_Probability);
	prob->ChangeValue (wxString::Format (wxT("%.1f"), m_thresholdPercent));
	
	TransferLinesColumnsToWindow ();

   return true;
	
}	// end "TransferDataToWindow"
