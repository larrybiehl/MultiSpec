//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WTextFrame.h
//	Implementation:		WTextFrame.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMTextFrame class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/13/2019
//
//------------------------------------------------------------------------------------

#pragma once

#ifndef __AFXEXT_H__
	#include <afxext.h>
#endif


class CMTextFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE (CMTextFrame)
	protected:
		CMTextFrame ();			// protected constructor used by dynamic creation

	// Implementation
	public:
		virtual ~CMTextFrame ();

		// Generated message map functions
		//{{AFX_MSG (CMTextFrame)
		afx_msg void OnClose ();
	
		afx_msg int OnCreate (
				LPCREATESTRUCT 					lpCreateStruct);
	
		afx_msg void OnInitMenuPopup (
				CMenu* 								pPopupMenu,
				UINT 									nIndex,
				BOOL 									bSysMenu);
	
		afx_msg BOOL OnSetCursor (
				CWnd* 								pWnd,
				UINT 									nHitTest,
				UINT 									message);
	
		afx_msg void OnUpdateFileTextClose (
				CCmdUI* 								pCmdUI);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
};	// end class CMTextFrame
