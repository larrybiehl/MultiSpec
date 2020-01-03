//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WOneColumnDialog.h
//	Implementation:		WOneColumnDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMOneColDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDialog.h"

class CMOneColDlg : public CMDialog
{
	// Construction
	public:
 		CMOneColDlg (	// standard constructor
				CWnd* 								pParent = NULL);

	// Dialog Data
		//{{AFX_DATA (CMOneColDlg)
		enum { IDD = IDD_OneColumn };
	
		CString								m_selectionCount;
	
		UINT									m_listEnd,
												m_listInterval,
												m_listStart;
		//}}AFX_DATA

	// Implementation
	protected:
		void CheckValue (
				UInt16								itemNumber,
				UINT									lastValue,
				UINT*									lastValuePtr);
	
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		void UpdateNumberOfSelections (void);

		// Generated message map functions
		//{{AFX_MSG (CMOneColDlg)
		afx_msg void OnAllSelected ();
	
		afx_msg void OnChangeFirst ();
	
		afx_msg void OnChangeInterval ();
	
		afx_msg void OnChangeLast ();
	
		afx_msg void OnEnterNewRange ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnNoneSelected ();
	
		virtual void OnOK ();
	
		afx_msg void OnSelchangeList1 ();
	
		afx_msg void OnShowWindow (
				BOOL 									bShow,
				UINT 									nStatus);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
		
	
		SInt16* 								m_selectedItemsPtr;
	
		UInt32								m_numberInputVecItems;
	
		SInt16								m_currentSelection,
												m_indexStart,
												m_listType,
												m_numberOutputItems;
	
		UInt16								m_minimumItemsRequired,
												m_numberSelections;
	
		Boolean								m_initializedFlag;
	
};	// end class CMOneColDlg
