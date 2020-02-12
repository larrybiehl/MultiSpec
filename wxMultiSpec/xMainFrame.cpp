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
//	File:						xMainFrame.cpp : class implementation file
//	Class Definition:		xMainFrame.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			10/05/2015 by Tsung Tai Yeh
//								01/11/2020 by Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMListResultsOptionsDlg class.
//
// Following is template for debugging
/*
		int numberChars = sprintf ((char*)gTextString3,
									 " xMainFrame:: (): %s",
									 gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "xMainFrame.h"
#include "xAbout.h"

// Toolbar icons//////
#if defined multispec_wxlin
	#include "res/copy.xpm"
	#include "res/cut.xpm"
	#include "res/help.xpm"
	#include "res/mspec.xpm"
	#include "res/open.xpm"
	#include "res/paste.xpm"
	#include "res/print.xpm"
	#include "res/save.xpm"
	#include "xToolBar_images.cpp"
#endif

#if defined multispec_wxmac
	//#include "mspec.xpm"
#endif


#include "wx/filedlg.h"
#include "wx/artprov.h"
#include "xMultiSpec.h"
#include "xImageDoc.h"
#include "xImageFrame.h"
#include "xGraphFrame.h"
#include "xGraphView.h"
#include "xStatisticsFrame.h"
#include "xStatisticsView.h"
#include "xTextFrame.h"
#include "xTextView.h"



IMPLEMENT_CLASS (CMainFrame, wxDocParentFrame)

BEGIN_EVENT_TABLE (CMainFrame, wxDocParentFrame)
	EVT_CHAR (CMainFrame::OnChar)

		// In order of location on the main menu

				// MultiSpec Application Menu
	EVT_MENU (wxID_ABOUT, CMainFrame::OnAbout)

				// File Menu
	EVT_MENU (ID_FILE_NEW_PROJECT, CMainFrame::OnFileNewProject)
						// Now in CMultiSpecApp (xMultiSpec.cpp)
	//EVT_MENU (wxID_OPEN, CMainFrame::OnFileOpen)
	EVT_MENU (ID_FILE_OPEN_PROJECT, CMainFrame::OnFileOpenProject)
	EVT_MENU (ID_FILE_OPEN_PROJECT_IMAGE, CMainFrame::OnFileOpenProjectImage)
	EVT_MENU (ID_FILE_OPEN_THEMATIC, CMainFrame::OnFileOpenThematic)
	EVT_MENU (ID_FILE_LOAD_TRANS_MATRIX, CMainFrame::OnFileLoadTransMatrix)
	EVT_MENU (ID_FILE_CLOSE_WINDOW, CMainFrame::OnFileCloseWindow)
	EVT_MENU (ID_FILE_SAVE, CMainFrame::OnFileSave)
	EVT_MENU (ID_FILE_SAVE_AS, CMainFrame::OnFileSaveAs)
	EVT_MENU (ID_FILE_SAVE_PROJECT, CMainFrame::OnFileSaveProject)
	EVT_MENU (ID_FILE_SAVE_PROJECT_AS, CMainFrame::OnFileSaveProjectAs)
	EVT_MENU (ID_EXPORT_FILE_AS, CMainFrame::OnFileExportFile)

				// Edit Menu
	EVT_MENU (ID_EDIT_UNDO, CMainFrame::OnEditUndo)
	EVT_MENU (wxID_CUT, CMainFrame::OnEditCut)
	EVT_MENU (wxID_COPY, CMainFrame::OnEditCopy)
	EVT_MENU (wxID_PASTE, CMainFrame::OnEditPaste)
	EVT_MENU (ID_EDIT_SELECT_ALL, CMainFrame::OnEditSelectAll)
	EVT_MENU (ID_EDIT_SELECTION_RECTANGLE, CMainFrame::OnEditSelectionRectangle)
	EVT_MENU (ID_EDIT_CLEAR_SELECT_RECTANGLE, CMainFrame::OnEditClearSelectionRectangle)
	EVT_MENU (ID_EDIT_CLEAR_TRANS_MATRIX, CMainFrame::OnEditClearTransMatrix)
	EVT_MENU (ID_EDIT_IMAGE_DESCRIPTION, CMainFrame::OnEditImageDescription)
	EVT_MENU (ID_EDIT_IMAGE_MAP_PARAMETERS, CMainFrame::OnEditImageMapParameters)
	EVT_MENU (ID_EDIT_CLEAR_ALL_IMAGE_OVERLAYS, CMainFrame::OnEditClearAllImageOverlays)
	EVT_MENU (ID_EDIT_CLEAR_ALL_VECTOR_OVERLAYS, CMainFrame::OnEditClearAllVectorOverlays)

				// View Menu Items
	EVT_MENU (ID_VIEW_COORDINATES_BAR, CMainFrame::OnViewCoordinatesBar)

				// Project Menu Items
	EVT_MENU (ID_PROJ_USE_ORIGINAL_STATS, CMainFrame::OnProjUseOriginalStats)
	EVT_MENU (ID_PROJ_USELEAVE1OUTSTATISTICS, CMainFrame::OnProjUseLeave1OutStats)
	EVT_MENU (ID_PROJ_USE_ENHANCED_STATS, CMainFrame::OnProjUseEnhancedStats)
	EVT_MENU (ID_PROJ_CLEAR_STATS, CMainFrame::OnProjClearStats)
	EVT_MENU (ID_PROJ_CHANGE_BASE_IMAGE_FILE, CMainFrame::OnProjChangeBaseImageFile)
	EVT_MENU (ID_PROJ_ADD_AS_ASSOCIATED_IMAGE, CMainFrame::OnProjAddAsAssociatedImage)

				// Processor Menu Items
	EVT_MENU (ID_PROC_DISPLAY_IMAGE, CMainFrame::OnProcDisplayImage)
	EVT_MENU (ID_PROC_HISTOGRAM_IMAGE, CMainFrame::OnProcHistogramImage)
	EVT_MENU (ID_PROC_LISTDATA, CMainFrame::OnProcListData)
	//EVT_MENU (ID_PROC_REFORMAT, CMainFrame::OnProcReformat)
	EVT_MENU (ID_PROC_CLUSTER, CMainFrame::OnProcCluster)
	EVT_MENU (ID_PROC_STATISTICS, CMainFrame::OnProcStatistics)
	EVT_MENU (ID_PROC_ENHANCE_STATISTICS, CMainFrame::OnProcEnhanceStatistics)
	EVT_MENU (ID_PROC_FEATURE_EXTRACTION, CMainFrame::OnProcFeatureExtraction)
	EVT_MENU (ID_PROC_FEATURE_SELECTION, CMainFrame::OnProcFeatureSelection)
	EVT_MENU (ID_PROC_CLASSIFY, CMainFrame::OnProcClassify)
	EVT_MENU (ID_PROC_LISTRESULTS, CMainFrame::OnProcListResults)

				// Processor->Reformat Menu Items
	EVT_MENU (ID_PROC_REFORMAT_CHANGE_IMAGE, CMainFrame::OnProcReformatChangeImage)
	EVT_MENU (ID_PROC_REFORMAT_CONVERT_PROJECT, CMainFrame::OnProcReformatConvertProject)
	EVT_MENU (ID_PROC_REFORMAT_CONVERT_SHAPE, CMainFrame::OnProcReformatConvertShape)
	EVT_MENU (ID_PROC_REFORMAT_MODIFY_CHANNEL, CMainFrame::OnProcReformatModifyChannel)
	EVT_MENU (ID_PROC_REFORMAT_MOSAIC_IMAGES, CMainFrame::OnProcReformatMosaicImages)
	EVT_MENU (ID_PROC_REFORMAT_RECODE_THEMATIC, CMainFrame::OnProcReformatRecodeThematic)
	EVT_MENU (ID_PROC_REFORMAT_RECTIFY_IMAGE, CMainFrame::OnProcReformatRectifyImage)
	EVT_MENU (ID_PROC_REFORMAT_CONVERT_ENVI, CMainFrame::OnProcReformatConvertEnvi)

				// Processor->Utility Menu Items
	EVT_MENU (ID_PROC_UTIL_PRIN_COMP_ANALYSIS, CMainFrame::OnProcUtilPrinCompAnalysis)
	EVT_MENU (ID_PROC_UTIL_CREATE_STAT_IMAGE, CMainFrame::OnProcUtilCreateStatImage)
	EVT_MENU (ID_PROC_UTIL_BIPLOTS_OF_DATA, CMainFrame::OnProcUtilBiplotsOfData)
	EVT_MENU (ID_PROC_UTIL_LIST_IMAGE_DESC, CMainFrame::OnProcUtilListImageDesc)
	EVT_MENU (ID_PROC_UTIL_CHECK_COVARIANCES, CMainFrame::OnProcUtilCheckCovariances)
	EVT_MENU (ID_PROC_UTIL_CHECKTRANS_MATRIX, CMainFrame::OnProcUtilCheckTransMatrix)

				// Options Menu Items
	EVT_MENU (ID_OPTIONS_AREAUNITS_SQKILOMETERS, CMainFrame::OnOptionsAreaUnitsSqKilometers)
	EVT_MENU (ID_OPTIONS_AREAUNITS_HECTARES, CMainFrame::OnOptionsAreaUnitsHectares)
	EVT_MENU (ID_OPTIONS_AREAUNITS_SQMETERS, CMainFrame::OnOptionsAreaUnitsSqMeters)
	EVT_MENU (ID_OPTIONS_AREAUNITS_SQCENTIMETERS, CMainFrame::OnOptionsAreaUnitsSqCentimeters)
	EVT_MENU (ID_OPTIONS_AREAUNITS_SQMILLIMETERS, CMainFrame::OnOptionsAreaUnitsSqMillimeters)
	EVT_MENU (ID_OPTIONS_AREAUNITS_SQMICROMETERS, CMainFrame::OnOptionsAreaUnitsSqMicrometers)
	EVT_MENU (ID_OPTIONS_AREAUNITS_SQMILES, CMainFrame::OnOptionsAreaUnitsSqMiles)
	EVT_MENU (ID_OPTIONS_AREAUNITS_ACRES, CMainFrame::OnOptionsAreaUnitsAcres)
	EVT_MENU (ID_OPTIONS_AREAUNITS_SQYARDS, CMainFrame::OnOptionsAreaUnitsSqYards)
	EVT_MENU (ID_OPTIONS_AREAUNITS_SQFEET, CMainFrame::OnOptionsAreaUnitsSqFeet)
	EVT_MENU (ID_OPTIONS_AREAUNITS_SQINCHES, CMainFrame::OnOptionsAreaUnitsSqInches)
	EVT_MENU (ID_OPTIONS_SWITCHCROSSCURSOR, CMainFrame::OnOptionsSwitchcrosscursor)
	EVT_MENU (ID_OPTIONS_SHOWTOOLTIP, CMainFrame::OnOptionsShowHideToolTip)

				// Window Menu Items
	EVT_MENU (ID_WINDOW_NEW_SELECTION_GRAPH, CMainFrame::OnWindowSelectionNewGraph)
	EVT_MENU (ID_WINDOW_TEXT_OUTPUT, CMainFrame::OnWindowOutputWindowSelection)
	EVT_MENU (ID_WINDOW_PROJECT, CMainFrame::OnWindowProjectWindowSelection)
	EVT_MENU_RANGE (ID_WINDOW_MENUITEMSTART, ID_WINDOW_MENUITEMEND, CMainFrame::OnWindowIorGWindowSelection)

		// In order of location on the main menu
				// File Menu Items
	EVT_UPDATE_UI (ID_FILE_NEW_PROJECT, CMainFrame::OnUpdateFileNewProject)
	EVT_UPDATE_UI (ID_IMAGE_OPEN, CMainFrame::OnUpdateFileOpen)
	EVT_UPDATE_UI (ID_FILE_OPEN_PROJECT, CMainFrame::OnUpdateFileOpenProject)
	EVT_UPDATE_UI (ID_FILE_OPEN_PROJECT_IMAGE, CMainFrame::OnUpdateFileOpenProjectImage)
	EVT_UPDATE_UI (ID_FILE_OPEN_THEMATIC, CMainFrame::OnUpdateFileOpenThematic)
	EVT_UPDATE_UI (ID_FILE_LOAD_TRANS_MATRIX, CMainFrame::OnUpdateFileLoadTransMatrix)
	EVT_UPDATE_UI (ID_FILE_CLOSE_WINDOW, CMainFrame::OnUpdateFileCloseWindow)
	EVT_UPDATE_UI (ID_FILE_SAVE, CMainFrame::OnUpdateFileSave)
	EVT_UPDATE_UI (ID_FILE_SAVE_AS, CMainFrame::OnUpdateFileSaveAs)
	EVT_UPDATE_UI (ID_FILE_SAVE_PROJECT, CMainFrame::OnUpdateFileSaveProject)
	EVT_UPDATE_UI (ID_FILE_SAVE_PROJECT_AS, CMainFrame::OnUpdateFileSaveProjectAs)
	EVT_UPDATE_UI (ID_FILE_PRINT, CMainFrame::OnUpdateFilePrint)
	EVT_UPDATE_UI (ID_PRINT_PREVIEW, CMainFrame::OnUpdateFilePrintPreview)
	EVT_UPDATE_UI (wxID_PAGE_SETUP, CMainFrame::OnUpdateFilePrintSetup)

				// Edit Menu Items
	EVT_UPDATE_UI (ID_EDIT_UNDO, CMainFrame::OnUpdateEditUndo)
	EVT_UPDATE_UI (wxID_CUT, CMainFrame::OnUpdateEditCut)
	EVT_UPDATE_UI (wxID_COPY, CMainFrame::OnUpdateEditCopy)
	EVT_UPDATE_UI (wxID_PASTE, CMainFrame::OnUpdateEditPaste)
	EVT_UPDATE_UI (wxID_CLEAR, CMainFrame::OnUpdateEditClear)
	EVT_UPDATE_UI (ID_EDIT_SELECT_ALL, CMainFrame::OnUpdateEditSelectAll)
	EVT_UPDATE_UI (ID_EDIT_SELECTION_RECTANGLE, CMainFrame::OnUpdateEditSelectionRectangle)
	EVT_UPDATE_UI (ID_EDIT_CLEAR_SELECT_RECTANGLE, CMainFrame::OnUpdateEditClearSelectionRectangle)
	EVT_UPDATE_UI (ID_EDIT_CLEAR_TRANS_MATRIX, CMainFrame::OnUpdateEditClearTransMatrix)
	EVT_UPDATE_UI (ID_EDIT_IMAGE_DESCRIPTION, CMainFrame::OnUpdateEditImageDescription)
	EVT_UPDATE_UI (ID_EDIT_IMAGE_MAP_PARAMETERS, CMainFrame::OnUpdateEditImageMapParameters)
	EVT_UPDATE_UI (ID_EDIT_CLEAR_ALL_IMAGE_OVERLAYS, CMainFrame::OnUpdateEditClearAllOverlays)
	EVT_UPDATE_UI (ID_EDIT_CLEAR_ALL_VECTOR_OVERLAYS, CMainFrame::OnUpdateEditClearAllVectorOverlays)

				// View Menu Items
	EVT_UPDATE_UI (ID_VIEW_TOOLBAR, CMainFrame::OnUpdateViewToolBar)
	EVT_UPDATE_UI (ID_VIEW_STATUS_BAR, CMainFrame::OnUpdateViewStatusBar)
	EVT_UPDATE_UI (ID_VIEW_COORDINATES_BAR, CMainFrame::OnUpdateViewCoordinatesBar)

				// Project Menu Items
	EVT_UPDATE_UI (ID_PROJ_USE_ORIGINAL_STATS, CMainFrame::OnUpdateProjectUseOriginalStats)
	EVT_UPDATE_UI (ID_PROJ_USELEAVE1OUTSTATISTICS, CMainFrame::OnUpdateProjectUseLeave1OutStatistics)
	EVT_UPDATE_UI (ID_PROJ_USE_ENHANCED_STATS, CMainFrame::OnUpdateProjectUseEnhancedStats)
	EVT_UPDATE_UI (ID_PROJ_MIXOFSTATISTICSUSED, CMainFrame::OnUpdateProjectMixofstatisticsused)
	EVT_UPDATE_UI (ID_PROJ_CLEAR_STATS, CMainFrame::OnUpdateProjectClearStats)
	EVT_UPDATE_UI (ID_PROJ_CHANGE_BASE_IMAGE_FILE, CMainFrame::OnUpdateProjectChangeBaseImageFile)
	EVT_UPDATE_UI (ID_PROJ_ADD_AS_ASSOCIATED_IMAGE, CMainFrame::OnUpdateProjectAddAsAssociatedImage)

				// Processor Menu Items
	EVT_UPDATE_UI (ID_PROC_DISPLAY_IMAGE, CMainFrame::OnUpdateProcDisplayImage)
	EVT_UPDATE_UI (ID_PROC_HISTOGRAM_IMAGE, CMainFrame::OnUpdateProcHistogramImage)
	EVT_UPDATE_UI (ID_PROC_LISTDATA, CMainFrame::OnUpdateProcListData)
	EVT_UPDATE_UI (ID_PROC_REFORMAT, CMainFrame::OnUpdateProcReformat)
	EVT_UPDATE_UI (ID_PROC_CLUSTER, CMainFrame::OnUpdateProcCluster)
	EVT_UPDATE_UI (ID_PROC_STATISTICS, CMainFrame::OnUpdateProcStatistics)
	EVT_UPDATE_UI (ID_PROC_ENHANCE_STATISTICS, CMainFrame::OnUpdateProcEnhanceStatistics)
	EVT_UPDATE_UI (ID_PROC_FEATURE_EXTRACTION, CMainFrame::OnUpdateProcFeatureExtraction)
	EVT_UPDATE_UI (ID_PROC_FEATURE_SELECTION, CMainFrame::OnUpdateProcFeatureSelection)
	EVT_UPDATE_UI (ID_PROC_CLASSIFY, CMainFrame::OnUpdateProcClassify)
	EVT_UPDATE_UI (ID_PROC_LISTRESULTS, CMainFrame::OnUpdateProcListResults)

				// Processor->Reformat Menu Items
	EVT_UPDATE_UI (ID_PROC_REFORMAT_CHANGE_IMAGE, CMainFrame::OnUpdateProcReformatChangeImage)
	EVT_UPDATE_UI (ID_PROC_REFORMAT_CONVERT_PROJECT, CMainFrame::OnUpdateProcReformatConvertProject)
	EVT_UPDATE_UI (ID_PROC_REFORMAT_CONVERT_SHAPE, CMainFrame::OnUpdateProcReformatConvertShape)
	EVT_UPDATE_UI (ID_PROC_REFORMAT_MODIFY_CHANNEL, CMainFrame::OnUpdateProcReformatModifyChannel)
	EVT_UPDATE_UI (ID_PROC_REFORMAT_MOSAIC_IMAGES, CMainFrame::OnUpdateProcReformatMosaicImages)
	EVT_UPDATE_UI (ID_PROC_REFORMAT_RECODE_THEMATIC, CMainFrame::OnUpdateProcReformatRecodeThematic)
	EVT_UPDATE_UI (ID_PROC_REFORMAT_RECTIFY_IMAGE, CMainFrame::OnUpdateProcReformatRectifyImage)
	EVT_UPDATE_UI (ID_PROC_REFORMAT_CONVERT_ENVI, CMainFrame::OnUpdateProcReformatConvertEnvi)

				// Processor->Utility Menu Items
	EVT_UPDATE_UI (ID_PROC_UTIL_PRIN_COMP_ANALYSIS, CMainFrame::OnUpdateProcUtilPrincipalComponentAnalysis)
	EVT_UPDATE_UI (ID_PROC_UTIL_CREATE_STAT_IMAGE, CMainFrame::OnUpdateProcUtilCreateStatImage)
	EVT_UPDATE_UI (ID_PROC_UTIL_BIPLOTS_OF_DATA, CMainFrame::OnUpdateProcUtilBiplotsOfData)
	EVT_UPDATE_UI (ID_PROC_UTIL_LIST_IMAGE_DESC, CMainFrame::OnUpdateProcUtilListImageDesc)
	EVT_UPDATE_UI (ID_PROC_UTIL_CHECK_COVARIANCES, CMainFrame::OnUpdateProcUtilCheckCovariances)
	EVT_UPDATE_UI (ID_PROC_UTIL_CHECKTRANS_MATRIX, CMainFrame::OnUpdateProcUtilCheckTransMatrix)

				// Options Menu Items
	EVT_UPDATE_UI (ID_OPT_MEMORY_STATUS, CMainFrame::OnUpdateOptionsMemoryStatus)
	EVT_UPDATE_UI (ID_OPTIONS_AREAUNITS_SQKILOMETERS, CMainFrame::OnUpdateOptionsAreaUnitsSqKilometers)
	EVT_UPDATE_UI (ID_OPTIONS_AREAUNITS_HECTARES, CMainFrame::OnUpdateOptionsAreaUnitsHectares)
	EVT_UPDATE_UI (ID_OPTIONS_AREAUNITS_SQMETERS, CMainFrame::OnUpdateOptionsAreaUnitsSqMeters)
	EVT_UPDATE_UI (ID_OPTIONS_AREAUNITS_SQCENTIMETERS, CMainFrame::OnUpdateOptionsAreaUnitsSqCentimeters)
	EVT_UPDATE_UI (ID_OPTIONS_AREAUNITS_SQMILLIMETERS, CMainFrame::OnUpdateOptionsAreaUnitsSqMillimeters)
	EVT_UPDATE_UI (ID_OPTIONS_AREAUNITS_SQMICROMETERS, CMainFrame::OnUpdateOptionsAreaUnitsSqMicrometers)
	EVT_UPDATE_UI (ID_OPTIONS_AREAUNITS_SQMILES, CMainFrame::OnUpdateOptionsAreaUnitsSqMiles)
	EVT_UPDATE_UI (ID_OPTIONS_AREAUNITS_ACRES, CMainFrame::OnUpdateOptionsAreaUnitsAcres)
	EVT_UPDATE_UI (ID_OPTIONS_AREAUNITS_SQYARDS, CMainFrame::OnUpdateOptionsAreaUnitsSqYards)
	EVT_UPDATE_UI (ID_OPTIONS_AREAUNITS_SQFEET, CMainFrame::OnUpdateOptionsAreaUnitsSqFeet)
	EVT_UPDATE_UI (ID_OPTIONS_AREAUNITS_SQINCHES, CMainFrame::OnUpdateOptionsAreaUnitsSqInches)
	EVT_UPDATE_UI (ID_OPTIONS_SWITCHCROSSCURSOR, CMainFrame::OnUpdateOptionsSwitchcrosscursor)
	EVT_UPDATE_UI (ID_OPTIONS_SHOWTOOLTIP, CMainFrame::OnUpdateOptionsShowToolTip)

				// Windows Menu Items
	EVT_UPDATE_UI (ID_WINDOW_NEW_SELECTION_GRAPH, CMainFrame::OnUpdateWindowNewSelectionGraph)
	EVT_UPDATE_UI (ID_WINDOW_TEXT_OUTPUT, CMainFrame::OnUpdateWindowTextOutput)
	EVT_UPDATE_UI (ID_WINDOW_PROJECT, CMainFrame::OnUpdateWindowProject)

			// Image Window Tool bar items
	EVT_UPDATE_UI (ID_MAGNIFICATION, CMainFrame::OnUpdateToolBarMagnification)
	EVT_UPDATE_UI (ID_ZOOM_IN, CMainFrame::OnUpdateToolBarZoomIn)
	EVT_UPDATE_UI (ID_ZOOM_OUT, CMainFrame::OnUpdateToolBarZoomOut)
	EVT_UPDATE_UI (ID_ZOOMINFO, CMainFrame::OnUpdateToolBarZoomInfo)
	EVT_UPDATE_UI (ID_OVERLAY, CMainFrame::OnUpdateToolBarOverlay)

	#if defined multispec_wxlin
		EVT_TOOL (ID_OVERLAY, CMainFrame::OnToolBarShowOverlay)
	#endif

	EVT_MENU_RANGE (ID_CLEAROVERLAYMENUITEMSTART, ID_CLEAROVERLAYMENUITEMEND, CMainFrame::OnEditClearSelectedOverlay)
	EVT_MENU_RANGE (ID_SHOWOVERLAYMENUITEMSTART, ID_SHOWOVERLAYMENUITEMEND, CMainFrame::OnToolBarOverlaySelection)

	EVT_IDLE (CMainFrame::OnIdle)
END_EVENT_TABLE ()



CMainFrame::CMainFrame (
				wxDocManager*						manager,
				wxDocParentFrame*					frame,
				const wxString& 					title,
				const wxPoint& 					pos,
				const wxSize& 						size,
				long 									type)
		: wxDocParentFrame (manager,
									frame,
									wxID_ANY,
									title,
									pos,
									size,
									type,
									_T("MainFrame"))

{
	m_imageZoomCode = 0;
	m_controlDelayFlag = TRUE;
	m_TOOL_PARAMETER_file_flag = TRUE;
   m_tooltipFlag = TRUE;
   m_toolBar1 = NULL;
   m_cancelOperationEventFlag = FALSE;
   m_nextControlTime = 0;
   
	//wxInitAllImageHandlers ();	// Being called in Init.
	#if defined multispec_wxlin
		SetIcon (wxICON (mspec));
		wxBitmap openbmp = wxBitmap (open_xpm);
		wxBitmap savebmp = wxBitmap (save_xpm);
		wxBitmap cutbmp = wxBitmap (cut_xpm);
		wxBitmap copybmp = wxBitmap (copy_xpm);
		wxBitmap pastebmp = wxBitmap (paste_xpm);
		wxBitmap printbmp = wxBitmap (print_xpm);
		wxBitmap helpbmp = wxBitmap (help_xpm);
		wxBitmap overlayi = wxBITMAP_PNG_FROM_DATA (overlay);
		/*
		wxBitmap zoom1 = wxBITMAP_PNG_FROM_DATA (zoomx1);
		wxBitmap zoomout = wxBITMAP_PNG_FROM_DATA (zoom_out);
		wxBitmap zoomin = wxBITMAP_PNG_FROM_DATA (zoom_in);
		*/
		wxBitmap zoom1 = wxBITMAP_PNG (zoomx1);
		wxBitmap zoomout = wxBITMAP_PNG (zoom_out);
		wxBitmap zoomin = wxBITMAP_PNG (zoom_in);
	#endif
	
	m_menubar1 = new wxMenuBar (0);
	filemenu = new wxMenu ();
	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem (
							filemenu,
							ID_FILE_NEW_PROJECT,
							wxString (wxT("New Project")),
							wxT("Create a New Project using active image window as the base"),
							wxITEM_NORMAL);
	filemenu->Append (m_menuItem3);

	wxMenuItem* m_menuItem4;
	m_menuItem4 = new wxMenuItem (
								filemenu,
								ID_IMAGE_OPEN,
								wxString (wxT("&Open Image...")) + wxT('\t') + wxT("Ctrl+O"),
								wxT("Select image file(s)"),
								wxITEM_NORMAL);
	filemenu->Append (m_menuItem4);

	wxMenuItem* m_menuItem5;
	m_menuItem5 = new wxMenuItem (
								filemenu,
								ID_FILE_OPEN_PROJECT,
								wxString (wxT("Open Project...")) + wxT('\t') + wxT("Ctrl+;"),
								wxEmptyString,
								wxITEM_NORMAL);
	filemenu->Append (m_menuItem5);

	wxMenuItem* m_menuItem6;
	m_menuItem6 = new wxMenuItem (filemenu,
											ID_FILE_OPEN_PROJECT_IMAGE,
											wxString (wxT("Open Project Image...")),
											wxEmptyString,
											wxITEM_NORMAL);
	filemenu->Append (m_menuItem6);

	wxMenuItem* m_menuItem7;
	m_menuItem7 = new wxMenuItem (filemenu,
											ID_FILE_OPEN_THEMATIC,
											wxString (wxT("Open Thematic Class Info...")),
											wxEmptyString,
											wxITEM_NORMAL);
	filemenu->Append (m_menuItem7);

	wxMenuItem* m_menuItem8;
	m_menuItem8 = new wxMenuItem (
									filemenu,
									ID_FILE_CLOSE_WINDOW,	 //wxID_CLOSE
									wxString (wxT("Close Window")) + wxT('\t') + wxT("Ctrl+W"),
									wxT("Close the active image window"),
									wxITEM_NORMAL);
	filemenu->Append (m_menuItem8);
	/*
   temporary
   wxMenuItem* m_menuItem109;
	m_menuItem109 = new wxMenuItem (filemenu,
												ID_PROJ_CLOSE_WINDOW,
												wxString (wxT("Close Project")),
												wxT("Close the active project window"),
												wxITEM_NORMAL); //wxID_CLOSE
	filemenu->Append (m_menuItem109);
   */
	filemenu->AppendSeparator ();

	wxMenuItem* m_menuItem9;
	m_menuItem9 = new wxMenuItem (filemenu,
											ID_FILE_LOAD_TRANS_MATRIX,
											wxString (wxT("Load Transformation Matrix...")),
											wxEmptyString,
											wxITEM_NORMAL);
	filemenu->Append (m_menuItem9);

	filemenu->AppendSeparator ();

	wxMenuItem* m_menuItem10;
	m_menuItem10 = new wxMenuItem (filemenu,
												ID_FILE_SAVE,
												wxString (wxT("Save")),
												wxEmptyString,
												wxITEM_NORMAL);
	filemenu->Append (m_menuItem10);

	wxMenuItem* m_menuItem11;
	m_menuItem11 = new wxMenuItem (
									filemenu,
									ID_FILE_SAVE_AS,
									wxString (wxT("&Save As...")) + wxT('\t') + wxT("Ctrl+S"),
									wxEmptyString,
									wxITEM_NORMAL);
	filemenu->Append (m_menuItem11);

	wxMenuItem* m_menuItem12;
	m_menuItem12 = new wxMenuItem (filemenu,
												ID_FILE_SAVE_PROJECT,
												wxString (wxT("Save Project")),
												wxEmptyString,
												wxITEM_NORMAL);
	filemenu->Append (m_menuItem12);

	wxMenuItem* m_menuItem13;
	m_menuItem13 = new wxMenuItem (filemenu,
												ID_FILE_SAVE_PROJECT_AS,
												wxString (wxT("Save Project As...")),
												wxEmptyString,
												wxITEM_NORMAL);
	filemenu->Append (m_menuItem13);
	
	#if defined multispec_wxlin
		wxMenuItem* m_menuItem13b;
		m_menuItem13b = new wxMenuItem (filemenu,
													ID_EXPORT_FILE_AS,
													wxString (wxT("Export File...")),
													wxT("Upload file to your computer"),
													wxITEM_NORMAL);
		filemenu->Append (m_menuItem13b);
	#endif

	filemenu->AppendSeparator ();

	wxMenuItem* m_menuItem14;
	m_menuItem14 = new wxMenuItem (
								filemenu,
								ID_FILE_PRINT,
								wxString (wxT("&Print Text...")) + wxT('\t') + wxT("Ctrl+P"),
								wxEmptyString,
								wxITEM_NORMAL); // wxID_PRINT
	filemenu->Append (m_menuItem14);
	m_menuItem14->Enable (false);

	wxMenuItem* m_menuItem15;
	m_menuItem15 = new wxMenuItem (filemenu,
												ID_PRINT_PREVIEW,
												wxString (wxT("Print Preview")),
												wxEmptyString,
												wxITEM_NORMAL); // wxID_PREVIEW
	filemenu->Append (m_menuItem15);
	m_menuItem15->Enable (false);

	wxMenuItem* m_menuItem16;
	m_menuItem16 = new wxMenuItem (filemenu,
												wxID_PAGE_SETUP,
												wxString (wxT("P&rint Setup")),
												wxEmptyString,
												wxITEM_NORMAL);
	filemenu->Append (m_menuItem16);
	m_menuItem16->Enable (false);

	filemenu->AppendSeparator ();

	wxMenuItem* m_menuItem17;
	m_menuItem17 = new wxMenuItem (
								filemenu,
								wxID_EXIT,
								wxString (wxT("E&xit MultiSpec")) + wxT('\t') + wxT("Ctrl+Q"),
								wxEmptyString,
								wxITEM_NORMAL);
	filemenu->Append (m_menuItem17);

	m_menubar1->Append (filemenu, wxT("&File"));

	editmenu = new wxMenu ();
	wxMenuItem* m_menuItem18;
			// Note that wxID_UNDO and wxID_REDO do not seem to be caught.
	m_menuItem18 = new wxMenuItem (editmenu,
											ID_EDIT_UNDO,
											wxString (wxT("&Undo")) + wxT('\t') + wxT("Ctrl+Z"),
											wxEmptyString,
											wxITEM_NORMAL);
	editmenu->Append (m_menuItem18);

	editmenu->AppendSeparator ();

	wxMenuItem* m_menuItem19;
	m_menuItem19 = new wxMenuItem (editmenu,
												wxID_CUT,
												wxString (wxT("Cut")) + wxT('\t') + wxT("Ctrl+X"),
												wxEmptyString,
												wxITEM_NORMAL);
	editmenu->Append (m_menuItem19);

	wxMenuItem* m_menuItem20;
	m_menuItem20 = new wxMenuItem (editmenu,
												wxID_COPY,
												wxString (wxT("Copy")) + wxT('\t') + wxT("Ctrl+C"),
												wxEmptyString,
												wxITEM_NORMAL);
	editmenu->Append (m_menuItem20);

	wxMenuItem* m_menuItem21;
	m_menuItem21 = new wxMenuItem (editmenu,
												wxID_PASTE,
												wxString (wxT("Paste")) + wxT('\t') + wxT("Ctrl+V"),
												wxEmptyString,
												wxITEM_NORMAL);
	editmenu->Append (m_menuItem21);

	wxMenuItem* m_menuItem22;
	m_menuItem22 = new wxMenuItem (
										editmenu,
										wxID_CLEAR,
										wxString (wxT("Clear")) /*+ wxT('\t') + wxT("Delete")*/,
										wxEmptyString,
										wxITEM_NORMAL);
	editmenu->Append (m_menuItem22);

	wxMenuItem* m_menuItem23;
	m_menuItem23 = new wxMenuItem (
										editmenu,
										ID_EDIT_SELECT_ALL,
										wxString (wxT("Select All")) + wxT('\t') + wxT("Ctrl+A"),
										wxEmptyString,
										wxITEM_NORMAL);
	editmenu->Append (m_menuItem23);

	editmenu->AppendSeparator ();

	wxMenuItem* m_menuItem24;
	m_menuItem24 = new wxMenuItem (editmenu,
												ID_EDIT_SELECTION_RECTANGLE,
												wxString (wxT("Edit Selection Rectangle...")),
												wxEmptyString,
												wxITEM_NORMAL);
	editmenu->Append (m_menuItem24);

	wxMenuItem* m_menuItem25;
	m_menuItem25 = new wxMenuItem (editmenu,
												ID_EDIT_CLEAR_SELECT_RECTANGLE,
												wxString (wxT("Clear Selection Rectangle")),
												wxEmptyString,
												wxITEM_NORMAL);
	editmenu->Append (m_menuItem25);

	editmenu->AppendSeparator ();

	wxMenuItem* m_menuItem26;
	m_menuItem26 = new wxMenuItem (editmenu,
												ID_EDIT_CLEAR_TRANS_MATRIX,
												wxString (wxT("Clear Transformation Matrix")),
												wxEmptyString,
												wxITEM_NORMAL);
	editmenu->Append (m_menuItem26);

	editmenu->AppendSeparator ();

	wxMenuItem* m_menuItem27;
	m_menuItem27 = new wxMenuItem (
							editmenu,
							ID_EDIT_IMAGE_DESCRIPTION,
							wxString (wxT("Image Description...")) + wxT('\t') + wxT("Ctrl+,"),
							wxT("View/edit map image file organization"),
							wxITEM_NORMAL);
	editmenu->Append (m_menuItem27);

	wxMenuItem* m_menuItem28;
	m_menuItem28 = new wxMenuItem (
						editmenu,
						ID_EDIT_IMAGE_MAP_PARAMETERS,
						wxString (wxT("Image Map Parameters...")) + wxT('\t') + wxT("Ctrl+`"),
						wxT("View/edit map image map parameters"),
						wxITEM_NORMAL);
	editmenu->Append (m_menuItem28);

	editmenu->AppendSeparator ();

	m_editClearOverlayMenu = new wxMenu ();
	wxMenuItem* m_menuItem29;
	m_menuItem29 = new wxMenuItem (
									m_editClearOverlayMenu,
									ID_EDIT_CLEAR_ALL_IMAGE_OVERLAYS,
									wxString (wxT("All Image")), wxT("Clear all image overlays"),
									wxITEM_NORMAL);
	m_editClearOverlayMenu->Append (m_menuItem29);

	wxMenuItem* m_menuItem30;
	m_menuItem30 = new wxMenuItem (m_editClearOverlayMenu,
												ID_EDIT_CLEAR_ALL_VECTOR_OVERLAYS,
												wxString (wxT("All Vector")),
												wxT("Clear all shape files"),
												wxITEM_NORMAL);
	m_editClearOverlayMenu->Append (m_menuItem30);

	m_editClearOverlayMenu->AppendSeparator ();

	editmenu->Append (-1,
							wxT("Clear Overlays"),
							m_editClearOverlayMenu,
							wxT("Clear selected image overlays and shape files"));

	m_menubar1->Append (editmenu, wxT("&Edit"));

	#if defined multispec_wxlin
		viewmenu = new wxMenu ();
	
		wxMenuItem* m_menuItem31;
		m_menuItem31 = new wxMenuItem (viewmenu,
													ID_VIEW_TOOLBAR,
													wxString (wxT("Toolbar")),
													wxEmptyString,
													wxITEM_CHECK);
		viewmenu->Append (m_menuItem31);

		wxMenuItem* m_menuItem32;
		m_menuItem32 = new wxMenuItem (viewmenu,
													ID_VIEW_STATUS_BAR,
													wxString (wxT("Status Bar")),
													wxEmptyString,
													wxITEM_CHECK);
		viewmenu->Append (m_menuItem32);

		wxMenuItem* m_menuItem33;
		m_menuItem33 = new wxMenuItem (
								viewmenu,
								ID_VIEW_COORDINATES_BAR,
								wxString (wxT("Coordinate View")) + wxT('\t') + wxT("Ctrl+1"),
								wxT("Show/hide coordinate info above active image window"),
								wxITEM_CHECK);
		viewmenu->Append (m_menuItem33);
		m_menuItem33->Enable (false);

		m_menubar1->Append (viewmenu, wxT("&View"));
	#endif

	#if defined multispec_wxmac
		viewmenu = new wxMenu();

		wxMenuItem* m_menuItem33;
		m_menuItem33 = new wxMenuItem (
								viewmenu,
								ID_VIEW_COORDINATES_BAR,
								wxString (wxT("Coordinate View")) + wxT('\t') + wxT("Ctrl+1"),
								wxT("Show/hide coordinate info above active image window"),
								wxITEM_CHECK);
		viewmenu->Append (m_menuItem33);
		m_menuItem33->Enable (false);

		m_menubar1->Append (viewmenu, wxT("&View"));
	#endif
	
	projectmenu = new wxMenu();
	wxMenuItem* m_menuItem34;
	m_menuItem34 = new wxMenuItem (projectmenu,
												ID_PROJ_USE_ORIGINAL_STATS,
												wxString (wxT("Use original statistics")),
												wxEmptyString,
												wxITEM_CHECK);
	projectmenu->Append (m_menuItem34);

	wxMenuItem* m_menuItem35;
	m_menuItem35 = new wxMenuItem (projectmenu,
												ID_PROJ_USELEAVE1OUTSTATISTICS,
												wxString (wxT("Use LOOC statistics")),
												wxEmptyString,
												wxITEM_CHECK);
	projectmenu->Append (m_menuItem35);

	wxMenuItem* m_menuItem36;
	m_menuItem36 = new wxMenuItem (projectmenu,
												ID_PROJ_USE_ENHANCED_STATS,
												wxString (wxT("Use enhanced statistics ")),
												wxEmptyString,
												wxITEM_CHECK);
	projectmenu->Append (m_menuItem36);

	wxMenuItem* m_menuItem37;
	m_menuItem37 = new wxMenuItem (projectmenu,
												ID_PROJ_MIXOFSTATISTICSUSED,
												wxString (wxT("Mix of statistics used")),
												wxEmptyString,
												wxITEM_CHECK);
	projectmenu->Append (m_menuItem37);

	projectmenu->AppendSeparator ();

	wxMenuItem* m_menuItem38;
	m_menuItem38 = new wxMenuItem (projectmenu,
												ID_PROJ_CLEAR_STATS,
												wxString (wxT("Clear statistics")),
												wxEmptyString,
												wxITEM_NORMAL);
	projectmenu->Append (m_menuItem38);

	wxMenuItem* m_menuItem39;
	m_menuItem39 = new wxMenuItem (projectmenu,
												ID_PROJ_CHANGE_BASE_IMAGE_FILE,
												wxString (wxT("Change Base Image File...")),
												wxEmptyString,
												wxITEM_NORMAL);
	projectmenu->Append (m_menuItem39);
	
 	projectmenu->AppendSeparator ();

	wxMenuItem* m_menuItem40;
	m_menuItem40 = new wxMenuItem (projectmenu,
												ID_PROJ_ADD_AS_ASSOCIATED_IMAGE,
												wxString (wxT("Add as associated Image")),
												wxEmptyString,
												wxITEM_NORMAL);
	projectmenu->Append (m_menuItem40);

	m_menubar1->Append (projectmenu, wxT("&Project"));

	processormenu = new wxMenu ();
	wxMenuItem* m_menuItem401;
	m_menuItem401 = new wxMenuItem (
								processormenu,
								ID_PROC_DISPLAY_IMAGE,
								wxString (wxT("Display Image...")) + wxT('\t') + wxT("Ctrl+D"),
								wxT("Edit specifications for the active displayed image"),
								wxITEM_NORMAL);
	processormenu->Append (m_menuItem401);

	wxMenuItem* m_menuItem41;
	m_menuItem41 = new wxMenuItem (
								processormenu,
								ID_PROC_HISTOGRAM_IMAGE,
								wxString (wxT("Histogram Image...")) + wxT('\t') + wxT("Ctrl+H"),
								wxT("Create a histogram statistics of the active image"),
								wxITEM_NORMAL);
	processormenu->Append (m_menuItem41);

	wxMenuItem* m_menuItem42;
	m_menuItem42 = new wxMenuItem (
									processormenu,
									ID_PROC_LISTDATA,
									wxString (wxT("List Data...")) + wxT('\t') + wxT("Ctrl+J"),
									wxT("List selected data values in the active image"),
									wxITEM_NORMAL);
	processormenu->Append (m_menuItem42);

	m_menu2 = new wxMenu ();

	wxMenuItem* m_menuItem45;
	m_menuItem45 = new wxMenuItem (
					m_menu2,
					ID_PROC_REFORMAT_CHANGE_IMAGE,
					wxString (wxT("Change Image File Format...")) + wxT('\t') + wxT("Ctrl+R"),
					wxT("Create new file based on function of active image"),
					wxITEM_NORMAL);
	m_menu2->Append (m_menuItem45);

	wxMenuItem* m_menuItem47;
	m_menuItem47 = new wxMenuItem (
							m_menu2,
							ID_PROC_REFORMAT_CONVERT_PROJECT,
							wxString (wxT("Convert Project File to Thematic Image File...")),
							wxEmptyString,
							wxITEM_NORMAL);
	m_menu2->Append (m_menuItem47);

	wxMenuItem* m_menuItem48;
	m_menuItem48 = new wxMenuItem (
								m_menu2,
								ID_PROC_REFORMAT_CONVERT_SHAPE,
								wxString (wxT("Convert Shape File to Thematic Image File...")),
								wxEmptyString,
								wxITEM_NORMAL);
	m_menu2->Append (m_menuItem48);

	wxMenuItem* m_menuItem49;
	m_menuItem49 = new wxMenuItem (m_menu2,
												ID_PROC_REFORMAT_MODIFY_CHANNEL,
												wxString (wxT("Modify Channel Descriptions...")),
												wxEmptyString,
												wxITEM_NORMAL);
	m_menu2->Append (m_menuItem49);

	wxMenuItem* m_menuItem50;
	m_menuItem50 = new wxMenuItem (m_menu2,
												ID_PROC_REFORMAT_MOSAIC_IMAGES,
												wxString (wxT("Mosaic Images...")),
												wxEmptyString,
												wxITEM_NORMAL);
	m_menu2->Append (m_menuItem50);

	wxMenuItem* m_menuItem51;
	m_menuItem51 = new wxMenuItem (m_menu2,
												ID_PROC_REFORMAT_RECODE_THEMATIC,
												wxString (wxT("Recode Thematic Image...")),
												wxEmptyString,
												wxITEM_NORMAL);
	m_menu2->Append (m_menuItem51);

	wxMenuItem* m_menuItem52;
	m_menuItem52 = new wxMenuItem (m_menu2,
												ID_PROC_REFORMAT_RECTIFY_IMAGE,
												wxString (wxT("Rectify Image...")),
												wxEmptyString,
												wxITEM_NORMAL);
	m_menu2->Append (m_menuItem52);

	wxMenuItem* m_menuItem53;
	m_menuItem53 = new wxMenuItem (
								m_menu2,
								ID_PROC_REFORMAT_CONVERT_ENVI,
								wxString (wxT("Convert ENVI ASCII ROI to Thematic Image...")),
								wxEmptyString,
								wxITEM_NORMAL);
	m_menu2->Append (m_menuItem53);

	processormenu->Append (-1, wxT("Reformat"), m_menu2);

	processormenu->AppendSeparator ();

	wxMenuItem* m_menuItem56;
	m_menuItem56 = new wxMenuItem (
						processormenu,
						ID_PROC_CLUSTER,
						wxString (wxT("Cluster...")) + wxT('\t') + wxT("Ctrl+L"),
						wxT("Unsupervised classification analysis of active image window"),
						wxITEM_NORMAL);
	processormenu->Append (m_menuItem56);

	wxMenuItem* m_menuItem57;
	m_menuItem57 = new wxMenuItem (
									processormenu,
									ID_PROC_STATISTICS,
									wxString (wxT("Statistics...")) + wxT('\t') + wxT("Ctrl+T"),
									wxEmptyString,
									wxITEM_NORMAL);
	processormenu->Append (m_menuItem57);

	wxMenuItem* m_menuItem58;
	m_menuItem58 = new wxMenuItem (
						processormenu,
						ID_PROC_ENHANCE_STATISTICS,
						wxString (wxT("Enhance Statistics...")) + wxT('\t') + wxT("Ctrl+E"),
						wxEmptyString,
						wxITEM_NORMAL);
	processormenu->Append (m_menuItem58);

	wxMenuItem* m_menuItem59;
	m_menuItem59 = new wxMenuItem (
							processormenu,
							ID_PROC_FEATURE_EXTRACTION,
							wxString (wxT("Feature Extraction...")) + wxT('\t') + wxT("Ctrl+F"),
							wxEmptyString,
							wxITEM_NORMAL);
	processormenu->Append (m_menuItem59);

	wxMenuItem* m_menuItem60;
	m_menuItem60 = new wxMenuItem (
							processormenu,
							ID_PROC_FEATURE_SELECTION,
							wxString (wxT("Feature Selection...")) + wxT('\t') + wxT("Ctrl+B"),
							wxEmptyString,
							wxITEM_NORMAL);
	processormenu->Append (m_menuItem60);

	wxMenuItem* m_menuItem61;
	m_menuItem61 = new wxMenuItem (
									processormenu,
									ID_PROC_CLASSIFY,
									wxString (wxT("Classify...")) + wxT('\t') + wxT("Ctrl+M"),
									wxEmptyString,
									wxITEM_NORMAL);
	processormenu->Append (m_menuItem61);

	wxMenuItem* m_menuItem62;
	m_menuItem62 = new wxMenuItem (
								processormenu,
								ID_PROC_LISTRESULTS,
								wxString (wxT("List Results...")) + wxT('\t') + wxT("Ctrl+Y"),
								wxEmptyString,
								wxITEM_NORMAL);
	processormenu->Append (m_menuItem62);

	processormenu->AppendSeparator ();

	m_menu3 = new wxMenu ();
	wxMenuItem* m_menuItem63;
	m_menuItem63 = new wxMenuItem (
			m_menu3,
			ID_PROC_UTIL_PRIN_COMP_ANALYSIS,
			wxString (wxT("Principal Component Analysis...")) + wxT('\t') + wxT("Ctrl+K"),
			wxT("Principal component analysis of active image window"),
			wxITEM_NORMAL);
	m_menu3->Append (m_menuItem63);

	wxMenuItem* m_menuItem64;
	m_menuItem64 = new wxMenuItem (
					m_menu3,
					ID_PROC_UTIL_CREATE_STAT_IMAGE,
					wxString (wxT("Create Statistics Image...")) + wxT('\t') + wxT("Ctrl+I"),
					wxEmptyString,
					wxITEM_NORMAL);
	m_menu3->Append (m_menuItem64);

	wxMenuItem* m_menuItem65;
	m_menuItem65 = new wxMenuItem (
							m_menu3,
							ID_PROC_UTIL_BIPLOTS_OF_DATA,
							wxString (wxT("Biplots of Data...")) + wxT('\t') + wxT("Ctrl+G"),
							wxEmptyString,
							wxITEM_NORMAL);
	m_menu3->Append (m_menuItem65);

	wxMenuItem* m_menuItem66;
	m_menuItem66 = new wxMenuItem (
						m_menu3,
						ID_PROC_UTIL_LIST_IMAGE_DESC,
						wxString (wxT("List Image Description")) + wxT('\t') + wxT("Ctrl+U"),
						wxEmptyString,
						wxITEM_NORMAL);
	m_menu3->Append (m_menuItem66);

	wxMenuItem* m_menuItem67;
	m_menuItem67 = new wxMenuItem (
							m_menu3,
							ID_PROC_UTIL_CHECK_COVARIANCES,
							wxString (wxT("Check Covariances...")) + wxT('\t') + wxT("Ctrl+["),
							wxEmptyString,
							wxITEM_NORMAL);
	m_menu3->Append (m_menuItem67);

	wxMenuItem* m_menuItem68;
	m_menuItem68 = new wxMenuItem (
				m_menu3,
				ID_PROC_UTIL_CHECKTRANS_MATRIX,
				wxString (wxT("Check Transformation Matrix...")) + wxT('\t') + wxT("Ctrl+]"),
				wxT("List and analyse saved transformation matrix"),
				wxITEM_NORMAL);
	m_menu3->Append (m_menuItem68);

	processormenu->Append (-1, wxT("Utilities"), m_menu3);

	m_menubar1->Append (processormenu, wxT("P&rocessor"));

	optionsmenu = new wxMenu ();
	wxMenuItem* m_menuItem69;
	m_menuItem69 = new wxMenuItem (
								optionsmenu,
								ID_OPT_MEMORY_STATUS,
								wxString (wxT("Memory Status...")) + wxT('\t') + wxT("Ctrl+\\"),
								wxEmptyString,
								wxITEM_NORMAL);
	optionsmenu->Append (m_menuItem69);

	m_menu5 = new wxMenu ();
	wxMenuItem* m_menuItem75;
	m_menuItem75 = new wxMenuItem (
					m_menu5,
					ID_OPTIONS_AREAUNITS_SQKILOMETERS,
					wxString (wxT("Sq. Kilometeres")), wxT("Areas given in square kilometer"),
					wxITEM_CHECK);
	m_menu5->Append (m_menuItem75);

	wxMenuItem* m_menuItem76;
	m_menuItem76 = new wxMenuItem (m_menu5,
												ID_OPTIONS_AREAUNITS_HECTARES,
												wxString (wxT("Hectares")),
												wxT("Areas given in hectare"),
												wxITEM_CHECK);
	m_menu5->Append (m_menuItem76);

	wxMenuItem* m_menuItem77;
	m_menuItem77 = new wxMenuItem (m_menu5,
												ID_OPTIONS_AREAUNITS_SQMETERS,
												wxString (wxT("Sq. meters")),
												wxT("Areas given in square meter"),
												wxITEM_CHECK);
	m_menu5->Append (m_menuItem77);

	wxMenuItem* m_menuItem78;
	m_menuItem78 = new wxMenuItem (m_menu5,
												ID_OPTIONS_AREAUNITS_SQCENTIMETERS,
												wxString (wxT("Sq. centimeters")),
												wxT("Areas given in square centimeter"),
												wxITEM_CHECK);
	m_menu5->Append (m_menuItem78);

	wxMenuItem* m_menuItem79;
	m_menuItem79 = new wxMenuItem (m_menu5,
												ID_OPTIONS_AREAUNITS_SQMILLIMETERS,
												wxString (wxT("Sq. millimeters")),
												wxT("Areas given in square milimeters"),
												wxITEM_CHECK);
	m_menu5->Append (m_menuItem79);

	wxMenuItem* m_menuItem80;
	m_menuItem80 = new wxMenuItem (m_menu5,
												ID_OPTIONS_AREAUNITS_SQMICROMETERS,
												wxString (wxT("Sq. micrometers")),
												wxT("Areas given in square micrometers"),
												wxITEM_CHECK);
	m_menu5->Append (m_menuItem80);

	wxMenuItem* m_menuItem81;
	m_menuItem81 = new wxMenuItem (m_menu5,
												ID_OPTIONS_AREAUNITS_SQMILES,
												wxString (wxT("Sq. miles")),
												wxT("Areas given in square miles"),
												wxITEM_CHECK);
	m_menu5->Append (m_menuItem81);

	wxMenuItem* m_menuItem82;
	m_menuItem82 = new wxMenuItem (m_menu5,
												ID_OPTIONS_AREAUNITS_ACRES,
												wxString (wxT("Acres")),
												wxT("Areas given in acres"),
												wxITEM_CHECK);
	m_menu5->Append (m_menuItem82);

	wxMenuItem* m_menuItem83;
	m_menuItem83 = new wxMenuItem (m_menu5,
												ID_OPTIONS_AREAUNITS_SQYARDS,
												wxString (wxT("Sq. yards")),
												wxT("Areas given in square yards"),
												wxITEM_CHECK);
	m_menu5->Append (m_menuItem83);

	wxMenuItem* m_menuItem84;
	m_menuItem84 = new wxMenuItem (m_menu5,
												ID_OPTIONS_AREAUNITS_SQFEET,
												wxString (wxT("Sq. feet")),
												wxT("Areas given in square feet"),
												wxITEM_CHECK);
	m_menu5->Append (m_menuItem84);

	wxMenuItem* m_menuItem85;
	m_menuItem85 = new wxMenuItem (m_menu5,
												ID_OPTIONS_AREAUNITS_SQINCHES,
												wxString (wxT("Sq. inches")),
												wxT("Areas given in square inches"),
												wxITEM_CHECK);
	m_menu5->Append (m_menuItem85);

	optionsmenu->Append (-1,
								wxT("Area Units"),
								m_menu5,
								wxT("Specify units that area are to be given in"));

	wxMenuItem* m_menuItem70;
	m_menuItem70 = new wxMenuItem (optionsmenu,
												ID_OPTIONS_SWITCHCROSSCURSOR,
												wxString (wxT("Switch Cross Cursor")),
												wxEmptyString,
												wxITEM_NORMAL);
	optionsmenu->Append (m_menuItem70);

   wxMenuItem* m_menuItem71;
	m_menuItem71 = new wxMenuItem (optionsmenu,
												ID_OPTIONS_SHOWTOOLTIP,
												wxString (wxT("Hide tooltips")),
												wxEmptyString,
												wxITEM_NORMAL);
	optionsmenu->Append (m_menuItem71);
   
	m_menubar1->Append (optionsmenu, wxT("&Options"));

	windowmenu = new wxMenu ();
	/*
	wxMenuItem* m_menuItem86;
	m_menuItem86 = new wxMenuItem (windowmenu,
												wxID_ANY,
												wxString (wxT("&New Window")),
												wxEmptyString,
												wxITEM_NORMAL);
	windowmenu->Append (m_menuItem86);
	
	wxMenuItem* m_menuItem87;
	m_menuItem87 = new wxMenuItem (windowmenu,
												wxID_ANY,
												wxString (wxT("&Cascade")),
												wxEmptyString,
												wxITEM_NORMAL);
	windowmenu->Append (m_menuItem87);

	wxMenuItem* m_menuItem88;
	m_menuItem88 = new wxMenuItem (windowmenu,
												wxID_ANY,
												wxString (wxT("&Tile")),
												wxEmptyString,
												wxITEM_NORMAL);
	windowmenu->Append (m_menuItem88);
	
	wxMenuItem* m_menuItem89;
	m_menuItem89 = new wxMenuItem (windowmenu,
												wxID_ANY,
												wxString (wxT("Arrange Icons")),
												wxEmptyString,
												wxITEM_NORMAL);
	windowmenu->Append (m_menuItem89);

	windowmenu->AppendSeparator ();
	*/
	#if defined multispec_wxlin
		wxMenuItem* m_menuItem91;
		m_menuItem91 = new wxMenuItem (
							windowmenu,
							ID_WINDOW_NEW_SELECTION_GRAPH,
							wxString (wxT("New Selection Window")) + wxT('\t') + wxT("Ctrl+2"),
							wxEmptyString,
							wxITEM_NORMAL);
		windowmenu->Append (m_menuItem91);
	 
		windowmenu->AppendSeparator ();

		wxMenuItem* m_menuItem92;
		m_menuItem92 = new wxMenuItem (windowmenu,
													ID_WINDOW_TEXT_OUTPUT,
													wxString (wxT("Text Output")),
													wxEmptyString,
													wxITEM_CHECK);
		windowmenu->Append (m_menuItem92);

		wxMenuItem* m_menuItem93;
		m_menuItem93 = new wxMenuItem (windowmenu,
													ID_WINDOW_PROJECT,
													wxString (wxT("Project")),
													wxEmptyString,
													wxITEM_CHECK);
		windowmenu->Append (m_menuItem93);
	#endif

	m_menubar1->Append (windowmenu, wxT("&Window"));

	helpmenu = new wxMenu ();
	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem (helpmenu,
											wxID_ABOUT,
											wxString (wxT("About MultiSpec...")),
											wxT("Display MultiSpec Information"),
											wxITEM_NORMAL);
	helpmenu->Append (m_menuItem2);

	m_menubar1->Append (helpmenu, wxT("&Help"));
	
	SetMenuBar (m_menubar1);
	
	#if defined multispec_wxmac
		int menu = m_menubar1->FindMenu (wxT("Window"));
		wxMenu* windowsMenu = m_menubar1->GetMenu (menu);
		
		windowsMenu->AppendSeparator ();

		wxMenuItem* m_menuItem91;
		m_menuItem91 = new wxMenuItem (
							windowmenu,
							ID_WINDOW_NEW_SELECTION_GRAPH,
							wxString (wxT("New Selection Window")) + wxT('\t') + wxT("Ctrl+2"),
							wxEmptyString,
							wxITEM_NORMAL);
		windowsMenu->Append (m_menuItem91);
	#endif
	
	#if defined multispec_wxlin
		m_toolBar1 = CreateToolBar (wxTB_HORIZONTAL, wxID_ANY);
		m_toolBar1->SetToolBitmapSize (wxSize (16, 16));
		m_toolBar1->AddTool (ID_IMAGE_OPEN,
									wxT("tool"),
									openbmp,
									wxNullBitmap,
									wxITEM_NORMAL,
									wxT("Select image file(s)"),
									wxEmptyString);

		m_toolBar1->AddTool (ID_FILE_SAVE_AS,
									wxT("tool"),
									savebmp,
									wxNullBitmap,
									wxITEM_NORMAL,
									wxT("Save (selected) active image file window to disk file"),
									wxEmptyString);
		m_toolBar1->AddSeparator ();
	
		m_toolBar1->AddTool (wxID_CUT,
									wxT("tool"),
									cutbmp,
									wxNullBitmap,
									wxITEM_NORMAL,
									wxEmptyString,
									wxEmptyString);
	
		m_toolBar1->AddTool (wxID_COPY,
									wxT("tool"),
									copybmp,
									wxNullBitmap,
									wxITEM_NORMAL,
									wxEmptyString,
									wxEmptyString);
	
		m_toolBar1->AddTool (wxID_PASTE,
									wxT("tool"),
									pastebmp,
									wxNullBitmap,
									wxITEM_NORMAL,
									wxEmptyString,
									wxEmptyString);
		m_toolBar1->AddSeparator ();
	
		m_toolBar1->AddTool (ID_FILE_PRINT,
									wxT("tool"),
									printbmp,
									wxNullBitmap,
									wxITEM_NORMAL,
									wxEmptyString,
									wxEmptyString);
	
		m_toolBar1->AddTool (wxID_ABOUT,
									wxT("tool"),
									helpbmp,
									wxNullBitmap,
									wxITEM_NORMAL,
									wxT("Display MultiSpec Information"),
									wxEmptyString);
		m_toolBar1->AddSeparator ();
	
				// Using buttons for zoom in, out
	
   	wxBitmapButton* bpButtonX1 = new wxBitmapButton (m_toolBar1,
																			ID_MAGNIFICATION,
																			zoom1,
																			wxDefaultPosition,
																			wxSize (32, 32),
																			wxBORDER_NONE+wxBU_EXACTFIT);
   	bpButtonX1->SetToolTip (wxT("Zoom image to X1.0 magnification"));
		bpButtonX1->Bind (wxEVT_LEFT_DOWN, &CMainFrame::DoZoomToOne, this);
		m_toolBar1->AddControl (bpButtonX1);
	
   	wxBitmapButton* bpButtonZoomIn = new wxBitmapButton (
   																	m_toolBar1,
																		ID_ZOOM_IN,
																		zoomin,
																		wxDefaultPosition,
																		wxSize (32, 32),
																		wxBORDER_NONE+wxBU_EXACTFIT);
   	bpButtonZoomIn->SetToolTip (
   				wxT("Zoom into image. Use shift key to zoom as fast as system can."));
		bpButtonZoomIn->Bind (wxEVT_LEFT_DOWN, &CMainFrame::DoZoomIn, this);
		m_toolBar1->AddControl (bpButtonZoomIn);
	
   	wxBitmapButton* bpButtonZoomOut = new wxBitmapButton (
   																	m_toolBar1,
																		ID_ZOOM_OUT,
																		zoomout,
																		wxDefaultPosition,
																		wxSize (32, 32),
																		wxBORDER_NONE+wxBU_EXACTFIT);
		bpButtonZoomOut->SetToolTip (
   				wxT("Zoom out of image. Use shift key to zoom as fast as system can."));
		bpButtonZoomOut->Bind (wxEVT_LEFT_DOWN, &CMainFrame::DoZoomOut, this);
		m_toolBar1->AddControl (bpButtonZoomOut);

		m_zoomText = new wxStaticText (m_toolBar1,
													ID_ZOOMINFO,
													wxT(""),
													wxDefaultPosition,
													wxSize (50, -1));
		m_toolBar1->AddControl (m_zoomText, wxT("add a fixed text"));
		wxFont currentFont = m_zoomText->GetFont ();
		currentFont.SetPointSize (gFontSize);
		currentFont.SetWeight (wxFONTWEIGHT_LIGHT);
		m_zoomText->SetFont (currentFont);
   	m_zoomText->SetToolTip (wxT("Displays current active image window magnification"));

		m_toolBar1->AddSeparator ();
		m_toolBar1->AddTool (ID_OVERLAY,
									wxT("tool"),
									overlayi,
									wxNullBitmap,
									wxITEM_NORMAL,
									wxT("Control image and vector overlays on active image window"),
									wxEmptyString);
		m_toolBar1->EnableTool (ID_OVERLAY, false);

		m_toolBar1->Realize ();
		Maximize (true);
	#endif	// defined multispec_wxlin
	
	SetFont (*wxSMALL_FONT);

}	// end "CMainFrame"



CMainFrame::~CMainFrame ()

{

}	// end "~CMainFrame"



void CMainFrame::DoZoomIn (
				wxMouseEvent&						event)

{
	if (gActiveImageViewCPtr != NULL)
		{
		SetZoomCode (ID_ZOOM_IN);
		gActiveImageViewCPtr->ZoomIn ();
		
		gActiveImageViewCPtr->m_frame->m_frameMaximized = false;
		
				// Prepare for case when user holds mouse button down.
		
		SetNextControlTime (gControlOffset);
		
		}	// end "if (gActiveImageViewCPtr != NULL)"
	
	event.Skip ();

}	// end "DoZoomIn"



void CMainFrame::DoZoomOut (
				wxMouseEvent&						event)

{
	if (gActiveImageViewCPtr != NULL)
		{
		SetZoomCode (ID_ZOOM_OUT);
		gActiveImageViewCPtr->ZoomOut ();
		
		gActiveImageViewCPtr->m_frame->m_frameMaximized = false;
		
				// Prepare for case when user holds mouse button down.
		
		SetNextControlTime (gControlOffset);
		
		}	// end "if (gActiveImageViewCPtr != NULL)"
	
	event.Skip ();

}	// end "DoZoomOut"



void CMainFrame::DoZoomToOne (
				wxMouseEvent&						event)

{
	if (gActiveImageViewCPtr != NULL)
		{
		SetZoomCode (ID_MAGNIFICATION);
		gActiveImageViewCPtr->ZoomSize ();
		
		gActiveImageViewCPtr->m_frame->m_frameMaximized = false;
		
		SetZoomCode (0);
		
		}	// end "if (gActiveImageViewCPtr != NULL)"
	
	event.Skip ();

}	// end "DoZoomToOne"



Boolean CMainFrame::GetCancelOperationEventFlag ()

{
	return (m_cancelOperationEventFlag);
	
			// Be sure to turn the flag off since it has been used.
	
	m_cancelOperationEventFlag = FALSE;
	
}	// end "GetCancelOperationEventFlag"



Boolean CMainFrame::GetEnableFlagForStatisticsAndCluster ()

{
	Boolean enableFlag = FALSE;

	if (gProjectInfoPtr != NULL)
		enableFlag = TRUE;

	else if (gActiveImageViewCPtr != NULL) 
		{
		Handle windowInfoHandle = GetWindowInfoHandle (gActiveImageViewCPtr);
		WindowInfoPtr windowInfoPtr =
											(WindowInfoPtr)GetHandlePointer (windowInfoHandle);

		if (windowInfoPtr->numberImageFiles == 1 &&
				 (windowInfoPtr->imageType == kMultispectralImageType ||
				 		windowInfoPtr->projectWindowFlag))
			enableFlag = TRUE;

		}	// end "else if (gActiveImageViewCPtr != NULL)"

	return (enableFlag);

}	// end "GetEnableFlagForStatisticsAndCluster"



SInt16 CMainFrame::GetZoomCode (void)

{
    return (m_imageZoomCode);

}	// end "GetZoomCode"



void CMainFrame::OnAbout (
				wxCommandEvent& 					event)

{
	#if defined multispec_wxlin
		CMAbout aboutDialog (this);
	#endif
	#if defined multispec_wxmac
		CMAbout aboutDialog (NULL);
	#endif
	
	aboutDialog.ShowModal ();
	
}	// end "OnAbout"



void CMainFrame::OnAsyncTermination (
				FileUploadProcess*				process)

{
	delete process;
	
}	// end "OnAsyncTermination"



void CMainFrame::OnChar (
				wxKeyEvent& 						event)

{
	if (event.GetEventType() == wxEVT_KEY_DOWN && gProcessorCode != 0)
		{
		if (event.GetKeyCode() == WXK_ESCAPE)
			{
			m_cancelOperationEventFlag = TRUE;
			event.Skip ();
			
			}	// end "if (event.GetKeyCode() == WXK_ESCAPE)"
			
		else if (event.GetKeyCode() == '.' && event.GetModifiers() == wxMOD_CONTROL)
			{
			m_cancelOperationEventFlag = TRUE;
			event.Skip ();
			
			}	// end "if (event.GetKeyCode() == '.' && ..."
		
		}	// end "if (event.GetEventType() == wxEVT_KEY_DOWN)"
	
}	// end "OnChar"



void CMainFrame::OnClose ()

{
	Close ();
		
}	// end "OnClose"



void CMainFrame::OnProcDisplayImage (wxCommandEvent& event)
{
	if (gActiveImageViewCPtr != NULL)
		DisplayImage ();

}	// end "OnProcDisplayImage"



void CMainFrame::OnEditClearAllImageOverlays (
				wxCommandEvent& 					event)

{
	CloseAllImageOverlayFiles ();

	if (gActiveImageViewCPtr != NULL)
		{
		gActiveImageViewCPtr->OnUpdate (NULL, NULL);
		(gActiveImageViewCPtr->m_Canvas)->Update ();
		
		}	// end "if (gActiveImageViewCPtr != NULL)"

	if (gNumberShapeFiles == 0 && gSelectionGraphViewCPtr != NULL)
		{
		if (gActiveImageViewCPtr != NULL)
			gActiveImageViewCPtr->m_Canvas->m_featureListShowFlag = false;
		
		if (gSelectionGraphViewCPtr != NULL)
			{
			gSelectionGraphViewCPtr->m_frame->m_checkBoxFeature->Enable (false);
			gSelectionGraphViewCPtr->m_frame->m_initGraphUpdate = false;
			gSelectionGraphViewCPtr->UpdateShowOrHideFeatureList ();
			
			}	// end "if (gSelectionGraphViewCPtr != NULL)"
		
		}	// end "if (gNumberShapeFiles == 0 && gSelectionGraphViewCPtr != NULL)"
	
}	// end "OnEditClearAllImageOverlays"



void CMainFrame::OnEditClearAllVectorOverlays (
				wxCommandEvent& 					event)

{
	CloseAllVectorOverlayFiles ();

	if (gNumberShapeFiles == 0 && gSelectionGraphViewCPtr != NULL)
		{
		if (gActiveImageViewCPtr != NULL)
			gActiveImageViewCPtr->m_Canvas->m_featureListShowFlag = false;
		
		gSelectionGraphViewCPtr->m_frame->m_checkBoxFeature->Enable (false);
		gSelectionGraphViewCPtr->m_frame->m_initGraphUpdate = false;
		gSelectionGraphViewCPtr->UpdateShowOrHideFeatureList ();
		
		}	// end "if (gNumberShapeFiles == 0 && ..."
	
}	// end "OnEditClearAllVectorOverlays"



void CMainFrame::OnEditClearSelectedOverlay (
				wxCommandEvent& 					event)

{
	SInt32 								selection;

    selection = event.GetId () - ID_CLEAROVERLAYMENUITEMSTART + 1;

    ClearOverlay (selection);

    if (gSelectionGraphViewCPtr != NULL)
		{
		if (gNumberShapeFiles == 0)
			{
         gSelectionGraphViewCPtr->m_frame->m_checkBoxFeature->Enable (false);
         gSelectionGraphViewCPtr->m_frame->m_initGraphUpdate = false;
			
			if (gActiveImageViewCPtr != NULL)
         	gActiveImageViewCPtr->m_Canvas->m_featureListShowFlag = false;
			
			}	// end "if (gNumberShapeFiles == 0)"
		
		gSelectionGraphViewCPtr->UpdateShowOrHideFeatureList ();
		
		}	// end "if (gSelectionGraphViewCPtr != NULL)"
    
}	// end "OnEditClearSelectedOverlay"



void CMainFrame::OnEditClearSelectionRectangle (
				wxCommandEvent& 					event)

{
    ClearSelectionArea (gActiveImageViewCPtr);
    ShowGraphSelection ();

}	// end "OnEditClearSelectionRectangle"



void CMainFrame::OnEditClearTransMatrix (
				wxCommandEvent& 					event)

{
    ClearTransformationMatrix (TRUE);

}	// end "OnEditClearTransMatrix"



void CMainFrame::OnEditCopy (
				wxCommandEvent& 					event)

{
	if (gActiveWindowType == kOutputWindowType)
		gOutputViewCPtr->m_textsw->Copy ();
	
}	// end "OnEditCopy"



void CMainFrame::OnEditCut (
				wxCommandEvent& 					event)

{
	if (gActiveWindowType == kProjectWindowType)
		DoStatisticsWCut ();
	
	else if (gActiveWindowType == kOutputWindowType)
		gOutputViewCPtr->m_textsw->Cut ();
	
}	// end "OnEditCut"



void CMainFrame::OnEditImageDescription (
				wxCommandEvent& 					event)

{
	Boolean 								parameterChangedFlag = FALSE;


	if (gActiveImageViewCPtr != NULL)
		{
		gProcessorCode = kChangeImageDescriptionProcessor;

		FileSpecificationDialog (gActiveImageFileInfoH,
											gActiveImageWindowInfoH,
											&parameterChangedFlag);
		gProcessorCode = 0;

		if (parameterChangedFlag)
			{
			gProcessorCode = kDisplayProcessor;
			DisplayImage ();
			gProcessorCode = 0;

			}	// end "if (parameterChangedFlag)"

		gMemoryTypeNeeded = 0;
		
	}	// end "if (gActiveImageViewCPtr != NULL)"

}	// end "OnEditImageDescription"



void CMainFrame::OnEditImageMapParameters (
				wxCommandEvent& 					event)

{
	CoordinateDialog ();

}	// end "OnEditImageMapParameters"



void CMainFrame::OnEditPaste (
				wxCommandEvent& 					event)

{
	if (gActiveWindowType == kProjectWindowType)
		DoStatisticsWPaste ();
	
	else if (gActiveWindowType == kOutputWindowType)
		gOutputViewCPtr->m_textsw->Paste ();
	
}	// end "OnEditPaste"



void CMainFrame::OnEditSelectAll (
				wxCommandEvent& 					event)

{
	if (gActiveWindowType == kImageWindowType ||
														gActiveWindowType == kThematicWindowType)
		DoEditSelectAllImage (gActiveImageViewCPtr);
    
	else	// gActiveWindowType == kOutputWindowType
    	{
		gOutputViewCPtr->m_textsw->SetSelection (-1, -1);
		gOutputViewCPtr->m_frame->GetDocument()->Activate ();
		
    	}	// end "else gActiveWindowType == kOutputWindowType"

}	// end "OnEditSelectAll"



void CMainFrame::OnEditSelectionRectangle (
				wxCommandEvent& 					event)

{
    EditSelectionDialog (kRectangleType,FALSE);

}	// end "OnEditSelectionRectangle"



void CMainFrame::OnEditUndo (
				wxCommandEvent& 					event)

{
	if (gActiveWindowType == kProjectWindowType)
		DoStatisticsWUndo ();
	
	else if (gActiveWindowType == kOutputWindowType)
		gOutputViewCPtr->m_textsw->Undo ();
	
}	// end "OnEditUndo"



void CMainFrame::OnFileCloseWindow (
				wxCommandEvent& 					event)

{
	if (gActiveWindowType == kImageWindowType ||
													gActiveWindowType == kThematicWindowType)
		{
		if (gActiveImageViewCPtr != NULL)
			{
			CMImageFrame* imageFrameCPtr = gActiveImageViewCPtr->GetImageFrameCPtr ();
			if (imageFrameCPtr != NULL)
				imageFrameCPtr->Close ();
				
			}	// end "if (gActiveImageViewCPtr != NULL)"
			
		}	// end "if (gActiveWindowType == kImageWindowType || ...)"
		
   else if (gActiveWindowType == kProjectWindowType)
   	{
      CMStatisticsFrame* statFrameCPtr = ((CMStatisticsView*)gProjectWindow)->m_frame;
      if (statFrameCPtr != NULL)
         statFrameCPtr->Close ();
		
		}	// end "else if (gActiveWindowType == kProjectWindowType)"
		
	else if (gActiveWindowType == kGraphicsWindowType)
		{
		if (gTheActiveWindow != NULL)
			{
			CMGraphFrame* graphFrameCPtr = ((CMGraphView*)gTheActiveWindow)->m_frame;
			if (graphFrameCPtr != NULL)
				graphFrameCPtr->Close ();
				
			}	// end "if (gTheActiveWindow != NULL)"
			
		}	// end "else if (gActiveWindowType == kGraphicsWindowType)"
	
}	// end "OnFileCloseWindow"



void CMainFrame::OnFileExportFile (
				wxCommandEvent& 					event)

{
	wxFrame* frame = GetActiveFrame ();
	wxFileDialog openFileDialog (frame,
											_("Select File to Export"),
											"",
											"",
											"All files (*.*)|*.*",
											wxFD_OPEN | wxFD_FILE_MUST_EXIST); //| wxSTAY_ON_TOP
	if (openFileDialog.ShowModal () == wxID_CANCEL)
				// the user cancel the opportunity to export a file
																							return;

    wxString filepath = openFileDialog.GetPath ();
    wxString cmd (wxT("exportfile "));
    wxExecute (cmd.Append (filepath), wxEXEC_ASYNC, NULL);
	
}	// end "OnFileExportFile"



void CMainFrame::OnFileLoadTransMatrix (
				wxCommandEvent& 					event)

{
	if (gTransformationMatrix.numberChannels > 0)
		WriteTransformationFile ();

	else	// gTransformationMatrix.numberChannels <= 0
		LoadTransformationFile ();

}	// end "OnFileLoadTransMatrix"



void CMainFrame::OnFileNewProject (
				wxCommandEvent& 					event)

{
	gProcessorCode = kStatisticsProcessor;
	StatisticsControl ();
	gProcessorCode = 0;

}	// end "OnFileNewProject"



void CMainFrame::OnFileOpenProject (
				wxCommandEvent& 					event)

{
    gProcessorCode = kOpenProjectFileProcessor;

    OpenProjectFile (NULL);

    if (gProjectInfoPtr != NULL && gProjectWindow == NULL)
        CreateProjectWindow ();

    gProcessorCode = 0;

}	// end "OnFileOpenProject"



void CMainFrame::OnFileOpenProjectImage (
				wxCommandEvent& 					event)

{
    OpenProjectImageWindow ();

}	// end "OnFileOpenProjectImage"



void CMainFrame::OnFileOpenThematic (
				wxCommandEvent& 					event)

{
    LoadThematicInfo ();

}	// end "OnFileOpenThematic"



void CMainFrame::OnFileSave (
				wxCommandEvent& 					event)

{		
	gProcessorCode = kSaveProcessor;
	WriteThematicInfo (NULL, 0);
		
	gProcessorCode = 0;

}	// end "OnFileSave"



void CMainFrame::OnFileSaveAs (
				wxCommandEvent& 					event)

{
	gProcessorCode = kSaveOutputAsProcessor;
	
	if (gActiveWindowType == kOutputWindowType) 
		{
		wxFrame* frame = GetActiveFrame ();	// GetMainFrame ()
		wxFileDialog dialog (frame,
									wxT("Save Text Output..."),
									wxT(""), wxT("MultiSpec_Text_Output.txt"),
									wxT("*.txt"),
									wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxSTAY_ON_TOP);
		
		if (dialog.ShowModal () == wxID_OK)
			{
			wxString path = dialog.GetPath ();
			gOutputViewCPtr->GetDocument()->OnSaveDocument (path);
			
			}	// end "if (dialog.ShowModal () == wxID_OK)"
			
		gOutputViewCPtr->m_frame->SetTitle (wxT("Text Output"));
		
		}	// end "if (gActiveWindowType == kOutputWindowType)"
	
	else if (gActiveWindowType == kImageWindowType ||
												gActiveWindowType == kThematicWindowType)
		SaveImageWindowAs ();
		
	gProcessorCode = 0;

}	// end "OnFileSaveAs"



void CMainFrame::OnFileSaveProject (
				wxCommandEvent& 					event)

{
    SaveProjectFile (0);

}	// end "OnFileSaveProject"



void CMainFrame::OnFileSaveProjectAs (
				wxCommandEvent& 					event)

{
    SaveProjectFile (1);

}	// end "OnFileSaveProjectAs"



void CMainFrame::OnProcHistogramImage (
				wxCommandEvent& 					event)

{
	if (gActiveImageViewCPtr != NULL) 
		{
		gProcessorCode = kHistogramProcessor;

		if (gActiveImageViewCPtr->m_histogramCPtr != NULL &&
                					gActiveImageViewCPtr->GetImageWindowCPtr() != NULL)
			HistogramControl (kComputeHistogram);

		gMemoryTypeNeeded = 0;
		gProcessorCode = 0;

		}	// end "if (gActiveImageViewCPtr != NULL)"

}	// end "OnProcHistogramImage"



void CMainFrame::OnIdle (
				wxIdleEvent& 						event)
				
{
	if (m_imageZoomCode > 0 && gActiveImageViewCPtr != NULL)
		{
		wxMouseState currentMouseState = wxGetMouseState ();
		if (currentMouseState.LeftIsDown ())
			{
			m_controlDelayFlag = !wxGetKeyState (WXK_SHIFT);
			time_t currentTickCount = GetTickCount ();

			if (currentTickCount >= m_nextControlTime || !m_controlDelayFlag)
				{
				m_nextControlTime = currentTickCount + gControlOffset;

				if (m_imageZoomCode == ID_ZOOM_IN)
					gActiveImageViewCPtr->ZoomIn ();
				
				else if (m_imageZoomCode == ID_ZOOM_OUT)
					gActiveImageViewCPtr->ZoomOut ();
					
				}	// end "if (!m_controlDelayFlag || (GetTickCount() > m_nextControlTime))
				
			event.RequestMore (true);
				
			}	// end "if (currentMouseState.LeftIsDown ())"
			
		else	// quit zooming
			{
			SetZoomCode (0);
			m_nextControlTime = 0;
			
			}	// end "else quit zooming"
			
		}	// end "if (m_imageZoomCode > 0 && ...)"
	
	#if defined multispec_wxlin
		if (m_TOOL_PARAMETER_file_flag)
			{
			m_TOOL_PARAMETER_file_flag = false;
			
					// Check if a file was selected for opening upon startup.
			
			wxString	parameterValue;
			if (wxGetEnv (wxT("TOOL_PARAMETERS"), &parameterValue))
				((CMultiSpecApp*)wxTheApp)->GetUserInputFilePath (parameterValue);
			
			else
				((CMultiSpecApp*)wxTheApp)->GetUserInputFilePath (parameterValue);
			
			}	// end "if (m_TOOL_PARAMETER_file_flag)"
	#endif

}	// end "OnIdle"



void CMainFrame::OnOptionsAreaUnitsAcres (
				wxCommandEvent& 					event)

{
	gDefaultAreaUnits = kAcresUnitsMenuItem;

}	// end "OnOptionsAreaUnitsAcres"



void CMainFrame::OnOptionsAreaUnitsHectares (
				wxCommandEvent& 					event)

{
	gDefaultAreaUnits = kHectareUnitsMenuItem;

}	// end "OnOptionsAreaUnitsHectares"



void CMainFrame::OnOptionsAreaUnitsSqCentimeters (
				wxCommandEvent& 					event)

{
	gDefaultAreaUnits = kSqCentimetersUnitsMenuItem;

}	// end "OnOptionsAreaUnitsSqCentimeters"



void CMainFrame::OnOptionsAreaUnitsSqFeet (
				wxCommandEvent& 					event)

{
	gDefaultAreaUnits = kSqFeetUnitsMenuItem;

}	// end "OnOptionsAreaUnitsSqFeet"



void CMainFrame::OnOptionsAreaUnitsSqInches (
				wxCommandEvent& 					event)

{
	gDefaultAreaUnits = kSqInchesUnitsMenuItem;

}	// end "OnOptionsAreaUnitsSqInches"



void CMainFrame::OnOptionsAreaUnitsSqKilometers (
				wxCommandEvent& 					event)

{
	gDefaultAreaUnits = kSqKilometersUnitsMenuItem;

}	// end "OnOptionsAreaUnitsSqKilometers"



void CMainFrame::OnOptionsAreaUnitsSqMeters (
				wxCommandEvent& 					event)

{
	gDefaultAreaUnits = kSqMetersUnitsMenuItem;

}	// end "OnOptionsAreaUnitsSqMeters"



void CMainFrame::OnOptionsAreaUnitsSqMicrometers (
				wxCommandEvent& 					event)

{
	gDefaultAreaUnits = kSqMicrometersUnitsMenuItem;

}	// end "OnOptionsAreaUnitsSqMicrometers"



void CMainFrame::OnOptionsAreaUnitsSqMiles (
				wxCommandEvent& 					event)

{
	gDefaultAreaUnits = kSqMilesUnitsMenuItem;

}	// end "OnOptionsAreaUnitsSqMiles"



void CMainFrame::OnOptionsAreaUnitsSqMillimeters (
				wxCommandEvent& 					event)

{
	gDefaultAreaUnits = kSqMillimetersUnitsMenuItem;

}	// end "OnOptionsAreaUnitsSqMillimeters"



void CMainFrame::OnOptionsAreaUnitsSqYards (
				wxCommandEvent& 					event)

{
	gDefaultAreaUnits = kSqYardsUnitsMenuItem;

}	// end "OnOptionsAreaUnitsSqYards"



void CMainFrame::OnOptionsShowHideToolTip (
				wxCommandEvent& 					event)

{
   wxToolTip* globalToolTip = new wxToolTip (wxT(""));
   if (m_tooltipFlag)
   	{
      globalToolTip->Enable (false);
      m_tooltipFlag = FALSE;
		
   	}	// end "if (m_tooltipFlag)"
	
	else	// !m_tooltipFlag
		{
      globalToolTip->Enable (true);
      m_tooltipFlag = TRUE;
		
   	}	// end "else !m_tooltipFlag"
	
}	// end "OnOptionsShowHideToolTip"



void CMainFrame::OnOptionsSwitchcrosscursor (
				wxCommandEvent& 					event)

{
	if (gCrossCursorID == IDC_CROSS_CURSOR)
		gCrossCursorID = IDC_CROSS_CURSOR2;

	else	// gCrossCursorID != IDC_CROSS_CURSOR
		gCrossCursorID = IDC_CROSS_CURSOR;
	
}	// end "OnOptionsSwitchcrosscursor"



void CMainFrame::OnToolBarOverlaySelection (
				wxCommandEvent& 					event)

{
	SInt32 								selection;
	bool 									continueFlag = TRUE,
            							shiftKeyFlag;


	selection = event.GetId() - ID_SHOWOVERLAYMENUITEMSTART + 1;

	shiftKeyFlag = false;

	if (m_optionOverlayFlag)
    	{
		continueFlag = OverlayControlDialog (gActiveImageWindowInfoH,
															(SInt16)(selection - 4));

		}	// end "if (m_optionOverlayFlag)"

	if (continueFlag)
		DoShowOverlaySelection (gActiveImageViewCPtr,
										gActiveImageWindowInfoH,
										(SInt16)selection,
										m_optionOverlayFlag,
										shiftKeyFlag);

    m_optionOverlayFlag = FALSE;
	
    if (gActiveImageViewCPtr != NULL)
    	{
    	gActiveImageViewCPtr->m_frame->Refresh ();
    	gActiveImageViewCPtr->m_frame->Update ();
		
    	}	// end "if (gActiveImageViewCPtr != NULL)"

}	// end "OnToolBarOverlaySelection"



void CMainFrame::OnProcClassify (
				wxCommandEvent& 					event)

{
    gProcessorCode = kClassifyProcessor;
    ClassifyControl ();
    gProcessorCode = 0;
    gTextMemoryShortCode = 0;

}	// end "OnProcClassify"



void CMainFrame::OnProcCluster (
				wxCommandEvent& 					event)

{
   if (GetEnableFlagForStatisticsAndCluster ())
		{
		gProcessorCode = kClusterProcessor;

		if (gProjectInfoPtr == NULL)
			OpenNewProject();

		else	// gProjectInfoPtr != NULL
			gProjectInfoPtr->newProjectFlag = FALSE;

		if (gProjectInfoPtr != NULL)
			ClusterControl ();

		gProcessorCode = 0;
		gTextMemoryShortCode = 0;
	
		}	// end "if (GetEnableFlagForStatisticsAndCluster ())"
	
}	// end "OnProcCluster"



void CMainFrame::OnProcEnhanceStatistics (
				wxCommandEvent& 					event)

{
	gMemoryTypeNeeded = 1;
	gProcessorCode = kStatEnhanceProcessor;

	StatisticsEnhanceControl ();

	gProcessorCode = 0;
	gMemoryTypeNeeded = 0;

}	// end "OnProcEnhanceStatistics"



void CMainFrame::OnProcFeatureExtraction (
				wxCommandEvent& 					event)

{
	gMemoryTypeNeeded = 1;
	gProcessorCode = kFeatureExtractionProcessor;

	FeatureExtractionControl ();

	gProcessorCode = 0;
	gMemoryTypeNeeded = 0;

}	// end "OnProcFeatureExtraction"



void CMainFrame::OnProcFeatureSelection (
				wxCommandEvent& 					event)

{
    gMemoryTypeNeeded = 1;
    gProcessorCode = kSeparabilityProcessor;

    SeparabilityControl ();

    gProcessorCode = 0;
    gMemoryTypeNeeded = 0;

}	// end "OnProcFeatureSelection"



void CMainFrame::OnProcListData (
				wxCommandEvent& 					event)

{
	gProcessorCode = kListDataProcessor;
	ListDataControl ();
	gProcessorCode = 0;

}	// end "OnProcListData"



void CMainFrame::OnProcListResults (
				wxCommandEvent& 					event)

{
	if (gActiveImageViewCPtr != NULL &&
					gActiveImageViewCPtr->GetWindowType () == kThematicWindowType)
		{
		gProcessorCode = kListResultsProcessor;
		ListResultsControl ();
		gProcessorCode = 0;
	
		}	// end "if (...->windowType == kThematicWindowType)"

}	// end "OnProcListResults"


/*
void CMainFrame::OnProcReformat (
				wxCommandEvent& 					event)

{
    gProcessorCode = kReformatProcessor;
    ReformatControl (kReformatChangeImageRequest);
    gMemoryTypeNeeded = 0;
    gProcessorCode = 0;

}	// end "OnProcReformat"
*/


void CMainFrame::OnProcReformatChangeImage (
				wxCommandEvent& 					event)

{
	gProcessorCode = kReformatProcessor;
	ReformatControl (kReformatChangeImageRequest);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;

}	// end "OnProcReformatChangeImage"



void CMainFrame::OnProcReformatConvertEnvi (
				wxCommandEvent& 					event)

{
	gProcessorCode = kENVIROItoThematicProcessor;
   ReformatControl (kReformatConvertENVIASCIIRequest);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnProcReformatConvertEnvi"



void CMainFrame::OnProcReformatConvertProject (
				wxCommandEvent& 					event)

{
	gProcessorCode = kReformatProcessor;
	ReformatControl (kReformatConvertProjectRequest);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;

}	// end "OnProcReformatConvertProject"



void CMainFrame::OnProcReformatConvertShape (
				wxCommandEvent& 					event)

{
	gProcessorCode = kReformatProcessor;
	ReformatControl (kReformatConvertShapeRequest);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;

}	// end "OnProcReformatConvertShape"



void CMainFrame::OnProcReformatModifyChannel (
				wxCommandEvent& 					event)

{
	gProcessorCode = kReformatProcessor;
	ReformatControl (kReformatModifyChannelRequest);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;

}	// end "OnProcReformatModifyChannel"



void CMainFrame::OnProcReformatMosaicImages (
				wxCommandEvent& 					event)

{
	gProcessorCode = kReformatProcessor;
	ReformatControl (kReformatMosaicImagesRequest);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnProcReformatMosaicImages"



void CMainFrame::OnProcReformatRecodeThematic (
				wxCommandEvent& 					event)

{
	gProcessorCode = kReformatProcessor;
	ReformatControl (kReformatRecodeThematicRequest);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;

}	// end "OnProcReformatRecodeThematic"



void CMainFrame::OnProcReformatRectifyImage (
				wxCommandEvent& 					event)

{
	gProcessorCode = kReformatProcessor;
	ReformatControl (kReformatRectifyImageRequest);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;

}	// end "OnProcReformatRectifyImage"



void CMainFrame::OnProcStatistics (
				wxCommandEvent& 					event)

{
	if (GetEnableFlagForStatisticsAndCluster ())
		{
		gProcessorCode = kStatisticsProcessor;
		StatisticsControl ();
		gProcessorCode = 0;
		gTextMemoryShortCode = 0;
		
		}	// end "if (GetEnableFlagForStatisticsAndCluster ())"

}	// end "OnProcStatistics"



void CMainFrame::OnProcUtilBiplotsOfData (
				wxCommandEvent&					event)

{
	gProcessorCode = kBiPlotDataProcessor;
	BiPlotDataControl ();

	gProcessorCode = 0;
	gMemoryTypeNeeded = 0;

}	// end "OnProcUtilBiplotsOfData"



void CMainFrame::OnProcUtilCheckCovariances (
				wxCommandEvent& 					event)

{
    gProcessorCode = kCovarianceCheckProcessor;
    EvaluateCovariancesControl ();
    gMemoryTypeNeeded = 0;
    gProcessorCode = 0;

}	// end "OnProcUtilCheckCovariances"



void CMainFrame::OnProcUtilCheckTransMatrix (
				wxCommandEvent& 					event)

{
	gProcessorCode = kTransformCheckProcessor;
	EvaluateTransformationControl ();
	gProcessorCode = 0;

}	// end "OnProcUtilCheckTransMatrix"



void CMainFrame::OnProcUtilCreateStatImage (
				wxCommandEvent& 					event)

{
	gProcessorCode = kStatisticsImageProcessor;
	StatisticsImageControl ();
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnProcUtilCreateStatImage"



void CMainFrame::OnProcUtilListImageDesc (
				wxCommandEvent& 					event)

{
	gProcessorCode = kListDescriptionProcessor;
	ListDescriptionInformation ();
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;

}	// end "OnProcUtilListImageDesc"



void CMainFrame::OnProcUtilPrinCompAnalysis (
				wxCommandEvent& 					event)

{
	if (gProjectInfoPtr != NULL || (gActiveImageViewCPtr != NULL &&
            	gActiveImageViewCPtr->GetImageType() == kMultispectralImageType))
		{
		gProcessorCode = kPrincipalComponentsProcessor;
		PrincipalComponentControl ();
		gProcessorCode = 0;
		
		}	// end if (gProjectInfoPtr != NULL || ...""

}	// end "OnProcUtilPrinCompAnalysis"



void CMainFrame::OnProjAddAsAssociatedImage (
				wxCommandEvent& 					event)

{
    Handle windowInfoHandle = GetWindowInfoHandle (gActiveImageViewCPtr);
    ChangeProjectAssociatedImageItem (windowInfoHandle);

}	// end "OnProjAddAsAssociatedImage"



void CMainFrame::OnProjChangeBaseImageFile (
				wxCommandEvent& 					event)

{
	ChangeProjectBaseImage ();

}	// end "OnProjChangeBaseImageFile"



void CMainFrame::OnProjClearStats (
				wxCommandEvent& 					event)

{
    ProjectMenuClearStatistics ();

}	// end "OnProjClearStats"



void CMainFrame::OnProjUseEnhancedStats (
				wxCommandEvent& 					event)

{
    SetProjectCovarianceStatsToUse (kEnhancedStats);

}	// end "OnProjUseEnhancedStats"



void CMainFrame::OnProjUseLeave1OutStats (
				wxCommandEvent& 					event)

{
    SetProjectCovarianceStatsToUse (kLeaveOneOutStats);

}	// end "OnProjUseLeave1OutStats"



void CMainFrame::OnProjUseOriginalStats (
				wxCommandEvent& 					event)

{
	SetProjectCovarianceStatsToUse (kOriginalStats);

}	// end "OnProjUseOriginalStats"



void CMainFrame::OnToolBarShowOverlay (
				wxCommandEvent& 					event)

{
			// Flag to see if option key is pressed
	
	m_optionOverlayFlag = wxGetKeyState (WXK_SHIFT);
	
	wxMenu* tooloverlay;
	tooloverlay = new wxMenu ();
	wxMenuItem* m_overlaymenu1;
	m_overlaymenu1 = new wxMenuItem (tooloverlay,
												ID_SHOWOVERLAYMENUITEMSTART,
												wxString (wxT("No overlays")),
												wxT(""),
												wxITEM_NORMAL);
	tooloverlay->Append (m_overlaymenu1);
	wxMenuItem* m_overlaymenu2;
	m_overlaymenu2 = new wxMenuItem (tooloverlay,
												ID_SHOWOVERLAYMENUITEMSTART + 1,
												wxString (wxT("All overlays")),
												wxT(""),
												wxITEM_NORMAL);
	tooloverlay->Append (m_overlaymenu2);
	tooloverlay->AppendSeparator ();
	
			// Now go to Overlay routine to add menu items for overlays
	
	SetUpWindowOverlayPopUpMenu ((Handle)tooloverlay, m_optionOverlayFlag);
	#if defined multispec_wxmac
		GetActiveFrame()->GetToolBar()->PopupMenu (tooloverlay);
	#else
		GetToolBar()->PopupMenu (tooloverlay);
	#endif
	
}	// end "OnToolBarShowOverlay"



void CMainFrame::OnUpdateEditClear (
				wxUpdateUIEvent& 					pCmdUI)

{
    pCmdUI.Enable (FALSE);

}	// end "OnUpdateEditClear"



void CMainFrame::OnUpdateEditClearAllVectorOverlays (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (gNumberShapeFiles > 0);

}	// end "OnUpdateEditClearAllVectorOverlays"



void CMainFrame::OnUpdateEditClearAllOverlays (
				wxUpdateUIEvent& 					pCmdUI)

{
    wxObject* overlayMenu = pCmdUI.GetEventObject ();
    pCmdUI.Enable (UpdateEditClearOverlays ((wxUpdateUIEvent*)overlayMenu));
	
}	// end "OnUpdateEditClearAllOverlays"



void CMainFrame::OnUpdateEditClearSelectionRectangle (
				wxUpdateUIEvent& 					event)

{
	SInt16 								selectionTypeCode;
	Boolean 								enableFlag = false;
	
	
	if (gActiveWindowType == kImageWindowType ||
													gActiveWindowType == kThematicWindowType)
		{
				// Update the menu text.
		
		selectionTypeCode = GetSelectionTypeCode (gActiveImageViewCPtr);
		UpdateEditClearSelection (&event, selectionTypeCode);

				// Determine whether menu item is enabled.

		enableFlag = (selectionTypeCode != 0);
		
		}	// end "if (gActiveWindowType == kImageWindowType || ..."

    event.Enable (enableFlag);

}	// end "OnUpdateEditClearSelectionRectangle"



void CMainFrame::OnUpdateEditClearTransMatrix (
				wxUpdateUIEvent& 					pCmdUI)

{
	Boolean enableFlag = FALSE;

	if (gTransformationMatrix.numberChannels > 0)
		enableFlag = TRUE;

	pCmdUI.Enable (enableFlag);

}	// end "OnUpdateEditClearTransMatrix"



void CMainFrame::OnUpdateEditCopy (
				wxUpdateUIEvent& 					pCmdUI)

{
	bool									enableFlag = false,
											defaultTextFlag = true;
	
	
	if (gActiveWindowType == kImageWindowType)
		{
		/* Do not change text until implemented
		Handle windowInfoHandle = GetActiveImageWindowInfoHandle ();

		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle,
																								kLock);

		Boolean enableFlag = UpdateEditImageCopy (&pCmdUI, windowInfoPtr);

		CheckAndUnlockHandle (windowInfoHandle);
		*/
		}	// end "if (gActiveWindowType == kImageWindowType)"
	
	else if (gActiveWindowType == kOutputWindowType)
		{
		long 		from,
					to;
		
		
		pCmdUI.SetText (wxT("&Copy Text\tCtrl+X"));
		defaultTextFlag = false;
		
		gOutputViewCPtr->m_textsw->GetSelection (&from, &to);
		if (from != to)
			enableFlag = true;
	
		}	// end "else if (gActiveWindowType == kOutputWindowType)"
	
	if (defaultTextFlag)
		pCmdUI.SetText (wxT("Copy\tCtrl+Z"));
	
	pCmdUI.Enable (enableFlag);
	
}	// end "OnUpdateEditCopy"



void CMainFrame::OnUpdateEditCut (
				wxUpdateUIEvent& 					pCmdUI)

{	
	bool									enableFlag = false,
											defaultTextFlag = true;
	

	if (gProjectInfoPtr != NULL && gActiveWindowType == kProjectWindowType)
		{
		if (gProjectInfoPtr->statsWindowMode == kClassListMode)
			{                
			pCmdUI.SetText (wxT("&Cut Class\tCtrl+X"));
			defaultTextFlag = false;
			if (gProjectInfoPtr->currentClass >= 0)
				enableFlag = true;
			
			}	// end "if (gProjectInfoPtr->statsWindowMode == kClassListMode)"
			
		else if (gProjectInfoPtr->statsWindowMode == kFieldListMode)
			{                                
			pCmdUI.SetText (wxT("&Cut Field\tCtrl+X"));
			defaultTextFlag = false;
			if (gProjectInfoPtr->currentField >= 0)
				enableFlag = true;
			
			}	// end "if (gProjectInfoPtr->statsWindowMode == kFieldListMode)"
			
		else if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode)
			{                                
					// Determine if this is a polygon field and if so if the number	
					// of points is greater than 3.												
			
			Boolean cutPolygonPointFlag = FALSE;
			if (gProjectInfoPtr->fieldIdentPtr[
							gProjectInfoPtr->currentField].pointType == kPolygonType &&
					gProjectInfoPtr->fieldIdentPtr[
								gProjectInfoPtr->currentField].numberOfPolygonPoints > 3)
				{
				//cell.v = 0;
				//if (LGetSelect (TRUE, &cell, gStatisticsListHandle))
				//	cutPolygonPointFlag = TRUE;
					
				}	// end "if (gProjectInfoPtr->fieldIdentPtr[..."
			
			if (cutPolygonPointFlag)
				{
				pCmdUI.SetText (wxT("&Cut Point\tCtrl+X"));
				defaultTextFlag = false;
				enableFlag = true;
				
				}	// end "if (cutPolygonPointFlag)"
							
			}	// end "if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode)"

		}	// end "if (gActiveWindowType == kProjectWindowType)"
	
	else if (gActiveWindowType == kOutputWindowType)
		{
		long 				from,
							to;
		
		pCmdUI.SetText (wxT("&Cut Text\tCtrl+X"));
		defaultTextFlag = false;
		
		gOutputViewCPtr->m_textsw->GetSelection (&from, &to);
		if (from != to)
			enableFlag = true;
	
		}	// end "else if (gActiveWindowType == kOutputWindowType)"
			
	if (defaultTextFlag)
		pCmdUI.SetText (wxT("&Cut\tCtrl+X"));
		
	pCmdUI.Enable (enableFlag);
	 
}	// end "OnUpdateEditCut"



void CMainFrame::OnUpdateEditImageDescription (
				wxUpdateUIEvent& 					pCmdUI)

{
    pCmdUI.Enable (gActiveImageViewCPtr != NULL);

}	// end "OnUpdateEditImageDescription"



void CMainFrame::OnUpdateEditImageMapParameters (
				wxUpdateUIEvent& 					pCmdUI)

{
    pCmdUI.Enable (UpdateEditImageMapParameters ());

}	// end "OnUpdateEditImageMapParameters"



void CMainFrame::OnUpdateEditPaste (
				wxUpdateUIEvent&  				pCmdUI)

{                                    
	SInt16								classNumber,
											editFieldClassStorage;
						
	bool									enableFlag = false,
											defaultTextFlag = true;


	if (gProjectInfoPtr != NULL && gActiveWindowType == kProjectWindowType)
		{			
		if (gProjectInfoPtr->statsWindowMode == kSelectFieldMode)
			{ 	              
			if (gProjectInfoPtr->editFieldNumber >= 0 &&
															gProjectInfoPtr->currentClass == -1)
				{
				pCmdUI.SetText (wxT("Paste Field\tCtrl+V"));
				defaultTextFlag = false;
				if (gProjectInfoPtr->numberStatisticsClasses < kMaxNumberStatClasses-1)
					enableFlag = true;
				
				}	// end "if (gProjectInfoPtr->editFieldNumber >= 0 && ..."
			
			}	// end "if (gProjectInfoPtr->statsWindowMode == kSelectFieldMode)"
		
		else if (gProjectInfoPtr->statsWindowMode == kClassListMode)
			{                     
			if (gProjectInfoPtr->editFieldNumber >= 0 && 
														gProjectInfoPtr->currentClass >= 0)
				{
				editFieldClassStorage = gProjectInfoPtr->fieldIdentPtr[
								gProjectInfoPtr->editFieldNumber].classStorage;
								
				classNumber = gProjectInfoPtr->
										classNamesPtr[editFieldClassStorage].classNumber - 1;
				
				if (classNumber != gProjectInfoPtr->currentClass)
					{             
					pCmdUI.SetText (wxT("Paste Field\tCtrl+V"));
					defaultTextFlag = false;
					enableFlag = TRUE;
					
					}	// end "if (classNumber != gProjectInfoPtr->currentClass)"
					
				}	// end "if (gProjectInfoPtr->editFieldNumber >= 0 && ..."
				
			if (gProjectInfoPtr->editClassStorageNumber >= 0 && 
														gProjectInfoPtr->currentClass >= 0)
				{                  
				pCmdUI.SetText (wxT("Paste Class Fields\tCtrl+V"));
				defaultTextFlag = false;
				enableFlag = TRUE;                                           
					
				}	// end "if (gProjectInfoPtr->editClassStorageNumber ..."
			
			}	// end "if (gProjectInfoPtr->statsWindowMode == kClassListMode)"
			
		else if (gProjectInfoPtr->statsWindowMode == kFieldListMode)
			{                                       
			if (gProjectInfoPtr->editFieldNumber >= 0)
				{                         
				pCmdUI.SetText (wxT("Paste Field\tCtrl+V"));
				defaultTextFlag = false;
				
				editFieldClassStorage = gProjectInfoPtr->fieldIdentPtr[
												gProjectInfoPtr->editFieldNumber].classStorage;
								
				classNumber = gProjectInfoPtr->
											classNamesPtr[editFieldClassStorage].classNumber - 1;
				
				if (classNumber != gProjectInfoPtr->currentClass)
					enableFlag = TRUE;                              
					
				}	// end "if (gProjectInfoPtr->editFieldNumber >= 0 && ..."
				
			if (gProjectInfoPtr->editClassStorageNumber >= 0)
				{                  
				pCmdUI.SetText (wxT("Paste Class Fields\tCtrl+V"));
				defaultTextFlag = false;
				enableFlag = TRUE;                                           
					
				}	// end "if (gProjectInfoPtr->editClassStorageNumber ..."
			
			}	// end "if (gProjectInfoPtr->statsWindowMode == kFieldListMode)"
			
		}	// end "if (gActiveWindowType == kProjectWindowType)"
	
	else if (gActiveWindowType == kOutputWindowType)
		{
		pCmdUI.SetText (wxT("&Paste Text\tCtrl+X"));
		defaultTextFlag = false;
		
		if (gOutputViewCPtr->m_textsw->CanPaste ())
			enableFlag = true;
	
		}	// end "else if (gActiveWindowType == kOutputWindowType)"
		                       
	if (defaultTextFlag)      
		pCmdUI.SetText (wxT("Paste\tCtrl+V"));
			                                                                                                                                              
	pCmdUI.Enable (enableFlag);
	
}	// end "OnUpdateEditPaste"



void CMainFrame::OnUpdateEditSelectAll (
				wxUpdateUIEvent& 					event)

{
	
	if (gActiveWindowType == kImageWindowType ||
													gActiveWindowType == kThematicWindowType)
		{
		UpdateEditImageSelectAll (&event);

		if (gActiveImageViewCPtr != NULL)
			{
			Boolean rectangularSelectionFlag = GetRectangularSelectionFlag ();

			//event.Enable (gActiveImageViewCPtr->CheckIfOffscreenImageExists () &&
			event.Enable (gActiveImageViewCPtr->ImageWindowIsAvailable () &&
																			rectangularSelectionFlag);
			
			}	// end "if (gActiveImageViewCPtr != NULL)"
		
		}	// end "if (gActiveWindowType == kImageWindowType || ..."
	
	else	// gActiveWindowType == kOutputWindowType
		{
		UpdateEditTextSelectAll (&event);
		event.Enable (false);
      if (gOutputViewCPtr->m_textsw->GetNumberOfLines () > 0 &&
      									gOutputViewCPtr->m_textsw->GetLineLength (0) > 0)
         event.Enable (true);
		
		}	// end "else gActiveWindowType == kOutputWindowType"

}	// end "OnUpdateEditSelectAll"



void CMainFrame::OnUpdateEditSelectionRectangle (
				wxUpdateUIEvent& 					pCmdUI)

{
	if (gActiveWindowType == kImageWindowType ||
													gActiveWindowType == kThematicWindowType)
		pCmdUI.Enable (gActiveImageViewCPtr != NULL);
		
	else
		pCmdUI.Enable (false);

}	// end "OnUpdateEditSelectionRectangle"



void CMainFrame::OnUpdateEditUndo (
				wxUpdateUIEvent&  				pCmdUI)

{
	bool									enableFlag = false,
											defaultTextFlag = true;
	
	
	if (gProjectInfoPtr != NULL && gActiveWindowType == kProjectWindowType)
		{
				// The Undo/Redo Cut Class item will be allowed for only the 'Select
				// Field' and 'Project Class List' stat window modes.
		
		if (gProjectInfoPtr->editClassStorageNumber >= 0 ||
															gProjectInfoPtr->editFieldNumber >= 0)
			{
			if (gProjectInfoPtr->editClassStorageNumber >= 0)
				{
				pCmdUI.SetText ("Undo Cut Class\tCtrl+Z");
				defaultTextFlag = false;
				
				}	// end "if (gProjectInfoPtr->editClassStorageNumber >= 0)"
			
			if (gProjectInfoPtr->editFieldNumber >= 0)
				{
				pCmdUI.SetText ("Undo Cut Field\tCtrl+Z");
				defaultTextFlag = false;
				
				}	// end "if (gProjectInfoPtr->editFieldNumber >= 0)"
			
			if (gProjectInfoPtr->numberStatisticsClasses < kMaxNumberStatClasses-1)
				enableFlag = true;
			
			}	// end "if gProjectInfoPtr->editClassStorageNumber >= 0 || ..."
		
		}	// end "if (gActiveWindowType == kProjectWindowType)"
	
	else if (gActiveWindowType == kOutputWindowType)
		{
		pCmdUI.SetText (wxT("&Undo\tCtrl+X"));
		defaultTextFlag = false;
		
		if (gOutputViewCPtr->m_textsw->CanUndo ())
			enableFlag = true;
	
		}	// end "else if (gActiveWindowType == kOutputWindowType)"
	
	if (defaultTextFlag)
		pCmdUI.SetText (wxT("Undo\tCtrl+Z"));
	
	pCmdUI.Enable (enableFlag);
	
}	// end "OnUpdateEditUndo"



void CMainFrame::OnUpdateFileCloseWindow (
				wxUpdateUIEvent& 					pCmdUI)

{
	if (gActiveWindowType == kImageWindowType ||
														gActiveWindowType == kThematicWindowType)
		{
		UpdateFileImageClose (&pCmdUI);
		pCmdUI.Enable (true);
		
		}	// end "if (gActiveWindowType == kImageWindowType || ..."
		
	else if (gActiveWindowType == kProjectWindowType)
		{
		UpdateFileProjectClose (&pCmdUI);
		pCmdUI.Enable (true);
		
		}	// end "else if (gActiveWindowType == kProjectWindowType)"
		
	else if (gActiveWindowType == kGraphicsWindowType)
		{
		UpdateFileGraphClose (&pCmdUI); 
		pCmdUI.Enable (true);
		
		}	// end "else if (gActiveWindowType == kGraphicsWindowType)"
	
	else
		{
		pCmdUI.SetText (wxT("&Close Window\tCtrl+W"));
		pCmdUI.Enable (false);
		
		}

}	// end "OnUpdateFileCloseWindow"



void CMainFrame::OnUpdateFileLoadTransMatrix (
				wxUpdateUIEvent& 					pCmdUI)

{
    Boolean enableFlag = UpdateFileTransform (&pCmdUI);
	
    		// Set to FALSE for now until implemented.
	
    enableFlag = FALSE;
    pCmdUI.Enable (enableFlag);

}	// end "OnUpdateFileLoadTransMatrix"



void CMainFrame::OnUpdateFileNewProject (
				wxUpdateUIEvent& 					pCmdUI)

{
	Boolean enableFlag = FALSE;

	if (gProjectInfoPtr == NULL)
		enableFlag = GetEnableFlagForStatisticsAndCluster ();

	pCmdUI.Enable (enableFlag);
	
}	// end "OnUpdateFileNewProject"



void CMainFrame::OnUpdateFileOpen (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (gNumberOfIWindows <= kMaxNumberIWindows);

}	// end "OnUpdateFileOpen"



void CMainFrame::OnUpdateFileOpenProject (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (gProjectInfoPtr == NULL);
	
}	// end "OnUpdateFileOpenProject"



void CMainFrame::OnUpdateFileOpenProjectImage (
				wxUpdateUIEvent& 					pCmdUI)

{
    pCmdUI.Enable (gProjectInfoPtr != NULL);

}	// end "OnUpdateFileOpenProjectImage"



void CMainFrame::OnUpdateFileOpenThematic (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (UpdateFileOpenThematicInfo (&pCmdUI));

}	// end "OnUpdateFileOpenThematic"



void CMainFrame::OnUpdateFilePrint (
				CCmdUI& 								pCmdUI)

{
    		// Disable until implemented.
	
    pCmdUI.Enable (false);

}	// end "OnUpdateFilePrint"



void CMainFrame::OnUpdateFilePrintPreview (
				CCmdUI& 								pCmdUI)

{
			// Disable until implemented
	
	pCmdUI.Enable (false);

}	// end "OnUpdateFilePrintPreview"



void CMainFrame::OnUpdateFilePrintSetup (
				CCmdUI& 								pCmdUI)

{
			// Set to false until set up.
	
	pCmdUI.Enable (false);

}	// end "OnUpdateFilePrintSetup"



void CMainFrame::OnUpdateFileSave (
        		wxUpdateUIEvent& 					pCmdUI)

{
    Boolean enableFlag = UpdateFileSave (&pCmdUI);

    pCmdUI.Enable (enableFlag);

}	// end "OnUpdateFileSave"



void CMainFrame::OnUpdateFileSaveAs (
				wxUpdateUIEvent& 					event)

{
	if (gActiveWindowType == kOutputWindowType) 
		{
		UpdateFileOutputTextSaveAs (&event);
	
		if ((gOutputViewCPtr->GetDocument ())->IsModified ())
			event.Enable (true);
		
		else	// document is not modified
			event.Enable (false);
		  
		}	// end "if (gActiveWindowType == kOutputWindowType) "
	
	else if (gActiveWindowType == kImageWindowType ||
														gActiveWindowType == kThematicWindowType)
		{
		Handle windowInfoHandle = GetActiveImageWindowInfoHandle ();

		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
													
		if (UpdateFileImageSaveAs (&event, windowInfoPtr))
			event.Enable (true);
	 		
	 	else	// !UpdateFileImageSaveAs (NULL, windowInfoPtr)
		  event.Enable (false);
		
		}	// end "else if (gActiveWindowType == kImageWindowType || ...
		
	else
		event.Enable (UpdateFileDefaultSaveAs (&event));

}	// end "OnUpdateFileSaveAs"



void CMainFrame::OnUpdateFileSaveProject (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (gProjectInfoPtr != NULL && gProjectInfoPtr->changedFlag);

}	// end "OnUpdateFileSaveProject"


void CMainFrame::OnUpdateFileSaveProjectAs (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (gProjectInfoPtr != NULL);

}	// end "OnUpdateFileSaveProjectAs"



void CMainFrame::OnUpdateOptionsAreaUnitsAcres (
				wxUpdateUIEvent& 					pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kAcresUnitsMenuItem);

}	// end "OnUpdateOptionsAreaUnitsAcres"



void CMainFrame::OnUpdateOptionsAreaUnitsSqCentimeters (
				wxUpdateUIEvent& 					pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqCentimetersUnitsMenuItem);

}	// end "OnUpdateOptionsAreaUnitsSqCentimeters"



void CMainFrame::OnUpdateOptionsAreaUnitsHectares (
				wxUpdateUIEvent& 					pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kHectareUnitsMenuItem);

}	// end "OnUpdateOptionsAreaUnitsHectares"



void CMainFrame::OnUpdateOptionsAreaUnitsSqFeet (
				wxUpdateUIEvent& 					pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqFeetUnitsMenuItem);

}	// end "OnUpdateOptionsAreaUnitsSqFeet"



void CMainFrame::OnUpdateOptionsAreaUnitsSqInches (
        		wxUpdateUIEvent& 					pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqInchesUnitsMenuItem);

}	// end "OnUpdateOptionsAreaUnitsSqInches"



void CMainFrame::OnUpdateOptionsAreaUnitsSqKilometers (
				wxUpdateUIEvent& 					pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqKilometersUnitsMenuItem);

}	// end "OnUpdateOptionsAreaUnitsSqKilometers"



void CMainFrame::OnUpdateOptionsAreaUnitsSqMeters (
				wxUpdateUIEvent& 					pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqMetersUnitsMenuItem);

}	// end "OnUpdateOptionsAreaUnitsSqMeters"



void CMainFrame::OnUpdateOptionsAreaUnitsSqMicrometers (
				wxUpdateUIEvent& 					pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqMicrometersUnitsMenuItem);

}	// end "OnUpdateOptionsAreaUnitsSqMicrometers"



void CMainFrame::OnUpdateOptionsAreaUnitsSqMiles (
				wxUpdateUIEvent& 					pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqMilesUnitsMenuItem);

}	// end "OnUpdateOptionsAreaUnitsSqMiles"



void CMainFrame::OnUpdateOptionsAreaUnitsSqMillimeters (
				wxUpdateUIEvent& 					pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqMillimetersUnitsMenuItem);

}	// end "OnUpdateOptionsAreaUnitsSqMillimeters"



void CMainFrame::OnUpdateOptionsAreaUnitsSqYards (
        		wxUpdateUIEvent& 					pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqYardsUnitsMenuItem);

}	// end "OnUpdateOptionsAreaUnitsSqYards"



void CMainFrame::OnUpdateOptionsMemoryStatus (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (FALSE);

}	// end "OnUpdateOptionsMemoryStatus"



void CMainFrame::OnUpdateOptionsShowToolTip (
				wxUpdateUIEvent& 					pCmdUI)

{
   if (m_tooltipFlag)
		pCmdUI.SetText (wxT("Hide tooltips"));
	
   else	// !m_tooltipFlag
      pCmdUI.SetText (wxT("Show tooltips"));

}	// end "OnUpdateOptionsShowToolTip"



void CMainFrame::OnUpdateOptionsSwitchcrosscursor (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (false);

}	// end "OnUpdateOptionsSwitchcrosscursor"



void CMainFrame::OnUpdateProcClassify (
				wxUpdateUIEvent& 					pCmdUI)

{
    pCmdUI.Enable (gProjectInfoPtr != NULL &&
            									gProjectInfoPtr->numberStatTrainClasses >= 1);

}	// end "OnUpdateProcClassify"



void CMainFrame::OnUpdateProcCluster (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (GetEnableFlagForStatisticsAndCluster ());
	
}	// end "OnUpdateProcCluster"



void CMainFrame::OnUpdateProcDisplayImage (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (gActiveImageViewCPtr != NULL);
	
}	// end "OnUpdateProcDisplayImage"



void CMainFrame::OnUpdateProcEnhanceStatistics (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (gProjectInfoPtr != NULL &&
	        									gProjectInfoPtr->numberStatTrainClasses >= 1);

}	// end "OnUpdateProcEnhanceStatistics"



void CMainFrame::OnUpdateProcFeatureExtraction (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (gProjectInfoPtr != NULL &&
            									gProjectInfoPtr->numberStatTrainClasses >= 2);

}	// end "OnUpdateProcFeatureExtraction"



void CMainFrame::OnUpdateProcFeatureSelection (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (gProjectInfoPtr != NULL &&
													gProjectInfoPtr->numberStatTrainClasses >= 2);

}	// end "OnUpdateProcFeatureSelection"



void CMainFrame::OnUpdateProcHistogramImage (
				wxUpdateUIEvent& 					pCmdUI)

{
    pCmdUI.Enable (gActiveImageViewCPtr != NULL);

}	// end "OnUpdateProcHistogramImage"



void CMainFrame::OnUpdateProcListData (
				wxUpdateUIEvent& 					pCmdUI)

{
    pCmdUI.Enable (gActiveImageViewCPtr != NULL);

}	// end "OnUpdateProcListData"



void CMainFrame::OnUpdateProcListResults (
				wxUpdateUIEvent& 					pCmdUI)

{
	Boolean 								enableFlag = FALSE;
	
			// Note that work needs to be done in the SListResults routines to allow
			// thematic windows for multispectral image type files.
			
	if (gActiveImageViewCPtr != NULL &&
			gActiveImageViewCPtr->GetWindowType() == kThematicWindowType &&
				gActiveImageViewCPtr->GetImageType() == kThematicImageType)
		enableFlag = TRUE;

	pCmdUI.Enable (enableFlag);

}	// end "OnUpdateProcListResults"



void CMainFrame::OnUpdateProcReformat (
				wxUpdateUIEvent& 					pCmdUI)

{
	Boolean 								enableFlag = FALSE;

	if (gActiveImageViewCPtr != NULL)
		enableFlag = TRUE;

	if (gProjectInfoPtr != NULL && gProjectInfoPtr->numberTotalPoints > 0)
		enableFlag = TRUE;

    pCmdUI.Enable (enableFlag);

}	// end "OnUpdateProcReformat"



void CMainFrame::OnUpdateProcReformatChangeImage (
				wxUpdateUIEvent& 					pCmdUI)

{
	Boolean 								enableFlag = FALSE;

	if (gActiveImageWindowInfoH != NULL)
		enableFlag = TRUE;

	pCmdUI.Enable (enableFlag);

}	// end "OnUpdateProcReformatChangeImage"



void CMainFrame::OnUpdateProcReformatConvertEnvi (
				wxUpdateUIEvent& 					pCmdUI)

{
    pCmdUI.Enable (true);
	
}	// end "OnUpdateProcReformatConvertEnvi"



void CMainFrame::OnUpdateProcReformatConvertProject (
				wxUpdateUIEvent& 					pCmdUI)

{
	Boolean enableFlag = FALSE;

	if (gProjectInfoPtr != NULL && gProjectInfoPtr->numberStatisticsClasses > 0)
		enableFlag = TRUE;

	pCmdUI.Enable (enableFlag);

}	// end "OnUpdateProcReformatConvertProject"



void CMainFrame::OnUpdateProcReformatConvertShape (
				wxUpdateUIEvent& 					pCmdUI)

{
	FileInfoPtr 						fileInfoPtr;
	WindowInfoPtr 						windowInfoPtr;

	Handle 								fileInfoHandle;


	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (gActiveImageWindowInfoH, kLock);

	fileInfoHandle = GetFileInfoHandle (windowInfoPtr);
	fileInfoPtr = (FileInfoPtr) GetHandlePointer (fileInfoHandle, kLock);

	pCmdUI.Enable (UpdateReformatConvertShape (windowInfoPtr, fileInfoPtr, &pCmdUI));

	CheckAndUnlockHandle (fileInfoHandle);
	CheckAndUnlockHandle (gActiveImageWindowInfoH);

}	// end "OnUpdateProcReformatConvertShape"



void CMainFrame::OnUpdateProcReformatModifyChannel (
				wxUpdateUIEvent& 					pCmdUI)

{
	FileInfoPtr 						fileInfoPtr;
	WindowInfoPtr 						windowInfoPtr;

	Handle 								fileInfoHandle;


	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (gActiveImageWindowInfoH, kLock);

	fileInfoHandle = GetFileInfoHandle (windowInfoPtr);
	fileInfoPtr = (FileInfoPtr) GetHandlePointer (fileInfoHandle, kLock);

	pCmdUI.Enable (UpdateReformatModifyChannel (windowInfoPtr, fileInfoPtr, &pCmdUI));

	CheckAndUnlockHandle (fileInfoHandle);
	CheckAndUnlockHandle (gActiveImageWindowInfoH);

}	// end "OnUpdateProcReformatModifyChannel"



void CMainFrame::OnUpdateProcReformatMosaicImages (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (UpdateReformatMosaicImages (&pCmdUI));

}	// end "OnUpdateProcReformatMosaicImages"



void CMainFrame::OnUpdateProcReformatRecodeThematic (
				wxUpdateUIEvent& 					pCmdUI)

{
	FileInfoPtr 						fileInfoPtr;
	WindowInfoPtr 						windowInfoPtr;

	Handle 								fileInfoHandle;

	Boolean 								enableFlag = FALSE;


	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (gActiveImageWindowInfoH, kLock);

	fileInfoHandle = GetFileInfoHandle (windowInfoPtr);
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle, kLock);

	if (fileInfoPtr != NULL &&
			fileInfoPtr->thematicType &&
				fileInfoPtr->bandInterleave <= kBNonSQMenuItem &&
					fileInfoPtr->numberBits >= 8)
	  enableFlag = true;

	CheckAndUnlockHandle (fileInfoHandle);
	CheckAndUnlockHandle (gActiveImageWindowInfoH);

	pCmdUI.Enable (enableFlag);

}	// end "OnUpdateProcReformatRecodeThematic"



void CMainFrame::OnUpdateProcReformatRectifyImage (
				wxUpdateUIEvent& 					pCmdUI)

{
	Boolean enableFlag = FALSE;

	if (gActiveImageWindowInfoH != NULL)
		enableFlag = TRUE;

	pCmdUI.Enable (enableFlag);

}	// end "OnUpdateProcReformatRectifyImage"



void CMainFrame::OnUpdateProcStatistics (
				wxUpdateUIEvent& 					pCmdUI)

{
   pCmdUI.Enable (GetEnableFlagForStatisticsAndCluster ());

}	// end "OnUpdateProcStatistics"



void CMainFrame::OnUpdateProcUtilBiplotsOfData (
				wxUpdateUIEvent& 					pCmdUI)

{
	Boolean 								enableFlag = FALSE;
	
	
	Handle windowInfoHandle = GetWindowInfoHandle (gActiveImageViewCPtr);
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
				 
	if (windowInfoPtr != NULL && 
				windowInfoPtr->windowType == kImageWindowType && 
						windowInfoPtr->totalNumberChannels >= 2)
		enableFlag = TRUE;
		
	if (gProjectInfoPtr != NULL && windowInfoPtr == NULL)
		{
		WindowInfoPtr projectWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																	gProjectInfoPtr->windowInfoHandle);
		if (projectWindowInfoPtr->totalNumberChannels >= 2)
			enableFlag = TRUE;
		
		}	// end "if (gProjectInfoPtr != NULL && windowInfoPtr == NULL)"
	
	pCmdUI.Enable (enableFlag);

}	// end "OnUpdateProcUtilBiplotsOfData"



void CMainFrame::OnUpdateProcUtilCheckCovariances (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (gProjectInfoPtr != NULL &&
            									gProjectInfoPtr->numberStatTrainClasses >= 1);

}	// end "OnUpdateProcUtilCheckCovariances"



void CMainFrame::OnUpdateProcUtilCheckTransMatrix (
				wxUpdateUIEvent& 					pCmdUI)

{
	Boolean enableFlag = FALSE;

	if (gTransformationMatrix.numberChannels > 0)
		enableFlag = TRUE;

    pCmdUI.Enable (enableFlag);

}	// end "OnUpdateProcUtilCheckTransMatrix"



void CMainFrame::OnUpdateProcUtilCreateStatImage (
				wxUpdateUIEvent& 					pCmdUI)

{
	Boolean enableFlag = FALSE;
	 
	if (gProjectInfoPtr != NULL || gActiveWindowType == kImageWindowType)
		enableFlag = TRUE;

	pCmdUI.Enable (enableFlag);

}	// end "OnUpdateProcUtilCreateStatImage"



void CMainFrame::OnUpdateProcUtilListImageDesc (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (gActiveImageViewCPtr != NULL);

}	// end "OnUpdateProcUtilListImageDesc"



void CMainFrame::OnUpdateProcUtilPrincipalComponentAnalysis (
				wxUpdateUIEvent& 					pCmdUI)

{
	Boolean enableFlag = FALSE;

	if (gProjectInfoPtr != NULL || (gActiveImageViewCPtr != NULL &&
						gActiveImageViewCPtr->GetImageType() == kMultispectralImageType))
	  enableFlag = TRUE;

	pCmdUI.Enable (enableFlag);

}	// end "OnUpdateProcUtilPrincipalComponentAnalysis"



void CMainFrame::OnUpdateProjectAddAsAssociatedImage (
				wxUpdateUIEvent& 					pCmdUI)

{
	Boolean 								enableFlag = FALSE;


	if (gActiveImageViewCPtr != NULL)
		{
		Handle windowInfoHandle = GetWindowInfoHandle (gActiveImageViewCPtr);
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

		if (windowInfoPtr != NULL)
			{
			if (windowInfoPtr->projectBaseImageFlag)
				pCmdUI.SetText ("Base Image");

			else	// !projectBaseImageFlag
				{
				if (windowInfoPtr->projectWindowFlag)
					pCmdUI.SetText ("Remove As Associated Image");

				else	// !windowInfoPtr->projectWindowFlag
					pCmdUI.SetText ("Add As Associated Image");

				enableFlag = (gProjectInfoPtr != NULL);

				}	// end "else !projectBaseImageFlag"

			}	// end "if (windowInfoPtr != NULL)"

		}	// end "if (gActiveImageViewCPtr != NULL)"

	pCmdUI.Enable (enableFlag);

}	// end "OnUpdateProjectAddAsAssociatedImage"



void CMainFrame::OnUpdateProjectChangeBaseImageFile (
				wxUpdateUIEvent& 					pCmdUI)

{
    pCmdUI.Enable (gProjectInfoPtr != NULL);

}	// end "OnUpdateProjectChangeBaseImageFile"



void CMainFrame::OnUpdateProjectClearStats (
				wxUpdateUIEvent& 					pCmdUI)

{
    pCmdUI.Enable (gProjectInfoPtr != NULL && gProjectInfoPtr->statsLoaded);

}	// end "OnUpdateProjectClearStats"



void CMainFrame::OnUpdateProjectMixofstatisticsused (
				wxUpdateUIEvent& 					pCmdUI)

{
	SInt16 								checkCode = 0;


	if (gProjectInfoPtr != NULL)
		{
		if (gProjectInfoPtr->covarianceStatsToUse == kMixedStats)
			checkCode = 1;

    	}	// end "if (gProjectInfoPtr != NULL)"

	pCmdUI.Check (checkCode);
	pCmdUI.Enable (FALSE);

}	// end "OnUpdateProjectMixofstatisticsused"



void CMainFrame::OnUpdateProjectUseEnhancedStats (
				wxUpdateUIEvent& 					pCmdUI)

{
	SInt16 								checkCode = 0;
	Boolean 								enableFlag = FALSE;


	if (gProjectInfoPtr != NULL)
		{
		if (gProjectInfoPtr->enhancedStatsExistFlag)
			enableFlag = TRUE;

		if (gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats)
			checkCode = 1;

		}	// end "if (gProjectInfoPtr != NULL)"

	pCmdUI.Check (checkCode);
	pCmdUI.Enable (enableFlag);

}	// end "OnUpdateProjectUseEnhancedStats"



void CMainFrame::OnUpdateProjectUseLeave1OutStatistics (
				wxUpdateUIEvent& 					pCmdUI)

{
	SInt16 								checkCode = 0;
	Boolean 								enableFlag = FALSE;


	if (gProjectInfoPtr != NULL)
		{
		enableFlag = TRUE;

		if (gProjectInfoPtr->covarianceStatsToUse == kLeaveOneOutStats)
			checkCode = 1;

		}	// end "if (gProjectInfoPtr != NULL)"

	pCmdUI.Check (checkCode);
	pCmdUI.Enable (enableFlag);

}	// end "OnUpdateProjectUseLeave1OutStatistics"



void CMainFrame::OnUpdateProjectUseOriginalStats (
				wxUpdateUIEvent& 					pCmdUI)

{
	SInt16 								checkCode = 0;
	Boolean 								enableFlag = FALSE;


	if (gProjectInfoPtr != NULL)
		{
		enableFlag = TRUE;

		if (gProjectInfoPtr->covarianceStatsToUse == kOriginalStats)
			checkCode = 1;

		}	// end "if (gProjectInfoPtr != NULL)"

	pCmdUI.Check (checkCode);
	pCmdUI.Enable (enableFlag);

}	// end "OnUpdateProjectUseOriginalStats"



void CMainFrame::OnUpdateToolBarMagnification (
				wxUpdateUIEvent& 					event)

{
	double magnification = -1;
	Boolean enableFlag = FALSE;

	if (gActiveImageViewCPtr != NULL &&
									gActiveImageViewCPtr->ImageWindowIsAvailable ())
		{
		CMDisplay* displayMultiCPtr = gActiveImageViewCPtr->m_displayMultiCPtr;
		magnification = displayMultiCPtr->GetMagnification ();
		if (displayMultiCPtr->GetMagnification () != 1.0)
			enableFlag = TRUE;

		}	// end "if (...->ImageWindowIsAvailable ())"

	event.Enable (enableFlag);

}	// end "OnUpdateToolBarMagnification"



void CMainFrame::OnUpdateToolBarOverlay (
				wxUpdateUIEvent&					event)

{
	Handle*								shapeHandlePtr;
   ShapeInfoPtr						shapeInfoPtr = NULL;
	WindowInfoPtr						windowInfoPtr;
	
	Handle								windowInfoHandle;
   SInt16								shapeFileIndex;
	Boolean								enableFlag = FALSE;
	
	
	windowInfoHandle = GetWindowInfoHandle (gActiveImageViewCPtr);
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

	if (windowInfoPtr != NULL && (windowInfoPtr->numberVectorOverlays > 0 ||
														windowInfoPtr->numberImageOverlays > 0))
		{
		enableFlag = TRUE;

		shapeHandlePtr = (Handle*)GetHandlePointer (gShapeFilesHandle);
		shapeFileIndex = abs (windowInfoPtr->overlayList[0].index) - 1;

		if (shapeHandlePtr != NULL && windowInfoPtr->numberVectorOverlays > 0)
			shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (
													shapeHandlePtr[shapeFileIndex], kLock);

		if (shapeInfoPtr != NULL &&
				shapeInfoPtr->dbfInfoPtr != NULL &&
						gSelectionGraphViewCPtr != NULL)
			{
			if (gSelectionGraphViewCPtr->m_frame->m_initGraphUpdate == false)
				{
				gSelectionGraphViewCPtr->m_frame->m_checkBoxFeature->Enable (true);
				gSelectionGraphViewCPtr->m_frame->m_initGraphUpdate = true;
				shapeInfoPtr->expandFeatureList = true;
				
				}	// end "if (...->m_frame->m_initGraphUpdate == false)"
			
			}	// end "if (shapeInfoPtr != NULL && ..."
		
      }	// end "if (windowInfoPtr != NULL && ..."
	
	else	// windowInfoPtr == NULL || ...
		{
		if (gSelectionGraphViewCPtr != NULL)
			gSelectionGraphViewCPtr->m_frame->m_initGraphUpdate = false;
		
      }	// end "else windowInfoPtr == NULL || ..."

	event.Enable (enableFlag);
	
}	// end "OnUpdateToolBarOverlay"



void CMainFrame::OnUpdateToolBarZoomIn (
				wxUpdateUIEvent& 					event)

{
	Boolean enableFlag = FALSE;	

	if (gActiveImageViewCPtr != NULL &&
										gActiveImageViewCPtr->ImageWindowIsAvailable ())
		{
		CMDisplay* displayMultiCPtr = gActiveImageViewCPtr->m_displayMultiCPtr;
		if (displayMultiCPtr->GetMagnification () <
                									displayMultiCPtr->GetMaxMagnification ())
			enableFlag = TRUE;

		}	// end "if (gActiveImageViewCPtr != NULL && ..."

	event.Enable (enableFlag);

}	// end "OnUpdateToolBarZoomIn"



void CMainFrame::OnUpdateToolBarZoomInfo (
				wxUpdateUIEvent&					event)

{
	if (m_toolBar1 != NULL)
		{
		if (gActiveImageViewCPtr != NULL &&
								!gActiveImageViewCPtr->ImageWindowIsAvailable ())
			{
			wxStaticText* m_zoomtext = (wxStaticText*)FindWindow (ID_ZOOMINFO);
			
			m_zoomtext->SetLabel ("");
			
			}	// end "if (gActiveImageViewCPtr != NULL && ..."
		
		}	// end "if (m_toolBar1 != NULL)"
	
}	// end "OnUpdateToolBarZoomInfo"



void CMainFrame::OnUpdateToolBarZoomOut (
				wxUpdateUIEvent& 					event)

{
	Boolean enableFlag = FALSE;

	if (gActiveImageViewCPtr != NULL &&
									gActiveImageViewCPtr->ImageWindowIsAvailable ())
		{
		CMDisplay* displayMultiCPtr = gActiveImageViewCPtr->m_displayMultiCPtr;
		if (displayMultiCPtr->GetMagnification () > gMinMagnification)
			enableFlag = TRUE;

		}	// end "if (gActiveImageViewCPtr != NULL && ..."

	event.Enable (enableFlag);

}	// end "OnUpdateToolBarZoomOut"



void CMainFrame::OnUpdateViewToolBar (
				wxUpdateUIEvent& 					pCmdUI)

{
    pCmdUI.Enable (FALSE);

}	// end "OnUpdateViewToolBar"



void CMainFrame::OnUpdateViewStatusBar (
				wxUpdateUIEvent& 					pCmdUI)

{
    pCmdUI.Enable (FALSE);

}	// end "OnUpdateViewStatusBar"



void CMainFrame::OnUpdateViewCoordinatesBar (
				wxUpdateUIEvent& 					event)

{
	if (gActiveImageViewCPtr != NULL &&
					(gActiveWindowType == kImageWindowType ||
								gActiveWindowType == kThematicWindowType))
		{
		bool m_displayCoordinatesFlag = 
				((CMImageDoc*)gActiveImageViewCPtr->GetDocument())->GetCoordinateFlag ();
		event.Enable (true);
		event.Check (m_displayCoordinatesFlag);
		
		}	// end "if (gActiveWindowType == kImageWindowType || ..."
	
	else	// active window not image window type
		{
		event.Enable (false);
		event.Check (false);
		
		}	// end "else active window not image window type"

}	// end "OnUpdateViewCoordinatesBar"



void CMainFrame::OnUpdateWindowNewSelectionGraph (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (UpdateWindowSelectionGraph ());

}	// end "OnUpdateWindowNewSelectionGraph"



void CMainFrame::OnUpdateWindowProject (
				wxUpdateUIEvent& 					pCmdUI)

{
	Boolean enableFlag = (gProjectInfoPtr != NULL);
	
	pCmdUI.Enable (enableFlag);
	
	if ((CMImageView*)gTheActiveWindow == gProjectWindow)
		pCmdUI.Check (true);
	else
		pCmdUI.Check (false);
	
			// Also add image and graph windows
	
	UpdateWindowMenuList ();

}	// end "OnUpdateWindowProject"



void CMainFrame::OnUpdateWindowTextOutput (
				wxUpdateUIEvent& 					pCmdUI)

{
	pCmdUI.Enable (true);

	if ((CMTextView*)gTheActiveWindow == gOutputViewCPtr)
		pCmdUI.Check (true);
	
	else
		pCmdUI.Check (false);
		
}	// end "OnUpdateWindowTextOutput"



void CMainFrame::OnViewCoordinatesBar (
				wxCommandEvent& 					event)

{
			// Toggle the coordinates bar
	
	if (gActiveImageViewCPtr != NULL)
		gActiveImageViewCPtr->m_frame->ShowCoordinateView (2);

}	// end "OnViewCoordinatesBar"



void CMainFrame::OnWindowSelectionNewGraph (
				wxCommandEvent& 					event)

{
	gProcessorCode = kSelectionGraphProcessor;
	
   ((CMultiSpecApp*)wxTheApp)->ActivateGraphView ();
	
	gProcessorCode = 0;
	
}	// end "OnWindowSelectionNewGraph"



void CMainFrame::OnWindowShowCoordinateView (
				wxCommandEvent& 					event)

{
			// Toggle the coordinates bar

	if (gActiveImageViewCPtr != NULL)
		gActiveImageViewCPtr->m_frame->ShowCoordinateView (2);

}	// end "OnWindowShowCoordinateView"



void CMainFrame::OnWindowOutputWindowSelection (
				wxCommandEvent& 					event)

{
	if ((CMTextView*)gTheActiveWindow != gOutputViewCPtr)
		gOutputViewCPtr->GetDocument()->Activate ();

}	// end "OnWindowWindowSelection"



void CMainFrame::OnWindowProjectWindowSelection (
				wxCommandEvent& 					event)

{
	if ((CMImageView*)gTheActiveWindow != gProjectWindow)
		((CMStatisticsView*)gProjectWindow)->GetDocument()->Activate ();

}	// end "OnWindowProjectWindowSelection"



void CMainFrame::OnWindowIorGWindowSelection (
				wxCommandEvent& 					event)

{
	CMImageView*						windowPtr;
	SInt32								selection;
	

	selection = event.GetId () - ID_WINDOW_MENUITEMSTART;
	 
	windowPtr = gWindowList[kNumberBaseWindows+selection];

	windowPtr->GetDocument()->Activate ();

}	// end "OnWindowWindowSelection"



void CMainFrame::OnZoomInMouseDown (
				wxMouseEvent& 						event)

{

}	// end "OnZoomInMouseDown"



void CMainFrame::SetNextControlTime (
				UInt32 								offset)

{
	m_nextControlTime = GetTickCount () + offset;

}	// end "SetNextControlTime"



void CMainFrame::SetToolParametersFlag (
				Boolean								toolParameterFileFlag)

{
	m_TOOL_PARAMETER_file_flag = toolParameterFileFlag;
	
	#if defined multispec_wxmac
		m_TOOL_PARAMETER_file_flag = FALSE;
	#endif
		
}	// end "SetToolParametersFlag"



void CMainFrame::SetUpdateAreaUnits (
				wxUpdateUIEvent& 					pCmdUI,
				SInt16 								unitsCode)

{
	SInt16 checkCode = 0;

	if (gDefaultAreaUnits == unitsCode)
		checkCode = 1;

	pCmdUI.Check (checkCode);
	pCmdUI.Enable (TRUE);

}	// end "OnUpdateAreaUnits"



void CMainFrame::SetZoomCode (
				SInt16 								zoomCode)

{
	m_imageZoomCode = zoomCode;

	if (m_imageZoomCode != 0)
		{
		m_controlDelayFlag = TRUE;
		
				// Currently for linux, if user holds the ctrl key down and holds the
				// zoom button down, zoom will continue for 1/10 increments. Have not
				// found a way to handle for left clicking on the button. Only event
				// returned is when mouse button goes up after a mouse down.
		
		if (wxGetKeyState (WXK_SHIFT))
			m_controlDelayFlag = FALSE;
		
		}	// end "else m_imageZoomCode != 0"

}	// end "SetZoomCode"



void CMainFrame::UpdateStatusBar (
				CMImageFrame* 						imageFrame,
				double 								magnification)

{
	char 									tempString[32];
	wxStaticText* 						zoomText = NULL;

	SInt16 								fieldPrecision;


	if (magnification > 0)
		{
		fieldPrecision = 1;
		if (magnification < 1)
			fieldPrecision = 3;

		sprintf (tempString, "x%4.*f", fieldPrecision, magnification);
		
		}	// end "if (magnification > 0)"

	else	// magnification <= 0
		sprintf (tempString, "");

			// Modify the zoom information at status bar to the toolbar

	wxString zoomStatusText (tempString, wxConvUTF8);

	#if defined multispec_wxmac
		if (imageFrame != NULL)
			zoomText = imageFrame->m_zoomText;
	#else
		zoomText = m_zoomText;
	#endif

	if (zoomText != NULL)
		zoomText->SetLabel (zoomStatusText);
	
}	// end "UpdateStatusBar"



void CMainFrame::UpdateWindowMenuList ()

{
	unsigned char						titleString[256];
	wxMenuItem*							menuItemPtr;
	WindowPtr							window;
	
	int									i,
											menuItem;
	

			// First make sure the image and graph window references are removed.
			
	ClearMenuItems (windowmenu, 6);
   
   if (gNumberOfIWindows+gNumberOfGWindows > 0)
		{
      menuItem = ID_WINDOW_MENUITEMSTART;
      for (i =0; i<gNumberOfIWindows; i++)
      	{
         window = gWindowList[kNumberBaseWindows+i];
         GetWindowTitle (window, titleString);
         
         menuItemPtr = windowmenu->Append (
													menuItem,
													wxString::FromAscii ((char*)&titleString[1]),
													wxEmptyString,
													wxITEM_CHECK);
										
			if (window == gActiveImageViewCPtr && window == gTheActiveWindow)
				menuItemPtr->Check (true);
				
			menuItem++;
			
			}	// end "for (i =0; i<gNumberOfIWindows; i++)"

      for (i=gNumberOfIWindows; i<gNumberOfIWindows+gNumberOfGWindows; i++)
      	{
         window = gWindowList[kNumberBaseWindows+i];
         GetGraphWindowTitle (window, titleString);
            
         menuItemPtr = windowmenu->Append (
         										menuItem,
													wxString::FromAscii ((char*)&titleString[1]),
													wxEmptyString,
													wxITEM_CHECK);
										
         if (window == gTheActiveWindow)
            menuItemPtr->Check (true);
				
         menuItem++;
			
			}	// end "for (i=gNumberOfIWindows; i<..."

		}	// end "if (gNumberOfIWindows+gNumberOfGWindows > 0)"
   
}	// end "UpdateWindowMenuList"
