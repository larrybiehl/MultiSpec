// LTextFrame.cpp
//
//	Revised By:			Larry L. Biehl			Date: 01/24/2019
//
/* Template for debugging
	int numberChars = sprintf ((char*)gTextString3,
				" LTextFrame: (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "LMainFrame.h"
#include "LMultiSpec.h"
#include "LOpenFileDialog.h"
#include "LTextDoc.h"
#include "LTextFrame.h"

#include "wx/app.h"
#include "wx/display.h"
#include "wx/filedlg.h"

IMPLEMENT_DYNAMIC_CLASS(CMTextFrame, wxDocChildFrame)

CMTextFrame::CMTextFrame ()
		: wxDocChildFrame (NULL,
									NULL,
									NULL,
									wxID_ANY,
									wxT(""),
									wxDefaultPosition,
									wxDefaultSize,
									wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT)

{
}	// end "CMTextFrame"


CMTextFrame::CMTextFrame (
				wxDocument* doc,
				wxView* view,
				wxDocParentFrame* parent,
				wxWindowID id,
				const wxString& title,
				const wxPoint& pos,
				const wxSize& size,
				long style)
		: wxDocChildFrame (doc, view, parent, id, title, pos, size, style)

{
	//this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	EnableCloseButton (false);
	SetMinSize (wxSize (150,150));

	m_statusBar1 = this->CreateStatusBar(1, wxST_SIZEGRIP, wxID_ANY);
	//wxBoxSizer* bSizer3;
	//bSizer3 = new wxBoxSizer( wxVERTICAL );

	//m_panel2 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	//bSizer3->Add( m_panel2, 1, wxEXPAND | wxALL, 0 );
	//textarea = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxALWAYS_SHOW_SB );
	//bSizer3->Add( textarea, 1, wxALL|wxEXPAND, 0 );
	//SetSizer( bSizer3 );
	Layout();
	SetLabel (wxT("Text Output"));
	//Centre(wxHORIZONTAL);
	
   		// Change text dialog window to the top-right screen -- Tsung Tai 12/10/18
   		// 20 in y-axis is for the menu bar size
	
   int			menuHeight = 0,
   				menuWidth = 0,
   				windowHeight = 0,
   				windowWidth = 0,
   				xPosition,
   				yPosition;
	
	GetMainFrame()->m_menubar1->GetSize (&menuWidth, &menuHeight);

   GetSize (&windowWidth, &windowHeight);
   #if defined multispec_wxlin
   	int		toolBarHeight, toolBarWidth, windowFrameWidth = 7;
	
		GetMainFrame()->m_toolBar1->GetSize (&toolBarWidth, &toolBarHeight);
	
		xPosition = wxDisplay().GetGeometry().GetWidth() - windowWidth - 2*windowFrameWidth;
		xPosition = MAX (2*windowFrameWidth, xPosition);
		GetMainFrame()->m_menubar1->GetSize (&menuWidth, &menuHeight);
		yPosition = 20 + menuHeight + toolBarHeight + 2;	// Allow 2 pixel space below toolbar
   #endif
   #if defined multispec_wxmac
		xPosition = wxDisplay().GetGeometry().GetWidth() - windowWidth - 3;
		xPosition = MAX (3, xPosition);
		yPosition = menuHeight + 3;
   #endif
	
   Move (xPosition, yPosition);
	
			// Set Accelerator Table

	wxAcceleratorEntry entries[32];
	entries[0].Set(wxACCEL_CTRL, (int) 'O', ID_IMAGE_OPEN);
	entries[1].Set(wxACCEL_CTRL, (int) ';', ID_FILE_OPEN_PROJECT);
	entries[2].Set(wxACCEL_CTRL, (int) 'W', ID_FILE_CLOSE_WINDOW);
	entries[3].Set(wxACCEL_CTRL, (int) 'S', ID_FILE_SAVE_AS);
	entries[4].Set(wxACCEL_CTRL, (int) 'P', ID_FILE_PRINT);
	entries[5].Set(wxACCEL_CTRL, (int) 'Q', wxID_EXIT);
	
	entries[6].Set(wxACCEL_CTRL, (int) 'Z', wxID_UNDO);
	entries[7].Set(wxACCEL_CTRL, (int) 'X', wxID_CUT);
	entries[8].Set(wxACCEL_CTRL, (int) 'C', wxID_COPY);
	entries[9].Set(wxACCEL_CTRL, (int) 'V', wxID_PASTE);
	entries[10].Set(wxACCEL_NORMAL, (int) WXK_DELETE, wxID_CLEAR);
	entries[11].Set(wxACCEL_CTRL, (int) 'A', ID_EDIT_SELECT_ALL);
	entries[12].Set(wxACCEL_CTRL, (int) ',', ID_EDIT_IMAGE_DESCRIPTION);
	entries[13].Set(wxACCEL_CTRL, (int) '`', ID_EDIT_IMAGE_MAP_PARAMETERS);
	
	entries[14].Set(wxACCEL_CTRL, (int) 'D', ID_PROC_DISPLAY_IMAGE);
	entries[15].Set(wxACCEL_CTRL, (int) 'H', ID_PROC_HISTOGRAM_IMAGE);
	entries[16].Set(wxACCEL_CTRL, (int) 'J', ID_PROC_LISTDATA);
	entries[17].Set(wxACCEL_CTRL, (int) 'R', ID_PROC_REFORMAT_CHANGE_IMAGE);
	entries[18].Set(wxACCEL_CTRL, (int) 'L', ID_PROC_CLUSTER);
	entries[19].Set(wxACCEL_CTRL, (int) 'T', ID_PROC_STATISTICS);
	entries[20].Set(wxACCEL_CTRL, (int) 'E', ID_PROC_ENHANCE_STATISTICS);
	entries[21].Set(wxACCEL_CTRL, (int) 'F', ID_PROC_FEATURE_EXTRACTION);
	entries[22].Set(wxACCEL_CTRL, (int) 'B', ID_PROC_FEATURE_SELECTION);
	entries[23].Set(wxACCEL_CTRL, (int) 'M', ID_PROC_CLASSIFY);
	entries[24].Set(wxACCEL_CTRL, (int) 'Y', ID_PROC_LISTRESULTS);
	entries[25].Set(wxACCEL_CTRL, (int) 'E', ID_PROC_ENHANCE_STATISTICS);
	
	entries[26].Set(wxACCEL_CTRL, (int) 'K', ID_PROC_UTIL_PRIN_COMP_ANALYSIS);
	entries[27].Set(wxACCEL_CTRL, (int) 'I', ID_PROC_UTIL_CREATE_STAT_IMAGE);
	entries[28].Set(wxACCEL_CTRL, (int) 'G', ID_PROC_UTIL_BIPLOTS_OF_DATA);
	entries[29].Set(wxACCEL_CTRL, (int) 'U', ID_PROC_UTIL_LIST_IMAGE_DESC);
	entries[30].Set(wxACCEL_CTRL, (int) '[', ID_PROC_UTIL_CHECK_COVARIANCES);
	entries[31].Set(wxACCEL_CTRL, (int) ']', ID_PROC_UTIL_CHECKTRANS_MATRIX);
	wxAcceleratorTable accel(32, entries);
	SetAcceleratorTable (accel);
	
}

BEGIN_EVENT_TABLE(CMTextFrame, wxDocChildFrame)

//EVT_SET_FOCUS(CMTextFrame::OnFocus)
//EVT_UPDATE_UI(ID_FILE_NEW_PROJECT, CMTextFrame::OnUpdateNewProject)
//EVT_UPDATE_UI(ID_FILE_LOAD_TRANS_MATRIX, CMTextFrame::OnUpdateLoadTransMatrix)
//EVT_UPDATE_UI(wxID_CLOSE, CMTextFrame::OnUpdateClose)
//EVT_UPDATE_UI(ID_FILE_CLOSE_WINDOW, CMTextFrame::OnUpdateFileCloseWindow)
//EVT_UPDATE_UI(ID_FILE_SAVE, CMTextFrame::OnUpdateSave)
//EVT_UPDATE_UI(ID_FILE_SAVE_AS, CMTextFrame::OnUpdateFileSaveAs)
EVT_UPDATE_UI(wxID_PRINT, CMTextFrame::OnUpdateFilePrint)
EVT_UPDATE_UI(wxID_PREVIEW, CMTextFrame::OnUpdateFilePrintPreview)
EVT_UPDATE_UI(wxID_PAGE_SETUP, CMTextFrame::OnUpdateFilePrintSetup)

//EVT_UPDATE_UI(wxID_COPY, CMTextFrame::OnUpdateEditCopy)
//EVT_UPDATE_UI(ID_EDIT_SELECT_ALL, CMTextFrame::OnUpdateSelectAll)
//EVT_UPDATE_UI(ID_EDIT_CLEAR_SELECT_RECTANGLE, CMTextFrame::OnUpdateEditClearSelectionRectangle)

EVT_UPDATE_UI(ID_WINDOW_SHOW_COORDINATE_VIEW, CMTextFrame::OnUpdateWindowShowCoordinateView)
EVT_UPDATE_UI(ID_WINDOW_NEW_SELECTION_GRAPH, CMTextFrame::OnUpdateWindowNewSelectionGraph)

EVT_UPDATE_UI(ID_MAGNIFICATION, CMTextFrame::OnUpdateMagnification)
EVT_UPDATE_UI(ID_ZOOM_IN, CMTextFrame::OnUpdateZoomIn)
EVT_UPDATE_UI(ID_ZOOM_OUT, CMTextFrame::OnUpdateZoomOut)
//EVT_UPDATE_UI(ID_INDICATOR_ZOOM, CMTextFrame::OnUpdateZoomIndicator)
EVT_UPDATE_UI(ID_OVERLAY, CMTextFrame::OnUpdateOverlay)

//EVT_MENU(ID_FILE_SAVE_AS, CMTextFrame::OnTextSaveAs)
END_EVENT_TABLE()


CMTextFrame::~CMTextFrame() {
}

/*
void CMTextFrame::OnFocus(wxFocusEvent& event) {
	CMainFrame * mainFrame = wxDynamicCast(wxTheApp->GetTopWindow(), CMainFrame);
	if (mainFrame != NULL)
		(mainFrame->m_closeWindowMenuItem)->Enable(false);
	wxBell();
         
}
*/
/*
void CMTextFrame::OnUpdateFileSaveAs(wxUpdateUIEvent& event) {

	UpdateFileOutputTextSaveAs (&event);
	
	if(((CMTextDoc *)GetDocument())->IsModified())
	  event.Enable(true);
	else
	  event.Enable(false);
}
*/

void CMTextFrame::OnUpdateFilePrint(CCmdUI& pCmdUI) {
    Boolean enableFlag = UpdateFileOutputTextPrint(&pCmdUI);

			// disable until implemented.
    pCmdUI.Enable(false);

}


void CMTextFrame::OnUpdateFilePrintPreview(CCmdUI& pCmdUI) {
			// Disable until implemented
    pCmdUI.Enable(false);

}


void CMTextFrame::OnUpdateFilePrintSetup(CCmdUI& pCmdUI) {
			// Set to false until set up.
    pCmdUI.Enable(false);

}


void CMTextFrame::OnUpdateWindowNewSelectionGraph(wxUpdateUIEvent& pCmdUI) {
   pCmdUI.Enable(false);

} // end "OnUpdateWindowNewSelectionGraph"


void CMTextFrame::OnUpdateWindowShowCoordinateView(wxUpdateUIEvent& pCmdUI) {
   pCmdUI.Enable(false);

} // end "OnUpdateWindowShowCoordinateView"


void CMTextFrame::OnUpdateMagnification (wxUpdateUIEvent& event)
{
	event.Enable(false);
		
	CMainFrame* pMainFrame = (CMainFrame*)wxGetApp().GetTopWindow();
	if (pMainFrame != NULL)
		pMainFrame->UpdateStatusBar (NULL, -1);
}


void CMTextFrame::OnUpdateZoomIn(wxUpdateUIEvent& event) {
      event.Enable(false);

}

void CMTextFrame::OnUpdateZoomOut(wxUpdateUIEvent& event) {
      event.Enable(false);
}


void CMTextFrame::OnUpdateOverlay(wxUpdateUIEvent& event) {
   event.Enable(false);

}
