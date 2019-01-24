// LMainFrame.h
//
// Revised by Larry L Biehl    01/24/2019

#ifndef __LMainFrame__
#	define __LMainFrame__

#include "LOpenFileDialog.h"

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

///////////////////////////////////////////////////////////////////////////////
/// Class CMainFrame
///////////////////////////////////////////////////////////////////////////////
#define CMMainFrameTitle _("MultiSpec")

class CMainFrame : public wxDocParentFrame

{
	DECLARE_CLASS(CMainFrame)
	protected:
		wxMenu*						filemenu;
		wxMenu*						editmenu;
		wxMenu*						m_editClearOverlayMenu;
		wxMenu*						viewmenu;
		wxMenu*						projectmenu;
		wxMenu*						processormenu;
		wxMenu*						m_menu2;
		wxMenu*						m_menu3;
		wxMenu*						optionsmenu;
		wxMenu*						m_menu4;
		wxMenu*						m_menu5;
		wxMenu*						windowmenu;
		wxMenu*						helpmenu;
		//wxStatusBar*				statusBar;
		wxStaticText*           m_zoomText;
		wxToolBarToolBase*		m_zoomInTool;
		wxToolBarToolBase*		m_zoomOutTool;
		time_t 						m_nextControlTime;

		SInt16						m_imageZoomCode;
		Boolean						m_controlDelayFlag;
		Boolean						m_TOOL_PARAMETER_file_flag;
	
		bool							m_cancelOperationEventFlag;
		bool							m_optionOverlayFlag;
		bool							m_tooltipFlag;
#ifndef multispec_wxmac
    enum{
      ID_FILE_SAVE,
      ID_FILE_SAVE_AS
    };
#endif
   

	Boolean 	GetEnableFlagForStatisticsAndCluster (void);
	
	void OnActivate(wxActivateEvent& event);
	
	void OnIdle(wxIdleEvent& event);

	void OnUpdateFileOpen(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFileOpenProjectImage(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFileOpenThematic(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFileLoadTransMatrix(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFileSave(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFileSaveAs(wxUpdateUIEvent& event);
	void OnUpdateFileCloseWindow(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFilePrint(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFilePrintPreview(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFilePrintSetup(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFileNewProject(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFileOpenProject(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFileSaveProject(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFileSaveProjectAs(wxUpdateUIEvent& pCmdUI);
	
	void OnUpdateEditUndo(wxUpdateUIEvent& pCmdUI);
	void OnUpdateEditCut(wxUpdateUIEvent& pCmdUI);
	void OnUpdateEditCopy(wxUpdateUIEvent& pCmdUI);
	void OnUpdateEditPaste(wxUpdateUIEvent& pCmdUI);
	void OnUpdateEditClear(wxUpdateUIEvent& pCmdUI);
	void OnUpdateEditSelectAll (wxUpdateUIEvent& event);
	void OnUpdateEditClearSelectionRectangle(wxUpdateUIEvent& event);
	void OnUpdateEditImageDescription(wxUpdateUIEvent& pCmdUI);
	
	void OnUpdateProjectAddAsAssociatedImage(wxUpdateUIEvent& pCmdUI);
	void OnUpdateProjectChangeBaseImageFile(wxUpdateUIEvent& pCmdUI);
	void OnUpdateProjectClearStats(wxUpdateUIEvent& pCmdUI);
	void OnUpdateProjectMixofstatisticsused(wxUpdateUIEvent& pCmdUI);
	void OnUpdateProjectUseEnhancedStats(wxUpdateUIEvent& pCmdUI);
	void OnUpdateProjectUseLeave1OutStatistics(wxUpdateUIEvent& pCmdUI);
	void OnUpdateProjectUseOriginalStats(wxUpdateUIEvent& pCmdUI);
	
	void OnUpdateDisplayImage(wxUpdateUIEvent& pCmdUI);
	void OnUpdateCluster(wxUpdateUIEvent& pCmdUI);
	void OnUpdateClassify(wxUpdateUIEvent& pCmdUI);
	void OnUpdateEnhanceStatistics(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFeatureExtraction(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFeatureSelection(wxUpdateUIEvent& pCmdUI);
	void OnUpdateHistogramImage(wxUpdateUIEvent& pCmdUI);
	void OnUpdateListdata(wxUpdateUIEvent& pCmdUI);
	void OnUpdateListResults(wxUpdateUIEvent& pCmdUI);
	void OnUpdateReformat(wxUpdateUIEvent& pCmdUI);
	void OnUpdateStatistics(wxUpdateUIEvent& pCmdUI);
	void OnUpdateBiplotsOfData(wxUpdateUIEvent& pCmdUI);
	void OnUpdateCheckCovariances(wxUpdateUIEvent& pCmdUI);
	void OnUpdateCheckTransMatrix(wxUpdateUIEvent& pCmdUI);
	void OnUpdateCreateStatImage(wxUpdateUIEvent& pCmdUI);
	void OnUpdateListImageDesc(wxUpdateUIEvent& pCmdUI);
	void OnUpdatePrincipalComponentAnalysis(wxUpdateUIEvent& pCmdUI);
	void OnUpdateClearSelectionRectangle(wxUpdateUIEvent& pCmdUI);
	void OnUpdateClearTransMatrix(wxUpdateUIEvent& pCmdUI);
	void OnUpdateMemoryStatus(wxUpdateUIEvent& pCmdUI);
	void OnUpdateNewSelectGraph(wxUpdateUIEvent& pCmdUI);
	void OnUpdateSelectGraphOptions(wxUpdateUIEvent& pCmdUI);
	void OnUpdateInvertPalette(wxUpdateUIEvent& pCmdUI);
	void OnUpdateShowClasses(wxUpdateUIEvent& pCmdUI);
	void OnUpdateShowInformationGroups(wxUpdateUIEvent& pCmdUI);
	
	void OnUpdateWindowShowCoordinateView(wxUpdateUIEvent& pCmdUI);
	void OnUpdateWindowNewSelectionGraph(wxUpdateUIEvent& pCmdUI);
	void OnUpdateWindowTextOutput(wxUpdateUIEvent& pCmdUI);
	void OnUpdateWindowProject(wxUpdateUIEvent& pCmdUI);
	
	void OnUpdateToolBarMagnification (wxUpdateUIEvent& event);
	void OnUpdateToolBarZoomIn (wxUpdateUIEvent& event);
	void OnUpdateToolBarZoomOut (wxUpdateUIEvent& event);
	void OnUpdateZoomInfo(wxUpdateUIEvent& event);
	void OnUpdateToolBarOverlay (wxUpdateUIEvent& event);
	
	void OnFileCloseWindow(wxCommandEvent& event);
	void OnFileOpen(wxCommandEvent& event);
	
	void OnDisplayImage(wxCommandEvent& event);

	void OnEditCut(wxCommandEvent& event);
	void OnEditPaste(wxCommandEvent& event);
	void OnEditUndo(wxCommandEvent& event);
	void OnEditSelectAll (wxCommandEvent& event);
	void OnEditSelectionRectangle(wxCommandEvent& event);
	void OnEditClearSelectionRectangle(wxCommandEvent& event);
	void OnEditClearTransMatrix(wxCommandEvent& event);
	void OnEditImageDescription(wxCommandEvent& event);
	void OnHistogramImage(wxCommandEvent& event);
	void OnZoom(wxCommandEvent& event);
	void OnZoomInMouseDown(wxMouseEvent& event);
	void OnZoomInMouseDown2(wxCommandEvent& event);
	void ActiveViewUpdate(wxCommandEvent& event);

	void OnUpdateZoomIndicator(wxUpdateUIEvent& pCmdUI);
	void OnUpdateEditImageMapParameters(wxUpdateUIEvent& pCmdUI);

	void OnNewSelectGraph(wxCommandEvent& event);
	void OnProcReformat(wxCommandEvent& event);
	void OnOpenProject(wxCommandEvent& event);
	void OnPaletteChanged(wxWindow* pFocusWnd);
	void OnProcUtilCheckCovariances(wxCommandEvent& event);
	void OnProjCloseProject(wxCommandEvent& event);
	void OnProjClearStats(wxCommandEvent& event);
	void OnProcStatistics(wxCommandEvent& event);
	void OnProcClassify(wxCommandEvent& event);
	void OnUpdateViewToolBar(wxUpdateUIEvent& pCmdUI);
	void OnUpdateViewStatusBar(wxUpdateUIEvent& pCmdUI);
	void OnUpdateViewCoordinatesBar(wxUpdateUIEvent& pCmdUI);
	void OnFileOpenProjectImage(wxCommandEvent& event);
	void OnProjAddAsAssociatedImage(wxCommandEvent& event);
	void OnProjSaveProject(wxCommandEvent& event);
	void OnProjSaveProjectAs(wxCommandEvent& event);
	void OnChar (wxKeyEvent& event);
	void OnPalShowClasses(wxCommandEvent& event);
	void OnPalShowInformationGroups(wxCommandEvent& event);
	void OnProcCluster(wxCommandEvent& event);
	void OnProjNewProject(wxCommandEvent& event);
	void OnProcUtilPrinCompAnalysis(wxCommandEvent& event);
	void OnProcUtilCreateStatImage(wxCommandEvent& event);
	void OnProcListdata(wxCommandEvent& event);
	void OnProcFeatureSelection(wxCommandEvent& event);
	void OnProcUtilBiplotsOfData(wxCommandEvent& event);
	void OnFileOpenThematic(wxCommandEvent& event);
	void OnFileSave(wxCommandEvent& event);
	void OnFileSaveAs(wxCommandEvent& event);
   void OnExportFile(wxCommandEvent& event);
	//void OnActivateApp(bool bActive, DWORD hTask);
	void OnProcUtilCheckTransMatrix(wxCommandEvent& event);
	bool OnQueryNewPalette();
	void OnMagnification(void);
	void OnFileLoadTransMatrix(wxCommandEvent& event);
	void OnProcListResults(wxCommandEvent& event);
	//void OnSysCommand(unsigned int nID, LPARAM lParam);
	void OnProjUseOriginalStats(wxCommandEvent& event);        
	void OnProjUseEnhancedStats(wxCommandEvent& event);
	void OnProjectUseleave1outstatistics(wxCommandEvent& event);
	void OnUpdateEditSelectionRectangle(wxUpdateUIEvent& pCmdUI);
	void OnProcFeatureExtraction(wxCommandEvent& event);
	void OnProcEnhanceStatistics(wxCommandEvent& event);
	void OnProjChangeBaseImageFile(wxCommandEvent& event);   
   /*     
	void OnUpdateProjectListStatistics(wxUpdateUIEvent& pCmdUI); //temporary
	void OnUpdateProjectHistogramStatistics(wxUpdateUIEvent& pCmdUI); //temporary
	void OnProjectListStatistics(wxCommandEvent& event); //temporary
	void OnProjectHistogramStatistics(wxCommandEvent& event); //temporary
	*/	       
	void OnClose();
	void OnUpdateWindowNew(wxUpdateUIEvent& pCmdUI);
	void OnEditImageMapParameters(wxCommandEvent& event);
	void OnUpdateEditClearOverlays(wxUpdateUIEvent& pCmdUI);
	void OnUpdateEditClearAllVectorOverlays(wxUpdateUIEvent& pCmdUI);
	void OnEditClearAllImageOverlays(wxCommandEvent& event);
	void OnEditClearAllVectorOverlays(wxCommandEvent& event);
	void OnEditClearSelectedOverlay(wxCommandEvent& event);
//	void OnProcReformatChangeHeader(wxCommandEvent& event);
//	void OnUpdateProcReformatChangeHeader(wxUpdateUIEvent& pCmdUI);
	void OnProcReformatChangeImage(wxCommandEvent& event);
	void OnUpdateProcReformatChangeImage(wxUpdateUIEvent& pCmdUI);
	void OnProcReformatConvertEnvi(wxCommandEvent& event);
	void OnUpdateProcReformatConvertEnvi(wxUpdateUIEvent& pCmdUI);
//	void OnProcReformatConvertMultispectral(wxCommandEvent& event);
//	void OnUpdateProcReformatConvertMultispectral(wxUpdateUIEvent& pCmdUI);
	void OnProcReformatConvertProject(wxCommandEvent& event);
	void OnUpdateProcReformatConvertProject(wxUpdateUIEvent& pCmdUI);
	void OnProcReformatModifyChannel(wxCommandEvent& event);
	void OnUpdateProcReformatModifyChannel(wxUpdateUIEvent& pCmdUI);
	void OnProcReformatMosaicImages(wxCommandEvent& event);
	void OnUpdateProcReformatMosaicImages(wxUpdateUIEvent& pCmdUI);
	void OnProcReformatRecodeThematic(wxCommandEvent& event);
	void OnUpdateProcReformatRecodeThematic(wxUpdateUIEvent& pCmdUI);
	void OnProcReformatRectifyImage(wxCommandEvent& event);
	void OnUpdateProcReformatRectifyImage(wxUpdateUIEvent& pCmdUI);
	void OnProcReformatConvertShape(wxCommandEvent& event);
	void OnUpdateProcReformatConvertShape(wxUpdateUIEvent& pCmdUI);

	void OnUpdateAreaUnitsSqKilometers(wxUpdateUIEvent& pCmdUI);
	void OnUpdateAreaUnitsHectares(wxUpdateUIEvent& pCmdUI);
	void OnUpdateAreaUnitsSqMeters(wxUpdateUIEvent& pCmdUI);
	void OnUpdateAreaUnitsSqCentimeters(wxUpdateUIEvent& pCmdUI);
	void OnUpdateAreaUnitsSqMillimeters(wxUpdateUIEvent& pCmdUI);
	void OnUpdateAreaUnitsSqMicrometers(wxUpdateUIEvent& pCmdUI);
	void OnUpdateAreaUnitsSqMiles(wxUpdateUIEvent& pCmdUI);
	void OnUpdateAreaUnitsAcres(wxUpdateUIEvent& pCmdUI);
	void OnUpdateAreaUnitsSqYards(wxUpdateUIEvent& pCmdUI);
	void OnUpdateAreaUnitsSqFeet(wxUpdateUIEvent& pCmdUI);
	void OnUpdateAreaUnitsSqInches(wxUpdateUIEvent& pCmdUI);
        void OnUpdateShowToolTip(wxUpdateUIEvent& pCmdUI);
	void OnUpdateOptionsSwitchcrosscursor(wxUpdateUIEvent& pCmdUI);

	void OnOptionsAreaUnitsSqKilometers(wxCommandEvent& event);
	void OnOptionsAreaUnitsHectares(wxCommandEvent& event);
	void OnOptionsAreaUnitsSqMeters(wxCommandEvent& event);
	void OnOptionsAreaUnitsSqCentimeters(wxCommandEvent& event);
	void OnOptionsAreaUnitsSqMillimeters(wxCommandEvent& event);
	void OnOptionsAreaUnitsSqMicrometers(wxCommandEvent& event);
	void OnOptionsAreaUnitsSqMiles(wxCommandEvent& event);
	void OnOptionsAreaUnitsAcres(wxCommandEvent& event);
	void OnOptionsAreaUnitsSqYards(wxCommandEvent& event);
	void OnOptionsAreaUnitsSqFeet(wxCommandEvent& event);
	void OnOptionsAreaUnitsSqInches(wxCommandEvent& event);
	
	void OnViewCoordinatesBar(wxCommandEvent& event);
	
	void OnWindowShowCoordinateView(wxCommandEvent& event);
	void OnWindowSelectionNewGraph(wxCommandEvent& event);
        void OnShowToolTip(wxCommandEvent& event);        
	void OnWindowOutputWindowSelection(wxCommandEvent& event);
	void OnWindowProjectWindowSelection(wxCommandEvent& event);
	void OnWindowIorGWindowSelection(wxCommandEvent& event);

	void OnAbout(wxCommandEvent& event);

	SInt32 OpenImageFile (LocalAppFile* localAppFilePtr,
									wxDocument** documentPtrPtr,
									Boolean fromOpenProjectImageWindowFlag, 
									UInt16 hdfDataSetSelection);
	
	void UpdateWindowMenuList ();
	/*
	enum{
		//ID_IMAGE_OPEN
               // ID_PROC_UTIL_LIST_IMAGE_DESC
          //ID_SHOWOVERLAYMENUITEMSTART

	}; */
	public:
		CMainFrame (wxDocManager *manager,
						wxDocParentFrame *frame,
						const wxString& title = wxT("MultiSpec"),
						const wxPoint& pos = wxDefaultPosition,
						const wxSize& size = wxSize( 800,600 ),
						long type=wxDEFAULT_FRAME_STYLE);
	
		Boolean	GetCancelOperationEventFlag (void);
		time_t GetNextControlTime () {return (m_nextControlTime);}
		SInt16 GetZoomCode (void);
		void OnShowOverlay(wxCommandEvent& event);
		void OnAsyncTermination (FileUploadProcess *process);
      void OnOptionsSwitchcrosscursor(wxCommandEvent& event);
		void OnOverlaySelection(wxCommandEvent& event);
		void OnProcUtilListImageDesc(wxCommandEvent& event);
		void SetNextControlTime (UInt32 offset);
		void SetZoomCode(SInt16 zoomCode);
		void SetToolParametersFlag(Boolean TOOL_PARAMETER_file_flag);
		void UpdateStatusBar (CMImageFrame* imageFrame, double magnification);
		//void OnImageOpen(wxCommandEvent& event);
		~CMainFrame();
	
		wxMenuBar*					m_menubar1;
		wxToolBar*					m_toolBar1;
		wxMenuItem* 				m_closeWindowMenuItem;
	private:
		DECLARE_EVENT_TABLE()
                void SetUpdateAreaUnits(wxUpdateUIEvent& pCmdUI, SInt16 unitsCode);
                //void OnUpdateMagnification(wxUpdateUIEvent& event);
                //void OnUpdateZoomIn(wxUpdateUIEvent& event);
                //void OnUpdateZoomOut(wxUpdateUIEvent& event);
                //void OnUpdateZoomIndicator(wxUpdateUIEvent& event);

};

#endif
