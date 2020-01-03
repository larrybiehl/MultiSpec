//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WPrincipalComponentsDialog.h
//	Implementation:		WPrincipalComponentsDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMPrincipalCompDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMPrincipalCompDialog : public CMDialog
{
	// Construction
	public:
		CMPrincipalCompDialog (
				CWnd* 								pParent = NULL);
	
		~CMPrincipalCompDialog ();		// standard desctructor
	
		SInt16 DoDialog (void);

		// Dialog Data
		//{{AFX_DATA (CMPrincipalCompDialog)
		enum { IDD = IDD_PrincipalComponent };
	
		int									m_dataCode;
	
		BOOL									m_diskFileFlag,
												m_equalVariancesFlag,
												m_listEigenvectorFlag,
												m_saveEigenvectorFlag,
												m_testFlag,
												m_textWindowFlag,
												m_trainingFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		void HideFieldTypeItems ();
	
		void ShowFieldTypeItems ();

		// Generated message map functions
		//{{AFX_MSG (CMPrincipalCompDialog)
		afx_msg void OnClasses ();
	
		afx_msg void OnImageArea ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnSelendokChannelCombo ();
	
		afx_msg void OnSelendokClassCombo ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		Boolean								m_initializedFlag;
	
};	// end class CMPrincipalCompDialog
