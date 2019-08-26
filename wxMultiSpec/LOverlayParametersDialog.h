//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LOverlayParametersDialog.h
//	Implementation:		LOverlayParametersDialog.cpp
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
//
#if !defined(AFX_WOVERLAYPARAMETERDLG_H)
#define AFX_WOVERLAYPARAMETERDLG_H

#include "LDialog.h"


class CMOverlayParameterDlg : public CMDialog
	{
			// Construction
	public:
				// standard constructor
		CMOverlayParameterDlg (
				wxWindow* 			pParent = NULL,
				wxWindowID 			id = wxID_ANY,
				const wxString& 	title = wxT("Set Overlay Parameters"));

		Boolean DoDialog (
				WindowInfoPtr 		windowInfoPtr,
				SInt16 				overlayCode,
				SInt16 				selection);

		enum { IDD = IDD_OverlayParameters };
		int m_overlayValue;
	
	protected:
		void CreateControls ();
		void OnInitDialog (wxInitDialogEvent& event);
		bool TransferDataFromWindow ();
		bool TransferDataToWindow ();

			// Implementation
	protected:
		void OnOverlayColor (wxCommandEvent& event);
		void OnPaint ();
		DECLARE_EVENT_TABLE ();

		WindowInfoPtr m_windowInfoPtr;

		Boolean m_initializedFlag;

		SInt16 		m_overlayCode,
					m_overlayIndex;

		int 			m_lineThickness,
					m_maxValue,
					m_minValue,
					m_transparency;

		RGBColor	m_currentSelectedColor;

		wxBoxSizer* bSizer107;
		wxButton* m_button23;
		wxStaticBitmap* m_bitmap3;
		wxTextCtrl* m_textCtrl55;
		wxStaticText* m_staticText139;
		wxButton* m_button20;
		wxButton* m_button21;
	
	};
#endif	// !defined(AFX_WOVERLAYPARAMETERDLG_H)
