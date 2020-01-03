//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WOverlayParameterDlg.h
//	Implementation:		WOverlayParameterDlg.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMOverlayParameterDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMOverlayParameterDlg : public CMDialog
{
	// Construction
	public:
		CMOverlayParameterDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
	
		Boolean DoDialog (
				WindowInfoPtr						windowInfoPtr,
				SInt16								overlayCode,
				SInt16								selection);

		// Dialog Data
		//{{AFX_DATA (CMOverlayParameterDlg)
		enum { IDD = IDD_OverlayParameters };
	
		int									m_overlayValue;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMOverlayParameterDlg)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnOverlayColor ();
	
		afx_msg void OnPaint ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()

		WindowInfoPtr						m_windowInfoPtr;

		int									m_lineThickness,
												m_maxValue,
												m_minValue,
												m_transparency;

		SInt16								m_overlayCode,
												m_overlayIndex;
	
		Boolean								m_initializedFlag;
	
};	// end class CMOverlayParameterDlg
