//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2018)
//						(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LStatisticsFrame.h
//	Implementation:		LStatisticsFrame.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/03/2019
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file is the definition for the CMStatisticsFrame class
//
//------------------------------------------------------------------------------------

#if !defined __LSTATFRM_H__
#define __LSTATFRM_H__
#include "wx/docview.h"

#include "LMultiSpec.h"
#include "wx/tooltip.h"

typedef bool BOOL;
typedef long unsigned int UINT;

//class CMStatisticsFrame : public CFrameWnd

class CMStatisticsFrame : public wxDocChildFrame

{

    DECLARE_DYNAMIC_CLASS(CMStatisticsFrame)
    //protected:
    //	CMStatisticsFrame();			// protected constructor used by dynamic creation
#ifndef multispec_wxmac
   enum {
        ID_FILE_SAVE,
        ID_FILE_SAVE_AS
    };
#endif
public:
    CMStatisticsFrame();
    CMStatisticsFrame(wxDocument* doc, wxView* view, wxDocParentFrame* parent, wxWindowID id,
            const wxString& title = wxT("Project"), const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE);
    //	virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);

    ~CMStatisticsFrame();
	 
	void ActivateStatisticsWindowItems (SInt16 statsWindowMode);
	void UpdateStatsTypeCombo(SInt16 statsWindowMode);
	void UpdateHistogramStatsCombo(void);
	void UpdateListStatsCombo(void);
	
protected:

			// Generated message map functions

    //virtual void OnCharHook ();
	virtual void OnCharHook (wxKeyEvent& event);
	
	void OnUpdateEditCut(wxUpdateUIEvent& pCmdUI);
	void OnUpdateEditPaste(wxUpdateUIEvent& pCmdUI);
	void OnEditCut(wxCommandEvent& event);
	void OnEditPaste(wxCommandEvent& event);
	void OnUpdateEditUndo(wxUpdateUIEvent& pCmdUI);
	void OnEditUndo(wxCommandEvent& event);
	void OnUpdateEditClearSelectRectangle(wxUpdateUIEvent& pCmdUI);
	void OnEditClearSelectRectangle(wxCommandEvent& event);
	void OnUpdateEditSelectionRectangle(wxUpdateUIEvent& pCmdUI);
	void OnEditSelectionRectangle(wxCommandEvent& event);
	void OnUpdateEditSelectAll(wxUpdateUIEvent& pCmdUI);
	void OnEditSelectAll(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnUpdateFileProjectClose(wxUpdateUIEvent& pCmdUI);
	void OnFileProjectClose(wxCommandEvent& event);

	void OnAddToList(wxCommandEvent& event);
	void OnSelendokClassList(wxCommandEvent& event);
	void OnProject(wxCommandEvent& event);
	void OnClass(wxCommandEvent& event);
	void OnField(wxCommandEvent& event);
	void OnUpdate(wxCommandEvent& event);
	void OnPolygon(wxCommandEvent& event);
	void OnSelect(wxCommandEvent& event);
	void OnList();
	void OnHistogram(wxCommandEvent& event);
	void OnSelchangeListBox(wxCommandEvent& event);
	void OnDblclkListBox(wxCommandEvent& event);
	void OnEditName(wxCommandEvent& event);
	void OnSelendokHistogramStatsCombo(wxCommandEvent& event);
	void OnHistogramStatsComboDropDown(wxCommandEvent& event);
	void OnHistogramStatsComboCloseUp(wxCommandEvent& event);
	void OnSelendokListStatsCombo(wxCommandEvent& event);
	void OnListStatsComboDropDown(wxCommandEvent& event);
	void OnListStatsComboCloseUp(wxCommandEvent& event);
	void OnSelendokStatsCombo(wxCommandEvent& event);
	void OnDropdownStatsTypeCombo(wxCommandEvent& event);

	void CreateControls2();

	int m_classList;
	int m_histogramStatsCode;
	int m_listStatsCode;
	int m_statsTypeCode;
	//new
	BOOL m_polygonFlag;
	Boolean m_initializedFlag,
	m_optionKeyFlag;

	wxStaticText* m_staticText223;
	wxStaticText* m_staticText224;
	wxStaticText* m_staticText225;
	wxStaticText* m_staticText226;
	wxStaticText* m_staticText227;
	wxStaticText* m_staticText228;
	wxStaticText* m_staticText229;
	wxStaticText* m_staticText230;
	wxStaticText* m_staticText231;
	wxStaticText* m_staticText259;
	wxStaticText* m_staticText260;
	wxStaticText* m_staticText261;
	wxStaticText* m_staticText262;
	wxStaticText* m_staticText263;
	wxStaticText* m_staticText264;
	wxComboBox* m_comboBox40;
	wxComboBox* m_comboBox41;
	wxComboBox* m_comboBox42;
	wxComboBox* m_comboBox43;
	wxCheckBox* m_checkBox53;
	wxButton* m_button56;
	wxButton* m_button60;
	wxButton* m_button61;
	wxButton* m_button62;
	wxButton* m_button63;
	wxButton* m_button64;
	wxButton* m_button65;
	wxListBox* m_listBox4;
	wxListBox* m_listBox;
        wxToolTip* m_tooltipEdit;
        wxToolTip* m_tooltipUpdate;
        wxToolTip* m_tooltipStats;
        wxToolTip* m_tooltipAdd;
	DECLARE_EVENT_TABLE()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// !defined __LSTATFRM_H__
