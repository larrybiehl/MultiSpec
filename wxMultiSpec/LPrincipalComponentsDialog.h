//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
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
//	Brief description:	Header file for the CMPrincipalCompDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 10/04/2018
//	
//------------------------------------------------------------------------------------
//
#if !defined __LPRINDLG_H__
	#define	__LPRINDLG_H__
            
#include "LDialog.h"
#include "wx/wx.h"


class CMPrincipalCompDialog : public CMDialog
	{
	DECLARE_DYNAMIC_CLASS (CMPrincipalCompDialog)
			// Construction
	public:
				// standard constructor
		CMPrincipalCompDialog (
		 		wxWindow* pParent = NULL,
		 		const wxString& title = wxT("Set Principal Component Analysis Specifications:"));

		~CMPrincipalCompDialog(); // standard desctructor

		SInt16 DoDialog(void);

		enum { IDD = IDD_PrincipalComponent };
		int m_dataCode;
		bool m_trainingFlag;
		bool m_testFlag;
		bool m_listEigenvectorFlag;
		bool m_equalVariancesFlag;
		bool m_saveEigenvectorFlag;
		bool m_textWindowFlag;
		bool m_diskFileFlag;

			// Implementation
	protected:
		void HideFieldTypeItems();

		void ShowFieldTypeItems();
		void CreateControls();
		bool TransferDataFromWindow();
		bool TransferDataToWindow();
	
		Boolean m_initializedFlag;

				// Generated message map functions
	
		void OnInitDialog(wxInitDialogEvent& event);
		void OnClasses(wxCommandEvent& event);
		void OnImageArea(wxCommandEvent& event);
		void OnSelendokClassCombo(wxCommandEvent& event);
		void OnSelendokChannelCombo(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
	
		wxRadioButton* m_radioBtn11;
		wxComboBox* m_comboBox28;
		wxComboBox* m_comboBox29;
		wxCheckBox* m_checkBox29;
		wxCheckBox* m_checkBox30;
		wxRadioButton* m_radioBtn10;
		wxBitmapButton* m_bpButton4;
		wxBitmapButton* m_bpButton5;

		wxStaticText* m_staticText118;
		wxChoice* m_choice7;
		wxCheckBox* m_checkBox24;
		wxCheckBox* m_checkBox25;
		wxCheckBox* m_checkBox26;
		wxCheckBox* m_checkBox27;
		wxCheckBox* m_checkBox28;
		wxButton* m_button31;
		wxButton* m_button32;
	};
#endif	// !defined __LPRINDLG_H__
