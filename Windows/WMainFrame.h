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
//	File:						WMainFrame.h
//	Implementation:		WMainFrame.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMainFrame class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 01/03/2020
//
//------------------------------------------------------------------------------------

#pragma once
	                     
#include "WStatisticsView.h"
#include "WToolBar.h"
	
class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC (CMainFrame)
	public:
		CMainFrame ();
	
		virtual ~CMainFrame ();
	
		void DoPaletteChanged (
				CWnd*									cWndPtr);
	
	#ifdef _DEBUG
		virtual void AssertValid () const;
		
		virtual void Dump (
				CDumpContext& 							dc) const;
	#endif
	
	protected:
		Boolean GetEnableFlagForStatisticsAndCluster (void);
	
		void OnDynamicMenuItem (
				UINT									menuID);
	
		void SetUpdateAreaUnits (
				CCmdUI* 								pCmdUI,
				SInt16								unitsCode);

		// Generated message map functions
		//{{AFX_MSG (CMainFrame)
		afx_msg void OnActivateApp (
				BOOL 									bActive,
				DWORD 								hTask);
	
		afx_msg void OnChar (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnClose ();
	
		afx_msg int OnCreate (
				LPCREATESTRUCT 					lpCreateStruct);
	
		afx_msg void OnPaletteChanged (
				CWnd* 								pFocusWnd);
	
		afx_msg BOOL OnQueryNewPalette ();
	
		afx_msg void OnSysCommand (
				UINT 									nID,
				LPARAM 								lParam);
	
		afx_msg BOOL OnSetCursor (
				CWnd* 								pWnd,
				UINT 									nHitTest,
				UINT 									message);
	
		// In order of location on the main menu
				// File Menu
		afx_msg void OnFileNewProject ();
	
		afx_msg void OnFileOpen ();
	
		afx_msg void OnFileOpenProject ();
	
		afx_msg void OnFileOpenProjectImage ();
	
		afx_msg void OnFileOpenThematic ();
	
		afx_msg void OnFileLoadTransMatrix ();
	
		afx_msg void OnFileCloseProject ();
	
		afx_msg void OnFileSaveAs ();
	
		afx_msg void OnFileSaveProject ();
	
		afx_msg void OnFileSaveProjectAs ();
	
				// Edit Menu
		afx_msg void OnEditClearTransMatrix ();
	
		afx_msg void OnEditImageDescription ();
	
		afx_msg void OnEditImageMapParameters ();
	
		afx_msg void OnEditClearAllImageOverlays ();
	
		afx_msg void OnEditClearAllVectorOverlays ();
	
				// View Menu Items
		afx_msg void OnWindowShowCoordinateView ();
	
				// Project Menu Items
		afx_msg void OnProjUseOriginalStats ();
	
		afx_msg void OnProjUseleave1outstatistics ();
	
		afx_msg void OnProjUseEnhancedStats ();
	
		afx_msg void OnProjClearStats ();
	
		afx_msg void OnProjChangeBaseImageFile ();
	
		afx_msg void OnProjAddAsAssociatedImage ();
	
				// Processor Menu Items
		afx_msg void OnProcDisplayImage ();
	
		afx_msg void OnProcHistogramImage ();
	
		afx_msg void OnProcListData ();
	
		afx_msg void OnProcReformat ();
	
		afx_msg void OnProcCluster ();
	
		afx_msg void OnProcStatistics ();
	
		afx_msg void OnProcEnhanceStatistics ();
	
		afx_msg void OnProcFeatureExtraction ();
	
		afx_msg void OnProcFeatureSelection ();
	
		afx_msg void OnProcClassify ();
	
		afx_msg void OnProcListResults ();
	
				// Processor->Reformat Menu Items
		afx_msg void OnProcReformatChangeHeader ();
	
		afx_msg void OnProcReformatChangeImage ();
	
		afx_msg void OnProcReformatConvertMultispectral ();
	
		afx_msg void OnProcReformatConvertProject ();
	
		afx_msg void OnProcReformatConvertShape ();
	
		afx_msg void OnProcReformatModifyChannel ();
	
		afx_msg void OnProcReformatMosaicImages ();
	
		afx_msg void OnProcReformatRecodeThematic ();
	
		afx_msg void OnProcReformatRectifyImage ();
	
		afx_msg void OnProcReformatConvertEnvi ();
	
				// Processor->Utility Menu Items
		afx_msg void OnProcUtilPrinCompAnalysis ();
	
		afx_msg void OnProcUtilCreateStatImage ();
	
		afx_msg void OnProcUtilBiplotsOfData ();
	
		afx_msg void OnProcUtilListImageDesc ();
	
		afx_msg void OnProcUtilCheckCovariances ();
	
		afx_msg void OnProcUtilCheckTransMatrix ();
	
		afx_msg void OnNewSelectGraph ();
	
				// Options Menu Items
		afx_msg void OnOptionsAreaUnitsSqKilometers ();
	
		afx_msg void OnOptionsAreaUnitsHectares ();
	
		afx_msg void OnOptionsAreaUnitsSqMeters ();
	
		afx_msg void OnOptionsAreaUnitsSqCentimeters ();
	
		afx_msg void OnOptionsAreaUnitsSqMillimeters ();

		afx_msg void OnOptionsAreaUnitsSqMicrometers ();
	
		afx_msg void OnOptionsAreaUnitsSqMiles ();
	
		afx_msg void OnOptionsAreaUnitsAcres ();
	
		afx_msg void OnOptionsAreaUnitsSqYards ();
	
		afx_msg void OnOptionsAreaUnitsSqFeet ();
	
		afx_msg void OnOptionsAreaUnitsSqInches ();
	
		afx_msg void OnOptionsSwitchcrosscursor ();
	
				// Window Menu Items
		afx_msg void OnWindowNewSelectionGraph ();
	
	
		// In order of location on the main menu
				// File Menu Items
		afx_msg void OnUpdateFileNewProject (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFileOpen (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFileOpenProject (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFileOpenProjectImage (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFileOpenThematic (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFileCloseProject (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFileLoadTransMatrix (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFileSave (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFileSaveProject (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFileSaveProjectAs (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFilePrintPreview (
				CCmdUI* 								pCmdUI);
	
				// Edit Menu Items
		afx_msg void OnUpdateEditUndo (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditCut (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditCopy (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditPaste (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditClear (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditSelectAll (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditSelectionRectangle (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditClearSelectionRectangle (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditClearTransMatrix (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditImageDescription (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditImageMapParameters (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditClearOverlays (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditClearAllVectorOverlays (
				CCmdUI* 								pCmdUI);
	
				// View Menu Items
		afx_msg void OnUpdateViewCoordinatesBar (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateWindowShowCoordinateView (
				CCmdUI* 								pCmdUI);
	
				// Project Menu Items
		afx_msg void OnUpdateProjUseOriginalStats (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProjUseleave1outstatistics (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProjUseEnhancedStats (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProjMixofstatisticsused (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProjClearStats (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProjChangeBaseImageFile (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProjAddAsAssociatedImage (
				CCmdUI* 								pCmdUI);
	
				// Processor Menu Items
		afx_msg void OnUpdateProcDisplayImage (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcHistogramImage (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcListData (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcReformat (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcCluster (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcStatistics (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcEnhanceStatistics (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcFeatureExtraction (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcFeatureSelection (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcClassify (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcListResults (
				CCmdUI* 								pCmdUI);
	
				// Processor->Reformat Menu Items
		afx_msg void OnUpdateProcReformatChangeHeader (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcReformatChangeImage (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcReformatConvertMultispectral (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcReformatConvertProject (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcReformatConvertShape (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcReformatModifyChannel (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcReformatMosaicImages (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcReformatRecodeThematic (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcReformatRectifyImage (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcReformatConvertEnvi (
				CCmdUI* 								pCmdUI);
	
				// Processor->Utility Menu Items
		afx_msg void OnUpdateProcUtilPrincipalComponentAnalysis (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcUtilCreateStatImage (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcUtilBiplotsOfData (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcUtilListImageDesc (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcUtilCheckCovariances (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateProcUtilCheckTransMatrix (
				CCmdUI* 								pCmdUI);
	
				// Options Menu Items
		afx_msg void OnUpdateOptionsMemoryStatus (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateOptionsAreaUnitsAcres (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateOptionsAreaUnitsHectares (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateOptionsAreaUnitsSqCentimeters (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateOptionsAreaUnitsSqFeet (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateOptionsAreaUnitsSqInches (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateOptionsAreaUnitsSqKilometers (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateOptionsAreaUnitsSqMeters (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateOptionsAreaUnitsSqMicrometers (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateOptionsAreaUnitsSqMiles (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateOptionsAreaUnitsSqMillimeters (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateOptionsAreaUnitsSqYards (
				CCmdUI* 								pCmdUI);
	
				// Windows Menu Items
		afx_msg void OnUpdateWindowNew (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateWindowNewSelectionGraph (
				CCmdUI* 								pCmdUI);
	
			// Tool Bar Control Items
		afx_msg void OnUpdateToolBarMagnification (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateToolBarZoomIn (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateToolBarZoomIndicator (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateToolBarZoomOut (
				CCmdUI* 								pCmdUI);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		CMToolBar    						m_wndToolBar;
		CStatusBar  						m_wndStatusBar;
	
};	// end class CMainFrame
