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
//	File:						xOverlayParametersDialog.h
//	Implementation:		xOverlayParametersDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMOverlayParameterDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 01/28/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMOverlayParameterDlg : public CMDialog
{
			// Construction
	public:
				// standard constructor
		CMOverlayParameterDlg (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = wxID_ANY,
				const wxString& 					title = wxT("Set Overlay Parameters"));

		Boolean DoDialog (
				WindowInfoPtr 						windowInfoPtr,
				SInt16 								overlayCode,
				SInt16 								selection);


		enum { IDD = IDD_OverlayParameters };
	
		int 									m_overlayValue;
	
			// Implementation
	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnOverlayColor (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ();

		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();


		RGBColor								m_currentSelectedColor;
	
		WindowInfoPtr 						m_windowInfoPtr;

		wxBoxSizer							*bSizer107;
	
		wxButton								*m_button23;
	
		wxStaticBitmap						*m_bitmap3;
	
		wxStaticText						*m_staticText139;
	
		wxTextCtrl							*m_textCtrl55;

		int 									m_lineThickness,
												m_maxValue,
												m_minValue,
												m_transparency;

		SInt16 								m_overlayCode,
												m_overlayIndex;

		Boolean 								m_initializedFlag;
	
};	// end "class CMOverlayParameterDlg"
