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
//	File:						xProjectionPursuitDialog.cpp : class implementation file
//	Class Definition:		xProjectionPursuitDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			11/16/2018
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMProjectionPursuitDialog class.
//
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h"
#include "xProjectionPursuitDialog.h"



BEGIN_EVENT_TABLE (CMProjectionPursuitDialog, CMDialog)
	EVT_CHECKBOX (IDC_NumericalOptimization, CMProjectionPursuitDialog::OnBnClickedNumericaloptimization)

   EVT_INIT_DIALOG (CMProjectionPursuitDialog::OnInitDialog)

	EVT_RADIOBUTTON (IDC_FS_Algorithm, CMProjectionPursuitDialog::OnBnClickedAlgorithm)
	EVT_RADIOBUTTON (IDC_PP_Algorithm, CMProjectionPursuitDialog::OnBnClickedAlgorithm)
	EVT_RADIOBUTTON (IDC_LastFinalGrouping, CMProjectionPursuitDialog::OnBnClickedInitialBandGrouping)
	EVT_RADIOBUTTON (IDC_TopDownBottomUpMethod, CMProjectionPursuitDialog::OnBnClickedBandGroupingMethod)
	EVT_RADIOBUTTON (IDC_TopDownMethod, CMProjectionPursuitDialog::OnBnClickedBandGroupingMethod)
	EVT_RADIOBUTTON (IDC_Uniform, CMProjectionPursuitDialog::OnBnClickedBandGroupingMethod)
	EVT_RADIOBUTTON (IDC_UniformGrouping, CMProjectionPursuitDialog::OnBnClickedInitialBandGrouping)

   EVT_TEXT (IDC_BothChoicesUpToValue, CMProjectionPursuitDialog::OnCheckValues)
   EVT_TEXT (IDC_BottomUpThreshold, CMProjectionPursuitDialog::OnCheckValues)
   EVT_TEXT (IDC_EditNumberFeatures, CMProjectionPursuitDialog::OnCheckValues)
   EVT_TEXT (IDC_MaxNumberOutputFeatures, CMProjectionPursuitDialog::OnCheckValues)
   EVT_TEXT (IDC_OptimizationThreshold, CMProjectionPursuitDialog::OnCheckValues)
   EVT_TEXT (IDC_TopDownThreshold, CMProjectionPursuitDialog::OnCheckValues)
END_EVENT_TABLE ()



CMProjectionPursuitDialog::CMProjectionPursuitDialog (
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title)
	: CMDialog (CMProjectionPursuitDialog::IDD, pParent, title)

{
	m_algorithmCode = 0;
	m_numericalOptimizationFlag = FALSE;
	m_optimizeThreshold =  0;
	m_initialGroupingCode = 0;
	m_firstStageMethod = 1;
	m_topDownThreshold = 0;
	m_bottomUpThreshold = 0;
	m_bothOddChoicesNumberFeatures = 0;
	m_maximumNumberFeatures = 0;
	m_initialNumberFeatures = 0;
	
	m_initializedFlag = TRUE;
   CreateControls ();
	
}	// end "CMProjectionPursuitDialog"



CMProjectionPursuitDialog::~CMProjectionPursuitDialog ()

{

}	// end "~CMProjectionPursuitDialog"



void CMProjectionPursuitDialog::CreateControls ()

{
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxBoxSizer* bVSizerMain = new wxBoxSizer (wxVERTICAL);
	
	bSizer267 = new wxBoxSizer (wxHORIZONTAL);
	
	wxBoxSizer* bSizer268;
	bSizer268 = new wxBoxSizer (wxVERTICAL);
	
	wxStaticBoxSizer* sbSizer12;
	sbSizer12 = new wxStaticBoxSizer (new wxStaticBox (this,
																		wxID_ANY,
																		wxT("Algorithm")),
													wxVERTICAL);
	
	m_radioBtn29 = new wxRadioButton (sbSizer12->GetStaticBox (),
													IDC_PP_Algorithm,
													wxT("Projection Pursuit (PP)"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_radioBtn29, IDS_ToolTip344);
	sbSizer12->Add (m_radioBtn29, 0, 0, 5);
	
	m_radioBtn30 = new wxRadioButton (sbSizer12->GetStaticBox (),
													IDC_FS_Algorithm,
													wxT("PP Feature Selection"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	SetUpToolTip (m_radioBtn30, IDS_ToolTip345);
   sbSizer12->Add (m_radioBtn30, 0, 0, 5);
	
	m_checkBox57 = new wxCheckBox (sbSizer12->GetStaticBox (),
												IDC_NumericalOptimization,
												wxT("with Numerical Optimization"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	SetUpToolTip (m_checkBox57, IDS_ToolTip351);
   sbSizer12->Add (m_checkBox57, 0, 0, 5);
	
	wxBoxSizer* bSizer270;
	bSizer270 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText257 = new wxStaticText (sbSizer12->GetStaticBox (),
													IDC_OptimizationThresholdPrompt,
													wxT("Optimation threshold (%):"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText257->Wrap (-1);
	SetUpToolTip (m_staticText257, IDS_ToolTip352);
   bSizer270->Add (m_staticText257, 0, wxALIGN_CENTER, 5);
	
	m_textCtrl109 = new wxTextCtrl (sbSizer12->GetStaticBox (),
												IDC_OptimizationThreshold,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (60, -1),
												0,
												wxTextValidator (wxFILTER_NUMERIC));
   SetUpToolTip (m_textCtrl109, IDS_ToolTip352);
   bSizer270->Add (m_textCtrl109, 0, 0, 5);
	
	sbSizer12->Add (bSizer270, 0, wxEXPAND|wxLEFT, 30);
	
	bSizer268->Add (sbSizer12, 0, wxALL|wxEXPAND, 5);
	
	wxStaticBoxSizer* sbSizer13;
	sbSizer13 = new wxStaticBoxSizer (new wxStaticBox (this,
																		wxID_ANY,
																		wxT("Initial Band Grouping")),
													wxVERTICAL);
	
	m_radioBtn31 = new wxRadioButton (sbSizer13->GetStaticBox (),
													IDC_UniformGrouping,
													wxT("Uniform grouping"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_radioBtn31, IDS_ToolTip353);
	sbSizer13->Add (m_radioBtn31, 0, 0, 5);
	
	m_radioBtn32 = new wxRadioButton (sbSizer13->GetStaticBox (),
													IDC_LastFinalGrouping,
													wxT("Last Final Grouping"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	SetUpToolTip (m_radioBtn32, IDS_ToolTip354);
   sbSizer13->Add (m_radioBtn32, 0, 0, 5);
	
	bSizer271 = new wxBoxSizer (wxHORIZONTAL);
	
	m_textCtrl110 = new wxTextCtrl (sbSizer13->GetStaticBox (),
												IDC_EditNumberFeatures,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (60, -1),
												0,
												wxTextValidator (wxFILTER_NUMERIC));
	SetUpToolTip (m_textCtrl110, IDS_ToolTip355);
   bSizer271->Add (m_textCtrl110, 0, wxALIGN_CENTER|wxLEFT, 15);
	
	m_staticText260 = new wxStaticText (sbSizer13->GetStaticBox (),
													IDC_SetFeatures,
													wxT("1"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText260->Wrap (-1);
	SetUpToolTip (m_staticText260, IDS_ToolTip355);
   bSizer271->Add (m_staticText260, 0, wxALIGN_CENTER|wxLEFT, 15);
	
	m_staticText258 = new wxStaticText (sbSizer13->GetStaticBox (),
													wxID_ANY,
													wxT("feature(s)"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText258->Wrap (-1);
	bSizer271->Add (m_staticText258, 0, wxALIGN_CENTER|wxALL, 5);
	
	sbSizer13->Add (bSizer271, 0, wxEXPAND|wxLEFT, 5);
	
	bSizer268->Add (sbSizer13, 0, wxALL|wxEXPAND, 5);
	
	wxBoxSizer* bSizer272;
	bSizer272 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText259 = new wxStaticText (this,
													wxID_ANY,
													wxT("Max number of output features:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText259->Wrap (-1);
   SetUpToolTip (m_staticText259, IDS_ToolTip350);
	bSizer272->Add (m_staticText259, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_textCtrl111 = new wxTextCtrl (this,
												IDC_MaxNumberOutputFeatures,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (70, -1),
												0,
												wxTextValidator (wxFILTER_NUMERIC));
   SetUpToolTip (m_textCtrl111, IDS_ToolTip350);
	bSizer272->Add (m_textCtrl111, 0, wxALIGN_CENTER|wxALL, 5);
	
	bSizer268->Add (bSizer272, 0, wxEXPAND, 5);
	
	bSizer267->Add (bSizer268, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxTOP, 12);
	
	wxBoxSizer* bSizer269;
	bSizer269 = new wxBoxSizer (wxVERTICAL);
	
	wxStaticBoxSizer* sbSizer14;
	sbSizer14 = new wxStaticBoxSizer (new wxStaticBox (this,
																		wxID_ANY,
																		wxT("Band Grouping Method")),
													wxVERTICAL);
	
	m_radioBtn33 = new wxRadioButton (sbSizer14->GetStaticBox (),
													IDC_Uniform,
													wxT("Uniform"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_radioBtn33, IDS_ToolTip356);
	sbSizer14->Add (m_radioBtn33, 0, 0, 5);
	
	m_radioBtn34 = new wxRadioButton (sbSizer14->GetStaticBox (),
													IDC_TopDownMethod,
													wxT("Top-down only method"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	SetUpToolTip (m_radioBtn34, IDS_ToolTip346);
   sbSizer14->Add (m_radioBtn34, 0, 0, 5);
	
	m_radioBtn35 = new wxRadioButton (sbSizer14->GetStaticBox (),
													IDC_TopDownBottomUpMethod,
													wxT("Top-down/Bottom-up method"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	SetUpToolTip (m_radioBtn35, IDS_ToolTip347);
   sbSizer14->Add (m_radioBtn35, 0, wxBOTTOM, 10);
	
	wxBoxSizer* bSizer273;
	bSizer273 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText261 = new wxStaticText (sbSizer14->GetStaticBox (),
													IDC_TopDownThresholdPrompt,
													wxT("Top-down threshold (%):  "),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText261->Wrap (-1);
   SetUpToolTip (m_staticText261, IDS_ToolTip348);
	bSizer273->Add (m_staticText261, 0, wxALIGN_CENTER, 5);
	
	m_textCtrl112 = new wxTextCtrl (sbSizer14->GetStaticBox (),
												IDC_TopDownThreshold,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (70, -1),
												0,
												wxTextValidator (wxFILTER_NUMERIC));
	SetUpToolTip (m_textCtrl112, IDS_ToolTip348);
   bSizer273->Add (m_textCtrl112, 0, wxALIGN_CENTER, 5);
	
	sbSizer14->Add (bSizer273, 0, wxLEFT, 15);
	
	wxBoxSizer* bSizer274;
	bSizer274 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText262 = new wxStaticText (sbSizer14->GetStaticBox (),
													IDC_BottomUpThresholdPrompt,
													wxT("Bottom-up threshold (%):"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText262->Wrap (-1);
   SetUpToolTip (m_staticText262, IDS_ToolTip349);
	bSizer274->Add (m_staticText262, 0, wxALIGN_CENTER, 5);
	
	m_textCtrl113 = new wxTextCtrl (sbSizer14->GetStaticBox (),
												IDC_BottomUpThreshold,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (70, -1),
												0,
												wxTextValidator (wxFILTER_NUMERIC));
   SetUpToolTip (m_textCtrl113, IDS_ToolTip349);
	bSizer274->Add (m_textCtrl113, 0, wxALIGN_CENTER|wxBOTTOM, 5);
	
	sbSizer14->Add (bSizer274, 1, wxEXPAND|wxLEFT, 15);
	
	wxBoxSizer* bSizer275;
	bSizer275 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText263 = new wxStaticText (sbSizer14->GetStaticBox (),
													IDC_BothChoicesLabel1,
													wxT("Both choices for odd groups up thru:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText263->Wrap (-1);
   SetUpToolTip (m_staticText263, IDS_ToolTip357);
	bSizer275->Add (m_staticText263, 0, wxLEFT, 15);
	
	wxBoxSizer* bSizer276;
	bSizer276 = new wxBoxSizer (wxHORIZONTAL);
	
	m_textCtrl114 = new wxTextCtrl (sbSizer14->GetStaticBox (),
												IDC_BothChoicesUpToValue,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (60, -1),
												0,
												wxTextValidator (wxFILTER_NUMERIC));
	SetUpToolTip (m_textCtrl114, IDS_ToolTip358);
   bSizer276->Add (m_textCtrl114, 0, wxALL, 5);
	
	m_staticText264 = new wxStaticText (sbSizer14->GetStaticBox (),
													IDC_BothChoicesLabel2,
													wxT("total features."),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText264->Wrap (-1);
	bSizer276->Add (m_staticText264, 0, wxALIGN_CENTER|wxALL, 5);
	
	bSizer275->Add (bSizer276, 0, wxEXPAND|wxLEFT, 30);
	
	sbSizer14->Add (bSizer275, 0, wxEXPAND, 5);
	
	bSizer269->Add (sbSizer14, 0, wxALL|wxEXPAND, 5);

	bSizer267->Add (bSizer269, 0, wxEXPAND|wxRIGHT|wxTOP, 12);
	
	bVSizerMain->Add (bSizer267, 0);
	
	CreateStandardButtons (bVSizerMain);
	
	SetSizer (bVSizerMain);
	Layout ();
	Centre (wxBOTH);
   SetSizerAndFit (bVSizerMain);
	
}	// end "CreateControls"



Boolean CMProjectionPursuitDialog::DoDialog (
				ProjectionPursuitSpecsPtr		projectionPursuitSpecsPtr)

{  
	SInt16								returnCode;
	Boolean								continueFlag = FALSE;

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
																																								
	m_projectionPursuitSpecsPtr = projectionPursuitSpecsPtr;

	returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{
		continueFlag = TRUE; 
		ProjectionPursuitDialogOK (m_projectionPursuitSpecsPtr,
												m_numericalOptimizationFlag,
												m_algorithmCode+1,
												m_initialGroupingCode+1,
												m_initialNumberFeatures,
												m_firstStageMethod+1,
												m_optimizeThreshold,
												m_topDownThreshold,
												m_bottomUpThreshold,
												m_maximumNumberFeatures,
												m_bothOddChoicesNumberFeatures);

		}	// end "if (returnCode == IDOK)"
			
	return (continueFlag);
		
}	// end "DoDialog"



void CMProjectionPursuitDialog::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	SInt16								algorithmCode,
											firstStageMethod,
											initialGroupingCode;

	Boolean								numericalOptimizationFlag;

	
	ProjectionPursuitDialogInitialize (this,
											 		m_projectionPursuitSpecsPtr,
													&numericalOptimizationFlag,
													&algorithmCode,
													&initialGroupingCode,
													&m_initialNumberFeatures,
													&m_finalNumberFeatures,
													&firstStageMethod,
													&m_optimizeThreshold,
													&m_topDownThreshold,
													&m_bottomUpThreshold,
													&m_maximumNumberFeatures,
													&m_bothOddChoicesNumberFeatures);
													
	m_numericalOptimizationFlag = numericalOptimizationFlag;
	m_algorithmCode = algorithmCode - 1;
	m_initialGroupingCode = initialGroupingCode - 1;
	m_firstStageMethod = firstStageMethod - 1;

	if (TransferDataToWindow ())
		PositionDialogWindow (); 
	
			// Set default text selection to first edit text item	
		                                                     
	SelectDialogItemText (this, IDC_MaxNumberOutputFeatures, 0, SInt16_MAX);

}	// end "OnInitDialog"



void CMProjectionPursuitDialog::OnBnClickedAlgorithm (
				wxCommandEvent& 					event)

{
   wxRadioButton*  projpursuit = (wxRadioButton*)FindWindow (IDC_PP_Algorithm);
   wxRadioButton*  featuresel = (wxRadioButton*)FindWindow (IDC_FS_Algorithm);
   if (projpursuit->GetValue())
   	{
      m_algorithmCode = 0;
      featuresel->SetValue (false);
		
   	}	// end "if (projpursuit->GetValue())"
	
   if (featuresel->GetValue())
   	{
      projpursuit->SetValue (false);
      m_algorithmCode = 1;
		
   	}	// end "if (featuresel->GetValue())"
   
	ProjectionPursuitDialogAlgorithm (
									this, m_algorithmCode+1, m_numericalOptimizationFlag);

	ProjectionPursuitDialogFSOptions (
									this, m_algorithmCode+1, m_firstStageMethod+1);

}	// end "OnBnClickedAlgorithm"



void CMProjectionPursuitDialog::OnBnClickedNumericaloptimization (
				wxCommandEvent& 					event)

{
   wxCheckBox*  numopt= (wxCheckBox*)FindWindow (IDC_NumericalOptimization);
   m_numericalOptimizationFlag = numopt->GetValue ();
   
	ProjectionPursuitDialogOptimizeFlag (this, m_numericalOptimizationFlag);

}	// end "OnBnClickedNumericaloptimization"



void CMProjectionPursuitDialog::OnBnClickedInitialBandGrouping (
				wxCommandEvent& 					event)

{
   wxRadioButton* uniformgroup = (wxRadioButton*)FindWindow (IDC_UniformGrouping);
   if (uniformgroup->GetValue())
      m_initialGroupingCode = 0;
	
   wxRadioButton* lastfinalgroup = (wxRadioButton*)FindWindow (IDC_LastFinalGrouping);
   if (lastfinalgroup->GetValue())
      m_initialGroupingCode = 1;
	
	ProjectionPursuitDialogInitialGrouping (this, m_initialGroupingCode+1);
   bSizer271->Layout ();
}	// end "OnBnClickedInitialBandGrouping"



void CMProjectionPursuitDialog::OnCheckValues (
				wxCommandEvent& 					event)

{
   CheckValues (IDC_OptimizationThreshold, 0 , 100, 1);
   CheckValues (IDC_TopDownThreshold, 0 , 100, 1);
   CheckValues (IDC_BottomUpThreshold, 0 , 100, 1);
   CheckValues (
			IDC_BothChoicesUpToValue, 0, gProjectInfoPtr->numberStatisticsChannels, 0);
   CheckValues (
   		IDC_MaxNumberOutputFeatures, 1, gProjectInfoPtr->numberStatisticsChannels, 0);
   CheckValues (
   		IDC_EditNumberFeatures, 1, gProjectInfoPtr->numberStatisticsChannels, 0);

}	// end "OnCheckValues"



void CMProjectionPursuitDialog::CheckValues (
				int 									controlID,
				SInt16 								minValue,
				SInt16 								maxValue,
				SInt16 								flag)

{
   		// Description of flag: 0 = int; 1 = double
	
   wxTextCtrl* textControlPtr = wxDynamicCast (FindWindow (controlID), wxTextCtrl);
        
   if (textControlPtr != NULL)
   	{
      wxString value = textControlPtr->GetValue ();

      if (value.Length() > 0)
      	{
         double outputValue;
         value.ToDouble (&outputValue);
         if (outputValue > maxValue || outputValue < minValue)
         	{
            if (outputValue < minValue)
               outputValue = minValue;
					
            else
               outputValue = maxValue;
            
            if (flag == 0) // input is int
               textControlPtr->ChangeValue (
               								wxString::Format (wxT("%.0f"), outputValue));
            else
               textControlPtr->ChangeValue (
               								wxString::Format (wxT("%.2f"), outputValue));
				
				}	// end "valueDouble > maxValue || valueDouble < minValue"

      	}	// end "if (m_value.Length() > 0)"

   	}	// end "if (textControlPtr != NULL)"

}	// end "CheckValues"



void CMProjectionPursuitDialog::OnBnClickedBandGroupingMethod (
				wxCommandEvent& 					event)

{
   wxRadioButton*  uniform = (wxRadioButton*)FindWindow (IDC_Uniform);
   wxRadioButton*  topdown = (wxRadioButton*)FindWindow (IDC_TopDownMethod);
   wxRadioButton*  topdownbottomup = (wxRadioButton*)FindWindow (IDC_TopDownBottomUpMethod);
   
   if (uniform->GetValue())
   	{
      m_firstStageMethod = 0;
      topdown->SetValue (false);
      topdownbottomup->SetValue (false);
		
   	}	// end "if (uniform->GetValue())"
	
   if (topdown->GetValue())
   	{
      m_firstStageMethod = 1;
      uniform->SetValue (false);
      topdownbottomup->SetValue (false);
		
   	}	// end "if (topdown->GetValue())"
	
   if (topdownbottomup->GetValue() == true)
   	{
      m_firstStageMethod = 2;
      topdown->SetValue (false);
      uniform->SetValue (false);
		
   	}	// end "if (topdownbottomup->GetValue())"
   
	ProjectionPursuitDialogFSMethod (this, m_firstStageMethod+1);

}	// end "OnBnClickedBandGroupingMethod"



bool CMProjectionPursuitDialog::TransferDataFromWindow ()

{
   		// For radio button selection values
	
   wxRadioButton*  projpursuit = (wxRadioButton*)FindWindow (IDC_PP_Algorithm);
   if (projpursuit->GetValue() == true)
      m_algorithmCode = 0;
	
   else	// projpursuit->GetValue() == false
      m_algorithmCode = 1;
	
   wxRadioButton*  uniformgroup = (wxRadioButton*)FindWindow (IDC_UniformGrouping);
   if (uniformgroup->GetValue() == true)
      m_initialGroupingCode = 0;
	
   else	// uniformgroup->GetValue() == false
      m_initialGroupingCode = 1;
	
   wxRadioButton*  uniform = (wxRadioButton*)FindWindow (IDC_Uniform);
   wxRadioButton*  topdown = (wxRadioButton*)FindWindow (IDC_TopDownMethod);
	if (uniform->GetValue() == true)
      m_firstStageMethod = 0;
	
   else if (topdown->GetValue() == true)
      m_firstStageMethod = 1;
	
   else	// uniform->GetValue() == false && topdown->GetValue() == false
      m_firstStageMethod = 2;
	
   wxCheckBox*  numopt= (wxCheckBox*)FindWindow (IDC_NumericalOptimization);
   m_numericalOptimizationFlag = numopt->GetValue ();
	
   		// For textCtrl values
	
   wxTextCtrl* optimizeThres = (wxTextCtrl*)FindWindow (IDC_OptimizationThreshold);
   wxString optimizeThresDouble = optimizeThres->GetValue ();
   optimizeThresDouble.ToDouble (&m_optimizeThreshold);
	
   wxTextCtrl* topdownThres = (wxTextCtrl*)FindWindow (IDC_TopDownThreshold);
   wxString topdownThresDouble = topdownThres->GetValue ();
   topdownThresDouble.ToDouble (&m_topDownThreshold);
	
   wxTextCtrl* bottomupThres = (wxTextCtrl*)FindWindow (IDC_BottomUpThreshold);
   wxString bottomupThresDouble = bottomupThres->GetValue ();
   bottomupThresDouble.ToDouble (&m_bottomUpThreshold);
	
   wxTextCtrl* bothchoice = (wxTextCtrl*)FindWindow (IDC_BothChoicesUpToValue);
   wxString bothchoiceString = bothchoice->GetValue ();
   m_bothOddChoicesNumberFeatures = wxAtoi (bothchoiceString);
	
   wxTextCtrl* editNumFeature = (wxTextCtrl*)FindWindow (IDC_EditNumberFeatures);
   wxString editNumFeatureString = editNumFeature->GetValue ();
   m_initialNumberFeatures = wxAtoi (editNumFeatureString);
	
   wxTextCtrl* maxOutputFeature =
										(wxTextCtrl*)FindWindow (IDC_MaxNumberOutputFeatures);
	wxString maxOutputFeatureString = maxOutputFeature->GetValue ();
   m_maximumNumberFeatures = wxAtoi (maxOutputFeatureString);
	
   return true;
	
}	// end "TransferDataFromWindow"



bool CMProjectionPursuitDialog::TransferDataToWindow ()

{
   wxRadioButton*  projpursuit = (wxRadioButton*)FindWindow (IDC_PP_Algorithm);
   wxRadioButton*  featuresel = (wxRadioButton*)FindWindow (IDC_FS_Algorithm);
   if (m_algorithmCode == 0)
   	{
      projpursuit->SetValue (true);
      featuresel->SetValue (false);
		
   	}	// end "if (m_algorithmCode == 0)"
	
	else	// m_algorithmCode != 0
		{
      projpursuit->SetValue (false);
      featuresel->SetValue (true);
		
		}	// end "else m_algorithmCode != 0"
	
   wxRadioButton* uniformgroup = (wxRadioButton*)FindWindow (IDC_UniformGrouping);
   wxRadioButton* lastfinalgroup = (wxRadioButton*)FindWindow (IDC_LastFinalGrouping);
   if (m_initialGroupingCode == 0)
   	{
      uniformgroup->SetValue (true);
      lastfinalgroup->SetValue (false);
		
   	}	// end "if (m_initialGroupingCode == 0)"
	
	else	// m_initialGroupingCode != 0
		{
      uniformgroup->SetValue (false);
      lastfinalgroup->SetValue (true);
		
   	}	// end "else m_initialGroupingCode != 0)
	
   wxRadioButton* uniform = (wxRadioButton*)FindWindow (IDC_Uniform);
   wxRadioButton* topdown = (wxRadioButton*)FindWindow (IDC_TopDownMethod);
   wxRadioButton* topdownbottomup =
   									(wxRadioButton*)FindWindow (IDC_TopDownBottomUpMethod);
	if (m_firstStageMethod == 0)
   	{
      uniform->SetValue (true);
      topdown->SetValue (false);
      topdownbottomup->SetValue (false);
		
   	}	// end "if (m_firstStageMethod == 0)"
	
	else if (m_firstStageMethod == 1)
		{
      uniform->SetValue (false);
      topdown->SetValue (true);
      topdownbottomup->SetValue (false);
		
   	}	// end "else if (m_firstStageMethod == 1)
	
   else	// m_firstStageMethod != 0 or 1
   	{
      uniform->SetValue (false);
      topdown->SetValue (false);
      topdownbottomup->SetValue (true);
		
   	}	// end "else m_firstStageMethod != 0 or 1"
	
   wxCheckBox*  numopt= (wxCheckBox*)FindWindow (IDC_NumericalOptimization);
   numopt->SetValue (m_numericalOptimizationFlag);
	
   wxTextCtrl* optimizeThres = (wxTextCtrl*)FindWindow (IDC_OptimizationThreshold);
   optimizeThres->ChangeValue (wxString::Format (wxT("%.2f"), m_optimizeThreshold));
	
   wxTextCtrl* topdownThres = (wxTextCtrl*)FindWindow (IDC_TopDownThreshold);
   topdownThres->ChangeValue (wxString::Format (wxT("%.2f"), m_topDownThreshold));
	
	wxTextCtrl* bottomupThres = (wxTextCtrl*)FindWindow (IDC_BottomUpThreshold);
   bottomupThres->ChangeValue (wxString::Format (wxT("%.2f"), m_bottomUpThreshold));
	
   wxTextCtrl* bothchoice = (wxTextCtrl*)FindWindow (IDC_BothChoicesUpToValue);
   bothchoice->ChangeValue (
   							wxString::Format (wxT("%i"), m_bothOddChoicesNumberFeatures));
	
   wxTextCtrl* editNumFeature = (wxTextCtrl*)FindWindow (IDC_EditNumberFeatures);
   editNumFeature->ChangeValue (wxString::Format (wxT("%i"), m_initialNumberFeatures));
	
   wxStaticText* lastNumFeature = (wxStaticText*)FindWindow (IDC_SetFeatures);
	lastNumFeature->SetLabel (wxString::Format (wxT("%i"), m_initialNumberFeatures));
	
   wxTextCtrl* maxOutputFeature = (wxTextCtrl*)FindWindow (IDC_MaxNumberOutputFeatures);
   maxOutputFeature->ChangeValue (
   									wxString::Format (wxT("%i"), m_maximumNumberFeatures));
	
   return true;
	
}	// end "TransferDataToWindow"
