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
//	File:						xBiplotDialog.cpp : class implementation file
//	Class Definition:		xBiplotDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/04/2019
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the CMBiPlotDialog
//								class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xBiplotDialog.h" 


IMPLEMENT_DYNAMIC_CLASS (CMBiPlotDialog, CMDialog)

BEGIN_EVENT_TABLE (CMBiPlotDialog, CMDialog)
   EVT_BUTTON (IDEntireImage, CMBiPlotDialog::ToEntireImage)
   EVT_BUTTON (IDSelectedImage, CMBiPlotDialog::ToSelectedImage)

   EVT_CHECKBOX (IDC_FeatureTransformation, CMBiPlotDialog::OnFeatureTranformation)
   EVT_CHECKBOX (IDC_TestAreas, CMBiPlotDialog::OnTestArea)
   EVT_CHECKBOX (IDC_Training, CMBiPlotDialog::OnTrainArea)
   EVT_CHECKBOX (IDC_ImageArea, CMBiPlotDialog::OnImageArea)
   EVT_CHECKBOX (IDC_ThresholdPixelCheck, CMBiPlotDialog::OnThresholdPixelFlag)
   EVT_CHECKBOX (IDC_CreateNewGraphWindow, CMBiPlotDialog::OnCreateNewWindowFlag)
   EVT_CHECKBOX (IDC_DisplayPixels, CMBiPlotDialog::OnDisplayPixelAsSymbol)
   EVT_CHECKBOX (IDC_OutlineClassAsEllipse, CMBiPlotDialog::OnOutlineClassAsEllipse)

   #if defined multispec_wxlin
      EVT_COMBOBOX (IDC_ClassCombo, CMBiPlotDialog::OnClassComboSelendok)
      //EVT_COMBOBOX (IDC_ClassWeightsCombo, CMBiPlotDialog::OnClassWeightsCombo)
   #endif
   #if defined multispec_wxmac
      EVT_CHOICE (IDC_ClassCombo, CMBiPlotDialog::OnClassComboSelendok)
      //EVT_CHOICE (IDC_ClassWeightsCombo, CMBiPlotDialog::OnClassWeightsCombo)
   #endif

   EVT_COMBOBOX_CLOSEUP (IDC_ClassCombo, CMBiPlotDialog::OnClassComboCloseUp)
   EVT_COMBOBOX_DROPDOWN (IDC_ClassCombo, CMBiPlotDialog::OnClassComboDropDown)

   EVT_INIT_DIALOG (CMBiPlotDialog::OnInitDialog)
   EVT_TEXT (IDC_HorizontalAxis, CMBiPlotDialog::OnChangeHorizontalAxis)
   EVT_TEXT (IDC_VerticalAxis, CMBiPlotDialog::OnChangeVerticalAxis)
   EVT_TEXT (IDC_ColumnEnd, CMBiPlotDialog::CheckColumnEnd)
   EVT_TEXT (IDC_ColumnStart, CMBiPlotDialog::CheckColumnStart)
   EVT_TEXT (IDC_LineEnd, CMBiPlotDialog::CheckLineEnd)
   EVT_TEXT (IDC_LineStart, CMBiPlotDialog::CheckLineStart)
   EVT_TEXT (IDC_LineInterval, CMBiPlotDialog::CheckLineInterval)
   EVT_TEXT (IDC_ColumnInterval, CMBiPlotDialog::CheckColumnInterval)
END_EVENT_TABLE ()



CMBiPlotDialog::CMBiPlotDialog (
				wxWindow* 							parent,
				wxWindowID 							id,
				const wxString& 					title)
		: CMDialog (CMBiPlotDialog::IDD, parent, title)

{
	m_classWeightsPtr = NULL;
	m_checkChannelStatisticsFlag = FALSE;
	m_checkClassesPopUpFlag = FALSE;
	m_createNewGraphicsWindowFlag = FALSE;
   
	m_checkFeatureTransformFlag = FALSE;
	m_featureTransformAllowedFlag = FALSE;
	m_featureTransformationFlag = FALSE;
   
	m_thresholdPercent = 0;
	m_saveThresholdPercent = 0.;
	m_maxChannelFeatureNum = 0;
	
	m_displayPixelCode = 0;
	m_outlineClassCode = 0;
	m_plotDataCode = 0;
	m_newXAxisFeature = 0;
	m_newYAxisFeature = 0;
	m_initializedFlag = CMDialog::m_initializedFlag;
   
	m_trainingAreaFlag = FALSE;
	m_imageAreaFlag = FALSE;
	m_testFlag = FALSE;

	if (gBiPlotDataSpecsPtr->projectFlag)
		m_initializedFlag = GetDialogLocalVectors (NULL,
                                                    NULL,
                                                    &m_classListPtr,
                                                    NULL,
                                                    &m_classWeightsPtr,
                                                    NULL,
                                                    NULL,
                                                    NULL);
   
	if (m_initializedFlag)
		CreateControls ();
   
	/*   
   if (m_initializedFlag)
		{
				// Get memory for the local class weights vector and initialize it.				
      if (gBiPlotDataSpecsPtr->projectFlag)
         m_classWeightsPtr = GetTempClassWeightsPtr ();
		//m_initializedFlag = (m_classWeightsPtr != NULL);
		}
		
   if (m_initializedFlag && gBiPlotDataSpecsPtr->projectFlag) 
		{
				// Get memory for the local class list vector.

      m_classListPtr = (UInt16*)MNewPointer (
              (UInt32) gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));

      m_initializedFlag = (m_classListPtr != NULL);

		}	// end "if (m_initializedFlag && ...->projectFlag)"
	*/
	 
}	// end "CMBiPlotDialog"



CMBiPlotDialog::~CMBiPlotDialog ()

{
	ReleaseDialogLocalVectors (NULL,
										NULL,
										m_classListPtr,
										NULL,
										m_classWeightsPtr,
										NULL,
										NULL,
										NULL);
   //m_classListPtr = CheckAndDisposePtr (m_classListPtr);
	
}	// end "~CMBiPlotDialog"



void CMBiPlotDialog::CreateControls () 

{
	this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxBoxSizer* bVSizerMain = new wxBoxSizer (wxVERTICAL);
	
	bSizer278 = new wxBoxSizer (wxHORIZONTAL);
	
	wxBoxSizer* bSizer279;
	bSizer279 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer281;
	bSizer281 = new wxBoxSizer (wxVERTICAL);
	
	
	bSizer282 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText265 = new wxStaticText (this, 
													IDC_ChannelsPrompt, 
													wxT("Channels to plot:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	//m_staticText265->Wrap (-1);
	bSizer282->Add (m_staticText265, 0, wxALL, 5);
   
   m_staticText26 = new wxStaticText (this, 
													IDD_FeatureSelection, 
													wxT("Features to plot:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	//m_staticText26->Wrap (-1);
	bSizer282->Add (m_staticText26, 0, wxALL, 5);

	m_staticText26->Hide ();
   
	bSizer281->Add (bSizer282, 0, wxEXPAND, 5);
	
	wxFlexGridSizer* fgSizer13;
	fgSizer13 = new wxFlexGridSizer (2, 2, 0, 0);
	fgSizer13->SetFlexibleDirection (wxBOTH);
	fgSizer13->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	m_staticText267 = new wxStaticText (this, 
													wxID_ANY, 
													wxT("Horizontal axis:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText267->Wrap (-1);
   SetUpToolTip (m_staticText267, IDS_ToolTip233);
	//fgSizer13->Add (m_staticText267, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	fgSizer13->Add (m_staticText267, 0, wxALL, 5);
   m_textCtrl115 = new wxTextCtrl (this, 
												IDC_HorizontalAxis, 
												wxEmptyString, 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
	SetUpToolTip (m_textCtrl115, IDS_ToolTip233);
   
   fgSizer13->Add (m_textCtrl115, 0, wxALIGN_CENTER, 5);
	
	m_staticText268 = new wxStaticText (this, 
													wxID_ANY, 
													wxT("Vertical axis:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText268->Wrap (-1);
   SetUpToolTip (m_staticText268, IDS_ToolTip234);
	fgSizer13->Add (m_staticText268, 0, wxALL, 5);
	m_textCtrl116 = new wxTextCtrl (this, 
												IDC_VerticalAxis, 
												wxEmptyString, 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
   SetUpToolTip (m_textCtrl116, IDS_ToolTip234);
	fgSizer13->Add (m_textCtrl116, 0, wxALIGN_CENTER|wxBOTTOM|wxTOP, 5);
	
	bSizer281->Add (fgSizer13, 1, wxEXPAND, 5);
	
	m_checkBox58 = new wxCheckBox (this, 
												IDC_FeatureTransformation, 
												wxT("Use feature transformation"), 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
   SetUpToolTip (m_checkBox58, IDS_ToolTip235);
	bSizer281->Add (m_checkBox58, 0, wxLEFT, 15);
   
	bSizer279->Add (bSizer281, 0, wxEXPAND, 5);
	
	wxBoxSizer* bSizer284;
	bSizer284 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText269 = new wxStaticText (this, 
													wxID_ANY, 
													wxT("Pixels to Plot:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText269->Wrap (-1);
	bSizer284->Add (m_staticText269, 0, wxALL, 5);
	
	m_checkBox59 = new wxCheckBox (this, 
												IDC_Training, 
												wxT("Training"), 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
   SetUpToolTip (m_checkBox59, IDS_ToolTip236);
	bSizer284->Add (m_checkBox59, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxRIGHT, 5));
	
	/* Not used.
	m_staticText270 = new wxStaticText (this, 
													IDC_Ellipse, 
													wxT("Ellipse"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText270->Wrap (-1);
	fgSizer14->Add (m_staticText270, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	*/
	m_checkBox60 = new wxCheckBox (this, 
												IDC_TestAreas, 
												wxT("Test"), 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
   SetUpToolTip (m_checkBox60, IDS_ToolTip237);
	bSizer284->Add (m_checkBox60, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxRIGHT, 5));
	
	/* Not used.
	m_staticText271 = new wxStaticText (this, 
													IDC_Symbols, 
													wxT("Symbols"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText271->Wrap (-1);
	fgSizer14->Add (m_staticText271, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	bSizer284->Add (fgSizer14, 0, wxEXPAND, 5);
	*/
	m_checkBox61 = new wxCheckBox (this, 
												IDC_ImageArea, 
												wxT("Area"), 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
   SetUpToolTip (m_checkBox61, IDS_ToolTip238);
	bSizer284->Add (m_checkBox61, 0, wxALL, 5);
	
	wxStaticBoxSizer* sbSizer8;
	wxStaticBox* areaStaticBox = new wxStaticBox (this, 
																	IDC_LineColFrame, 
																	wxT("Area to Plot"),
																	wxDefaultPosition,
																	wxDefaultSize,
																	wxTAB_TRAVERSAL);
   sbSizer8 = new wxStaticBoxSizer (areaStaticBox, wxHORIZONTAL);
	
	CreateLineColumnControls (sbSizer8);
	
	bSizer284->Add (sbSizer8, wxSizerFlags(0).Expand().Border(wxLEFT, 15));
	
	bSizer279->Add (bSizer284, wxSizerFlags(1).Expand());
	
	bSizer278->Add (bSizer279,
							wxSizerFlags(0).Expand().Border(wxBOTTOM|wxLEFT|wxTOP, 12));
	
	wxBoxSizer* bSizer280;
	bSizer280 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText278 = new wxStaticText (this, 
													wxID_ANY, wxT("Options"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText278->Wrap (-1);
	bSizer280->Add (m_staticText278, 0, wxALL, 5);
	
	m_checkBox62 = new wxCheckBox (this, 
												IDC_DisplayPixels, 
												wxT("Display pixels as symbols"), 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
   SetUpToolTip (m_checkBox62, IDS_ToolTip239);
	bSizer280->Add (m_checkBox62, 0, wxLEFT, 15);
	
	m_checkBox63 = new wxCheckBox (this, 
												IDC_OutlineClassAsEllipse, 
												wxT("Outline classes as ellipses"), 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
   SetUpToolTip (m_checkBox63, IDS_ToolTip240);
	bSizer280->Add (m_checkBox63, 0, wxLEFT, 15);
	
	wxBoxSizer* bSizer289;
	bSizer289 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText279 = new wxStaticText (this, 
													IDC_ThresholdLevelPrompt, 
													wxT("Threshold Level:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText279->Wrap (-1);
   SetUpToolTip (m_staticText279, IDS_ToolTip241);
	bSizer289->Add (m_staticText279, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_textCtrl129 = new wxTextCtrl (this, 
												IDC_ThresholdLevel, 
												wxEmptyString, 
												wxDefaultPosition, 
												wxSize (70,-1), 
												0);
   SetUpToolTip (m_textCtrl129, IDS_ToolTip241);
	bSizer289->Add (m_textCtrl129, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_staticText280 = new wxStaticText (this, 
													IDC_PercentPrompt, 
													wxT("%"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText280->Wrap (-1);
	bSizer289->Add (m_staticText280, 0, wxALIGN_CENTER, 5);
	
	
	bSizer280->Add (bSizer289, 0, wxEXPAND|wxLEFT, 15);
	
	m_checkBox64 = new wxCheckBox (this, 
												IDC_ThresholdPixelCheck, 
												wxT("Threshold pixels"), 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
	bSizer280->Add (m_checkBox64, 0, wxLEFT, 15);
	
	wxBoxSizer* bSizer290;
	bSizer290 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText281 = new wxStaticText (this, 
													IDC_ClassPrompt, 
													wxT("Classes:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText281->Wrap (-1);
   SetUpToolTip (m_staticText281, IDS_ToolTip103);
	bSizer290->Add (m_staticText281, 0, wxALIGN_CENTER|wxALL, 5);

	GetAllSubsetMenuControl (kClassesMenu,
										this,
										IDC_ClassCombo,
										100,
										IDS_ToolTip103);
	bSizer290->Add (m_classesCtrl, 0, wxALL, 5);
	
	bSizer280->Add (bSizer290, 0, wxEXPAND|wxLEFT, 15);
	
	wxBoxSizer* bSizer291;
	bSizer291 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText282 = new wxStaticText (this, 
													IDC_SymbolPrompt, 
													wxT("Symbols:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText282->Wrap (-1);
   SetUpToolTip (m_staticText282, IDS_ToolTip83);
	bSizer291->Add (m_staticText282, 0, wxALIGN_CENTER|wxALL, 5);
   /* User defined not yet implemented
	m_comboBox52 = new wxComboBox (this, 
												IDC_SymbolCombo, 
												wxT("Combo!"), 
												wxDefaultPosition, 
												wxDefaultSize, 
												0, 
												NULL, 
												wxCB_READONLY);
	m_comboBox52->Append (wxT("Default"));
	m_comboBox52->Append (wxT("User defined..."));
   bSizer291->Add (m_comboBox52, 0, wxALIGN_CENTER|wxALL, 5);
   */
   m_staticText284 = new wxStaticText (this, 
													IDC_SymbolCombo, 
													wxT("Default"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
   SetUpToolTip (m_staticText284, IDS_ToolTip83);
	bSizer291->Add (m_staticText284, 0, wxALIGN_CENTER|wxALL, 5);
	
	bSizer280->Add (bSizer291, 0, wxEXPAND|wxLEFT, 15);
	
	wxBoxSizer* bSizer292;
	bSizer292 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText283 = new wxStaticText (this, 
													IDC_WeightsPrompt, 
													wxT("Class weights:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText283->Wrap (-1);
	bSizer292->Add (m_staticText283, 0, wxALIGN_CENTER|wxALL, 5);

	GetEqualUnequalMenuControl (kWeightsMenu,
											this,
											IDC_WeightsCombo,
											120,
											IDS_ToolTip157);
	bSizer292->Add (m_weightsCtrl, 0, wxALL, 5);
	
	bSizer280->Add (bSizer292, 0, wxEXPAND|wxLEFT, 15);
	
	m_checkBox65 = new wxCheckBox (this, 
												IDC_CreateNewGraphWindow, 
												wxT("Create new graphics window"), 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
	m_checkBox65->SetValue (true); 
	bSizer280->Add (m_checkBox65, 0, wxLEFT, 15);

   bSizer278->Add (bSizer280,
							wxSizerFlags(0).Border(wxRIGHT|wxTOP, 12));
	
   bVSizerMain->Add (bSizer278, wxSizerFlags(0));
	
	CreateStandardButtons (bVSizerMain);
	
	SetSizer (bVSizerMain);
	Layout ();
	Centre (wxBOTH);
   
   //SetSizerAndFit (bVSizerMain);
	
}	// end "CreateControls"



Boolean CMBiPlotDialog::DoDialog ()

{
   SInt16		returnCode;
	
	Boolean		continueFlag = FALSE; 
	
	
			// Make sure intialization has been completed.		
								                         
	if (!m_initializedFlag)
																					return (continueFlag);
   
	returnCode = ShowModal (); 
	
	if (returnCode == wxID_OK)
		{
		continueFlag = TRUE;

				// Area (or) line/column interval to use for pixels to be graphed.

		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;

		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;
		
      BiPlotDataDialogOK (this,
									gBiPlotDataSpecsPtr,
									&m_dialogSelectArea,
									m_newXAxisFeature,
									m_newYAxisFeature,
									m_featureTransformationFlag,
									m_createNewGraphicsWindowFlag,
									m_plotDataCode,
									m_displayPixelCode,
									m_outlineClassCode,
									m_saveThresholdPercent,
									m_thresholdFlag,
									m_classSelection,
									m_localNumberClasses,
									m_classListPtr,
									m_symbolSelection,
									m_localSymbolsPtr,
									m_classWeightsSelection + 1,
									m_classWeightsPtr); 
      
		}	// end "if (returnCode == wxID_OK)"
   
	return (continueFlag);
	
}	// end "DoDialog"



void CMBiPlotDialog::OnChangeHorizontalAxis (
				wxCommandEvent&					event)

{     
   m_checkChannelStatisticsFlag = TRUE;
	
}	// end "OnChangeHorizontalAxis"



void CMBiPlotDialog::OnChangeVerticalAxis (
				wxCommandEvent&					event)

{     
   m_checkChannelStatisticsFlag = TRUE;
	
}	// end "OnChangeVerticalAxis"



void CMBiPlotDialog::OnCreateNewWindowFlag (
				wxCommandEvent&					event)

{
   wxCheckBox* createNewWindow = (wxCheckBox*)FindWindow (IDC_CreateNewGraphWindow);
   m_createNewGraphicsWindowFlag = createNewWindow->GetValue ();
   
}	// end "OnCreateNewWindowFlag"



void CMBiPlotDialog::OnDisplayPixelAsSymbol (
				wxCommandEvent&					event)

{
   wxCheckBox* displayPixel = (wxCheckBox*)FindWindow (IDC_DisplayPixels);
   if (displayPixel->GetValue () == 0)
      m_displayPixelCode &= (kPixelsAsSymbols+kPixelsAsColor);
	
   else
      m_displayPixelCode |= kDisplayPixels;

}	// end "OnDisplayPixelAsSymbol"



void CMBiPlotDialog::OnFeatureTranformation (
				wxCommandEvent&					event)

{    
   wxCheckBox* featureTrans = (wxCheckBox*)FindWindow (IDC_FeatureTransformation);
   
   m_featureTransformationFlag = featureTrans->GetValue ();
   if (m_featureTransformationFlag)
		{
      MHideDialogItem (this, IDC_ChannelsPrompt);
      MShowDialogItem (this, IDD_FeatureSelection);
      
		}	// end "if (m_featureTransformationFlag)"
		
	else	// !m_featureTransformationFlag
		{
      MShowDialogItem (this, IDC_ChannelsPrompt);
      MHideDialogItem (this, IDD_FeatureSelection);
		
		}	// end "else !m_featureTransformationFlag"
   
   bSizer282->Layout ();
	
}	// end "OnFeatureTranformation"



void CMBiPlotDialog::OnImageArea (
				wxCommandEvent&					event)

{   
   UInt16 selectItem;
	
	m_imageAreaFlag = ((wxCheckBox *)FindWindow (IDC_ImageArea))->GetValue ();
	if (m_imageAreaFlag) 
		{
		ShowSomeAreaSelectionItems ();
		selectItem = IDC_LineStart;
		
		}	// end "if (m_areaFlag)"
	
	else	// !m_areaFlag
		{
		HideSomeAreaSelectionItems ();
		selectItem = IDC_LineInterval;
		
		}	// end "else !m_areaFlag"
	
	SelectDialogItemText (this, selectItem, 0, SInt16_MAX);
	
   if (m_imageAreaFlag ==  0)
      m_plotDataCode -= kAreaType;

	else	// m_imageAreaFlag != 0
      m_plotDataCode += kAreaType;      
   
}	// end "OnImageArea"



void CMBiPlotDialog::OnInitDialog (
				wxInitDialogEvent&				event)

{
	SInt16								classSelection,
											symbolSelection,
											weightsSelection;
	
      
	BiPlotDataDialogInitialize (this,
											gBiPlotDataSpecsPtr,
											&m_dialogSelectArea,
											gImageWindowInfoPtr,
											gActiveImageWindow,
											m_classListPtr,
											m_localSymbolsPtr,
											&m_newXAxisFeature,
											&m_newYAxisFeature,
											&m_featureTransformationFlag,
											&m_featureTransformAllowedFlag,
											&m_checkFeatureTransformFlag,
											&m_maxChannelFeatureNum,
											&m_plotDataCode,
											&m_displayPixelCode,
											&m_outlineClassCode,
											&m_thresholdFlag,
											&m_saveThresholdPercent,
											&classSelection,
											&m_localNumberClasses,
											&symbolSelection,
											&weightsSelection,
											&m_createNewGraphicsWindowFlag);
      
    m_LineStart = m_dialogSelectArea.lineStart;
    m_LineEnd = m_dialogSelectArea.lineEnd;
    m_LineInterval = m_dialogSelectArea.lineInterval;
    m_ColumnStart = m_dialogSelectArea.columnStart;
    m_ColumnEnd = m_dialogSelectArea.columnEnd;
    m_ColumnInterval = m_dialogSelectArea.columnInterval;

			// Classes to use.

    m_classSelection = classSelection;

			// Class weights.
			// Adjust for 0 base index.

    m_classWeightsSelection = weightsSelection - 1;

			// Symbols to use.
			// Adjust for 0 base index.
			// User defined symbols are not available yet.

    m_symbolSelection = symbolSelection - 1;
	 
			// Set check boxes for "pixels to plot".	      
	if (m_plotDataCode & kTrainingType) 
		m_trainingAreaFlag = TRUE;

	if (m_plotDataCode & kTestingType) 
		m_testFlag = TRUE;

	if (m_plotDataCode & kAreaType) 
		m_imageAreaFlag = TRUE;
		
			// Hide some of the area selection boxes if needed.
			
	if (!(m_plotDataCode & kAreaType))
		{
      HideSomeAreaSelectionItems ();
      
		}	// end "if (!(plotDataCode & kAreaType))" 

    //m_textCtrl115->SetBackgroundColour (wxColour (208,237,247));
    if (TransferDataToWindow ())
      PositionDialogWindow ();
	
	SelectDialogItemText (this, IDC_HorizontalAxis, 0, SInt16_MAX);
      
}	// end "OnInitDialog"



void CMBiPlotDialog::OnOutlineClassAsEllipse (
				wxCommandEvent&					event)
				
{
   wxCheckBox* outlineClass = (wxCheckBox*)FindWindow (IDC_OutlineClassAsEllipse);
	
   if (outlineClass->GetValue () == 0)
      m_outlineClassCode &= kClassesAsEllipses+kClassesAsBorders;

   else	// outlineClass->GetValue () != 0
      m_outlineClassCode |= kOutlineClasses;
		
	BiPlotDataDialogHideShowClassItems (this, m_plotDataCode, m_outlineClassCode);
	
}	// end "OnOutlineClassAsEllipse"



void CMBiPlotDialog::OnTestArea (
				wxCommandEvent&					event)
				
{ 
	wxCheckBox* test = (wxCheckBox*)FindWindow (IDC_TestAreas);   
	m_testFlag = test->GetValue ();
   if (m_testFlag == 0)
      m_plotDataCode -= kTestingType;

	else	// m_testFlag != 0
      m_plotDataCode += kTestingType;      
		
	BiPlotDataDialogHideShowClassItems (this, m_plotDataCode, m_outlineClassCode);

}	// end "OnTestArea"



void CMBiPlotDialog::OnThresholdPixelFlag (
				wxCommandEvent&					event)
				
{
   wxCheckBox* thresholdPixel = (wxCheckBox*)FindWindow (IDC_ThresholdPixelCheck);
   m_thresholdFlag = thresholdPixel->GetValue ();
   
}	// end "OnThresholdPixelFlag"



void CMBiPlotDialog::OnTrainArea (
				wxCommandEvent&					event)
				
{
   wxCheckBox* train = (wxCheckBox*)FindWindow (IDC_Training);
   m_trainingAreaFlag = train->GetValue ();
   if (m_trainingAreaFlag == 0)
      m_plotDataCode -= kTrainingType;

	else	// m_trainingAreaFlag != 0
      m_plotDataCode += kTrainingType;      
	
	BiPlotDataDialogHideShowClassItems (this, m_plotDataCode, m_outlineClassCode);
		
}	// end "OnTrainArea"



bool CMBiPlotDialog::TransferDataFromWindow ()
 
{
   SInt16								returnCode = 0;
	
	
   wxTextCtrl* horizontalAxis = (wxTextCtrl*)FindWindow (IDC_HorizontalAxis);
   wxString horizontalAxisString = horizontalAxis->GetValue ();
   m_newXAxisFeature = wxAtoi (horizontalAxisString);
	
   wxTextCtrl* verticalAxis = (wxTextCtrl*)FindWindow (IDC_VerticalAxis);
   wxString verticalAxisString = verticalAxis->GetValue ();
   m_newYAxisFeature = wxAtoi (verticalAxisString);
	
   wxTextCtrl* thresholdLevel = (wxTextCtrl*)FindWindow (IDC_ThresholdLevel);
   wxString thresholdString  = thresholdLevel->GetValue ();
   thresholdString.ToDouble (&m_thresholdPercent);
   if (m_thresholdPercent > kMaxThreshold)
      DisplayAlert (kErrorAlertID, kStopAlert, kAlertStrID,IDS_Alert115, 0, NULL);
		
	if (m_thresholdPercent >= 0 && m_thresholdPercent <= kMaxThreshold)
		m_saveThresholdPercent = m_thresholdPercent;
	
	TransferLinesColumnsFromWindow ();

   wxCheckBox* thresholdPixel = (wxCheckBox*)FindWindow (IDC_ThresholdPixelCheck);
   m_thresholdFlag = thresholdPixel->GetValue ();
	
   wxCheckBox* createNewGraphic = (wxCheckBox*)FindWindow (IDC_CreateNewGraphWindow);
   m_createNewGraphicsWindowFlag = createNewGraphic->GetValue ();
	
   wxCheckBox* train = (wxCheckBox*)FindWindow (IDC_Training);
   m_trainingAreaFlag = train->GetValue ();
	
	wxCheckBox* test = (wxCheckBox*)FindWindow (IDC_TestAreas);
   m_testFlag = test->GetValue ();
	
   wxCheckBox* area = (wxCheckBox*)FindWindow (IDC_ImageArea);
   m_imageAreaFlag = area->GetValue ();
	
   wxCheckBox* featureTrans = (wxCheckBox*)FindWindow (IDC_FeatureTransformation);
   m_featureTransformationFlag = featureTrans->GetValue ();
   
   m_classSelection = m_classesCtrl->GetSelection ();
	
   m_classWeightsSelection= m_weightsCtrl->GetSelection ();
     
   if (m_imageAreaFlag)
		returnCode = VerifyLineColumnValues (IDC_LineStart,
															IDC_ColumnStart,
															true);
	
	if (returnCode == 0 && (m_plotDataCode == 0))
		{
		SetDLogControlHilite (this, wxID_OK, 255);
		DisplayAlert (kErrorAlertID, 
							kStopAlert, 
							kAlertStrID, 
							IDS_Alert117,
							0, 
							NULL);
		SetDLogControlHilite (this, wxID_OK, 0);
							
		returnCode = 1;
								
		}	// end "if (returnCode == 0 && ..."
	
	if (returnCode == 0 &&
						!((m_displayPixelCode & kDisplayPixels) ||
													(m_outlineClassCode & kOutlineClasses)))
		{
		SetDLogControlHilite (this, wxID_OK, 255);
		DisplayAlert (kErrorAlertID, 
							kStopAlert, 
							kAlertStrID, 
							IDS_Alert151,
							0, 
							NULL);
		SetDLogControlHilite (this, wxID_OK, 0);
		
		returnCode = 1;
								
		}	// end "if (returnCode == 0 && ..."

	return (returnCode == 0);
   
}	// end "TransferDataFromWindow"



bool CMBiPlotDialog::TransferDataToWindow () 

{
   bool 									controlValue;
	
	
			// Default feature for x and y axes.	
	
   wxTextCtrl* horizontalAxis = (wxTextCtrl*)FindWindow (IDC_HorizontalAxis);
   horizontalAxis->ChangeValue (wxString::Format (wxT("%d"), m_newXAxisFeature));
   horizontalAxis->SetFocus ();
	
   wxTextCtrl* verticalAxis = (wxTextCtrl*)FindWindow (IDC_VerticalAxis);
   verticalAxis->ChangeValue (wxString::Format (wxT("%d"), m_newYAxisFeature));
	
   wxTextCtrl* thresholdLevel = (wxTextCtrl*)FindWindow (IDC_ThresholdLevel);
   thresholdLevel->ChangeValue (
   							wxString::Format (wxT("%.2f"), m_saveThresholdPercent));
	
	TransferLinesColumnsToWindow ();

   wxCheckBox* thresholdPixel = (wxCheckBox*)FindWindow (IDC_ThresholdPixelCheck);
   thresholdPixel->SetValue (m_thresholdFlag);
	
   wxCheckBox* createNewGraphic =
   									(wxCheckBox*)FindWindow (IDC_CreateNewGraphWindow);
   createNewGraphic->SetValue (m_createNewGraphicsWindowFlag);
	
   wxCheckBox* featureTrans = (wxCheckBox*)FindWindow (IDC_FeatureTransformation);
   featureTrans->SetValue (m_featureTransformationFlag);
	
   wxCheckBox* train = (wxCheckBox*)FindWindow (IDC_Training);
   train->SetValue (m_trainingAreaFlag);
	
	wxCheckBox* test = (wxCheckBox*)FindWindow (IDC_TestAreas);
   test->SetValue (m_testFlag);
	
   wxCheckBox* area = (wxCheckBox*)FindWindow (IDC_ImageArea);
   area->SetValue (m_imageAreaFlag);
	
   wxCheckBox* displayPixelSymbol = (wxCheckBox*)FindWindow (IDC_DisplayPixels);
   if (m_displayPixelCode & kDisplayPixels) 
		displayPixelSymbol->SetValue (true);
   else 
		displayPixelSymbol->SetValue (false);
   
   controlValue = (m_outlineClassCode & kOutlineClasses)  ? 1 : 0;
   wxCheckBox* outlineClasses =
   								(wxCheckBox*)FindWindow (IDC_OutlineClassAsEllipse);
   if (m_outlineClassCode)
   	outlineClasses->SetValue (controlValue);
   else
   	outlineClasses->SetValue (controlValue);
   
   if (m_imageAreaFlag)
      ShowSomeAreaSelectionItems ();
   
   m_classesCtrl->SetSelection (m_classSelection);
	
	//wxComboBox* symbolcb = (wxComboBox*)FindWindow (IDC_SymbolCombo);
	//symbolcb->SetSelection (0); // not yet implemented
	
   m_weightsCtrl->SetSelection (m_classWeightsSelection);
   
   return true;
	
}	// end "TransferDataToWindow"
