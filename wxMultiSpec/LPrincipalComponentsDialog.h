//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LPrincipalComponentsDialog.h
//	Implementation:		LPrincipalComponentsDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMClassifyDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 10/04/2018
//	
//------------------------------------------------------------------------------------

#if !defined __LPRINDLG_H__
#	define	__LPRINDLG_H__ 
            
#include "LDialog.h"
#include "wx/wx.h"
/////////////////////////////////////////////////////////////////////////////
// CMPrincipalCompDialog dialog

class CMPrincipalCompDialog : public CMDialog {
    DECLARE_DYNAMIC_CLASS(CMPrincipalCompDialog)
    // Construction
public:
    CMPrincipalCompDialog(wxWindow* pParent = NULL, const wxString& title = wxT("Set Principal Component Analysis Specifications:")); // standard constructor

    ~CMPrincipalCompDialog(); // standard desctructor

    SInt16 DoDialog(void);

    // Dialog Data
    //{{AFX_DATA(CMPrincipalCompDialog)

    enum {
        IDD = IDD_PrincipalComponent
    };
    int m_dataCode;
    bool m_trainingFlag;
    bool m_testFlag;
    bool m_listEigenvectorFlag;
    bool m_equalVariancesFlag;
    bool m_saveEigenvectorFlag;
    bool m_textWindowFlag;
    bool m_diskFileFlag;
    //}}AFX_DATA

    // Implementation
protected:
    //virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support

	void HideFieldTypeItems();

	void ShowFieldTypeItems();
	void CreateControls();
	bool TransferDataFromWindow();
	bool TransferDataToWindow();
	
	Boolean m_initializedFlag;

		// Generated message map functions
	//void OnButtonPress(wxKeyEvent& event);
	void OnInitDialog(wxInitDialogEvent& event);
	void OnClasses(wxCommandEvent& event);
	void OnImageArea(wxCommandEvent& event);
	void OnSelendokClassCombo(wxCommandEvent& event);
	void OnSelendokChannelCombo(wxCommandEvent& event);
//        void OnSelendokChannelComboDropDown(wxCommandEvent& event);
//        void OnSelendokClassComboDropDown(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
	
	wxRadioButton* m_radioBtn11;
//    wxChoice* m_choice8;
    wxComboBox* m_comboBox28;
    wxComboBox* m_comboBox29;
    wxCheckBox* m_checkBox29;
    wxCheckBox* m_checkBox30;
    wxRadioButton* m_radioBtn10;
    wxBitmapButton* m_bpButton4;
    wxBitmapButton* m_bpButton5;
    //wxStaticText* m_staticText63;
    //wxStaticText* m_staticText64;
    //wxStaticText* m_staticText65;
    //wxStaticText* m_staticText66;
    //wxStaticText* m_staticText60;
    //wxTextCtrl* m_linestartctrl;
    //wxTextCtrl* m_lineendctrl;
    //wxTextCtrl* m_lineintctrl;
    //wxStaticText* m_staticText62;
    //wxTextCtrl* m_colstartctrl;
    //wxTextCtrl* m_colendctrl;
    //wxTextCtrl* m_colintctrl;
    wxStaticText* m_staticText118;
    wxChoice* m_choice7;
    wxCheckBox* m_checkBox24;
    wxCheckBox* m_checkBox25;
    wxCheckBox* m_checkBox26;
    wxCheckBox* m_checkBox27;
    wxCheckBox* m_checkBox28;
    wxButton* m_button31;
    wxButton* m_button32;

    //wxString m_LineStartString;
    //wxString m_LineEndString;
    //wxString m_LineIntervalString;
    //wxString m_ColumnStartString;
    //wxString m_ColumnEndString;
    //wxString m_ColumnIntervalString;
};

#endif // !defined __LPRINDLG_H__
