//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LFieldsToThematicDialog.h
//	Implementation:		LFieldsToThematicDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMFieldsToThematicDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 06/21/2017
//
//------------------------------------------------------------------------------------
//
#if !defined __LFTOTDLG_H__
	#define	__LFTOTDLG_H__    
	
	#include "LDialog.h"


	class CMFieldsToThematicDialog : public CMDialog
		{
				// Construction
		public:
					// standard constructor
			CMFieldsToThematicDialog (
					wxWindow* pParent = NULL,
					wxWindowID id = IDD_FeatureExtraction,
					const wxString& title = wxT("Project Areas to Thematic Image"));
			
			Boolean DoDialog (
					ReformatOptionsPtr				reformatOptionsPtr);

					// Dialog Data
			enum {IDD = IDD_AreasToThematicImage};
			bool	m_trainingAreasFlag;
			bool	m_testingAreasFlag;
			int	m_outputFormatCode;

				// Implementation
		protected:
			bool TransferDataFromWindow();
			bool TransferDataToWindow();
			void CreateControls();
				  
					// Generated message map functions

			void OnInitDialog (wxInitDialogEvent& event);    
			DECLARE_EVENT_TABLE()
			
			ReformatOptionsPtr		m_reformatOptionsPtr;
			
			Boolean						m_initializedFlag;
				  
			wxStaticText* m_staticText282;
			wxStaticText* m_staticText177;
			wxStaticText* m_staticText60;
			wxStaticText* m_staticText62;
			wxStaticText* m_staticText283;
			wxStaticText* m_staticText284;
			wxStaticText* m_staticText285;
			wxButton* m_button71;
			wxButton* m_button72;
			wxComboBox* m_comboBox55;
			wxComboBox* m_comboBox54;
			wxCheckBox* m_checkBox66;
			wxCheckBox* m_checkBox67;
			wxTextCtrl* m_linestartctrl;
			wxTextCtrl* m_lineendctrl;
			wxTextCtrl* m_lineintctrl;
			wxTextCtrl* m_colstartctrl;
			wxTextCtrl* m_colendctrl;
			wxTextCtrl* m_colintctrl;
			wxBitmapButton* m_bpButton4;
			wxBitmapButton* m_bpButton5;
					 
		};    
#endif	// !defined __LFTOTDLG_H__
