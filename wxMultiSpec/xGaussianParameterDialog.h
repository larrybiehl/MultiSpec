//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
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
