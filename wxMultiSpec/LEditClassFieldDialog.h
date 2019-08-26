//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LEditClassFieldDialog.h
//	Implementation:		LEditClassFieldDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMEditClassFieldDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 07/07/2017
//
//------------------------------------------------------------------------------------
//
#if !defined __LEDITCLASSFIELDDLG_H__
	#define	__LEDITCLASSFIELDDLG_H__  

#include "LDialog.h"  
 
typedef wxString CString;


class CMEditClassFieldDlg : public CMDialog
	{
		 // Construction
	public:
				// standard constructor
		CMEditClassFieldDlg (
				wxWindow* 				pParent = NULL,
				wxWindowID 				id = IDD_ClassifyDialog,
				const wxString& 		title = wxT("Set Project Options"));

				// Dialog Data

		enum {
			IDD = IDD_ClassField
			};
		CString m_className;
		CString m_fieldName;
		char m_classNamebuf[1024];
		char m_fieldNamebuf[1024];

		int m_fieldType;

		Boolean DoDialog (
				SInt16 classFieldCode,
				SInt16 currentClass,
				SInt16 currentField,
				char* titleStringPtr);

		 // Implementation
	protected:
				// Generated message map functions
		void OnInitDialog (wxInitDialogEvent& event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
		void CheckLength (wxCommandEvent& event);
		bool TransferDataFromWindow ();
		bool TransferDataToWindow ();
	
		char				*m_classNameCStringPtr,
							*m_fieldNameCStringPtr,
							*m_titleStringPtr;
	
		SInt16*			m_fieldTypePtr;

		SInt16			m_classFieldCode,
							m_currentField,
							m_classStorage;
	
		Boolean			m_initializedFlag;
	
		wxBoxSizer* bSizer285;
		wxStaticText* m_staticText265;
		wxStaticText* m_staticText266;
		wxStaticText* m_staticText267;
		wxStaticText* m_staticText268;
		wxStaticText* m_staticText269;
		wxStaticText* m_staticText270;
		wxComboBox* m_comboBox56;
		wxTextCtrl* m_textCtrl102;
		wxTextCtrl* m_textCtrl103;
		wxRadioButton* m_radioBtn27;
		wxRadioButton* m_radioBtn28;
		wxButton* m_button84;
		wxButton* m_button85;
	
	};
#endif // !defined __LEDITCLASSFIELDDLG_H__ 
