//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						CMLOOMixingDialog.h : CMLOOMixingDialog Definition
//	Implementation:		CMLOOMixingDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMClassesDlg class
//
//	Written By:				Wei-Kang Hsu			Date: 01/26/2018
//	Revised By:				Larry L. Biehl			Date: 01/29/2018
//	
//------------------------------------------------------------------------------------

#if !defined(AFX_WLOOMDLG_H__337E2183_F42A_11D3_8D9B_00105AA88EE3__INCLUDED_)
#define AFX_WLOOMDLG_H__337E2183_F42A_11D3_8D9B_00105AA88EE3__INCLUDED_

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "LDialog.h" 

class CMLOOMixingDialog : public CMDialog
	{
			// Construction
	public:
	
				 // standard constructor
		CMLOOMixingDialog (
						wxWindow* pParent = NULL, 
						wxWindowID id = wxID_ANY, 
						const wxString& title = wxT("Set LOO Mixing Parameter"));
						
				// Dialog Data
		enum {IDD = IDD_LOOMixingParameter};
		int		m_mixingParameterCode;
		double	m_userMixingParameter;

		Boolean DoDialog (
			SInt16				statsWindowMode);


				// Overrides
				// ClassWizard generated virtual function overrides
	protected:
		bool TransferDataFromWindow ();
		bool TransferDataToWindow ();
	
			// Implementation
	protected:
		void OnInitDialog (
			wxInitDialogEvent&	event);
		
		DECLARE_EVENT_TABLE ()
	 
		void CreateControls ();
	
			// Generated message map functions
			
		void OnUserMixing (
			wxCommandEvent&		event);
			
		void OnOptimumMixing (
			wxCommandEvent&		event);
			
		void OnIdentityMatrix (
			wxCommandEvent&		event);

		double						m_loocMixingParameter;

		SInt16						m_statsWindowMode;
		
		wxButton*					m_button76;
		wxButton*					m_button77; 
		  
		wxRadioButton*				m_radioBtn36;
		wxRadioButton*				m_radioBtn37;
		wxRadioButton*				m_radioBtn38;

		wxStaticText*				m_staticText291;
		wxStaticText*				m_staticText293;
		
		wxTextCtrl*					m_textCtrl131;
			      
	};

#endif // !defined(AFX_WLOOMDLG_H__337E2183_F42A_11D3_8D9B_00105AA88EE3__INCLUDED_)
