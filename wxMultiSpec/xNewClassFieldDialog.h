//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xNewClassFieldDialog.h
//	Implementation:		xNewClassFieldDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMNewClassFieldDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 12/31/2018
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMNewClassFieldDlg : public CMDialog
{
			// Construction
	public:
				// standard constructor
		CMNewClassFieldDlg (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = IDD_ClassField,
				const wxString& 					title =
													wxT("Define Class and/or Field Description"));

		Boolean DoDialog (
				Boolean 								newClassOnlyFlag,
				char* 								classNamePtr,
				char* 								fieldNamePtr,
				SInt16* 								fieldTypePtr);


				// Dialog Data
	
		enum { IDD = IDD_ClassField };
		
		wxString 							m_className,
												m_fieldName;
	
		int 									m_classList,
												m_fieldType;

			// Implementation
	protected:
		void CheckLength (
				wxCommandEvent& 					event);
	
		void OnFieldType (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnSelendokClassList (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
		
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();


		char 									m_classNamebuf[1024],
			 									m_fieldNamebuf[1024];
	
		char									*m_classNameCStringPtr,
												*m_classNamePtr,
												*m_fieldNameCStringPtr,
												*m_fieldNamePtr;
	
		SInt16* 								m_fieldTypePtr;

		wxBoxSizer							*bSizer285;
		
		wxRadioButton						*m_radioBtn27,
												*m_radioBtn28;
		
		wxChoice								*m_classListCtrl;
	
		wxStaticText						*m_staticText265,
												*m_staticText266,
												*m_staticText267,
												*m_staticText268,
												*m_staticText269,
												*m_staticText270;
	
		wxTextCtrl							*m_textCtrl102,
												*m_textCtrl103;

		SInt64 								m_numberSelectionPixels;

		Boolean 								m_initializedFlag,
				 								m_newClassOnlyFlag;
					
};	// end "class CMNewClassFieldDlg"
