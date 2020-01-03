//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
