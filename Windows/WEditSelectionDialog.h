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
//	File:						WEditSelectionDialog.h
//	Implementation:		WEditSelectionDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMEditCoordinatesDlg class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDialog.h"


class CMEditCoordinatesDlg : public CMDialog
{
	// Construction
	public:
		CMEditCoordinatesDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				WindowPtr							windowPtr,
				WindowInfoPtr						windowInfoPtr,
				LongRect*							inputSelectionRectanglePtr,
				LongRect*							selectionRectanglePtr,
				DoubleRect*							coordinateRectanglePtr,
				SInt16								pointType,
				SInt16*								unitsDisplayCodePtr,
				Boolean*								changedFlagPtr,
				Boolean*								applyToAllWindowsPtr,
				Boolean*								useStartLineColumnFlagPtr,
				Boolean*								previewWasUsedFlagPtr,
				SInt16								stringID);

		// Dialog Data
		//{{AFX_DATA (CMEditCoordinatesDlg)
		enum { IDD = IDD_FieldCoordinates };
	
		double								m_newColumnEnd,
												m_newColumnStart,
												m_newLineEnd,
												m_newLineStart;
	
		int									m_listSelectionUnits;
	
		BOOL									m_applyToAllWindowsFlag,
												m_useStartLineColumnFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		Boolean CheckLineColumnValues (
				UInt16*								controlIDPtr);

		void CheckOKFlag (void);

		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
		
		// Generated message map functions
		//{{AFX_MSG (CMEditCoordinatesDlg)
		afx_msg void OnApplyToAllCheckbox ();
	
		afx_msg void OnChangeNewColumnEnd ();
	
		afx_msg void OnChangeNewColumnStart ();
	
		afx_msg void OnChangeNewLineEnd ();
	
		afx_msg void OnChangeNewLineStart ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnPreview ();
		
		afx_msg void OnSelendokCoordinateUnits ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP () 
		
		DoubleRect							m_inputCoordinateRectangle,
												m_minMaxCoordinateRectangle;

		LongRect								m_minMaxSelectionRectangle;
	
		DoubleRect*							m_coordinateRectanglePtr;
		
		WindowPtr							m_windowPtr;

		LongRect								*m_inputSelectionRectanglePtr,
												*m_selectionRectanglePtr;
	
		WindowInfoPtr						m_windowInfoPtr;

		double								m_realValue;

		SInt16								m_pointType,
												m_selectionUnits,
												m_unitsDisplayCode;

		Boolean								m_coordinateUnitsChangingFlag,
												m_initializedFlag,
												m_previewWasUsedFlag,
												m_stringID,
												m_valueChangedFlag;
	
	
};	// end class CMEditCoordinatesDlg
