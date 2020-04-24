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
//	File:						WLegendView.h
//	Implementation:		WLegendView.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMButton, CMComboBox, and CMLegendView
//								classes.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WLegendList.h"


#ifndef __AFXEXT_H__
	#include <afxext.h>
#endif                
       

class CMButton : public CButton
{          
	public:
		CMButton ();
	
		virtual ~CMButton ();
	
		void SetButtonID (
				UInt16 								buttonID);

	// Generated message map functions
	protected:
		//{{AFX_MSG (CMButton)
		afx_msg void OnKeyDown (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnKeyUp (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnLButtonDown (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg void OnLButtonUp (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg void OnMouseMove (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg void OnRButtonDown (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg void OnRButtonUp (
				UINT 									nFlags,
				CPoint 								point);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		UInt16	m_buttonID;
							
};	// end class CMButton
       


class CMComboBox : public CComboBox
{          
	public:
		CMComboBox ();
	
		virtual ~CMComboBox ();
	
		void DrawLegendTitle ();

	// Generated message map functions
	protected:
		//{{AFX_MSG (CMComboBox)
		afx_msg void OnKeyDown (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnKeyUp (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnDrawItem (
				int 									/*nIDCtl*/,
				LPDRAWITEMSTRUCT 					lpDrawItemStruct);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
							
};	// end class CMComboBox



class CMLegendView : public CFormView
{
	DECLARE_DYNCREATE(CMLegendView)
	protected:
		CMLegendView (); 	// protected constructor used by dynamic creation
	
		virtual void OnInitialUpdate ();	// first time after construct

	public:
		void AdjustLegendListLength (void);
	
		CMImageDoc* GetDocument (void);
	
		CMImageFrame* GetImageFrameCPtr (void);

		CMImageView* GetImageView (void);
	
		CMLegendList* GetLegendListCPtr (void);
	
		void SetImageView (
				CMImageView*						imageViewCPtr);
	
		void SetLegendWidthSetFlag (void);
	
		void UpdateClassGroupComboList (
				SInt16								newClassGroupCode);
	
		void UpdateThematicLegendControls (void);
	
	
		//{{AFX_DATA (CMLegendView)
		enum { IDD = IDD_LegendWindow };
	
		CMButton								m_legendScrollDown,
												m_legendScrollUp;
	
		CMComboBox							m_legendPaletteCombo,
												m_legendTitleCombo;

		CMLegendList						m_legendListBox;
	
		int									m_classGroupCode,
												m_paletteSelection;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual ~CMLegendView ();
	
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		virtual void OnActivateView (
				BOOL 									bActivate,
				CView* 								pActivateView,
				CView* 								pDeactiveView);
	
		void OnDraw (
				CDC* 									pDC);
	
		virtual void OnUpdate (
				CView*								pSender,
				LPARAM								lHint = 0L,
				CObject*								pHint = NULL);
	
		// Generated message map functions
		//{{AFX_MSG (CMLegendView)
		afx_msg void OnDblclkList1 ();
	
		afx_msg LRESULT OnDoRealize (
				WPARAM 								wParam,
				LPARAM 								lParam);  // user message
	
		afx_msg void OnDropdownClassGroupCombo ();
	
		afx_msg void OnDropdownLegendCombo ();
	
		afx_msg BOOL OnEraseBkgnd (
				CDC* 									pDC);
	
		afx_msg void OnKeyDown (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnKeyUp (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnLButtonUp (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg void OnMouseMove (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg void OnPaint ();
	
		afx_msg void OnScrollDown ();
	
		afx_msg void OnScrollUp ();
	
		afx_msg void OnSelendcancelPaletteCombo ();
	
		afx_msg void OnSelendokClassGroup ();
	
		afx_msg void OnSelendokPalette ();
	
		afx_msg BOOL OnSetCursor (
				CWnd* 								pWnd,
				UINT 									nHitTest,
				UINT 									message);
	
		afx_msg void OnSetFocus (
				CWnd* 								pOldWnd);
	
		afx_msg void OnSize (
				UINT 									nType,
				int 									cx,
				int 									cy);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		CDataExchange						*m_dialogFromPtr,
												*m_dialogToPtr;
	
		CMImageView*						m_imageViewCPtr;
	
		SInt16								m_numberDisplayedListItems;
	
		Boolean								m_initializedFlag,
												m_legendWidthSetFlag,
												m_localPaletteUpToDateFlag,
												m_paletteChangedFlag;

				// Flag indicating whether we are already in the SetLegendWidth
				// routine flag. This is to stop recursion in the routine from
				// call to OnSize..
		static Boolean						s_inSetLegendWidthFlag;
							
};	// end class CMLegendView


inline CMImageDoc* CMLegendView::GetDocument ()
	   { return (CMImageDoc*)m_pDocument; }

inline CMImageView* CMLegendView::GetImageView ()
	   { return (CMImageView*)m_imageViewCPtr; }
