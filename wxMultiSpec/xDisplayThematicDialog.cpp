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
//	File:						xDisplayThematicDialog.cpp : class implementation file
//	Class Definition:		xDisplayThematicDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/09/2020
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMDisplayThematicDlg class.
//
// Following is template for debugging
/*
		int numberChars = sprintf ((char*)gTextString3,
									 " xDisplayThematicDialog:: (): %s",
									 gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "SDisplay_class.h" 
#include "SImageWindow_class.h"

#include "xClassGroupDialog.h"
#include "xDisplayThematicDialog.h"  
#include "xImageView.h"  
#include "xLegendView.h"

#include "wx/window.h"
#include "wx/valnum.h"



BEGIN_EVENT_TABLE (CMDisplayThematicDlg, CMDialog)
	EVT_BUTTON (IDC_BackgroundColor, CMDisplayThematicDlg::OnBackgroundColor)
	EVT_BUTTON (IDEntireImage, CMDisplayThematicDlg::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMDisplayThematicDlg::ToSelectedImage)

	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_ClassesGroupsCombo, CMDisplayThematicDlg::OnClassesGroupsComboSelendok)
		EVT_COMBOBOX (IDC_PaletteCombo, CMDisplayThematicDlg::OnPaletteComboSelendok)
	#endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_ClassesGroupsCombo, CMDisplayThematicDlg::OnClassesGroupsComboSelendok)
		EVT_CHOICE (IDC_PaletteCombo, CMDisplayThematicDlg::OnPaletteComboSelendok)
	#endif

	EVT_COMBOBOX_DROPDOWN (IDC_ClassesGroupsCombo, CMDisplayThematicDlg::OnClassesGroupsComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_PaletteCombo, CMDisplayThematicDlg::OnPaletteComboDropDown)

	EVT_INIT_DIALOG (CMDisplayThematicDlg::OnInitDialog)

	EVT_TEXT (ID3C_ColumnEnd, CMDisplayThematicDlg::CheckColumnEnd)
	EVT_TEXT (ID3C_ColumnStart, CMDisplayThematicDlg::CheckColumnStart)
	EVT_TEXT (ID3C_ColumnInterval, CMDisplayThematicDlg::CheckColumnInterval)
	EVT_TEXT (ID3C_LineEnd, CMDisplayThematicDlg::CheckLineEnd)
	EVT_TEXT (ID3C_LineStart, CMDisplayThematicDlg::CheckLineStart)
	EVT_TEXT (ID3C_LineInterval, CMDisplayThematicDlg::CheckLineInterval)
END_EVENT_TABLE ()



CMDisplayThematicDlg::CMDisplayThematicDlg (void)

{

}	// end "CMDisplayThematicDlg"



CMDisplayThematicDlg::CMDisplayThematicDlg (
				wxWindow*							pParent, 
				wxWindowID							id, 
				const wxString&					title, 
				const wxPoint&						pos, 
				const wxSize&						size, 
				long									style)
		: CMDialog (CMDisplayThematicDlg::IDD, pParent, title)

{
	FileStringPtr						fileNamePtr;
	
	
	m_classGroupSelection = 0;
	m_classSelection = 0;
	m_showLegendFlag = FALSE;
	m_paletteSelection = 0;
	m_numberClasses = "";
	m_numberGroups = "";
	m_fileName = "";
	m_magnification = 1.;
	m_useThresholdFileFlag = FALSE;
	m_includeVectorOverlaysFlag = FALSE;

	m_localClassGroupsHandle = NULL;
	m_localClassToGroupPtr = NULL;
	m_localClassGroupsPtr = NULL;

	m_initializedFlag = CMDialog::m_initializedFlag;

	CreateControls ();

	if (m_initializedFlag) 
		{
				//	Set the image file name
				
		fileNamePtr = (FileStringPtr)(gActiveImageViewCPtr->GetImageWindowCPtr())->
																			GetImageFileNamePointer ();
		if (fileNamePtr != NULL)
			m_fileName = wxString (fileNamePtr, wxConvUTF8);
		wxStaticText * fname = (wxStaticText*)FindWindow (IDC_FileName);
		fname->SetLabel (m_fileName);

		}	// end "if (m_initializedFlag)"

	if (m_initializedFlag) 
		{
				// Get pointer to local class to group list.									

		m_localClassGroupsHandle = MNewHandle (
				 (SInt32)gImageFileInfoPtr->numberClasses * 2 * sizeof (SInt16));

		m_initializedFlag = (m_localClassGroupsHandle != NULL);

		}	// end "if (m_initializedFlag)"

	if (m_initializedFlag) 
		{
		m_localClassGroupsPtr = (SInt16*)GetHandlePointer (
																m_localClassGroupsHandle, kLock);
		}	// end "if (m_initializedFlag)"                   

	Centre ();

}	// end "CMDisplayThematicDlg" 



CMDisplayThematicDlg::~CMDisplayThematicDlg (void)

{
	m_localClassGroupsHandle = UnlockAndDispose (m_localClassGroupsHandle);

	m_localClassToGroupPtr = CheckAndDisposePtr (m_localClassToGroupPtr);

}	// end "~CMDisplayThematicDlg"



void CMDisplayThematicDlg::CreateControls ()

{
	SetSizeHints (wxDefaultSize, wxDefaultSize);

	bSizer18 = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer (wxHORIZONTAL);

	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer (wxVERTICAL);

	m_staticText30 = new wxStaticText (this,
													IDC_FileName,
													wxT("file_name"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText30->Wrap	(-1);
	bSizer25->Add (m_staticText30, wxSizerFlags(0).Border(wxBOTTOM, 5));

	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer (new wxStaticBox (this,
																		IDC_LineColFrame,
																		wxT("Area to Display"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL),
												wxHORIZONTAL);
	
	CreateLineColumnControls (sbSizer8);

	bSizer25->Add (sbSizer8, wxSizerFlags(0).Expand().Border(wxTOP|wxRIGHT|wxBOTTOM,5));

	bSizer19->Add (bSizer25, wxSizerFlags(1).Expand().Border(wxTOP|wxRIGHT|wxBOTTOM,5));

	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer (wxVERTICAL);

	m_staticText31 = new wxStaticText (this,
													wxID_ANY,
													wxT("Magnification"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText31->Wrap	(-1);
   SetUpToolTip	(m_staticText31, IDS_ToolTip33);
	bSizer26->Add	(m_staticText31, 0, wxALL, 5);

	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer	(wxHORIZONTAL);

	m_staticText38 = new wxStaticText	(this,
														wxID_ANY,
														wxT("x"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	m_staticText38->Wrap (-1);
	bSizer34->Add (m_staticText38, 0, wxALIGN_CENTER | wxALL, 5);

	wxFloatingPointValidator<double> _val (3, &m_magnification);
	_val.SetMin (0);
	m_textCtrl14 = new wxTextCtrl (this,
												IDC_Magnification,
												wxT("1"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_textCtrl14, IDS_ToolTip33);
	m_textCtrl14->SetValidator (_val);
	bSizer34->Add (m_textCtrl14, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,5));

	bSizer26->Add (bSizer34, wxSizerFlags(0).Expand());

	bSizer19->Add (bSizer26, wxSizerFlags(0).Center().Border(wxLEFT|wxTOP|wxBOTTOM,5));

	bSizer18->Add (bSizer19, wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT,12));

	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer (wxHORIZONTAL);

	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer (new wxStaticBox (this,
																		wxID_ANY,
																		wxT("Options")),
													wxVERTICAL);

	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText36 = new wxStaticText (sbSizer9->GetStaticBox (),
													wxID_ANY,
													wxT("Palette:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText36->Wrap (-1);
   SetUpToolTip (m_staticText36, IDS_ToolTip41);
	bSizer31->Add (m_staticText36,
						wxSizerFlags(0).Center().Border(wxTOP|wxRIGHT|wxBOTTOM,5));

	GetPaletteMenuControl (kPaletteMenu,
										sbSizer9->GetStaticBox (),
										IDC_PaletteCombo,
										-1,
										IDS_ToolTip41);
	bSizer31->Add (m_paletteCtrl, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 5));

	sbSizer9->Add (bSizer31, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT,5));

	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText37 = new wxStaticText (sbSizer9->GetStaticBox (),
													wxID_ANY,
													wxT("Display:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText37->Wrap (-1);
   SetUpToolTip (m_staticText37, IDS_ToolTip42);
	bSizer32->Add (m_staticText37,
						wxSizerFlags(0).Center().Border(wxLEFT|wxTOP|wxRIGHT|wxBOTTOM,5));

	m_classGroupCtrl = new wxChoice (sbSizer9->GetStaticBox (),
												IDC_DisplayCombo,
												wxDefaultPosition,
												wxDefaultSize);
	m_classGroupCtrl->Append (wxT("Classes"));
	m_classGroupCtrl->Append (wxT("Information groups"));
   SetUpToolTip (m_classGroupCtrl, IDS_ToolTip42);
	bSizer32->Add (m_classGroupCtrl, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,5));

	sbSizer9->Add (bSizer32, wxSizerFlags(0).Expand().Border(wxRIGHT,5));

	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText23 = new wxStaticText (sbSizer9->GetStaticBox (),
													wxID_ANY,
													wxT("Display classes/groups:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText23->Wrap (-1);
	bSizer22->Add (m_staticText23,
						wxSizerFlags(0).Center().Border(wxLEFT|wxTOP|wxRIGHT|wxBOTTOM,5));

	GetAllSubsetMenuControl (kClassesMenu,
										sbSizer9->GetStaticBox (),
										IDC_ClassesGroupsCombo,
										120,
										IDS_ToolTip79);
	bSizer22->Add (m_classesCtrl, wxSizerFlags(0).Center().Border(wxTOP|wxBOTTOM, 5));

	sbSizer9->Add (bSizer22, wxSizerFlags(0).Expand().Border(wxRIGHT, 90));
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer (wxHORIZONTAL);

	m_button3 = new wxButton (sbSizer9->GetStaticBox (),
										IDC_BackgroundColor,
										wxT("Background color..."),
										wxDefaultPosition,
										wxDefaultSize,
										0);
   SetUpToolTip (m_button3, IDS_ToolTip43);
	bSizer23->Add (m_button3, wxSizerFlags(0).Border(wxTOP|wxRIGHT|wxBOTTOM,5));

	m_bitmap1 = new wxStaticBitmap (sbSizer9->GetStaticBox (),
												IDC_BackgroundColorChip,
												wxNullBitmap,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer23->Add (m_bitmap1, wxSizerFlags(0).Center().Border(wxTOP|wxBOTTOM, 5));

	sbSizer9->Add (bSizer23, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 5));

	display_legend = new wxCheckBox (sbSizer9->GetStaticBox (),
												IDC_DisplayLegend,
												wxT("Display legend"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (display_legend, IDS_ToolTip44);
	
	sbSizer9->Add (display_legend, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxBOTTOM, 5));
	
	wxCheckBox* checkBox17 = new wxCheckBox (sbSizer9->GetStaticBox (),
															IDC_VectorOverlays,
															wxT("Include Vector Overlays"),
															wxDefaultPosition,
															wxDefaultSize,
															0);
   SetUpToolTip (checkBox17, IDS_ToolTip31);
	sbSizer9->Add (checkBox17, 0, wxALL, 5);

	bSizer21->Add (sbSizer9,
						wxSizerFlags(0).Expand().Border(wxTOP|wxRIGHT|wxBOTTOM, 5));
	
			// Second column
	
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText32 = new wxStaticText (this,
													wxID_ANY,
													wxT("Number of classes:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText32->Wrap (-1);
   SetUpToolTip (m_staticText32, IDS_ToolTip45);
	bSizer29->Add (m_staticText32, 0, wxALL, 5);

	m_staticText33 = new wxStaticText (this,
													IDC_NumberClasses,
													wxT("0"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_staticText33, IDS_ToolTip45);
	m_staticText33->Wrap (-1);
	bSizer29->Add (m_staticText33, 0, wxALL, 5);

	bSizer28->Add (bSizer29, 0, wxALL | wxEXPAND, 5);

	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText34 = new wxStaticText (this,
													wxID_ANY,
													wxT("Number of groups:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText34->Wrap (-1);
   SetUpToolTip (m_staticText34, IDS_ToolTip46);
	bSizer30->Add (m_staticText34, 0, wxALL, 5);

	m_staticText35 = new wxStaticText (this,
													IDC_NumberGroups,
													wxT("0"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_staticText35, IDS_ToolTip46);
	m_staticText35->Wrap (-1);
	bSizer30->Add (m_staticText35, wxSizerFlags(0).Border(wxTOP|wxBOTTOM|wxRIGHT, 5));

	bSizer28->Add (bSizer30, wxSizerFlags(0).Expand().Border(wxALL,5));

	bSizer21->Add (bSizer28, wxSizerFlags(0).Expand().Border(wxALL,5));

	bSizer18->Add (bSizer21, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT,12));

	CreateStandardButtons (bSizer18);

	SetSizerAndFit (bSizer18);
	
}	// end "CreateControls"



//------------------------------------------------------------------------------------
//                   Copyright 2009-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the display
//							specification dialog box to the user and copy the
//							revised back to the display specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			DisplayThematicDialog in SDisplayThematic.cpp
//
//	Coded By:			Abdur R. Maud			Date: ??/??/2009
//	Revised By:			Larry L. Biehl			Date: 02/16/2016	

Boolean CMDisplayThematicDlg::DoDialog (
				DisplaySpecsPtr					displaySpecsPtr)

{
	Boolean continueFlag = FALSE;

	SInt16 returnCode;


	// Make sure intialization has been completed.

	if (!m_initializedFlag)
																							return (FALSE);

	m_displaySpecsPtr = displaySpecsPtr;

	returnCode = ShowModal ();

	if (returnCode == wxID_OK) 
		{
				// Display area													

		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;

		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;

		gCurrentSelectedColor = m_oldRGB;

		DisplayThematicDialogOK (m_magnification,
										 displaySpecsPtr,
										 &m_dialogSelectArea,
										 m_localClassGroupsPtr,
										 m_localClassGroupsHandle,
										 m_localDisplayClasses,
										 m_localDisplayGroups,
										 m_localAllSubsetClass,
										 m_localAllSubsetGroup,
										 m_localPaletteUpToDateFlag,
										 m_showLegendFlag,
										 m_includeVectorOverlaysFlag);

				// Get maximum magnification value to use.

		displaySpecsPtr->maxMagnification = CMDisplay::GetMaxZoomValue (displaySpecsPtr);

				// Magnification to use										

		displaySpecsPtr->magnification =
				 MIN (m_magnification, displaySpecsPtr->maxMagnification);
		displaySpecsPtr->magnification =
				 MAX (gMinMagnification, displaySpecsPtr->magnification);

				// Update the combo list in the legend. 

		CMLegendView* legendViewCPtr = gActiveImageViewCPtr->GetImageLegendViewCPtr ();
		legendViewCPtr->UpdateClassGroupComboList (gClassGroupSelection);

		continueFlag = TRUE;

		}	// end "if (returnCode == IDOK)"

	return (continueFlag);

}	// end "DoDialog"



void CMDisplayThematicDlg::OnBackgroundColor (
				wxCommandEvent& 					event)

{
	RGBColor 							newRGB;


	if (SelectColor (3, &m_oldRGB, &newRGB))
		{
		m_oldRGB = newRGB;

		Refresh (FALSE, NULL);

		wxColour* wxicolor = new wxColour (*wxWHITE);
		wxicolor->Set ((UChar)m_oldRGB.red, (UChar)m_oldRGB.green, (UChar)m_oldRGB.blue);

		m_displaySpecsPtr->backgroundColor = m_oldRGB;

		wxMemoryDC imdc;
		wxBrush cbrush (*wxicolor);
		wxBitmap legrect (16, 16);
		imdc.SelectObject (legrect);
		imdc.SetBrush (cbrush);
		imdc.SetBackground (cbrush);
		wxRect rectDraw (0, 0, 16, 16);
		imdc.DrawRectangle (rectDraw);
		m_bitmap1->SetBitmap (legrect);

				// To indicate the color has been changed.
		
		m_localPaletteUpToDateFlag = FALSE;
		
		}	// end "if (SelectColor (3, ..."

}	// end "OnBackgroundColor"



void CMDisplayThematicDlg::OnClassesGroupsComboDropDown (
				wxCommandEvent& 					event)

{
	m_classesCtrl->SetSelection (0);
	m_classSelection = 0;
	m_localPaletteUpToDateFlag = FALSE;

	m_classGroupSelection = m_classGroupCtrl->GetSelection ();

			// Adjust for an offset of 1 not 0
	
	m_classGroupSelection++;

	if (m_classGroupSelection == kClassDisplay)
		m_localAllSubsetClass = m_classSelection + 1;

	else // m_classGroupSelection == kGroupDisplay
		m_localAllSubsetGroup = m_classSelection + 1;
	
}	// end "OnClassesGroupsComboDropDown"



void CMDisplayThematicDlg::OnClassesGroupsComboSelendok (
				wxCommandEvent& 					event)

{
	Boolean returnFlag = FALSE;


			// Determine whether all or subset of classes/groups are
			// was selected.

	m_classSelection = m_classesCtrl->GetSelection ();

			// Determine whether classes or groups are being used.

	m_classGroupSelection = m_classGroupCtrl->GetSelection ();

			// Adjust for an offset of 1 not 0
	
	m_classGroupSelection++;

	if (m_classSelection == kSubsetMenuItem)
		{
				// Subset of classes or groups to be used.

		SetDLogControlHilite (NULL, wxID_OK, 255);

		CMClassGroupDlg* classesGroupsDialogPtr = NULL;

		classesGroupsDialogPtr = new CMClassGroupDlg (this);

		if (m_classGroupSelection == kClassDisplay)
			{
			returnFlag = classesGroupsDialogPtr->DoDialog (
													  &m_localDisplayClasses,
													  m_localClassGroupsPtr,
													  1,
													  (SInt16)gImageFileInfoPtr->numberClasses,
													  kClassDisplay);

			if (m_localDisplayClasses == gImageFileInfoPtr->numberClasses)
				{
				m_classSelection = kAllMenuItem;
				m_classSelection = m_classesCtrl->GetSelection ();

				}	// end "if (m_localDisplayClasses == ..."

			}	// end "if (m_classGroupSelection == kClassDisplay)"

		if (m_classGroupSelection == kGroupDisplay)
			{
			returnFlag = classesGroupsDialogPtr->DoDialog (
									&m_localDisplayGroups,
									&m_localClassGroupsPtr[gImageFileInfoPtr->numberClasses],
									1,
									m_localNumberGroups,
									kGroupDisplay);

			if (m_localDisplayGroups == (UInt16) m_localNumberGroups)
				m_classSelection = kAllMenuItem;

			}	// end "if (m_classGroupSelection == kGroupDisplay)"

		if (classesGroupsDialogPtr != NULL)
			delete classesGroupsDialogPtr;
		
		}	// end "if (m_classSelection == kSubsetMenuItem)"

	SetDLogControlHilite (NULL, wxID_OK, 0);

	if (returnFlag)
		m_localPaletteUpToDateFlag = FALSE;

	if (m_classGroupSelection == kClassDisplay)
		m_localAllSubsetClass = m_classSelection + 1;

	else // m_classGroupSelection == kGroupDisplay
		m_localAllSubsetGroup = m_classSelection + 1;

}	// end "OnClassesGroupsComboSelendok"



void CMDisplayThematicDlg::OnInitDialog (
				wxInitDialogEvent&				event)

{
	SInt32								numberClasses,
											numberGroups;

	SInt16								classGroupSelection,
											classSelection;

	Boolean								includeVectorOverlaysFlag,
											showLegendFlag,
											useThresholdFileFlag;


	wxDialog::OnInitDialog (event);

	DisplayThematicDialogInitialize (this,
												m_displaySpecsPtr,
												gImageWindowInfoPtr,
												gImageFileInfoPtr,
												&m_dialogSelectArea,
												&m_magnification,
												&classGroupSelection,
												&m_paletteChangedFlag,
												&m_localPaletteUpToDateFlag,
												m_localClassGroupsPtr,
												&m_localDisplayClasses,
												&m_localDisplayGroups,
												&m_localAllSubsetClass,
												&m_localAllSubsetGroup,
												&classSelection,
												&m_oldRGB,
												&showLegendFlag,
												&useThresholdFileFlag,
												&numberClasses,
												&numberGroups,
												&includeVectorOverlaysFlag);
	
	m_classGroupSelection = classGroupSelection - 1;
	m_classSelection = classSelection - 1;

	m_showLegendFlag = showLegendFlag;
	m_useThresholdFileFlag = useThresholdFileFlag;
	m_includeVectorOverlaysFlag = includeVectorOverlaysFlag;

	m_LineStart = m_displaySpecsPtr->lineStart;
	m_LineEnd = m_displaySpecsPtr->lineEnd;
	m_LineInterval = m_displaySpecsPtr->lineInterval;
	m_ColumnStart = m_displaySpecsPtr->columnStart;
	m_ColumnEnd = m_displaySpecsPtr->columnEnd;
	m_ColumnInterval = m_displaySpecsPtr->columnInterval;

	m_oldRGB = m_displaySpecsPtr->backgroundColor;
	m_numberClasses = wxString::Format (wxT("%i"),
													(SInt32)m_displaySpecsPtr->numberLevels);
	m_numberGroups =
			wxString::Format (wxT("%i"), (SInt32)gImageFileInfoPtr->numberGroups);

	SetEntireImageButtons (NULL,
									m_LineStart,
									m_LineEnd,
									m_ColumnStart,
									m_ColumnEnd);

			// Update the default palette selection in the combo base on the
			// items actually in the list.

	m_paletteSelection = GetComboListSelection (IDC_PaletteCombo,
																gPaletteSelection);
	
	if (TransferDataToWindow ())
		PositionDialogWindow ();

	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);

}	// end "OnInitDialog"



void CMDisplayThematicDlg::OnPaletteComboDropDown (
				wxCommandEvent& 					event)

{
	m_paletteSelection = m_paletteCtrl->GetSelection ();
	
			// The drop down is forced to be -1 setting with each combo
			// box selection so that the 'user selection' box will be opened if
			// it is the last selection. Otherwise it will not open the dialog window.

	m_paletteCtrl->SetSelection (-1);
	
}	// end "OnPaletteComboDropDown"



void CMDisplayThematicDlg::OnPaletteComboSelendok (
				wxCommandEvent& 					event)

{
	int									lastPaletteSelection;


	lastPaletteSelection = m_paletteSelection;

	m_paletteSelection = m_paletteCtrl->GetSelection ();

   SInt64 palette64 =
					(SInt64)((int*)m_paletteCtrl->GetClientData (m_paletteSelection));
   gPaletteSelection = (SInt16)palette64;

	if (gPaletteSelection == kFalseColors)
		{
		if (FalseColorPaletteDialog ())
			m_localPaletteUpToDateFlag = FALSE;

		else	// !FalseColorPaletteDialog ()
			{
			if (lastPaletteSelection != m_paletteSelection)
				{
				m_paletteSelection = lastPaletteSelection;
				m_paletteCtrl->SetSelection (m_paletteSelection);
				
				}	// end "if (lastPaletteSelection != m_paletteSelection)"

			}	// end "else !FalseColorPaletteDialog ()"

		}	// end "if (gPaletteSelection == kFalseColors)"

	if (!m_localPaletteUpToDateFlag || lastPaletteSelection != m_paletteSelection)
		m_paletteChangedFlag = TRUE;

}	// end "OnPaletteComboSelendok"



bool CMDisplayThematicDlg::TransferDataFromWindow ()

{
	SInt16								returnCode;
	
	
	TransferLinesColumnsFromWindow ();

	wxTextCtrl* magnify = (wxTextCtrl*)FindWindow (IDC_Magnification);
	wxString magnstring = magnify->GetValue ();
	magnstring.ToDouble (&m_magnification);

	m_paletteSelection = m_paletteCtrl->GetSelection ();
	m_paletteSelection = m_paletteSelection + 1;

	m_classGroupSelection = m_classGroupCtrl->GetSelection ();
	
	m_classSelection = m_classesCtrl->GetSelection ();

	wxCheckBox* legend = (wxCheckBox*)FindWindow (IDC_DisplayLegend);
	m_showLegendFlag = legend->GetValue ();
	m_oldRGB = m_displaySpecsPtr->backgroundColor;
	
	wxCheckBox* voverlays = (wxCheckBox*)FindWindow (IDC_VectorOverlays);
	m_includeVectorOverlaysFlag = voverlays->GetValue ();

			// Adjust Class/Group selection to be 1 based.
			
	gClassGroupSelection = m_classGroupSelection + 1;

			// Adjust All or Subset of Classes/Groups selection to be 1 based.

	gClassSelection = m_classSelection + 1;

	returnCode = VerifyLineColumnValues (IDC_LineStart,
													  IDC_ColumnStart,
													  true);
					  
	if (returnCode == 0)
		{
				// Verify that the number of lines to be displayed is less than
				// the maximum possible.

		if (CheckNumberDisplayLines (m_LineStart,
												m_LineEnd,
												m_LineInterval,
												this,
												IDC_LineStart) == 0)
			returnCode = IDC_LineEnd;
			
		}	// end "if (returnCode == 0)"

	if (returnCode == 0)
		{
				// Verify that the number of columns to be displayed is less than
				// the maximum possible.
				// This part needs to be updated.

		if (CheckNumberDisplayColumns (m_ColumnStart,
													m_ColumnEnd,
													m_ColumnInterval,
													24,
													2,
													1,
													this,
													IDC_ColumnStart) == 0)
			returnCode = IDC_ColumnEnd;
			
		}	// end "if (returnCode == 0)"
	
	if (returnCode == 0)
		{
				// Check magnification setting.
		
		if (m_magnification <= 0 || m_magnification > 99)
			{
			DisplayAlert (kErrorAlertID,
								kStopAlert,
								kAlertStrID,
								IDS_Alert152,
								0,
								NULL);
			
			returnCode = IDC_Magnification;
			
			SelectDialogItemText (this, IDC_Magnification, 0, SInt16_MAX);
			
			}	// end "if (m_magnification <= 0 || m_magnification > 99)"
			
		}	// end "if (returnCode == 0)"
	
	return (returnCode == 0);
	 
}	// end "TransferDataFromWindow"



bool CMDisplayThematicDlg::TransferDataToWindow ()

{
	TransferLinesColumnsToWindow ();

	wxTextCtrl* magnify = (wxTextCtrl*)FindWindow (IDC_Magnification);
	magnify->ChangeValue (wxString::Format (wxT("%.3f"), m_magnification));
	
	wxCheckBox* voverlays = (wxCheckBox*)FindWindow (IDC_VectorOverlays);
	voverlays->SetValue (m_includeVectorOverlaysFlag);

	m_paletteCtrl->SetSelection (m_paletteSelection);
	wxColour* wxicolor = new wxColour (*wxWHITE);
	wxicolor->Set ((UChar)m_oldRGB.red, (UChar)m_oldRGB.green, (UChar)m_oldRGB.blue);
	wxMemoryDC imdc;
	wxBrush cbrush (*wxicolor);
	wxBitmap legrect (16, 16);
	imdc.SelectObject (legrect);
	imdc.SetBrush (cbrush);
	imdc.SetBackground (cbrush);
	wxRect rectDraw (0, 0, 16, 16);
	imdc.DrawRectangle (rectDraw);
	m_bitmap1->SetBitmap (legrect);

	m_classGroupCtrl->SetSelection (m_classGroupSelection);
	
	m_classesCtrl->SetSelection (m_classSelection);

	wxCheckBox* legend = (wxCheckBox*)FindWindow (IDC_DisplayLegend);
	legend->SetValue (m_showLegendFlag);
	
	wxStaticText* file_name = (wxStaticText*)FindWindow (IDC_FileName);
	file_name->SetLabel (m_fileName);
	
	wxStaticText* num_class = (wxStaticText*)FindWindow (IDC_NumberClasses);
	num_class->SetLabel (m_numberClasses);
	
	wxStaticText* num_group = (wxStaticText*)FindWindow (IDC_NumberGroups);
	num_group->SetLabel (m_numberGroups);
	
	return true;

}	// end "TransferDataToWindow"
