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
