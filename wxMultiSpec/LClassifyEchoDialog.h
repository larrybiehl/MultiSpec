//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClassifyEchoDialog.h
//	Implementation:		LClassifyEchoDialog.cpp
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
//	Revised By:				Larry L. Biehl			Date: 03/13/2018
//	
//------------------------------------------------------------------------------------

#if !defined __LECHODLG_H__
	#define	__LECHODLG_H__

	#include "LDialog.h"

	typedef bool BOOL;

	class CMEchoClassifyDialog : public CMDialog 
		{
				// Construction
		public:
					 // standard constructor 
			CMEchoClassifyDialog (
					wxWindow*						pParent = NULL, 
					wxWindowID						id = IDD_EchoClassify, 
					const wxString&				title = wxT("Specify Echo Parameters"));   

			~CMEchoClassifyDialog (); // standard desctructor 

			Boolean DoDialog (
					EchoClassifierVarPtr			echoClassifierVarPtr);

					// Dialog Data

			enum {
				IDD = IDD_EchoClassify
				};
				
			double				m_annexationThreshold,
									m_homogeneityThreshold;
			
			int					m_echoAlgorithmProcedure,
									m_homogeneousThresholdType;
				
			long					m_cellWidth;
			
			BOOL					m_combineLikeFieldsFlag,
									m_createHomogeneousFilesFlag,
									m_mixCellsFlag;

				// Implementation
		protected:
			void CreateControls ();
			bool TransferDataFromWindow ();
			bool TransferDataToWindow ();
			
					// Generated message map functions
			//void OnButtonPress (wxKeyEvent& event);
			void OnInitDialog (wxInitDialogEvent& event);
			void OnLogLike (wxCommandEvent& event);
			void OnPercent (wxCommandEvent& event);
			DECLARE_EVENT_TABLE ()

			double				m_maxPhase1,
									m_minPhase1,
									m_savedPhase1LogLikelihoodThreshold,
									m_savedPhase1PercentThreshold;

			EchoClassifierVarPtr m_echoClassifierVarPtr;

			Boolean				m_initializedFlag;

			wxBoxSizer*			bSizer148; 
			   
			wxButton				*m_button33,
									*m_button34;
			
			wxCheckBox			*m_checkBox25,
									*m_checkBox26,
									*m_checkBox27;
			
			wxComboBox*			m_comboBox25;
			
			wxRadioButton		*m_radioBtn13,
									*m_radioBtn14;
			
			wxStaticText		*m_staticText171,
									*m_staticText172,
									*m_staticText173,
									*m_staticText174;
			
			wxString				m_cellWidthString,
									m_homoThresString,
									m_annexThresString;
			
			wxTextCtrl			*m_textCtrl72,
									*m_textCtrl73,
									*m_textCtrl74;
			
		};

#endif // !defined __LECHODLG_H__

