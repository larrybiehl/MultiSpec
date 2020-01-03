//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xGaussianParameterDialog.h
//	Implementation:		xGaussianParameterDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMGaussianParameterDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 08/23/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMGaussianParameterDlg : public CMDialog
{
		 // Construction
	public:
				// standard constructor
		CMGaussianParameterDlg (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = wxID_ANY,
				const wxString& 					title =
													wxT("Set Gaussian Stretch Parameter"));

		Boolean DoDialog (
				double* 								gaussianStretchPtr);


		enum { IDD = IDD_GaussianParameter };

	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();

		wxBoxSizer							*bSizer308;
	
		wxButton								*m_button75,
												*m_button76;
	
		wxStaticText						*m_staticText289,
												*m_staticText290;
	
		wxTextCtrl							*m_textCtrl130;
	
		double 								m_gaussianStretch;

};	// end "class CMGaussianParameterDlg"
