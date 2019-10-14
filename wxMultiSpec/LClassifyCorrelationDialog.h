//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClassifyCorrelationDialog.h
//	Implementation:		LClassifyCorrelationDialog.cpp
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
//	Revised By:				Larry L. Biehl			Date: 10/08/2019
//	
//------------------------------------------------------------------------------------

#if !defined __LCCORDLG_H__
	#define	__LCCORDLG_H__             

	#include "LDialog.h"

			// CMCorrelationClassifyDialog dialog

	class CMCorrelationClassifyDialog : public CMDialog 
		{
		public:
			 CMCorrelationClassifyDialog (
					wxWindow*								parent = NULL, 
					wxWindowID								id = IDD_SAMParameters, 
					const wxString&						title = wxT("Specify Correlation Classifier Parameters"));

			 Boolean DoDialog (
					SInt16*									covarianceEstimatePtr);

			 int m_covarianceEstimate;

				// Implementation
		protected:
			 void CreateControls ();
			 void OnCovarianceComboSelendok (wxCommandEvent& event);
			 void OnInitDialog (wxInitDialogEvent&	event);
			 bool TransferDataFromWindow ();
			 bool TransferDataToWindow ();

			 DECLARE_EVENT_TABLE()

			 bool										m_initializedFlag;
			 
					// Linux GUI controls
					
			 wxBoxSizer*							bSizer166;
			 wxStaticText*							m_staticText183;
			 wxComboBox*							m_comboBox27;
			 wxButton*								m_button37;
			 wxButton*								m_button38;
			 
		};	// end "CMCorrelationClassifyDialog dialog" 

#endif	// !defined __LCLSFDLG_H__
