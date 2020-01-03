//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xEvaluateTransformationDialog.h
//	Implementation:		xEvaluateTransformationDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMEvalTransformDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 06/21/2017
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "xDialog.h"


class CMEvalTransformDialog : public CMDialog
{
			// Construction
	public:
		CMEvalTransformDialog (  // standard constructor
		 		wxWindow* 							pParent = NULL,
				wxWindowID 							id = wxID_ANY,
		 		const wxString& 					title = wxT("Check Transformation Matrix"));
	
		~CMEvalTransformDialog ();		// standard desctructor
		 
		Boolean DoDialog (
		 		EvaluateTransformSpecsPtr 		evaluateTransformSpecsPtr);

		 
				// Dialog Data
		enum { IDD = IDD_EvalTransform };
	
			// Implementation
	protected:
		void OnCheckTransform (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ();

		void CreateControls ();

		void SetListCheckTransform (
				Boolean 								checkTransformFlag);

		bool TransferDataFromWindow ();

		bool TransferDataToWindow ();

		EvaluateTransformSpecsPtr   	m_evaluateTransformSpecsPtr;

				// Dialog items
	
		wxBoxSizer							*bSizer107,
												*bSizer108,
												*bSizer175;
	
		wxButton								*m_button20,
												*m_button21;
	
		wxCheckBox							*m_checkBox14,
												*m_checkBox15,
												*m_checkBox16,
												*m_checkBox23,
												*m_checkBox32;
	
		wxStaticText						*m_staticText196;
	
		Boolean								m_checkTransformationFlag,
												m_listCheckMatrixFlag,
												m_listEigenvaluesFlag,
												m_listOriginalMatrixFlag;
	
		Boolean								m_initializedFlag;
	
};	// end "class CMEvalTransformDialog"
