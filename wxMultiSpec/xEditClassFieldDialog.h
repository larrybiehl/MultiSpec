//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xEditClassFieldDialog.h
//	Implementation:		xEditClassFieldDialog.cpp
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

#pragma once

#include "xDialog.h"


class CMEditClassFieldDlg : public CMDialog
{
		 // Construction
	public:
				// standard constructor
		CMEditClassFieldDlg (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = IDD_ClassifyDialog,
				const wxString& 					title = wxT("Set Project Options"));

		Boolean DoDialog (
				SInt16 								classFieldCode,
				SInt16 								currentClass,
				SInt16 								currentField,
				char* 								titleStringPtr);


				// Dialog Data

		enum {
			IDD = IDD_ClassField
			};
	
		wxString								m_className,
				 								m_fieldName;
	
		char 									m_classNamebuf[1024],
			 									m_fieldNamebuf[1024];

		 // Implementation
	protected:
				// Generated message map functions
	
		void CheckLength (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	
		char									*m_classNameCStringPtr,
												*m_fieldNameCStringPtr,
												*m_titleStringPtr;
	
		SInt16*								m_fieldTypePtr;
	
		wxBoxSizer							*bSizer285;
	
		wxButton								*m_button84,
												*m_button85;
	
		wxComboBox							*m_comboBox56;
	
		wxRadioButton						*m_radioBtn27,
												*m_radioBtn28;
	
		wxStaticText						*m_staticText265,
												*m_staticText266,
												*m_staticText267,
												*m_staticText268,
												*m_staticText269,
												*m_staticText270;
	
		wxTextCtrl							*m_textCtrl102,
												*m_textCtrl103;

		int 									m_fieldType;

		SInt16								m_classFieldCode,
												m_classStorage,
												m_currentField;
	
		Boolean								m_initializedFlag;
	
};	// end "class CMEditClassFieldDlg"
