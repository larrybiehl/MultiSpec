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
