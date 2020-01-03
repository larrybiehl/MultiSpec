//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						CMLOOMixingDialog.h
//	Implementation:		CMLOOMixingDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMLOOMixingDialog class
//
//	Written By:				Wei-Kang Hsu			Date: 01/26/2018
//	Revised By:				Larry L. Biehl			Date: 01/29/2018
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMLOOMixingDialog : public CMDialog
{
			// Construction
	public:
	
				 // standard constructor
		CMLOOMixingDialog (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = wxID_ANY,
				const wxString& 					title = wxT("Set LOO Mixing Parameter"));

		Boolean DoDialog (
				SInt16								statsWindowMode);
	
	
		enum {IDD = IDD_LOOMixingParameter};


			// Implementation
	protected:
		void OnIdentityMatrix (
				wxCommandEvent&					event);

		void OnInitDialog (
				wxInitDialogEvent&				event);
			
		void OnOptimumMixing (
				wxCommandEvent&					event);
			
		void OnUserMixing (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ()
	 
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
		
		wxButton								*m_button76,
												*m_button77;
		  
		wxRadioButton						*m_radioBtn36,
												*m_radioBtn37,
												*m_radioBtn38;

		wxStaticText						*m_staticText291,
												*m_staticText293;
		
		wxTextCtrl							*m_textCtrl131;
	
		double								m_loocMixingParameter,
												m_userMixingParameter;
	
		int									m_mixingParameterCode;

		SInt16								m_statsWindowMode;
			      
};	// end "class CMLOOMixingDialog"
