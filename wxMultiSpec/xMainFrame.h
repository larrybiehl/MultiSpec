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
//	File:						xMainFrame.h
//	Implementation:		xMainFrame.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMDisplaySpecsDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 04/12/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "xOpenFileDialog.h"

#include "wx/string.h"
#include "wx/gdicmn.h"
#include "wx/settings.h"
#include "wx/statusbr.h"
#include "wx/frame.h"
#include "wx/mdi.h"
#include "wx/docview.h"
#include "wx/toolbar.h"
#include "wx/bitmap.h"
#include "wx/image.h"
#include "wx/menu.h"

#include "SDefines.h"

class FileUploadProcess;

#define CMMainFrameTitle _("MultiSpec")


class CMainFrame : public wxDocParentFrame
{
	DECLARE_CLASS (CMainFrame)
	public:
		CMainFrame (
				wxDocManager*						manager,
				wxDocParentFrame*					frame,
				const wxString& 					title = wxT("MultiSpec"),
				const wxPoint& 					pos = wxDefaultPosition,
				const wxSize& 						size = wxSize (800, 600),
				long 									type=wxDEFAULT_FRAME_STYLE);

		~CMainFrame ();
	
		Boolean GetCancelOperationEventFlag (void);
	
		time_t GetNextControlTime ()
				{return (m_nextControlTime);}
	
		SInt16 GetZoomCode (void);
	
		void OnAsyncTermination (
				FileUploadProcess*				process);
	
		void OnToolBarShowOverlay (
				wxCommandEvent& 					event);
	
		void SetNextControlTime (
				UInt32 								offset);
	
		void SetToolParametersFlag (
				Boolean 								TOOL_PARAMETER_file_flag);
	
		void SetZoomCode (
				SInt16 								zoomCode);
	
		void UpdateStatusBar (
				CMImageFrame* 						imageFrame,
				double 								magnification);
	
		wxMenuBar*							m_menubar1;
		wxMenuItem* 						m_closeWindowMenuItem;
		wxToolBar*							m_toolBar1;

	protected:
		void OnChar (wxKeyEvent& event);
		void OnClose ();
		void OnIdle (wxIdleEvent& event);
		void OnZoomInMouseDown (wxMouseEvent& event);
	
			// In order of location on the main menu
				// MultiSpec Application Menu
		void OnAbout (wxCommandEvent& event);
	
				// File Menu
		void OnFileNewProject (wxCommandEvent& event);
						// Now in CMultiSpecApp (xMultiSpec.cpp)
		//void OnFileOpen (wxCommandEvent& event);
		void OnFileOpenProject (wxCommandEvent& event);
		void OnFileOpenProjectImage (wxCommandEvent& event);
		void OnFileOpenThematic (wxCommandEvent& event);
		void OnFileLoadTransMatrix (wxCommandEvent& event);
		void OnFileCloseWindow (wxCommandEvent& event);
		void OnFileCloseProject (wxCommandEvent& event);
		void OnFileSave (wxCommandEvent& event);
		void OnFileSaveAs (wxCommandEvent& event);
		void OnFileSaveProject (wxCommandEvent& event);
		void OnFileSaveProjectAs (wxCommandEvent& event);
		void OnFileExportFile (wxCommandEvent& event);

				// Edit Menu
		void OnEditUndo (wxCommandEvent& event);
		void OnEditCut (wxCommandEvent& event);
		void OnEditCopy (wxCommandEvent& event);
		void OnEditPaste (wxCommandEvent& event);
		void OnEditSelectAll (wxCommandEvent& event);
		void OnEditSelectionRectangle (wxCommandEvent& event);
		void OnEditClearSelectionRectangle (wxCommandEvent& event);
		void OnEditClearTransMatrix (wxCommandEvent& event);
		void OnEditImageDescription (wxCommandEvent& event);
		void OnEditImageMapParameters (wxCommandEvent& event);
		void OnEditClearAllImageOverlays (wxCommandEvent& event);
		void OnEditClearAllVectorOverlays (wxCommandEvent& event);
		void OnEditClearSelectedOverlay (wxCommandEvent& event);

				// View Menu
		void OnViewCoordinatesBar (wxCommandEvent& event);
	
				// Project Menu
		void OnProjUseOriginalStats (wxCommandEvent& event);
		void OnProjUseLeave1OutStats (wxCommandEvent& event);
		void OnProjUseEnhancedStats (wxCommandEvent& event);
		void OnProjClearStats (wxCommandEvent& event);
		void OnProjChangeBaseImageFile (wxCommandEvent& event);
		void OnProjAddAsAssociatedImage (wxCommandEvent& event);
	
				// Processor Menu
		void OnProcDisplayImage (wxCommandEvent& event);
		void OnProcHistogramImage (wxCommandEvent& event);
		void OnProcListData (wxCommandEvent& event);
		//void OnProcReformat (wxCommandEvent& event);
		void OnProcCluster (wxCommandEvent& event);
		void OnProcStatistics (wxCommandEvent& event);
		void OnProcEnhanceStatistics (wxCommandEvent& event);
		void OnProcFeatureExtraction (wxCommandEvent& event);
		void OnProcFeatureSelection (wxCommandEvent& event);
		void OnProcClassify (wxCommandEvent& event);
		void OnProcListResults (wxCommandEvent& event);
	
				// Processor->Reformat Menu Items
		void OnProcReformatChangeImage (wxCommandEvent& event);
		void OnProcReformatConvertProject (wxCommandEvent& event);
		void OnProcReformatConvertShape (wxCommandEvent& event);
		void OnProcReformatModifyChannel (wxCommandEvent& event);
		void OnProcReformatMosaicImages (wxCommandEvent& event);
		void OnProcReformatRecodeThematic (wxCommandEvent& event);
		void OnProcReformatRectifyImage (wxCommandEvent& event);
		void OnProcReformatConvertEnvi (wxCommandEvent& event);
	
				// Processor->Utility Menu Items
		void OnProcUtilPrinCompAnalysis (wxCommandEvent& event);
		void OnProcUtilCreateStatImage (wxCommandEvent& event);
		void OnProcUtilBiplotsOfData (wxCommandEvent& event);
		void OnProcUtilListImageDesc (wxCommandEvent& event);
		void OnProcUtilCheckCovariances (wxCommandEvent& event);
		void OnProcUtilCheckTransMatrix (wxCommandEvent& event);

				// Options Menu Items
		void OnOptionsAreaUnitsSqKilometers (wxCommandEvent& event);
		void OnOptionsAreaUnitsHectares (wxCommandEvent& event);
		void OnOptionsAreaUnitsSqMeters (wxCommandEvent& event);
		void OnOptionsAreaUnitsSqCentimeters (wxCommandEvent& event);
		void OnOptionsAreaUnitsSqMillimeters (wxCommandEvent& event);
		void OnOptionsAreaUnitsSqMicrometers (wxCommandEvent& event);
		void OnOptionsAreaUnitsSqMiles (wxCommandEvent& event);
		void OnOptionsAreaUnitsAcres (wxCommandEvent& event);
		void OnOptionsAreaUnitsSqYards (wxCommandEvent& event);
		void OnOptionsAreaUnitsSqFeet (wxCommandEvent& event);
		void OnOptionsAreaUnitsSqInches (wxCommandEvent& event);
      void OnOptionsSwitchcrosscursor (wxCommandEvent& event);
		void OnOptionsShowHideToolTip (wxCommandEvent& event);
	
				// Windows Menu Items
		void OnWindowShowCoordinateView (wxCommandEvent& event);
		void OnWindowSelectionNewGraph (wxCommandEvent& event);
		void OnWindowOutputWindowSelection (wxCommandEvent& event);
		void OnWindowProjectWindowSelection (wxCommandEvent& event);
		void OnWindowIorGWindowSelection (wxCommandEvent& event);
	
			// Image Window Tool bar items
		void OnToolBarOverlaySelection (wxCommandEvent& event);

		// In order of location on the main menu
				// File Menu Items
		void OnUpdateFileNewProject (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFileOpen (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFileOpenProject (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFileOpenProjectImage (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFileOpenThematic (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFileLoadTransMatrix (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFileCloseWindow (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFileSave (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFileSaveAs (wxUpdateUIEvent& event);
		void OnUpdateFileSaveProject (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFileSaveProjectAs (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFilePrint (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFilePrintPreview (wxUpdateUIEvent& pCmdUI);
		void OnUpdateFilePrintSetup (wxUpdateUIEvent& pCmdUI);
	
				// Edit Menu Items
		void OnUpdateEditUndo (wxUpdateUIEvent& pCmdUI);
		void OnUpdateEditCut (wxUpdateUIEvent& pCmdUI);
		void OnUpdateEditCopy (wxUpdateUIEvent& pCmdUI);
		void OnUpdateEditPaste (wxUpdateUIEvent& pCmdUI);
		void OnUpdateEditClear (wxUpdateUIEvent& pCmdUI);
		void OnUpdateEditSelectAll (wxUpdateUIEvent& event);
		void OnUpdateEditSelectionRectangle (wxUpdateUIEvent& pCmdUI);
		void OnUpdateEditClearSelectionRectangle (wxUpdateUIEvent& pCmdUI);
		void OnUpdateEditClearTransMatrix (wxUpdateUIEvent& pCmdUI);
		void OnUpdateEditImageDescription (wxUpdateUIEvent& pCmdUI);
		void OnUpdateEditImageMapParameters (wxUpdateUIEvent& pCmdUI);
		void OnUpdateEditClearAllOverlays (wxUpdateUIEvent& pCmdUI);
		void OnUpdateEditClearAllVectorOverlays (wxUpdateUIEvent& pCmdUI);
	
				// View Menu Items
		void OnUpdateViewCoordinatesBar (wxUpdateUIEvent& pCmdUI);
		void OnUpdateViewToolBar (wxUpdateUIEvent& pCmdUI);
		void OnUpdateViewStatusBar (wxUpdateUIEvent& pCmdUI);
	
				// Project Menu Items
		void OnUpdateProjectUseOriginalStats (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProjectUseLeave1OutStatistics (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProjectUseEnhancedStats (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProjectMixofstatisticsused (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProjectClearStats (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProjectChangeBaseImageFile (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProjectAddAsAssociatedImage (wxUpdateUIEvent& pCmdUI);
	
				// Processor Menu Items
		void OnUpdateProcDisplayImage (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcHistogramImage (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcListData (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcReformat (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcCluster (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcStatistics (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcEnhanceStatistics (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcFeatureExtraction (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcFeatureSelection (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcClassify (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcListResults (wxUpdateUIEvent& pCmdUI);
	
				// Processor->Reformat Menu Items
		void OnUpdateProcReformatChangeImage (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcReformatConvertProject (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcReformatModifyChannel (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcReformatMosaicImages (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcReformatRecodeThematic (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcReformatRectifyImage (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcReformatConvertShape (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcReformatConvertEnvi (wxUpdateUIEvent& pCmdUI);
	
				// Processor->Utility Menu Items
		void OnUpdateProcUtilBiplotsOfData (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcUtilCheckCovariances (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcUtilCheckTransMatrix (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcUtilCreateStatImage (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcUtilListImageDesc (wxUpdateUIEvent& pCmdUI);
		void OnUpdateProcUtilPrincipalComponentAnalysis (wxUpdateUIEvent& pCmdUI);
	
				// Options Menu Items
		void OnUpdateOptionsMemoryStatus (wxUpdateUIEvent& pCmdUI);
		void OnUpdateOptionsAreaUnitsSqKilometers (wxUpdateUIEvent& pCmdUI);
		void OnUpdateOptionsAreaUnitsHectares (wxUpdateUIEvent& pCmdUI);
		void OnUpdateOptionsAreaUnitsSqMeters (wxUpdateUIEvent& pCmdUI);
		void OnUpdateOptionsAreaUnitsSqCentimeters (wxUpdateUIEvent& pCmdUI);
		void OnUpdateOptionsAreaUnitsSqMillimeters (wxUpdateUIEvent& pCmdUI);
		void OnUpdateOptionsAreaUnitsSqMicrometers (wxUpdateUIEvent& pCmdUI);
		void OnUpdateOptionsAreaUnitsSqMiles (wxUpdateUIEvent& pCmdUI);
		void OnUpdateOptionsAreaUnitsAcres (wxUpdateUIEvent& pCmdUI);
		void OnUpdateOptionsAreaUnitsSqYards (wxUpdateUIEvent& pCmdUI);
		void OnUpdateOptionsAreaUnitsSqFeet (wxUpdateUIEvent& pCmdUI);
		void OnUpdateOptionsAreaUnitsSqInches (wxUpdateUIEvent& pCmdUI);
		void OnUpdateOptionsSwitchcrosscursor (wxUpdateUIEvent& pCmdUI);
		void OnUpdateOptionsShowToolTip (wxUpdateUIEvent& pCmdUI);
	
				// Window Menu Items
		void OnUpdateNewSelectGraph (wxUpdateUIEvent& pCmdUI);
		void OnUpdateWindowNewSelectionGraph (wxUpdateUIEvent& pCmdUI);
		void OnUpdateWindowTextOutput (wxUpdateUIEvent& pCmdUI);
		void OnUpdateWindowProject (wxUpdateUIEvent& pCmdUI);
	
			// Image Window Tool bar items
		void OnUpdateToolBarMagnification (wxUpdateUIEvent& event);
		void OnUpdateToolBarZoomIn (wxUpdateUIEvent& event);
		void OnUpdateToolBarZoomOut (wxUpdateUIEvent& event);
		void OnUpdateToolBarZoomInfo (wxUpdateUIEvent& event);
		void OnUpdateZoomIndicator (wxUpdateUIEvent& pCmdUI);
		void OnUpdateToolBarOverlay (wxUpdateUIEvent& event);
		DECLARE_EVENT_TABLE ()
	
		void 	DoZoomIn (
				wxMouseEvent&						event);
	
		void 	DoZoomOut (
				wxMouseEvent&						event);
	
		void 	DoZoomToOne (
				wxMouseEvent&						event);
		
		Boolean 	GetEnableFlagForStatisticsAndCluster (void);

		SInt32 OpenImageFile (
				LocalAppFile* 						localAppFilePtr,
				wxDocument** 						documentPtrPtr,
				Boolean 								fromOpenProjectImageWindowFlag,
				UInt16 								hdfDataSetSelection);
	
		void SetUpdateAreaUnits (
				wxUpdateUIEvent& 					pCmdUI,
				SInt16 								unitsCode);
	
		void UpdateWindowMenuList ();
	
	
		#ifndef multispec_wxmac
			 enum {
				ID_FILE_SAVE,
				ID_FILE_SAVE_AS
				};
		#endif
	
		wxMenu								*filemenu,
												*editmenu,
												*m_editClearOverlayMenu,
												*viewmenu,
												*projectmenu,
												*processormenu,
												*m_menu2,
												*m_menu3,
												*optionsmenu,
												*m_menu4,
												*m_menu5,
												*windowmenu,
												*helpmenu;
	
		wxStaticText						*m_zoomText;
	
		wxToolBarToolBase					*m_zoomInTool,
												*m_zoomOutTool;
	
		time_t 								m_nextControlTime;

		SInt16								m_imageZoomCode;
	
		bool									m_cancelOperationEventFlag,
												m_optionOverlayFlag,
												m_tooltipFlag;
	
		Boolean								m_controlDelayFlag,
												m_TOOL_PARAMETER_file_flag;

};	// end "class CMainFrame"
