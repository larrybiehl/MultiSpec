//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WClassGroupDialog.h
//	Implementation:		WClassGroupDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMClassGroupDlg class
//
//	Written By:				Larry L. Biehl			Date: 0??/??/1990's
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WOneColumnDialog.h"


class CMClassGroupDlg : public CMOneColDlg
{
	// Construction
	public:
		CMClassGroupDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				UInt32*								numberOutputItemsPtr,
				SInt16* 								listPtr,
				SInt16								minimumNumberItems,
				SInt16								numberInputItems,
				SInt16								classGroupCode);

		// Dialog Data
		//{{AFX_DATA (CMClassGroupDlg)
			// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMClassGroupDlg)
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
		
	
		SInt16								m_classGroupCode;
	
		Boolean								m_initializedFlag;
	
};	// end class CMClassGroupDlg
