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
//	File:						WOneColumnDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/03/2018
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMOneColDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SImageWindow_class.h"

#include "WOneColumnDialog.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMOneColDlg, CMDialog)
	//{{AFX_MSG_MAP (CMOneColDlg)
	ON_BN_CLICKED (IDC_AllSelected, OnAllSelected)
	ON_BN_CLICKED (IDC_EnterNewRange, OnEnterNewRange)
	ON_BN_CLICKED (IDC_NoneSelected, OnNoneSelected)

	ON_EN_CHANGE (IDC_First, OnChangeFirst)
	ON_EN_CHANGE (IDC_Interval, OnChangeInterval)
	ON_EN_CHANGE (IDC_Last, OnChangeLast)

	ON_LBN_SELCHANGE (IDC_List1, OnSelchangeList1)

	ON_WM_SHOWWINDOW ()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMOneColDlg::CMOneColDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMOneColDlg::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMOneColDlg)
	m_listStart = 1;
	m_listInterval = 1;
	m_listEnd = 1;
	m_selectionCount = "1";
	//}}AFX_DATA_INIT
	
			// The m_indexStart variable is used to specify a 0 base or 1 base list.
			// I.E. the list starts at 0 or 1.
			        
	m_currentSelection = 0;   
	m_indexStart = 0;         
	m_listType = 0; 
	m_numberOutputItems = 0;
	m_selectedItemsPtr = NULL; 
	m_numberInputVecItems = 0;
	m_numberSelections = 0;
	m_minimumItemsRequired = 1; 
	                        
	m_initializedFlag = CMDialog::m_initializedFlag;
	
}	// end "CMOneColDlg"



void CMOneColDlg::CheckValue (
				UInt16								itemNumber,
				UINT									lastValue,
				UINT*									lastValuePtr)

{
	UINT									newValue;
	
	
	DDX_Text (m_dialogFromPtr, itemNumber, newValue);
	
	if (newValue <= 0 || newValue > m_numberInputVecItems)
		{
		DDX_Text (m_dialogToPtr, itemNumber, lastValue);
		
		DDV_MinMaxUInt (m_dialogFromPtr, newValue, 1, (UInt16)m_numberInputVecItems);
		
		}	// end "if (m_listInterval <= 0 || ..."
	
	else	// newValue > 0 && newValue <= m_numberInputVecItems
		*lastValuePtr = newValue;
	
	OnEnterNewRange ();
	
}	// end "CheckValue"



void CMOneColDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMOneColDlg)
	DDX_Text (pDX, IDC_First, m_listStart);
	DDV_MinMaxUInt (pDX, m_listStart, 1, (UInt16)m_numberInputVecItems);
	DDX_Text (pDX, IDC_Interval, m_listInterval);
	DDV_MinMaxUInt (pDX, m_listInterval, 1, (UInt16)m_numberInputVecItems);
	DDX_Text (pDX, IDC_Last, m_listEnd);
	DDV_MinMaxUInt (pDX, m_listEnd, 1, (UInt16)m_numberInputVecItems);
	DDX_Text (pDX, IDC_SelectionCount, m_selectionCount);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



void CMOneColDlg::OnAllSelected ()

{
	UInt16								item;
	
	
	for (item=0; item<m_numberInputVecItems; item++)
		((CListBox*)GetDlgItem (IDC_List1))->SetSel (item, TRUE);
	
	m_numberSelections = (UInt16)m_numberInputVecItems;
	
	UpdateNumberOfSelections ();
	
}	// end "OnAllSelected"



void CMOneColDlg::OnChangeFirst (void)

{							                     
	CheckValue (IDC_First, m_listStart, &m_listStart); 
	
}	// end "OnChangeFirst"



void CMOneColDlg::OnChangeInterval (void)

{                     
	CheckValue (IDC_Interval, m_listInterval, &m_listInterval);
	
}	// end "OnChangeInterval"



void CMOneColDlg::OnChangeLast (void)

{                             
	CheckValue (IDC_Last, m_listEnd, &m_listEnd);
	
}	// end "OnChangeLast"



void CMOneColDlg::OnEnterNewRange (void)

{
	m_numberSelections = UpdateOneColListSelections ((UInt16)m_numberInputVecItems,
																		m_listStart,
																		m_listEnd,
																		m_listInterval);
	
	UpdateNumberOfSelections ();
	
}	// end "OnEnterNewRange"



BOOL CMOneColDlg::OnInitDialog (void)

{
	CMDialog::OnInitDialog ();
	
			// Initialize the channel range text edit items.
	
	m_listStart = 1;
	m_listInterval = 1;
	m_listEnd = (UInt16)m_numberInputVecItems;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"



void CMOneColDlg::OnNoneSelected (void)

{
	UInt16								item;
	
	
	for (item=0; item<m_numberInputVecItems; item++)
		((CListBox*)GetDlgItem (IDC_List1))->SetSel (item, FALSE);
	
	m_numberSelections = 0;
	
	UpdateNumberOfSelections ();
	
}	// end "OnNoneSelected"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CMOneColDlg::OnOK
//
//	Software purpose:	The purpose of this routine is to fill the vector
//							with the selected items in the one column list.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: ??/??/19??
//	Revised By:			Larry L. Biehl			Date: 04/30/1998

void CMOneColDlg::OnOK ()

{
			// Get items to use.
	
	if (m_listType == kSelectItemsList || m_listType == kSelectPCList)
		{
		UInt16 index = 0;
		UInt16 item;
			
		for (item=0; item<m_numberInputVecItems; item++)
			{
			if (((CListBox*)GetDlgItem (IDC_List1))->GetSel (item) > 0)
				{
				m_selectedItemsPtr[index] = item + m_indexStart;
				index++;
				
				}	// end "if (((CListBox*)GetDlgItem (IDC_List1))->GetSel ..."
			
			}	// end "for (channel=0; channel<..."
		
		m_numberSelections = index;
		
		}	// end "if (m_listType == kSelectItemsList || ..."
	
	CMDialog::OnOK ();
	
}	// end "OnOK"
 


void CMOneColDlg::OnSelchangeList1 (void)

{
	m_numberSelections = ((CListBox*)GetDlgItem (IDC_List1))->GetSelCount ();
	
	UpdateNumberOfSelections ();
	
}	// end "OnSelchangeList1"



void CMOneColDlg::OnShowWindow (
				BOOL 									bShow,
				UINT 									nStatus)

{
	CMDialog::OnShowWindow (bShow, nStatus);
	
	if (bShow)
		{
				// Set default text selection to first edit text item 
			                                       
		SelectDialogItemText (this, IDC_First, 0, SInt16_MAX);
		
		}	// end "if (bShow)"
	
}	// end "OnShowWindow"



void CMOneColDlg::UpdateNumberOfSelections (void)

{
	if (m_listType != kItemsListOnly && m_listType != kSelectedItemsListOnly)
		{
				// Hilite OK button depending on number of selections.
		
		if (m_numberSelections >= m_minimumItemsRequired)
			SetDLogControlHilite (this, IDOK, 0);
		
		else	// m_numberSelections < m_minimumItemsRequired
			SetDLogControlHilite (this, IDOK, 255);
	
				// Set the number of selected items.
		
		::LoadDItemValue (this, IDC_SelectionCount, m_numberSelections);
		
		}	// end "if (m_listType != kChannelsListOnly && ..."
	
}	// end "UpdateNumberOfSelections"
