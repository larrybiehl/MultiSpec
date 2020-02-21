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
