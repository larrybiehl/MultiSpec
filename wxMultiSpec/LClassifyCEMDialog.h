//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClassifyCEMDialog.h : CMCEMClassifyDialog Definition
//	Implementation:		LClassifyCEMDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMCEMClassifyDialog class
//
//	Written By:				Wei-Kang Hsu			Date: 04/09/2015
//	Revised By:				Larry L. Biehl			Date: 01/12/2018
//	
//------------------------------------------------------------------------------------

#if !defined __LCCEMDLG_H__
	#define	__LCCEMDLG_H__             

	#include "LDialog.h"

	class CMCEMClassifyDialog : public CMDialog 
		{
				// Construction
		public:
			 CMCEMClassifyDialog (
					wxWindow*								pParent = NULL, 
					wxWindowID								id = IDD_ClassifyDialog, 
					const wxString&						title = wxT("Specify CEM Parameters"));

			 Boolean DoDialog (
					CEMParametersPtr						cemParametersPtr,
					UInt16*									classifyProcedureEnteredCodePtr);

			int										m_correlationMatrixCode;
			bool										m_testFieldsFlag,
														m_trainingFieldsFlag;

				// Implementation
		protected:

			void CreateControls ();
			void OnInitDialog (wxInitDialogEvent& event);
			void OnSelendokClassCombo (wxCommandEvent& event);
			void OnUseClasses (wxCommandEvent& event);
			void OnUseSelectedArea (wxCommandEvent& event);
			bool TransferDataToWindow ();
			bool TransferDataFromWindow ();

			DECLARE_EVENT_TABLE()

			void BackgroundCorrelationSettings (Boolean areaCode);

			CEMParametersPtr						m_cemParametersPtr;

			SInt16									*m_correlationMatrixClassPtr, 
														*m_localClassAreaPtr;

			wxBitmapButton							*m_bpButton4,
														*m_bpButton5;
			
			wxBoxSizer								*bSizer156;
			
			wxButton									*m_button35,
														*m_button36;

			wxComboBox								*m_comboBox26;
			
			wxRadioButton							*m_radioBtn15,
														*m_radioBtn16;
			
			wxStaticText							*m_staticText60,
														*m_staticText62,
														*m_staticText175,
														*m_staticText177;

			wxTextCtrl								*m_linestartctrl,
														*m_lineendctrl,
														*m_lineintctrl,
														*m_colstartctrl,
														*m_colendctrl,    
														*m_colintctrl;

			UInt32									m_localNumberClassAreas;

			Boolean									m_initializedFlag;

	}; // end "CMCEMClassifyDialog dialog"
#endif // !defined __LCCEMDLG_H__
