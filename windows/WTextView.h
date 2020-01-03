//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WTextView.h
//	Implementation:		WTextView.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMTextView class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 03/27/2017
//
//------------------------------------------------------------------------------------

#pragma once

	
class CMTextView : public CEditView
{
	DECLARE_DYNCREATE (CMTextView)
	protected:
		CMTextView ();		// protected constructor used by dynamic creation
	
		virtual ~CMTextView ();
	
		virtual void OnInitialUpdate ();	// first time after construct

		virtual void OnActivateView (
				BOOL 									bActivate,
				CView* 								pActivateView,
				CView* 								pDeactiveView);
	
		virtual BOOL PreCreateWindow (
				CREATESTRUCT& 						cs);
	
			// Generated message map functions
	
		//{{AFX_MSG (CMTextView)
		afx_msg void OnChar (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg int OnCreate (
				LPCREATESTRUCT 					lpCreateStruct);
	
		afx_msg void OnFilePrintPreview ();
	
		afx_msg void OnMouseMove (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg BOOL OnSetCursor (
				CWnd* 								pWnd,
				UINT 									nHitTest,
				UINT 									message);
	
		afx_msg void OnUpdateEditClear (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditCopy (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditCut (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditPaste (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditSelectAll (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFileClose (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFilePrint (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFilePrintPreview (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFilePrintSetup (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFileSave (
				CCmdUI* 								pCmdUI);
				
		afx_msg void OnUpdateFileSaveAs (
				CCmdUI* 								pCmdUI);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		#ifdef _DEBUG
			virtual void AssertValid () const;
	
			virtual void Dump (
					CDumpContext& 						dc) const;
		#endif
	
	
		CFont 								m_font;
	
		static LOGFONT NEAR 				m_lfDefFont;
	
				 // Flag indicated whether this view is active
		Boolean	 							m_activeFlag;

	
	// Attributes
	public:
		CMTextDoc* GetDocument ();

		SInt32 GetTextBufferLength (void);

		Boolean ListString (
				HPtr									textPtr,
				UInt32								textLength,
				SInt16								charFormatCode=kASCIICharString);
	
};	// end class CMTextView


#ifndef _DEBUG  // debug version in mspecvw.cpp
	inline CMTextDoc* CMTextView::GetDocument ()
   	{ return (CMTextDoc*)m_pDocument; }
#endif
