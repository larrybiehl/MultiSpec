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
//	Revised By:				Larry L. Biehl			Date: 04/21/2020
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
				
		void OnLeftDown (
				wxMouseEvent&						event);
	
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
