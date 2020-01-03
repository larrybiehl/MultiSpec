//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
