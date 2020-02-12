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
//	File:						xListDataDialog.cpp : class implementation file
//	Class Definition:		xListDataDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
// Created:					??/??/2009
//	Revision date:			11/21/2019
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMListDataDialog class.
/* Template for debugging
		int numberChars = sprintf (
			(char*)&gTextString3,
			" xListDataDialog: (): %s",
			gEndOfLine);
		ListString ((char*)&gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "SImageWindow_class.h"

#include "xListDataDialog.h"
#include "xMultiSpec.h"

#include "wx/valnum.h"
#include "wx/tipwin.h"

extern ListDataSpecsPtr gListDataSpecsPtr;



BEGIN_EVENT_TABLE (CMListDataDialog, CMDialog)
	EVT_BUTTON (IDEntireImage, CMListDataDialog::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMListDataDialog::ToSelectedImage)

	EVT_CHECKBOX (IDC_Area, CMListDataDialog::OnArea)
	EVT_CHECKBOX (IDC_Classes, CMListDataDialog::OnClasses)
	EVT_CHECKBOX (IDC_GraphData, CMListDataDialog::OnGraphData)

	EVT_CHOICE (IDC_ListChannelsFormatCombo, CMListDataDialog::OnListChannelsFormatSelendok)

	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_ClassCombo, CMListDataDialog::OnClassComboSelendok)
		EVT_COMBOBOX (IDC_ChannelCombo, CMListDataDialog::OnChannelComboSelendok)
	#endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_ChannelCombo, CMListDataDialog::OnChannelComboSelendok)
		EVT_CHOICE (IDC_ClassCombo, CMListDataDialog::OnClassComboSelendok)
	#endif

	EVT_COMBOBOX_CLOSEUP (IDC_ChannelCombo, CMListDataDialog::OnChannelComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (IDC_ClassCombo, CMListDataDialog::OnClassComboCloseUp)

	EVT_COMBOBOX_DROPDOWN (IDC_ChannelCombo, CMListDataDialog::OnChannelComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_ClassCombo, CMListDataDialog::OnClassComboDropDown)

	EVT_INIT_DIALOG (CMListDataDialog::OnInitDialog)

	EVT_TEXT (IDC_ColumnEnd, CMListDataDialog::CheckColumnEnd)
	EVT_TEXT (IDC_ColumnStart, CMListDataDialog::CheckColumnStart)
	EVT_TEXT (IDC_ColumnInterval, CMListDataDialog::CheckColumnInterval)
	EVT_TEXT (IDC_LineEnd, CMListDataDialog::CheckLineEnd)
	EVT_TEXT (IDC_LineStart, CMListDataDialog::CheckLineStart)
	EVT_TEXT (IDC_LineInterval, CMListDataDialog::CheckLineInterval)
	EVT_TEXT (IDC_NumberDecimalPlaces, CMListDataDialog::OnChangeDecimalPlaces)
END_EVENT_TABLE ()



CMListDataDialog::CMListDataDialog (
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title /*=NULL*/)
		: CMDialog (CMListDataDialog::IDD, pParent, title)

{
	m_areaFlag = FALSE;
	m_classFlag = FALSE;
	m_textWindowFlag = FALSE;
	m_diskFileFlag = FALSE;
	m_graphDataFlag = FALSE;
	m_includeClassFieldFlag = FALSE;
	m_includeLineColumnFlag = FALSE;
	m_includeLatLongFlag = FALSE;
	m_trainingFlag = FALSE;
	m_testFlag = FALSE;
	m_latLongPossibleFlag = FALSE;
	m_listDataFormatCode = -1;
	m_numberDecimalPlaces = 2;

	CreateControls ();

	m_initializedFlag = CMDialog::m_initializedFlag;

	if (m_initializedFlag)
		{
				// Get pointer to memory for temporary storage of channel list.

		m_localFeaturesPtr = (UInt16*)MNewPointer (
				 (UInt32) gImageWindowInfoPtr->totalNumberChannels * sizeof (UInt16));

		m_initializedFlag = (m_localFeaturesPtr != NULL);

		}	// end "if (m_initializedFlag)"

	if (m_initializedFlag && gListDataSpecsPtr->projectFlag)
		{
				// Get memory for the local class list vector.

		m_classListPtr = (UInt16*)MNewPointer (
				 (UInt32) gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));

		m_initializedFlag = (m_classListPtr != NULL);

		}	// end "if (m_initializedFlag && ...->projectFlag)"
	
}	// end "CMListDataDialog"



CMListDataDialog::~CMListDataDialog (void)

{
    m_classListPtr = CheckAndDisposePtr (m_classListPtr);
    m_localFeaturesPtr = CheckAndDisposePtr (m_localFeaturesPtr);

}	// end "~CMListDataDialog"



void CMListDataDialog::CheckClassItems (
				Boolean 								listClassDataFlag)

{
	if (listClassDataFlag)
		{
		MShowDialogItem (this, IDC_ClassCombo);
		MShowDialogItem (this, IDC_IncludeClassField);
		MShowDialogItem (this, IDC_Training);
		MShowDialogItem (this, IDC_Test);
		
		}	// end "if (listClassDataFlag)"
	
	else	// !listClassDataFlag
		{
		MHideDialogItem (this, IDC_ClassCombo);
		MHideDialogItem (this, IDC_IncludeClassField);
		MHideDialogItem (this, IDC_Training);
		MHideDialogItem (this, IDC_Test);
		
		}	// end "else !listClassDataFlag"
	
}	// end "CheckClassItems"



void CMListDataDialog::CheckOKButton (void)

{
	if (m_areaFlag || m_classFlag)
		SetDLogControlHilite (this, wxID_OK, 0);
	
	else // !m_areaFlag && !m_classFlag
		SetDLogControlHilite (this, wxID_OK, 255);
	
}	// end "CheckOKButton"



void CMListDataDialog::CreateControls (void)

{
	SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	bSizer155 = new wxBoxSizer (wxVERTICAL);
	
 	wxStaticBoxSizer* sbSizer26;
   m_staticBoxArea = new wxStaticBox (this, wxID_ANY, wxT("Area(s)"));
	sbSizer26 = new wxStaticBoxSizer (m_staticBoxArea, wxVERTICAL);
	
	wxBoxSizer* bSizer161;
	bSizer161 = new wxBoxSizer (wxHORIZONTAL);
	m_checkBox50 = new wxCheckBox (sbSizer26->GetStaticBox (),
												IDC_Classes,
												wxT("Classes:"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox50, IDS_ToolTip47);

   bSizer161->Add (m_checkBox50,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

   GetAllSubsetMenuControl (kClassesMenu,
										sbSizer26->GetStaticBox (),
										IDC_ClassCombo,
										120,
										IDS_ToolTip48);
	
	bSizer161->Add (m_classesCtrl,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	wxBoxSizer* bSizer162;
	bSizer162 = new wxBoxSizer (wxVERTICAL);
	
	m_checkBox51 = new wxCheckBox (sbSizer26->GetStaticBox (),
												IDC_Training,
												wxT("Training"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox51, IDS_ToolTip49);
	bSizer162->Add (m_checkBox51,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	m_checkBox52 = new wxCheckBox (sbSizer26->GetStaticBox (),
												IDC_Test,
												wxT("Test"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox52, IDS_ToolTip50);
	bSizer162->Add (m_checkBox52,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	bSizer161->Add (bSizer162, wxSizerFlags(0).Expand());

	sbSizer26->Add (bSizer161, wxSizerFlags(0).Expand());
	
	wxBoxSizer* bSizer163;
	bSizer163 = new wxBoxSizer (wxVERTICAL);
	
	m_checkBox54 = new wxCheckBox (sbSizer26->GetStaticBox (),
												IDC_Area,
												wxT("Area"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox54, IDS_ToolTip51);
	bSizer163->Add (m_checkBox54, wxSizerFlags(0).Border(wxALL, 5));
	
	wxStaticBoxSizer* sbSizer29;
   wxStaticBox* areaStaticBox = new wxStaticBox (sbSizer26->GetStaticBox (),
   																IDC_LineColFrame,
   																wxT("Selected Area"),
																	wxDefaultPosition,
																	wxDefaultSize,
																	wxTAB_TRAVERSAL);
	sbSizer29 = new wxStaticBoxSizer (areaStaticBox, wxHORIZONTAL);

	CreateLineColumnControls (sbSizer29);
	
	bSizer163->Add (sbSizer29,
							wxSizerFlags(0).Expand().Border(wxLEFT|wxBOTTOM|wxRIGHT, 6));
	
	sbSizer26->Add (bSizer163, wxSizerFlags(0).Expand());
	
	bSizer155->Add (sbSizer26,
							wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT|wxBOTTOM, 12));
	
	wxBoxSizer* bSizer160;
	bSizer160 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText205 = new wxStaticText (this, wxID_ANY, wxT("Channels:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText205->Wrap (-1);
   SetUpToolTip (m_staticText205, IDS_ToolTip52);
	bSizer160->Add (
			m_staticText205,
			wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxRIGHT|wxBOTTOM, 5));

   GetAllSubsetMenuControl (kChannelsMenu,
										this,
										IDC_ChannelCombo,
										120,
										IDS_ToolTip52);
	bSizer160->Add (m_channelsCtrl, wxSizerFlags(0).Border(wxLEFT|wxBOTTOM, 5));
	
	bSizer155->Add (bSizer160, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	wxFlexGridSizer* gSizer13;
	gSizer13 = new wxFlexGridSizer (0, 2, 0, 0);
	gSizer13->SetFlexibleDirection (wxHORIZONTAL);
	gSizer13->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	wxStaticBoxSizer* sbSizer27;
	sbSizer27 = new wxStaticBoxSizer (new wxStaticBox (this, wxID_ANY, wxT("Options")),
													wxVERTICAL);
	
	wxBoxSizer* bSizer164;
	bSizer164 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText206 = new wxStaticText (sbSizer27->GetStaticBox (),
													wxID_ANY,
													wxT("List channels in"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText206->Wrap (-1);
	bSizer164->Add (m_staticText206, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxChoice*		comboBox30;
	comboBox30 = new wxChoice (sbSizer27->GetStaticBox (),
										IDC_ListChannelsFormatCombo,
										wxDefaultPosition,
										wxDefaultSize);
	comboBox30->Append ("Rows");
	comboBox30->Append ("Columns");
   SetUpToolTip (comboBox30, IDS_ToolTip53);
	bSizer164->Add (comboBox30, wxSizerFlags(0).Border(wxALL, 5));
	
	sbSizer27->Add (bSizer164, wxSizerFlags(0).Expand());
	
	m_checkBox55 = new wxCheckBox (sbSizer27->GetStaticBox (),
												IDC_IncludeLineColumn,
												wxT("Include line and column values"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	sbSizer27->Add (m_checkBox55, wxSizerFlags(0).Border(wxALL, 5));
	
	m_checkBox56 = new wxCheckBox (sbSizer27->GetStaticBox (),
												IDC_IncludeLatitudeLongitude,
												wxT("Include latitude and longitude values"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	sbSizer27->Add (m_checkBox56,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	m_checkBox57 = new wxCheckBox (sbSizer27->GetStaticBox (),
												IDC_IncludeClassField,
												wxT("Include class and field identification"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	sbSizer27->Add (m_checkBox57,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));
	
	m_checkBox58 = new wxCheckBox (sbSizer27->GetStaticBox (),
												IDC_GraphData,
												wxT("Graph data values"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	sbSizer27->Add (m_checkBox58, wxSizerFlags(0).Border(wxALL, 5));
	
   SetUpToolTip (m_checkBox55, IDS_ToolTip54);
   SetUpToolTip (m_checkBox56, IDS_ToolTip55);
   SetUpToolTip (m_checkBox57, IDS_ToolTip56);
   SetUpToolTip (m_checkBox58, IDS_ToolTip57);
   
	wxBoxSizer* bSizer180;
	bSizer180 = new wxBoxSizer (wxHORIZONTAL);
	m_staticText216 = new wxStaticText (sbSizer27->GetStaticBox (),
													IDC_NumberDecimalPlacesPrompt,
													wxT("Number decimal places to use:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText216->Wrap (-1);
	bSizer180->Add (m_staticText216,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().
											Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

	wxString numdecstring = wxString::Format (wxT("%ld"), m_numberDecimalPlaces);
	m_numberdec = new wxTextCtrl (sbSizer27->GetStaticBox (),
											IDC_NumberDecimalPlaces,
											numdecstring,
											wxDefaultPosition,
											wxSize (40, -1),
											0);
	m_numberdec->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_numdecString));
	bSizer180->Add (m_numberdec,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().
												Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));
	
	sbSizer27->Add (bSizer180, wxSizerFlags(0).Border(wxALL, 5));
	
   gSizer13->Add (sbSizer27, wxSizerFlags(0).Border(wxTOP|wxRIGHT|wxBOTTOM, 6));
	
	wxStaticBoxSizer* sbSizer281;
	sbSizer281 = new wxStaticBoxSizer (new wxStaticBox (this,
																			wxID_ANY,
																			wxT("Output results to:")),
													wxVERTICAL);
	
	m_checkBox59 = new wxCheckBox (sbSizer281->GetStaticBox (),
												IDC_TextWindow,
												wxT("Text window  "),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox59, IDS_ToolTip59);
	m_checkBox59->SetValue (true);
	//sbSizer281->Add (m_checkBox59, 0, wxALL, 5);
	sbSizer281->Add (m_checkBox59, wxSizerFlags(0).Border(wxALL, 5));
	
	m_checkBox60 = new wxCheckBox (sbSizer281->GetStaticBox (),
												IDC_DiskFile,
												wxT("Disk file"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox60, IDS_ToolTip58);
	sbSizer281->Add (m_checkBox60, wxSizerFlags(0).Border(wxALL, 5));
	
	gSizer13->Add (sbSizer281,
						wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxBOTTOM, 6));
	
	bSizer155->Add (gSizer13, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	CreateStandardButtons (bSizer155);
	
	this->SetSizer (bSizer155);
	this->Layout ();
	bSizer155->Fit (this);
	
}	// end "CreateControls"



SInt16 CMListDataDialog::DoDialog (void)

{
	Boolean continueFlag = FALSE;

	SInt16 returnCode;


			// Make sure intialization has been completed.

	if (!m_initializedFlag)
																							return (FALSE);

	returnCode = ShowModal ();

	if (returnCode == wxID_OK)
		{
		DialogSelectArea dialogSelectArea;


		continueFlag = TRUE;

		dialogSelectArea.lineStart = m_LineStart;
		dialogSelectArea.lineEnd = m_LineEnd;
		dialogSelectArea.lineInterval = m_LineInterval;
		dialogSelectArea.columnStart = m_ColumnStart;
		dialogSelectArea.columnEnd = m_ColumnEnd;
		dialogSelectArea.columnInterval = m_ColumnInterval;

		ListDataDialogOK (gListDataSpecsPtr,
								 m_classFlag,
								 m_classSelection,
								 m_localNumberClasses,
								 m_classListPtr,
								 m_areaFlag,
								 &dialogSelectArea,
								 m_channelSelection,
								 m_localActiveNumberFeatures,
								 m_localFeaturesPtr,
								 m_includeLineColumnFlag,
								 m_includeLatLongFlag,
								 m_includeClassFieldFlag,
								 m_textWindowFlag,
								 m_diskFileFlag,
								 m_trainingFlag,
								 m_testFlag,
								 m_graphDataFlag,
								 (UInt16) m_numberDecimalPlaces,
								 m_listDataFormatCode + 1);

		}	// end "if (returnCode == IDOK)"

	return (continueFlag);

}	// end "DoDialog"



void CMListDataDialog::OnArea (
				wxCommandEvent&					event)

{
	UInt16 selectItem;
	
	m_areaFlag = ((wxCheckBox*)FindWindow (IDC_Area))->GetValue ();
	if (m_areaFlag)
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
	
	CheckOKButton ();
	
}	// end "OnArea"



void CMListDataDialog::OnListChannelsFormatSelendok (
				wxCommandEvent&					event)

{
	m_listDataFormatCode = ((wxChoice*)FindWindow (
													IDC_ListChannelsFormatCombo))->GetSelection ();
	
	if (m_listDataFormatCode == 0 && m_latLongPossibleFlag)
		MShowDialogItem (this, IDC_IncludeLatitudeLongitude);
	
	else // m_listDataFormatCode = 1 || !m_latLongPossibleFlag
		MHideDialogItem (this, IDC_IncludeLatitudeLongitude);
	
}	// end "OnListChannelsFormatSelendok"



void CMListDataDialog::OnChangeDecimalPlaces (
				wxCommandEvent&					event)

{
	m_numberDecimalPlaces = (SInt32)CheckValue (IDC_NumberDecimalPlaces, 0, 9);
	
}	// end "OnChangeBlueChannel"



void CMListDataDialog::OnClasses (
				wxCommandEvent&					event)

{
	wxCheckBox* classcb = (wxCheckBox*)FindWindow (IDC_Classes);
	m_classFlag = classcb->GetValue ();
	CheckClassItems (m_classFlag);
	
	CheckOKButton ();
	
}	// end "OnClasses"



void CMListDataDialog::OnGraphData (
				wxCommandEvent& 					event)

{
	wxCheckBox* graphDataCheckBox = (wxCheckBox*)FindWindow (IDC_GraphData);
	m_savedGraphDataFlag = graphDataCheckBox->GetValue ();
	
}	// end "OnGraphData"



void CMListDataDialog::OnInitDialog (
				wxInitDialogEvent&				event)

{
    UInt32 index;
    UInt16 selectItem;

    CMDialog::OnInitDialog (event);

			// Update the modal dialog with the default settings
			// Set check box for "Classes".

    m_classFlag = gListDataSpecsPtr->listClassesDataFlag;

	if (!gListDataSpecsPtr->projectFlag)
		MHideDialogItem (this, IDC_Classes);

	m_classSelection = gListDataSpecsPtr->classSet;
	m_localNumberClasses = gListDataSpecsPtr->numberClasses;

	UInt16* classPtr = (UInt16*)GetHandlePointer (gListDataSpecsPtr->classHandle);

	if (gListDataSpecsPtr->projectFlag)
		{
		for (index = 0; index < m_localNumberClasses; index++)
			m_classListPtr[index] = classPtr[index];

		}	// end "if (gListDataSpecsPtr->projectFlag)"

	CheckClassItems (gListDataSpecsPtr->listClassesDataFlag);

			// Selected area for list data
			// Initialize selected area structure.

	InitializeDialogSelectArea (&m_dialogSelectArea,
											gImageWindowInfoPtr,
											gActiveImageWindow,
											gListDataSpecsPtr->columnStart,
											gListDataSpecsPtr->columnEnd,
											gListDataSpecsPtr->columnInterval,
											gListDataSpecsPtr->lineStart,
											gListDataSpecsPtr->lineEnd,
											gListDataSpecsPtr->lineInterval,
											13,
											11,
											kDontAdjustToBaseImage);

	m_LineStart = gListDataSpecsPtr->lineStart;
	m_LineEnd = gListDataSpecsPtr->lineEnd;
	m_LineInterval = gListDataSpecsPtr->lineInterval;
	m_ColumnStart = gListDataSpecsPtr->columnStart;
	m_ColumnEnd = gListDataSpecsPtr->columnEnd;
	m_ColumnInterval = gListDataSpecsPtr->columnInterval;

			// Set check box for area.

	m_areaFlag = gListDataSpecsPtr->listSelectedAreaDataFlag;
	if (gListDataSpecsPtr->listSelectedAreaDataFlag)
		{
		ShowSomeAreaSelectionItems ();

		selectItem = IDC_LineStart;

    	}	// end "if (gListDataSpecsPtr->listSelectedAreaDataFlag)"

	else	// !gListDataSpecsPtr->listSelectedAreaDataFlag
    	{
		HideSomeAreaSelectionItems ();

		selectItem = IDC_LineInterval;

    	}	// end "else !gListDataSpecsPtr->listSelectedAreaDataFlag"

			//	Set the All/Subset channels list item

	m_channelSelection = gListDataSpecsPtr->channelSet;
	m_localNumberFeatures = gListDataSpecsPtr->numberChannels;

	SInt16* channelsPtr = (SInt16*)GetHandlePointer (
																gListDataSpecsPtr->featureHandle);
	
	for (index=0; index<m_localNumberFeatures; index++)
        m_localFeaturesPtr[index] = channelsPtr[index];

			// Set feature parameters

	m_localActiveNumberFeatures = m_localNumberFeatures;
	m_localActiveFeaturesPtr = m_localFeaturesPtr;

	if (gImageFileInfoPtr->thematicType)
		{
		MHideDialogItem (this, IDC_ChannelPrompt);
		MHideDialogItem (this, IDC_ChannelCombo);

		}	// end "if (gImageFileInfoPtr->thematicType)"

	else // !gImageFileInfoPtr->thematicType
		{
		MShowDialogItem (this, IDC_ChannelPrompt);
		MShowDialogItem (this, IDC_ChannelCombo);

		}	// end "else !gImageFileInfoPtr->thematicType"

			// Set check box for "Include line and column values".

	m_includeLineColumnFlag = gListDataSpecsPtr->includeLineColumnValuesFlag;

			// Set check box for "Include latitude and longitude values".

	m_includeLatLongFlag = gListDataSpecsPtr->includeLatLongValuesFlag;

			// Set check box for "Inlude class and field codes".

	m_includeClassFieldFlag = gListDataSpecsPtr->includeClassFieldFlag;

			// Set check box for "text output window".

	if (gListDataSpecsPtr->outputStorageType & 0x0001)
		m_textWindowFlag = TRUE;

			// Set check box for "disk file".

	if (gListDataSpecsPtr->outputStorageType & 0x0002)
		m_diskFileFlag = TRUE;

			// Set control for including training fields.

	if (gListDataSpecsPtr->fieldTypeCode & kTrainingType)
		m_trainingFlag = TRUE;

			// Set control for including test fields.

	if (gListDataSpecsPtr->fieldTypeCode & kTestingType)
		m_testFlag = TRUE;

	if (gProjectInfoPtr != NULL && gProjectInfoPtr->numberStatTestFields <= 0)
		{
		SetDLogControlHilite (this, IDC_Test, 255);
		m_testFlag = FALSE;

		}	// end "if (gProjectInfoPtr != NULL && ..."

			// Set check box for "Graph data values".

	m_graphDataFlag = gListDataSpecsPtr->graphDataFlag;
	m_savedGraphDataFlag = m_graphDataFlag;
	if (gNumberOfGWindows >= kMaxNumberGWindows ||
											m_channelSelection == kSubsetMenuItem)
		{
		SetDLogControl (this, IDC_GraphData, 0);
		SetDLogControlHilite (this, IDC_GraphData, 255);
		m_graphDataFlag = false;

		}	// end "if (gNumberOfGWindows >= kMaxNumberGWindows || ..."

	m_listDataFormatCode = gListDataSpecsPtr->outputFormatCode - 1;
    
	m_numberDecimalPlaces = gListDataSpecsPtr->numberFDecimalDigits;
	if (gImageFileInfoPtr->dataTypeCode == kIntegerType)
		{
		HideDialogItem (this, IDC_NumberDecimalPlacesPrompt);
		HideDialogItem (this, IDC_NumberDecimalPlaces);

		}	// end "if (gImageFileInfoPtr->dataTypeCode == kIntegerType)"

	else		// gImageFileInfoPtr->dataTypeCode == kRealType
		{
		ShowDialogItem (this, IDC_NumberDecimalPlacesPrompt);
		ShowDialogItem (this, IDC_NumberDecimalPlaces);

		}	// end "if (gImageFileInfoPtr->dataTypeCode == kIntegerType)"
    
	if (CMListDataDialog::TransferDataToWindow ())
		PositionDialogWindow ();
	
	SelectDialogItemText (this, selectItem, 0, SInt16_MAX);

}	// end "OnInitDialog"



void CMListDataDialog::OnChannelComboSelendok (
				wxCommandEvent& 					event)

{
	HandleChannelsMenu (IDC_ChannelCombo,
								kNoTransformation,
								(SInt16)gImageWindowInfoPtr->totalNumberChannels,
								1,
								TRUE);
	
   m_channelSelection = m_channelsCtrl->GetSelection ();

	if (gNumberOfGWindows < kMaxNumberGWindows &&
											m_channelSelection == kAllMenuItem)
		{
		SetDLogControl (this, IDC_GraphData, m_savedGraphDataFlag);
		SetDLogControlHilite (this, IDC_GraphData, 0);

		}	// end "if (gNumberOfGWindows < kMaxNumberGWindows && ..."

	else	// gNumberOfGWindows >= kMaxNumberGWindows || ...
		{
		SetDLogControl (this, IDC_GraphData, 0);
		SetDLogControlHilite (this, IDC_GraphData, 255);

		}	// end "else gNumberOfGWindows >= kMaxNumberGWindows || ..."
	
}	// end "OnChannelComboSelendok"



void CMListDataDialog::OnClassComboSelendok (
				wxCommandEvent&					event)

{
	HandleClassesMenu (&m_localNumberClasses,
            				(SInt16*)m_classListPtr,
            				1,
            				(SInt16)gProjectInfoPtr->numberStatisticsClasses,
            				IDC_ClassCombo,
            				&m_classSelection);
    
}	// end "OnClassComboSelendok"



void CMListDataDialog::OnSelAreaUpdate (
				wxUpdateUIEvent&					event)

{
	m_areaFlag = ((wxCheckBox*)FindWindow (IDC_Area))->GetValue ();
	if (m_areaFlag)
		event.Enable (true);
	
	else event.Enable (false);
    
	CheckOKButton ();
	
}	// end "OnSelAreaUpdate"



bool CMListDataDialog::TransferDataFromWindow ()

{
	SInt16								returnCode = 0;
	
	
	TransferLinesColumnsFromWindow ();

	wxTextCtrl* num_dec_place = (wxTextCtrl*)FindWindow (IDC_NumberDecimalPlaces);
	m_numdecString = num_dec_place ->GetValue ();
	m_numberDecimalPlaces = wxAtoi (m_numdecString);

	m_classSelection = m_classesCtrl->GetSelection ();
	
   m_channelSelection = m_channelsCtrl->GetSelection ();
	
	wxChoice* listchannelcombo = (wxChoice*)FindWindow (IDC_ListChannelsFormatCombo);
	m_listDataFormatCode = listchannelcombo->GetSelection ();
	
	wxCheckBox* areacb = (wxCheckBox*)FindWindow (IDC_Area);
	m_areaFlag = areacb->GetValue ();
	
	wxCheckBox* classcb = (wxCheckBox*)FindWindow (IDC_Classes);
	m_classFlag = classcb->GetValue ();
	
	wxCheckBox* traincb = (wxCheckBox*)FindWindow (IDC_Training);
	m_trainingFlag = traincb->GetValue ();
	
	wxCheckBox* testcb = (wxCheckBox*)FindWindow (IDC_Test);
	m_testFlag = testcb->GetValue ();
	
	wxCheckBox* optwnd = (wxCheckBox*)FindWindow (IDC_TextWindow);
	m_textWindowFlag = optwnd->GetValue ();
	
	wxCheckBox* opdskf = (wxCheckBox*)FindWindow (IDC_DiskFile);
	m_diskFileFlag = opdskf->GetValue ();
	
	wxCheckBox* graphdatacb = (wxCheckBox*)FindWindow (IDC_GraphData);
	m_graphDataFlag = graphdatacb->GetValue ();
	
	wxCheckBox* inclascb = (wxCheckBox*)FindWindow (IDC_IncludeClassField);
	m_includeClassFieldFlag = inclascb->GetValue ();
	
	wxCheckBox* inlinecb = (wxCheckBox*)FindWindow (IDC_IncludeLineColumn);
	m_includeLineColumnFlag = inlinecb->GetValue ();
	
	wxCheckBox* inlaticb = (wxCheckBox*)FindWindow (IDC_IncludeLatitudeLongitude);
	m_includeLatLongFlag = inlaticb->GetValue ();
	
	if (m_areaFlag)
		returnCode = VerifyLineColumnValues (IDC_LineStart,
															IDC_ColumnStart,
															true);

	return (returnCode == 0);
	
}	// end "TransferDataFromWindow"



bool CMListDataDialog::TransferDataToWindow ()

{
	TransferLinesColumnsToWindow ();

	wxTextCtrl* num_dec_place = (wxTextCtrl*)FindWindow (IDC_NumberDecimalPlaces);
	num_dec_place->ChangeValue (wxString::Format (wxT("%ld"), m_numberDecimalPlaces));

	m_classesCtrl->SetSelection (m_classSelection);
	
	wxChoice* listchannelcombo = (wxChoice*)FindWindow (IDC_ListChannelsFormatCombo);
	listchannelcombo->SetSelection (m_listDataFormatCode);
	
	m_channelsCtrl->SetSelection (m_channelSelection);

	wxCheckBox* graphdatacb = (wxCheckBox*)FindWindow (IDC_GraphData);
	graphdatacb->SetValue (m_graphDataFlag);
	
	wxCheckBox* inclascb = (wxCheckBox*)FindWindow (IDC_IncludeClassField);
	inclascb->SetValue (m_includeClassFieldFlag);
	
	wxCheckBox* inlinecb = (wxCheckBox*)FindWindow (IDC_IncludeLineColumn);
	inlinecb->SetValue (m_includeLineColumnFlag);

	wxCheckBox* inlaticb = (wxCheckBox*)FindWindow (IDC_IncludeLatitudeLongitude);
	if (DetermineIfLatLongPossible (gListDataSpecsPtr->windowInfoHandle))
		{
		inlaticb-> SetValue (m_includeLatLongFlag);
		m_latLongPossibleFlag = true;

		}	// end "if (DetermineIfLatLongPossible (...->windowInfoHandle))"

	else	// !DetermineIfLatLongPossible (...->windowInfoHandle)
		{
		inlaticb->SetValue (false);
		MHideDialogItem (this, IDC_IncludeLatitudeLongitude);
		m_latLongPossibleFlag = false;

		}	// end "else !DetermineIfLatLongPossible (...->windowInfoHandle))"

	wxCheckBox* areacb = (wxCheckBox*)FindWindow (IDC_Area);
	areacb->SetValue (m_areaFlag);
	
	wxCheckBox* classcb = (wxCheckBox*)FindWindow (IDC_Classes);
	classcb->SetValue (m_classFlag);
	
	wxCheckBox* traincb = (wxCheckBox*)FindWindow (IDC_Training);
	traincb->SetValue (m_trainingFlag);
	
	wxCheckBox* testcb = (wxCheckBox*)FindWindow (IDC_Test);
	testcb->SetValue (m_testFlag);
	
	wxCheckBox* optwnd = (wxCheckBox*)FindWindow (IDC_TextWindow);
	optwnd->SetValue (m_textWindowFlag);
	
	wxCheckBox* opdskf = (wxCheckBox*)FindWindow (IDC_DiskFile);
	opdskf->SetValue (m_diskFileFlag);

	return true;
	
}	// end "TransferDataToWindow"
