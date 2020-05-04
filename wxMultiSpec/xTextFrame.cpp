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
//	File:						xTextFrame.cpp : class implementation file
//	Class Definition:		xTextFrame.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			04/22/2020
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMTextDoc class.
//
/* Template for debugging
	int numberChars = sprintf ((char*)gTextString3,
				" xTextFrame: (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "xMainFrame.h"
#include "xMultiSpec.h"
#include "xOpenFileDialog.h"
#include "xTextDoc.h"
#include "xTextFrame.h"

#include "wx/app.h"
#include "wx/display.h"
#include "wx/filedlg.h"


IMPLEMENT_DYNAMIC_CLASS (CMTextFrame, wxDocChildFrame)

BEGIN_EVENT_TABLE (CMTextFrame, wxDocChildFrame)
	//EVT_LEFT_DOWN (CMTextFrame::OnLeftDown)

	EVT_MENU (ID_EDIT_UNDO, CMTextFrame::OnEditUndo)
	EVT_MENU (wxID_COPY, CMTextFrame::OnEditCopy)
	EVT_MENU (wxID_CUT, CMTextFrame::OnEditCut)
	EVT_MENU (wxID_PASTE, CMTextFrame::OnEditPaste)
	
	EVT_UPDATE_UI (wxID_PAGE_SETUP, CMTextFrame::OnUpdateFilePrintSetup)
	EVT_UPDATE_UI (wxID_PREVIEW, CMTextFrame::OnUpdateFilePrintPreview)
	EVT_UPDATE_UI (wxID_PRINT, CMTextFrame::OnUpdateFilePrint)

	EVT_UPDATE_UI (ID_EDIT_UNDO, CMTextFrame::OnUpdateEditUndo)
	EVT_UPDATE_UI (wxID_CUT, CMTextFrame::OnUpdateEditCut)
	EVT_UPDATE_UI (wxID_COPY, CMTextFrame::OnUpdateEditCopy)
	EVT_UPDATE_UI (wxID_PASTE, CMTextFrame::OnUpdateEditPaste)
	EVT_UPDATE_UI (wxID_CLEAR, CMTextFrame::OnUpdateEditClear)

	EVT_UPDATE_UI (ID_WINDOW_NEW_SELECTION_GRAPH, CMTextFrame::OnUpdateWindowNewSelectionGraph)

	EVT_UPDATE_UI (ID_MAGNIFICATION, CMTextFrame::OnUpdateMagnification)
	EVT_UPDATE_UI (ID_OVERLAY, CMTextFrame::OnUpdateOverlay)
	EVT_UPDATE_UI (ID_ZOOM_IN, CMTextFrame::OnUpdateZoomIn)
	EVT_UPDATE_UI (ID_ZOOM_OUT, CMTextFrame::OnUpdateZoomOut)
END_EVENT_TABLE ()



CMTextFrame::CMTextFrame (void)
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
	EnableCloseButton (false);
	SetMinSize (wxSize (150, 150));

	m_statusBar1 = this->CreateStatusBar (1, wxST_SIZEGRIP, wxID_ANY);

	Layout ();
	SetLabel (wxT("Text Output"));
	
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
	
		xPosition = wxDisplay().GetGeometry().GetWidth() -
																windowWidth - 2*windowFrameWidth;
		xPosition = MAX (2*windowFrameWidth, xPosition);
		GetMainFrame()->m_menubar1->GetSize (&menuWidth, &menuHeight);
	
				// Allow 2 pixel space below toolbar
		yPosition = 20 + menuHeight + toolBarHeight + 2;
   #endif
   #if defined multispec_wxmac
		xPosition = wxDisplay().GetGeometry().GetWidth() - windowWidth - 3;
		xPosition = MAX (3, xPosition);
		yPosition = menuHeight + 3;
   #endif
	
   Move (xPosition, yPosition);
	
			// Set Accelerator Table

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
	
}	// end "CMTextFrame"



CMTextFrame::~CMTextFrame (void)

{

}	// end "~CMTextFrame"



void CMTextFrame::OnEditCopy (
				wxCommandEvent& 					event)

{
	gOutputViewCPtr->m_textsw->Copy ();
	//event.Skip ();
	
}	// end "OnEditCopy"



void CMTextFrame::OnEditCut (
				wxCommandEvent& 					event)

{
	gOutputViewCPtr->m_textsw->Cut ();
	//event.Skip ();
	
}	// end "OnEditCut"



void CMTextFrame::OnEditPaste (
				wxCommandEvent& 					event)

{
	gOutputViewCPtr->m_textsw->Paste ();
	//event.Skip ();
	
}	// end "OnEditPaste"



void CMTextFrame::OnEditUndo (
				wxCommandEvent& 					event)

{
	gOutputViewCPtr->m_textsw->Undo ();
	//event.Skip ();
	
}	// end "OnEditUndo"


/*
void CMTextFrame::OnLeftDown (
				wxMouseEvent&						event)

{
	int numberChars = sprintf ((char*)gTextString3,
				" xTextFrame:OnLeftDown (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);

	//event.Skip ();
	
}	// end "OnLeftDown"
*/


void CMTextFrame::OnUpdateEditClear (
				wxUpdateUIEvent&  				pCmdUI)

{
    pCmdUI.Enable (true);
	
}	// end "OnUpdateEditClear"



void CMTextFrame::OnUpdateEditCopy (
				wxUpdateUIEvent&  				pCmdUI)

{
    pCmdUI.Enable (true);
	
}	// end "OnUpdateEditCopy"



void CMTextFrame::OnUpdateEditCut (
				wxUpdateUIEvent&  				pCmdUI)

{
    pCmdUI.Enable (true);
	
}	// end "OnUpdateEditCut"



void CMTextFrame::OnUpdateEditPaste (
				wxUpdateUIEvent&  				pCmdUI)

{
    pCmdUI.Enable (true);
	
}	// end "OnUpdateEditPaste"



void CMTextFrame::OnUpdateEditUndo (
				wxUpdateUIEvent&  				pCmdUI)

{
    pCmdUI.Enable (true);
	
}	// end "OnUpdateEditUndo"



void CMTextFrame::OnUpdateFilePrint (
				CCmdUI& 								pCmdUI)

{
			// Disable until implemented.
	
    //Boolean enableFlag = UpdateFileOutputTextPrint (&pCmdUI);
    pCmdUI.Enable (false);

}	// end "OnUpdateFilePrint"



void CMTextFrame::OnUpdateFilePrintPreview (
				CCmdUI& 								pCmdUI)

{
			// Disable until implemented
	
    pCmdUI.Enable (false);

}	// end "OnUpdateFilePrintPreview"



void CMTextFrame::OnUpdateFilePrintSetup (
				CCmdUI& 								pCmdUI)

{
			// Set to false until set up.
	
    pCmdUI.Enable (false);

}	// end "OnUpdateFilePrintSetup"



void CMTextFrame::OnUpdateMagnification (
				wxUpdateUIEvent& 					event)

{
	event.Enable (false);
		
	CMainFrame* pMainFrame = (CMainFrame*)wxGetApp().GetTopWindow ();
	if (pMainFrame != NULL)
		pMainFrame->UpdateStatusBar (NULL, -1);
	
}	// end "OnUpdateMagnification"


void CMTextFrame::OnUpdateWindowNewSelectionGraph (
				wxUpdateUIEvent& 					pCmdUI)

{
   pCmdUI.Enable (UpdateWindowSelectionGraph ());

}	// end "OnUpdateWindowNewSelectionGraph"



void CMTextFrame::OnUpdateZoomIn (
				wxUpdateUIEvent& 					event)

{
      event.Enable (false);

}	// end "OnUpdateZoomIn"



void CMTextFrame::OnUpdateZoomOut (
				wxUpdateUIEvent& 					event)

{
      event.Enable (false);
	
}	// end "OnUpdateZoomOut"


void CMTextFrame::OnUpdateOverlay (
				wxUpdateUIEvent& 					event)

{
   event.Enable (false);

}	// end "OnUpdateOverlay"
