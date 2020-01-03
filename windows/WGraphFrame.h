//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WGraphFrame.h
//	Implementation:		WGraphFrame.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMGraphFrame class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once


class CMGraphFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CMGraphFrame)
	protected:
		CMGraphFrame ();			// protected constructor used by dynamic creation
	
		virtual ~CMGraphFrame ();

		// Generated message map functions
		//{{AFX_MSG (CMGraphFrame)
		afx_msg void OnClose ();
	
		afx_msg void OnFilePrint ();
	
		afx_msg void OnFilePrintPreview ();
	
		afx_msg void OnSysCommand (
				UINT 									nID,
				LPARAM 								lParam);
	
		afx_msg void OnUpdateFileGraphClose (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFilePrint (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFilePrintPreview (
				CCmdUI* 								pCmdUI);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		CMGraphView*						m_graphViewCPtr;
	
	public:
		virtual BOOL OnCreateClient (
				LPCREATESTRUCT 					lpcs,
				CCreateContext* 					pContext);
	
		virtual BOOL PreCreateWindow (
				CREATESTRUCT& 						cs);
	
		void SetGraphViewCPtr (
				CMGraphView*						graphViewCPtr);
	
};	// end class CMGraphFrame
