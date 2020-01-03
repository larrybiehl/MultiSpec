//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WAbout.h
//	Implementation:		WAbout.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CAboutDlg class
//
//	Written By:				Larry L. Biehl			Date: 05/23/1995
//	Revised By:				Larry L. Biehl			Date: 12/31/2017
//	
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"
#include "afxwin.h"

class CAboutDlg : public CMDialog
{
	public:
		CAboutDlg (
				CWnd* 								pParent = NULL);
		
		BOOL Create (
				CWnd* 								pParent);
		
		Boolean DoDialog (void);
	
		afx_msg void OnStnClickedStatic11 ();
	
		afx_msg void OnBnClickedCheck1 ();
	
		afx_msg void OnBnClickedSimplerversion ();
	
		// Dialog Data
		//{{AFX_DATA (CAboutDlg)
		enum { IDD = IDD_ABOUTBOX };
		//}}AFX_DATA
	
		BOOL 									m_simplerVersionFlag;
			
	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support
		
		Boolean								m_fromStartUpFlag,
												m_FullVersionSettingFlag,
												m_initializedFlag;
		
	//{{AFX_MSG (CAboutDlg)
	virtual BOOL OnInitDialog ();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP ()
	
};	// end class CAboutDlg
