//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClassWeightsDialog.h
//	Implementation:		LClassWeightsDialog.cpp
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
//	Revised By:				Larry L. Biehl			Date: 04/30/2018
//	
//------------------------------------------------------------------------------------

#if !defined(AFX_LCWTDLG_H__)
	#define AFX_LCWTDLG_H__

	#include "LDialog.h"

	class CMClassWeightsDlg : public CMDialog
		{
				// Construction
		public:
			CMClassWeightsDlg (
					wxWindow*						pParent = NULL,
					wxWindowID						id = IDD_ClassWeights,
					const wxString&				title = wxT("Specify Class Weights"));
		
			~CMClassWeightsDlg ();
		
					// Dialog Data

			enum {
				IDD = IDD_ClassWeights
				};
		
			wxString							m_weightString;
			double							m_saveNewWeight;
			int								m_weightUnits;

			 SInt16 DoDialog (
					UInt16							numberOutputClassesToUse,
					SInt16*							classListPtr,
					float*							weightsListPtr,
					SInt16							weightsSelection,
					Boolean							useEnhancedStatFlag);

				// Implementation
		protected:
			void CreateControls ();
			void OnChangeWeight (wxCommandEvent& event);
			void OnEnterNewWeightButton (wxCommandEvent& event);
			void OnEqualWeightButton (wxCommandEvent& event);
			void OnHelpButton (wxCommandEvent& event);
			void OnInitDialog (wxInitDialogEvent& event);
			void OnOK (wxCommandEvent& event);
			void OnSelchangeList (wxCommandEvent& event);
			bool TransferDataFromWindow ();
			bool TransferDataToWindow ();
			DECLARE_EVENT_TABLE ()

			wxFont				m_weightsListFont;
		
			float*				m_weightsListPtr;
			SInt16*				m_classListPtr;
		
			double				m_weightSum;

			SInt16				m_selectedCell,
									m_weightsSelection;
			UInt16				m_numberOutputClassesToUse;

			Boolean				m_initializedFlag,
									m_useEnhancedStatFlag;

			wxBoxSizer			*bSizer136;
			
			wxButton				*m_button30,
									*m_button31,
									*m_button32,
									*m_button33,
									*m_button34;
		
			wxListBox			*m_listBox1;
		
			wxRadioButton		*m_radioBtn11,
									*m_radioBtn12;
		
			wxStaticText		*m_staticText188,
									*m_staticText189,
									*m_staticText190,
									*m_staticText191,
									*m_staticText192;
		
			wxTextCtrl 			*m_textCtrl89;
		};

#endif // !defined(AFX_LCWTDLG_H__)
