//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WEditClassGroupDialog.h
//	Implementation:		WEditClassGroupDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMEditClassGroupDlg class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDialog.h"
#include "WLegendList.h"


class CMEditClassGroupDlg : public CMDialog
{
	// Construction
	public:
		CMEditClassGroupDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				CMLegendList*						legendListCPtr,
				SInt16								selectedCell,
				SInt16								newEditCode,
				SInt16								classGroupIndex,
				Handle								fileInfoHandle,
				Handle								nameHandle);

		// Dialog Data
		//{{AFX_DATA (CMEditClassGroupDlg)
		enum { IDD = IDD_EditClassGroupName };
	
		CString								m_classGroupName;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMEditClassGroupDlg)
		virtual BOOL OnInitDialog ();
	
		virtual void OnOK ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		TBYTE*								m_classGroupCStringPtr;
	
		Handle								m_fileInfoHandle,
												m_nameHandle;
	
		SInt16								m_classGroupIndex,
												m_newEditCode,
												m_numberClassesGroups;
	
		Boolean								m_initializedFlag,
												m_noChangeFlag;
	
};	// end class CMEditClassGroupDlg
