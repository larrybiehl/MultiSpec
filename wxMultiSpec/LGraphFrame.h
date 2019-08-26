//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//									Copyright (1988-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LGraphFrame.h
//	Implementation:		LGraphFrame.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file is the definition for the CMGraphCanvas and
//								CMGraphFrame classes
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 02/15/2019
//
//------------------------------------------------------------------------------------
//
#ifndef __LGRAFFRM_H__
#define __LGRAFFRM_H__

#include "LMultiSpec.h"
#include "wx/docview.h"
#include "wx/splitter.h"
#include "wx/treelist.h"

typedef bool BOOL;


class CMGraphCanvas : public wxPanel
	{
	DECLARE_DYNAMIC_CLASS (CMGraphCanvas);
	
	public:
		 CMGraphCanvas (wxWindow* parent);
	
		 void paintEvent(wxPaintEvent & evt);
		 void paintNow ();
	
		 void render (wxDC& dc);
		 CMGraphView*    m_graphViewCPtr;
		 DECLARE_EVENT_TABLE ()
	
	};



class CMGraphFrame : public wxDocChildFrame 
	{
    DECLARE_DYNAMIC_CLASS(CMGraphFrame);

	private:
		 DECLARE_EVENT_TABLE()
		 wxStatusBar* m_statusBar1;
	 #ifndef multispec_wxmac
		 enum{
			ID_FILE_SAVE,
			ID_FILE_SAVE_AS
		 };
	#endif
		bool 						m_frameMaximized;
		wxSize 					m_frameSizeSaved;
		wxPoint 					m_framePosSaved;
		wxBitmapButton* 		m_buttonNext;
		wxBitmapButton* 		m_buttonPrevious;
		wxBitmapButton* 		m_buttonOverlay;
		wxBitmapButton* 		m_buttonBinWidth;
		wxBitmapButton* 		m_buttonVectors;

		void CreateBinWidthMenu ();
		void CreateHistogramDensityMenu ();
		void DoNextChannel (wxMouseEvent& event);
		void DoNextOrPreviousChannelEvent (
						wxMouseEvent& 						event,
						SInt16								controlIdentifier);
		void DoPreviousChannel (wxMouseEvent& event);
		void DoSelectVectors (wxMouseEvent& event);
		void DoShowBinWidth (wxMouseEvent& event);
		void DoShowOverlay (wxMouseEvent& event);
		void DoShowVectors (wxMouseEvent& event);
		Boolean UpdateGraphChannels ();
	
	public:
		CMGraphFrame(); // protected constructor used by dynamic creation
	
		CMGraphFrame(wxDocument* doc, wxView* view, wxDocParentFrame* parent, wxWindowID id,
							  const wxString& title, const wxPoint& pos = wxDefaultPosition,
							  const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE);

		~CMGraphFrame();
		BOOL OnCreateClient();
		void CreateHistogramControls ();
		//void CreateButton2();
	
		void OnNextListData(wxCommandEvent& event);
				void PrintKey(wxKeyEvent& event);
	
		void SetUpGraphXLabelPopUpMenu (
					Handle								windowInfoHandle);
	
		CMGraphView*    m_graphViewCPtr;
		wxButton* m_button69;
	
		wxSplitterWindow* m_splitter1;
		wxSplitterWindow* m_splitter2;
		wxSplitterWindow* m_splitter3;
		wxPanel* m_panel1;
		CMGraphCanvas* m_panel2;
		wxPanel* m_panel3;
		wxPanel* m_panel4;
		//wxListView* m_listCtrl1;
		wxTreeListCtrl* m_listCtrl1;
		wxListView* m_listCtrl2;
		wxCheckBox* m_checkBoxGraph;
		wxCheckBox* m_checkBoxFeature;
		wxCheckBox* m_checkBoxData;
		wxComboBox* m_comboXlabel;
		wxBitmapButton* m_ButtonNext;
	
		bool	m_dataListShowFlag;
		bool	m_featureListShowFlag;

		bool m_initGraphUpdate;
		void CreateControls ();
		void CreateControlsBiPlot ();
		void CreateControlsListData ();
	
		wxComboBox* CreateXAxisComboControl (
					wxWindow*							parent);
	
	//protected:
	//    wxComboBox* m_comboXlabel;
	
	public:
		wxMenu*					m_binWidthMenu;
		wxMenu*					m_graphOverlayMenu;
		wxMenu*             	m_selectVectorMenu;
	
		int						activateFlag,
									actEntryPrev,
									actEntryNext,
									binWidthCheckID,
									m_height,
									m_width,
									m_minFrameYSize,
									m_overlayCheckID,
									selectVectorCheckID;
	
		SInt32 					selectionVector;
	
		wxToolBar*				m_toolBar1;

		bool                	m_optionBinWidthFlag;


		void CreateSelectVector (int vectorSize);
		void OnUpdateFilePrint(wxUpdateUIEvent& pCmdUI);
		void OnFilePrint();
		void OnUpdateFilePrintPreview(wxUpdateUIEvent& pCmdUI);
		void OnUpdateFileGraphClose(wxUpdateUIEvent& pCmdUI);
		void OnUpdateFilePrintSetup(wxUpdateUIEvent& pCmdUI);
		void OnUpdateWindowNewSelectionGraph(wxUpdateUIEvent& pCmdUI);
		void OnUpdateWindowShowCoordinateView(wxUpdateUIEvent& pCmdUI);
		void OnUpdateMagnification(wxUpdateUIEvent& pCmdUI);
		void OnUpdateZoomIn(wxUpdateUIEvent& pCmdUI);
		void OnUpdateZoomOut(wxUpdateUIEvent& pCmdUI);
		void OnUpdateOverlay(wxUpdateUIEvent& event);

		void OnPaint(wxPaintEvent & event);
		void OnSize(wxSizeEvent& event);
		void OnMaximizeWindow(wxMaximizeEvent& event);
		void OnShowBinWidth (wxCommandEvent& event);
		void OnShowOverlay (wxCommandEvent& event);
		void OnShowSelectVector (wxCommandEvent& event);
		void OnBinWidth (wxCommandEvent& event);
		void OnGraphWindow(wxCommandEvent & WXUNUSED(event));
		void OnFeature(wxCommandEvent & WXUNUSED(event));
		void OnData(wxCommandEvent & WXUNUSED(event));
		void OnChangeXAxis(wxCommandEvent & WXUNUSED(event));
		void OnOverlay (wxCommandEvent& event);
		void OnSelectVector (wxCommandEvent& event);
		void OnUpdateFileSaveAs (wxUpdateUIEvent& event);
		void Render(wxDC& dc);
		void SetOverlayMenuCheck (int selectedMenuID);
		void SetGraphViewCPtr( CMGraphView*  graphViewCPtr);

		void UpdateSplitterWindowLayout();
			  void UpdateExpandFlag();
		void UpdateWavelengthComboUnit();
	};

#endif
