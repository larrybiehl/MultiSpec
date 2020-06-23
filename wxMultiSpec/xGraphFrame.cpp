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
//	Implementation file:	xGraphFrame.cpp
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			02/20/2017 by Wei-Kang Hsu
//								06/10/2020 by Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	The routines in this file control the frame class for 
//								CMGraphCanvas class (graph windows).
//	
/*	Template for debugging.
	int numberChars = sprintf ((char*)&gTextString3,
											" xGraphFrame: (): %s",
											gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);	
*/				
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xGraphView.h"
#include "xGraphFrame.h"

#include "xGraphWindow_images.cpp"
#include "wx/treelist.h"
#include "wx/display.h"

#define	kXAxis			1
#define	kYAxis			2
#define	kBothXYAxes		3

IMPLEMENT_CLASS (CMGraphFrame, wxDocChildFrame)
IMPLEMENT_CLASS (CMGraphCanvas, wxPanel)



BEGIN_EVENT_TABLE (CMGraphCanvas, wxPanel)
	EVT_PAINT (CMGraphCanvas::paintEvent) 
END_EVENT_TABLE ()



CMGraphCanvas::CMGraphCanvas (
				wxWindow* 							parent)
		: wxPanel (parent)

{

}	// end "CMGraphCanvas"

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh ()/Update ().
 */
void CMGraphCanvas::paintEvent (
				wxPaintEvent&						evt)

{   
   wxPaintDC dc (this);
   render (dc); 
   
}	// end "paintEvent"



void CMGraphCanvas::paintNow ()

{
	wxClientDC dc (this);
	render (dc);
	
}	// end "paintNow"



void CMGraphCanvas::render (
				wxDC&									dc)

{
	m_graphViewCPtr->OnDraw (&dc);
	
}	// end "render"



// CMGraphFrame 

BEGIN_EVENT_TABLE (CMGraphFrame, wxDocChildFrame)
   EVT_BUTTON (IDC_NEXT_CHANNEL, CMGraphFrame::OnNextListData)

	EVT_CHECKBOX (ID_GRAPHWINDOW, CMGraphFrame::OnGraphWindow)
	EVT_CHECKBOX (ID_FEATURELIST, CMGraphFrame::OnFeature)
	EVT_CHECKBOX (ID_DATALIST, CMGraphFrame::OnData)

	EVT_CHOICE (ID_SelectionWinXlabelCombo, CMGraphFrame::OnChangeXAxis)

	EVT_MAXIMIZE (CMGraphFrame::OnMaximizeWindow)

	EVT_MENU_RANGE (ID_SELECTBINWIDTHMENUITEMSTART,
							ID_SELECTBINWIDTHMENUITEMSTART+10,
							CMGraphFrame::OnBinWidth)
	EVT_MENU_RANGE (ID_GRAPHOVERLAYMENUITEMSTART,
							ID_GRAPHOVERLAYMENUITEMSTART+10,
							CMGraphFrame::OnOverlay)
	EVT_MENU_RANGE (ID_SELECTVECTORMENUITEMSTART,
							ID_SELECTVECTORMENUITEMSTART+255,
							CMGraphFrame::OnSelectVector)

	EVT_PAINT (CMGraphFrame::OnPaint)

	EVT_SIZE (CMGraphFrame::OnSize)

	EVT_UPDATE_UI (ID_FILE_SAVE_AS, CMGraphFrame::OnUpdateFileSaveAs)
	EVT_UPDATE_UI (ID_MAGNIFICATION, CMGraphFrame::OnUpdateMagnification)
	EVT_UPDATE_UI (ID_OVERLAY, CMGraphFrame::OnUpdateOverlay)
	EVT_UPDATE_UI (ID_WINDOW_NEW_SELECTION_GRAPH,
						CMGraphFrame::OnUpdateWindowNewSelectionGraph)
	EVT_UPDATE_UI (ID_ZOOM_IN, CMGraphFrame::OnUpdateZoomIn)
	EVT_UPDATE_UI (ID_ZOOM_OUT, CMGraphFrame::OnUpdateZoomOut)
	EVT_UPDATE_UI (wxID_PRINT, CMGraphFrame::OnUpdateFilePrint)
	EVT_UPDATE_UI (wxID_PREVIEW, CMGraphFrame::OnUpdateFilePrintPreview)
	EVT_UPDATE_UI (wxID_PAGE_SETUP, CMGraphFrame::OnUpdateFilePrintSetup)
END_EVENT_TABLE ()



CMGraphFrame::CMGraphFrame ()

{
	m_graphViewCPtr = NULL;
   m_selectionVector = NULL;
	m_listCtrl1 = NULL;
	m_comboXlabel = NULL;
	m_dataListShowFlag = false;
	m_featureListShowFlag = false;
	m_featureListShowFlag = false;
	
}	// end "CMGraphFrame"



CMGraphFrame::CMGraphFrame (
				wxDocument*							doc, 
				wxView*								view, 
				wxDocParentFrame*					parent, 
				wxWindowID							id, 
				const wxString&					title, 
				const wxPoint&						pos, 
				const wxSize&						size, 
				long									style)
		: wxDocChildFrame (doc, view, parent, id, title, pos, size, style)
		 
{
	GraphPtr								graphRecordPtr;
	
	
   EnableCloseButton (true);
   m_height = size.y;
   m_width = size.x;
	
	m_minFrameYSize = 340;
   
   binWidthCheckID = ID_SELECTBINWIDTHMENUITEMSTART;
   m_overlayCheckID = ID_GRAPHOVERLAYMENUITEMSTART;
   selectVectorCheckID = ID_SELECTVECTORMENUITEMSTART;
   activateFlag = 0;
   actEntryPrev = 0;
   m_statusBar1 = this->CreateStatusBar (1, wxST_SIZEGRIP, wxID_ANY);
	
   m_graphViewCPtr = (CMGraphView*)view;
	m_comboXlabel = NULL;
   m_panel2 = NULL;
   m_listCtrl1 = NULL;
   m_listCtrl2 = NULL;
   m_selectionVector = NULL;
	m_toolBar1 = NULL;
	
	m_dataListShowFlag = false;
	m_featureListShowFlag = false;
   m_frameMaximized = false;
   m_initGraphUpdate = false;
	
	
	graphRecordPtr = (GraphPtr)GetHandlePointer (
													m_graphViewCPtr->m_graphRecordHandle);
   
   if (gProcessorCode == kHistogramStatsProcessor)
   	{
      CreateHistogramControls ();
		SetMinSize (wxSize (340, 300));
		
      }	// end "if (gProcessorCode == kHistogramStatsProcessor)"
		
   else if (gProcessorCode == kListDataProcessor)
		{
      CreateControlsListData ();
		
		if (gImageFileInfoPtr == NULL || gImageFileInfoPtr->channelValuesHandle == NULL)
         m_comboXlabel->Hide ();
         
      else	// gImageFileInfoPtr != NULL && gImageFileInfoPtr->channelValuesHandle != NULL
         {
			SetXAxisDescriptionInfo (graphRecordPtr,
												gImageWindowInfoPtr,
												gImageFileInfoPtr);
			SetUpXAxisPopUpMenu (graphRecordPtr, (MenuHandle)m_comboXlabel);
         m_comboXlabel->Show ();
          
         }	// end "else fileInfoPtr != NULL && ..."
		
		m_panel2->Layout ();
		
		}	// end "else if (gProcessorCode == kListDataProcessor)"
		
   else if (gProcessorCode == kBiPlotDataProcessor)
		CreateControlsBiPlot ();   
		
   else	// gProcessorCode != kHistogramStatsProcessor, kListDataProcessor,
			// kBiPlotDataProcessor
   	{
      CreateControls ();
		#ifdef NetBeansProject
      	SetMinSize (wxSize (300, 300));
		#else
         SetMinSize (wxSize (400, m_minFrameYSize));
		#endif

   	}	// end "else gProcessorCode != ...
	
			// Store the x-axis combobox pointer in the graph record structure
	
	graphRecordPtr->xAxisPopupControlHandle = m_comboXlabel;

   Layout ();
	SetLabel (wxT("Selection Window"));
   
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
      
}	// end "CMGraphFrame"



CMGraphFrame::~CMGraphFrame ()

{

}	// end "~CMGraphFrame"



void CMGraphFrame::CreateBinWidthMenu ()

{
	m_optionBinWidthFlag = false;
	m_optionBinWidthFlag = wxGetKeyState (WXK_SHIFT);

	m_binWidthMenu = new wxMenu ();

	m_binWidthMenu->Append (ID_SELECTBINWIDTHMENUITEMSTART, 
									wxString (wxT("Auto")), 
									wxT(""), 
									wxITEM_CHECK);

	m_binWidthMenu->Append (ID_SELECTBINWIDTHMENUITEMSTART+1, 
									wxString (wxT("x1")), 
									wxT(""), 
									wxITEM_CHECK);

	m_binWidthMenu->Append (ID_SELECTBINWIDTHMENUITEMSTART+2, 
									wxString (wxT("x2")), 
									wxT(""), 
									wxITEM_CHECK);

	m_binWidthMenu->Append (ID_SELECTBINWIDTHMENUITEMSTART+3, 
									wxString (wxT("x3")), 
									wxT(""), 
									wxITEM_CHECK);

	m_binWidthMenu->Append (ID_SELECTBINWIDTHMENUITEMSTART+4, 
									wxString (wxT("x4")), 
									wxT(""), 
									wxITEM_CHECK);

	m_binWidthMenu->Append (ID_SELECTBINWIDTHMENUITEMSTART+5, 
									wxString (wxT("x5")), 
									wxT(""), 
									wxITEM_CHECK);

	m_binWidthMenu->Append (ID_SELECTBINWIDTHMENUITEMSTART+6, 
									wxString (wxT("x10")),
									wxT(""), 
									wxITEM_CHECK);

	m_binWidthMenu->Append (ID_SELECTBINWIDTHMENUITEMSTART+7, 
									wxString (wxT("x20")),
									wxT(""), 
									wxITEM_CHECK);

	m_binWidthMenu->Append (ID_SELECTBINWIDTHMENUITEMSTART+8, 
									wxString (wxT("x30")), 
									wxT(""), 
									wxITEM_CHECK);

	m_binWidthMenu->Append (ID_SELECTBINWIDTHMENUITEMSTART+9, 
									wxString (wxT("x50")), 
									wxT(""), 
									wxITEM_CHECK);

	m_binWidthMenu->Check (ID_SELECTBINWIDTHMENUITEMSTART, 1);
	
}	// end "CreateBinWidthMenu"



void CMGraphFrame::CreateControls ()

{
	wxBoxSizer* bSizer321;
	bSizer321 = new wxBoxSizer (wxVERTICAL);
	
   m_splitter1 = new wxSplitterWindow (this, 
													IDC_SplitterWindow1, 
													wxDefaultPosition, 
													wxDefaultSize, 
													wxSP_3D|wxSP_LIVE_UPDATE);
   
	m_splitter2 = new wxSplitterWindow (m_splitter1, 
													wxID_ANY, 
													wxDefaultPosition, 
													wxDefaultSize, 
													wxSP_3D);
   m_splitter2->SetSashGravity (0);
   m_splitter2->SetSashInvisible (true);

	m_panel1 = new wxPanel (m_splitter2,
									wxID_ANY, 
									wxDefaultPosition, 
									wxDefaultSize, 
									wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer324;
	bSizer324 = new wxBoxSizer (wxVERTICAL);
	
   wxBoxSizer* bSizer3241 = new wxBoxSizer (wxHORIZONTAL);
   
	m_checkBoxGraph = new wxCheckBox (m_panel1, 
													ID_GRAPHWINDOW, 
													wxT("Show Graph"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
   m_checkBoxGraph->SetValue (true);
   bSizer3241->Add (m_checkBoxGraph,  0, wxALL, 5);
   
   m_checkBoxData = new wxCheckBox (m_panel1, 
												ID_DATALIST, 
												wxT("Data List"), 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
   m_checkBoxData->SetValue (false);
	bSizer3241->Add (m_checkBoxData, 0, wxALL, 5);
   
   m_checkBoxFeature = new wxCheckBox (m_panel1, 
													ID_FEATURELIST, 
													wxT("Feature List"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
   m_checkBoxFeature->SetValue (false);
	bSizer3241->Add (m_checkBoxFeature, 0, wxALL, 5);
   
	bSizer324->Add (bSizer3241, 0, wxALL, 5);
	
	
	m_panel1->SetSizer (bSizer324);
	m_panel1->Layout ();
	bSizer324->Fit (m_panel1);

   wxBoxSizer* bSizer325 = new wxBoxSizer (wxVERTICAL);
   wxBoxSizer* bSizer3251 = new wxBoxSizer (wxHORIZONTAL);   
   
   m_panel2 = new CMGraphCanvas (m_splitter2);
	
	m_comboXlabel = CreateXAxisComboControl (m_panel2);

   bSizer3251->Add (m_comboXlabel,
							0,
							wxALIGN_BOTTOM|wxALL,
							5);
	
   bSizer325->Add (bSizer3251, 1, wxALIGN_CENTER, 5);
   m_panel2->SetSizer (bSizer325);
   m_panel2->Layout ();

	m_splitter2->SplitHorizontally (m_panel1, m_panel2, 45);
	
	m_splitter3 = new wxSplitterWindow (m_splitter1, 
													wxID_ANY, 
													wxDefaultPosition, 
													wxDefaultSize, 
													wxSP_3D|wxSP_LIVE_UPDATE);
   m_splitter3->SetSashGravity (0.5);

	m_panel3 = new wxPanel (m_splitter3,
									wxID_ANY, 
									wxDefaultPosition, 
									wxDefaultSize, 
									wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer327;
	bSizer327 = new wxBoxSizer (wxVERTICAL);
	
	m_listCtrl2 = new wxListView (m_panel3, 
											IDC_DATA_LIST, 
											wxDefaultPosition, 
											wxDefaultSize, 
											wxLC_REPORT|wxTL_CHECKBOX);
	bSizer327->Add (m_listCtrl2, 1, wxALL|wxEXPAND, 5);
	
   wxFont  font (gFontSize,
   					wxFONTFAMILY_MODERN,
   					wxFONTSTYLE_NORMAL,
   					wxFONTWEIGHT_NORMAL);
   m_listCtrl2->SetFont (font);
   
   m_listCtrl2->InsertColumn (0, 
										wxT("  Channel  "), 
										wxLIST_FORMAT_RIGHT, 
										wxLIST_AUTOSIZE);

			// Add second column
	
   m_listCtrl2->InsertColumn (1, 
										wxT("Data value"), 
										wxLIST_FORMAT_RIGHT,
										wxLIST_AUTOSIZE);
   m_listCtrl2->Hide ();
   
	m_panel3->SetSizer (bSizer327);
	m_panel3->Layout ();
	bSizer327->Fit (m_panel3);
	
	m_panel4 = new wxPanel (m_splitter3, 
									wxID_ANY, 
									wxDefaultPosition, 
									wxDefaultSize, 
									wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer326;
	bSizer326 = new wxBoxSizer (wxVERTICAL);
	
   m_listCtrl1 = new wxTreeListCtrl (m_panel4, 
													IDC_FEATURE_LIST, 
													wxDefaultPosition, 
													wxDefaultSize); 
   m_listCtrl1->SetFont (font);
	bSizer326->Add (m_listCtrl1, 1, wxALL|wxEXPAND, 5);
	
   m_listCtrl1->AppendColumn (wxT("Feature Name"), 
										wxCOL_WIDTH_AUTOSIZE, 	// 150
										wxALIGN_LEFT, 
										wxVSCROLL|wxCOL_RESIZABLE);
												
   m_listCtrl1->AppendColumn (wxT("Feature Value"), 
										150,
										wxALIGN_LEFT,
										wxVSCROLL|wxCOL_RESIZABLE);
   
			// Can later try to add  wxCOL_SORTABLE
   
	m_panel4->SetSizer (bSizer326);
	m_panel4->Layout ();
	bSizer326->Fit (m_panel4);
   
	m_splitter3->SplitHorizontally (m_panel3, m_panel4, 50);
   
			// Set this to prevent unsplitting
			
   m_splitter1->SetMinimumPaneSize (45);  	// 45
   m_splitter2->SetMinimumPaneSize (40);  	// 40
   m_splitter3->SetMinimumPaneSize (40);  

	
   m_splitter1->SplitHorizontally (m_splitter2, m_splitter3, 0);
   m_splitter1->SetSashGravity (0.5);
   
   bSizer321->Add (m_splitter1, 1, wxEXPAND, 5);
	
	SetSizer (bSizer321);
	Layout ();
	Centre (wxBOTH);
	
}	// end "CreateControls"



void CMGraphFrame::CreateControlsBiPlot ()

{
   wxBoxSizer* bSizer321;
	
	bSizer321 = new wxBoxSizer (wxVERTICAL);
   m_panel2 = new CMGraphCanvas (this);
 
   bSizer321->Add (m_panel2, 1, wxEXPAND, 0);
	
	SetSizer (bSizer321);
	Layout ();
	Centre (wxBOTH);
	   
}	// end "CreateControlsBiPlot"



void CMGraphFrame::CreateControlsListData ()

{
   wxBoxSizer							*bSizer321,
                        			*bSizer322,
											*bSizer3221;
	
	
	bSizer321 = new wxBoxSizer (wxVERTICAL);
	
   m_panel2 = new CMGraphCanvas (this);  

   bSizer322 = new wxBoxSizer (wxHORIZONTAL);
   
   wxBitmap rightarrow = wxBITMAP_PNG_FROM_DATA (rightarrow);
   m_buttonNext = new wxBitmapButton (m_panel2,
   												IDC_NEXT_CHANNEL,
                                       rightarrow, 
                                       wxDefaultPosition, 
                                       wxDefaultSize, 
                                       wxBU_AUTODRAW);
	bSizer322->Add (m_buttonNext, 0, wxALIGN_BOTTOM|wxLEFT, 25);
   
   bSizer3221 = new wxBoxSizer (wxVERTICAL);   
	m_comboXlabel = CreateXAxisComboControl (m_panel2);
	
   bSizer3221->Add (m_comboXlabel, 
							0,
							//wxALIGN_BOTTOM|wxALIGN_CENTER|wxALL,
							wxALIGN_CENTER_HORIZONTAL|wxALL,
							5);
	
	bSizer322->Add (bSizer3221, 1, wxALIGN_BOTTOM, 5);
   bSizer321->Add (bSizer322, 1, wxEXPAND, 5);

   m_panel2->SetSizer (bSizer321);
   m_panel2->Layout ();
	
	Centre (wxBOTH);
	
}	// end "CreateControlsListData"



void CMGraphFrame::CreateHistogramControls ()

{
	#if defined multispec_wxlin
		wxBitmap density_histogram = wxBITMAP_PNG_FROM_DATA (density_histogram);
		wxBitmap binwidth = wxBITMAP_PNG_FROM_DATA (binwidth);
		wxBitmap classes = wxBITMAP_PNG_FROM_DATA (classes);
		wxBitmap arrow_next = wxBITMAP_PNG_FROM_DATA (arrow_next);
		wxBitmap arrow_previous = wxBITMAP_PNG_FROM_DATA (arrow_previous);
	#endif

	#if defined multispec_wxmac
		wxBitmap density_histogram = wxBITMAP_PNG (density_histogram);
		wxBitmap binwidth = wxBITMAP_PNG (binwidth);
		wxBitmap classes = wxBITMAP_PNG (classes);
		wxBitmap arrow_next = wxBITMAP_PNG (arrow_next);
		wxBitmap arrow_previous = wxBITMAP_PNG (arrow_previous);
	#endif
	
   m_panel2 = new CMGraphCanvas (this);
	
	wxBoxSizer* bVSizer100 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bHSizer110 = new wxBoxSizer (wxHORIZONTAL);
	
	wxFont font (gFontSize+1,
						wxFONTFAMILY_MODERN,
						wxFONTSTYLE_NORMAL,
						wxFONTWEIGHT_NORMAL);
	wxStaticText *text = new wxStaticText (m_panel2,
														IDC_ChangeChannelPrompt,
														wxT("Change channel:"),
														wxDefaultPosition,
														wxDefaultSize);
	bHSizer110->Add (text, wxSizerFlags(0).Align(wxALIGN_BOTTOM).Border(wxLEFT, 3));
	
	text->SetFont (font);
	
	m_buttonPrevious = new wxBitmapButton (m_panel2,
														IDC_PreviousChannel,
														arrow_previous,
														wxDefaultPosition,
														wxDefaultSize,
														//wxSize (16, 16),
														wxBORDER_NONE+wxBU_EXACTFIT);
	m_buttonPrevious->SetToolTip (wxT("Go to previous channel"));
	m_buttonPrevious->Bind (wxEVT_LEFT_DOWN, &CMGraphFrame::DoPreviousChannel, this);
	m_buttonPrevious->Bind (wxEVT_LEFT_DCLICK, &CMGraphFrame::DoPreviousChannel, this);
	m_buttonPrevious->Enable (false);
	bHSizer110->Add (m_buttonPrevious,
							wxSizerFlags(0).Align(wxALIGN_BOTTOM).Border(wxLEFT|wxRIGHT, 3));
	
	m_buttonNext = new wxBitmapButton (m_panel2,
													IDC_NextChannel,
													arrow_next,
													wxDefaultPosition,
													wxDefaultSize,
													wxBORDER_NONE+wxBU_EXACTFIT);
	m_buttonNext->SetToolTip (wxT("Go to next channel"));
	m_buttonNext->Bind (wxEVT_LEFT_DOWN, &CMGraphFrame::DoNextChannel, this);
	m_buttonNext->Bind (wxEVT_LEFT_DCLICK, &CMGraphFrame::DoNextChannel, this);
	bHSizer110->Add (m_buttonNext,
							wxSizerFlags(0).Align(wxALIGN_BOTTOM).Border(wxRIGHT, 12));
	
	m_buttonVectors = new wxBitmapButton (m_panel2,
													IDC_SelectVectors,
													classes,
													wxDefaultPosition,
													wxDefaultSize,
													wxBORDER_NONE+wxBU_EXACTFIT);
	m_buttonVectors->SetToolTip (wxT("Select classes to be displayed"));
	m_buttonVectors->Bind (wxEVT_LEFT_DOWN, &CMGraphFrame::DoShowVectors, this);
	bHSizer110->Add (m_buttonVectors,
							wxSizerFlags(0).Align(wxALIGN_BOTTOM).Border(wxRIGHT, 3));
	
	m_buttonOverlay = new wxBitmapButton (m_panel2,
														IDC_GraphOverlay,
														density_histogram,
														wxDefaultPosition,
														wxDefaultSize,
														wxBORDER_NONE+wxBU_EXACTFIT);
	m_buttonOverlay->SetToolTip (wxT("Display histogram and/or density function"));
	m_buttonOverlay->Bind (wxEVT_LEFT_DOWN, &CMGraphFrame::DoShowOverlay, this);
	bHSizer110->Add (m_buttonOverlay,
							wxSizerFlags(0).Align(wxALIGN_BOTTOM).Border(wxRIGHT, 3));
	
	m_buttonBinWidth = new wxBitmapButton (m_panel2,
														IDC_BinWidth,
														binwidth,
														wxDefaultPosition,
														wxDefaultSize,
														wxBORDER_NONE+wxBU_EXACTFIT);
	m_buttonBinWidth->SetToolTip (wxT("Set bin width"));
	m_buttonBinWidth->Bind (wxEVT_LEFT_DOWN, &CMGraphFrame::DoShowBinWidth, this);
	bHSizer110->Add (m_buttonBinWidth,
							wxSizerFlags(0).Align(wxALIGN_BOTTOM).Border(wxRIGHT, 3));
	
	wxStaticText* binWidthText = new wxStaticText (m_panel2,
																	IDC_BinWidthText,
																	wxT("Bin width: 1"),
																	wxDefaultPosition,
																	wxDefaultSize);
	bHSizer110->Add (binWidthText,
							wxSizerFlags(0).Align(wxALIGN_BOTTOM));
	binWidthText->SetFont (font);
	
	bVSizer100->Add (bHSizer110, wxSizerFlags(1).Expand().Border(wxLEFT, 3));
	
   m_panel2->SetSizer (bVSizer100);
   m_panel2->Layout ();
	
	CreateBinWidthMenu ();
	CreateHistogramDensityMenu ();
	
}	// end "CreateHistogramControls"



void CMGraphFrame::CreateHistogramDensityMenu ()

{
	m_graphOverlayMenu = new wxMenu ();

	m_graphOverlayMenu->Append (ID_GRAPHOVERLAYMENUITEMSTART, 
											wxString (wxT("&Draw histogram")), 
											wxT(""), 
											wxITEM_CHECK);
	
	if (gStatHistogramSpecsPtr->overlayDensityFunctionFlag)
		{
		m_graphOverlayMenu->Append (ID_GRAPHOVERLAYMENUITEMSTART+1,
												wxString (wxT("&Draw density function")), 
												wxT(""), 
												wxITEM_CHECK);

		m_graphOverlayMenu->Append (ID_GRAPHOVERLAYMENUITEMSTART+2, 
												wxString (wxT("&Draw histogram && density function")), 
												wxT(""), 
												wxITEM_CHECK);

   	m_overlayCheckID = ID_GRAPHOVERLAYMENUITEMSTART + 2;
		
		}
	
	else	// !gStatHistogramSpecsPtr->overlayDensityFunctionFlag
   	m_overlayCheckID = ID_GRAPHOVERLAYMENUITEMSTART;
	
	m_graphOverlayMenu->Check (m_overlayCheckID, 1);
      
}	// end "CreateHistogramDensityMenu"



void CMGraphFrame::CreateSelectVector (
				int									vectorSize)

{
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;

	int									line;
										
										
	m_selectVectorMenu = new wxMenu ();

	m_selectionVector = vectorSize;

	if (vectorSize == gProjectInfoPtr->numberStatisticsClasses)
		{
		classNamesPtr = gProjectInfoPtr->classNamesPtr;
		for (line=0; line<vectorSize; line++)
			{
			m_selectVectorMenu->Append (ID_SELECTVECTORMENUITEMSTART+line, 
													wxString ((char*)&classNamesPtr[line].name[1]),
													wxT(""), 
													wxITEM_CHECK);
			m_selectVectorMenu->Check (ID_SELECTVECTORMENUITEMSTART+line, 1); 
			
			}	// end "for (line=0; line<vectorSize; line++)"
			
		}	// end "if (vectorSize == gProjectInfoPtr->numberStatisticsClasses)"
		
	else	// vectorSize != gProjectInfoPtr->numberStatisticsClasses
		{
		fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
		for (line=0; line<vectorSize; line++)
			{
			m_selectVectorMenu->Append (ID_SELECTVECTORMENUITEMSTART+line, 
								wxString ((char*)&fieldIdentPtr[line].name[1]),
								wxT(""), wxITEM_CHECK);
			m_selectVectorMenu->Check (ID_SELECTVECTORMENUITEMSTART+line, 1);
			 
			}	// end "for (line=0; line<vectorSize; line++)"
			
		}	// end "else vectorSize != gProjectInfoPtr->numberStatisticsClasses"

	m_selectVectorMenu->Append (ID_SELECTVECTORMENUITEMSTART+line+1, 
											wxString (wxT("All")), 
											wxT(""), 
											wxITEM_CHECK);
	m_selectVectorMenu->Check (ID_SELECTVECTORMENUITEMSTART+line+1, 0);

	m_selectVectorMenu->Append (ID_SELECTVECTORMENUITEMSTART+line+2, 
											wxString (wxT("None")), 
											wxT(""), 
											wxITEM_CHECK);
	m_selectVectorMenu->Check (ID_SELECTVECTORMENUITEMSTART+line+2, 0);

}	// end "CreateSelectVector"



wxChoice* CMGraphFrame::CreateXAxisComboControl (
				wxWindow*							parent)

{
	wxChoice*							comboXLabel;
	
	
   comboXLabel = new wxChoice (parent,
											ID_SelectionWinXlabelCombo,
											wxDefaultPosition,
											wxSize (280,-1));
   comboXLabel->Append (wxT("Channel number"));
	comboXLabel->SetClientData (0, (void*)kChannels);
	
	if (gActiveImageViewCPtr != NULL &&
						gActiveImageViewCPtr->GetImageType () == kMultispectralImageType)
		{
		comboXLabel->Append (wxT("Wavelength (um)"));
		comboXLabel->SetClientData (1, (void*)kWavelengths);
		comboXLabel->Append (wxT("Wavelength-bandwidths (um)"));
		comboXLabel->SetClientData (2, (void*)kBandWidths);
		comboXLabel->Append (wxT("Wavelength-reflective (um)"));
		comboXLabel->SetClientData (3, (void*)kReflectiveWavelengths);
		comboXLabel->Append (wxT("Wavelength-reflective bandwidths (um)"));
		comboXLabel->SetClientData (4, (void*)kReflectiveBandWidths);
		comboXLabel->Append (wxT("Wavelength-thermal (um)"));
		comboXLabel->SetClientData (5, (void*)kThermalWavelengths);
		comboXLabel->Append (wxT("Wavelength-thermal bandwidths (um)"));
		comboXLabel->SetClientData (6, (void*)kThermalBandWidths);
		
		comboXLabel->Enable (true);
		
		}	// end "if (gActiveImageViewCPtr != NULL && ..."
		
	else	// Not multispectral image, hide the menu for now.
		comboXLabel->Hide ();

	comboXLabel->SetSelection (0); 
	
	return (comboXLabel);
    
}	// end "CreateXAxisComboControl"



void CMGraphFrame::OnBinWidth (
				wxCommandEvent&					event)

{
	SInt32								menuID,
											selection;
	
	
   menuID = event.GetId ();
	
   selection = menuID - ID_SELECTBINWIDTHMENUITEMSTART + 1;
	
   m_graphViewCPtr->OnBinWidth (selection);

   if (binWidthCheckID != menuID)
		{
      m_binWidthMenu->Check (menuID, 1);
      m_binWidthMenu->Check (binWidthCheckID, 0);
      binWidthCheckID = menuID;
		
		}	// end "if (binWidthCheckID != menuID)"
	
}	// end "OnBinWidth"



void CMGraphFrame::DoNextChannel (
				wxMouseEvent& 						event)

{
	DoNextOrPreviousChannelEvent (event, kNextGraphSetControl);
	
}	// end "DoNextChannel"



void CMGraphFrame::DoNextOrPreviousChannelEvent (
				wxMouseEvent& 						event,
				SInt16								controlIdentifier)

{
	time_t								currentTickCount,
											nextControlTime;
	
   Boolean								controlDelayFlag,
   										leftMouseStillDownFlag = TRUE;
	
	
	nextControlTime = GetTickCount ();
	while (leftMouseStillDownFlag)
		{
		controlDelayFlag = !wxGetKeyState (WXK_CONTROL);
		currentTickCount = GetTickCount ();
		
		if (currentTickCount >= nextControlTime || !controlDelayFlag)
			{
			nextControlTime = currentTickCount + gControlOffset;
			
			DoNextOrPreviousChannel	(m_graphViewCPtr, controlIdentifier);
			if (!UpdateGraphChannels ())
				break;
	
			#if defined multispec_wxlin
				Update ();
			#endif
		
			#if defined multispec_wxmac
				((CMultiSpecApp*)wxTheApp)->SafeYieldFor (NULL, wxEVT_CATEGORY_UI);
			#endif
			
			}	// end "if (!controlDelayFlag || (GetTickCount () > m_nextControlTime))
		
		leftMouseStillDownFlag = wxGetMouseState ().LeftIsDown ();
		
		}	// end "while (leftMouseStillDownFlag)"
	
}	// end "DoNextOrPreviousChannelEvent"



void CMGraphFrame::DoPreviousChannel (
				wxMouseEvent& 						event)

{
	DoNextOrPreviousChannelEvent (event, kPreviousGraphSetControl);
	
}	// end "DoPreviousChannel"



void CMGraphFrame::DoSelectVectors (
				wxMouseEvent& 						event)

{
	GraphPtr								graphRecordPtr;
	Handle								graphRecordHandle;
   SInt16								selection;
	
	
   selection = (SInt16)0;
   SetVectorDisplayList (m_graphViewCPtr, (SInt16)selection);

   graphRecordHandle = m_graphViewCPtr->GetGraphRecordHandle ();
	graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle,
																kLock);
	
   SetGraphMinMax (graphRecordPtr, kBothXYAxes);
   Refresh ();
	
}	// end "DoSelectVectors"



bool CMGraphFrame::OnCreateClient ()
		
{
   Boolean returnFlag = FALSE;
   if (wxDocChildFrame::GetView ())
   	{
      ((CMGraphDoc*)m_childDocument)->SetGraphFrameCPtr (this);
      returnFlag = TRUE;
		
   	}	// end "if (wxDocChildFrame::GetView ())"
   	
   return (returnFlag);
   
}	// end "OnCreateClient"



void CMGraphFrame::OnChangeXAxis (
				wxCommandEvent&					WXUNUSED (event))

{
	GraphPtr								graphRecordPtr;
	
	SInt64								xAxisCode64;
	
	int 									currentSelection,
											xAxisCode,
											xAxisSelection;
	
	
	if (m_comboXlabel != NULL)
		{
		xAxisSelection = m_comboXlabel->GetSelection ();			
		xAxisCode64 = (SInt64)((int*)m_comboXlabel->GetClientData (xAxisSelection));
		xAxisCode = (int)xAxisCode64;
		
		graphRecordPtr = (GraphPtr)GetHandlePointer (
													m_graphViewCPtr->m_graphRecordHandle);
													
		currentSelection = graphRecordPtr->xAxisCode;
		
		if (xAxisCode > 0 && currentSelection != xAxisCode)
			{
         graphRecordPtr->xAxisCode = xAxisCode;
         
         if (xAxisCode == kChannels)
         	graphRecordPtr->xValueTypeCode = kIntegerType;
         	
         else	// xAxisCode != kChannels
         	graphRecordPtr->xValueTypeCode = kRealType;
         
			ReloadXAxis (graphRecordPtr, xAxisCode);
   
			m_panel2->Refresh ();
		
			}	// end "if (xAxisCode > 0 && currentSelection != xAxisCode)"
		
		}	// end "if (m_comboXlabel != NULL)"
	
}	// end "OnChangeXAxis"



void CMGraphFrame::OnData (
				wxCommandEvent&					WXUNUSED (event))

{
   wxCheckBox* dataBtn = (wxCheckBox*)FindWindow (ID_DATALIST);
   m_dataListShowFlag = dataBtn->GetValue ();
   bool featureListShowFlag = m_featureListShowFlag;
      
   if (gActiveImageViewCPtr != NULL)
   	{ 
				// Update show feature/data flag
				
      gActiveImageViewCPtr->m_Canvas->m_dataListShowFlag = m_dataListShowFlag;
      featureListShowFlag = gActiveImageViewCPtr->m_Canvas->m_featureListShowFlag;
		
   	}	// end " if (gActiveImageViewCPtr != NULL)"
	
   if (m_dataListShowFlag == true) 
		{
      m_splitter3->SplitHorizontally (m_panel3, m_panel4, 150);
      m_graphViewCPtr->UpdateDataListCtrl ();
      m_listCtrl2->Show ();

      if (featureListShowFlag == false)
         m_splitter3->Unsplit (m_panel4);
			
      else 
         m_checkBoxFeature->SetValue (true);

		}	// end "if (m_dataListShowFlag == true)"
		
   else	// m_dataListShowFlag != true"
   	{
      m_listCtrl2->Hide ();
      if (featureListShowFlag == true)
         m_splitter3->Unsplit (m_panel3); 
			
   	}	// end "else m_dataListShowFlag != true"
   
   UpdateSplitterWindowLayout ();
	
}	// end "OnData"



void CMGraphFrame::OnFeature (
				wxCommandEvent&					WXUNUSED (event))

{
   wxCheckBox* featureBtn = (wxCheckBox*)FindWindow (ID_FEATURELIST);   
   m_featureListShowFlag  = featureBtn->GetValue ();
   bool dataListShowFlag = m_dataListShowFlag;
   
   if (gActiveImageViewCPtr != NULL)
		{
      dataListShowFlag = gActiveImageViewCPtr->m_Canvas->m_dataListShowFlag;
      gActiveImageViewCPtr->m_Canvas->m_featureListShowFlag = m_featureListShowFlag;
		
		}	// end "if (gActiveImageViewCPtr != NULL)"
   
   if (m_featureListShowFlag == true)
		{
      m_listCtrl1->Show ();
      m_graphViewCPtr->UpdateShowOrHideFeatureList ();
      m_splitter3->SplitHorizontally (m_panel3, m_panel4, 0);
      if (dataListShowFlag == true)
			{
         m_splitter3->SetSashPosition (150, true);     
         m_checkBoxData->SetValue (true);
			
			}	// end "if (dataListShowFlag == true)"
			
		else
         m_splitter3->Unsplit (m_panel3); 
			    
		}	// end "if (m_featureListShowFlag == true)"
		
	else	// m_featureListShowFlag != true
		{
      m_listCtrl1->Hide ();
      m_splitter3->SetSashPosition (30,true);
      m_splitter3->Unsplit (m_panel4); 
		
		}	// end "else m_featureListShowFlag != true"
  
   UpdateSplitterWindowLayout ();
   
}	// end "OnFeature"



void CMGraphFrame::OnFilePrint ()

{             
	//((CMultiSpecApp*)AfxGetApp ())->SetPrintOrientation (GetActiveWindowInfoHandle ());
   
	//m_graphViewCPtr->DoFilePrint ();
	
}	// end "OnFilePrint" 



void CMGraphFrame::OnGraphWindow (
				wxCommandEvent&					WXUNUSED (event))

{
   wxCheckBox* graphBtn = (wxCheckBox*)FindWindow (ID_GRAPHWINDOW);
	 
	if (graphBtn != NULL)
		{
		if (graphBtn->GetValue () == true)
			{
			m_panel2->Show ();
			m_splitter2->SplitHorizontally (m_panel1, m_panel2, 45);      
			m_splitter1->SetSashInvisible (false);
			m_splitter1->SetSashPosition (300, true);
			
			}	// end "if (graphBtn->GetValue () == true)"
			
		else	// graphBtn->GetValue () == false
			{      
			m_panel2->Hide ();  
			m_splitter2->Unsplit (m_panel2);  
			m_splitter1->SetSashPosition (45, true);
			m_splitter1->SetSashInvisible (true);
			
			}	// end "else graphBtn->GetValue () == false"
			
		UpdateSplitterWindowLayout ();
		Refresh (false);
		
		}	// end "if (graphBtn != NULL)"
	
}	// end "OnGraphWindow" 



void CMGraphFrame::OnMaximizeWindow (
				wxMaximizeEvent&					event)

{
	wxPoint 								ImgNewOrig = GetPosition (),
   	 									MainOrig;
	
   int									clientWidth,
   										clientHeight;
	
	
   Maximize (false);
   
   if (m_frameMaximized == true)
   	{ 
				// Currently maximized, set to the saved values
				
      Restore ();
      SetPosition (m_framePosSaved);         
      SetSize (m_frameSizeSaved.x, m_frameSizeSaved.y);       
      m_frameMaximized = false; 
		
      }	// end "if (m_frameMaximized == true)"
      
   else	// m_frameMaximized != true
   	{    
				// Currently unmaximized, save the current settings
				
      m_frameSizeSaved = GetSize ();
      GetPosition (&m_framePosSaved.x, &m_framePosSaved.y);
      GetMainFrame()->GetClientSize (&clientWidth, &clientHeight);
      GetMainFrame()->GetPosition (&MainOrig.x, &MainOrig.y);
      UInt16 MainFrameBarHeight;
		#ifdef NetBeansProject
         MainFrameBarHeight = 80;
		#else	// mygeohub
         MainFrameBarHeight = 75;
         clientHeight -= 30;
		#endif

      SetSize (clientWidth, clientHeight); 
      Restore ();
      ImgNewOrig = wxPoint (MainOrig.x, MainOrig.y+ MainFrameBarHeight);
      SetPosition (ImgNewOrig);   
      m_frameMaximized = true;
      
   	}	// end "else m_frameMaximized != true"
   
}	// end "OnMaximizeWindow"



void CMGraphFrame::OnNextListData (
				wxCommandEvent&					event)

{
	gListDataCode = 1;

}	// end "OnNextListData"



void CMGraphFrame::OnOverlay (
				wxCommandEvent&					event)

{
   int									menuID;
	
	SInt32								selection; 
	        
   
   menuID = event.GetId ();
   
   selection = menuID - ID_GRAPHOVERLAYMENUITEMSTART + 1;
   
   m_graphViewCPtr->OnOverlay (selection);   
   
	SetOverlayMenuCheck (menuID);
   	
}	// end "OnOverlay"



void CMGraphFrame::OnPaint (
				wxPaintEvent&						event)

{
   if (gProcessorCode == kSelectionGraphProcessor)
																								return;
   
   wxPaintDC dc (this);
   m_graphViewCPtr->OnDraw (&dc);
	
}	// end "OnPaint"



void CMGraphFrame::DoShowBinWidth (
				wxMouseEvent& 						event)

{
   m_buttonBinWidth->PopupMenu (m_binWidthMenu);
      
}	// end "DoShowBinWidth"



void CMGraphFrame::DoShowOverlay (
				wxMouseEvent& 						event)

{
   m_buttonOverlay->PopupMenu (m_graphOverlayMenu);
      
}	// end "DoShowOverlay"



void CMGraphFrame::DoShowVectors (
				wxMouseEvent& 						event)
 
{
	int 									vectorSize;
	
	
   vectorSize = m_selectionVector;
      
   if (m_selectVectorMenu->IsChecked (ID_SELECTVECTORMENUITEMSTART+vectorSize+1))
      m_selectVectorMenu->Check (ID_SELECTVECTORMENUITEMSTART+vectorSize+1, 0); 
   
   if (m_selectVectorMenu->IsChecked (ID_SELECTVECTORMENUITEMSTART+vectorSize+2))
      m_selectVectorMenu->Check (ID_SELECTVECTORMENUITEMSTART+vectorSize+2, 0); 
	
   m_buttonVectors->PopupMenu (m_selectVectorMenu);

}	// end "DoShowVectors"



void CMGraphFrame::OnSelectVector (
				wxCommandEvent&					event)

{
	int									line,
											vectorSize;
											
	SInt32								menuID,
											selection;

   
   menuID = event.GetId ();
   
   selection = menuID - ID_SELECTVECTORMENUITEMSTART;
   
   vectorSize = m_selectionVector;
   
   m_graphViewCPtr->OnSelectVector (selection);

   if (selection == vectorSize+2)
   	{
      for (line=0; line<vectorSize; line++)
         m_selectVectorMenu->Check (ID_SELECTVECTORMENUITEMSTART+line, 0); 

   	}	// end "if (selection == vectorSize+2)"
   
   else if (selection == vectorSize+1)
   	{
      for (line=0; line<vectorSize; line++)
         m_selectVectorMenu->Check (ID_SELECTVECTORMENUITEMSTART+line, 1); 
			
   	}	// end "if (selection == vectorSize+1)"
    
}	// end "OnSelectVector"



void CMGraphFrame::OnSize (
				wxSizeEvent&						event)
				
{
   Refresh ();

	if (m_listCtrl1 != NULL)
		{
		wxSize listSize = m_listCtrl1->GetSize ();
		listSize.IncBy (0, 1);
	
		m_listCtrl1->SetSize (listSize);
		m_listCtrl1->SetColumnWidth (1, 150);
		m_listCtrl1->Refresh (true);
		
		}	// end "if (m_listCtrl1 != NULL)"

	event.Skip ();
	
}	// end "OnSize"



void CMGraphFrame::OnUpdateFilePrint (
				wxUpdateUIEvent&					pCmdUI)

{                                                        
	Boolean enableFlag = UpdateFileGraphPrint (&pCmdUI);
	                                                                                     
	pCmdUI.Enable (enableFlag); 
	
}	// end "OnUpdateFilePrint"



void CMGraphFrame::OnUpdateFilePrintPreview (
				wxUpdateUIEvent&					pCmdUI)

{                                                        
	                                                                                     
	pCmdUI.Enable (TRUE); 
	
}	// end "OnUpdateFilePrintPreview"



void CMGraphFrame::OnUpdateFilePrintSetup (
				wxUpdateUIEvent&					pCmdUI)
				
{
			// Set to false until set up.
			
    pCmdUI.Enable (false);

}	// end "OnUpdateFilePrintSetup"



void CMGraphFrame::OnUpdateFileSaveAs (
				wxUpdateUIEvent&					event) 

{

	UpdateFileOutputTextSaveAs (&event);
	
	if (((CMTextDoc*)GetDocument())->IsModified ())
		event.Enable (true);
	  
	else
		event.Enable (false);
	  
}	// end "OnUpdateFileSaveAs"



void CMGraphFrame::OnUpdateMagnification (
				wxUpdateUIEvent&					event)

{
	event.Enable (false);
		
	CMainFrame* pMainFrame = (CMainFrame*)wxGetApp ().GetTopWindow ();
	if (pMainFrame != NULL)
		pMainFrame->UpdateStatusBar (NULL, -1);
		
}	// end "OnUpdateMagnification"



void CMGraphFrame::OnUpdateOverlay (
				wxUpdateUIEvent&					event)
 
{
   event.Enable (false);

}	// end "OnUpdateOverlay"



void CMGraphFrame::OnUpdateWindowNewSelectionGraph (
				wxUpdateUIEvent&					pCmdUI)
{
   pCmdUI.Enable (true);

}	// end "OnUpdateWindowNewSelectionGraph"



void CMGraphFrame::OnUpdateZoomIn (
				wxUpdateUIEvent&					event) 

{
      event.Enable (false);
		
}	// end "OnUpdateZoomIn"


void CMGraphFrame::OnUpdateZoomOut (
				wxUpdateUIEvent&					event) 

{
      event.Enable (false);
		
}	// end "OnUpdateZoomOut"



void CMGraphFrame::Render (
				wxDC&									dc)

{
	wxColour								blue,
											brown, 
											gray, 
											green, 
											orange, 
											red, 
											white;
											

	gray.Set (wxT("#d4d4d4"));
	white.Set (wxT("#ffffff"));
	red.Set (wxT("#ff0000"));
	orange.Set (wxT("#fa8e00"));
	green.Set (wxT("#619e1b"));
	brown.Set (wxT("#715b33"));
	blue.Set (wxT("#0d0060"));

	dc.SetPen (wxPen (gray));
	dc.SetBrush (wxBrush (white));

}	// end "Render"



void CMGraphFrame::SetGraphViewCPtr (
				CMGraphView*						graphViewCPtr)
				
{  
   //if (graphViewCPtr != NULL)

	m_graphViewCPtr = graphViewCPtr;
	
}	// end "SetGraphViewCPtr"



void CMGraphFrame::SetOverlayMenuCheck (
				int									selectedMenuID)

{
   m_graphOverlayMenu->Check (m_overlayCheckID, 0);
   m_graphOverlayMenu->Check (selectedMenuID, 1);
   m_overlayCheckID = selectedMenuID;
	
}	// end "SetOverlayMenuCheck"



void CMGraphFrame::UpdateExpandFlag ()

{			
	Handle*								shapeHandlePtr;
	OverlaySpecsPtr					overlayListPtr;
   ShapeInfoPtr						shapeInfoPtr = NULL;
   WindowInfoPtr						windowInfoPtr;
  
   int									index = 0, 
											numberOverlays, 
											shapeFileIndex;
	
   SignedByte							windowHandleStatus;
	

			// If it's a shapefile selected, change the expand/collapse flag

   windowInfoPtr = (WindowInfoPtr) GetHandleStatusAndPointer (
                      gActiveImageWindowInfoH, &windowHandleStatus);
   if (windowInfoPtr == NULL || !m_checkBoxFeature->IsEnabled ()) 
																									return;   
      
   shapeHandlePtr = (Handle*)GetHandlePointer (gShapeFilesHandle, kLock);
   
   overlayListPtr = windowInfoPtr->overlayList;
   numberOverlays = (shapeHandlePtr == NULL)? 0 : windowInfoPtr->numberVectorOverlays;
   
   if (numberOverlays == 0) 
      m_listCtrl1->DeleteAllItems ();
	
   wxTreeListItem rootitem = m_listCtrl1->GetRootItem ();
   wxTreeListItem firstitem = m_listCtrl1->GetFirstChild (rootitem);
   
   while (firstitem != NULL)
		{      
      shapeFileIndex = overlayListPtr[index++].index;
      shapeFileIndex = abs (shapeFileIndex) - 1;
      shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (shapeHandlePtr[shapeFileIndex],
																		kLock);
      shapeInfoPtr->expandFeatureList = m_listCtrl1->IsExpanded (firstitem);
      firstitem = m_listCtrl1->GetNextSibling (firstitem);
		
		}	// end "while (firstitem != NULL)"
   
}	// end "UpdateExpandFlag"



Boolean CMGraphFrame::UpdateGraphChannels (void)

{
   GraphPtr       					graphRecordPtr;
	
	SInt16								numberSets,
											numberVectors,
											set;
	
	Boolean								returnFlag = TRUE;
	
	
   graphRecordPtr = (GraphPtr)GetHandlePointer (m_graphViewCPtr->m_graphRecordHandle);
	
   numberSets = graphRecordPtr->numberSets;
	set = graphRecordPtr->set;
	numberVectors = graphRecordPtr->numberVectors;
	
   if (numberSets <= 1)
   	{
		m_buttonNext->Enable (false);
		m_buttonPrevious->Enable (false);
		
   	}	// end "if (numberSets <= 1)"
	
   else	// numberSets > 1
   	{
      if (set < numberSets)
			m_buttonNext->Enable (true);

      else	// set >= numberSets
      	{
			m_buttonNext->Enable (false);
			returnFlag = FALSE;
			
			}	// end "else set >= numberSets"
		
      if (set > 1)
			m_buttonPrevious->Enable (true);
			
      else	// >set <= 1
      	{
			m_buttonPrevious->Enable (false);
			returnFlag = FALSE;
		
			}	// end "else set >= numberSets"
			
   	}	// end "elseset <= 1"

   return (returnFlag);
	
}	// end "UpdateGraphChannels"



void CMGraphFrame::UpdateSplitterWindowLayout ()

{
   wxSize 								windowSize,
   										splitterWinSize;
	
   bool 									featureListShowFlag = false,
   	 									dataListShowFlag = false;
	
   
   if (gActiveImageViewCPtr != NULL)
		{
      featureListShowFlag = gActiveImageViewCPtr->m_Canvas->m_featureListShowFlag;
      dataListShowFlag = gActiveImageViewCPtr->m_Canvas->m_dataListShowFlag;
		
		}	// end "if (gActiveImageViewCPtr != NULL)"
	
	else	// gActiveImageViewCPtr == NULL
		{
		dataListShowFlag = m_dataListShowFlag;
		featureListShowFlag = m_featureListShowFlag;
		
		}	// end "else gActiveImageViewCPtr == NULL"
   
   if (dataListShowFlag == true && featureListShowFlag == true)
		{
      m_splitter3->SetSashInvisible (false);   
      m_splitter3->SetSashPosition (m_splitter3->GetSashPosition (), true);
		
		}	// end "if (dataListShowFlag == true && featureListShowFlag == true)"
	
	else
      m_splitter3->SetSashInvisible (true); 
		  
   if (dataListShowFlag == false && featureListShowFlag == false)
		{
		Boolean  splitter1SplitFlag,
					splitter2SplitFlag,
					splitter3SplitFlag;
		
		splitter1SplitFlag = m_splitter1->IsSplit ();
		splitter2SplitFlag = m_splitter2->IsSplit ();
		splitter3SplitFlag = m_splitter3->IsSplit ();
		
      splitterWinSize = m_splitter2->GetSize ();
		windowSize = GetSize ();
      #ifdef NetBeansProject
			windowSize = splitterWinSize + wxSize (4, 52);
      #else
      	#if defined multispec_wxmac
				windowSize = splitterWinSize + wxSize (4, 50);
				if (splitter1SplitFlag)
							// Being unsplit; need to add 2 pixels back
					windowSize.y += 2;
		
				windowSize.y = MAX (m_minFrameYSize, windowSize.y);
			#else
				windowSize = splitterWinSize + wxSize (0, 20);
			#endif
      #endif

      m_splitter1->Unsplit (m_splitter3);
      SetSize (windowSize);
		
		}	// end "if (dataListShowFlag == false && featureListShowFlag == false)"
		
   else	// dataListShowFlag || featureListShowFlag
		{
      if (!m_splitter1->IsSplit ())
			{
			int maximumGraphHeight;
			
					// if not split,  expand the height and then split the window
					
         windowSize = this->GetSize ();
         splitterWinSize = m_splitter2->GetSize ();
				
         		// Get maximum height for the graph window.
				
			#if defined multispec_wxlin
   			int		clientWidth,
   						menuHeight,
   						menuWidth,
   						toolBarHeight,
   						toolBarWidth;
	
				GetMainFrame()->m_menubar1->GetSize (&menuWidth, &menuHeight);
				GetMainFrame()->m_toolBar1->GetSize (&toolBarWidth, &toolBarHeight);
		
				GetMainFrame()->GetClientSize (&clientWidth, &maximumGraphHeight);

						// Need to allow for MultiSpec Title line in MyGeoHub Workspace
		
				maximumGraphHeight -= (27 + menuHeight + toolBarHeight);
			#endif
			
			#if defined multispec_wxmac
				wxRect	clientRect;
		
				clientRect = wxDisplay().GetClientArea ();
				maximumGraphHeight = clientRect.GetHeight ();
				maximumGraphHeight -= clientRect.GetTop ();
			#endif
				
         #ifdef NetBeansProject
				windowSize.y = MIN ((int)(windowSize.y * 1.8), 600);
         #else // mygeohub
				windowSize.y = MIN ((int)(windowSize.y * 1.6), maximumGraphHeight);
			#endif

         m_splitter1->SetSashGravity (0);
         m_splitter1->SplitHorizontally (m_splitter2, m_splitter3, splitterWinSize.y);
         m_splitter1->SetMinimumPaneSize (175);  
         
         SetSize (windowSize);
			
			}	// end "if (!m_splitter1->IsSplit ())"
		
		}	// end "else dataListShowFlag || featureListShowFlag"
   
   if (!m_checkBoxGraph->GetValue ())
		{
      m_splitter1->SetSashGravity (0.0);      
      m_splitter1->SetMinimumPaneSize (45);  
      m_splitter1->SetSashPosition (45, true);
		
		}	// end "if (!m_checkBoxGraph->GetValue ())"
		
}	// end "UpdateSplitterWindowLayout"

