//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LGaussianParameterDlg.h
//	Implementation:		LGaussianParameterDlg.cpp
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
//
#if !defined __LGaussianParameterDlg_H_
	#define __LGaussianParameterDlg_H_

#include "LDialog.h"


class CMGaussianParameterDlg : public CMDialog
	{
		 // Construction
	public:
				// standard constructor
		CMGaussianParameterDlg (
				wxWindow* 			pParent = NULL,
				wxWindowID 			id = wxID_ANY,
				const wxString& 	title = wxT("Set Gaussian Stretch Parameter"));

		Boolean DoDialog (
					double* gaussianStretchPtr);

				// Dialog Data

		enum { IDD = IDD_GaussianParameter };
		double m_gaussianStretch;

	protected:
		void CreateControls ();
		void OnInitDialog (wxInitDialogEvent& event);
		bool TransferDataFromWindow ();
		bool TransferDataToWindow ();

		wxBoxSizer* bSizer308;
		wxStaticText* m_staticText289;
		wxStaticText* m_staticText290;
		wxTextCtrl* m_textCtrl130;
		wxButton* m_button75;
		wxButton* m_button76;

		DECLARE_EVENT_TABLE()
	};
#endif
