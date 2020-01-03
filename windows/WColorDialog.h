//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WColorDialog.h
//	Implementation:		WColorDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMColorDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/2019
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once


class CMColorDialog : public CColorDialog
{
	// Construction
	public:
		CMColorDialog ( 	// standard constructor
				COLORREF 							clrInit = 0,
				DWORD 								dwFlags = 0,
				CWnd* 								pParentWnd = NULL);

		// Dialog Data
		//{{AFX_DATA (CMColorDialog)          
			// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMColorDialog)
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
};	// end class CMColorDialog
