//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WStatusDialog.h
//	Implementation:		WStatusDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CShortStatusDlg and CMListStatsDialog
//								classes
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/13/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


//------------------------------------------------------------------------------------
// CShortStatusDlg dialog

class CShortStatusDlg : public CMDialog
{
	// Construction
	public:
		CShortStatusDlg (		// standard constructor
				UInt16 								identifier,
				CWnd* 								pParent = NULL);

		// Dialog Data
		//{{AFX_DATA (CShortStatusDlg)
		enum { IDD = IDD_GraphicsStatus };
	
		Boolean								m_initializedFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		void PostNcDestroy () ;

		// Generated message map functions
		//{{AFX_MSG (CShortStatusDlg)
		virtual void OnCancel ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnMouseMove (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg void OnNcMouseMove (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg BOOL OnSetCursor (
				CWnd* 								pWnd,
				UINT 									nHitTest,
				UINT 									message);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()

};	// end class CShortStatusDlg


                        
//------------------------------------------------------------------------------------
// CMListStatsDialog dialog

class CMListStatsDialog : public CMDialog
{
	// Construction
	public:
		CMListStatsDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				SInt16								statsWindowMode);
	
		void CheckListFieldClassSettings (void);
	

		// Dialog Data
		//{{AFX_DATA (CMListStatsDialog)
		enum { IDD = IDD_ListStats };
	
		UINT									m_listCovCorPrecision,
												m_listMeanStdPrecision;
	
		BOOL									m_featureTransformationFlag,
												m_listClassFlag,
												m_listCorrelationFlag,
												m_listCovarianceFlag,
												m_listFieldFlag,
												m_listMeansStdDevFlag;
		//}}AFX_DATA


	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMListStatsDialog)
		afx_msg void OnClasses ();
	
		afx_msg void OnFields ();
	
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		SInt16								m_statsWindowMode;
	
};	// end class CMListStatsDialog
