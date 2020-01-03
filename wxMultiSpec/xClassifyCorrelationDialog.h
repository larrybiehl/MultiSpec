//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xClassifyCorrelationDialog.h
//	Implementation:		xClassifyCorrelationDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMCorrelationClassifyDialog class
//
//	Written By:				Wei-Kang Hsu			Date: 04/09/2015
//	Revised By:				Larry L. Biehl			Date: 11/14/2019
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMCorrelationClassifyDialog : public CMDialog 
{
	public:
		CMCorrelationClassifyDialog (
				wxWindow*							parent = NULL,
				wxWindowID							id = IDD_SAMParameters,
				const wxString&					title =
												wxT("Specify Correlation Classifier Parameters"));

		Boolean DoDialog (
				SInt16*								covarianceEstimatePtr);

			// Implementation
	protected:
		void OnCovarianceComboSelendok (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent&				event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
			
		wxBoxSizer*							bSizer166;
	
		wxButton*							m_button37,
												m_button38;
	
		wxChoice*							m_covarianceCtrl;
	
		wxStaticText*						m_staticText183;

		int 									m_covarianceEstimate;

		bool									m_initializedFlag;
		 
};	// end "class CMCorrelationClassifyDialog"
