//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2020)
//						(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xTextFrame.h
//	Implementation:		xTextFrame.cpp
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

#pragma once

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

	public:
		CMTextFrame ();
	
		CMTextFrame (
				wxDocument* 						doc,
				wxView* 								view,
				wxDocParentFrame* 				parent,
				wxWindowID 							id,
				const wxString& 					title,
				const wxPoint& 					pos = wxDefaultPosition,
				const wxSize& 						size = wxDefaultSize,
				long 									style = wxDEFAULT_FRAME_STYLE);
	
		~CMTextFrame ();

	
	protected:
	
		void OnEditCopy (
				wxCommandEvent& 					event);
	
		void OnEditCut (
				wxCommandEvent& 					event);
	
		void OnEditPaste (
				wxCommandEvent& 					event);
	
		void OnEditUndo (
				wxCommandEvent& 					event);
	
		void OnUpdateEditClear (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateEditCopy (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateEditCut (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateEditPaste (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateEditUndo (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateFilePrint (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateFilePrintPreview (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateFilePrintSetup (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateMagnification (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateOverlay (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateWindowNewSelectionGraph (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateZoomIn (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateZoomOut (
				wxUpdateUIEvent& 					pCmdUI);
		DECLARE_EVENT_TABLE ()

		#ifndef multispec_wxmac
			enum 	{
					ID_FILE_SAVE,
					ID_FILE_SAVE_AS
					};
		#endif
	
		wxStatusBar							*m_statusBar1;

};	// end "class CMTextFrame"
