//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WClassesDialog.h
//	Implementation:		WClassesDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMClassesDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1990's
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WOneColumnDialog.h"


class CMClassesDlg : public CMOneColDlg
{
	// Construction
	public:
		CMClassesDlg (	// standard constructor
				CWnd* 								pParent = NULL);
		
		Boolean DoDialog (
				UInt32*								numberOutputClassesPtr,
				SInt16* 								classListPtr,
				SInt16								minimumNumberClasses,
				SInt16								numberInputClasses);

		// Dialog Data
		//{{AFX_DATA (CMClassesDlg)
		enum { IDD = IDD_Channels };
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMClassesDlg)
		virtual BOOL OnInitDialog ();                        
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP () 
		
		Boolean								m_initializedFlag;
	
};	// end class CMClassesDlg
