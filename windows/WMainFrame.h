// WMainFrame.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
                
#if !defined __MAINFRM_H__
	#define __MAINFRM_H__    
	                     
	#include "WStatisticsView.h"
	#include "WToolBar.h" 
	
class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
                 
	virtual ~CMainFrame();
	
	void		DoPaletteChanged(
					CWnd*				cWndPtr);
				
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif                                     
        
protected:
	Boolean GetEnableFlagForStatisticsAndCluster(void);
	
	void OnDynamicMenuItem (
				UINT							menuID);
	
	void SetUpdateAreaUnits(
				CCmdUI* 				pCmdUI,
				SInt16				unitsCode);
//	void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hMenu);

//	void OnExitMenuLoop(
//				Boolean				isPopupMenuFlag);

protected:  // control bar embedded members
	CStatusBar  			m_wndStatusBar;
	CMToolBar    			m_wndToolBar;       

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)          
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpenProjectImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpenThematic(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditImageDescription(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLoadTransMatrix(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClassify(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCluster(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplayImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEnhanceStatistics(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFeatureExtraction(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFeatureSelection(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHistogramImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateListdata(CCmdUI* pCmdUI);
	afx_msg void OnUpdateListResults(CCmdUI* pCmdUI);
	afx_msg void OnUpdateReformat(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatistics(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBiplotsOfData(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCheckCovariances(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCheckTransMatrix(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCreateStatImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateListImageDesc(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePrincipalComponentAnalysis(CCmdUI* pCmdUI);
	afx_msg void OnDisplayImage();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);          
	afx_msg void OnUpdateClearSelectionRectangle(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClearTransMatrix(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMemoryStatus(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewSelectGraph(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectGraphOptions(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInvertPalette(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowClasses(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowInformationGroups(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddAsAssociatedImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChangeBaseImageFile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClearStats(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCloseProject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewProject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOpenProject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveProject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveProjectAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUseEnhancedStats(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUseOriginalStats(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI); 
	afx_msg void OnUpdateMagnification(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomIn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomOut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnEditImageDescription();
	afx_msg void OnHistogramImage();
	afx_msg void OnUpdateZoomIndicator(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditImageMapParameters(CCmdUI* pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnNewSelectGraph();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnProcReformat();
	afx_msg void OnFileOpen();
	afx_msg void OnOpenProject();
	afx_msg void OnProcUtilListImageDesc();
	afx_msg void OnProcUtilCheckCovariances();
	afx_msg void OnProjCloseProject();
	afx_msg void OnProjClearStats();
	afx_msg void OnProcStatistics();
	afx_msg void OnProcClassify();
	afx_msg void OnProcUtilCreateStatImage();
	afx_msg void OnUpdateViewCoordinatesBar(CCmdUI* pCmdUI);
	afx_msg void OnFileOpenProjectImage();
	afx_msg void OnProjAddAsAssociatedImage();
	afx_msg void OnProjSaveProject();
	afx_msg void OnProjSaveProjectAs();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPalShowClasses();
	afx_msg void OnPalShowInformationGroups();
	afx_msg void OnProcCluster();
	afx_msg void OnProjNewProject();
	afx_msg void OnProcUtilPrinCompAnalysis();
	afx_msg void OnProcListdata();
	afx_msg void OnProcFeatureSelection();
	afx_msg void OnProcUtilBiplotsOfData();               
	afx_msg void OnFileOpenThematic();
	afx_msg void OnFileSaveAs();                  
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
	afx_msg void OnProcUtilCheckTransMatrix();
	afx_msg void OnEditClearTransMatrix();                    
	afx_msg BOOL OnQueryNewPalette(); 
	afx_msg void OnMagnification(void);
	afx_msg void OnFileLoadTransMatrix();
	afx_msg void OnProcListResults();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnUpdateProjectUseleave1outstatistics(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProjectMixofstatisticsused(CCmdUI* pCmdUI);
	afx_msg void OnProjUseOriginalStats();
	afx_msg void OnProjUseEnhancedStats();
	afx_msg void OnProjectUseleave1outstatistics();
	afx_msg void OnUpdateEditSelectionRectangle(CCmdUI* pCmdUI);
	afx_msg void OnProcFeatureExtraction();
	afx_msg void OnProcEnhanceStatistics();
	afx_msg void OnProjChangeBaseImageFile();
	afx_msg void OnClose();
	afx_msg void OnUpdateWindowNew(CCmdUI* pCmdUI);
	afx_msg void OnEditImageMapParameters();
	afx_msg void OnWindowNewSelectionGraph();
	afx_msg void OnUpdateWindowNewSelectionGraph(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWindowShowCoordinateView(CCmdUI* pCmdUI);
	afx_msg void OnWindowShowCoordinateView();
	afx_msg void OnUpdateEditClearOverlays(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditClearAllVectorOverlays(CCmdUI* pCmdUI);
	afx_msg void OnEditClearAllImageOverlays();
	afx_msg void OnEditClearAllVectorOverlays();
	afx_msg void OnProcReformatChangeHeader();
	afx_msg void OnUpdateProcReformatChangeHeader(CCmdUI* pCmdUI);
	afx_msg void OnProcReformatChangeImage();
	afx_msg void OnUpdateProcReformatChangeImage(CCmdUI* pCmdUI);
	afx_msg void OnProcReformatConvertEnvi();
	afx_msg void OnUpdateProcReformatConvertEnvi(CCmdUI* pCmdUI);
	afx_msg void OnProcReformatConvertMultispectral();
	afx_msg void OnUpdateProcReformatConvertMultispectral(CCmdUI* pCmdUI);
	afx_msg void OnProcReformatConvertProject();
	afx_msg void OnUpdateProcReformatConvertProject(CCmdUI* pCmdUI);
	afx_msg void OnProcReformatModifyChannel();
	afx_msg void OnUpdateProcReformatModifyChannel(CCmdUI* pCmdUI);
	afx_msg void OnProcReformatMosaicImages();
	afx_msg void OnUpdateProcReformatMosaicImages(CCmdUI* pCmdUI);
	afx_msg void OnProcReformatRecodeThematic();
	afx_msg void OnUpdateProcReformatRecodeThematic(CCmdUI* pCmdUI);
	afx_msg void OnProcReformatRectifyImage();
	afx_msg void OnUpdateProcReformatRectifyImage(CCmdUI* pCmdUI);
	afx_msg void OnProcReformatConvertShape();
	afx_msg void OnUpdateProcReformatConvertShape(CCmdUI* pCmdUI);

	afx_msg void OnUpdateAreaUnitsSqKilometers(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAreaUnitsHectares(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAreaUnitsSqMeters(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAreaUnitsSqCentimeters(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAreaUnitsSqMillimeters(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAreaUnitsSqMicrometers(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAreaUnitsSqMiles(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAreaUnitsAcres(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAreaUnitsSqYards(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAreaUnitsSqFeet(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAreaUnitsSqInches(CCmdUI* pCmdUI);

	afx_msg void OnOptionsAreaUnitsSqKilometers();
	afx_msg void OnOptionsAreaUnitsHectares();
	afx_msg void OnOptionsAreaUnitsSqMeters();
	afx_msg void OnOptionsAreaUnitsSqCentimeters();
	afx_msg void OnOptionsAreaUnitsSqMillimeters();
	afx_msg void OnOptionsAreaUnitsSqMicrometers();
	afx_msg void OnOptionsAreaUnitsSqMiles();
	afx_msg void OnOptionsAreaUnitsAcres();
	afx_msg void OnOptionsAreaUnitsSqYards();
	afx_msg void OnOptionsAreaUnitsSqFeet();
	afx_msg void OnOptionsAreaUnitsSqInches();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOptionsSwitchcrosscursor();
};

/////////////////////////////////////////////////////////////////////////////
                     
#endif	// !defined __MAINFRM_H__
