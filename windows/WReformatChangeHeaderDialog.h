//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WReformatChangeHeaderDialog.h
//	Implementation:		WReformatChangeHeaderDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMChangeFormatDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/13/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDialog.h"


class CMChangeHeaderDlg : public CMDialog
{
	// Construction
	public:
		CMChangeHeaderDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		SInt16 DoDialog (
				SInt16*								requestedFormatPtr);
	

		// Dialog Data
		//{{AFX_DATA (CMChangeHeaderDlg)
		enum { IDD = IDD_ChangeHeader };
	
		int									m_headerOptionsSelection;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		void LoadCurrentHeaderParametersInDialog (
				CDataExchange* 					dxToPtr);

		// Generated message map functions
		//{{AFX_MSG (CMChangeHeaderDlg)
		afx_msg void OnChangeMapParameters ();
	
		afx_msg void OnChangeParameters ();
	
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		Boolean								m_initializedFlag;
	
};	// end class CMChangeHeaderDlg
