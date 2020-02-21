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
//	File:						WTextDoc.h
//	Implementation:		WTextDoc.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMTextDoc class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/13/2019
//
//------------------------------------------------------------------------------------

#pragma once


class CMTextDoc : public CDocument
{
	DECLARE_SERIAL (CMTextDoc)
	protected:
		CMTextDoc ();			// protected constructor used by dynamic creation
	
		virtual ~CMTextDoc (void);
	
		void InitDocument (void);
	
		virtual void Serialize (
				CArchive& 							ar);	// overridden for document i/o
	
		void OnCloseDocument (void);
	
		virtual BOOL OnSaveDocument (
				LPCTSTR								lpszPathName);
	
		virtual BOOL OnNewDocument (void);
	
		// Generated message map functions
		//{{AFX_MSG (CMTextDoc)
		afx_msg void OnUpdateViewCoordinatesBar (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnViewCoordinatesBar ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
		
	
		CSize       						m_sizeDoc;


	public:
		CSize GetDocSize () { return m_sizeDoc; }
		
		BOOL SaveModified (void);

};	// end class CMTextDoc
