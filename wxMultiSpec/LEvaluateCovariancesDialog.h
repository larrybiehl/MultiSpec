//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LEvaluateCovariancesDialog.h
//	Implementation:		LEvaluateCovariancesDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMEvalCovarianceDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 10/07/2019
//
//------------------------------------------------------------------------------------
//
#if !defined __LCOVEDLG_H__
	#define	__LCOVEDLG_H__             
	     
#include "LDialog.h"


class CMEvalCovarianceDialog : public CMDialog
	{
	public:
				 // standard constructor
		CMEvalCovarianceDialog (
				wxWindow* 		pParent = NULL,
				wxWindowID 		id = wxID_ANY,
				const 			wxString& title = wxT("Check Covariances"));

		~CMEvalCovarianceDialog();		// standard desctructor
		Boolean DoDialog(
		EvaluateCovarianceSpecsPtr  evaluateCovarianceSpecsPtr);

		bool TransferDataToWindow();
		bool TransferDataFromWindow();

		// Dialog Data    
		enum {IDD = IDD_EvalCovariances};
		bool	m_listOriginalMatrixFlag;
		bool	m_listInvertedMatrixFlag;
		bool	m_listInvertedInvertedMatrixFlag;
		bool	m_listOriginalXInvertedMatrixFlag;
		bool	m_featureTransformationFlag; 
		 
			// Implementation
	protected:
		void CreateControls();
		virtual void  OnInitDialog (wxInitDialogEvent& event);

		void OnTransformation (wxCommandEvent& event);
		void OnChannelComboSelendok (wxCommandEvent& event);
		void OnClassComboSelendok (wxCommandEvent& event);

		EvaluateCovarianceSpecsPtr  m_evaluateCovarianceSpecsPtr;
		Boolean		  	m_featureTransformAllowedFlag,
										  m_initializedFlag;
		UInt16			m_numberEigenvectors;

		DECLARE_EVENT_TABLE();

		/*Dialog items*/
		wxBoxSizer* bSizer107;
		wxBoxSizer* bSizer108;
		wxBoxSizer* bSizer175;
		wxCheckBox* m_checkBox14;
		wxCheckBox* m_checkBox15;
		wxCheckBox* m_checkBox16;
		wxCheckBox* m_checkBox17;
		wxCheckBox* m_checkBox18;

		wxStaticText* m_staticText190;
		wxComboBox*   m_comboBox26;
		wxComboBox*   m_comboBox27;
		wxStaticText* m_staticText191;
		wxButton* m_button20;
		wxButton* m_button21;
		 
	};
#endif // !defined __LCOVEDLG_H__
