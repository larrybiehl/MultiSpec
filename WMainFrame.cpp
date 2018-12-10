//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2017)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	File:						WMainFrame.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			03/27/2017
//
//	Language:				C/C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains the implementation of the CMainFrame
//								class.
//
//	Functions in file:	
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
                   
#include "SMultiSpec.h"

#include "CDisplay.h"
#include "CHistogram.h"
#include "CImageWindow.h"

#include "WMultiSpec.h"
#include "WMainFrame.h"
#include "WImageView.h" 
#include "WDrawObjects.h"  
#include "WFileFormatDialog.h"
#include "WImageDoc.h"
#include "WImageFrame.h"
#include "WOpenFileDialog.h"
#include "WStatisticsView.h" 

//#include	"SExtGlob.h"  
//#include ".\wmainfrm.h"

extern void 				ENVI_ROIToThematicFileControl(void); 

extern Boolean 			FileSpecificationDialog (
									Handle								fileInfoHandle, 
									Handle								windowInfoHandle,
									Boolean*								parameterChangeFlagPtr);

extern UInt16							gDisplayBitsPerPixel;

extern Boolean 			UpdateEditClearOverlays (
									CCmdUI*								pCmdUI);   

extern Boolean 			UpdateFileOpenThematicInfo (
									CCmdUI*					pCmdUI); 

extern Boolean 			UpdateFileTransform (
									CCmdUI*					pCmdUI);

extern Boolean				UpdateEditImageMapParameters (void);

extern Boolean 			UpdateWindowCoordinateView (
									CCmdUI*								pCmdUI);
									
extern Boolean 			UpdateWindowSelectionGraph (void);
										
										

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame) 
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN_PROJECT_IMAGE, OnUpdateFileOpenProjectImage)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN_THEMATIC, OnUpdateFileOpenThematic)
	ON_UPDATE_COMMAND_UI(ID_EDIT_IMAGE_DESCRIPTION, OnUpdateEditImageDescription)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOAD_TRANS_MATRIX, OnUpdateLoadTransMatrix)
	ON_UPDATE_COMMAND_UI(ID_PROC_CLASSIFY, OnUpdateClassify)
	ON_UPDATE_COMMAND_UI(ID_PROC_CLUSTER, OnUpdateCluster)
	ON_UPDATE_COMMAND_UI(ID_PROC_DISPLAY_IMAGE, OnUpdateDisplayImage)
	ON_UPDATE_COMMAND_UI(ID_PROC_ENHANCE_STATISTICS, OnUpdateEnhanceStatistics)
	ON_UPDATE_COMMAND_UI(ID_PROC_FEATURE_EXTRACTION, OnUpdateFeatureExtraction)
	ON_UPDATE_COMMAND_UI(ID_PROC_FEATURE_SELECTION, OnUpdateFeatureSelection)
	ON_UPDATE_COMMAND_UI(ID_PROC_HISTOGRAM_IMAGE, OnUpdateHistogramImage)
	ON_UPDATE_COMMAND_UI(ID_PROC_LISTDATA, OnUpdateListdata)
	ON_UPDATE_COMMAND_UI(ID_PROC_LISTRESULTS, OnUpdateListResults)
	ON_UPDATE_COMMAND_UI(ID_PROC_REFORMAT, OnUpdateReformat)
	ON_UPDATE_COMMAND_UI(ID_PROC_STATISTICS, OnUpdateStatistics)
	ON_UPDATE_COMMAND_UI(ID_PROC_UTIL_BIPLOTS_OF_DATA, OnUpdateBiplotsOfData)
	ON_UPDATE_COMMAND_UI(ID_PROC_UTIL_CHECK_COVARIANCES, OnUpdateCheckCovariances)
	ON_UPDATE_COMMAND_UI(ID_PROC_UTIL_CHECKTRANS_MATRIX, OnUpdateCheckTransMatrix)
	ON_UPDATE_COMMAND_UI(ID_PROC_UTIL_CREATE_STAT_IMAGE, OnUpdateCreateStatImage)
	ON_UPDATE_COMMAND_UI(ID_PROC_UTIL_LIST_IMAGE_DESC, OnUpdateListImageDesc)
	ON_UPDATE_COMMAND_UI(ID_PROC_UTIL_PRIN_COMP_ANALYSIS, OnUpdatePrincipalComponentAnalysis)
	ON_COMMAND(ID_PROC_DISPLAY_IMAGE, OnDisplayImage)
	ON_WM_CREATE()                                         
	ON_WM_PALETTECHANGED()      
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR_SELECT_RECTANGLE, OnUpdateClearSelectionRectangle)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR_TRANS_MATRIX, OnUpdateClearTransMatrix)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateSelectAll)
	ON_UPDATE_COMMAND_UI(ID_OPT_MEMORY_STATUS, OnUpdateMemoryStatus)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PALETTE_INVERT_PALETTE, OnUpdateInvertPalette)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PALETTE_SHOW_CLASSES, OnUpdateShowClasses)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PALETTE_SHOW_INFORMATION_GROUPS, OnUpdateShowInformationGroups)
	ON_UPDATE_COMMAND_UI(ID_PROJ_ADD_AS_ASSOCIATED_IMAGE, OnUpdateAddAsAssociatedImage)
	ON_UPDATE_COMMAND_UI(ID_PROJ_CHANGE_BASE_IMAGE_FILE, OnUpdateChangeBaseImageFile)
	ON_UPDATE_COMMAND_UI(ID_PROJ_CLEAR_STATS, OnUpdateClearStats)                 
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW_PROJECT, OnUpdateNewProject)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN_PROJECT, OnUpdateOpenProject)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_PROJECT, OnUpdateSaveProject)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_PROJECT_AS, OnUpdateSaveProjectAs)
	ON_UPDATE_COMMAND_UI(ID_PROJ_USE_ENHANCED_STATS, OnUpdateUseEnhancedStats)
	ON_UPDATE_COMMAND_UI(ID_PROJ_USE_ORIGINAL_STATS, OnUpdateUseOriginalStats)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)          
	ON_UPDATE_COMMAND_UI(ID_MAGNIFICATION, OnUpdateMagnification)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, OnUpdateZoomIn)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, OnUpdateZoomOut) 
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_EDIT_IMAGE_DESCRIPTION, OnEditImageDescription)
	ON_COMMAND(ID_PROC_HISTOGRAM_IMAGE, OnHistogramImage)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ZOOM, OnUpdateZoomIndicator)
	ON_UPDATE_COMMAND_UI(ID_EDIT_IMAGE_MAP_PARAMETERS, OnUpdateEditImageMapParameters)
	ON_WM_SETCURSOR()    
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_COMMAND(ID_PROC_REFORMAT, OnProcReformat)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)                             
	ON_COMMAND(ID_FILE_OPEN_PROJECT, OnOpenProject)
	ON_COMMAND(ID_PROC_UTIL_LIST_IMAGE_DESC, OnProcUtilListImageDesc)
	ON_COMMAND(ID_PROC_UTIL_CHECK_COVARIANCES, OnProcUtilCheckCovariances)
	ON_COMMAND(ID_PROJ_CLEAR_STATS, OnProjClearStats)
	ON_COMMAND(ID_PROC_STATISTICS, OnProcStatistics)
	ON_COMMAND(ID_PROC_CLASSIFY, OnProcClassify)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COORDINATES_BAR, OnUpdateViewCoordinatesBar)
	ON_COMMAND(ID_FILE_OPEN_PROJECT_IMAGE, OnFileOpenProjectImage)
	ON_COMMAND(ID_PROJ_ADD_AS_ASSOCIATED_IMAGE, OnProjAddAsAssociatedImage) 
	ON_COMMAND(ID_FILE_SAVE_PROJECT, OnProjSaveProject)
	ON_COMMAND(ID_FILE_SAVE_PROJECT_AS, OnProjSaveProjectAs)
	ON_WM_CHAR()
	ON_COMMAND(ID_OPTIONS_PALETTE_SHOW_CLASSES, OnPalShowClasses)
	ON_COMMAND(ID_OPTIONS_PALETTE_SHOW_INFORMATION_GROUPS, OnPalShowInformationGroups)
	ON_COMMAND(ID_PROC_CLUSTER, OnProcCluster)
	ON_COMMAND(ID_FILE_NEW_PROJECT, OnProjNewProject)
	ON_COMMAND(ID_PROC_UTIL_PRIN_COMP_ANALYSIS, OnProcUtilPrinCompAnalysis)
	ON_COMMAND(ID_PROC_LISTDATA, OnProcListdata)
	ON_COMMAND(ID_PROC_FEATURE_SELECTION, OnProcFeatureSelection)
	ON_COMMAND(ID_PROC_UTIL_BIPLOTS_OF_DATA, OnProcUtilBiplotsOfData)  
	ON_COMMAND(ID_PROC_UTIL_CREATE_STAT_IMAGE, OnProcUtilCreateStatImage)
	ON_COMMAND(ID_FILE_OPEN_THEMATIC, OnFileOpenThematic)
	ON_WM_ACTIVATEAPP()
	ON_COMMAND(ID_PROC_UTIL_CHECKTRANS_MATRIX, OnProcUtilCheckTransMatrix) 
	ON_COMMAND(ID_EDIT_CLEAR_TRANS_MATRIX, OnEditClearTransMatrix)
	ON_WM_QUERYNEWPALETTE()
	ON_COMMAND(ID_FILE_LOAD_TRANS_MATRIX, OnFileLoadTransMatrix)
	ON_COMMAND(ID_PROC_LISTRESULTS, OnProcListResults)
	ON_WM_SYSCOMMAND()                              
	ON_UPDATE_COMMAND_UI(ID_PROJ_USELEAVE1OUTSTATISTICS, OnUpdateProjectUseleave1outstatistics)
	ON_UPDATE_COMMAND_UI(ID_PROJ_MIXOFSTATISTICSUSED, OnUpdateProjectMixofstatisticsused) 
	ON_COMMAND(ID_PROJ_USE_ORIGINAL_STATS, OnProjUseOriginalStats)
	ON_COMMAND(ID_PROJ_USE_ENHANCED_STATS, OnProjUseEnhancedStats)
	ON_COMMAND(ID_PROJ_USELEAVE1OUTSTATISTICS, OnProjectUseleave1outstatistics)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECTION_RECTANGLE, OnUpdateEditSelectionRectangle)
	ON_COMMAND(ID_PROC_FEATURE_EXTRACTION, OnProcFeatureExtraction)
	ON_COMMAND(ID_PROC_ENHANCE_STATISTICS, OnProcEnhanceStatistics)
	ON_COMMAND(ID_PROJ_CHANGE_BASE_IMAGE_FILE, OnProjChangeBaseImageFile)
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_WINDOW_NEW, OnUpdateWindowNew)
	ON_COMMAND(ID_EDIT_IMAGE_MAP_PARAMETERS, OnEditImageMapParameters)
	ON_COMMAND(ID_WINDOW_NEW_SELECTION_GRAPH, OnWindowNewSelectionGraph)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_NEW_SELECTION_GRAPH, OnUpdateWindowNewSelectionGraph)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_SHOW_COORDINATE_VIEW, OnUpdateWindowShowCoordinateView)
	ON_COMMAND(ID_WINDOW_SHOW_COORDINATE_VIEW, OnWindowShowCoordinateView)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR_ALL_IMAGE_OVERLAYS, OnUpdateEditClearOverlays)
//	ON_UPDATE_COMMAND_UI(ID_CLEAROVERLAYMENUITEMSTART, OnUpdateEditClearOverlays)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR_ALL_VECTOR_OVERLAYS, OnUpdateEditClearAllVectorOverlays)
	ON_COMMAND(ID_EDIT_CLEAR_ALL_IMAGE_OVERLAYS, OnEditClearAllImageOverlays)
//	ON_COMMAND(ID_CLEAROVERLAYMENUITEMSTART, OnEditClearAllImageOverlays)
	ON_COMMAND(ID_EDIT_CLEAR_ALL_VECTOR_OVERLAYS, OnEditClearAllVectorOverlays)
	ON_COMMAND(ID_PROC_REFORMAT_CHANGE_HEADER, OnProcReformatChangeHeader)
	ON_UPDATE_COMMAND_UI(ID_PROC_REFORMAT_CHANGE_HEADER, OnUpdateProcReformatChangeHeader)
	ON_COMMAND(ID_PROC_REFORMAT_CHANGE_IMAGE, OnProcReformatChangeImage)
	ON_UPDATE_COMMAND_UI(ID_PROC_REFORMAT_CHANGE_IMAGE, OnUpdateProcReformatChangeImage)
	ON_COMMAND(ID_PROC_REFORMAT_CONVERT_ENVI, OnProcReformatConvertEnvi)
	ON_UPDATE_COMMAND_UI(ID_PROC_REFORMAT_CONVERT_ENVI, OnUpdateProcReformatConvertEnvi)
	ON_COMMAND(ID_PROC_REFORMAT_CONVERT_MULTISPECTRAL, OnProcReformatConvertMultispectral)
	ON_UPDATE_COMMAND_UI(ID_PROC_REFORMAT_CONVERT_MULTISPECTRAL, OnUpdateProcReformatConvertMultispectral)
	ON_COMMAND(ID_PROC_REFORMAT_CONVERT_PROJECT, OnProcReformatConvertProject)
	ON_UPDATE_COMMAND_UI(ID_PROC_REFORMAT_CONVERT_PROJECT, OnUpdateProcReformatConvertProject)
	ON_COMMAND(ID_PROC_REFORMAT_MODIFY_CHANNEL, OnProcReformatModifyChannel)
	ON_UPDATE_COMMAND_UI(ID_PROC_REFORMAT_MODIFY_CHANNEL, OnUpdateProcReformatModifyChannel)
	ON_COMMAND(ID_PROC_REFORMAT_MOSAIC_IMAGES, OnProcReformatMosaicImages)
	ON_UPDATE_COMMAND_UI(ID_PROC_REFORMAT_MOSAIC_IMAGES, OnUpdateProcReformatMosaicImages)
	ON_COMMAND(ID_PROC_REFORMAT_RECODE_THEMATIC, OnProcReformatRecodeThematic)
	ON_UPDATE_COMMAND_UI(ID_PROC_REFORMAT_RECODE_THEMATIC, OnUpdateProcReformatRecodeThematic)
	ON_COMMAND(ID_PROC_REFORMAT_RECTIFY_IMAGE, OnProcReformatRectifyImage)
	ON_UPDATE_COMMAND_UI(ID_PROC_REFORMAT_RECTIFY_IMAGE, OnUpdateProcReformatRectifyImage)
	ON_COMMAND(ID_PROC_REFORMAT_CONVERT_SHAPE, OnProcReformatConvertShape)
	ON_UPDATE_COMMAND_UI(ID_PROC_REFORMAT_CONVERT_SHAPE, OnUpdateProcReformatConvertShape)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_AREAUNITS_SQKILOMETERS, OnUpdateAreaUnitsSqKilometers)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_AREAUNITS_HECTARES, OnUpdateAreaUnitsHectares)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_AREAUNITS_SQMETERS, OnUpdateAreaUnitsSqMeters)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_AREAUNITS_SQCENTIMETERS, OnUpdateAreaUnitsSqCentimeters)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_AREAUNITS_SQMILLIMETERS, OnUpdateAreaUnitsSqMillimeters)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_AREAUNITS_SQMICROMETERS, OnUpdateAreaUnitsSqMicrometers)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_AREAUNITS_SQMILES, OnUpdateAreaUnitsSqMiles)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_AREAUNITS_ACRES, OnUpdateAreaUnitsAcres)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_AREAUNITS_SQYARDS, OnUpdateAreaUnitsSqYards)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_AREAUNITS_SQFEET, OnUpdateAreaUnitsSqFeet)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_AREAUNITS_SQINCHES, OnUpdateAreaUnitsSqInches)
	ON_COMMAND(ID_OPTIONS_AREAUNITS_SQKILOMETERS, OnOptionsAreaUnitsSqKilometers)
	ON_COMMAND(ID_OPTIONS_AREAUNITS_HECTARES, OnOptionsAreaUnitsHectares)
	ON_COMMAND(ID_OPTIONS_AREAUNITS_SQMETERS, OnOptionsAreaUnitsSqMeters)
	ON_COMMAND(ID_OPTIONS_AREAUNITS_SQCENTIMETERS, OnOptionsAreaUnitsSqCentimeters)
	ON_COMMAND(ID_OPTIONS_AREAUNITS_SQMILLIMETERS, OnOptionsAreaUnitsSqMillimeters)
	ON_COMMAND(ID_OPTIONS_AREAUNITS_SQMICROMETERS, OnOptionsAreaUnitsSqMicrometers)
	ON_COMMAND(ID_OPTIONS_AREAUNITS_SQMILES, OnOptionsAreaUnitsSqMiles)
	ON_COMMAND(ID_OPTIONS_AREAUNITS_ACRES, OnOptionsAreaUnitsAcres)
	ON_COMMAND(ID_OPTIONS_AREAUNITS_SQYARDS, OnOptionsAreaUnitsSqYards)
	ON_COMMAND(ID_OPTIONS_AREAUNITS_SQFEET, OnOptionsAreaUnitsSqFeet)
	ON_COMMAND(ID_OPTIONS_AREAUNITS_SQINCHES, OnOptionsAreaUnitsSqInches)
	
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_CLEAROVERLAYMENUITEMSTART, ID_CLEAROVERLAYMENUITEMEND, OnDynamicMenuItem)
	ON_COMMAND(ID_OPTIONS_SWITCHCROSSCURSOR, OnOptionsSwitchcrosscursor)
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

	static UINT BASED_CODE indicators[] =
{                                                
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_ZOOM,
//	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction



CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}



CMainFrame::~CMainFrame()
{                                   
	
}		// end "~CMainFrame"



void
CMainFrame::DoPaletteChanged(
				CWnd*				cWndPtr)
				
{       
	OnPaletteChanged (cWndPtr);                                   
	
}		// end "~CMainFrame" 



int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR))
		{
			TRACE("Failed to create toolbar\n");
			return -1;      // fail to create
		}
		
	else		// Tool bar created.
		{
		SInt16 itemID = m_wndToolBar.GetItemID(12);
		CWnd* zoomInWindow = GetDlgItem(itemID);
		
		}		// end "else Tool bar created"

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
		{
			TRACE("Failed to create status bar\n");
			return -1;      // fail to create
		}

	return 0; 
	
}		// end "OnCreate"



/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG  

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
                              
                                               
void CMainFrame::OnDisplayImage()
{                                  
	if (gActiveImageViewCPtr != NULL) 
		DisplayImage ();
	                                                            	
}		// end "OnDisplayImage" 



void CMainFrame::OnPaletteChanged(CWnd* pFocusWnd)
{                                                                           
	CMDIFrameWnd::OnPaletteChanged(pFocusWnd);

			// always realize the palette for the active view
			
	CMDIChildWnd* pMDIChildWnd = MDIGetActive();
	
			// Return if no active MDI child frame
			
	if (pMDIChildWnd == NULL)
																							return;
		
	CView* pView = pMDIChildWnd->GetActiveView();
	ASSERT(pView != NULL); 
/*					             
	sprintf(	(char*)&gTextString,
				"OnPaletteChanged %s",
				gEndOfLine);
	OutputString ( NULL, 
					(char*)&gTextString, 
					0, 
					1, 
					TRUE);  
 */
			// notify all child windows that the palette has changed
			
	SendMessageToDescendants(
			WM_DOREALIZE, (WPARAM)pView->m_hWnd, 0, TRUE);
	
}	// end "OnPaletteChanged"



BOOL CMainFrame::OnQueryNewPalette()
{                                                                   
			// always realize the palette for the active view
			
	CMDIChildWnd* pMDIChildWnd = MDIGetActive();
	
			// If no active MDI child frame (no new palette)
			
	if (pMDIChildWnd == NULL)
																					return FALSE;
																							
//	CView* pView = pMDIChildWnd->GetActiveView();
//	ASSERT(pView != NULL);
	
			// Activate the palette for the active image window if
			// it exists.
			
	if (gActiveImageViewCPtr == NULL)
																					return FALSE; 
/*					             
	sprintf(	(char*)&gTextString,
				"OnQueryNewPalette %s",
				gEndOfLine);
	OutputString ( NULL, 
					(char*)&gTextString, 
					0, 
					1, 
					TRUE);  
 */
			// just notify the target view
			                                                     
	gActiveImageViewCPtr->SendMessage(WM_DOREALIZE, 
												(WPARAM)gActiveImageViewCPtr->m_hWnd, 
												1);

	return TRUE;
	
//	return CMDIFrameWnd::OnQueryNewPalette();

}		// end "OnQueryNewPalette"
 


void CMainFrame::OnUpdateFileOpen(CCmdUI* pCmdUI)
{                                                                                                                                                      
//	pCmdUI->Enable(gStretchDIBitsFlag);                                                                                                                 
	pCmdUI->Enable(TRUE);
	
}



void CMainFrame::OnUpdateFileOpenProjectImage(CCmdUI* pCmdUI)
{                                                                                                                                                      
	pCmdUI->Enable(gProjectInfoPtr != NULL);
	
}

                                      

void CMainFrame::OnUpdateEditImageDescription(CCmdUI* pCmdUI)
{                                                                                                                                                    
	pCmdUI->Enable(gActiveImageViewCPtr != NULL);
	
}		// end "OnUpdateEditImageDescription"
 
 

void CMainFrame::OnUpdateLoadTransMatrix(CCmdUI* pCmdUI)
{  
	Boolean enableFlag = UpdateFileTransform(pCmdUI);
	                                                                                    
	pCmdUI->Enable(enableFlag);
	
}		// end "OnUpdateLoadTransMatrix"     



void CMainFrame::OnUpdateAddAsAssociatedImage(CCmdUI* pCmdUI)
{  
	Boolean		enableFlag = FALSE;
	
	
	if (gActiveImageViewCPtr != NULL)
		{
		Handle windowInfoHandle = GetWindowInfoHandle (gActiveImageViewCPtr);
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
														windowInfoHandle,
														kNoLock,
														kNoMoveHi);
														
		if (windowInfoPtr != NULL)
			{
			if (windowInfoPtr->projectBaseImageFlag)
				pCmdUI->SetText ((LPCTSTR)_T("Base Image"));
				
			else		// !projectBaseImageFlag
				{
				if (windowInfoPtr->projectWindowFlag)     
					pCmdUI->SetText ((LPCTSTR)_T("Remove As Associated Image"));
					
				else		// !windowInfoPtr->projectWindowFlag      
					pCmdUI->SetText ((LPCTSTR)_T("Add As Associated Image"));
				                             	 
				enableFlag = (gProjectInfoPtr != NULL);
				
				}		// end "else !projectBaseImageFlag"
				
			}		// end "if (windowInfoPtr != NULL)"  
		
		}		// end "if (gActiveImageViewCPtr != NULL)"
		
	pCmdUI->Enable(enableFlag); 
	
}		// end "OnUpdateAddAsAssociatedImage"



void CMainFrame::OnUpdateChangeBaseImageFile(CCmdUI* pCmdUI)
{                                                                                                                                                   
	pCmdUI->Enable(gProjectInfoPtr != NULL);
	
}		// end "OnUpdateChangeBaseImageFile"

void CMainFrame::OnUpdateClassify(CCmdUI* pCmdUI)
{                                                                                   
	pCmdUI->Enable(gProjectInfoPtr != NULL && 
						gProjectInfoPtr->numberStatTrainClasses >= 1);
	
}

void CMainFrame::OnUpdateCluster(CCmdUI* pCmdUI)
{                                                                                  
	pCmdUI->Enable( GetEnableFlagForStatisticsAndCluster() );
	
}

void CMainFrame::OnUpdateDisplayImage(CCmdUI* pCmdUI)
{                               
	pCmdUI->Enable(gActiveImageViewCPtr != NULL);
	
}

void CMainFrame::OnUpdateEnhanceStatistics(CCmdUI* pCmdUI)
{                                                              
	pCmdUI->Enable(gProjectInfoPtr != NULL && 
						gProjectInfoPtr->numberStatTrainClasses >= 1);
						
//	pCmdUI->Enable(FALSE);
	
}

void CMainFrame::OnUpdateFeatureExtraction(CCmdUI* pCmdUI)
{                                                               
	pCmdUI->Enable(gProjectInfoPtr != NULL && 
						gProjectInfoPtr->numberStatTrainClasses >= 2);
	
}

void CMainFrame::OnUpdateFeatureSelection(CCmdUI* pCmdUI)
{                                                               
	pCmdUI->Enable(gProjectInfoPtr != NULL && 
						gProjectInfoPtr->numberStatTrainClasses >= 2);
	
}

void CMainFrame::OnUpdateHistogramImage(CCmdUI* pCmdUI)
{                         
	pCmdUI->Enable(gActiveImageViewCPtr != NULL);
	
}

void CMainFrame::OnUpdateListdata(CCmdUI* pCmdUI)
{                         
	Boolean     enableFlag = FALSE;
	
	
	if (gActiveImageViewCPtr != NULL || gProjectInfoPtr != NULL) 
		enableFlag = TRUE;                                 
		                                                                                             
	pCmdUI->Enable(enableFlag);
	
}

void CMainFrame::OnUpdateListResults(CCmdUI* pCmdUI)
{  
	SInt16 windowType = gActiveImageViewCPtr->GetWindowType();
	
	Boolean enableFlag = FALSE;
	if (windowType == kThematicWindowType)
		enableFlag = TRUE;
		                                                                                                               
	pCmdUI->Enable(enableFlag);
	
}		// end "OnUpdateListResults"



void 
CMainFrame::OnUpdateReformat(
				CCmdUI* 				pCmdUI)

{  
	Boolean     enableFlag = FALSE;
	
	
	if (gActiveImageViewCPtr != NULL)
		enableFlag = TRUE;  
		
	if (gProjectInfoPtr != NULL && gProjectInfoPtr->numberTotalPoints > 0)
		enableFlag = TRUE;  
		                                                                                                                
	pCmdUI->Enable(enableFlag);
	
}		// end "OnUpdateReformat"



void CMainFrame::OnUpdateStatistics(
				CCmdUI* 				pCmdUI)
				
{      
	Boolean     enableFlag = FALSE;

	if (gFullVersionFlag)
		enableFlag = GetEnableFlagForStatisticsAndCluster();

	pCmdUI->Enable(enableFlag);
	
}		// end "OnUpdateStatistics"  



Boolean CMainFrame::GetEnableFlagForStatisticsAndCluster()
				
{            
	Boolean     enableFlag = FALSE;
	
	
	if (gProjectInfoPtr != NULL)
		enableFlag = TRUE;
		                          
	else if (gActiveImageViewCPtr != NULL)
		{
		Handle windowInfoHandle = GetWindowInfoHandle (gActiveImageViewCPtr);
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
														windowInfoHandle,
														kNoLock,
														kNoMoveHi);
														
		Handle fileInfoHandle = GetFileInfoHandle (windowInfoPtr); 
		FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (
														fileInfoHandle,
														kNoLock,
														kNoMoveHi);
														
		if (windowInfoPtr->numberImageFiles == 1 &&
				(windowInfoPtr->imageType == kMultispectralImageType ||  
											windowInfoPtr->projectWindowFlag) ) 
			enableFlag = TRUE;
														
		}		// end "else if (gActiveImageViewCPtr != NULL)"
		                                                                                                                
	return (enableFlag);
	
}		// end "GetEnableFlagForStatisticsAndCluster"



void CMainFrame::OnUpdateBiplotsOfData(CCmdUI* pCmdUI)
{  
	Boolean enableFlag = FALSE;

	Handle windowInfoHandle = GetWindowInfoHandle(gActiveImageViewCPtr);
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
	
}	// end "OnUpdateBiplotsOfData"

void CMainFrame::OnUpdateCheckCovariances(CCmdUI* pCmdUI)
{                                                                                                                      
	pCmdUI->Enable(gProjectInfoPtr != NULL && 
								gProjectInfoPtr->numberStatTrainClasses >= 1);
	
} 



void 
CMainFrame::OnUpdateCheckTransMatrix(
				CCmdUI* 				pCmdUI)
                    	
{  
	Boolean  enableFlag = FALSE;
	
	
	if (gTransformationMatrix.numberChannels > 0)
		enableFlag = TRUE;
		                                                                                                                    
	pCmdUI->Enable(enableFlag);
	
}		// end "OnUpdateCheckTransMatrix"



void CMainFrame::OnUpdateCreateStatImage(CCmdUI* pCmdUI)
{                                                                                                                      
	//pCmdUI->Enable(FALSE);
	pCmdUI->Enable(TRUE);
	
}



void CMainFrame::OnUpdateListImageDesc(CCmdUI* pCmdUI)
{                                                                                                                      
	pCmdUI->Enable(gActiveImageViewCPtr != NULL);
	
}



void CMainFrame::OnUpdatePrincipalComponentAnalysis(CCmdUI* pCmdUI)
{                        
	Boolean     enableFlag = FALSE;
	
	
	if (gFullVersionFlag && (gProjectInfoPtr != NULL ||
							gActiveImageViewCPtr->GetImageType() == kMultispectralImageType)) 
		enableFlag = TRUE;                                 
		                                                                                             
	pCmdUI->Enable(enableFlag);
	
}		// end "OnUpdatePrincipalComponentAnalysis"

  

void 
CMainFrame::OnUpdateClearSelectionRectangle(
				CCmdUI* 				pCmdUI)
				
{  	
	pCmdUI->Enable ( FALSE );
	
}		// end "OnUpdateClearSelectionRectangle"



void CMainFrame::OnUpdateClearTransMatrix(
				CCmdUI* 				pCmdUI)
				
{                                                                                                                                                    
	Boolean		enableFlag = FALSE;
	
	if (gTransformationMatrix.numberChannels > 0)
		enableFlag = TRUE;
		
	pCmdUI->Enable(enableFlag);
	
}

void CMainFrame::OnUpdateSelectAll(CCmdUI* pCmdUI)
{                                                                                                                                                   
	pCmdUI->Enable(FALSE);
	
}

void CMainFrame::OnUpdateMemoryStatus(CCmdUI* pCmdUI)
{                                                                                                                                                   
	pCmdUI->Enable(FALSE);
	
}



void CMainFrame::OnUpdateEditImageMapParameters(CCmdUI* pCmdUI)
{                                                                                                                                                   
	pCmdUI->Enable( UpdateEditImageMapParameters() );
	
}		// end "OnUpdateEditImageMapParameters"  



void CMainFrame::OnUpdateClearStats(CCmdUI* pCmdUI)
{  
	                                                                                                                                                 
	pCmdUI->Enable(gProjectInfoPtr != NULL && gProjectInfoPtr->statsLoaded);
	
} 



void CMainFrame::OnUpdateCloseProject(CCmdUI* pCmdUI)
{                                                                                                                                                   
	pCmdUI->Enable(gProjectInfoPtr != NULL);
	
}



void CMainFrame::OnUpdateNewProject(CCmdUI* pCmdUI)
{  
	Boolean enableFlag = FALSE;
	
	if (gProjectInfoPtr == NULL && gFullVersionFlag)
	   enableFlag = GetEnableFlagForStatisticsAndCluster();
	                                                                               
	pCmdUI->Enable(enableFlag);
	
}



void CMainFrame::OnUpdateOpenProject(CCmdUI* pCmdUI)
{       
	Boolean enableFlag = FALSE;

	if (gFullVersionFlag && gProjectInfoPtr == NULL)
		enableFlag = TRUE;

	pCmdUI->Enable(enableFlag);
	
}

void CMainFrame::OnUpdateSaveProject(CCmdUI* pCmdUI)
{                                                                                                                                                   
	pCmdUI->Enable(gProjectInfoPtr != NULL &&
						gProjectInfoPtr->changedFlag);
	
} 



void CMainFrame::OnUpdateSaveProjectAs(CCmdUI* pCmdUI)
{                                                                                                                                                  
	pCmdUI->Enable(gProjectInfoPtr != NULL);
	
}

void CMainFrame::OnUpdateUseEnhancedStats(CCmdUI* pCmdUI)
{                         
	SInt16			checkCode = 0;
	Boolean			enableFlag = FALSE;
	
	
	if (gProjectInfoPtr != NULL)
		{
		if (gProjectInfoPtr->enhancedStatsExistFlag)
			enableFlag = TRUE;
		
		if (gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats)
			checkCode = 1; 
		
		}		// end "if (gProjectInfoPtr != NULL)"
		                                      
	pCmdUI->SetCheck(checkCode);                                                                                                                           
	pCmdUI->Enable(enableFlag);
	
}		// end "OnUpdateUseEnhancedStats"



void CMainFrame::OnUpdateUseOriginalStats(CCmdUI* pCmdUI)
{                                    
	SInt16			checkCode = 0;
	Boolean			enableFlag = FALSE;
	
	
	if (gProjectInfoPtr != NULL)
		{
		enableFlag = TRUE;
		
		if (gProjectInfoPtr->covarianceStatsToUse == kOriginalStats)
			checkCode = 1; 
		
		}		// end "if (gProjectInfoPtr != NULL)"
		                                      
	pCmdUI->SetCheck(checkCode);                                                                                                                           
	pCmdUI->Enable(enableFlag);
	
}		// end "OnUpdateUseOriginalStats"   
   


void CMainFrame::OnUpdateEditCut(CCmdUI* pCmdUI)
{                                                                                                                                                                                  
	pCmdUI->Enable(FALSE);
	
}		// end "OnUpdateEditCut"


void CMainFrame::OnUpdateEditCopy(CCmdUI* pCmdUI)
{                                                                                                                                                                                 
	pCmdUI->Enable(FALSE);
	
}

void CMainFrame::OnUpdateEditPaste(CCmdUI* pCmdUI)
{                                                                                                                                                                                 
	pCmdUI->Enable(FALSE);
	
}      


void 
CMainFrame::OnUpdateMagnification(
				CCmdUI* 		pCmdUI) 
				
{                                                                                                                                                     
	pCmdUI->Enable(FALSE);
	
}		// end "OnUpdateMagnification"


void 
CMainFrame::OnUpdateZoomIn(
				CCmdUI* 		pCmdUI) 
				
{                                                                                                                                                     
	pCmdUI->Enable(FALSE);
	
}		// end "OnUpdateZoomIn"


void 
CMainFrame::OnUpdateZoomOut(
				CCmdUI* 		pCmdUI) 
				
{                                                                                                                                                     
	pCmdUI->Enable(FALSE);
	
}		// end "OnUpdateZoomOut"  

 
 
void 
CMainFrame::OnUpdateZoomIndicator(
				CCmdUI* 		pCmdUI) 
				
{                                                                                                                                                                                       
	pCmdUI->Enable(FALSE);
	
}		// end "OnUpdateZoomIndicator"



void 
CMainFrame::OnUpdateEditClear(
				CCmdUI* pCmdUI)

{                                                                                                                                                                                                                       
	pCmdUI->Enable(FALSE);
	
}



void 
CMainFrame::OnUpdateEditUndo(
				CCmdUI* pCmdUI)

{                                                                                                                                                                                                                       
	pCmdUI->Enable(FALSE);
	
}



void 
CMainFrame::OnUpdateFilePrintPreview(
				CCmdUI* pCmdUI)

{                                                                                                                                                                                                                       
	pCmdUI->Enable(FALSE);
	
}



void 
CMainFrame::OnUpdateShowClasses(
				CCmdUI* 					pCmdUI)
				
{                        
	Boolean		enableFlag = FALSE;
	SInt16		checkCode = 0;
	
	                       
	if (gActiveImageViewCPtr != NULL)
		{
		if ( gActiveImageViewCPtr->CheckIfOffscreenImageExists() )
			{
			SInt16 windowType = gActiveImageViewCPtr->GetWindowType();
			
			if (windowType == kThematicWindowType)
				{
				enableFlag = TRUE;
				
				SInt16 classGroupCode = gActiveImageViewCPtr->GetClassGroupCode ();                 
				if (classGroupCode == kClassDisplay)
					checkCode = 1;
				
				}		// end "if (windowType == kThematicType)"
			
			}		// end "if ( gActiveImageViewCPtr->..."
		
		}		// end "if (gActiveImageViewCPtr != NULL)" 
		                                         
	pCmdUI->SetCheck(checkCode);                                                                                                   
	pCmdUI->Enable(enableFlag);
	
}		// end "OnUpdateShowClasses"



void 
CMainFrame::OnUpdateShowInformationGroups(
				CCmdUI* 					pCmdUI)

{                         
	Boolean		enableFlag = FALSE; 
	SInt16		checkCode = 0;
	
	                       
	if (gActiveImageViewCPtr != NULL)
		{
		if ( gActiveImageViewCPtr->CheckIfOffscreenImageExists() )
			{
			SInt16 windowType = gActiveImageViewCPtr->GetWindowType();
			
			if (windowType == kThematicWindowType)
				{
				SInt16 numberGroups = gActiveImageViewCPtr->GetNumberGroups();
				if (numberGroups > 0)
					enableFlag = TRUE;
				
				SInt16 classGroupCode = gActiveImageViewCPtr->GetClassGroupCode ();     
				if (classGroupCode == kGroupDisplay)
					checkCode = 1;
				
				}		// end "if (windowType == kThematicType)"
			
			}		// end "if ( gActiveImageViewCPtr->..."
		
		}		// end "if (gActiveImageViewCPtr != NULL)" 
		                                          
	pCmdUI->SetCheck(checkCode);                                                                                                 
	pCmdUI->Enable(enableFlag);
	
}		// end "OnUpdateShowInformationGroups"



void CMainFrame::OnUpdateInvertPalette(CCmdUI* pCmdUI)
{                                                                                                                                                   
	pCmdUI->Enable(FALSE);
	
}		// end "OnUpdateInvertPalette"  



void CMainFrame::OnUpdateViewCoordinatesBar(CCmdUI* pCmdUI)
{                                                                                                                                                                                                                                                       
	pCmdUI->Enable(FALSE);
	
}		// end "OnUpdateViewCoordinatesBar" 



void 
CMainFrame::OnFileOpen(void)

{           
	Boolean 					continueFlag = FALSE;
	CMOpenFileDialog*		dialogPtr;	
	
	
	TRY
		{ 
		dialogPtr = new CMOpenFileDialog(); 
		
		if (dialogPtr != NULL)
			{
			dialogPtr->DoModal (); 
			delete dialogPtr;

			}
		}
				
	CATCH_ALL(e)
		{
		MemoryMessage(0, kObjectMessage);
		}
	END_CATCH_ALL 
	
} 		// end "OnFileOpen"



void CMainFrame::OnFileOpenProjectImage(void)
{
	OpenProjectImageWindow ();
	
}		// end "OnFileOpenProjectImage"



void CMainFrame::OnOpenProject()
{
	gProcessorCode = kOpenProjectFileProcessor;

	OpenProjectFile (NULL);

	if (gProjectInfoPtr != NULL)
		CreateProjectWindow ();

	gProcessorCode = 0;
	
}		// end "OnOpenProject"

 

void 
CMainFrame::OnEditImageDescription()

{  
	Boolean					parameterChangedFlag;


	gProcessorCode = kChangeImageDescriptionProcessor;
	
	FileSpecificationDialog (gActiveImageFileInfoH, 
										gActiveImageWindowInfoH,
										&parameterChangedFlag); 
	gProcessorCode = 0;

	if (parameterChangedFlag)
		{
		gProcessorCode = kDisplayProcessor;
		DisplayImage ();

		}		// end "if (parameterChangedFlag)"
	
	gMemoryTypeNeeded = 0;
	
}		// end "OnEditImageDescription"



void CMainFrame::OnProjCloseProject()
{
	Boolean returnFlag = CloseTheProject ();
	
}		// end "OnProjCloseProject" 



void CMainFrame::OnProjSaveProject()
{
	SaveProjectFile (0);
	
}		// end "OnProjSaveProject"



void CMainFrame::OnProjSaveProjectAs()
{                                             
	SaveProjectFile (1);
	
}		// end "OnProjSaveProjectAs" 



void CMainFrame::OnProjClearStats()
{
	Boolean returnFlag = ProjectMenuClearStatistics ();
	
}		// end "OnProjClearStats" 



void CMainFrame::OnProjAddAsAssociatedImage()
{  
	Handle windowInfoHandle = GetWindowInfoHandle (gActiveImageViewCPtr);
	ChangeProjectAssociatedImageItem (windowInfoHandle);
	
}		// end "OnProjAddAsAssociatedImage" 



void CMainFrame::OnHistogramImage()
{                                             
	
	
	if (gActiveImageViewCPtr != NULL)    
		{ 																				 
		gProcessorCode = kHistogramProcessor;
		
		if (gActiveImageViewCPtr->m_histogramCPtr != NULL &&
					gActiveImageViewCPtr->GetImageWindowCPtr() != NULL )
			HistogramControl(kComputeHistogram);
			                 
		gMemoryTypeNeeded = 0; 
		gProcessorCode = 0;
		
		}		// end "if (gActiveImageViewCPtr != NULL)"
	
}		// end "OnHistogramImage"    



void 
CMainFrame::OnProcReformat(void)

{                                              
	gProcessorCode = kReformatProcessor;
	ReformatControl(2);   
	gMemoryTypeNeeded = 0; 
	gProcessorCode = 0;
	
}		// end "OnProcReformat" 



void CMainFrame::OnProcStatistics(void)

{
	gProcessorCode = kStatisticsProcessor;
	StatisticsControl();   
	gProcessorCode = 0;
	gTextMemoryShortCode = 0;
	
}		// end "OnProcStatistics"



void CMainFrame::OnProcClassify(void)
{                                             
	gProcessorCode = kClassifyProcessor;
	ClassifyControl ();  
	gProcessorCode = 0;
	gTextMemoryShortCode = 0;
	
}		// end "OnProcClassify" 

void CMainFrame::OnProcUtilCreateStatImage(void)
{
	gProcessorCode = kStatisticsImageProcessor;
	StatisticsImageControl();
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;
}

void 
CMainFrame::OnProcUtilListImageDesc(void)

{                               
	gProcessorCode = kListDescriptionProcessor;
	ListDescriptionInformation (); 
	gMemoryTypeNeeded = 0; 
	gProcessorCode = 0;
	
}		// end "OnProcUtilListImageDesc"



void 
CMainFrame::OnProcUtilCheckCovariances(void)

{
	gProcessorCode = kCovarianceCheckProcessor;
	EvaluateCovariancesControl ();
	gMemoryTypeNeeded = 0;
	gProcessorCode = 0;    
	
}		// end "OnProcUtilCheckCovariances" 



void 
CMainFrame::OnPalShowClasses()
{  
	CMImageFrame* imageFrameCPtr = gActiveImageViewCPtr->GetImageFrameCPtr();
	
	if (imageFrameCPtr != NULL)
		imageFrameCPtr->ChangeClassGroupDisplay (kClassDisplay, TRUE); 
	
}		// end "OnPalShowClasses"



void 
CMainFrame::OnPalShowInformationGroups()
{                         
	CMImageFrame* imageFrameCPtr = gActiveImageViewCPtr->GetImageFrameCPtr();
	
	if (imageFrameCPtr != NULL)
		imageFrameCPtr->ChangeClassGroupDisplay (kGroupDisplay, TRUE); 
	
}		// end "OnPalShowInformationGroups" 



void CMainFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	
	CMDIFrameWnd::OnChar(nChar, nRepCnt, nFlags);
	
}		// end "OnChar" 



// Coded By:			Larry L. Biehl				Date:
// Revised By:			Larry L. Biehl				Date: 08/30/2018

BOOL CMainFrame::OnSetCursor (
				CWnd* pWnd, 
				UINT nHitTest, 
				UINT message)
				
{  
	if (gPresentCursor == kWait || gPresentCursor == kSpin)
		{
				// Wait cursor in affect. Processing in progress.
				// Do not change the cursor or change it back to wait cursor

				// If the nHitTest is set then the cursor may have moved 
				// outside of the application window and them back. 
				// Make sure it is set back to the wait cursor.

		//if (nHitTest != HTNOWHERE)
		AfxGetApp ()->DoWaitCursor (0);

																		return (TRUE);

		}	// end "if (gPresentCursor == kWait || gPresentCursor == kSpin)"

	if (gPresentCursor != kArrow)
		{        

		if (gActiveImageViewCPtr != NULL && 
								gActiveImageViewCPtr->CheckIfOffscreenImageExists ())
			gActiveImageViewCPtr->UpdateCursorCoordinates();
		                                                                
		gPresentCursor = kArrow; 		// Non image window cursor
		
		}		// end "if (gPresentCursor != kArrow)"
	
	return CMDIFrameWnd::OnSetCursor (pWnd, nHitTest, message);
	
}		// end "OnSetCursor"



void CMainFrame::OnProcCluster()
{                                               
	gProcessorCode = kClusterProcessor;
	
	if (gProjectInfoPtr == NULL)                                        
		OpenNewProject();  
		
	else		// gProjectInfoPtr != NULL
		gProjectInfoPtr->newProjectFlag = FALSE;
		
	if (gProjectInfoPtr != NULL)
		ClusterControl();
	   
	gProcessorCode = 0;
	gTextMemoryShortCode = 0;
	
}		// end "OnProcCluster"



void CMainFrame::OnProjNewProject()
{                                                                                     
//	OpenNewProject();
	
//	Handle windowInfoHandle = GetActiveImageWindowInfoHandle ();
//	LoadProjectFileAndLayerInformation (windowInfoHandle); 

	gProcessorCode = kStatisticsProcessor;

	StatisticsControl (); 
	
	gProcessorCode = 0;   
	
}		// end "OnProjNewProject"



void CMainFrame::OnProcUtilPrinCompAnalysis()
{                                                         
	gProcessorCode = kPrincipalComponentsProcessor; 
	
	PrincipalComponentControl();
	                                             
	gProcessorCode = 0;   
	
}		// end "OnProcUtilPrinCompAnalysis"



void CMainFrame::OnProcListdata()
{                                                          
	gProcessorCode = kListDataProcessor;
	
	ListDataControl (); 
                     
	gProcessorCode = 0;    
	
}		// end "OnProcListdata"



void CMainFrame::OnProcFeatureSelection()
{                                              
	gMemoryTypeNeeded = 1;                                  
	gProcessorCode = kSeparabilityProcessor;
	
	SeparabilityControl ();
	                                                      
	gProcessorCode = 0; 
	gMemoryTypeNeeded = 0;
	
}		// end "OnProcFeatureSelection"



void CMainFrame::OnProcUtilBiplotsOfData()
{                                              
	gMemoryTypeNeeded = 1;                                  
	gProcessorCode = kBiPlotDataProcessor;
	
	ForceGraphCodeResourceLoad ();
	
	if (gMemoryTypeNeeded == 0)
		{                                   
		gMemoryTypeNeeded = 1;
		BiPlotDataControl (); 
		
		}		// end "if (gMemoryTypeNeeded == 0)"
	                                                  
	gProcessorCode = 0; 
	gMemoryTypeNeeded = 0;
	
} 		// end "OnProcUtilBiplotsOfData"



void CMainFrame::OnUpdateFileOpenThematic(
				CCmdUI* 			pCmdUI)
{  				                                                                                                                                                   
	pCmdUI->Enable( UpdateFileOpenThematicInfo (pCmdUI) );
	
} 		// end "OnUpdateFileOpenThematic"



void CMainFrame::OnFileOpenThematic()
{
	LoadThematicInfo ();
	
} 		// end "OnFileOpenThematic" 



void 
CMainFrame::OnUpdateFileSave(
				CCmdUI* 				pCmdUI)

{                                                                                                                                            
	pCmdUI->Enable( FALSE ); 
	
}

 

void 
CMainFrame::OnActivateApp(
				BOOL 					bActive, 
//				HTASK 				hTask) 
				DWORD 				hTask)

{
	CMDIFrameWnd::OnActivateApp(bActive, hTask);
	
	gInBackground = !bActive; 

	if (!gInBackground)
		{
				// Update the global display bits per pixel in case
				// the user has changed the monitor setting.
				
		CDC				pDC;                                             
		
		if ( pDC.CreateIC((LPCTSTR)_T("DISPLAY"), NULL, NULL, NULL) )
			gDisplayBitsPerPixel = pDC.GetDeviceCaps(BITSPIXEL);
	
		if (gActiveImageViewCPtr != NULL)
			{
					// This instance of MultiSpec was moved to the foreground. If there 
					// is an active image window, force it to be updated.
					
			CMImageDoc* pDoc = gActiveImageViewCPtr->GetDocument();
			pDoc->UpdateAllViews(NULL);
					
			}		// end "if (gActiveImageViewCPtr != NULL)" 

		}		// end "if (!gInBackground)"
	
}		// end "OnActivateApp"



void 
CMainFrame::OnProcUtilCheckTransMatrix()
{                                             
	gProcessorCode = kTransformCheckProcessor;
	EvaluateTransformationControl ();
	gProcessorCode = 0;
	
}		// end "OnProcUtilCheckTransMatrix" 



void 
CMainFrame::OnEditClearTransMatrix()

{
	ClearTransformationMatrix (TRUE);
	
}		// end "ClearTransformMatrix"



void 
CMainFrame::OnFileLoadTransMatrix()

{                                             
	if (gTransformationMatrix.numberChannels > 0)
		WriteTransformationFile ();
		
	else		// gTransformationMatrix.numberChannels <= 0 
		LoadTransformationFile ();
	
}		// end "OnFileLoadTransMatrix"



void 
CMainFrame::OnProcListResults()
{
	gProcessorCode = kListResultsProcessor;
	ListResultsControl (); 
	gProcessorCode = 0;
	
}		// end "OnProcListResults" 



void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{                                                                 
	//if (gProcessorCode == 0 || nID != SC_CLOSE)                   
	if (gProcessorCode != kListDataProcessor)
		CMDIFrameWnd::OnSysCommand(nID, lParam);
}

void CMainFrame::OnUpdateProjectUseleave1outstatistics(CCmdUI* pCmdUI)
{                                                        
	SInt16			checkCode = 0;
	Boolean			enableFlag = FALSE;
	
	
	if (gProjectInfoPtr != NULL)
		{                                            
		enableFlag = TRUE;
		
		if (gProjectInfoPtr->covarianceStatsToUse == kLeaveOneOutStats)
			checkCode = 1; 
		
		}		// end "if (gProjectInfoPtr != NULL)"
		                                      
	pCmdUI->SetCheck(checkCode);                                                                                                                           
	pCmdUI->Enable(enableFlag);
	
}		// end "OnUpdateProjectUseleave1outstatistics"



void CMainFrame::OnUpdateProjectMixofstatisticsused(CCmdUI* pCmdUI)
{                                                                                
	SInt16			checkCode = 0;
	
	
	if (gProjectInfoPtr != NULL)
		{                  
		if (gProjectInfoPtr->covarianceStatsToUse == kMixedStats)
			checkCode = 1; 
		
		}		// end "if (gProjectInfoPtr != NULL)"
		                                      
	pCmdUI->SetCheck(checkCode);                                                                                                                           
	pCmdUI->Enable(FALSE);
	
}		// end "OnUpdateProjectMixofstatisticsused"



void CMainFrame::OnProjUseOriginalStats()
{                                   
	SetProjectCovarianceStatsToUse (kOriginalStats);	
	
}		// end "OnProjUseOriginalStats" 



void CMainFrame::OnProjUseEnhancedStats()
{                                              
	SetProjectCovarianceStatsToUse (kEnhancedStats);
	
}		// end "SetProjectCovarianceStatsToUse"



void CMainFrame::OnProjectUseleave1outstatistics()
{                                              
	SetProjectCovarianceStatsToUse (kLeaveOneOutStats);
	
}		// end "OnProjectUseleave1outstatistics"



void CMainFrame::OnUpdateEditSelectionRectangle(CCmdUI* pCmdUI)
{                                                               
	pCmdUI->Enable( FALSE );
	
}		// end "OnUpdateEditSelectionRectangle"



void CMainFrame::OnProcFeatureExtraction()
{                                                                    
	gMemoryTypeNeeded = 1;                                  
	gProcessorCode = kFeatureExtractionProcessor;
	
	FeatureExtractionControl ();
	                                                      
	gProcessorCode = 0; 
	gMemoryTypeNeeded = 0;
	
}		// end "OnProcFeatureExtraction"



void CMainFrame::OnProcEnhanceStatistics()
{                                                                                          
	gMemoryTypeNeeded = 1;                                  
	gProcessorCode = kStatEnhanceProcessor;
	
	StatisticsEnhanceControl ();
	                                                      
	gProcessorCode = 0; 
	gMemoryTypeNeeded = 0;
	
}		// end "OnProcEnhanceStatistics"



void CMainFrame::OnProjChangeBaseImageFile()
{
	ChangeProjectBaseImage ();
	
}		// end "OnProjChangeBaseImageFile" 



void CMainFrame::OnClose()
{  
	if (gProcessorCode != -1)
		((CMultiSpecApp*)AfxGetApp())->ExitApplication ();
	
	else		// gProcessorCode == -1
		CMDIFrameWnd::OnClose(); 
		
}		// end "OnClose" 



void CMainFrame::OnUpdateWindowNew(CCmdUI* pCmdUI)
{                                                                                                                                                                        
	pCmdUI->Enable(FALSE);
	
}



void CMainFrame::OnEditImageMapParameters() 
{
	CoordinateDialog ();
	
}		// end "OnEditImageMapParameters"



void CMainFrame::OnWindowNewSelectionGraph() 
{
	Boolean						returnFlag = TRUE;


	gProcessorCode = kSelectionGraphProcessor;
		                           
			// Make certain that the graphics code resourses are loaded.		
				
	if (gNumberOfGWindows == 0)
		{
		gMemoryTypeNeeded = 1;
		ForceGraphCodeResourceLoad ();  
				
		}		// end "if (gNumberOfGWindows == 0)"
	
	if (gMemoryTypeNeeded == 0)
		{ 	
				// open a new selection graph window 
		
		CMultiDocTemplate* graphDocTemplatePtr = 
								((CMultiSpecApp*)AfxGetApp())->GetGraphDocTemplate();

		if (graphDocTemplatePtr == NULL || 
									!graphDocTemplatePtr->OpenDocumentFile(NULL))
			returnFlag = FALSE;
		
		}		// end "if (gMemoryTypeNeeded == 0)" 
		
	gMemoryTypeNeeded = 0; 
	gProcessorCode = 0;
	
}		// end "OnWindowNewSelectionGraph"



void CMainFrame::OnUpdateWindowNewSelectionGraph(CCmdUI* pCmdUI) 
{	                                                                                                                            
	pCmdUI->Enable( UpdateWindowSelectionGraph() );
	
}		// end "OnUpdateWindowNewSelectionGraph"



void CMainFrame::OnUpdateWindowShowCoordinateView(CCmdUI* pCmdUI) 
{                                                                                                                                                                                                                              
	pCmdUI->Enable( UpdateWindowCoordinateView (pCmdUI) );
	
}



void CMainFrame::OnWindowShowCoordinateView() 
{
	// TODO: Add your command handler code here
	
}



void CMainFrame::OnUpdateEditClearOverlays(CCmdUI* pCmdUI) 
{                                            
	pCmdUI->Enable( UpdateEditClearOverlays (pCmdUI) );

}		// end "OnUpdateEditClearOverlays"



void CMainFrame::OnUpdateEditClearAllVectorOverlays(CCmdUI* pCmdUI) 
{                                            
	pCmdUI->Enable (gNumberShapeFiles > 0);

}		// end "OnUpdateEditClearAllVectorOverlays"


void CMainFrame::OnEditClearAllImageOverlays() 
{
	CloseAllImageOverlayFiles ();
	
}


void CMainFrame::OnEditClearAllVectorOverlays() 
{
	CloseAllVectorOverlayFiles ();
	
}

void CMainFrame::OnProcReformatChangeHeader() 
{                       
	gProcessorCode = kReformatProcessor;
	ReformatControl(kReformatChangeHeaderRequest);   
	gMemoryTypeNeeded = 0; 
	gProcessorCode = 0;
	
}

void CMainFrame::OnUpdateProcReformatChangeHeader(CCmdUI* pCmdUI) 
	{
	FileInfoPtr							fileInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	
	Handle								fileInfoHandle;
											
														
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
														gActiveImageWindowInfoH,
														kLock,
														kNoMoveHi);
	
		
	fileInfoHandle = GetFileInfoHandle (windowInfoPtr);
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (
														fileInfoHandle, 
														kLock, 
														kNoMoveHi);
															
	pCmdUI->Enable ( UpdateReformatChangeHeader (
													windowInfoPtr, 
													fileInfoPtr, 
													pCmdUI) );
													
	CheckAndUnlockHandle (fileInfoHandle);
	CheckAndUnlockHandle (gActiveImageWindowInfoH);
	
}



void CMainFrame::OnProcReformatChangeImage() 
{
	gProcessorCode = kReformatProcessor;
	ReformatControl(kReformatChangeImageRequest);   
	gMemoryTypeNeeded = 0; 
	gProcessorCode = 0;
	
}



void CMainFrame::OnUpdateProcReformatChangeImage(CCmdUI* pCmdUI) 
{
	Boolean								enableFlag = FALSE;
											
	
	if (gActiveImageWindowInfoH != NULL)
		enableFlag = TRUE;
															
	pCmdUI->Enable (enableFlag);
	
}



void CMainFrame::OnProcReformatConvertEnvi() 
{
	gProcessorCode = kENVIROItoThematicProcessor;
	ENVI_ROIToThematicFileControl ();   
	gMemoryTypeNeeded = 0; 
	gProcessorCode = 0;
	
}



void CMainFrame::OnUpdateProcReformatConvertEnvi(CCmdUI* pCmdUI) 
{                
	Boolean								enableFlag = FALSE;

	if (gFullVersionFlag)
			enableFlag = TRUE;

	pCmdUI->Enable(enableFlag);
	
}



void CMainFrame::OnProcReformatConvertMultispectral() 
{
	// TODO: Add your command handler code here
	
}



void CMainFrame::OnUpdateProcReformatConvertMultispectral(CCmdUI* pCmdUI) 
{                                                                                                                                               
	pCmdUI->Enable(FALSE);
	
}



void CMainFrame::OnProcReformatConvertProject() 
{
	gProcessorCode = kReformatProcessor;
	ReformatControl(kReformatConvertProjectRequest);   
	gMemoryTypeNeeded = 0; 
	gProcessorCode = 0;
	
}



void CMainFrame::OnUpdateProcReformatConvertProject(CCmdUI* pCmdUI) 
{
	Boolean								enableFlag = FALSE;
											
	
	if (gProjectInfoPtr != NULL && gProjectInfoPtr->numberStatisticsClasses > 0)
		enableFlag = TRUE;
															
	pCmdUI->Enable (enableFlag);
	
}



void CMainFrame::OnProcReformatModifyChannel() 
{
	gProcessorCode = kReformatProcessor;
	ReformatControl(kReformatModifyChannelRequest);   
	gMemoryTypeNeeded = 0; 
	gProcessorCode = 0;
	
}



void CMainFrame::OnUpdateProcReformatModifyChannel(CCmdUI* pCmdUI) 
{
	FileInfoPtr							fileInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	
	Handle								fileInfoHandle;
											
														
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
														gActiveImageWindowInfoH,
														kLock,
														kNoMoveHi);
	
		
	fileInfoHandle = GetFileInfoHandle (windowInfoPtr);
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (
														fileInfoHandle, 
														kLock, 
														kNoMoveHi);
															
	pCmdUI->Enable ( UpdateReformatModifyChannel (windowInfoPtr, 
														fileInfoPtr, 
														pCmdUI) );
													
	CheckAndUnlockHandle (fileInfoHandle);
	CheckAndUnlockHandle (gActiveImageWindowInfoH);
	
}		// end "OnUpdateProcReformatModifyChannel"



void CMainFrame::OnProcReformatMosaicImages() 
{
	ReformatControl(kReformatMosaicImagesRequest);  
	gMemoryTypeNeeded = 0; 
	gProcessorCode = 0;
	
}		// end "OnProcReformatMosaicImages"



void CMainFrame::OnUpdateProcReformatMosaicImages(CCmdUI* pCmdUI) 
{                        	                                                                                                                       
	pCmdUI->Enable(UpdateReformatMosaicImages (pCmdUI));
	
}



void CMainFrame::OnProcReformatRecodeThematic() 
{
	gProcessorCode = kReformatProcessor;
	ReformatControl(kReformatRecodeThematicRequest);   
	gMemoryTypeNeeded = 0; 
	gProcessorCode = 0;
	
}		// end "OnProcReformatRecodeThematic"



void CMainFrame::OnUpdateProcReformatRecodeThematic(CCmdUI* pCmdUI) 
{            
	FileInfoPtr							fileInfoPtr;
//	WindowInfoPtr						windowInfoPtr;
	
	Handle								fileInfoHandle;
	
	Boolean								enableFlag = FALSE;
											
														
//	windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
//														gActiveImageWindowInfoH,
//														kLock,
//														kNoMoveHi);
	
		
	fileInfoHandle = GetFileInfoHandle (gActiveImageWindowInfoH);
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (
														fileInfoHandle, 
														kLock, 
														kNoMoveHi);
														
//	if (fileInfoPtr != NULL && 
//			fileInfoPtr->thematicType &&
//				fileInfoPtr->bandInterleave <= kBNonSQMenuItem &&
//					fileInfoPtr->numberBytes >= 1 &&
//						fileInfoPtr->numberBytes <= 2&&
//							fileInfoPtr->gdalDataSetH == NULL)
//		enableFlag = TRUE;
	enableFlag = UpdateReformatRecodeThematicImage (fileInfoPtr);
													
	CheckAndUnlockHandle (fileInfoHandle);
//	CheckAndUnlockHandle (gActiveImageWindowInfoH);  

	pCmdUI->Enable (enableFlag);           
	
}		// end "OnUpdateProcReformatRecodeThematic"



void CMainFrame::OnProcReformatRectifyImage() 
{  
	gProcessorCode = kReformatProcessor;
	ReformatControl(kReformatRectifyImageRequest);        
	gMemoryTypeNeeded = 0; 
	gProcessorCode = 0;                 
	
}		// end "OnProcReformatRectifyImage"



void CMainFrame::OnUpdateProcReformatRectifyImage(CCmdUI* pCmdUI) 
{                          
	Boolean								enableFlag = FALSE;
											
	
	if (gActiveImageWindowInfoH != NULL)
		enableFlag = TRUE;
															
	pCmdUI->Enable (enableFlag);
	
}



void CMainFrame::OnDynamicMenuItem (
				UINT							menuID)

{
	ClearOverlay(menuID - ID_CLEAROVERLAYMENUITEMSTART + 1);

}		// end "OnDynamicMenuItem"



void CMainFrame::OnProcReformatConvertShape() 
{
	gProcessorCode = kReformatProcessor;
	ReformatControl(kReformatConvertShapeRequest);   
	gMemoryTypeNeeded = 0; 
	gProcessorCode = 0;
	
}		// end "OnProcReformatConvertShape"



void CMainFrame::OnUpdateProcReformatConvertShape(CCmdUI* pCmdUI) 
{
	FileInfoPtr							fileInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	
	Handle								fileInfoHandle;
											
														
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
														gActiveImageWindowInfoH,
														kLock,
														kNoMoveHi);
	
		
	fileInfoHandle = GetFileInfoHandle (windowInfoPtr);
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (
														fileInfoHandle, 
														kLock, 
														kNoMoveHi);

	pCmdUI->Enable( UpdateReformatConvertShape (
														windowInfoPtr,
														fileInfoPtr,
														pCmdUI) );
													
	CheckAndUnlockHandle (fileInfoHandle);
	CheckAndUnlockHandle (gActiveImageWindowInfoH);
	
}		// end "OnUpdateProcReformatConvertShape"



void 
CMainFrame::OnUpdateAreaUnitsSqKilometers(
				CCmdUI* 				pCmdUI)

{  
	SetUpdateAreaUnits (pCmdUI, kSqKilometersUnitsMenuItem);
	
}		// end "OnUpdateAreaUnitsSqKilometers"



void 
CMainFrame::OnUpdateAreaUnitsHectares(
				CCmdUI* 				pCmdUI)

{  
	SetUpdateAreaUnits (pCmdUI, kHectareUnitsMenuItem);
	
}		// end "OnUpdateAreaUnitsHectares"



void 
CMainFrame::OnUpdateAreaUnitsSqMeters(
				CCmdUI* 				pCmdUI)

{  
	SetUpdateAreaUnits (pCmdUI, kSqMetersUnitsMenuItem);
	
}		// end "OnUpdateAreaUnitsSqMeters"



void 
CMainFrame::OnUpdateAreaUnitsSqCentimeters(
				CCmdUI* 				pCmdUI)

{  
	SetUpdateAreaUnits (pCmdUI, kSqCentimetersUnitsMenuItem);
	
}		// end "OnUpdateAreaUnitsSqCentimeters"




void 
CMainFrame::OnUpdateAreaUnitsSqMillimeters(
				CCmdUI* 				pCmdUI)

{  
	SetUpdateAreaUnits (pCmdUI, kSqMillimetersUnitsMenuItem);
	
}		// end "OnUpdateAreaUnitsSqMillimeters"



void 
CMainFrame::OnUpdateAreaUnitsSqMicrometers(
				CCmdUI* 				pCmdUI)

{  
	SetUpdateAreaUnits (pCmdUI, kSqMicrometersUnitsMenuItem);
	
}		// end "OnUpdateAreaUnitsSqMicrometers"



void 
CMainFrame::OnUpdateAreaUnitsSqMiles(
				CCmdUI* 				pCmdUI)

{  
	SetUpdateAreaUnits (pCmdUI, kSqMilesUnitsMenuItem);
	
}		// end "OnUpdateAreaUnitsSqMiles"



void 
CMainFrame::OnUpdateAreaUnitsAcres(
				CCmdUI* 				pCmdUI)

{  
	SetUpdateAreaUnits (pCmdUI, kAcresUnitsMenuItem);
	
}		// end "OnUpdateAreaUnitsAcres"



void 
CMainFrame::OnUpdateAreaUnitsSqYards(
				CCmdUI* 				pCmdUI)

{  
	SetUpdateAreaUnits (pCmdUI, kSqYardsUnitsMenuItem);
	
}		// end "OnUpdateAreaUnitsSqYards"



void 
CMainFrame::OnUpdateAreaUnitsSqFeet(
				CCmdUI* 				pCmdUI)

{  
	SetUpdateAreaUnits (pCmdUI, kSqFeetUnitsMenuItem);
	
}		// end "OnUpdateAreaUnitsSqFeet"



void 
CMainFrame::OnUpdateAreaUnitsSqInches(
				CCmdUI* 				pCmdUI)

{  
	SetUpdateAreaUnits (pCmdUI, kSqInchesUnitsMenuItem);
	
}		// end "OnUpdateAreaUnitsSqInches"



void 
CMainFrame::SetUpdateAreaUnits(
				CCmdUI* 				pCmdUI,
				SInt16				unitsCode)

{  
	SInt16			checkCode = 0;

	
	if (gDefaultAreaUnits == unitsCode)
		checkCode = 1;

	pCmdUI->SetCheck(checkCode); 
	pCmdUI->Enable(TRUE);
	
}		// end "OnUpdateAreaUnits"



void CMainFrame::OnOptionsAreaUnitsSqKilometers() 
{
	gDefaultAreaUnits = kSqKilometersUnitsMenuItem;
	
}		// end "OnOptionsAreaUnitsSqKilometers"



void CMainFrame::OnOptionsAreaUnitsHectares() 
{
	gDefaultAreaUnits = kHectareUnitsMenuItem;
	
}		// end "OnOptionsAreaUnitsHectares"



void CMainFrame::OnOptionsAreaUnitsSqMeters() 
{
	gDefaultAreaUnits = kSqMetersUnitsMenuItem;
	
}		// end "OnOptionsAreaUnitsSqMeters"



void CMainFrame::OnOptionsAreaUnitsSqCentimeters() 
{
	gDefaultAreaUnits = kSqCentimetersUnitsMenuItem;
	
}		// end "OnOptionsAreaUnitsSqCentimeters"



void CMainFrame::OnOptionsAreaUnitsSqMillimeters() 
{
	gDefaultAreaUnits = kSqMillimetersUnitsMenuItem;
	
}		// end "OnOptionsAreaUnitsSqMillimeters"



void CMainFrame::OnOptionsAreaUnitsSqMicrometers() 
{
	gDefaultAreaUnits = kSqMicrometersUnitsMenuItem;
	
}		// end "OnOptionsAreaUnitsSqMicrometers"



void CMainFrame::OnOptionsAreaUnitsSqMiles() 
{
	gDefaultAreaUnits = kSqMilesUnitsMenuItem;
	
}		// end "OnOptionsAreaUnitsSqMiles"



void CMainFrame::OnOptionsAreaUnitsAcres() 
{
	gDefaultAreaUnits = kAcresUnitsMenuItem;
	
}		// end "OnOptionsAreaUnitsAcres"



void CMainFrame::OnOptionsAreaUnitsSqYards() 
{
	gDefaultAreaUnits = kSqYardsUnitsMenuItem;
	
}		// end "OnOptionsAreaUnitsSqYards"



void CMainFrame::OnOptionsAreaUnitsSqFeet() 
{
	gDefaultAreaUnits = kSqFeetUnitsMenuItem;
	
}		// end "OnOptionsAreaUnitsSqFeet"



void CMainFrame::OnOptionsAreaUnitsSqInches() 
{
	gDefaultAreaUnits = kSqInchesUnitsMenuItem;
	
}		// end "OnOptionsAreaUnitsSqInches"

void CMainFrame::OnOptionsSwitchcrosscursor()
{
	if (gCrossCursorID == IDC_CROSS_CURSOR)
		gCrossCursorID = IDC_CROSS_CURSOR2;

	else		// gCrossCursorID != IDC_CROSS_CURSOR
		gCrossCursorID = IDC_CROSS_CURSOR;

}		// end "OnOptionsSwitchcrosscursor"
