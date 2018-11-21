//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LListDataDialog.cpp : class implementation file
//	Class Definition:		LListDataDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
// Created:					??/??/2009
//	Revision date:			11/16/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMListDataDialog class.
/* Template for debugging
		int numberChars = sprintf (
			(char*)&gTextString3,
			" LListDataDialog: (): %s",
			gEndOfLine);
		ListString ((char*)&gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "CImageWindow.h"

#include "LImage_dialog.cpp"
#include "LListDataDialog.h"
#include "LMultiSpec.h"

#include "wx/valnum.h"
#include "wx/tipwin.h"

extern ListDataSpecsPtr gListDataSpecsPtr;

extern void ListDataDialogOK (
				ListDataSpecsPtr listDataSpecsPtr,
				Boolean classFlag,
				SInt16 classSelection,
				UInt32 localNumberClasses,
				UInt16* localClassPtr,
				Boolean areaFlag,
				DialogSelectArea* dialogSelectAreaPtr,
				SInt16 channelSelection,
				UInt16 localNumberChannels,
				UInt16* localChannelPtr,
				Boolean includeLineColumnFlag,
				Boolean includeLatLongFlag,
				Boolean includeClassFieldFlag,
				Boolean textWindowFlag,
				Boolean diskFileFlag,
				Boolean trainingFlag,
				Boolean testFlag,
				Boolean graphDataFlag,
				UInt16 numberFDecimalDigits,
				SInt16 listDataFormatCode);

/////////////////////////////////////////////////////////////////////////////
// CMListDataDialog dialog

CMListDataDialog::CMListDataDialog (
				wxWindow* pParent,
				wxWindowID id,
				const wxString& title /*=NULL*/)
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

	CreateControls();

	m_initializedFlag = CMDialog::m_initializedFlag;

	if (m_initializedFlag)
		{
				// Get pointer to memory for temporary storage of channel list.

		m_localFeaturesPtr = (UInt16*) MNewPointer (
				 (UInt32) gImageWindowInfoPtr->totalNumberChannels * sizeof (UInt16));

		m_initializedFlag = (m_localFeaturesPtr != NULL);

		}	// end "if (m_initializedFlag)"

	if (m_initializedFlag && gListDataSpecsPtr->projectFlag)
		{
				// Get memory for the local class list vector.

		m_classListPtr = (UInt16*) MNewPointer(
				 (UInt32) gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));

		m_initializedFlag = (m_classListPtr != NULL);

		}	// end "if (m_initializedFlag && ...->projectFlag)"
	
}	// end "CMListDataDialog"



CMListDataDialog::~CMListDataDialog (void)

{
    m_classListPtr = CheckAndDisposePtr(m_classListPtr);
    m_localFeaturesPtr = CheckAndDisposePtr(m_localFeaturesPtr);

}	// end "~CMListDataDialog"



BEGIN_EVENT_TABLE (CMListDataDialog, CMDialog)
	EVT_INIT_DIALOG (CMListDataDialog::OnInitDialog)
	EVT_COMBOBOX (IDC_ChannelCombo, CMListDataDialog::OnSelendokChannelCombo)
	EVT_COMBOBOX (IDC_ClassCombo, CMListDataDialog::OnSelendokClassCombo)
	EVT_TEXT (IDC_ColumnEnd, CMListDataDialog::CheckColumnEnd)
	EVT_TEXT (IDC_ColumnStart, CMListDataDialog::CheckColumnStart)
	EVT_TEXT (IDC_LineEnd, CMListDataDialog::CheckLineEnd)
	EVT_TEXT (IDC_LineStart, CMListDataDialog::CheckLineStart)
	EVT_TEXT (IDC_LineInterval, CMListDataDialog::CheckLineInterval)
	EVT_TEXT (IDC_ColumnInterval, CMListDataDialog::CheckColumnInterval)
	EVT_TEXT (IDC_NumberDecimalPlaces, CMListDataDialog::OnChangeDecimalPlaces)
	EVT_BUTTON (IDEntireImage, CMListDataDialog::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMListDataDialog::ToSelectedImage)
	EVT_CHECKBOX (IDC_Classes, CMListDataDialog::OnClasses)
	EVT_CHECKBOX (IDC_Area, CMListDataDialog::OnArea)
	EVT_CHECKBOX (IDC_GraphData, CMListDataDialog::OnGraphData)
	EVT_COMBOBOX (IDC_ListChannelsFormatCombo, CMListDataDialog::OnCbnSelendokListchannelsformatcombo)
	EVT_COMBOBOX_DROPDOWN (IDC_ChannelCombo,CMListDataDialog::OnSelendokChannelComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_ClassCombo,CMListDataDialog::OnSelendokClassComboDropDown)
END_EVENT_TABLE()



void CMListDataDialog::CheckClassItems(
													Boolean listClassDataFlag)
{
	if (listClassDataFlag)
		{
		MShowDialogItem(this, IDC_ClassCombo);
		MShowDialogItem(this, IDC_IncludeClassField);
		MShowDialogItem(this, IDC_Training);
		MShowDialogItem(this, IDC_Test);
		
		}	// end "if (listClassDataFlag)"
	
	else	// !listClassDataFlag
		{
		MHideDialogItem(this, IDC_ClassCombo);
		MHideDialogItem(this, IDC_IncludeClassField);
		MHideDialogItem(this, IDC_Training);
		MHideDialogItem(this, IDC_Test);
		
		}	// end "else !listClassDataFlag"
	
}	// end "CheckClassItems"



void CMListDataDialog::CheckOKButton (void)

{
	if (m_areaFlag || m_classFlag)
		SetDLogControlHilite (this, wxID_OK, 0);
	
	else // !m_areaFlag && !m_classFlag
		SetDLogControlHilite (this, wxID_OK, 255);
	
}	// end "CheckOKButton"



void CMListDataDialog::CreateControls ()

{
	this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxBitmap entireimi = wxBITMAP_PNG_FROM_DATA (entireim);
	wxBitmap toentirei = wxBITMAP_PNG_FROM_DATA (toentire);
	wxBitmap selectedi = wxBITMAP_PNG_FROM_DATA (selected);
	wxBitmap bmp4i = wxBITMAP_PNG_FROM_DATA (bmp4);
	
	//bSizer155 = new wxBoxSizer (wxHORIZONTAL);
	bSizer155 = new wxBoxSizer (wxVERTICAL);
	
	//wxBoxSizer* bSizer157;
	//bSizer157 = new wxBoxSizer (wxVERTICAL);
	
 	wxStaticBoxSizer* sbSizer26;
   m_staticBoxArea = new wxStaticBox (this, wxID_ANY, wxT("Area(s)"));
	sbSizer26 = new wxStaticBoxSizer (m_staticBoxArea, wxVERTICAL);
	
	wxBoxSizer* bSizer161;
	bSizer161 = new wxBoxSizer (wxHORIZONTAL);
	m_checkBox50 = new wxCheckBox (sbSizer26->GetStaticBox(),
												IDC_Classes,
												wxT("Classes:"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox50, IDS_ToolTip47);

   //bSizer161->Add (m_checkBox50, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL, 5);
   bSizer161->Add (m_checkBox50,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));
	//bSizer161->Add (m_checkBox50, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));
	m_comboBox27 = new wxComboBox (sbSizer26->GetStaticBox(),
												IDC_ClassCombo,
												wxT("Combo!"),
												wxDefaultPosition,
												wxDefaultSize,
												0,
												NULL,
												wxCB_READONLY);
	m_comboBox27->Append (wxT("All"));
	m_comboBox27->Append (wxT("Subset..."));
   SetUpToolTip (m_comboBox27, IDS_ToolTip48);
	//bSizer161->Add (m_comboBox27, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL, 5);
	bSizer161->Add (m_comboBox27,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	wxBoxSizer* bSizer162;
	bSizer162 = new wxBoxSizer (wxVERTICAL);
	
	m_checkBox51 = new wxCheckBox (sbSizer26->GetStaticBox(),
												IDC_Training,
												wxT("Training"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox51, IDS_ToolTip49);
	//bSizer162->Add (m_checkBox51, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL, 5);
	bSizer162->Add (m_checkBox51,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	m_checkBox52 = new wxCheckBox (sbSizer26->GetStaticBox(),
												IDC_Test,
												wxT("Test"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox52, IDS_ToolTip50);
	//bSizer162->Add (m_checkBox52, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL, 5);
	bSizer162->Add (m_checkBox52,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	//bSizer161->Add (bSizer162, 0, wxEXPAND, 5);
	bSizer161->Add (bSizer162, wxSizerFlags(0).Expand());

	//sbSizer26->Add (bSizer161, 0, wxEXPAND, 5);
	sbSizer26->Add (bSizer161, wxSizerFlags(0).Expand());
	
	wxBoxSizer* bSizer163;
	bSizer163 = new wxBoxSizer (wxVERTICAL);
	
	m_checkBox54 = new wxCheckBox (sbSizer26->GetStaticBox(),
												IDC_Area,
												wxT("Area"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox54, IDS_ToolTip51);
	//bSizer163->Add (m_checkBox54, 0, wxALL, 5);
	bSizer163->Add (m_checkBox54, wxSizerFlags(0).Border(wxALL, 5));
	
	wxStaticBoxSizer* sbSizer29;
   wxStaticBox* areaStaticBox = new wxStaticBox (sbSizer26->GetStaticBox(),
   																IDC_LineColFrame,
   																wxT("Selected Area"));
	sbSizer29 = new wxStaticBoxSizer (areaStaticBox, wxHORIZONTAL);
	/*
	wxBoxSizer* bSizer791;
	bSizer791 = new wxBoxSizer (wxHORIZONTAL);
	
	m_bpButton4 = new wxBitmapButton (areaStaticBox,
													IDEntireImage,
													entireimi,
													wxDefaultPosition,
													wxDefaultSize,
													wxBU_AUTODRAW);
	m_bpButton4->SetBitmapDisabled (toentirei);
   SetUpToolTip (m_bpButton4, IDS_ToolTip40);
	bSizer791->Add (m_bpButton4,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

	m_bpButton5 = new wxBitmapButton (areaStaticBox,
													IDSelectedImage,
													selectedi,
													wxDefaultPosition,
													wxDefaultSize,
													wxBU_AUTODRAW);
	m_bpButton5->SetBitmapDisabled (selectedi);
	bSizer791->Add (m_bpButton5,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
	m_bpButton5->Hide();
	
   //sbSizer29->Add (bSizer791, 0, wxEXPAND, 5);
	sbSizer29->Add (bSizer791, wxSizerFlags(0).Expand());
	*/
	CreateLineColumnControls (sbSizer29);
	
	bSizer163->Add (sbSizer29,
							wxSizerFlags(0).Expand().Border(wxLEFT|wxBOTTOM|wxRIGHT, 6));
	
	sbSizer26->Add (bSizer163, wxSizerFlags(0).Expand());
	
	bSizer155->Add (sbSizer26,
							wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT|wxBOTTOM, 12));
	
	wxBoxSizer* bSizer160;
	bSizer160 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText205 = new wxStaticText(this, wxID_ANY, wxT("Channels:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText205->Wrap(-1);
   SetUpToolTip(m_staticText205, IDS_ToolTip52);
	//bSizer160->Add (m_staticText205, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	bSizer160->Add (m_staticText205,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxRIGHT|wxBOTTOM, 5));
	
	m_comboBox29 = new wxComboBox (this,
												IDC_ChannelCombo,
												wxT("All"),
												wxDefaultPosition,
												wxDefaultSize,
												0,
												NULL,
												wxCB_READONLY);
	m_comboBox29->Append (wxT("All"));
	m_comboBox29->Append (wxT("Subset..."));
   SetUpToolTip (m_comboBox29, IDS_ToolTip52);
	bSizer160->Add (m_comboBox29, wxSizerFlags(0).Border(wxLEFT|wxBOTTOM, 5));
	//bSizer157->Add (bSizer160, 0, wxEXPAND, 5);
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
	
	m_staticText206 = new wxStaticText (sbSizer27->GetStaticBox(),
													wxID_ANY,
													wxT("List channels in"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText206->Wrap (-1);
	bSizer164->Add (m_staticText206, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	m_comboBox30 = new wxComboBox (sbSizer27->GetStaticBox(),
												IDC_ListChannelsFormatCombo,
												wxT("Rows"),
												wxDefaultPosition,
												wxDefaultSize,
												0,
												NULL,
												wxCB_READONLY);
	m_comboBox30->Append ("Rows");
	m_comboBox30->Append ("Columns");
	//bSizer164->Add (m_comboBox30, 0, wxALL, 5);
	bSizer164->Add (m_comboBox30, wxSizerFlags(0).Border(wxALL, 5));
	
	//sbSizer27->Add (bSizer164, 0, wxEXPAND, 5);
	sbSizer27->Add (bSizer164, wxSizerFlags(0).Expand());
	
	m_checkBox55 = new wxCheckBox (sbSizer27->GetStaticBox(),
												IDC_IncludeLineColumn,
												wxT("Include line and column values"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	//sbSizer27->Add (m_checkBox55, 0, wxALL, 5);
	sbSizer27->Add (m_checkBox55, wxSizerFlags(0).Border(wxALL, 5));
	
	m_checkBox56 = new wxCheckBox (sbSizer27->GetStaticBox(),
												IDC_IncludeLatitudeLongitude,
												wxT("Include latitude and longitude values"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	//sbSizer27->Add (m_checkBox56, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL, 5);
	sbSizer27->Add (m_checkBox56,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	m_checkBox57 = new wxCheckBox (sbSizer27->GetStaticBox(),
												IDC_IncludeClassField,
												wxT("Include class and field identification"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	//sbSizer27->Add (m_checkBox57, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALL, 5);
	sbSizer27->Add (m_checkBox57,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));
	
	m_checkBox58 = new wxCheckBox (sbSizer27->GetStaticBox(),
												IDC_GraphData,
												wxT("Graph data values"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	//sbSizer27->Add(m_checkBox58, 0, wxALL, 5);
	sbSizer27->Add (m_checkBox58, wxSizerFlags(0).Border(wxALL, 5));
	
   SetUpToolTip (m_comboBox30, IDS_ToolTip53);
   SetUpToolTip (m_checkBox55, IDS_ToolTip54);
   SetUpToolTip (m_checkBox56, IDS_ToolTip55);
   SetUpToolTip (m_checkBox57, IDS_ToolTip56);
   SetUpToolTip (m_checkBox58, IDS_ToolTip57);
   
	wxBoxSizer* bSizer180;
	bSizer180 = new wxBoxSizer (wxHORIZONTAL);
	m_staticText216 = new wxStaticText (sbSizer27->GetStaticBox(),
													IDC_NumberDecimalPlacesPrompt,
													wxT("Number decimal places to use:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText216->Wrap (-1);
	//m_staticText216->Hide ();
	bSizer180->Add (m_staticText216,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	wxString numdecstring = wxString::Format (wxT("%ld"), m_numberDecimalPlaces);
	m_numberdec = new wxTextCtrl (sbSizer27->GetStaticBox(),
											IDC_NumberDecimalPlaces,
											numdecstring,
											wxDefaultPosition,
											wxSize (40, -1),
											0);
	m_numberdec->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_numdecString));
	bSizer180->Add (m_numberdec,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));
	
	//sbSizer27->Add (bSizer180, 0, wxALL, 5);
	sbSizer27->Add (bSizer180, wxSizerFlags(0).Border(wxALL, 5));
	
	//gSizer13->Add (sbSizer27, 0, wxALL|wxEXPAND, 5);
	//gSizer13->Add (sbSizer27, wxSizerFlags(0).Expand().Border(wxTOP|wxRIGHT|wxBOTTOM, 6));
   //gSizer13->Add (sbSizer27, 0, wxTOP|wxRIGHT|wxBOTTOM, 6);
   gSizer13->Add (sbSizer27, wxSizerFlags(0).Border(wxTOP|wxRIGHT|wxBOTTOM, 6));
	
	wxStaticBoxSizer* sbSizer281;
	sbSizer281 = new wxStaticBoxSizer (new wxStaticBox (this,
																			wxID_ANY,
																			wxT("Output results to:")),
													wxVERTICAL);
	
	m_checkBox59 = new wxCheckBox (sbSizer281->GetStaticBox(),
												IDC_TextWindow,
												wxT("Text window  "),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox59, IDS_ToolTip59);
	m_checkBox59->SetValue (true);
	//sbSizer281->Add (m_checkBox59, 0, wxALL, 5);
	sbSizer281->Add (m_checkBox59, wxSizerFlags(0).Border(wxALL, 5));
	
	m_checkBox60 = new wxCheckBox (sbSizer281->GetStaticBox(),
												IDC_DiskFile,
												wxT("Disk file"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox60, IDS_ToolTip58);
	//sbSizer281->Add (m_checkBox60, 0, wxALL, 5);
	sbSizer281->Add (m_checkBox60, wxSizerFlags(0).Border(wxALL, 5));
	
	//gSizer13->Add (sbSizer281, 0, wxALL|wxEXPAND, 5);
	gSizer13->Add (sbSizer281,
						wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxBOTTOM, 6));
	
	//bSizer157->Add (gSizer13, 0, wxALL|wxEXPAND, 5);
	bSizer155->Add (gSizer13, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	//bSizer157->Add(sbSizer27, 0, wxALL|wxEXPAND, 5);
	/*
	wxBoxSizer* bSizer168;
	bSizer168 = new wxBoxSizer (wxHORIZONTAL);
	
	m_button46 = new wxButton (this,
										wxID_CANCEL,
										wxT("Cancel"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
	//bSizer168->Add(m_button46, 0, wxALL, 5);
	bSizer168->Add (m_button46, wxSizerFlags(0).Border(wxRIGHT, 6));
	
	m_button47 = new wxButton (this,
										wxID_OK,
										wxT("OK"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
	//bSizer168->Add (m_button47, 0, wxALL, 5);
	bSizer168->Add (m_button47, wxSizerFlags(0));
	
	//bSizer157->Add (bSizer168, wxSizerFlags(0).Right());
	bSizer155->Add (bSizer168,
							wxSizerFlags(0).Right().Border(wxLEFT|wxBOTTOM|wxRIGHT, 12));
	*/
	CreateStandardButtons (bSizer155);
	
	this->SetSizer (bSizer155);
	this->Layout ();
	bSizer155->Fit (this);
	//this-> Centre ();
	//SetSizerAndFit (bSizer155);
	
}	// end "CreateControls"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the list
//							data specification dialog box to the user and copy the
//							revised back to the list data specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			ListDataDialog in SListData.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/28/1997
//	Revised By:			Larry L. Biehl			Date: 07/19/2018

SInt16 CMListDataDialog::DoDialog (void)

{
	Boolean continueFlag = FALSE;

	SInt16 returnCode;


			// Make sure intialization has been completed.

	if (!m_initializedFlag)
																							return (FALSE);

	returnCode = ShowModal();

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
	
	m_areaFlag = ((wxCheckBox*)FindWindow (IDC_Area))->GetValue();
	if (m_areaFlag)
		{
		ShowSomeAreaSelectionItems();
		selectItem = IDC_LineStart;
		
		}	// end "if (m_areaFlag)"
	
	else	// !m_areaFlag
		{
		HideSomeAreaSelectionItems();
		selectItem = IDC_LineInterval;
		
		}	// end "else !m_areaFlag"
	
	SelectDialogItemText (this, selectItem, 0, SInt16_MAX);
	
	CheckOKButton();
	
}	// end "OnArea"



void CMListDataDialog::OnCbnSelendokListchannelsformatcombo (
				wxCommandEvent&					event)

{
	m_listDataFormatCode = ((wxComboBox*)FindWindow (
													IDC_ListChannelsFormatCombo))->GetSelection();
	
	if (m_listDataFormatCode == 0 && m_latLongPossibleFlag)
		MShowDialogItem (this, IDC_IncludeLatitudeLongitude);
	
	else // m_listDataFormatCode = 1 || !m_latLongPossibleFlag
		MHideDialogItem(this, IDC_IncludeLatitudeLongitude);
	
}	// end "OnCbnSelendokListchannelsformatcombo"



void CMListDataDialog::OnChangeDecimalPlaces (
				wxCommandEvent&					event)

{
	m_numberDecimalPlaces = (SInt32)CheckValue (IDC_NumberDecimalPlaces, 0, 9);
	
}	// end "OnChangeBlueChannel"



void CMListDataDialog::OnClasses (
				wxCommandEvent&					event)

{
	wxCheckBox* classcb = (wxCheckBox*)FindWindow(IDC_Classes);
	m_classFlag = classcb->GetValue();
	CheckClassItems (m_classFlag);
	
	CheckOKButton();
	
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

    CMDialog::OnInitDialog(event);

			// Make sure that we have the bitmaps for the entire image buttons.

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

	CheckClassItems(gListDataSpecsPtr->listClassesDataFlag);

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
		ShowSomeAreaSelectionItems();

		selectItem = IDC_LineStart;

    	}	// end "if (gListDataSpecsPtr->listSelectedAreaDataFlag)"

	else	// !gListDataSpecsPtr->listSelectedAreaDataFlag
    	{
		HideSomeAreaSelectionItems();

		selectItem = IDC_LineInterval;

    	} // end "else !gListDataSpecsPtr->listSelectedAreaDataFlag"

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

		}		// end "if (gImageFileInfoPtr->dataTypeCode == kIntegerType)"

	else		// gImageFileInfoPtr->dataTypeCode == kRealType
		{
		ShowDialogItem (this, IDC_NumberDecimalPlacesPrompt);
		ShowDialogItem (this, IDC_NumberDecimalPlaces);

		}		// end "if (gImageFileInfoPtr->dataTypeCode == kIntegerType)"
    
	if (CMListDataDialog::TransferDataToWindow())
		PositionDialogWindow();
	
	SelectDialogItemText (this, selectItem, 0, SInt16_MAX);

}	// end "OnInitDialog"



void CMListDataDialog::OnSelendokChannelCombo (
				wxCommandEvent& 					event)

{
	wxComboBox* 					channelcombo;
	
	
	HandleChannelsMenu (IDC_ChannelCombo,
								kNoTransformation,
								(SInt16)gImageWindowInfoPtr->totalNumberChannels,
								1,
								TRUE);
	
   channelcombo = (wxComboBox*)FindWindow (IDC_ChannelCombo);
   m_channelSelection = channelcombo->GetSelection ();

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
	
}	// end "OnSelendokChannelCombo"



void CMListDataDialog::OnSelendokClassCombo (
				wxCommandEvent&					event)

{
	HandleClassesMenu (&m_localNumberClasses,
            				(SInt16*)m_classListPtr,
            				1,
            				(SInt16)gProjectInfoPtr->numberStatisticsClasses,
            				IDC_ClassCombo,
            				&m_classSelection);
    
} // end "OnSelendokClassCombo"



void CMListDataDialog::OnSelAreaUpdate (
				wxUpdateUIEvent&					event)
{
	m_areaFlag = ((wxCheckBox*)FindWindow (IDC_Area))->GetValue ();
	if (m_areaFlag)
		event.Enable(true);
	
	else event.Enable(false);
    
	CheckOKButton();
	
}	// end "OnSelAreaUpdate"



bool CMListDataDialog::TransferDataFromWindow ()

{
	SInt16			returnCode = 0;
	
	
	wxCheckBox* classcb = (wxCheckBox*)FindWindow (IDC_Classes);
	wxCheckBox* traincb = (wxCheckBox*)FindWindow (IDC_Training);
	wxCheckBox* testcb = (wxCheckBox*)FindWindow (IDC_Test);
	wxCheckBox* areacb = (wxCheckBox*)FindWindow (IDC_Area);
	wxCheckBox* optwnd = (wxCheckBox*)FindWindow (IDC_TextWindow);
	wxCheckBox* opdskf = (wxCheckBox*)FindWindow (IDC_DiskFile);
	wxCheckBox* inlinecb = (wxCheckBox*)FindWindow (IDC_IncludeLineColumn);
	wxCheckBox* inlaticb = (wxCheckBox*)FindWindow (IDC_IncludeLatitudeLongitude);
	wxCheckBox* inclascb = (wxCheckBox*)FindWindow (IDC_IncludeClassField);
	wxCheckBox* graphdatacb = (wxCheckBox*)FindWindow (IDC_GraphData);
	 
	wxComboBox* classcombo = (wxComboBox*)FindWindow (IDC_ClassCombo);
   wxComboBox* channelcombo = (wxComboBox*)FindWindow (IDC_ChannelCombo);
	wxComboBox* listchannelcombo = (wxComboBox*)FindWindow (IDC_ListChannelsFormatCombo);
			
	wxTextCtrl* c_end = (wxTextCtrl*)FindWindow (IDC_ColumnEnd);
	wxTextCtrl* c_inter = (wxTextCtrl*)FindWindow (IDC_ColumnInterval);
	wxTextCtrl* c_start = (wxTextCtrl*)FindWindow (IDC_ColumnStart);
	wxTextCtrl* l_end = (wxTextCtrl*)FindWindow (IDC_LineEnd);
	wxTextCtrl* l_inter = (wxTextCtrl*)FindWindow (IDC_LineInterval);
	wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
	wxTextCtrl* num_dec_place = (wxTextCtrl*)FindWindow (IDC_NumberDecimalPlaces);

	m_LineStartString = l_start->GetValue();
	m_LineEndString = l_end->GetValue();
	m_LineIntervalString = l_inter->GetValue();
	m_ColumnStartString = c_start->GetValue();
	m_ColumnEndString = c_end->GetValue();
	m_ColumnIntervalString = c_inter->GetValue();
	m_numdecString = num_dec_place ->GetValue();

	m_LineStart = wxAtoi(m_LineStartString);
	m_LineEnd = wxAtoi(m_LineEndString);
	m_LineInterval = wxAtoi(m_LineIntervalString);
	m_ColumnStart = wxAtoi(m_ColumnStartString);
	m_ColumnEnd = wxAtoi(m_ColumnEndString);
	m_ColumnInterval = wxAtoi(m_ColumnIntervalString);
	m_numberDecimalPlaces = wxAtoi(m_numdecString);

	m_classSelection = classcombo->GetSelection();
   m_channelSelection = channelcombo->GetSelection();
	m_listDataFormatCode = listchannelcombo->GetSelection();
	m_areaFlag = areacb->GetValue();
	m_classFlag = classcb->GetValue();
	m_trainingFlag = traincb->GetValue();
	m_testFlag = testcb->GetValue();
	m_textWindowFlag = optwnd->GetValue();
	m_diskFileFlag = opdskf->GetValue();
	m_graphDataFlag = graphdatacb->GetValue();
	m_includeClassFieldFlag = inclascb->GetValue();
	m_includeLineColumnFlag = inlinecb->GetValue();
	m_includeLatLongFlag = inlaticb->GetValue();

   if (m_classSelection < 0)
      m_classSelection = m_classSelection_Saved;
	
   if (m_channelSelection < 0)
      m_channelSelection = m_channelSelection_Saved;
   
	if (m_areaFlag)
		returnCode = VerifyLineColumnValues (IDC_LineStart,
															IDC_ColumnStart,
															true);

	return (returnCode == 0);
	
}	// end "TransferDataFromWindow"



bool CMListDataDialog::TransferDataToWindow ()

{
	wxCheckBox* classcb = (wxCheckBox*)FindWindow (IDC_Classes);
	wxCheckBox* traincb = (wxCheckBox*)FindWindow (IDC_Training);
	wxCheckBox* testcb = (wxCheckBox*)FindWindow (IDC_Test);
	wxCheckBox* areacb = (wxCheckBox*)FindWindow (IDC_Area);
	wxCheckBox* optwnd = (wxCheckBox*)FindWindow (IDC_TextWindow);
	wxCheckBox* opdskf = (wxCheckBox*)FindWindow (IDC_DiskFile);
	wxCheckBox* inlinecb = (wxCheckBox*)FindWindow (IDC_IncludeLineColumn);
	wxCheckBox* inlaticb = (wxCheckBox*)FindWindow (IDC_IncludeLatitudeLongitude);
	wxCheckBox* inclascb = (wxCheckBox*)FindWindow (IDC_IncludeClassField);
	wxCheckBox* graphdatacb = (wxCheckBox*)FindWindow (IDC_GraphData);
			
	wxComboBox* listchannelcombo = (wxComboBox*)FindWindow (IDC_ListChannelsFormatCombo);
	wxComboBox* classcombo = (wxComboBox*)FindWindow (IDC_ClassCombo);
	wxComboBox* channelcombo = (wxComboBox*)FindWindow (IDC_ChannelCombo);

	wxTextCtrl* c_end = (wxTextCtrl*)FindWindow (IDC_ColumnEnd);
	wxTextCtrl* c_inter = (wxTextCtrl*)FindWindow (IDC_ColumnInterval);
	wxTextCtrl* c_start = (wxTextCtrl*)FindWindow (IDC_ColumnStart);
	wxTextCtrl* l_end = (wxTextCtrl*)FindWindow (IDC_LineEnd);
	wxTextCtrl* l_inter = (wxTextCtrl*)FindWindow (IDC_LineInterval);
	wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
	wxTextCtrl* num_dec_place = (wxTextCtrl*)FindWindow (IDC_NumberDecimalPlaces);

	//wxStaticText* fname = (wxStaticText*)FindWindow (IDC_FileName);
	//wxStaticText* dfname = (wxStaticText*)FindWindow (IDC_DefaultFile);

	c_end->ChangeValue(wxString::Format (wxT("%i"), m_ColumnEnd));
	c_inter->ChangeValue(wxString::Format (wxT("%i"), m_ColumnInterval));
	c_start->ChangeValue(wxString::Format (wxT("%i"), m_ColumnStart));
	l_end->ChangeValue(wxString::Format (wxT("%i"), m_LineEnd));
	l_inter->ChangeValue(wxString::Format (wxT("%i"), m_LineInterval));
	l_start->ChangeValue(wxString::Format (wxT("%i"), m_LineStart));
	num_dec_place->ChangeValue(wxString::Format (wxT("%ld"), m_numberDecimalPlaces));

	classcombo->SetSelection (m_classSelection);
	listchannelcombo->SetSelection (m_listDataFormatCode);
	channelcombo->SetSelection (m_channelSelection);

	graphdatacb->SetValue (m_graphDataFlag);
	inclascb->SetValue (m_includeClassFieldFlag);
	inlinecb->SetValue (m_includeLineColumnFlag);

	if (DetermineIfLatLongPossible (gListDataSpecsPtr->windowInfoHandle))
		{
		inlaticb-> SetValue(m_includeLatLongFlag);
		m_latLongPossibleFlag = true;

		}	// end "if (DetermineIfLatLongPossible (...->windowInfoHandle))"

	else	// !DetermineIfLatLongPossible (...->windowInfoHandle)
		{
		inlaticb->SetValue (false);
		MHideDialogItem (this, IDC_IncludeLatitudeLongitude);
		m_latLongPossibleFlag = false;

		}	// end "else !DetermineIfLatLongPossible (...->windowInfoHandle))"

	areacb->SetValue (m_areaFlag);
	classcb->SetValue (m_classFlag);
	traincb->SetValue (m_trainingFlag);
	testcb->SetValue (m_testFlag);
	optwnd->SetValue (m_textWindowFlag);
	opdskf->SetValue (m_diskFileFlag);

	return true;
	
}	// end "TransferDataToWindow"
