//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
