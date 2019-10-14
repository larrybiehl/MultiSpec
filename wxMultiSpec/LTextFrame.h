//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2019)
//						(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LTextFrame.h
//	Implementation:		LTextFrame.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file is the definition for the CMTextFrame class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 04/12/2019
//
//------------------------------------------------------------------------------------
//
#ifndef _LTEXTFRAME
	#define _LTEXTFRAME

#include "wx/textctrl.h"
#include "wx/stattext.h"
#include "wx/docview.h"
#include "wx/string.h"
#include "wx/bitmap.h"
#include "wx/image.h"
#include "wx/icon.h"
#include "wx/menu.h"
#include "wx/gdicmn.h"
#include "wx/font.h"
#include "wx/colour.h"
#include "wx/settings.h"
#include "wx/toolbar.h"
#include "wx/statusbr.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/frame.h"
#include "wx/button.h"
#include "wx/dialog.h"

class CMTextFrame : public wxDocChildFrame
	{
	DECLARE_DYNAMIC_CLASS (CMTextFrame)

	private:
		DECLARE_EVENT_TABLE ()

		wxMenuBar* m_menubar1;
		wxMenu* filemenu;
		wxMenu* editmenu;
		wxMenu* m_menu1;
		wxMenu* viewmenu;
		wxMenu* projectmenu;
		wxMenu* processormenu;
		wxMenu* m_menu2;
		wxMenu* m_menu3;
		wxMenu* optionsmenu;
		wxMenu* m_menu4;
		wxMenu* m_menu5;
		wxMenu* windowmenu;
		wxMenu* helpmenu;
		wxStatusBar* m_statusBar1;
		wxPanel* m_panel2;
		#ifndef multispec_wxmac
			enum 	{
					ID_FILE_SAVE,
					ID_FILE_SAVE_AS
					};
		#endif

	protected:
		void OnUpdateEditUndo(wxUpdateUIEvent& pCmdUI);
		void OnUpdateEditCut(wxUpdateUIEvent& pCmdUI);
		void OnUpdateEditCopy(wxUpdateUIEvent& pCmdUI);
		void OnUpdateEditPaste(wxUpdateUIEvent& pCmdUI);
		void OnUpdateEditClear(wxUpdateUIEvent& pCmdUI);
	
		void OnEditCopy (wxCommandEvent& event);
		void OnEditCut (wxCommandEvent& event);
		void OnEditPaste (wxCommandEvent& event);
		void OnEditUndo (wxCommandEvent& event);

		 bool m_frameMaximized;
		 wxSize m_frameSizeSaved;
		 wxPoint m_framePosSaved;

	public:
		CMTextFrame ();
		CMTextFrame (
				wxDocument* 			doc,
				wxView* 					view,
				wxDocParentFrame* 	parent,
				wxWindowID 				id,
				const wxString& 		title,
				const wxPoint& 		pos = wxDefaultPosition,
				const wxSize& 			size = wxDefaultSize,
				long 						style = wxDEFAULT_FRAME_STYLE);
	
		~CMTextFrame ();
	
		void OnFocus (wxFocusEvent& event);
		void OnMaximizeWindow (wxMaximizeEvent& event);
		void OnUpdateEditClearSelectionRectangle (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFileCloseWindow (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFilePrint (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFilePrintPreview (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFilePrintSetup (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFileSave (wxUpdateUIEvent& event);
	
		void OnUpdateMagnification (wxUpdateUIEvent& pCmdUI);
		void OnUpdateNewProject (wxUpdateUIEvent& event);
		void OnUpdateOverlay (wxUpdateUIEvent& event);
		void OnUpdateSelectAll (wxUpdateUIEvent& pCmdUI);

		void OnUpdateWindowNewSelectionGraph (wxUpdateUIEvent& pCmdUI);
		void OnUpdateWindowShowCoordinateView (wxUpdateUIEvent& pCmdUI);
		void OnUpdateZoomIn (wxUpdateUIEvent& pCmdUI);
		void OnUpdateZoomOut (wxUpdateUIEvent& pCmdUI);

		void OnTextSaveAs (wxCommandEvent& event);

	};
#endif
