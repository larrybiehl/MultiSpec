//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xPrincipalComponentsDialog.h
//	Implementation:		xPrincipalComponentsDialog.cpp
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
//	Revised By:				Larry L. Biehl			Date: 11/11/2019
//	
//------------------------------------------------------------------------------------

#pragma once
            
#include "xDialog.h"
#include "wx/wx.h"


class CMPrincipalCompDialog : public CMDialog
{
	DECLARE_DYNAMIC_CLASS (CMPrincipalCompDialog)
			// Construction
	public:
				// standard constructor
		CMPrincipalCompDialog (
		 		wxWindow* 							pParent = NULL,
		 		const wxString& 					title =
									wxT("Set Principal Component Analysis Specifications:"));

		~CMPrincipalCompDialog (); // standard desctructor

		SInt16 DoDialog (void);


		enum { IDD = IDD_PrincipalComponent };

			// Implementation
	protected:
				// Generated message map functions
	
		void OnChannelComboSelendok (
				wxCommandEvent& 					event);
	
		void OnClassComboSelendok (
				wxCommandEvent& 					event);
	
		void OnClasses (
				wxCommandEvent& 					event);
	
		void OnImageArea (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		void HideFieldTypeItems ();

		void ShowFieldTypeItems ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	

		wxCheckBox							*m_checkBox24,
												*m_checkBox25,
												*m_checkBox26,
												*m_checkBox27,
												*m_checkBox28,
												*m_checkBox29,
												*m_checkBox30;
	
		wxRadioButton						*m_radioBtn10,
												*m_radioBtn11;

		wxStaticText						*m_staticText118;
	
		int 									m_dataCode;
	
		bool 									m_diskFileFlag,
												m_equalVariancesFlag,
												m_listEigenvectorFlag,
												m_saveEigenvectorFlag,
												m_testFlag,
												m_textWindowFlag,
												m_trainingFlag;
	
		Boolean 								m_initializedFlag;
	
};	// end "class CMPrincipalCompDialog"
