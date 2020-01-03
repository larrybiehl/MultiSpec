//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xEvaluateCovariancesDialog.h
//	Implementation:		xEvaluateCovariancesDialog.cpp
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
//	Revised By:				Larry L. Biehl			Date: 11/11/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "xDialog.h"


class CMEvalCovarianceDialog : public CMDialog
{
	public:
				 // standard constructor
		CMEvalCovarianceDialog (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = wxID_ANY,
				const 								wxString& title = wxT("Check Covariances"));

		~CMEvalCovarianceDialog ();		// standard desctructor
	
		Boolean DoDialog (
				EvaluateCovarianceSpecsPtr  	evaluateCovarianceSpecsPtr);

				// Dialog Data
	
		enum {IDD = IDD_EvalCovariances};
		 
			// Implementation
	protected:
		void OnChannelComboSelendok (
				wxCommandEvent&					event);

		void OnClassComboSelendok (
				wxCommandEvent&					event);

		virtual void OnInitDialog (
				wxInitDialogEvent& 				event);

		void OnTransformation (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ();
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();


		EvaluateCovarianceSpecsPtr  	m_evaluateCovarianceSpecsPtr;

				// Dialog items
	
		wxBoxSizer							*bSizer107,
												*bSizer108,
												*bSizer175;
	
		wxButton								*m_button20,
												*m_button21;
	
		wxCheckBox							*m_checkBox14,
												*m_checkBox15,
												*m_checkBox16,
												*m_checkBox17,
												*m_checkBox18;

		wxStaticText						*m_staticText190,
												*m_staticText191;
	
		UInt16								m_numberEigenvectors;
	
		bool									m_featureTransformationFlag,
												m_listInvertedInvertedMatrixFlag,
												m_listInvertedMatrixFlag,
												m_listOriginalMatrixFlag,
												m_listOriginalXInvertedMatrixFlag;
	
		Boolean		  						m_featureTransformAllowedFlag,
										  		m_initializedFlag;
		 
};	// end "class CMEvalCovarianceDialog"
