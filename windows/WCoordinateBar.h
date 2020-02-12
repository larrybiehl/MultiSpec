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
//	File:						WCoordinateBar.h
//	Implementation:		WCoordinateBar.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMCoordinateBar class
//
//	Written By:				Larry L. Biehl			Date: ??/??/2019
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once

	
class CMCoordinateBar : public CDialogBar
{
	public:                  
		CMCoordinateBar ();
	                 
		virtual ~CMCoordinateBar ();
			
		virtual void OnDraw (
				CDC* 									pDC);  // overridden to draw this view
										
		virtual void OnUpdate (
				CView*								pSender,
				LPARAM								lHint = 0L,
				CObject*								pHint = NULL);
		
		void UpdateCoordinateView (
				Handle								windowInfoHandle);
	
	
		CDataExchange						*m_dialogToPtr,
												*m_dialogFromPtr;

		int									m_areaUnitsListSelection,
												m_displayUnitsListSelection;

		SInt16								m_areaUnitsCode,
												m_displayUnitsCode;


	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		// Generated message map functions
		//{{AFX_MSG (CMImageFrame)
		afx_msg void OnCloseupAreaUnitsCombo ();
	
		afx_msg void OnDropdownAreaUnitsCombo ();
	
		afx_msg void OnPaint ();
	
		afx_msg void OnSelendokAreaUnits ();
	
		afx_msg void OnSelendokDisplayUnits ();
	
		afx_msg BOOL OnSetCursor (
				CWnd* 								pWnd,
				UINT 									nHitTest,
				UINT 									message);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
};	// end class CMCoordinateBar
