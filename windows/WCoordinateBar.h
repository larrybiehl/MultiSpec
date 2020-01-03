//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
