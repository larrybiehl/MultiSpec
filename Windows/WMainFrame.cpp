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
//	File:						WMainFrame.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			03/07/2020
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains the implementation of the CMainFrame
//								class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SDisplay_class.h"
#include "SHistogram_class.h"
#include "SImageWindow_class.h"

#include "WDrawObjects.h"
#include "WFileFormatDialog.h"
#include "WImageDoc.h"
#include "WImageFrame.h"
#include "WImageView.h"
#include "WMainFrame.h"
#include "WMultiSpec.h"
#include "WOpenFileDialog.h"
#include "WStatisticsView.h" 

extern UInt16							gDisplayBitsPerPixel;

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNAMIC (CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP (CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP (CMainFrame)
		// In order of location on the main menu
				// File Menu
	ON_COMMAND (ID_FILE_NEW_PROJECT, OnFileNewProject)
	ON_COMMAND (ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND (ID_FILE_OPEN_PROJECT, OnFileOpenProject)
	ON_COMMAND (ID_FILE_OPEN_PROJECT_IMAGE, OnFileOpenProjectImage)
	ON_COMMAND (ID_FILE_OPEN_THEMATIC, OnFileOpenThematic)
	ON_COMMAND (ID_FILE_LOAD_TRANS_MATRIX, OnFileLoadTransMatrix)
	ON_COMMAND (ID_FILE_SAVE_PROJECT, OnFileSaveProject)
	ON_COMMAND (ID_FILE_SAVE_PROJECT_AS, OnFileSaveProjectAs)

				// Edit Menu
	ON_COMMAND (ID_EDIT_CLEAR_TRANS_MATRIX, OnEditClearTransMatrix)
	ON_COMMAND (ID_EDIT_IMAGE_DESCRIPTION, OnEditImageDescription)
	ON_COMMAND (ID_EDIT_IMAGE_MAP_PARAMETERS, OnEditImageMapParameters)
	ON_COMMAND (ID_EDIT_CLEAR_ALL_IMAGE_OVERLAYS, OnEditClearAllImageOverlays)
	ON_COMMAND (ID_EDIT_CLEAR_ALL_VECTOR_OVERLAYS, OnEditClearAllVectorOverlays)

				// View Menu Items
	ON_COMMAND (ID_WINDOW_SHOW_COORDINATE_VIEW, OnWindowShowCoordinateView)

				// Project Menu Items
	ON_COMMAND (ID_PROJ_USE_ORIGINAL_STATS, OnProjUseOriginalStats)
	ON_COMMAND (ID_PROJ_USELEAVE1OUTSTATISTICS, OnProjUseleave1outstatistics)
	ON_COMMAND (ID_PROJ_USE_ENHANCED_STATS, OnProjUseEnhancedStats)
	ON_COMMAND (ID_PROJ_CLEAR_STATS, OnProjClearStats)
	ON_COMMAND (ID_PROJ_CHANGE_BASE_IMAGE_FILE, OnProjChangeBaseImageFile)
	ON_COMMAND (ID_PROJ_ADD_AS_ASSOCIATED_IMAGE, OnProjAddAsAssociatedImage)

				// Processor Menu Items
	ON_COMMAND (ID_PROC_DISPLAY_IMAGE, OnProcDisplayImage)
	ON_COMMAND (ID_PROC_HISTOGRAM_IMAGE, OnProcHistogramImage)
	ON_COMMAND (ID_PROC_LISTDATA, OnProcListData)
	ON_COMMAND (ID_PROC_REFORMAT, OnProcReformat)
	ON_COMMAND (ID_PROC_CLUSTER, OnProcCluster)
	ON_COMMAND (ID_PROC_STATISTICS, OnProcStatistics)
	ON_COMMAND (ID_PROC_ENHANCE_STATISTICS, OnProcEnhanceStatistics)
	ON_COMMAND (ID_PROC_FEATURE_EXTRACTION, OnProcFeatureExtraction)
	ON_COMMAND (ID_PROC_FEATURE_SELECTION, OnProcFeatureSelection)
	ON_COMMAND (ID_PROC_CLASSIFY, OnProcClassify)
	ON_COMMAND (ID_PROC_LISTRESULTS, OnProcListResults)

				// Processor->Reformat Menu Items
	ON_COMMAND (ID_PROC_REFORMAT_CHANGE_HEADER, OnProcReformatChangeHeader)
	ON_COMMAND (ID_PROC_REFORMAT_CHANGE_IMAGE, OnProcReformatChangeImage)
	ON_COMMAND (ID_PROC_REFORMAT_CONVERT_MULTISPECTRAL, OnProcReformatConvertMultispectral)
	ON_COMMAND (ID_PROC_REFORMAT_CONVERT_PROJECT, OnProcReformatConvertProject)
	ON_COMMAND (ID_PROC_REFORMAT_CONVERT_SHAPE, OnProcReformatConvertShape)
	ON_COMMAND (ID_PROC_REFORMAT_MODIFY_CHANNEL, OnProcReformatModifyChannel)
	ON_COMMAND (ID_PROC_REFORMAT_MOSAIC_IMAGES, OnProcReformatMosaicImages)
	ON_COMMAND (ID_PROC_REFORMAT_RECODE_THEMATIC, OnProcReformatRecodeThematic)
	ON_COMMAND (ID_PROC_REFORMAT_RECTIFY_IMAGE, OnProcReformatRectifyImage)
	ON_COMMAND (ID_PROC_REFORMAT_CONVERT_ENVI, OnProcReformatConvertEnvi)

				// Processor->Utility Menu Items
	ON_COMMAND (ID_PROC_UTIL_PRIN_COMP_ANALYSIS, OnProcUtilPrinCompAnalysis)
	ON_COMMAND (ID_PROC_UTIL_CREATE_STAT_IMAGE, OnProcUtilCreateStatImage)
	ON_COMMAND (ID_PROC_UTIL_BIPLOTS_OF_DATA, OnProcUtilBiplotsOfData)
	ON_COMMAND (ID_PROC_UTIL_LIST_IMAGE_DESC, OnProcUtilListImageDesc)
	ON_COMMAND (ID_PROC_UTIL_CHECK_COVARIANCES, OnProcUtilCheckCovariances)
	ON_COMMAND (ID_PROC_UTIL_CHECKTRANS_MATRIX, OnProcUtilCheckTransMatrix)

				// Options Menu Items
	ON_COMMAND (ID_OPTIONS_AREAUNITS_SQKILOMETERS, OnOptionsAreaUnitsSqKilometers)
	ON_COMMAND (ID_OPTIONS_AREAUNITS_HECTARES, OnOptionsAreaUnitsHectares)
	ON_COMMAND (ID_OPTIONS_AREAUNITS_SQMETERS, OnOptionsAreaUnitsSqMeters)
	ON_COMMAND (ID_OPTIONS_AREAUNITS_SQCENTIMETERS, OnOptionsAreaUnitsSqCentimeters)
	ON_COMMAND (ID_OPTIONS_AREAUNITS_SQMILLIMETERS, OnOptionsAreaUnitsSqMillimeters)
	ON_COMMAND (ID_OPTIONS_AREAUNITS_SQMICROMETERS, OnOptionsAreaUnitsSqMicrometers)
	ON_COMMAND (ID_OPTIONS_AREAUNITS_SQMILES, OnOptionsAreaUnitsSqMiles)
	ON_COMMAND (ID_OPTIONS_AREAUNITS_ACRES, OnOptionsAreaUnitsAcres)
	ON_COMMAND (ID_OPTIONS_AREAUNITS_SQYARDS, OnOptionsAreaUnitsSqYards)
	ON_COMMAND (ID_OPTIONS_AREAUNITS_SQFEET, OnOptionsAreaUnitsSqFeet)
	ON_COMMAND (ID_OPTIONS_AREAUNITS_SQINCHES, OnOptionsAreaUnitsSqInches)
	ON_COMMAND (ID_OPTIONS_SWITCHCROSSCURSOR, OnOptionsSwitchcrosscursor)

				// Window Menu Items
	ON_COMMAND (ID_WINDOW_NEW_SELECTION_GRAPH, OnWindowNewSelectionGraph)

		// Tool Bar Control Items
	ON_UPDATE_COMMAND_UI (ID_MAGNIFICATION, OnUpdateToolBarMagnification)
	ON_UPDATE_COMMAND_UI (ID_ZOOM_IN, OnUpdateToolBarZoomIn)
	ON_UPDATE_COMMAND_UI (ID_INDICATOR_ZOOM, OnUpdateToolBarZoomIndicator)
	ON_UPDATE_COMMAND_UI (ID_ZOOM_OUT, OnUpdateToolBarZoomOut)

		// In order of location on the main menu
				// File Menu Items
	ON_UPDATE_COMMAND_UI (ID_FILE_NEW_PROJECT, OnUpdateFileNewProject)
	ON_UPDATE_COMMAND_UI (ID_FILE_OPEN, OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI (ID_FILE_OPEN_PROJECT, OnUpdateFileOpenProject)
	ON_UPDATE_COMMAND_UI (ID_FILE_OPEN_PROJECT_IMAGE, OnUpdateFileOpenProjectImage)
	ON_UPDATE_COMMAND_UI (ID_FILE_OPEN_THEMATIC, OnUpdateFileOpenThematic)
	ON_UPDATE_COMMAND_UI (ID_FILE_LOAD_TRANS_MATRIX, OnUpdateFileLoadTransMatrix)
	ON_UPDATE_COMMAND_UI (ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI (ID_FILE_SAVE_PROJECT, OnUpdateFileSaveProject)
	ON_UPDATE_COMMAND_UI (ID_FILE_SAVE_PROJECT_AS, OnUpdateFileSaveProjectAs)
	ON_UPDATE_COMMAND_UI (ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)

				// Edit Menu Items
	ON_UPDATE_COMMAND_UI (ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI (ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI (ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI (ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI (ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_UPDATE_COMMAND_UI (ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_UPDATE_COMMAND_UI (ID_EDIT_SELECTION_RECTANGLE, OnUpdateEditSelectionRectangle)
	ON_UPDATE_COMMAND_UI (ID_EDIT_CLEAR_SELECT_RECTANGLE, OnUpdateEditClearSelectionRectangle)
	ON_UPDATE_COMMAND_UI (ID_EDIT_CLEAR_TRANS_MATRIX, OnUpdateEditClearTransMatrix)
	ON_UPDATE_COMMAND_UI (ID_EDIT_IMAGE_DESCRIPTION, OnUpdateEditImageDescription)
	ON_UPDATE_COMMAND_UI (ID_EDIT_IMAGE_MAP_PARAMETERS, OnUpdateEditImageMapParameters)
	ON_UPDATE_COMMAND_UI (ID_EDIT_CLEAR_ALL_IMAGE_OVERLAYS, OnUpdateEditClearOverlays)
	ON_UPDATE_COMMAND_UI (ID_EDIT_CLEAR_ALL_VECTOR_OVERLAYS, OnUpdateEditClearAllVectorOverlays)

				// View Menu Items
	ON_UPDATE_COMMAND_UI (ID_VIEW_COORDINATES_BAR, OnUpdateViewCoordinatesBar)
	ON_UPDATE_COMMAND_UI (ID_WINDOW_SHOW_COORDINATE_VIEW, OnUpdateWindowShowCoordinateView)

				// Project Menu Items
	ON_UPDATE_COMMAND_UI (ID_PROJ_USE_ORIGINAL_STATS, OnUpdateProjUseOriginalStats)
	ON_UPDATE_COMMAND_UI (ID_PROJ_USELEAVE1OUTSTATISTICS, OnUpdateProjUseleave1outstatistics)
	ON_UPDATE_COMMAND_UI (ID_PROJ_USE_ENHANCED_STATS, OnUpdateProjUseEnhancedStats)
	ON_UPDATE_COMMAND_UI (ID_PROJ_MIXOFSTATISTICSUSED, OnUpdateProjMixofstatisticsused)
	ON_UPDATE_COMMAND_UI (ID_PROJ_CLEAR_STATS, OnUpdateProjClearStats)
	ON_UPDATE_COMMAND_UI (ID_PROJ_CHANGE_BASE_IMAGE_FILE, OnUpdateProjChangeBaseImageFile)
	ON_UPDATE_COMMAND_UI (ID_PROJ_ADD_AS_ASSOCIATED_IMAGE, OnUpdateProjAddAsAssociatedImage)

				// Processor Menu Items
	ON_UPDATE_COMMAND_UI (ID_PROC_DISPLAY_IMAGE, OnUpdateProcDisplayImage)
	ON_UPDATE_COMMAND_UI (ID_PROC_HISTOGRAM_IMAGE, OnUpdateProcHistogramImage)
	ON_UPDATE_COMMAND_UI (ID_PROC_LISTDATA, OnUpdateProcListData)
	ON_UPDATE_COMMAND_UI (ID_PROC_REFORMAT, OnUpdateProcReformat)
	ON_UPDATE_COMMAND_UI (ID_PROC_CLUSTER, OnUpdateProcCluster)
	ON_UPDATE_COMMAND_UI (ID_PROC_STATISTICS, OnUpdateProcStatistics)
	ON_UPDATE_COMMAND_UI (ID_PROC_ENHANCE_STATISTICS, OnUpdateProcEnhanceStatistics)
	ON_UPDATE_COMMAND_UI (ID_PROC_FEATURE_EXTRACTION, OnUpdateProcFeatureExtraction)
	ON_UPDATE_COMMAND_UI (ID_PROC_FEATURE_SELECTION, OnUpdateProcFeatureSelection)
	ON_UPDATE_COMMAND_UI (ID_PROC_CLASSIFY, OnUpdateProcClassify)
	ON_UPDATE_COMMAND_UI (ID_PROC_LISTRESULTS, OnUpdateProcListResults)

				// Processor->Reformat Menu Items
	ON_UPDATE_COMMAND_UI (ID_PROC_REFORMAT_CHANGE_HEADER, OnUpdateProcReformatChangeHeader)
	ON_UPDATE_COMMAND_UI (ID_PROC_REFORMAT_CHANGE_IMAGE, OnUpdateProcReformatChangeImage)
	ON_UPDATE_COMMAND_UI (ID_PROC_REFORMAT_CONVERT_MULTISPECTRAL, OnUpdateProcReformatConvertMultispectral)
	ON_UPDATE_COMMAND_UI (ID_PROC_REFORMAT_CONVERT_PROJECT, OnUpdateProcReformatConvertProject)
	ON_UPDATE_COMMAND_UI (ID_PROC_REFORMAT_CONVERT_SHAPE, OnUpdateProcReformatConvertShape)
	ON_UPDATE_COMMAND_UI (ID_PROC_REFORMAT_MODIFY_CHANNEL, OnUpdateProcReformatModifyChannel)
	ON_UPDATE_COMMAND_UI (ID_PROC_REFORMAT_MOSAIC_IMAGES, OnUpdateProcReformatMosaicImages)
	ON_UPDATE_COMMAND_UI (ID_PROC_REFORMAT_RECODE_THEMATIC, OnUpdateProcReformatRecodeThematic)
	ON_UPDATE_COMMAND_UI (ID_PROC_REFORMAT_RECTIFY_IMAGE, OnUpdateProcReformatRectifyImage)
	ON_UPDATE_COMMAND_UI (ID_PROC_REFORMAT_CONVERT_ENVI, OnUpdateProcReformatConvertEnvi)

				// Processor->Utility Menu Items
	ON_UPDATE_COMMAND_UI (ID_PROC_UTIL_PRIN_COMP_ANALYSIS, OnUpdateProcUtilPrincipalComponentAnalysis)
	ON_UPDATE_COMMAND_UI (ID_PROC_UTIL_CREATE_STAT_IMAGE, OnUpdateProcUtilCreateStatImage)
	ON_UPDATE_COMMAND_UI (ID_PROC_UTIL_BIPLOTS_OF_DATA, OnUpdateProcUtilBiplotsOfData)
	ON_UPDATE_COMMAND_UI (ID_PROC_UTIL_LIST_IMAGE_DESC, OnUpdateProcUtilListImageDesc)
	ON_UPDATE_COMMAND_UI (ID_PROC_UTIL_CHECK_COVARIANCES, OnUpdateProcUtilCheckCovariances)
	ON_UPDATE_COMMAND_UI (ID_PROC_UTIL_CHECKTRANS_MATRIX, OnUpdateProcUtilCheckTransMatrix)

				// Options Menu Items
	ON_UPDATE_COMMAND_UI (ID_OPT_MEMORY_STATUS, OnUpdateOptionsMemoryStatus)
	ON_UPDATE_COMMAND_UI (ID_OPTIONS_AREAUNITS_SQKILOMETERS, OnUpdateOptionsAreaUnitsSqKilometers)
	ON_UPDATE_COMMAND_UI (ID_OPTIONS_AREAUNITS_HECTARES, OnUpdateOptionsAreaUnitsHectares)
	ON_UPDATE_COMMAND_UI (ID_OPTIONS_AREAUNITS_SQMETERS, OnUpdateOptionsAreaUnitsSqMeters)
	ON_UPDATE_COMMAND_UI (ID_OPTIONS_AREAUNITS_SQCENTIMETERS, OnUpdateOptionsAreaUnitsSqCentimeters)
	ON_UPDATE_COMMAND_UI (ID_OPTIONS_AREAUNITS_SQMILLIMETERS, OnUpdateOptionsAreaUnitsSqMillimeters)
	ON_UPDATE_COMMAND_UI (ID_OPTIONS_AREAUNITS_SQMICROMETERS, OnUpdateOptionsAreaUnitsSqMicrometers)
	ON_UPDATE_COMMAND_UI (ID_OPTIONS_AREAUNITS_SQMILES, OnUpdateOptionsAreaUnitsSqMiles)
	ON_UPDATE_COMMAND_UI (ID_OPTIONS_AREAUNITS_ACRES, OnUpdateOptionsAreaUnitsAcres)
	ON_UPDATE_COMMAND_UI (ID_OPTIONS_AREAUNITS_SQYARDS, OnUpdateOptionsAreaUnitsSqYards)
	ON_UPDATE_COMMAND_UI (ID_OPTIONS_AREAUNITS_SQFEET, OnUpdateOptionsAreaUnitsSqFeet)
	ON_UPDATE_COMMAND_UI (ID_OPTIONS_AREAUNITS_SQINCHES, OnUpdateOptionsAreaUnitsSqInches)

				// Windows Menu Items
	ON_UPDATE_COMMAND_UI (ID_WINDOW_NEW, OnUpdateWindowNew)
	ON_UPDATE_COMMAND_UI (ID_WINDOW_NEW_SELECTION_GRAPH, OnUpdateWindowNewSelectionGraph)

	ON_WM_ACTIVATEAPP ()
	ON_WM_CHAR ()
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_PALETTECHANGED ()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_SETCURSOR ()
	ON_WM_SYSCOMMAND ()
	//}}AFX_MSG_MAP

	ON_COMMAND_RANGE(ID_CLEAROVERLAYMENUITEMSTART, ID_CLEAROVERLAYMENUITEMEND, OnDynamicMenuItem)
	END_MESSAGE_MAP ()



			// arrays of IDs used to initialize control bars

static UINT BASED_CODE indicators[] =
{                                                
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_ZOOM,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};



CMainFrame::CMainFrame ()

{
			// Add member initialization code here
	
}	// end "CMainFrame"



CMainFrame::~CMainFrame ()

{                                   
	
}	// end "~CMainFrame"


#ifdef _DEBUG
void CMainFrame::AssertValid () const

{
	CMDIFrameWnd::AssertValid ();
	
}	// end "AssertValid"
#endif	//_DEBUG


void CMainFrame::DoPaletteChanged (
				CWnd*									cWndPtr)

{
	OnPaletteChanged (cWndPtr);
	
}	// end "DoPaletteChanged"


#ifdef _DEBUG
void CMainFrame::Dump (
				CDumpContext& 						dc) const
{
	CMDIFrameWnd::Dump (dc);
	
}	// end "Dump"
#endif	//_DEBUG


Boolean CMainFrame::GetEnableFlagForStatisticsAndCluster ()

{
	Boolean     						enableFlag = FALSE;
	
	
	if (gProjectInfoPtr != NULL)
		enableFlag = TRUE;
	
	else if (gActiveImageViewCPtr != NULL)
		{
		Handle windowInfoHandle = GetWindowInfoHandle (gActiveImageViewCPtr);
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																						windowInfoHandle);
		
		Handle fileInfoHandle = GetFileInfoHandle (windowInfoPtr);
		FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
		
		if (windowInfoPtr->numberImageFiles == 1 &&
				(windowInfoPtr->imageType == kMultispectralImageType ||
													windowInfoPtr->projectWindowFlag))
			enableFlag = TRUE;
		
		}	// end "else if (gActiveImageViewCPtr != NULL)"
	
	return (enableFlag);
	
}	// end "GetEnableFlagForStatisticsAndCluster"



void CMainFrame::OnActivateApp (
				BOOL 									bActive,
				DWORD 								hTask)

{
	CMDIFrameWnd::OnActivateApp (bActive, hTask);
	
	gInBackground = !bActive;

	if (!gInBackground)
		{
				// Update the global display bits per pixel in case
				// the user has changed the monitor setting.
		
		CDC				pDC;
		
		if (pDC.CreateIC ((LPCTSTR)_T("DISPLAY"), NULL, NULL, NULL))
			gDisplayBitsPerPixel = pDC.GetDeviceCaps (BITSPIXEL);
	
		if (gActiveImageViewCPtr != NULL)
			{
					// This instance of MultiSpec was moved to the foreground. If there
					// is an active image window, force it to be updated.
			
			CMImageDoc* pDoc = gActiveImageViewCPtr->GetDocument ();
			pDoc->UpdateAllViews (NULL);
			
			}	// end "if (gActiveImageViewCPtr != NULL)"

		}	// end "if (!gInBackground)"
	
}	// end "OnActivateApp"



void CMainFrame::OnChar (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags)

{
			// Add your message handler code here and/or call default
	
	CMDIFrameWnd::OnChar (nChar, nRepCnt, nFlags);
	
}	// end "OnChar"



void CMainFrame::OnClose ()

{
	if (gProcessorCode != -1)
		((CMultiSpecApp*)AfxGetApp())->ExitApplication ();
	
	else	// gProcessorCode == -1
		CMDIFrameWnd::OnClose ();
	
}	// end "OnClose"



int CMainFrame::OnCreate (
				LPCREATESTRUCT 					lpCreateStruct)

{
	if (CMDIFrameWnd::OnCreate (lpCreateStruct) == -1)
																								return -1;

	if (!m_wndToolBar.Create (this) || !m_wndToolBar.LoadToolBar (IDR_TOOLBAR))
		{
		TRACE("Failed to create toolbar\n");
				// fail to create
																								return -1;
		
		}	// end "if (!m_wndToolBar.Create (this) || ..."
		
	else	// Tool bar created.
		{
		SInt16 itemID = m_wndToolBar.GetItemID (12);
		CWnd* zoomInWindow = GetDlgItem (itemID);
		/*	
				//get DPI scaling 
		
		double fx = GetSystemMetrics(SM_CXSMICON) / 16.0f;
		double fy = GetSystemMetrics(SM_CYSMICON) / 16.0f;
		if (fx < 1) fx = 1;
		if (fy < 1) fy = 1;

		CRect temp;
		m_wndToolBar.GetItemRect(0, &temp);
		temp.MoveToXY(0, 0);
		temp.right = int(temp.right * fx);
		temp.bottom = int(temp.bottom * fy);

		CSize bmp_size (16, 15);
		m_wndToolBar.SetSizes (temp.Size(), bmp_size);
		*/
		}	// end "else Tool bar created"

	if (!m_wndStatusBar.Create (this) ||
					!m_wndStatusBar.SetIndicators (indicators,
		  														sizeof (indicators)/sizeof (UINT)))
		{
		TRACE("Failed to create status bar\n");
				// fail to create
																								return -1;
		}	// end "if (!m_wndStatusBar.Create (this) || ..."

	return 0; 
	
}	// end "OnCreate"



void CMainFrame::OnDynamicMenuItem (
				UINT									menuID)

{
	ClearOverlay (menuID - ID_CLEAROVERLAYMENUITEMSTART + 1);

}	// end "OnDynamicMenuItem"



void CMainFrame::OnEditClearAllImageOverlays ()

{
	CloseAllImageOverlayFiles ();
	
}	// end "OnEditClearAllImageOverlays"



void CMainFrame::OnEditClearAllVectorOverlays ()

{
	CloseAllVectorOverlayFiles ();
	
}	// end "OnEditClearAllVectorOverlays"



void CMainFrame::OnEditClearTransMatrix ()

{
	ClearTransformationMatrix (TRUE);
	
}	// end "OnEditClearTransMatrix"



void CMainFrame::OnEditImageDescription ()

{
	Boolean								parameterChangedFlag;


	gProcessorCode = kChangeImageDescriptionProcessor;
	
	FileSpecificationDialog (gActiveImageFileInfoH,
										gActiveImageWindowInfoH,
										&parameterChangedFlag);
	gProcessorCode = 0;

	if (parameterChangedFlag)
		{
		gProcessorCode = kDisplayProcessor;
		DisplayImage ();

		}	// end "if (parameterChangedFlag)"
	
	gMemoryTypeNeeded = 0;
	
}	// end "OnEditImageDescription"



void CMainFrame::OnEditImageMapParameters ()

{
	CoordinateDialog ();
	
}	// end "OnEditImageMapParameters"



void CMainFrame::OnFileCloseProject ()

{
	Boolean returnFlag = CloseTheProject ();
	
}	// end "OnFileCloseProject"



void CMainFrame::OnFileLoadTransMatrix ()

{
	if (gTransformationMatrix.numberChannels > 0)
		WriteTransformationFile ();
	
	else	// gTransformationMatrix.numberChannels <= 0
		LoadTransformationFile ();
	
}	// end "OnFileLoadTransMatrix"



void CMainFrame::OnFileNewProject ()

{
	gProcessorCode = kStatisticsProcessor;
	StatisticsControl ();
	gProcessorCode = 0;
	
}	// end "OnFileNewProject"



void CMainFrame::OnFileOpen (void)

{
	CMOpenFileDialog*					dialogPtr;
	Boolean 								continueFlag = FALSE;
	
	
	TRY
		{
		dialogPtr = new CMOpenFileDialog ();
		
		if (dialogPtr != NULL)
			{
			dialogPtr->DoModal ();
			delete dialogPtr;

			}	// end "if (dialogPtr != NULL)"
			
		}	// end TRY
	
	CATCH_ALL (e)
		{
		MemoryMessage (0, kObjectMessage);
		}
	
	END_CATCH_ALL
	
}	// end "OnFileOpen"



void CMainFrame::OnFileOpenProjectImage (void)

{
	OpenProjectImageWindow ();
	
}	// end "OnFileOpenProjectImage"



void CMainFrame::OnFileOpenThematic ()

{
	LoadThematicInfo ();
	
}	// end "OnFileOpenThematic"



void CMainFrame::OnFileSaveProject ()

{
	SaveProjectFile (0);
	
}	// end "OnFileSaveProject"



void CMainFrame::OnFileSaveProjectAs ()

{
	SaveProjectFile (1);
	
}	// end "OnFileSaveProjectAs"



void CMainFrame::OnFileOpenProject ()

{
	gProcessorCode = kOpenProjectFileProcessor;

	OpenProjectFile (NULL);

	if (gProjectInfoPtr != NULL)
		CreateProjectWindow ();

	gProcessorCode = 0;
	
}	// end "OnFileOpenProject"



void CMainFrame::OnOptionsAreaUnitsAcres ()

{
	gDefaultAreaUnits = kAcresUnitsMenuItem;
	
}	// end "OnOptionsAreaUnitsAcres"



void CMainFrame::OnOptionsAreaUnitsHectares ()
{
	gDefaultAreaUnits = kHectareUnitsMenuItem;
	
}	// end "OnOptionsAreaUnitsHectares"



void CMainFrame::OnOptionsAreaUnitsSqCentimeters ()

{
	gDefaultAreaUnits = kSqCentimetersUnitsMenuItem;
	
}	// end "OnOptionsAreaUnitsSqCentimeters"



void CMainFrame::OnOptionsAreaUnitsSqFeet ()

{
	gDefaultAreaUnits = kSqFeetUnitsMenuItem;
	
}	// end "OnOptionsAreaUnitsSqFeet"



void CMainFrame::OnOptionsAreaUnitsSqInches ()

{
	gDefaultAreaUnits = kSqInchesUnitsMenuItem;
	
}	// end "OnOptionsAreaUnitsSqInches"



void CMainFrame::OnOptionsAreaUnitsSqKilometers ()

{
	gDefaultAreaUnits = kSqKilometersUnitsMenuItem;
	
}	// end "OnOptionsAreaUnitsSqKilometers"



void CMainFrame::OnOptionsAreaUnitsSqMeters ()

{
	gDefaultAreaUnits = kSqMetersUnitsMenuItem;
	
}	// end "OnOptionsAreaUnitsSqMeters"



void CMainFrame::OnOptionsAreaUnitsSqMicrometers ()

{
	gDefaultAreaUnits = kSqMicrometersUnitsMenuItem;
	
}	// end "OnOptionsAreaUnitsSqMicrometers"



void CMainFrame::OnOptionsAreaUnitsSqMiles ()

{
	gDefaultAreaUnits = kSqMilesUnitsMenuItem;
	
}	// end "OnOptionsAreaUnitsSqMiles"



void CMainFrame::OnOptionsAreaUnitsSqMillimeters ()

{
	gDefaultAreaUnits = kSqMillimetersUnitsMenuItem;
	
}	// end "OnOptionsAreaUnitsSqMillimeters"



void CMainFrame::OnOptionsAreaUnitsSqYards ()

{
	gDefaultAreaUnits = kSqYardsUnitsMenuItem;
	
}	// end "OnOptionsAreaUnitsSqYards"



void CMainFrame::OnOptionsSwitchcrosscursor ()

{
	if (gCrossCursorID == IDC_CROSS_CURSOR)
		gCrossCursorID = IDC_CROSS_CURSOR2;

	else	// gCrossCursorID != IDC_CROSS_CURSOR
		gCrossCursorID = IDC_CROSS_CURSOR;

}	// end "OnOptionsSwitchcrosscursor"



void CMainFrame::OnPaletteChanged (
				CWnd* 								pFocusWnd)

{
	CMDIFrameWnd::OnPaletteChanged (pFocusWnd);

			// always realize the palette for the active view
	
	CMDIChildWnd* pMDIChildWnd = MDIGetActive ();
	
			// Return if no active MDI child frame
	
	if (pMDIChildWnd == NULL)
																							return;
	
	CView* pView = pMDIChildWnd->GetActiveView ();
	ASSERT (pView != NULL);

			// notify all child windows that the palette has changed
	
	SendMessageToDescendants (WM_DOREALIZE, (WPARAM)pView->m_hWnd, 0, TRUE);
	
}	// end "OnPaletteChanged"



void CMainFrame::OnProcClassify (void)

{
	gProcessorCode = kClassifyProcessor;
	ClassifyControl ();
	gProcessorCode = 0;
	gTextMemoryShortCode = 0;
	
}	// end "OnProcClassify"



void CMainFrame::OnProcCluster (void)

{
	gProcessorCode = kClusterProcessor;
	
	if (gProjectInfoPtr == NULL)
		OpenNewProject ();
	
	else	// gProjectInfoPtr != NULL
		gProjectInfoPtr->newProjectFlag = FALSE;
	
	if (gProjectInfoPtr != NULL)
		ClusterControl ();
	
	gProcessorCode = 0;
	gTextMemoryShortCode = 0;
	
}	// end "OnProcCluster"



void CMainFrame::OnProcDisplayImage ()

{
	if (gActiveImageViewCPtr != NULL)
		DisplayImage ();
	
}	// end "OnProcDisplayImage"



void CMainFrame::OnProcEnhanceStatistics ()

{
	gMemoryTypeNeeded = 1;
	gProcessorCode = kStatEnhanceProcessor;
	
	StatisticsEnhanceControl ();
	
	gProcessorCode = 0;
	gMemoryTypeNeeded = 0;
	
}	// end "OnProcEnhanceStatistics"



void CMainFrame::OnProcFeatureExtraction ()

{
	gMemoryTypeNeeded = 1;
	gProcessorCode = kFeatureExtractionProcessor;
	
	FeatureExtractionControl ();
	
	gProcessorCode = 0;
	gMemoryTypeNeeded = 0;
	
}	// end "OnProcFeatureExtraction"



void CMainFrame::OnProcFeatureSelection ()

{
	gMemoryTypeNeeded = 1;
	gProcessorCode = kSeparabilityProcessor;
	
	SeparabilityControl ();
	
	gProcessorCode = 0;
	gMemoryTypeNeeded = 0;
	
}	// end "OnProcFeatureSelection"



void CMainFrame::OnProcHistogramImage ()

{
	if (gActiveImageViewCPtr != NULL)
		{
		gProcessorCode = kHistogramProcessor;
		
		if (gActiveImageViewCPtr->m_histogramCPtr != NULL &&
					gActiveImageViewCPtr->GetImageWindowCPtr () != NULL)
			HistogramControl (kComputeHistogram);
		
		gMemoryTypeNeeded = 0;
		gProcessorCode = 0;
		
		}	// end "if (gActiveImageViewCPtr != NULL)"
	
}	// end "OnProcHistogramImage"



void CMainFrame::OnProcListData ()

{
	gProcessorCode = kListDataProcessor;
	ListDataControl ();
	gProcessorCode = 0;
	
}	// end "OnProcListData"



void CMainFrame::OnProcListResults ()

{
	gProcessorCode = kListResultsProcessor;
	ListResultsControl ();
	gProcessorCode = 0;
	
}	// end "OnProcListResults"



void CMainFrame::OnProcReformat (void)

{
	gProcessorCode = kReformatProcessor;
	ReformatControl (2);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnProcReformat"



void CMainFrame::OnProcReformatChangeHeader ()

{
	gProcessorCode = kReformatProcessor;
	ReformatControl (kReformatChangeHeaderRequest);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnProcReformatChangeHeader"



void CMainFrame::OnProcReformatChangeImage ()

{
	gProcessorCode = kReformatProcessor;
	ReformatControl (kReformatChangeImageRequest);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnProcReformatChangeImage"



void CMainFrame::OnProcReformatConvertEnvi ()

{
	gProcessorCode = kENVIROItoThematicProcessor;
	ENVI_ROIToThematicFileControl ();
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnProcReformatConvertEnvi"



void CMainFrame::OnProcReformatConvertMultispectral ()

{
			// Add your command handler code here
	
}	// end "OnProcReformatConvertMultispectral"



void CMainFrame::OnProcReformatConvertProject ()

{
	gProcessorCode = kReformatProcessor;
	ReformatControl (kReformatConvertProjectRequest);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnProcReformatConvertProject"



void CMainFrame::OnProcReformatConvertShape ()

{
	gProcessorCode = kReformatProcessor;
	ReformatControl (kReformatConvertShapeRequest);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnProcReformatConvertShape"



void CMainFrame::OnProcReformatModifyChannel ()

{
	gProcessorCode = kReformatProcessor;
	ReformatControl (kReformatModifyChannelRequest);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnProcReformatModifyChannel"



void CMainFrame::OnProcReformatMosaicImages ()

{
	ReformatControl (kReformatMosaicImagesRequest);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnProcReformatMosaicImages"



void CMainFrame::OnProcReformatRecodeThematic ()

{
	gProcessorCode = kReformatProcessor;
	ReformatControl (kReformatRecodeThematicRequest);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnProcReformatRecodeThematic"



void CMainFrame::OnProcReformatRectifyImage ()

{
	gProcessorCode = kReformatProcessor;
	ReformatControl (kReformatRectifyImageRequest);
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnProcReformatRectifyImage"



void CMainFrame::OnProcStatistics (void)

{
	gProcessorCode = kStatisticsProcessor;
	StatisticsControl ();
	gProcessorCode = 0;
	gTextMemoryShortCode = 0;
	
}	// end "OnProcStatistics"



void CMainFrame::OnProcUtilBiplotsOfData ()

{
	gMemoryTypeNeeded = 1;
	gProcessorCode = kBiPlotDataProcessor;
	
	ForceGraphCodeResourceLoad ();
	
	if (gMemoryTypeNeeded == 0)
		{
		gMemoryTypeNeeded = 1;
		BiPlotDataControl ();
		
		}	// end "if (gMemoryTypeNeeded == 0)"
	
	gProcessorCode = 0;
	gMemoryTypeNeeded = 0;
	
}	// end "OnProcUtilBiplotsOfData"



void CMainFrame::OnProcUtilCheckCovariances (void)

{
	gProcessorCode = kCovarianceCheckProcessor;
	EvaluateCovariancesControl ();
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnProcUtilCheckCovariances"



void CMainFrame::OnProcUtilCheckTransMatrix ()

{
	gProcessorCode = kTransformCheckProcessor;
	EvaluateTransformationControl ();
	gProcessorCode = 0;
	
}	// end "OnProcUtilCheckTransMatrix"



void CMainFrame::OnProcUtilCreateStatImage (void)

{
	gProcessorCode = kStatisticsImageProcessor;
	StatisticsImageControl ();
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnProcUtilCreateStatImage"



void CMainFrame::OnProcUtilListImageDesc (void)

{
	gProcessorCode = kListDescriptionProcessor;
	ListDescriptionInformation ();
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnProcUtilListImageDesc"



void CMainFrame::OnProcUtilPrinCompAnalysis ()

{
	gProcessorCode = kPrincipalComponentsProcessor;
	PrincipalComponentControl ();
	gProcessorCode = 0;
	
}	// end "OnProcUtilPrinCompAnalysis"



void CMainFrame::OnProjAddAsAssociatedImage ()

{
	Handle windowInfoHandle = GetWindowInfoHandle (gActiveImageViewCPtr);
	ChangeProjectAssociatedImageItem (windowInfoHandle);
	
}	// end "OnProjAddAsAssociatedImage"



void CMainFrame::OnProjChangeBaseImageFile ()

{
	ChangeProjectBaseImage ();
	
}	// end "OnProjChangeBaseImageFile"



void CMainFrame::OnProjClearStats ()

{
	Boolean returnFlag = ProjectMenuClearStatistics ();
	
}	// end "OnProjClearStats"



void CMainFrame::OnProjUseEnhancedStats ()

{
	SetProjectCovarianceStatsToUse (kEnhancedStats);
	
}	// end "SetProjectCovarianceStatsToUse"



void CMainFrame::OnProjUseleave1outstatistics ()

{
	SetProjectCovarianceStatsToUse (kLeaveOneOutStats);
	
}	// end "OnProjUseleave1outstatistics"



void CMainFrame::OnProjUseOriginalStats ()

{
	SetProjectCovarianceStatsToUse (kOriginalStats);
	
}	// end "OnProjUseOriginalStats"



BOOL CMainFrame::OnQueryNewPalette ()

{
			// always realize the palette for the active view
	
	CMDIChildWnd* pMDIChildWnd = MDIGetActive ();
	
			// If no active MDI child frame (no new palette)
	
	if (pMDIChildWnd == NULL)
																					return FALSE;
	
			// Activate the palette for the active image window if
			// it exists.
	
	if (gActiveImageViewCPtr == NULL)
																					return FALSE;

			// just notify the target view
	
	gActiveImageViewCPtr->SendMessage (WM_DOREALIZE,
													(WPARAM)gActiveImageViewCPtr->m_hWnd,
													1);

	return TRUE;
	
}	// end "OnQueryNewPalette"



BOOL CMainFrame::OnSetCursor (
				CWnd* 								pWnd,
				UINT 									nHitTest,
				UINT 									message)

{
	if (gPresentCursor == kWait || gPresentCursor == kSpin)
		{
				// Wait cursor in affect. Processing in progress.
				// Do not change the cursor or change it back to wait cursor

				// If the nHitTest is set then the cursor may have moved
				// outside of the application window and them back.
				// Make sure it is set back to the wait cursor.

		AfxGetApp()->DoWaitCursor (0);
																				return (TRUE);

		}	// end "if (gPresentCursor == kWait || gPresentCursor == kSpin)"

	if (gPresentCursor != kArrow)
		{
		if (gActiveImageViewCPtr != NULL &&
								gActiveImageViewCPtr->CheckIfOffscreenImageExists ())
			gActiveImageViewCPtr->UpdateCursorCoordinates ();
		
		gPresentCursor = kArrow; 		// Non image window cursor
		
		}	// end "if (gPresentCursor != kArrow)"
	
	return CMDIFrameWnd::OnSetCursor (pWnd, nHitTest, message);
	
}	// end "OnSetCursor"



void CMainFrame::OnSysCommand (
				UINT 									nID,
				LPARAM 								lParam)

{
	if (gProcessorCode != kListDataProcessor)
		CMDIFrameWnd::OnSysCommand (nID, lParam);
	
}	// end "OnSysCommand"



void CMainFrame::OnUpdateEditClear (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateEditClear"



void CMainFrame::OnUpdateEditClearAllVectorOverlays (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (gNumberShapeFiles > 0);

}	// end "OnUpdateEditClearAllVectorOverlays"



void CMainFrame::OnUpdateEditClearOverlays (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (UpdateEditClearOverlays (pCmdUI));

}	// end "OnUpdateEditClearOverlays"



void CMainFrame::OnUpdateEditClearSelectionRectangle (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateEditClearSelectionRectangle"



void CMainFrame::OnUpdateEditClearTransMatrix (
				CCmdUI* 								pCmdUI)

{
	Boolean								enableFlag = FALSE;
	
	
	if (gTransformationMatrix.numberChannels > 0)
		enableFlag = TRUE;
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateEditClearTransMatrix"



void CMainFrame::OnUpdateEditCopy (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateEditCopy"



void CMainFrame::OnUpdateEditCut (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateEditCut"



void CMainFrame::OnUpdateEditImageDescription (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (gActiveImageViewCPtr != NULL);
	
}	// end "OnUpdateEditImageDescription"



void CMainFrame::OnUpdateEditImageMapParameters (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (UpdateEditImageMapParameters ());
	
}	// end "OnUpdateEditImageMapParameters"



void CMainFrame::OnUpdateEditPaste (
				CCmdUI* 								pCmdUI)
{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateEditPaste"



void CMainFrame::OnUpdateEditSelectAll (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateEditSelectAll"



void CMainFrame::OnUpdateEditSelectionRectangle (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateEditSelectionRectangle"



void CMainFrame::OnUpdateEditUndo (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateEditUndo"



void CMainFrame::OnUpdateFileCloseProject (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (gProjectInfoPtr != NULL);
	
}	// end "OnUpdateFileCloseProject"



void CMainFrame::OnUpdateFileLoadTransMatrix (
				CCmdUI* 								pCmdUI)

{
	Boolean enableFlag = UpdateFileTransform (pCmdUI);
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateFileLoadTransMatrix"



void CMainFrame::OnUpdateFileNewProject (
				CCmdUI* 								pCmdUI)

{
	Boolean 								enableFlag = FALSE;
	
	if (gProjectInfoPtr == NULL && gFullVersionFlag)
	   enableFlag = GetEnableFlagForStatisticsAndCluster ();
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateFileNewProject"



void CMainFrame::OnUpdateFileOpen (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (TRUE);
	
}	// end "OnUpdateFileOpen"



void CMainFrame::OnUpdateFileOpenProject (
				CCmdUI* 								pCmdUI)

{
	Boolean 								enableFlag = FALSE;


	if (gFullVersionFlag && gProjectInfoPtr == NULL)
		enableFlag = TRUE;

	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateFileOpenProject"



void CMainFrame::OnUpdateFileOpenProjectImage (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (gProjectInfoPtr != NULL);
	
}	// end "OnUpdateFileOpenProjectImage"



void CMainFrame::OnUpdateFileOpenThematic (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (UpdateFileOpenThematicInfo (pCmdUI));
	
}	// end "OnUpdateFileOpenThematic"



void CMainFrame::OnUpdateFilePrintPreview (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateFilePrintPreview"



void CMainFrame::OnUpdateFileSave (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateFileSave"



void CMainFrame::OnUpdateFileSaveProject (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (gProjectInfoPtr != NULL && gProjectInfoPtr->changedFlag);
	
}	// end "OnUpdateFileSaveProject"



void CMainFrame::OnUpdateFileSaveProjectAs (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (gProjectInfoPtr != NULL);
	
}	// end "OnUpdateFileSaveProjectAs"



void CMainFrame::OnUpdateOptionsAreaUnitsSqKilometers (
				CCmdUI* 								pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqKilometersUnitsMenuItem);
	
}	// end "OnUpdateOptionsAreaUnitsSqKilometers"



void CMainFrame::OnUpdateOptionsAreaUnitsHectares (
				CCmdUI* 								pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kHectareUnitsMenuItem);
	
}	// end "OnUpdateOptionsAreaUnitsHectares"



void CMainFrame::OnUpdateOptionsAreaUnitsSqMeters (
				CCmdUI* 								pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqMetersUnitsMenuItem);
	
}	// end "OnUpdateOptionsAreaUnitsSqMeters"



void CMainFrame::OnUpdateOptionsAreaUnitsSqCentimeters (
				CCmdUI* 								pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqCentimetersUnitsMenuItem);
	
}	// end "OnUpdateOptionsAreaUnitsSqCentimeters"



void CMainFrame::OnUpdateOptionsAreaUnitsSqMillimeters (
				CCmdUI* 								pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqMillimetersUnitsMenuItem);
	
}	// end "OnUpdateOptionsAreaUnitsSqMillimeters"



void CMainFrame::OnUpdateOptionsAreaUnitsSqMicrometers (
				CCmdUI* 								pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqMicrometersUnitsMenuItem);
	
}	// end "OnUpdateOptionsAreaUnitsSqMicrometers"



void CMainFrame::OnUpdateOptionsAreaUnitsSqMiles (
				CCmdUI* 								pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqMilesUnitsMenuItem);
	
}	// end "OnUpdateOptionsAreaUnitsSqMiles"



void CMainFrame::OnUpdateOptionsAreaUnitsAcres (
				CCmdUI* 								pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kAcresUnitsMenuItem);
	
}	// end "OnUpdateOptionsAreaUnitsAcres"



void CMainFrame::OnUpdateOptionsAreaUnitsSqYards (
				CCmdUI* 								pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqYardsUnitsMenuItem);
	
}	// end "OnUpdateOptionsAreaUnitsSqYards"



void CMainFrame::OnUpdateOptionsAreaUnitsSqFeet (
				CCmdUI* 								pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqFeetUnitsMenuItem);
	
}	// end "OnUpdateOptionsAreaUnitsSqFeet"



void CMainFrame::OnUpdateOptionsAreaUnitsSqInches (
				CCmdUI* 								pCmdUI)

{
	SetUpdateAreaUnits (pCmdUI, kSqInchesUnitsMenuItem);
	
}	// end "OnUpdateOptionsAreaUnitsSqInches"



void CMainFrame::OnUpdateOptionsMemoryStatus (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateOptionsMemoryStatus"



void CMainFrame::OnUpdateProcClassify (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (gProjectInfoPtr != NULL &&
						gProjectInfoPtr->numberStatTrainClasses >= 1);
	
}	// end "OnUpdateProcClassify"



void CMainFrame::OnUpdateProcCluster (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (GetEnableFlagForStatisticsAndCluster ());
	
}	// end "OnUpdateProcCluster"



void CMainFrame::OnUpdateProcDisplayImage (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (gActiveImageViewCPtr != NULL);
	
}	// end "OnUpdateProcDisplayImage"



void CMainFrame::OnUpdateProcEnhanceStatistics (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (gProjectInfoPtr != NULL &&
								gProjectInfoPtr->numberStatTrainClasses >= 1);
	
}	// end "OnUpdateProcEnhanceStatistics"



void CMainFrame::OnUpdateProcFeatureExtraction (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (gProjectInfoPtr != NULL &&
								gProjectInfoPtr->numberStatTrainClasses >= 2);
	
}	// end "OnUpdateProcFeatureExtraction"



void CMainFrame::OnUpdateProcFeatureSelection (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (gProjectInfoPtr != NULL &&
								gProjectInfoPtr->numberStatTrainClasses >= 2);
	
}	// end "OnUpdateProcFeatureSelection"



void CMainFrame::OnUpdateProcHistogramImage (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (gActiveImageViewCPtr != NULL);
	
}	// end "OnUpdateProcHistogramImage"



void CMainFrame::OnUpdateProcListData (
				CCmdUI* 								pCmdUI)

{
	Boolean     						enableFlag = FALSE;
	
	
	if (gActiveImageViewCPtr != NULL || gProjectInfoPtr != NULL)
		enableFlag = TRUE;
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateProcListData"



void CMainFrame::OnUpdateProcListResults (
				CCmdUI* 								pCmdUI)

{
	Boolean enableFlag = FALSE;
	if (gActiveImageViewCPtr != NULL &&
			gActiveImageViewCPtr->GetWindowType() == kThematicWindowType &&
				gActiveImageViewCPtr->GetImageType() == kThematicImageType)
		enableFlag = TRUE;
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateProcListResults"



void CMainFrame::OnUpdateProcReformat (
				CCmdUI* 								pCmdUI)

{
	Boolean    							enableFlag = FALSE;
	
	
	if (gActiveImageViewCPtr != NULL)
		enableFlag = TRUE;
	
	if (gProjectInfoPtr != NULL && gProjectInfoPtr->numberTotalPoints > 0)
		enableFlag = TRUE;
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateProcReformat"



void CMainFrame::OnUpdateProcReformatChangeHeader (
				CCmdUI* 								pCmdUI)

{
	FileInfoPtr							fileInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	
	Handle								fileInfoHandle;
	
	
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (gActiveImageWindowInfoH,
																		kLock);
	
	
	fileInfoHandle = GetFileInfoHandle (windowInfoPtr);
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle,
																kLock);
	
	pCmdUI->Enable (UpdateReformatChangeHeader (windowInfoPtr,
																fileInfoPtr,
																pCmdUI));
	
	CheckAndUnlockHandle (fileInfoHandle);
	CheckAndUnlockHandle (gActiveImageWindowInfoH);
	
}	// end "OnUpdateProcReformatChangeHeader"



void CMainFrame::OnUpdateProcReformatChangeImage (
				CCmdUI* 								pCmdUI)

{
	Boolean								enableFlag = FALSE;
	
	
	if (gActiveImageWindowInfoH != NULL)
		enableFlag = TRUE;
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateProcReformatChangeImage"



void CMainFrame::OnUpdateProcReformatConvertEnvi (
				CCmdUI* 								pCmdUI)
{
	Boolean								enableFlag = FALSE;
	

	if (gFullVersionFlag)
			enableFlag = TRUE;

	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateProcReformatConvertEnvi"



void CMainFrame::OnUpdateProcReformatConvertMultispectral (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateProcReformatConvertMultispectral"



void CMainFrame::OnUpdateProcReformatConvertProject (
				CCmdUI* 								pCmdUI)

{
	Boolean								enableFlag = FALSE;
	
	
	if (gProjectInfoPtr != NULL && gProjectInfoPtr->numberStatisticsClasses > 0)
		enableFlag = TRUE;
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateProcReformatConvertProject"



void CMainFrame::OnUpdateProcReformatConvertShape (
				CCmdUI* 								pCmdUI)

{
	FileInfoPtr							fileInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	
	Handle								fileInfoHandle;
	
	
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (gActiveImageWindowInfoH,
																		kLock);
	
	
	fileInfoHandle = GetFileInfoHandle (windowInfoPtr);
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle,
																kLock);

	pCmdUI->Enable (UpdateReformatConvertShape (windowInfoPtr,
																fileInfoPtr,
																pCmdUI));
	
	CheckAndUnlockHandle (fileInfoHandle);
	CheckAndUnlockHandle (gActiveImageWindowInfoH);
	
}	// end "OnUpdateProcReformatConvertShape"



void CMainFrame::OnUpdateProcReformatModifyChannel (
				CCmdUI* 								pCmdUI)

{
	FileInfoPtr							fileInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	
	Handle								fileInfoHandle;
	
	
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (gActiveImageWindowInfoH,
																		kLock);
	
	
	fileInfoHandle = GetFileInfoHandle (windowInfoPtr);
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle,
																kLock);
	
	pCmdUI->Enable (UpdateReformatModifyChannel (windowInfoPtr,
																fileInfoPtr,
																pCmdUI));
	
	CheckAndUnlockHandle (fileInfoHandle);
	CheckAndUnlockHandle (gActiveImageWindowInfoH);
	
}	// end "OnUpdateProcReformatModifyChannel"



void CMainFrame::OnUpdateProcReformatMosaicImages (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (UpdateReformatMosaicImages (pCmdUI));
	
}	// end "OnUpdateProcReformatMosaicImages"



void CMainFrame::OnUpdateProcReformatRecodeThematic (
				CCmdUI* 								pCmdUI)

{
	FileInfoPtr							fileInfoPtr;
	
	Handle								fileInfoHandle;
	
	Boolean								enableFlag = FALSE;
	
	
	fileInfoHandle = GetFileInfoHandle (gActiveImageWindowInfoH);
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle,
																kLock);
	
	enableFlag = UpdateReformatRecodeThematicImage (fileInfoPtr);
	
	CheckAndUnlockHandle (fileInfoHandle);

	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateProcReformatRecodeThematic"



void CMainFrame::OnUpdateProcReformatRectifyImage (
				CCmdUI* 								pCmdUI)

{
	Boolean								enableFlag = FALSE;
	
	
	if (gActiveImageWindowInfoH != NULL)
		enableFlag = TRUE;
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateProcReformatRectifyImage"



void CMainFrame::OnUpdateProcStatistics (
				CCmdUI* 								pCmdUI)

{
	Boolean     						enableFlag = FALSE;


	if (gFullVersionFlag)
		enableFlag = GetEnableFlagForStatisticsAndCluster ();

	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateProcStatistics"



void CMainFrame::OnUpdateProcUtilBiplotsOfData (
				CCmdUI* 								pCmdUI)

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
			if (projectWindowInfoPtr != NULL &&
								projectWindowInfoPtr->totalNumberChannels >= 2)
				enableFlag = TRUE;

		}	// end "if (gProjectInfoPtr != NULL && windowInfoPtr == NULL)"

	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateProcUtilBiplotsOfData"



void CMainFrame::OnUpdateProcUtilCheckCovariances (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (gProjectInfoPtr != NULL &&
								gProjectInfoPtr->numberStatTrainClasses >= 1);
	
}	// end "OnUpdateProcUtilCheckCovariances"



void CMainFrame::OnUpdateProcUtilCheckTransMatrix (
				CCmdUI* 								pCmdUI)

{
	Boolean  							enableFlag = FALSE;
	
	
	if (gTransformationMatrix.numberChannels > 0)
		enableFlag = TRUE;
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateProcUtilCheckTransMatrix"



void CMainFrame::OnUpdateProcUtilCreateStatImage (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (TRUE);
	
}	// end "OnUpdateProcUtilCreateStatImage"



void CMainFrame::OnUpdateProcUtilListImageDesc (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (gActiveImageViewCPtr != NULL);
	
}	// end "OnUpdateProcUtilListImageDesc"



void CMainFrame::OnUpdateProcUtilPrincipalComponentAnalysis (
				CCmdUI* 								pCmdUI)

{
	Boolean     						enableFlag = FALSE;
	
	
	if (gFullVersionFlag && (gProjectInfoPtr != NULL ||
					gActiveImageViewCPtr->GetImageType () == kMultispectralImageType))
		enableFlag = TRUE;
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateProcUtilPrincipalComponentAnalysis"



void CMainFrame::OnUpdateProjAddAsAssociatedImage (
				CCmdUI* 								pCmdUI)

{
	Boolean								enableFlag = FALSE;
	
	
	if (gActiveImageViewCPtr != NULL)
		{
		Handle windowInfoHandle = GetWindowInfoHandle (gActiveImageViewCPtr);
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																						windowInfoHandle);
		
		if (windowInfoPtr != NULL)
			{
			if (windowInfoPtr->projectBaseImageFlag)
				pCmdUI->SetText ((LPCTSTR)_T("Base Image"));
			
			else	// !projectBaseImageFlag
				{
				if (windowInfoPtr->projectWindowFlag)
					pCmdUI->SetText ((LPCTSTR)_T("Remove As Associated Image"));
				
				else	// !windowInfoPtr->projectWindowFlag
					pCmdUI->SetText ((LPCTSTR)_T("Add As Associated Image"));
				
				enableFlag = (gProjectInfoPtr != NULL);
				
				}	// end "else !projectBaseImageFlag"
			
			}	// end "if (windowInfoPtr != NULL)"
		
		}	// end "if (gActiveImageViewCPtr != NULL)"
	
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateProjAddAsAssociatedImage"



void CMainFrame::OnUpdateProjChangeBaseImageFile (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (gProjectInfoPtr != NULL);
	
}	// end "OnUpdateProjChangeBaseImageFile"



void CMainFrame::OnUpdateProjClearStats (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (gProjectInfoPtr != NULL && gProjectInfoPtr->statsLoaded);
	
}	// end "OnUpdateProjClearStats"



void CMainFrame::OnUpdateProjMixofstatisticsused (
				CCmdUI* 								pCmdUI)

{
	SInt16								checkCode = 0;
	
	
	if (gProjectInfoPtr != NULL)
		{
		if (gProjectInfoPtr->covarianceStatsToUse == kMixedStats)
			checkCode = 1;
		
		}	// end "if (gProjectInfoPtr != NULL)"
	
	pCmdUI->SetCheck (checkCode);
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateProjMixofstatisticsused"



void CMainFrame::OnUpdateProjUseleave1outstatistics (
				CCmdUI* 								pCmdUI)

{
	SInt16								checkCode = 0;
	Boolean								enableFlag = FALSE;
	
	
	if (gProjectInfoPtr != NULL)
		{
		enableFlag = TRUE;
		
		if (gProjectInfoPtr->covarianceStatsToUse == kLeaveOneOutStats)
			checkCode = 1;
		
		}	// end "if (gProjectInfoPtr != NULL)"
	
	pCmdUI->SetCheck (checkCode);
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateProjUseleave1outstatistics"



void CMainFrame::OnUpdateProjUseEnhancedStats (
				CCmdUI* 								pCmdUI)

{                         
	SInt16								checkCode = 0;
	Boolean								enableFlag = FALSE;
	
	
	if (gProjectInfoPtr != NULL)
		{
		if (gProjectInfoPtr->enhancedStatsExistFlag)
			enableFlag = TRUE;
		
		if (gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats)
			checkCode = 1; 
		
		}	// end "if (gProjectInfoPtr != NULL)"
		                                      
	pCmdUI->SetCheck (checkCode);                                                                                                                           
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateProjUseEnhancedStats"



void CMainFrame::OnUpdateProjUseOriginalStats (
				CCmdUI* 								pCmdUI)

{                                    
	SInt16								checkCode = 0;
	Boolean								enableFlag = FALSE;
	
	
	if (gProjectInfoPtr != NULL)
		{
		enableFlag = TRUE;
		
		if (gProjectInfoPtr->covarianceStatsToUse == kOriginalStats)
			checkCode = 1; 
		
		}	// end "if (gProjectInfoPtr != NULL)"
		                                      
	pCmdUI->SetCheck (checkCode);                                                                                                                           
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateProjUseOriginalStats"



void CMainFrame::OnUpdateToolBarMagnification (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateToolBarMagnification"



void CMainFrame::OnUpdateToolBarZoomIn (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateToolBarZoomIn"



void CMainFrame::OnUpdateToolBarZoomIndicator (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateToolBarZoomIndicator"



void CMainFrame::OnUpdateToolBarZoomOut (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateToolBarZoomOut"



void CMainFrame::OnUpdateViewCoordinatesBar (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateViewCoordinatesBar"



void CMainFrame::OnUpdateWindowNew (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateWindowNew"



void CMainFrame::OnUpdateWindowNewSelectionGraph (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (UpdateWindowSelectionGraph ());
	
}	// end "OnUpdateWindowNewSelectionGraph"



void CMainFrame::OnUpdateWindowShowCoordinateView (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (UpdateWindowCoordinateView (pCmdUI));
	
}	// end "OnUpdateWindowShowCoordinateView"



void CMainFrame::SetUpdateAreaUnits (
				CCmdUI* 								pCmdUI,
				SInt16								unitsCode)

{  
	SInt16								checkCode = 0;

	
	if (gDefaultAreaUnits == unitsCode)
		checkCode = 1;

	pCmdUI->SetCheck (checkCode); 
	pCmdUI->Enable (TRUE);
	
}	// end "SetUpdateAreaUnits"



void CMainFrame::OnWindowNewSelectionGraph ()

{
	Boolean								returnFlag = TRUE;


	gProcessorCode = kSelectionGraphProcessor;
	
			// Make certain that the graphics code resourses are loaded.
	
	if (gNumberOfGWindows == 0)
		{
		gMemoryTypeNeeded = 1;
		ForceGraphCodeResourceLoad ();
		
		}	// end "if (gNumberOfGWindows == 0)"
	
	if (gMemoryTypeNeeded == 0)
		{
				// open a new selection graph window
		
		CMultiDocTemplate* graphDocTemplatePtr =
								((CMultiSpecApp*)AfxGetApp())->GetGraphDocTemplate ();

		if (graphDocTemplatePtr == NULL ||
									!graphDocTemplatePtr->OpenDocumentFile (NULL))
			returnFlag = FALSE;
		
		}	// end "if (gMemoryTypeNeeded == 0)"
	
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
	
}	// end "OnWindowNewSelectionGraph"



void CMainFrame::OnWindowShowCoordinateView ()

{
			// Add your command handler code here
	
}	// end "OnWindowShowCoordinateView"
