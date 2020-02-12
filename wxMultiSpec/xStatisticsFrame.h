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
//	File:						xStatisticsFrame.h
//	Implementation:		xStatisticsFrame.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file is the definition for the CMStatisticsFrame class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 01/03/2020
//
//------------------------------------------------------------------------------------

#pragma once

#include "wx/docview.h"
#include "wx/tooltip.h"
#include "xMultiSpec.h"


class CMStatisticsFrame : public wxDocChildFrame
{
		 DECLARE_DYNAMIC_CLASS (CMStatisticsFrame)

	public:
		 CMStatisticsFrame ();
	
		 CMStatisticsFrame (
		 		wxDocument* 						doc,
		 		wxView* 								view,
		 		wxDocParentFrame* 				parent,
		 		wxWindowID 							id,
				const wxString& 					title = wxT("Project"),
				const wxPoint& 					pos = wxDefaultPosition,
				const wxSize& 						size = wxDefaultSize,
				long 									style = wxDEFAULT_FRAME_STYLE);

		 ~CMStatisticsFrame ();
	
		void ActivateStatisticsWindowItems (
				SInt16 								statsWindowMode);
	
		void UpdateStatsTypeCombo (
				SInt16 								statsWindowMode);
	
		void UpdateHistogramStatsCombo (void);
	
		void UpdateListStatsCombo (void);

		#ifndef multispec_wxmac
			enum {
				  ID_FILE_SAVE,
				  ID_FILE_SAVE_AS
				  };
		#endif

	protected:
				// Generated message map functions
		void OnAddToList (
				wxCommandEvent& 					event);
	
		virtual void OnCharHook (
				wxKeyEvent& 						event);
	
		void OnClass (
				wxCommandEvent& 					event);
	
		void OnClose (
				wxCloseEvent& 						event);
	
		void OnDblclkListBox (
				wxCommandEvent& 					event);
	
		void OnEditClearSelectRectangle (
				wxCommandEvent& 					event);
	
		void OnEditCut (
				wxCommandEvent& 					event);
	
		void OnEditName (
				wxCommandEvent& 					event);
	
		void OnEditPaste (
				wxCommandEvent& 					event);
	
		void OnEditSelectAll (
				wxCommandEvent& 					event);
	
		void OnEditSelectionRectangle (
				wxCommandEvent& 					event);
	
		void OnEditUndo (
				wxCommandEvent& 					event);
	
		void OnField (
				wxCommandEvent& 					event);
	
		void OnFileProjectClose (
				wxCommandEvent& 					event);
	
		void OnHistogram (
				wxCommandEvent& 					event);
	
		void OnHistogramStatsComboCloseUp (
				wxCommandEvent& 					event);
	
		void OnHistogramStatsComboDropDown (
				wxCommandEvent& 					event);
	
		void OnHistogramStatsComboSelendok (
				wxCommandEvent& 					event);
	
		void OnList ();
	
		void OnListStatsComboCloseUp (
				wxCommandEvent& 					event);
	
		void OnListStatsComboDropDown (
				wxCommandEvent& 					event);
	
		void OnListStatsComboSelendok (
				wxCommandEvent& 					event);
	
		void OnPolygon (
				wxCommandEvent& 					event);
	
		void OnProject (
				wxCommandEvent& 					event);
	
		void OnSelchangeListBox (
				wxCommandEvent& 					event);
	
		void OnSelect (
				wxCommandEvent& 					event);
	
		void OnSelendokClassList (
				wxCommandEvent& 					event);
	
		void OnStatsTypeComboCloseUp (
				wxCommandEvent& 					event);
	
		void OnStatsTypeComboDropDown (
				wxCommandEvent& 					event);
	
		void OnStatsTypeComboSelendok (
				wxCommandEvent& 					event);
	
		void OnUpdate (
				wxCommandEvent& 					event);
	
		void OnUpdateEditCut (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateEditPaste (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateEditUndo (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateEditClearSelectRectangle (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateEditSelectionRectangle (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateEditSelectAll (
				wxUpdateUIEvent& 					pCmdUI);
	
		void OnUpdateFileProjectClose (
				wxUpdateUIEvent& 					pCmdUI);
		DECLARE_EVENT_TABLE ()

		void CreateControls2 ();


		wxButton								*m_button56,
												*m_button60,
												*m_button61,
												*m_button62,
												*m_button63,
												*m_button64,
												*m_button65;
	
		wxCheckBox							*m_checkBox53;
	
		wxChoice								*m_classListCtrl;
		#if defined multispec_wxmac
			wxChoice								*m_histogramCtrl,
													*m_listCtrl,
													*m_statsCtrl;
		#endif
	
		#if defined multispec_wxlin
			wxComboBox 							*m_histogramCtrl,
													*m_listCtrl,
													*m_statsCtrl;
		#endif
	
		wxListBox							*m_listBox;
	
		wxStaticText						*m_staticText223,
												*m_staticText224,
												*m_staticText225,
												*m_staticText226,
												*m_staticText227,
												*m_staticText228,
												*m_staticText229,
												*m_staticText230,
												*m_staticText231,
												*m_staticText259,
												*m_staticText260,
												*m_staticText261,
												*m_staticText262,
												*m_staticText263,
												*m_staticText264;

		int 									m_classList,
												m_histogramStatsCode,
												m_listStatsCode,
												m_statsTypeCode;
	
		bool 									m_polygonFlag;
	
		Boolean 								m_initializedFlag,
												m_optionKeyFlag;
	
};	// "class CMStatisticsFrame"
