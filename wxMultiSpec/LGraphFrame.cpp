//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Implementation file:	LGraphFrame.cpp
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			02/20/2017 by Wei-Kang Hsu
//								11/20/2018 by Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	The routines in this file control the frame class for 
//								graph windows.
//	
/*	Template for debugging.
	int numberChars = sprintf ((char*)&gTextString3,
											" LGraphFrame: (): %s",
											gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);	
*/				
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "LGraphView.h" 
#include "LGraphFrame.h"

#include "LImage_dialog.cpp"
#include "LGraphWindow_img.cpp"
#include "wx/treelist.h"

IMPLEMENT_CLASS (CMGraphFrame, wxDocChildFrame)
IMPLEMENT_CLASS (CMGraphCanvas, wxPanel)
/////////////////////////////////////////////////////////////////////////////
// CMGraphCanvas 
// catch paint events
BEGIN_EVENT_TABLE (CMGraphCanvas, wxPanel)
	EVT_PAINT (CMGraphCanvas::paintEvent) 
END_EVENT_TABLE ()

CMGraphCanvas::CMGraphCanvas (wxWindow* parent) : wxPanel (parent)

{
}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh ()/Update ().
 */
void CMGraphCanvas::paintEvent (
				wxPaintEvent&							evt)
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
				wxDC&										dc)

{
	m_graphViewCPtr->OnDraw (&dc);
	
}	// end "render"



		// CMGraphFrame frame

CMGraphFrame::CMGraphFrame ()

{
	m_graphViewCPtr = NULL;
   selectionVector = NULL;
	m_listCtrl1 = NULL;
	m_comboXlabel = NULL;
	m_dataListShowFlag = false;
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
				long									style) : 
		wxDocChildFrame (doc, view, parent, id, title, pos, size, style)
		 
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
   //this->Connect (wxEVT_PAINT, wxPaintEventHandler (CMGraphFrame::OnPaint));
   m_statusBar1 = this->CreateStatusBar (1, wxST_SIZEGRIP, wxID_ANY);
	
   m_graphViewCPtr = (CMGraphView*)view;
	m_comboXlabel = NULL;
   m_panel2 = NULL;
   m_listCtrl1 = NULL;
   m_listCtrl2 = NULL;
	m_toolBar1 = NULL;
   
   m_initGraphUpdate = false;
   
   m_frameMaximized = false;
	
	graphRecordPtr = (GraphPtr)GetHandlePointer (
													m_graphViewCPtr->m_graphRecordHandle);
   
   if (gProcessorCode == kHistogramStatsProcessor)
      CreateButton ();
		
   else if (gProcessorCode == kListDataProcessor)
		{
      CreateControlsListData ();
		
      //Handle fileInfoHandle = GetFileInfoHandle (gActiveImageWindowInfoH);
      //FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
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
      
      //CreateButton2();
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
	
	entries[6].Set (wxACCEL_CTRL, (int) 'Z', wxID_UNDO);
	entries[7].Set (wxACCEL_CTRL, (int) 'X', wxID_CUT);
	entries[8].Set (wxACCEL_CTRL, (int) 'C', wxID_COPY);
	entries[9].Set (wxACCEL_CTRL, (int) 'V', wxID_PASTE);
	entries[10].Set (wxACCEL_NORMAL, (int) WXK_DELETE, wxID_CLEAR);
	entries[11].Set (wxACCEL_CTRL, (int) 'A', ID_EDIT_SELECT_ALL);
	entries[12].Set (wxACCEL_CTRL, (int) ',', ID_EDIT_IMAGE_DESCRIPTION);
	entries[13].Set (wxACCEL_CTRL, (int) '`', ID_EDIT_IMAGE_MAP_PARAMETERS);
	
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
}



BEGIN_EVENT_TABLE (CMGraphFrame, wxDocChildFrame)
	EVT_UPDATE_UI (wxID_PRINT, CMGraphFrame::OnUpdateFilePrint)
	EVT_UPDATE_UI (wxID_PREVIEW, CMGraphFrame::OnUpdateFilePrintPreview)
	EVT_UPDATE_UI (wxID_PAGE_SETUP, CMGraphFrame::OnUpdateFilePrintSetup)
	EVT_UPDATE_UI (ID_FILE_SAVE_AS, CMGraphFrame::OnUpdateFileSaveAs)

	EVT_UPDATE_UI (ID_WINDOW_NEW_SELECTION_GRAPH, 
						CMGraphFrame::OnUpdateWindowNewSelectionGraph)
	EVT_UPDATE_UI (ID_WINDOW_SHOW_COORDINATE_VIEW, 
						CMGraphFrame::OnUpdateWindowShowCoordinateView)
	EVT_UPDATE_UI (ID_MAGNIFICATION, CMGraphFrame::OnUpdateMagnification)
	EVT_UPDATE_UI (ID_ZOOM_IN, CMGraphFrame::OnUpdateZoomIn)
	EVT_UPDATE_UI (ID_ZOOM_OUT, CMGraphFrame::OnUpdateZoomOut)
	EVT_UPDATE_UI (ID_OVERLAY, CMGraphFrame::OnUpdateOverlay)
	EVT_TOOL (IDC_BinWidth, CMGraphFrame::OnShowBinWidth)
	EVT_TOOL (IDC_GraphOverlay, CMGraphFrame::OnShowOverlay)
	EVT_TOOL (IDC_SelectVectors, CMGraphFrame::OnShowSelectVector)
	EVT_MENU_RANGE (ID_SELECTBINWIDTHMENUITEMSTART, 
							ID_SELECTBINWIDTHMENUITEMSTART+10, 
							CMGraphFrame::OnBinWidth)
	EVT_MENU_RANGE (ID_GRAPHOVERLAYMENUITEMSTART, 
							ID_GRAPHOVERLAYMENUITEMSTART+10, 
							CMGraphFrame::OnOverlay)
	EVT_MENU_RANGE (ID_SELECTVECTORMENUITEMSTART, 
							ID_SELECTVECTORMENUITEMSTART+50, 
							CMGraphFrame::OnSelectVector)
	EVT_PAINT (CMGraphFrame::OnPaint)
	EVT_SIZE (CMGraphFrame::OnSize)
//	EVT_TOOL (IDC_NEXT_CHANNEL, CMGraphFrame::OnNextListData)
   EVT_BUTTON (IDC_NEXT_CHANNEL, CMGraphFrame::OnNextListData)
	EVT_MAXIMIZE (CMGraphFrame::OnMaximizeWindow)
	EVT_CHECKBOX (ID_GRAPHWINDOW, CMGraphFrame::OnGraphWindow)
	EVT_CHECKBOX (ID_FEATURELIST, CMGraphFrame::OnFeature)
	EVT_CHECKBOX (ID_DATALIST, CMGraphFrame::OnData)   
	EVT_COMBOBOX (ID_SelectionWinXlabelCombo, CMGraphFrame::OnChangeXAxis)
	//EVT_TREELIST_ITEM_CHECKED (IDC_FEATURE_LIST, CMGraphFrame::UpdateExpandFlag)
END_EVENT_TABLE ()



void CMGraphFrame::CreateBinWidth () 

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
	//SetUpWindowOverlayPopUpMenu ((Handle) m_binWidthMenu, m_optionBinWidthFlag);
	
}	// end "CreateBinWidth"



void CMGraphFrame::CreateButton ()

{
	wxBitmap overlay = wxBITMAP_PNG_FROM_DATA (overlay1);
	wxBitmap binwidth = wxBITMAP_PNG_FROM_DATA (binwidth);
	wxBitmap bin = wxBITMAP_PNG_FROM_DATA (bin);
	wxBitmap arrowup = wxBITMAP_PNG_FROM_DATA (arrowup);
	wxBitmap arrowdown = wxBITMAP_PNG_FROM_DATA (arrowdown);
  
	m_toolBar1 = CreateToolBar (wxTB_BOTTOM, wxID_ANY);
  
	wxFont font (gFontSize, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxStaticText *text = new wxStaticText (m_toolBar1, 
														wxID_ANY, 
														wxT("Change Channel:")); 
	//wxStaticText *text1 = new wxStaticText (m_toolBar1, wxID_ANY, wxT("Bin width:1"));
	text->SetFont (font);
	m_toolBar1->AddControl (text, wxT("tool"));
	m_toolBar1->AddTool (IDC_NextChannel, 
								wxT("tool"), 
								arrowup, 
								wxNullBitmap, 
								wxITEM_NORMAL, 
								wxT(""), 
								wxEmptyString);
								
	m_toolBar1->AddTool (IDC_PreviousChannel, 
								wxT("tool"), arrowdown, 
								wxNullBitmap, 
								wxITEM_NORMAL, 
								wxT(""), 
								wxEmptyString);
								
	m_toolBar1->AddTool (IDC_SelectVectors, 
								wxT("tool"), 
								bin, 
								wxNullBitmap, 
								wxITEM_NORMAL, 
								wxT(""), 
								wxEmptyString);
								
	m_toolBar1->AddTool (IDC_GraphOverlay, 
								wxT("tool"), 
								overlay, 
								wxNullBitmap, 
								wxITEM_NORMAL, 
								wxT(""), 
								wxEmptyString);
								
	m_toolBar1->AddTool (IDC_BinWidth, 
								wxT("tool"), 
								binwidth, 
								wxNullBitmap, 
								wxITEM_NORMAL, 
								wxT(""), 
								wxEmptyString);
  
	m_toolBar1->Realize ();
  
	CreateBinWidth ();
	CreateOverlay ();
	
}	// end "CreateButton"


/*
void CMGraphFrame::CreateButton2 ()

{
	wxBitmap rightarrow = wxBITMAP_PNG_FROM_DATA (rightarrow);
	m_toolBar1 = CreateToolBar (wxTB_BOTTOM, wxID_ANY);

	wxFont font (8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	m_toolBar1->AddTool (IDC_NEXT_CHANNEL, 
								wxT("tool"), 
								rightarrow, 
								wxNullBitmap, 
								wxITEM_NORMAL, 
								wxT(""), 
								wxEmptyString);
	
	m_toolBar1->Realize ();  
    
}	// end "CreateButton2"
*/



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
	/*
	m_splitter2->Connect (wxEVT_IDLE, 
									wxIdleEventHandler (Graph Selection Window ::m_splitter2OnIdle), 
									NULL, 
									this);
	*/
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
   /*
	m_panel2 = new wxPanel (m_splitter2, 
										ID_panel2, 
										wxDefaultPosition, 
										wxDefaultSize, 
										wxTAB_TRAVERSAL);
	*/
   wxBoxSizer* bSizer325 = new wxBoxSizer (wxVERTICAL);   
   wxBoxSizer* bSizer3251 = new wxBoxSizer (wxHORIZONTAL);   
   
   m_panel2 = new CMGraphCanvas (m_splitter2);
	
	m_comboXlabel = CreateXAxisComboControl (m_panel2);

   bSizer3251->Add (m_comboXlabel,
							0,
							//wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALL,
							wxALIGN_BOTTOM|wxALL,
							5);
	
   //bSizer325->Add (bSizer3251, 1, wxALIGN_BOTTOM|wxALIGN_CENTER, 5);
   bSizer325->Add (bSizer3251, 1, wxALIGN_CENTER, 5);
   m_panel2->SetSizer (bSizer325);
   m_panel2->Layout ();

	m_splitter2->SplitHorizontally (m_panel1, m_panel2, 45);
	//bSizer321->Add (m_splitter2, 1, wxEXPAND, 5);
	
	m_splitter3 = new wxSplitterWindow (m_splitter1, 
													wxID_ANY, 
													wxDefaultPosition, 
													wxDefaultSize, 
													wxSP_3D|wxSP_LIVE_UPDATE);
   m_splitter3->SetSashGravity (0.5);
	/*
	m_splitter3->Connect (
							wxEVT_IDLE, 
							wxIdleEventHandler (Graph Selection Window ::m_splitter3OnIdle), 
							NULL, 
							this);
	*/
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
	
   wxFont  font (gFontSize, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
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
										150, 
										wxALIGN_LEFT, 
										wxVSCROLL|wxCOL_RESIZABLE);
												
   m_listCtrl1->AppendColumn (wxT("Feature Value"), 
										150, wxALIGN_LEFT, 
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
	this->SetSizer (bSizer321);
	this->Layout ();
	
	this->Centre (wxBOTH);
	
}	// end "CreateControls"



void CMGraphFrame::CreateControlsBiPlot ()

{
   wxBoxSizer* bSizer321;
	
	bSizer321 = new wxBoxSizer (wxVERTICAL);
   m_panel2 = new CMGraphCanvas (this);
 
   bSizer321->Add (m_panel2, 1, wxEXPAND, 0);
	this->SetSizer (bSizer321);
	this->Layout ();
	
	this->Centre (wxBOTH);
	   
}	// end "CreateControlsBiPlot"



void CMGraphFrame::CreateControlsListData ()

{
   wxBoxSizer				*bSizer321,
                        *bSizer322,
								*bSizer3221;
	
	
	bSizer321 = new wxBoxSizer (wxVERTICAL);
	
   m_panel2 = new CMGraphCanvas (this);  

   bSizer322 = new wxBoxSizer (wxHORIZONTAL);
   
   wxBitmap rightarrow = wxBITMAP_PNG_FROM_DATA (rightarrow);
   m_ButtonNext = new wxBitmapButton( m_panel2, IDC_NEXT_CHANNEL, 
                                       rightarrow, 
                                       wxDefaultPosition, 
                                       wxDefaultSize, 
                                       wxBU_AUTODRAW );
	//bSizer322->Add( m_ButtonNext, 0, wxALIGN_BOTTOM|wxALIGN_LEFT|wxLEFT, 25 );
	bSizer322->Add( m_ButtonNext, 0, wxALIGN_BOTTOM|wxLEFT, 25 );
   
   bSizer3221 = new wxBoxSizer (wxVERTICAL);   
	m_comboXlabel = CreateXAxisComboControl (m_panel2);
	
   bSizer3221->Add (m_comboXlabel, 
							0,
							//wxALIGN_BOTTOM|wxALIGN_CENTER|wxALL,
							wxALIGN_CENTER_HORIZONTAL|wxALL,
							5);
	
	//bSizer322->Add (bSizer3221, 1, wxALIGN_BOTTOM|wxALIGN_CENTER, 5);
	bSizer322->Add (bSizer3221, 1, wxALIGN_BOTTOM, 5);
   bSizer321->Add( bSizer322, 1, wxEXPAND, 5 );

   m_panel2->SetSizer (bSizer321);
   m_panel2->Layout();
   
//	this->SetSizer (bSizer321);
//	this->Layout ();
	
	this->Centre (wxBOTH);
	
}	// end "CreateControlsListData"



void CMGraphFrame::CreateOverlay () 

{
	m_graphOverlayMenu = new wxMenu ();

	m_graphOverlayMenu->Append (ID_GRAPHOVERLAYMENUITEMSTART, 
											wxString (wxT("&Draw histogram")), 
											wxT(""), 
											wxITEM_CHECK);

	m_graphOverlayMenu->Append (ID_GRAPHOVERLAYMENUITEMSTART+1, 
											wxString (wxT("&Draw density function")), 
											wxT(""), 
											wxITEM_CHECK);

	m_graphOverlayMenu->Append (ID_GRAPHOVERLAYMENUITEMSTART+2, 
											wxString (wxT("&Draw histogram && density function")), 
											wxT(""), 
											wxITEM_CHECK);

	m_graphOverlayMenu->Check (m_overlayCheckID, 1);
	//SetUpWindowOverlayPopUpMenu ((Handle)m_graphOverlayMenu, 1);
      
}	// end "CreateOverlay"



void CMGraphFrame::CreateSelectVector (
				int									vectorSize)

{
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;

	int									j,
											line, 
											textWidth;
										
										
	m_selectVectorMenu = new wxMenu ();

	selectionVector = vectorSize;

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

	//vectorSize = (int)gProjectInfoPtr->numberStatisticsClasses; 

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

	//SetUpWindowOverlayPopUpMenu ((Handle) m_selectVectorMenu, 1);

}	// end "CreateSelectVector"


wxComboBox* CMGraphFrame::CreateXAxisComboControl (
				wxWindow*							parent)

{
	wxComboBox*					comboXLabel;
	
	
   comboXLabel = new wxComboBox (parent, 
												ID_SelectionWinXlabelCombo, 
												wxT("Channel number"), 
												wxDefaultPosition, 
												wxSize (260,-1),
												0, 
												NULL, 
												0);
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



BOOL CMGraphFrame::OnCreateClient ()
		
{
   Boolean returnFlag = FALSE;
   if (wxDocChildFrame::GetView ())
   	{
      //CMGraphDoc* pContext;
      //pContext->SetGraphFrameCPtr (this);
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
		
		GraphPtr graphRecordPtr = (GraphPtr)GetHandlePointer ( 
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
   
	//this->Update ();
	
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
         m_splitter3->SetSashPosition (150,true);     
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
	//this->Update ();
   
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
		this->Refresh (false);
		
		}	// end "if (graphBtn != NULL)"
	
}	// end "OnGraphWindow" 



void CMGraphFrame::OnMaximizeWindow (
				wxMaximizeEvent&					event)

{
	wxPoint ImgNewOrig = GetPosition ();
   wxPoint MainOrig;
   int	clientWidth,clientHeight;
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
      GetMainFrame ()->GetClientSize (&clientWidth, &clientHeight);
      GetMainFrame ()->GetPosition (&MainOrig.x, &MainOrig.y);
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
//	m_toolBar1->EnableTool (IDC_NEXT_CHANNEL, 1);
   
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



void CMGraphFrame::OnShowBinWidth (
				wxCommandEvent&					event) 

{
   m_toolBar1->PopupMenu (m_binWidthMenu);
      
}	// end "OnShowBinWidth"



void CMGraphFrame::OnShowOverlay (
				wxCommandEvent&					event) 

{
   m_toolBar1->PopupMenu (m_graphOverlayMenu);
      
}	// end "OnShowOverlay"



void CMGraphFrame::OnShowSelectVector (
				wxCommandEvent&					event)
 
{
   int vectorSize;
   
   //vectorSize = (int)gProjectInfoPtr->numberStatisticsClasses;
   vectorSize = selectionVector;
      
   if (m_selectVectorMenu->IsChecked (ID_SELECTVECTORMENUITEMSTART+vectorSize+1))
      m_selectVectorMenu->Check (ID_SELECTVECTORMENUITEMSTART+vectorSize+1, 0); 
   
   if (m_selectVectorMenu->IsChecked (ID_SELECTVECTORMENUITEMSTART+vectorSize+2))
      m_selectVectorMenu->Check (ID_SELECTVECTORMENUITEMSTART+vectorSize+2, 0); 
   
   m_toolBar1->PopupMenu (m_selectVectorMenu);

}	// end "OnShowSelectVector"



void CMGraphFrame::OnSelectVector (
				wxCommandEvent&					event)

{
	int									line,
											vectorSize;
											
	SInt32								menuID,
											selection;

   
   menuID = event.GetId ();
   
   selection = menuID - ID_SELECTVECTORMENUITEMSTART;
   
   vectorSize = selectionVector;
   //vectorSize = (int)gProjectInfoPtr->numberStatisticsClasses;
   
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
		wxSize s = m_listCtrl1->GetSize ();
		s.IncBy (0,1);
	
		m_listCtrl1->SetSize (s);
		m_listCtrl1->SetColumnWidth (1, 150);
		m_listCtrl1->Refresh (true);
		
		}	// end "if (m_listCtrl1 != NULL)"

	event.Skip ();
	
}	// end "OnSize"


/*
void CMGraphFrame::OnUpdateFileGraphClose (
				wxUpdateUIEvent&					pCmdUI) 
{
	UpdateFileGraphClose (&pCmdUI);

	pCmdUI.Enable (TRUE);
	
}
*/


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
	
	if (((CMTextDoc*)GetDocument ())->IsModified ())
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



void CMGraphFrame::OnUpdateWindowShowCoordinateView (
				wxUpdateUIEvent&					pCmdUI) 

{
   pCmdUI.Enable (false);

}	// end "OnUpdateWindowShowCoordinateView"



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
	/*  
	CMGraphView::s_updateRect.top = 20;
	CMGraphView::s_updateRect.left = 20;
	CMGraphView::s_updateRect.bottom = y - 50;
	CMGraphView::s_updateRect.right =  x - 50;

	int numberChars = sprintf (
					(char*)&gTextString3,
					" LGraphFrame::Render (left, right, top, bottom): %d, %d, %d, %d%s", 
					CMGraphView::s_updateRect.left,
					CMGraphView::s_updateRect.right,
					CMGraphView::s_updateRect.top,
					CMGraphView::s_updateRect.bottom,
					gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);
	*/  
	//dc.DrawRectangle (20, 20, x-50, y-50);

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


/*
void CMGraphFrame::SetUpGraphXLabelPopUpMenu (
				Handle								windowInfoHandle)

{
	wxString								wavelength,
											wavelengthUnit;
	
	FileInfoPtr							fileInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	
	SInt64								xAxisCode;
	
	int									currentSelection,
											newSelection;
	
	UInt16								newAxisCode,
											numberComboItems;
				
	
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	
	
	if (windowInfoPtr != NULL)
		{
				// Get the current selection
				
		currentSelection = m_comboXlabel->GetSelection ();			
		xAxisCode = (SInt64)((int*)m_comboXlabel->GetClientData (currentSelection));

				// Remove all items in the list.
				
		m_comboXlabel->Clear ();

		fileInfoPtr = (FileInfoPtr)GetHandlePointer (windowInfoPtr->fileInfoHandle);

				// Get the units string
		
		wavelengthUnit = wxT("um)");
		if (fileInfoPtr != NULL && fileInfoPtr->channelDescriptionUnitString[0] != 0)
			{
			wxString unit (fileInfoPtr->channelDescriptionUnitString, wxConvUTF8);
			wavelengthUnit = unit;
			wavelengthUnit.append (wxT(")"));
			
			}	// end "if (fileInfoPtr->channelDescriptionUnitString[0] != 0)"
			
				// Now add those items that make sense for the data set.
		
		numberComboItems = 0;
		m_comboXlabel->Append (wxT("Channel number"));
		m_comboXlabel->SetClientData (numberComboItems, (void*)kChannels);
		numberComboItems++;
		
		newSelection = kChannels-1;
		newAxisCode = (UInt16)xAxisCode;

		if (windowInfoPtr->descriptionCode & kBothReflectiveThermalData)
			{
			wavelength = wxT("Wavelength (");
			wavelength.append (wavelengthUnit);
			m_comboXlabel->Append (wavelength);
			//m_comboXlabel->Append (wxT("Wavelength (um)"));
			m_comboXlabel->SetClientData (numberComboItems, (void*)kWavelengths);
			if (xAxisCode == kWavelengths)
				newSelection = numberComboItems;
			numberComboItems++;
			
			if (windowInfoPtr->descriptionCode & kBandWidthInfoExists)
				{
				wavelength = wxT("Wavelength-bandwidths (");
				wavelength.append (wavelengthUnit);
				m_comboXlabel->Append (wavelength);
				//m_comboXlabel->Append (wxT("Wavelength-bandwidths (um)"));
				m_comboXlabel->SetClientData (numberComboItems, (void*)kBandWidths);
				if (xAxisCode == kBandWidths)
					newSelection = numberComboItems;
				numberComboItems++;
				
				}	// end "if (windowInfoPtr->descriptionCode & kBandWidthInfoExists)"
			
			if (windowInfoPtr->descriptionCode & kReflectiveData && 
														windowInfoPtr->descriptionCode & kThermalData)
				{
						// Both Reflective and Thermal data exists, all the ability for the 
						// user to select one or the other.
						
				wavelength = wxT("Wavelength-reflective (");
				wavelength.append (wavelengthUnit);
				m_comboXlabel->Append (wavelength);
				//m_comboXlabel->Append (wxT("Wavelength-reflective (um)"));
				m_comboXlabel->SetClientData (
												numberComboItems, (void*)kReflectiveWavelengths);
				if (xAxisCode == kReflectiveWavelengths)
					newSelection = numberComboItems;
				numberComboItems++;
			
				if (windowInfoPtr->descriptionCode & kBandWidthInfoExists)	
					{
					wavelength = wxT("Wavelength-reflective bandwidths (");
					wavelength.append (wavelengthUnit);
					m_comboXlabel->Append (wavelength);
					//m_comboXlabel->Append (wxT("Wavelength-reflective bandwidths (um)"));
					m_comboXlabel->SetClientData (
												numberComboItems, (void*)kReflectiveBandWidths);
					if (xAxisCode == kReflectiveBandWidths)
						newSelection = numberComboItems;
					numberComboItems++;
					
					}	// end "if (windowInfoPtr->descriptionCode & kBandWidthInfoExists)"
								
				wavelength = wxT("Wavelength-thermal (");
				wavelength.append (wavelengthUnit);
				m_comboXlabel->Append (wavelength);
				//m_comboXlabel->Append (wxT("Wavelength-thermal (um)"));
				m_comboXlabel->SetClientData (
												numberComboItems, (void*)kThermalWavelengths);
				if (xAxisCode == kThermalWavelengths)
					newSelection = numberComboItems;
				numberComboItems++;
				
				if (windowInfoPtr->descriptionCode & kBandWidthInfoExists)
					{
					wavelength = wxT("Wavelength-thermal bandwidths (");
					wavelength.append (wavelengthUnit);
					m_comboXlabel->Append (wavelength);
					//m_comboXlabel->Append (wxT("Wavelength-thermal bandwidths (um)"));
					m_comboXlabel->SetClientData (
													numberComboItems, (void*)kThermalBandWidths);
					if (xAxisCode == kThermalBandWidths)
						newSelection = numberComboItems;
					
					}	// end "if (windowInfoPtr->descriptionCode & kBandWidthInfoExists)"
				
				}	// end "if (windowInfoPtr->descriptionCode & kReflectiveData && ...)"
				
			else	// Either reflective or thermal data exists not both
				{
						// Update new selection to be wavelength or bandwidth if the
						// current selection is for reflective or thermal of the same.
						
				if (xAxisCode > kBandWidths)
					{
					newSelection = kWavelengths - 1;
					if ((xAxisCode == kReflectiveBandWidths || 
															xAxisCode == kThermalBandWidths) &&
										windowInfoPtr->descriptionCode & kBandWidthInfoExists)
						newSelection = kBandWidths - 1;
						
					}	// end "if (xAxisCode > kBandWidths)"
				
				}	// end "else Either reflective or thermal data exists not both"
			
			}	// end "if (windowInfoPtr->descriptionCode & kBothReflectiveThermalData)"
			
		m_comboXlabel->SetSelection (newSelection);
		
		if (newSelection != currentSelection)
			//gSelectionGraphViewCPtr->ReloadXAxis (newSelection+1);
			m_graphViewCPtr->ReloadXAxis (newSelection+1);
		
		}	// end "if (windowInfoPtr != NULL)"

}	// end "SetUpGraphXLabelPopUpMenu"
*/


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
	
	/*
	wxTreeListItem itemSelect = event.GetItem ();
	   
	if (m_listCtrl1->GetItemParent (itemSelect) != m_listCtrl1->GetRootItem ())
				Not a shapefile selected
																									return;
   */
			// If it's a shapefile selected, change the expand/collapse flag

   windowInfoPtr = (WindowInfoPtr) GetHandleStatusAndPointer (
                      gActiveImageWindowInfoH, &windowHandleStatus);
   if (windowInfoPtr == NULL || !m_checkBoxFeature->IsEnabled ()) 
																									return;   
      
   shapeHandlePtr = (Handle*)GetHandlePointer (gShapeFilesHandle, kLock);
   
   overlayListPtr = windowInfoPtr->overlayList;
   numberOverlays = (shapeHandlePtr == NULL)? 0 : windowInfoPtr->numberOverlays;
   
   if (numberOverlays == 0) 
      m_listCtrl1->DeleteAllItems ();
   
	//wxTreeListItem tempList = itemSelect;
   wxTreeListItem rootitem = m_listCtrl1->GetRootItem ();
   wxTreeListItem firstitem = m_listCtrl1->GetFirstChild (rootitem);
   
   while (firstitem != NULL)
		{      
      shapeFileIndex = overlayListPtr[index++].index;
      shapeFileIndex = abs (shapeFileIndex) - 1;
      shapeInfoPtr = (ShapeInfoPtr) GetHandlePointer (shapeHandlePtr[shapeFileIndex],
																		kLock);
      shapeInfoPtr->expandFeatureList = m_listCtrl1->IsExpanded (firstitem);
      firstitem = m_listCtrl1->GetNextSibling (firstitem);
		
		}	// end "while (firstitem != NULL)"
   
}	// end "UpdateExpandFlag"



void CMGraphFrame::UpdateSplitterWindowLayout ()

{
   bool featureListShowFlag = false;
   bool dataListShowFlag = false;
   wxSize windowSize, splitterWinSize;
	
   
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
      splitterWinSize = m_splitter2->GetSize ();
		windowSize = GetSize();
      #ifdef NetBeansProject
			windowSize = splitterWinSize + wxSize (4, 52);
      #else
      	#if defined multispec_wxmac
				windowSize = splitterWinSize + wxSize (4, 20);
				windowSize.y = MAX (m_minFrameYSize, windowSize.y);
			#else
				windowSize = splitterWinSize + wxSize (0, 20);
			#endif
      #endif

      m_splitter1->Unsplit (m_splitter3);
      SetSize (windowSize);
		
      //splitterWinSize = m_splitter2->GetSize ();
		//windowSize = GetSize();
		
		}	// end "if (dataListShowFlag == false && featureListShowFlag == false)"
		
   else	// dataListShowFlag || featureListShowFlag
		{
      if (!m_splitter1->IsSplit ())
			{ 
					// if not split,  expand the height and then split the window
					
         windowSize = this->GetSize ();
         splitterWinSize = m_splitter2->GetSize ();
        
         #ifdef NetBeansProject
				windowSize.y = MIN ((int)(windowSize.y * 1.8), 600);
         #else // mygeohub
            windowSize.y = MIN ((int)(windowSize.y *1.6), 600);
         #endif

			//m_splitter1->SetSashGravity (0.5);
         m_splitter1->SetSashGravity (0);
         m_splitter1->SplitHorizontally (m_splitter2, m_splitter3, splitterWinSize.y);
         m_splitter1->SetMinimumPaneSize (175);  
         
         SetSize (windowSize);
			
			}	// end "if (!m_splitter1->IsSplit ())"
			   
		//this->Layout ();     
		}	// end "else dataListShowFlag || featureListShowFlag"
   
   if (!m_checkBoxGraph->GetValue ())
		{
      m_splitter1->SetSashGravity (0.0);      
      m_splitter1->SetMinimumPaneSize (45);  
      m_splitter1->SetSashPosition (45, true);
		
		}	// end "if (!m_checkBoxGraph->GetValue ())"
		
}	// end "UpdateSplitterWindowLayout"


/*
void CMGraphFrame::UpdateWavelengthComboUnit ()

{
   SignedByte							handleStatus,
											windowHandleStatus;
	
	
   if (m_comboXlabel == NULL) 
																										return;
   
   WindowInfoPtr	windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
													gActiveImageWindowInfoH, &windowHandleStatus);
   if (windowInfoPtr == NULL) 
																										return;
      
   FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
													windowInfoPtr->fileInfoHandle, &handleStatus);
													   
	//if (gSelectionGraphViewCPtr == NULL)  
	if (m_graphViewCPtr == NULL)
																										return;
   
	GraphPtr graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (
													//gSelectionGraphViewCPtr->m_graphRecordHandle, 
													m_graphViewCPtr->m_graphRecordHandle,                                         
													&handleStatus);
	
	int numberChars = sprintf ((char*)&gTextString3,
											" LGraphFrame:UpdateWavelengthComboUnit (): %d, %s%s",
											m_comboXlabel->GetCount (),
											&fileInfoPtr->channelDescriptionUnitString[1],
											gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);	


   if (fileInfoPtr != NULL && 
			graphRecordPtr != NULL && 
					m_comboXlabel->GetCount () > 1)
		{
		if (fileInfoPtr->channelDescriptionUnitString[0] != 0)
			{
			wxString wavelenunit (fileInfoPtr->channelDescriptionUnitString, wxConvUTF8);
			wxString wavelength = wxT("Wavelength (");
			wavelength.append (wavelenunit);
			wavelength.append (wxT(")")); 
			//graphRecordPtr->graphViewCPtr->m_frame->m_comboXlabel->SetString (
			m_comboXlabel->SetString (1, wavelength);
			
			}	// end "if (fileInfoPtr->channelDescriptionUnitString[0] != 0)"
			
		else
			//graphRecordPtr->graphViewCPtr->m_frame->m_comboXlabel->SetString (
			m_comboXlabel->SetString (1, wxT("Wavelength (um)"));
		
		}	// end " if (fileInfoPtr != NULL && ..."
		  
}	// end "UpdateWavelengthComboUnit"
*/
