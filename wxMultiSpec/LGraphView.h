//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2018)
//						(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LGraphView.h
//	Implementation:		LGraphView.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			10/19/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file is the definition for the CMGraphView and
//								CMGraphViewButton classes
//
//------------------------------------------------------------------------------------

#ifndef __LVIEWGRAPHH__
#define __LVIEWGRAPHH__

#include "SMultiSpec.h"

#include "wx/wx.h"
//#include "wx/docmdi.h"
#include "wx/docview.h"
#include "wx/window.h"
#include "wx/dc.h"
#include "LMainFrame.h"
#include "LGraphFrame.h"

#include "LMultiSpec.h" 
//#include "SExternalGlobals.h"
//#include "SProtype.h"

#include "wx/evtloop.h"

typedef bool BOOL;
typedef long unsigned int UINT;

class CMGraphViewButton : public wxButton
	{
	public:
    						CMGraphViewButton();
		virtual 			~CMGraphViewButton();

		void            SetButtonID(UInt16  buttonID); 
		void            SetGraphViewCPtr (CMGraphView*  graphViewCPtr);

		void            OnLines2(UINT  menuID);
		void            OnSelectVector(UINT menuID);
		void            OnBinWidth(UINT menuID);
		UInt16          m_buttonID; 
		CMGraphView*    m_graphViewCPtr;

	};



class CMGraphView : public wxView 
	{
	DECLARE_DYNAMIC_CLASS (CMGraphView)

	public:
		CMGraphView ();
    
		wxDocChildFrame *frame;
	
		~CMGraphView ();
		CMGraphFrame *m_frame;
            
		bool OnCreate (
						wxDocument *doc,
						long WXUNUSED(flags));
	
		void OnPaint (
						wxPaintEvent & event);
    
		virtual void OnDraw (
						wxDC* dc);
	 
		virtual void OnActivateView (
						bool bActivate,
						wxView* pActivateView,
						wxView* pDeactiveView);
		  
		void DrawGraphGrowIcon (GraphPtr graphRecordPtr);
		Boolean FinishGraphRecordSetUp (
						SInt16*								channelListPtr,
						SInt32								channelListLength,
						SInt32								vectorLength,
						SInt32								numberYVectors,
						SInt32								numberSets,
						UInt16								xValueType,
						UInt16								yValueType);
	
		virtual void OnInitialUpdate();
    
    void        SetCheckIOMemoryFlag (Boolean  flagSetting);
	 
	 void			GetGraphWindowClientRect(
						Rect*								clientRectPtr);
    
    void	IntializeGraphRecord (void);
    
    Handle      GetGraphRecordHandle();
    UInt16      GetGraphWindowNumber();
    Handle      GetSelectionIOHandle();
    wxComboBox* GetXAxisComboBoxPtr();
    WindowPtr   GetWindowPtr();
    void        SetWindowPtr(WindowPtr window);
    void        MoveGraphControls();
    void        OnUpdateFileSaveAs(CCmdUI*  pCmdUI);
    void        HideFeatureList();
    void        ShowFeatureList();
    void        UpdateShowOrHideFeatureList();
    CMGraphDoc* GetDocument();
    void        OnDestroy(wxCloseEvent& event);
    void        DoFilePrint();
    void        OnPrint(wxDC* pDC);
    
    Boolean     UpdateGraphControls();
    
    
    
    enum { IDD = IDD_GraphWindow };
    CMGraphViewButton       m_nextChannel;
    CMGraphViewButton       m_previousChannel;
    CMGraphViewButton       m_selectVectors;
    CMGraphViewButton       m_overlayControl;
    CMGraphViewButton       m_binWidth;
        
    Handle		m_graphRecordHandle;	
    static Handle       s_selectionIOInfoHandle;
    WindowPtr		m_window;
    static UInt16       s_graphicsWindowCount; 
    UInt16		m_graphicsWindowNumber;  
    static UInt16       s_numberOfGWindows;
    
    static wxBrush	s_grayBrush;         
    static wxPen 	s_bluePen;
    static wxPen 	s_grayPen; 			
    static wxPen 	s_greenPen;   
    static wxPen        s_ltGrayPen; 
    static wxPen 	s_redPen;   
    static wxPen 	s_whitePen;
    static wxFont 	s_font;
    //static RECT       s_updateRect;
    
    
    wxMenu*		m_graphOverlayMenu;
    wxMenu*             m_selectVectorMenu;
    wxMenu*		m_binWidthMenu;
   
    
    double		m_printerTextScaling;
					// Flag indicating whether the current draw mode is for printing
					// or copying.	
		
    Boolean		m_printCopyModeFlag; 
    SInt16		m_xPixelsPerInch;
    SInt16		m_yPixelsPerInch; 
    
    
    
    wxBitmapButton      *nextChannel;
    
    wxToolBar* m_toolBar1;
        
    void OnPreviousChannel(wxCommandEvent& event);
    void OnNextChannel(wxCommandEvent& event);
    void OnBinWidth(int menuID);
    void OnOverlay(int menuID);
    void OnSelectVector(int menuID);
    Boolean UpdateGraphChannels();
    void ResetListControls();
    void UpdateListData ();
    void UpdateDataListCtrl();
    void ReloadXAxis(int selection);
    bool LoadChannelNumbersAndValues(GraphPtr graphRecordPtr);
    bool LoadChannelDescriptionIntoList(FileInfoPtr fileInfoPtr, LayerInfoPtr layerInfoPtr, SInt16 channel);
    void UpdateFeatureListCtrl(wxPoint SelectionPoint);
    void UpdateFieldNames (ShapeInfoPtr shapeInfoPtr, int shapefileNumber);
    wxString GetShapeFileName(ShapeInfoPtr shapeInfoPtr);
    void UpdateFieldValues(ShapeInfoPtr shapeInfoPtr, wxPoint SelectionPoint, int shapefileNumber);
    ShapeInfoPtr GetShapeInfoFromHandle(Handle ShapeInfoHandle, Handle ActiveImageWindowHandle);
    //CMGraphView*        m_pDocument;
    
    DECLARE_EVENT_TABLE()
    		
};

    inline Handle CMGraphView::GetGraphRecordHandle()
   { return (Handle)m_graphRecordHandle; }  
      
    inline UInt16 CMGraphView::GetGraphWindowNumber()
   { return (UInt16)m_graphicsWindowNumber; }
      
    inline Handle CMGraphView::GetSelectionIOHandle()
   { return (Handle)s_selectionIOInfoHandle; } 
      
    inline WindowPtr CMGraphView::GetWindowPtr()
   { return (WindowPtr)m_window; }
      
    inline void CMGraphView::SetWindowPtr(WindowPtr window)
   { m_window = window; }
#endif
