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
//	File:						xGraphFrame.h
//	Implementation:		xGraphFrame.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file is the definition for the CMGraphCanvas and
//								CMGraphFrame classes
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 02/28/2020
//
//------------------------------------------------------------------------------------

#pragma once

#include "xMultiSpec.h"
#include "wx/docview.h"
#include "wx/splitter.h"
#include "wx/treelist.h"


class CMGraphCanvas : public wxPanel
{
	DECLARE_DYNAMIC_CLASS (CMGraphCanvas);
	
	public:
		 CMGraphCanvas (
		 		wxWindow* 							parent);
	
		 void paintNow ();
	
		 CMGraphView*    					m_graphViewCPtr;
	
	
	private:
		 void paintEvent (
		 		wxPaintEvent& 						evt);
		 DECLARE_EVENT_TABLE ()
	
		 void render (
		 		wxDC& 								dc);

};	// end "class CMGraphCanvas"



//------------------------------------------------------------------------------------

class CMGraphFrame : public wxDocChildFrame 
{
	DECLARE_DYNAMIC_CLASS (CMGraphFrame);
	
	public:
		CMGraphFrame (); // protected constructor used by dynamic creation
	
		CMGraphFrame (
				wxDocument* 						doc,
				wxView* 								view = NULL,
				wxDocParentFrame* 				parent = NULL,
				wxWindowID 							id = NULL,
				const wxString& 					title = wxT("Selection Window"),
				const wxPoint& 					pos = wxDefaultPosition,
				const wxSize& 						size = wxDefaultSize,
				long style = 						wxDEFAULT_FRAME_STYLE);

		~CMGraphFrame ();

		void CreateSelectVector (
				int 									vectorSize);
	
		bool OnCreateClient ();
	
		void OnData (
				wxCommandEvent& 					WXUNUSED (event));
	
		void OnFeature (
				wxCommandEvent& 					WXUNUSED (event));
	
		void PrintKey (
				wxKeyEvent& 						event);
	
		void SetGraphViewCPtr (
				CMGraphView*  						graphViewCPtr);
	
		void SetUpGraphXLabelPopUpMenu (
					Handle							windowInfoHandle);
	
		void UpdateExpandFlag ();

		void UpdateSplitterWindowLayout ();
	
	
	 	#ifndef multispec_wxmac
			enum {
				ID_FILE_SAVE,
				ID_FILE_SAVE_AS
		 		};
		#endif
	
		CMGraphCanvas* 					m_panel2;
		CMGraphView*    					m_graphViewCPtr;
	
		wxCheckBox	 						*m_checkBoxData,
												*m_checkBoxFeature;
	
		wxChoice* 							m_comboXlabel;
		wxListView*							m_listCtrl2;
	
		wxPanel	 							*m_panel3,
												*m_panel4;
	
		wxSplitterWindow	 				*m_splitter1,
												*m_splitter2,
												*m_splitter3;
	
		wxTreeListCtrl* 					m_listCtrl1;
	
		int									m_minFrameYSize;
	
		bool 									m_initGraphUpdate;
	
	
	private:
		void DoNextChannel (
				wxMouseEvent& 						event);
	
		void DoPreviousChannel (
				wxMouseEvent& 						event);
	
		void DoSelectVectors (
				wxMouseEvent& 						event);
	
		void DoShowBinWidth (
				wxMouseEvent& 						event);
	
		void DoShowOverlay (
				wxMouseEvent& 						event);
	
		void DoShowVectors (
				wxMouseEvent& 						event);

		void OnBinWidth (
				wxCommandEvent& 					event);
	
		void OnChangeXAxis (
				wxCommandEvent& 					WXUNUSED (event));
	
		void OnFilePrint ();
	
		void OnGraphWindow (
				wxCommandEvent& 					WXUNUSED (event));
	
		void OnMaximizeWindow (
				wxMaximizeEvent& 					event);
	
		void OnNextListData (
				wxCommandEvent& 					event);
	
		void OnOverlay (
				wxCommandEvent& 					event);
	
		void OnPaint (
				wxPaintEvent& 						event);
	
		void OnSelectVector (
				wxCommandEvent& 					event);
	
		void OnShowBinWidth (
				wxCommandEvent& 					event);
	
		void OnShowSelectVector (
				wxCommandEvent& 					event);
	
		void OnSize (
				wxSizeEvent& 						event);
	
		void OnUpdateFileGraphClose (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateFilePrint (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateFilePrintPreview (
				wxUpdateUIEvent& 					pCmdUI);
		
		void OnUpdateFilePrintSetup (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateFileSaveAs (
				wxUpdateUIEvent& 					event);
	
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

		void CreateBinWidthMenu ();
	
		void CreateControls ();
	
		void CreateControlsBiPlot ();
	
		void CreateControlsListData ();
	
		void CreateHistogramControls ();
	
		void CreateHistogramDensityMenu ();
	
		wxChoice* CreateXAxisComboControl (
				wxWindow*							parent);
	
		void DoNextOrPreviousChannelEvent (
				wxMouseEvent& 						event,
				SInt16								controlIdentifier);
	
		void Render (
				wxDC& 								dc);

		void SetOverlayMenuCheck (
				int 									selectedMenuID);
	
		Boolean UpdateGraphChannels ();
	
		void UpdateWavelengthComboUnit ();
	
	
		wxPoint 								m_framePosSaved;
	
		wxSize 								m_frameSizeSaved;
	
		wxBitmapButton						*m_buttonBinWidth,
												*m_buttonNext,
												*m_buttonOverlay,
												*m_buttonPrevious,
												*m_buttonVectors;
	
		wxButton								*m_button69;
	
		wxCheckBox							*m_checkBoxGraph;
	
		wxMenu								*m_binWidthMenu,
												*m_graphOverlayMenu,
												*m_selectVectorMenu;
	
		wxPanel								*m_panel1;
	
		wxStatusBar							*m_statusBar1;
	
		wxToolBar							*m_toolBar1;
	
		int									actEntryNext,
												actEntryPrev,
												activateFlag,
												binWidthCheckID,
												m_height,
												m_overlayCheckID,
												m_width,
												selectVectorCheckID;
	
		SInt32 								m_selectionVector;

		bool									m_dataListShowFlag,
												m_featureListShowFlag,
			 									m_frameMaximized,
			                				m_optionBinWidthFlag;
	
};	// end "class CMGraphFrame"
