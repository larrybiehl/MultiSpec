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
//   File:                 xStatisticsFrame.cpp : class implementation file
//   Class Definition:     xStatisticsFrame.h
//
//   Authors:              Abdur Rahman Maud, Larry L. Biehl
//
//   Revision date:        04/22/2020
//
//   Language:					C++
//
//   System:               Linux and MacOS Operating Systems
//
//   Brief description:  	This file contains functions that relate to the
//                       	CMStatisticsFrame class.
//
/*  Template for writing something to text window for debugging.
	int numberChars = sprintf (
				(char*)&gTextString3,
				" xStatisticsFrame::UpdateStatsTypeCombo 1: (): %s",
				gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
                      
#include "xImageView.h"
#include "xImageDoc.h"
#include "xStatisticsFrame.h"
#include "xStatisticsDoc.h"
#include "xTools.h"

#include "wx/display.h"


IMPLEMENT_DYNAMIC_CLASS (CMStatisticsFrame, wxDocChildFrame)

BEGIN_EVENT_TABLE (CMStatisticsFrame, wxDocChildFrame)
   EVT_BUTTON (IDC_AddToList, CMStatisticsFrame::OnAddToList)
   EVT_BUTTON (IDC_Class, CMStatisticsFrame::OnClass)
   EVT_BUTTON (IDC_EditName, CMStatisticsFrame::OnEditName)
   EVT_BUTTON (IDC_Field, CMStatisticsFrame::OnField)
   EVT_BUTTON (IDC_Project, CMStatisticsFrame::OnProject)
   EVT_BUTTON (IDC_Select, CMStatisticsFrame::OnSelect)
   EVT_BUTTON (IDC_Update, CMStatisticsFrame::OnUpdate)

	EVT_CHAR_HOOK (CMStatisticsFrame::OnCharHook)

   EVT_CHECKBOX (IDC_Polygon, CMStatisticsFrame::OnPolygon)

   EVT_CHOICE (IDC_ClassList, CMStatisticsFrame::OnSelendokClassList)

   #if defined multispec_wxlin
		EVT_COMBOBOX (IDC_HistogramStatsCombo, CMStatisticsFrame::OnHistogramStatsComboSelendok)
   	EVT_COMBOBOX (IDC_ListStatsCombo, CMStatisticsFrame::OnListStatsComboSelendok)
  		EVT_COMBOBOX (IDC_StatsCombo, CMStatisticsFrame::OnStatsTypeComboSelendok)
   #endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_HistogramStatsCombo, CMStatisticsFrame::OnHistogramStatsComboSelendok)
   	EVT_CHOICE (IDC_ListStatsCombo, CMStatisticsFrame::OnListStatsComboSelendok)
   	EVT_CHOICE (IDC_StatsCombo, CMStatisticsFrame::OnStatsTypeComboSelendok)
   #endif

	EVT_COMBOBOX_CLOSEUP (IDC_StatsCombo, CMStatisticsFrame::OnStatsTypeComboCloseUp)

   EVT_COMBOBOX_DROPDOWN (IDC_HistogramStatsCombo, CMStatisticsFrame::OnHistogramStatsComboDropDown)
   EVT_COMBOBOX_DROPDOWN (IDC_ListStatsCombo, CMStatisticsFrame::OnListStatsComboDropDown)
   EVT_COMBOBOX_DROPDOWN (IDC_StatsCombo, CMStatisticsFrame::OnStatsTypeComboDropDown)

   EVT_LISTBOX (IDC_ListBox, CMStatisticsFrame::OnSelchangeListBox)
   EVT_LISTBOX_DCLICK (IDC_ListBox, CMStatisticsFrame::OnDblclkListBox)

	EVT_MENU (ID_EDIT_UNDO, CMStatisticsFrame::OnEditUndo)
	EVT_MENU (wxID_CUT, CMStatisticsFrame::OnEditCut)
	EVT_MENU (wxID_PASTE, CMStatisticsFrame::OnEditPaste)
	EVT_MENU (ID_EDIT_CLEAR_SELECT_RECTANGLE, CMStatisticsFrame::OnEditClearSelectRectangle)
	EVT_MENU (ID_EDIT_SELECTION_RECTANGLE, CMStatisticsFrame::OnEditSelectionRectangle)
	EVT_MENU (ID_EDIT_SELECT_ALL, CMStatisticsFrame::OnEditSelectAll)

	EVT_UPDATE_UI (ID_EDIT_CLEAR_SELECT_RECTANGLE, CMStatisticsFrame::OnUpdateEditClearSelectRectangle)
   EVT_UPDATE_UI (ID_EDIT_SELECT_ALL, CMStatisticsFrame::OnUpdateEditSelectAll)
   EVT_UPDATE_UI (ID_EDIT_SELECTION_RECTANGLE, CMStatisticsFrame::OnUpdateEditSelectionRectangle)
END_EVENT_TABLE ()



CMStatisticsFrame::CMStatisticsFrame (void)

{

}	// end "CMStatisticsFrame"



CMStatisticsFrame::CMStatisticsFrame (
				wxDocument* 						doc,
				wxView* 								view,
				wxDocParentFrame* 				parent,
            wxWindowID 							id,
            const wxString& 					title,
				const wxPoint& 					pos,
				const wxSize& 						size,
				long 									style)
		: wxDocChildFrame (doc, view, parent, id, title, pos, size, style)

{
   m_classList = 0;
   m_histogramStatsCode = 0;
   m_listStatsCode = 0;
   m_statsTypeCode = 0;
   m_polygonFlag = FALSE;

   m_initializedFlag = FALSE;
   m_optionKeyFlag = FALSE;
   
   CreateControls2 ();
	
   		// Change statistic dialog window to the top-right screen -
   		// Tsung Tai 12/10/18
	
   int			menuHeight = 0,
   				menuWidth = 0,
   				windowHeight = 0,
   				windowWidth = 0,
   				xPosition,
   				yPosition;
	
   GetSize (&windowWidth, &windowHeight);
   xPosition = wxDisplay().GetGeometry().GetWidth () - windowWidth - 3;
   xPosition = MAX (3, xPosition);
   GetMainFrame()->m_menubar1->GetSize (&menuWidth, &menuHeight);
   yPosition = menuHeight + 3;
   Move (xPosition, yPosition);
			
	wxAcceleratorEntry entries[32];
	entries[0].Set (wxACCEL_CTRL, (int) 'O', ID_IMAGE_OPEN);
	entries[1].Set (wxACCEL_CTRL, (int) ';', ID_FILE_OPEN_PROJECT);
	entries[2].Set (wxACCEL_CTRL, (int) 'W', ID_FILE_CLOSE_WINDOW);
	entries[3].Set (wxACCEL_CTRL, (int) 'S', ID_FILE_SAVE_AS);
	entries[4].Set (wxACCEL_CTRL, (int) 'P', ID_FILE_PRINT);
	entries[5].Set (wxACCEL_CTRL, (int) 'Q', wxID_EXIT);
	
	entries[6].Set (wxACCEL_CTRL, (int) 'Z', ID_EDIT_UNDO);	// wxID_UNDO
	entries[7].Set (wxACCEL_CTRL, (int) 'X', wxID_CUT);
	entries[8].Set (wxACCEL_CTRL, (int) 'C', wxID_COPY);
	entries[9].Set (wxACCEL_CTRL, (int) 'V', wxID_PASTE);
	entries[10].Set (wxACCEL_NORMAL, (int) WXK_DELETE, wxID_CLEAR);
	entries[11].Set (wxACCEL_CTRL, (int) 'A', ID_EDIT_SELECT_ALL);
	entries[12].Set (wxACCEL_CTRL, (int) ',', ID_EDIT_IMAGE_DESCRIPTION);
	entries[13].Set (wxACCEL_CTRL, (int) '-', ID_EDIT_IMAGE_MAP_PARAMETERS);
	
	entries[14].Set (wxACCEL_CTRL, (int) 'D', ID_PROC_DISPLAY_IMAGE);
	entries[15].Set (wxACCEL_CTRL, (int) 'H', ID_PROC_HISTOGRAM_IMAGE);
	entries[16].Set (wxACCEL_CTRL, (int) 'J', ID_PROC_LISTDATA);
	entries[17].Set (wxACCEL_CTRL, (int) 'R', ID_PROC_REFORMAT_CHANGE_IMAGE);
	entries[18].Set (wxACCEL_CTRL, (int) 'L', ID_PROC_CLUSTER);
	entries[19].Set (wxACCEL_CTRL, (int) 'T', ID_PROC_STATISTICS);
	entries[20].Set (wxACCEL_CTRL, (int) 'E', ID_PROC_ENHANCE_STATISTICS);
	entries[21].Set (wxACCEL_CTRL, (int) 'F', ID_PROC_FEATURE_EXTRACTION);
	entries[22].Set (wxACCEL_CTRL, (int) 'B', ID_PROC_FEATURE_SELECTION);
	entries[23].Set (wxACCEL_CTRL, (int) 'M', ID_PROC_CLASSIFY);
	entries[24].Set (wxACCEL_CTRL, (int) 'Y', ID_PROC_LISTRESULTS);
	entries[25].Set (wxACCEL_CTRL, (int) 'E', ID_PROC_ENHANCE_STATISTICS);
	
	entries[26].Set (wxACCEL_CTRL, (int) 'K', ID_PROC_UTIL_PRIN_COMP_ANALYSIS);
	entries[27].Set (wxACCEL_CTRL, (int) 'I', ID_PROC_UTIL_CREATE_STAT_IMAGE);
	entries[28].Set (wxACCEL_CTRL, (int) 'G', ID_PROC_UTIL_BIPLOTS_OF_DATA);
	entries[29].Set (wxACCEL_CTRL, (int) 'U', ID_PROC_UTIL_LIST_IMAGE_DESC);
	entries[30].Set (wxACCEL_CTRL, (int) '[', ID_PROC_UTIL_CHECK_COVARIANCES);
	entries[31].Set (wxACCEL_CTRL, (int) ']', ID_PROC_UTIL_CHECKTRANS_MATRIX);
	wxAcceleratorTable accel (32, entries);
	SetAcceleratorTable (accel);

}	// end "CMStatisticsFrame"



CMStatisticsFrame::~CMStatisticsFrame (void)

{

}	// end "~CMStatisticsFrame"



void CMStatisticsFrame::ActivateStatisticsWindowItems (
				SInt16								statsWindowMode)

{			
	switch (statsWindowMode)
		{
		case kSelectFieldMode:
			m_classListCtrl->SetFocus ();
			break;
			
		case kClassListMode:
			m_histogramCtrl->SetFocus ();
			break;
			
		case kFieldListMode:
			m_histogramCtrl->SetFocus ();
			break;
			
		case kCoordinateListMode:
			m_histogramCtrl->SetFocus ();
			break;
			
		}	// end "switch (statsWindowMode)"

}	// end "ActivateStatisticsWindowItems"



void CMStatisticsFrame::CreateControls2 ()

{
	int	buttonWidth = 210;
	
	
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxFont  font (gFontSize,
						wxFONTFAMILY_MODERN,
						wxFONTSTYLE_NORMAL,
						wxFONTWEIGHT_NORMAL);
	wxBoxSizer* bSizer233;
	bSizer233 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer234;
	bSizer234 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText223 = new wxStaticText (this,
													IDC_ProjectName,
													wxT("Project Name"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText223->Wrap (-1);
	bSizer234->Add (m_staticText223, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_staticText224 = new wxStaticText (this,
													IDC_ProjectWindowMode,
													wxT("SELECT FIELD"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText224->Wrap (-1);
	bSizer234->Add (m_staticText224, 0, wxALIGN_CENTER, 5);
	
	bSizer233->Add (bSizer234, 0, wxEXPAND, 3);
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer (2, 1, 0, 0);
	fgSizer8->SetFlexibleDirection (wxBOTH);
	fgSizer8->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	wxBoxSizer* bSizer235 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText225 = new wxStaticText (this,
													IDC_STATIC_Class,
													wxT("Class:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText225->Wrap (-1);
	bSizer235->Add (m_staticText225,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).
																Border(wxBOTTOM|wxTOP|wxLEFT, 5));
	
	m_staticText228 = new wxStaticText (this,
													IDC_ClassName,
													wxT("Class 1"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText228->Wrap (-1);
	bSizer235->Add (m_staticText228,
							wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxBOTTOM|wxTOP, 5));
	
	m_classListCtrl = new wxChoice (this,
												IDC_ClassList,
												wxDefaultPosition,
												wxSize (125, 25));
   SetUpToolTip (m_classListCtrl, IDS_ToolTip1);
	bSizer235->Add (m_classListCtrl, wxSizerFlags(0).Align(wxALIGN_CENTER));
	
	m_staticText260 = new wxStaticText (this,
													wxID_ANY,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText260->Wrap (-1);
	bSizer235->Add (m_staticText260,
							wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxBOTTOM|wxTOP, 5));
	
	fgSizer8->Add (bSizer235, 0, wxEXPAND|wxLEFT|wxRIGHT, 6);
	
	wxBoxSizer* bSizer243 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText230 = new wxStaticText (this,
													IDC_STATIC_Field,
													wxT("Field:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText230->Wrap (-1);
	bSizer243->Add (
				m_staticText230,
				wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxBOTTOM|wxTOP|wxLEFT, 5));
	
	m_staticText231 = new wxStaticText (this,
													IDC_FieldName,
													wxT("Field 1"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText231->Wrap (-1);
	bSizer243->Add (m_staticText231,
							wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxBOTTOM|wxTOP, 5));
	
	m_staticText259 = new wxStaticText (this,
													wxID_ANY,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText259->Wrap (-1);
	bSizer243->Add (m_staticText259,
							wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxBOTTOM|wxTOP, 5));
	
	fgSizer8->Add (bSizer243, 0, wxEXPAND|wxLEFT|wxRIGHT, 6);
	
	bSizer233->Add (fgSizer8, 0, wxEXPAND, 6);
	
	wxFlexGridSizer* fgSizer12;
	fgSizer12 = new wxFlexGridSizer (2, 1, 0, 0);
	fgSizer12->SetFlexibleDirection (wxBOTH);
	fgSizer12->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	wxBoxSizer* bSizer281;
	bSizer281 = new wxBoxSizer (wxHORIZONTAL);
	
	m_checkBox53 = new wxCheckBox (this,
												IDC_Polygon,
												wxT("Polygon Enter"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox53, IDS_ToolTip2);
	bSizer281->Add (m_checkBox53, 0, wxALIGN_CENTER|wxLEFT, 5);
	
	m_staticText261 = new wxStaticText (this,
													wxID_ANY,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText261->Wrap (-1);
	bSizer281->Add (m_staticText261, 0, wxALIGN_CENTER|wxALL, 5);
	
	fgSizer12->Add (bSizer281, 0, wxEXPAND|wxLEFT|wxRIGHT, 6);
	
	m_button56 = new wxButton (this,
										IDC_EditName,
										wxT("Edit Class Name ..."),
										wxDefaultPosition,
										wxSize (buttonWidth, 25), 0);
   SetUpToolTip (m_button56, IDS_ToolTip3);
	fgSizer12->Add (m_button56, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 5));
	
	bSizer233->Add (fgSizer12, wxSizerFlags(0).Border(wxBOTTOM|wxLEFT|wxRIGHT, 3));
	
	wxFlexGridSizer* fgSizer13;
	fgSizer13 = new wxFlexGridSizer (1, 1, 0, 0);
	fgSizer13->SetFlexibleDirection (wxBOTH);
	fgSizer13->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	wxBoxSizer* bSizer236;
	bSizer236 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText226 = new wxStaticText (this,
													IDC_StatsPrompt,
													wxT("Stats:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText226->Wrap (-1);
	bSizer236->Add (m_staticText226, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_staticText227 = new wxStaticText (this,
													IDC_StatsType,
													wxT("Original"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText227->Wrap (-1);
	bSizer236->Add (m_staticText227, 0, wxALIGN_CENTER|wxALL, 5);
	
	#if defined multispec_wxlin
		m_statsCtrl = new wxComboBox (this,
												IDC_StatsCombo,
												wxT("Combo!"),
												wxDefaultPosition,
												wxSize (-1, 25),
												0,
												NULL,
												wxCB_READONLY);
	#endif	// multispec_wxlin
	#if defined multispec_wxmac
		m_statsCtrl = new wxChoice (this,
												IDC_StatsCombo,
												wxDefaultPosition,
												wxSize (-1, 25));
	#endif	// multispec_wxmac
	m_statsCtrl->Append (wxT("Original"));
	m_statsCtrl->Append (wxT("LOOC  +\u21E7..."));
	m_statsCtrl->Append (wxT("Enhanced"));
	m_statsCtrl->Append (wxT("Mixed"));
   SetUpToolTip (m_statsCtrl, IDS_ToolTip4);
   bSizer236->Add (m_statsCtrl, 0, wxALIGN_CENTER);
	
	m_staticText262 = new wxStaticText (this,
													wxID_ANY,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText262->Wrap (-1);
	bSizer236->Add (m_staticText262, 0, wxALIGN_CENTER|wxALL, 5);
	
	fgSizer13->Add (bSizer236, 0, wxEXPAND|wxLEFT|wxRIGHT, 5);
	
	bSizer233->Add (fgSizer13, 0, wxEXPAND);
	
	wxFlexGridSizer* fgSizer14;
	fgSizer14 = new wxFlexGridSizer (3, 1, 0, 0);
	fgSizer14->SetFlexibleDirection (wxBOTH);
	fgSizer14->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	wxBoxSizer* bSizer282 = new wxBoxSizer (wxHORIZONTAL);
	#if defined multispec_wxlin
		m_histogramCtrl = new wxComboBox (this,
														IDC_HistogramStatsCombo,
														wxT ("Combo!"),
														wxDefaultPosition,
														wxSize (buttonWidth, 25),
														0,
														NULL,
														wxCB_READONLY);
	#endif	// multispec_wxlin
	#if defined multispec_wxmac
		m_histogramCtrl = new wxChoice (this,
													IDC_HistogramStatsCombo,
													wxDefaultPosition,
													wxSize (buttonWidth, 25));
	#endif	// multispec_wxmac
	m_histogramCtrl->Append (wxT("Histogram Classes"));
	m_histogramCtrl->Append (wxT("Histrgram Fields"));
	m_histogramCtrl->Append (wxT("Histogram..."));
   SetUpToolTip (m_histogramCtrl, IDS_ToolTip5);
   bSizer282->Add (
   		m_histogramCtrl,
			wxSizerFlags(0).ReserveSpaceEvenIfHidden().Expand().Border(wxBOTTOM, 5));
	
   fgSizer14->Add (bSizer282,
   						wxSizerFlags(0).Expand().Border(wxTOP|wxLEFT|wxRIGHT, 5));
	
	wxBoxSizer* bSizer283 = new wxBoxSizer (wxHORIZONTAL);
	#if defined multispec_wxlin
		m_listCtrl = new wxComboBox (this,
												IDC_ListStatsCombo,
												wxT("Combo!"),
												wxDefaultPosition,
												wxSize (buttonWidth, 25),
												0,
												NULL,
												wxCB_READONLY);
	#endif	// multispec_wxlin
	#if defined multispec_wxmac
		m_listCtrl = new wxChoice (this,
											IDC_ListStatsCombo,
											wxDefaultPosition,
											wxSize (buttonWidth, 25));
	#endif	// multispec_wxmac
	m_listCtrl->Append (wxT("List Classes Stats"));
	m_listCtrl->Append (wxT("List Classes & Fields Stats"));
	m_listCtrl->Append (wxT("List Fields Stats"));
	m_listCtrl->Append (wxT("List Stats..."));
   SetUpToolTip (m_listCtrl, IDS_ToolTip6);
   bSizer283->Add (m_listCtrl, wxSizerFlags(0).ReserveSpaceEvenIfHidden().Expand());
	
	fgSizer14->Add (bSizer283,
							wxSizerFlags(0).Expand().Border(wxTOP|wxLEFT|wxRIGHT, 5));
	
	wxBoxSizer* bSizer238;
	bSizer238 = new wxBoxSizer (wxHORIZONTAL);
	
	m_button61 = new wxButton (this,
										IDC_AddToList,
										wxT("Add To List..."),
										wxDefaultPosition,
										wxSize (buttonWidth, 25),
										0);
   SetUpToolTip (m_button61, IDS_ToolTip7);
	bSizer238->Add (m_button61,
							wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));
	
	m_button60 = new wxButton (this,
										IDC_Update,
										wxT("Update Project Stats"),
										wxDefaultPosition,
										wxSize (buttonWidth, 25),
										0);
   SetUpToolTip (m_button60, IDS_ToolTip8);
	bSizer238->Add (m_button60,
							wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));
	
	fgSizer14->Add (bSizer238, 0, wxEXPAND, 5);
	
	bSizer233->Add (fgSizer14,
							wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxLEFT|wxRIGHT, 3));
	
	wxFlexGridSizer* fgSizer15 = new wxFlexGridSizer (2, 2, 0, 0);
	fgSizer15->SetFlexibleDirection (wxBOTH);
	fgSizer15->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	m_button62 = new wxButton (this,
										IDC_Project,
										wxT(">Classes"),
										wxDefaultPosition,
										wxSize (90, -1),
										0)
										;
   SetUpToolTip (m_button62, IDS_ToolTip9);
   fgSizer15->Add (m_button62, wxSizerFlags(0).Border(wxLEFT|wxRIGHT, 5));
	
	m_button63 = new wxButton (this,
										IDC_Class,
										wxT(">Fields"),
										wxDefaultPosition,
										wxSize (105, -1),
										0);
   SetUpToolTip (m_button63, IDS_ToolTip10);
   fgSizer15->Add (m_button63, wxSizerFlags(0).Border(wxLEFT|wxRIGHT, 5));
	
	m_button64 = new wxButton (this,
										IDC_Select,
										wxT(">Select"),
										wxDefaultPosition,
										wxSize (90,-1),
										0);
   SetUpToolTip (m_button64, IDS_ToolTip11);
	fgSizer15->Add (m_button64, wxSizerFlags(0).Border(wxRIGHT|wxTOP|wxLEFT, 5));
	
	m_button65 = new wxButton (this,
										IDC_Field,
										wxT(">Coordinates"),
										wxDefaultPosition,
										wxSize (105, -1),
										0);
   SetUpToolTip (m_button65, IDS_ToolTip12);
	fgSizer15->Add (m_button65, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxRIGHT, 5));
	
	bSizer233->Add (fgSizer15, wxSizerFlags(0).Border(wxLEFT|wxRIGHT, 3));
	
	wxBoxSizer* bSizer242 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText229 = new wxStaticText (this,
													IDC_ListTitle,
													wxT("Classes"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText229->Wrap (-1);
	bSizer242->Add (m_staticText229,
							wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxTOP, 5));
	
	m_listBox = new wxListBox (this,
											IDC_ListBox,
											wxDefaultPosition,
											wxSize (buttonWidth, 156),
											0,
											NULL,
											0);
	bSizer242->Add (
					m_listBox,
					wxSizerFlags(0).Align(wxALIGN_CENTER_HORIZONTAL).Border(wxTOP, 5));
	
	bSizer233->Add (bSizer242, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxBOTTOM|wxLEFT|wxRIGHT, 8));
	
	m_staticText223->SetFont (font);
	m_staticText224->SetFont (font);
	m_staticText225->SetFont (font);
	m_staticText226->SetFont (font);
	m_staticText227->SetFont (font);
	m_staticText228->SetFont (font);
	m_staticText229->SetFont (font);
	m_staticText230->SetFont (font);
	m_staticText231->SetFont (font);
	m_staticText259->SetFont (font);
	m_staticText260->SetFont (font);
	m_staticText261->SetFont (font);
	m_staticText262->SetFont (font);
	m_classListCtrl->SetFont (font);
	m_statsCtrl->SetFont (font);
	m_histogramCtrl->SetFont (font);
	m_listCtrl->SetFont (font);
	m_checkBox53->SetFont (font);
	m_button56->SetFont (font);
	m_button60->SetFont (font);
	m_button61->SetFont (font);
	m_button62->SetFont (font);
	m_button63->SetFont (font);
	m_button64->SetFont (font);
	m_button65->SetFont (font);
	m_listBox->SetFont (font);
	
   bSizer234->SetMinSize (bSizer234->GetSize ());
   fgSizer8->SetMinSize (fgSizer8->GetSize ());
   fgSizer12->SetMinSize (fgSizer12->GetSize ());
   fgSizer13->SetMinSize (fgSizer13->GetSize ());
   fgSizer14->SetMinSize (fgSizer14->GetSize ());
   fgSizer15->SetMinSize (fgSizer15->GetSize ());
	
	SetSizerAndFit (bSizer233);
	Layout ();
	
}	// end "CreateControls2"



void CMStatisticsFrame::OnAddToList (
				wxCommandEvent& 					event)

{
	Point 								location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent (gProjectInfoPtr->addToControlH,
										FALSE,
										location,
										kAddToListControl);

}	// end "OnAddToList"



void CMStatisticsFrame::OnCharHook (
				wxKeyEvent& 						event)

{
	Boolean								eventHandledFlag = false;
	
	
   CMTool* pTool = CMTool::FindTool (CMTool::c_toolType);
	if (pTool != NULL)
		{
		int keyCode = event.GetKeyCode ();
	
		if (keyCode == WXK_RETURN || keyCode == WXK_NUMPAD_ENTER)
			{
			eventHandledFlag = pTool->OnCharHook ();
			
			}	// end "if (keyCode == WXK_RETURN || ..."
		
		}	// end "if (pTool != NULL)"
	
	if (!eventHandledFlag)
		event.Skip ();

}	// end "OnCharHook"



void CMStatisticsFrame::OnClass (
				wxCommandEvent& 					event)
				
{
	Point 								location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent (gProjectInfoPtr->toClassControlH,
										FALSE,
										location,
										kToClassControl);

   UpdateStatsTypeCombo (kFieldListMode);

   UpdateHistogramStatsCombo ();

   UpdateListStatsCombo ();
		
	ActivateStatisticsWindowItems (gProjectInfoPtr->statsWindowMode);
   
   SetUpToolTip (m_button56, IDS_ToolTip14);
   SetUpToolTip (m_button60, IDS_ToolTip15);

}	// end "OnClass"


void CMStatisticsFrame::OnDblclkListBox (
				wxCommandEvent& 					event)

{
	SInt16 								cellIndex = -1,
      									savedStatsMode;


   savedStatsMode = gProjectInfoPtr->statsWindowMode;
   cellIndex = gStatisticsListHandle->GetSelection ();

   if (cellIndex >= 0)
   	{
      if (savedStatsMode == kClassListMode)
      	{
         gProjectInfoPtr->currentClass = cellIndex;
         OnClass (event);

      	}	// end "if (savedStatsMode == kClassListMode)"

      else if (savedStatsMode == kFieldListMode)
         OnField (event);

      else if (savedStatsMode == kCoordinateListMode)
         EditCoordinatesDialog (gProjectInfoPtr->currentClass,
											gProjectInfoPtr->currentField,
											gProjectInfoPtr->currentCoordinate);

   	}	// end "if (cellIndex >= 0)"

   else	// cellIndex < 0
   	{
      if (savedStatsMode == kClassListMode)
      	{
         gProjectInfoPtr->currentClass = -1;
         MHiliteControl (gProjectWindow, gProjectInfoPtr->toClassControlH, 255);
         MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 255);

      	}	// end "if (savedStatsMode == kClassListMode)"

      if (savedStatsMode == kFieldListMode)
      	{
         gProjectInfoPtr->currentField = -1;
         MHiliteControl (gProjectWindow, gProjectInfoPtr->toFieldControlH, 255);
         MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 255);

      	}	// end "if (savedStatsMode == kFieldListMode)"

      if (savedStatsMode == kCoordinateListMode)
			{
         gProjectInfoPtr->currentCoordinate = -1;
         if (gProjectInfoPtr->fieldIdentPtr[
            					gProjectInfoPtr->currentField].pointType == kPolygonType)
            MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 255);

      	}	// end "if (savedStatsMode == kCoordinateListMode)"

   	}	// end "else cellIndex < 0"

}	// end "OnDblclkListBox"



void CMStatisticsFrame::OnEditClearSelectRectangle (
				wxCommandEvent& 					event)

{                  
	if (gProjectSelectionWindow != NULL)
		{                                                                                  
		ClearSelectionArea (gProjectSelectionWindow);
		
		if (gActiveImageViewCPtr == gProjectSelectionWindow)
			ShowGraphSelection ();
		
		}	// end "if (gProjectSelectionWindow != NULL)"
	
}	// end "OnEditClearSelectRectangle"



void CMStatisticsFrame::OnEditCut (
				wxCommandEvent& 					event)

{
	if (gActiveWindowType == kProjectWindowType)
		DoStatisticsWCut ();
	
}	// end "OnEditCut"



void CMStatisticsFrame::OnEditName (
				wxCommandEvent& 					event)

{
	Point 								location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent (gProjectInfoPtr->editNameControlH,
										FALSE,
										location,
										kEditNameControl);
   
}	// end "OnEditName"



void CMStatisticsFrame::OnEditPaste (
				wxCommandEvent& 					event)

{
	if (gActiveWindowType == kProjectWindowType)
		DoStatisticsWPaste ();
	
}	// end "OnEditPaste"



void CMStatisticsFrame::OnEditSelectionRectangle (
				wxCommandEvent& 					event)

{                                              
	EditSelectionDialog (kRectangleType, TRUE);
	
}	// end "OnEditSelectionRectangle"


void CMStatisticsFrame::OnEditSelectAll (
				wxCommandEvent& 					event)

{
	DoEditSelectAllImage (gProjectSelectionWindow);
	
}	// end "OnEditSelectAll"



void CMStatisticsFrame::OnEditUndo (
				wxCommandEvent& 					event)

{
	if (gActiveWindowType == kProjectWindowType)
		DoStatisticsWUndo ();
	
}	// end "OnEditUndo"



void CMStatisticsFrame::OnField (
				wxCommandEvent& 					event)

{
	Point 								location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent (gProjectInfoPtr->toFieldControlH,
										FALSE,
										location,
										kToFieldControl);

   UpdateHistogramStatsCombo ();

   UpdateListStatsCombo ();
		
	ActivateStatisticsWindowItems (gProjectInfoPtr->statsWindowMode);
   
   SetUpToolTip (m_button56, IDS_ToolTip16);
   SetUpToolTip (m_button60, IDS_ToolTip17);
   
}	// end "OnField"



void CMStatisticsFrame::OnHistogram (
				wxCommandEvent& 					event)

{

}	// end "OnHistogram"



void CMStatisticsFrame::OnHistogramStatsComboDropDown (
				wxCommandEvent& 					event)

{
   m_histogramCtrl->SetSelection (-1);
	
}	// end "OnHistogramStatsComboDropDown"



void CMStatisticsFrame::OnList ()

{
	Point 								location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent (gProjectInfoPtr->listControlH,
										FALSE,
										location,
										kStatPrintControl);
   
}	// end "OnList"



void CMStatisticsFrame::OnListStatsComboDropDown (
				wxCommandEvent& 					event)

{
    m_listCtrl->SetSelection (-1);
   
}	// end "OnListStatsComboDropDown"



void CMStatisticsFrame::OnPolygon (
				wxCommandEvent& 					event)

{
	Point 								location;
	
	
   ClearSelectionArea (gProjectSelectionWindow);
	
   wxCheckBox* polygonFlag = (wxCheckBox*)this->FindWindow (IDC_Polygon);
   m_polygonFlag = polygonFlag->GetValue ();

   location.h = 0;
   location.v = 0;
	
   StatisticsWControlEvent (gProjectInfoPtr->polygonTypeControlH,
										FALSE,
										location,
										kPolygonEnterControl);
   
}	// end "OnPolygon"



void CMStatisticsFrame::OnProject (
				wxCommandEvent& 					event)

{
	Point 								location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent (gProjectInfoPtr->toProjectControlH,
										FALSE,
										location,
										kToProjectControl);
		
	ActivateStatisticsWindowItems (gProjectInfoPtr->statsWindowMode);
   
   SetUpToolTip (m_button56, IDS_ToolTip3);
   SetUpToolTip (m_button60, IDS_ToolTip18);
   
}	// end "OnProject"



void CMStatisticsFrame::OnSelchangeListBox (
				wxCommandEvent& 					event)

{
	SInt16 								cellIndex = -1;


   cellIndex = gStatisticsListHandle->GetSelection ();
   if (cellIndex >= 0)
   	{
      if (gProjectInfoPtr->statsWindowMode == kClassListMode)
      	{
         gProjectInfoPtr->currentClass = cellIndex;
         MHiliteControl (gProjectWindow, gProjectInfoPtr->toClassControlH, 0);
         MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 0);

      	}	// end "if (gProjectInfoPtr->statsWindowMode == kClassListMode)"

      if (gProjectInfoPtr->statsWindowMode == kFieldListMode)
      	{
         gProjectInfoPtr->currentField =
            				GetCurrentField (gProjectInfoPtr->currentClass, cellIndex);

         if (gProjectInfoPtr->fieldIdentPtr[
									gProjectInfoPtr->currentField].pointType < kClusterType)
            MHiliteControl (gProjectWindow, gProjectInfoPtr->toFieldControlH, 0);
			
         else	// ...pointType >= kClusterType
            MHiliteControl (gProjectWindow, gProjectInfoPtr->toFieldControlH, 255);

         MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 0);

      	}	// end "if (gProjectInfoPtr->statsWindowMode == kFieldListMode)"

      if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode)
      	{
         gProjectInfoPtr->currentCoordinate = cellIndex;
         MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 0);

      	}	// end "if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode)"

   	}	// end "if (cellIndex != LB_ERR)"

   else	// cellIndex < 0
		{
      if (gProjectInfoPtr->statsWindowMode == kClassListMode)
      	{
         gProjectInfoPtr->currentClass = -1;
         MHiliteControl (gProjectWindow, gProjectInfoPtr->toClassControlH, 255);
         MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 255);

      	}	// end "if (gProjectInfoPtr->statsWindowMode == kClassListMode)"

      if (gProjectInfoPtr->statsWindowMode == kFieldListMode)
      	{
         gProjectInfoPtr->currentField = -1;
         MHiliteControl (gProjectWindow, gProjectInfoPtr->toFieldControlH, 255);
         MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 255);

      	}	// end "if (gProjectInfoPtr->statsWindowMode == kFieldListMode)"

      if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode)
      	{
         gProjectInfoPtr->currentCoordinate = -1;
         if (gProjectInfoPtr->fieldIdentPtr[
								gProjectInfoPtr->currentField].pointType == kPolygonType)
            MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 255);

      	}	// end "if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode)"

   	}	// end "else cellIndex < 0"

}	// end "OnSelchangeListBox"



void CMStatisticsFrame::OnSelect (
				wxCommandEvent& 					event)

{
	Point 								location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent (gProjectInfoPtr->newFieldControlH,
										FALSE,
										location,
										kNewFieldControl);
		
	ActivateStatisticsWindowItems (gProjectInfoPtr->statsWindowMode);
   SetUpToolTip (m_button56, IDS_ToolTip13);

}	// end "OnSelect"



void CMStatisticsFrame::OnSelendokClassList (
				wxCommandEvent& 					event)

{
   m_classList = m_classListCtrl->GetSelection ();
   gProjectInfoPtr->lastClassForAddFieldSelection = m_classList + 1;

}	// end "OnSelendokClassList"



void CMStatisticsFrame::OnHistogramStatsComboSelendok (
				wxCommandEvent& 					event)

{
   m_histogramStatsCode = m_histogramCtrl->GetSelection ();
   
   if (m_histogramStatsCode >= 0) 
		{
				// Make 1 base.

      m_histogramStatsCode++;

      if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode)
         m_histogramStatsCode += 1;

      gProcessorCode = kHistogramStatsProcessor;
      HistogramStatsControl (gProjectInfoPtr->statsWindowMode, m_histogramStatsCode);
      gProcessorCode = 0;

      if (m_histogramStatsCode > 0) 
			{
         m_histogramStatsCode = 0;
         m_histogramCtrl->SetSelection (m_histogramStatsCode);
			
			}	// end "if (m_histogramStatsCode > 0)"

		}	// end "if (m_histogramStatsCode >= 0)"

}	// end "OnHistogramStatsComboSelendok"



void CMStatisticsFrame::OnListStatsComboSelendok (
				wxCommandEvent& 					event)

{
	SInt16 								listStatsCode = 1;

	
  	m_listStatsCode = m_listCtrl->GetSelection ();
   
   if (m_listStatsCode >= 0) 
		{
      listStatsCode = m_listStatsCode + 1;

      if (gProjectInfoPtr->statsWindowMode == 4)
         listStatsCode += 2;

      gProcessorCode = kListStatsProcessor;
      ListStatsControl (gProjectInfoPtr->statsWindowMode, listStatsCode);
      gProcessorCode = 0;

      if (m_listStatsCode > 0) 
			{
         m_listStatsCode = 0;
         m_listCtrl->SetSelection (m_listStatsCode);
			
			}	// end "if (m_listStatsCode > 0)"

		}	// end "if (m_listStatsCode >= 0)"

}	// end "OnListStatsComboSelendok"



void CMStatisticsFrame::OnStatsTypeComboCloseUp (
				wxCommandEvent& 					event)

{
	int statsTypeSelection = m_statsCtrl->GetSelection ();
	
	if (statsTypeSelection == -1)
		m_statsCtrl->SetSelection (m_statsTypeCode);
	
	event.Skip ();
	
}	// end "OnStatsTypeComboCloseUp"



void CMStatisticsFrame::OnStatsTypeComboDropDown (
				wxCommandEvent& 					event)

{
			// This will allow the user to be able to select an item that is already
			// selected
			
	m_statsCtrl->SetSelection (-1);

}	// end "OnStatsTypeComboDropDown"



void CMStatisticsFrame::OnStatsTypeComboSelendok (
				wxCommandEvent& 					event)

{
	wxMouseState 						mousestate;
	
	SInt16 								covarianceStatsToUse,
      									savedStatsTypeCode;
	
	Boolean								shiftKeyFlag = FALSE;

      
   savedStatsTypeCode = m_statsTypeCode;
   m_statsTypeCode = m_statsCtrl->GetSelection ();
   shiftKeyFlag = (mousestate.RightIsDown () || wxGetKeyState (WXK_SHIFT));
   
   if (m_statsTypeCode == 1 && shiftKeyFlag)
		{
      if (!LOOCOptionsDialog (gProjectInfoPtr->statsWindowMode))
         m_statsTypeCode = savedStatsTypeCode;

      else	// LOOCOptionsDialog (...
         m_statsTypeCode = 1;

		}	// end "if (m_statsTypeCode == 1 && ..."

	SInt64 statstype64 = (SInt64)((int*)m_statsCtrl->GetClientData (m_statsTypeCode));
	covarianceStatsToUse = (SInt16)statstype64;
    
   if (covarianceStatsToUse > 0) 
		{
      if (gProjectInfoPtr->statsWindowMode == kClassListMode)
         SetProjectCovarianceStatsToUse (covarianceStatsToUse);

      else	// gProjectInfoPtr->statsWindowMode == kFieldListMode
         SetClassCovarianceStatsToUse (covarianceStatsToUse);

		}	// end "if (covarianceStatsToUse > 0)"

			// Make sure that the drown down selection is drawn properly.

   m_statsCtrl->SetSelection (m_statsTypeCode);
	
}	// end "OnStatsTypeComboSelendok"



void CMStatisticsFrame::OnUpdate (
				wxCommandEvent& 					event)

{
	Point 								location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent (gProjectInfoPtr->updateControlH,
										FALSE,
										location,
										kUpdateStatsControl);
	
}	// end "OnUpdate"


void CMStatisticsFrame::OnUpdateEditClearSelectRectangle (
				wxUpdateUIEvent&  				pCmdUI)

{                                                                                                                                                                                                                      
	SInt16 selectionTypeCode = GetSelectionTypeCode (gProjectSelectionWindow);
	
			// Update Menu item text.
			
	UpdateEditClearSelection (&pCmdUI, selectionTypeCode); 
		
	pCmdUI.Enable (selectionTypeCode != 0);
	
}	// end "OnUpdateEditClearSelectRectangle"



void CMStatisticsFrame::OnUpdateEditSelectAll (
				wxUpdateUIEvent&  				pCmdUI)

{  
	Boolean enableFlag = FALSE;
	                                                      
	if (gProjectInfoPtr != NULL &&
						gProjectInfoPtr->selectionType == kRectangleType) 
		enableFlag = TRUE; 
						                                                                                            
	pCmdUI.Enable (enableFlag);
	
}	// end "OnUpdateEditSelectAll"


void CMStatisticsFrame::OnUpdateEditSelectionRectangle (
				wxUpdateUIEvent&  				pCmdUI)

{  
	Boolean	enableFlag = FALSE;
	if (gProjectInfoPtr != NULL &&
					gProjectInfoPtr->selectionType == kRectangleType)
		enableFlag = TRUE;
   
	pCmdUI.Enable (enableFlag);
	
}	// end "OnUpdateEditSelectionRectangle"


void CMStatisticsFrame::UpdateHistogramStatsCombo ()

{
   if (gProjectInfoPtr->statsWindowMode > 1)
		{     
				// Clear the current items.
            
      m_histogramCtrl->Clear ();

				// Load the new items depending on the current statistics window mode.

      switch (gProjectInfoPtr->statsWindowMode) 
			{
         case kClassListMode:
            m_histogramCtrl->Append ("Histogram Classes");
            m_histogramCtrl->Append ("Histogram Fields");
            m_histogramCtrl->Append ("Histogram...");
            break;

         case kFieldListMode:
            m_histogramCtrl->Append ("Histogram Class");
            m_histogramCtrl->Append ("Histogram Fields");
            m_histogramCtrl->Append ("Histogram...");
            break;

         case kCoordinateListMode:
            m_histogramCtrl->Append ("Histogram Field");
            m_histogramCtrl->Append ("Histogram...");
            break;

			}	// end "switch (gProjectInfoPtr->statsWindowMode)"

      m_histogramStatsCode = 0;
      m_histogramCtrl->SetSelection (m_histogramStatsCode);

		}	// end "if (gProjectInfoPtr->statsWindowMode > 1)"

}	// end "UpdateHistogramStatsCombo"



void CMStatisticsFrame::UpdateListStatsCombo ()

{
   if (gProjectInfoPtr->statsWindowMode > 1)
		{
				// Clear the current items.

      m_listCtrl->Clear ();

				// Load the new items depending on the current statistics window
				// mode.
      
      switch (gProjectInfoPtr->statsWindowMode) 
			{
         case kClassListMode:
            m_listCtrl->Append ("List Classes Stats");
            m_listCtrl->Append ("List Classes & Fields Stats");
            m_listCtrl->Append ("List Fields Stats");
            m_listCtrl->Append ("List Stats...");
            break;

         case kFieldListMode:
            m_listCtrl->Append ("List Class Stats");
            m_listCtrl->Append ("List Class & Fields Stats");
            m_listCtrl->Append ("List Fields Stats");
            m_listCtrl->Append ("List Stats...");
            break;

         case kCoordinateListMode:
            m_listCtrl->Append ("List Field Stats");
            m_listCtrl->Append ("List Stats...");
            break;

      	}	// end "switch (gProjectInfoPtr->statsWindowMode)"

      m_listStatsCode = 0;
      m_listCtrl->SetSelection (m_listStatsCode);
		
		}	// end "if (gProjectInfoPtr->statsWindowMode > 1)"

}	// end "UpdateListStatsCombo"



void CMStatisticsFrame::UpdateStatsTypeCombo (
				SInt16 								statsWindowMode)

 {
	SInt16								numberItems,
											classStorage;


   		// Draw the prompt string and current covariance stats to be used.
 
   if (m_statsCtrl->GetCount () > 3)
      m_statsCtrl->Delete (3);
 
	if (m_statsCtrl->GetCount () > 2)
		m_statsCtrl->Delete (2);
	
   if (statsWindowMode == kClassListMode)
   	{
      m_statsTypeCode = GetProjectStatisticsTypeMenuItem () - 1;

      if (gProjectInfoPtr->enhancedStatsExistFlag)
			{
         m_statsCtrl->Append ("Enhanced");
         m_statsCtrl->SetClientData (2, (void*)kEnhancedStats);

			}	// end "if (gProjectInfoPtr->enhancedStatsExistFlag)"

      if (m_statsTypeCode == 3)
			{
         m_statsCtrl->Append ("Mixed");
         numberItems = m_statsCtrl->GetCount ();
         m_statsCtrl->SetClientData (numberItems - 1, (void*)kMixedStats);
         m_statsTypeCode = numberItems - 1;

			}	// end "if (gProjectInfoPtr->enhancedStatsExistFlag)"

		}	// end "if (statsWindowMode == kClassListMode)"

   else	// statsWindowMode == kFieldListMode
		{
      m_statsTypeCode = GetClassStatisticsTypeMenuItem () - 1;

      classStorage = gProjectInfoPtr->storageClass[gProjectInfoPtr->currentClass];
      if (gProjectInfoPtr->classNamesPtr[classStorage].modifiedStatsFlag) 
			{
         m_statsCtrl->Append ("Enhanced");
         m_statsCtrl->SetClientData (2, (void*)kEnhancedStats);

			}	// end "if (...modifiedStatsFlag)"

		}	// end "else statsWindowMode == kFieldListMode"

   m_statsCtrl->SetSelection (m_statsTypeCode);
	
}	// end "UpdateStatsTypeCombo"
