//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WNewClassFieldDialog.h
//	Implementation:		WNewClassFieldDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMNewClassFieldDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 03/16/2017
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDialog.h"


class CMNewClassFieldDlg : public CMDialog
{
	// Construction
	public:
		CMNewClassFieldDlg (	// standard constructor
				CWnd* 								pParent = NULL);

		// Dialog Data
		//{{AFX_DATA (CMNewClassFieldDlg)
		enum { IDD = IDD_ClassField };
	
		CString								m_className,
												m_fieldName;
	
		int									m_classList,
												m_fieldType;
		//}}AFX_DATA
	
		Boolean DoDialog (
				Boolean								newClassOnlyFlag,
				UCharPtr								classNamePtr,
				UCharPtr								fieldNamePtr,
				SInt16*		 						fieldTypePtr);

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMNewClassFieldDlg)
		afx_msg void OnFieldType ();
	
		virtual BOOL OnInitDialog ();
	
		virtual void OnOK ();
	
		afx_msg void OnSelendokClassList ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		SInt64								m_numberSelectionPixels;
	
		UCharPtr								m_classNamePtr,
												m_fieldNamePtr;
	
		SInt16*								m_fieldTypePtr;
	
		TBYTE									*m_classNameCStringPtr,
												*m_fieldNameCStringPtr;
	
		Boolean								m_initializedFlag,
												m_newClassOnlyFlag;
	
};	// end class CMNewClassFieldDlg
