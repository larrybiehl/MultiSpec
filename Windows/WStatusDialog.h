//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
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
