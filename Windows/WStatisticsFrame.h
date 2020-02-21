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
//	File:						WStatisticsFrame.h
//	Implementation:		WStatisticsFrame.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMStatisticsFrame class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/13/2019
//
//------------------------------------------------------------------------------------

#pragma once


class CMStatisticsFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CMStatisticsFrame)
	
	protected:
		CMStatisticsFrame ();			// protected constructor used by dynamic creation
	
		virtual ~CMStatisticsFrame ();

		// Generated message map functions
		//{{AFX_MSG (CMStatisticsFrame)
		afx_msg void OnChar (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnClose ();
	
		afx_msg void OnEditClearSelectRectangle ();
	
		afx_msg void OnEditCut ();
	
		afx_msg void OnEditPaste ();
	
		afx_msg void OnEditSelectAll ();
	
		afx_msg void OnEditSelectionRectangle ();
	
		afx_msg void OnEditUndo ();
	
		afx_msg void OnFileProjectClose ();
	
		afx_msg void OnUpdateEditClearSelectRectangle (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditCut (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditPaste (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditSelectAll (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditSelectionRectangle (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditUndo (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFileProjectClose (
				CCmdUI* 								pCmdUI);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	

	public:
		virtual BOOL PreCreateWindow (
				CREATESTRUCT& 						cs);
	
};	// end class CMStatisticsFrame
