//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2019)
//						(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LImageFrame.h
//	Implementation:		LImageFrame.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/08/2019
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file is the definition for the CMImageFrame class
//
//------------------------------------------------------------------------------------

#ifndef __LImageFrame__
#	define __LImageFrame__

#include "SMultiSpec.h"
#include "CDisplay.h"

#include "LCoordinateBar.h"
//#include "LImageSplitterWnd.h"
#include "LImageView.h"
#include "LLegendView.h"

#include "wx/docview.h"
#include "wx/wx.h"
#include "wx/laywin.h"
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
#include "wx/textctrl.h"
#include "wx/stattext.h"
#include "wx/button.h"
#include "wx/dialog.h"

class CMImageFrame : public wxDocChildFrame

{
    DECLARE_DYNAMIC_CLASS(CMImageFrame)

private:
	void 	DoZoomIn (
				wxMouseEvent&							event);
	
	void 	DoZoomOut (
				wxMouseEvent&							event);
	
	void 	DoZoomToOne (
				wxMouseEvent&							event);
	
	void OnActivate(wxActivateEvent& event);
	void OnCancelDraw (wxCommandEvent& event);
	
	void OnClose(wxCommandEvent& event);
	
	void OnEditSelectAll(wxCommandEvent& event);
	void OnEditClearSelectionRectangle(wxCommandEvent& event);
	
	void OnFileSave(wxCommandEvent& event);
	void OnFileSaveAs(wxCommandEvent& event);
	void OnFocus(wxFocusEvent& event);
	void OnRefresh(wxCommandEvent& event);
	void OnSize(wxSizeEvent& event);
        
	void OnMaximizeWindow(wxMaximizeEvent& event);
	void GetMinimumDisplaySizeForImage (Handle windowInfoHandle,LongPoint* minimumWindowSizePointPtr);
	void OnSashDrag(wxSashEvent& event);
	
	void OnUpdateFileCloseWindow(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFileSave(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFileSaveAs(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFilePrint(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFilePrintPreview(wxUpdateUIEvent& pCmdUI);
	void OnUpdateFilePrintSetup(wxUpdateUIEvent& pCmdUI);
	void OnUpdateEditCopy(wxUpdateUIEvent& pCmdUI);
	void OnUpdateEditSelectAll(wxUpdateUIEvent& event);
	void OnUpdateEditClearSelectionRectangle(wxUpdateUIEvent& event);
	void OnUpdateOverlay(wxUpdateUIEvent& event);
	void OnUpdateRefresh(wxUpdateUIEvent& event);
	void OnUpdateWindowNewSelectionGraph(wxUpdateUIEvent& pCmdUI);
	void OnUpdateWindowShowCoordinateView(wxUpdateUIEvent& pCmdUI);
	
	void OnWindowNewSelectionGraph(wxCommandEvent& event);
	void OnWindowShowCoordinateView(wxCommandEvent& event);
	
	void OnLeftUp (wxMouseEvent& event);
        
    
protected:
    wxMenuBar* m_menubar1;
    wxMenu* filemenu;
    wxMenu* editmenu;
    wxMenu* projectmenu;
    wxMenu* m_editClearOverlayMenu;
    wxMenu* viewmenu;
    wxMenu* processormenu;
    wxMenu* m_menu2;
    wxMenu* m_menu3;
    wxMenu* optionsmenu;
    wxMenu* m_menu4;
    wxMenu* m_menu5;
    wxMenu* windowmenu;
    wxMenu* helpmenu;
	
#if defined multispec_wxmac
	wxToolBar* 					m_toolBar;
#endif

#ifndef multispec_wxmac
    enum{
      ID_FILE_SAVE,
      ID_FILE_SAVE_AS
    };
#endif
	//void OnClose(void);
public:
	CMImageFrame();
	CMImageFrame(wxDocument* doc, wxView* view, wxDocParentFrame *parent);
    ~CMImageFrame();
    double m_zoom;
    CMCoordinateBar*		m_coordinatesBar;
    CMLegendView*		m_imageLegendViewCPtr;
    void ActivateImageWindowItems(
            Boolean activateFlag,
            Boolean changeWindowFlag);
    void UpdateSelectionWindowList();
    void ChangeClassGroupDisplay(
            SInt16 newClassGroupCode);
            //Boolean updateClassGroupListFlag);

 /*   LRESULT DoRealize(
            Boolean backgroundFlag,
            wxWindow* hWnd);
*/
    Boolean GetActiveWindowFlag(void);

    void GetCoordinateViewComboText(
            char* comboItemStringPtr,
            UInt16 itemNumber);
	
		int GetTitleAndToolBarHeight (void);

    UInt16 GetDisplayPixelSize(void); // inline

    CMImageDoc* GetDocument(void); // inline

    CMLegendView* GetLegendViewCPtr(void); // inline
	 
    void DoKeyDownDownArrow(wxCommandEvent& event);
    void DoKeyDownLeftArrow(wxCommandEvent& event);
    void DoKeyDownRightArrow(wxCommandEvent& event);
    void DoKeyDownUpArrow(wxCommandEvent& event);

    void InitialUpdate(
            CSize imageViewSize);

    void OnViewCoordinatesBar(wxCommandEvent& event);
    void OnUpdateViewCoordinatesBar(wxUpdateUIEvent& event);
	 
	 void		SetActiveWindowFlag(
				Boolean setting);

    void SetImageViewCPtr(
            CMImageView* imageViewCPtr);

    void SetLegendWidth(
            SInt16 newLegendWidth);

    void SetSplitterParameters(
            SInt16 windowType);
									
	void ShowCoordinateView(
				SInt16								inputCode);

    void UpdateActiveImageWindowInfo(void);

    void UpdateScaleInformation(
            double scale,
            char* scaleStringPtr);

    void UpdateSelectedAreaInformation(
            char* areaDescriptionStringPtr,
            char* areaValueStringPtr);

    void UpdateSelectionCoordinates(void);

    void UpdateSelectionCoordinates(
            char* lineValueStringPtr,
            char* columnValueStringPtr);

    void UpdateCursorCoordinates(void);

    void UpdateCursorCoordinates(
            char* lineValueStringPtr,
            char* columnValueStringPtr);
    void OnMagnification(void);
    void OnUpdateMagnification(wxUpdateUIEvent& event);
    void OnUpdateZoomIn(wxUpdateUIEvent& event);
    void OnUpdateZoomOut(wxUpdateUIEvent& event);
    void OnUpdateZoomIndicator(wxUpdateUIEvent& event);
    void OnZoom(wxCommandEvent& event);
    
    wxSashLayoutWindow* m_mainWindow;// Window for displaying image
    wxSashLayoutWindow* m_topWindow;// Window for displaying CMCoordinatebar
    wxSashLayoutWindow* m_leftWindow;// Window for displaying CMLegendView
    bool   								m_frameMaximized;
	 wxStaticText* 					m_zoomText;
    
    protected:
//	CSplitterWnd			m_wndSplitter;
//	CMImageSplitterWnd	m_wndSplitter;

//	CMLegendView*		m_imageLegendViewCPtr;

	CMImageView*		m_imageViewCPtr;

	UInt16			m_displayPixelSize;

	// Flag indicated whether this image frame is active.

	Boolean			m_imageFrameActiveFlag;

	Boolean			m_forcePaletteBackgroundUpdateFlag;

	static Boolean		s_forcePaletteBackgroundFlag;
	
        LongPoint m_TempLongPoint;
        wxSize m_frameSizeSaved; 
        wxPoint m_scrollPos;

    
	private:
		DECLARE_EVENT_TABLE()
};

inline UInt16 CMImageFrame::GetDisplayPixelSize()
	   { return m_displayPixelSize; }

inline CMImageDoc* CMImageFrame::GetDocument()
   { return m_imageViewCPtr->GetDocument(); }

inline CMLegendView* CMImageFrame::GetLegendViewCPtr()
   { return m_imageLegendViewCPtr; }
enum
{
	REFRESH=101,
	MDI_CHANGE_TITLE,
	MDI_CHANGE_POSITION,
	MDI_CHANGE_SIZE,
	ID_WINDOW_TOP,
	ID_WINDOW_RIGHT,
	ID_WINDOW_LEFT
};

#endif
