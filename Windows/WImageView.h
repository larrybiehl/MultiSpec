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
//	File:						WImageView.h
//	Implementation:		WImageView.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating Systems
//
//	Brief description:	This file is the definition for the CMImageView class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 04/17/2020
//
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"

#include "WDrawObjects.h"
#include "WLegendView.h"

class CMImageView : public CScrollView
	{
	DECLARE_DYNCREATE(CMImageView)
	
	protected: // create from serialization only
		CMImageView ();

		void OnMenuSelect (
				UINT									nItemID,
				UINT									nFlags,
				HMENU									hMenu);
		
	public:
		virtual ~CMImageView ();
	
		Boolean CheckIfOffscreenImageExists (void);
		
		void ClientToDoc (
				CPoint&								point);
	
		void DisposeImageWindowSupportMemory (void);
		
		Boolean DoDeleteKey (
				UINT									nChar);
		
		void DoEditCopy (void);
		
		void DoFilePrint ();
		
		void DoFilePrintPreview ();

		void DrawDC (
				CDC*									pDC);
	
		void DrawLegend ();
		
		Boolean GetActiveWindowFlag (void);
		
		SInt16 GetClassGroupCode (void);

		Boolean GetControlKeyFlag (void);
	
		UInt16 GetDisplayPixelSize (void);
	
		Handle GetDisplaySpecsHandle (void);
	
		CMImageDoc* GetDocument (void);
	
		CMImageFrame* GetImageFrameCPtr (void);

		CMLegendList* GetImageLegendListCPtr (void);

		CMLegendView* GetImageLegendViewCPtr (void);
		
		SInt16 GetImageType (void);

		CMImageWindow* GetImageWindowCPtr (void);

		SInt32 GetLegendFullHeight (void);

		SInt16 GetLegendWidth (void);
		
		SInt16 GetNumberGroups (void);
		
		CMOutlineArea* GetSelectionAreaCPtr (void);
		
		SInt16 GetTitleHeight (void);
		
		SInt16 GetWindowType (void);

		virtual void OnDraw (
						// overridden to draw this view
				CDC*									pDC);
		
		virtual void OnPrint (
				CDC*									pDC,
				CPrintInfo*							pInfo);
		
		virtual void OnUpdate (
				CView*								pSender,
				LPARAM								lHint = 0L,
				CObject*								pHint = NULL);
	
		void Remove (
				CMDrawObject*						pObject);
	
		void ScrollH (
				UINT									nSBCode);

		void ScrollV (
				UINT									nSBCode);
	
		void SetControlKey (
				Boolean								settingFlag);
	
		void SetControlKeyFlag (
				Boolean								flag);

		void SetImageWindowCPtr (
				CMImageWindow						*imageWindowCPtr);

		void SetLegendBitMapInfoHeaderHandle (
				Handle								bitMapInfoHeaderHandle);
	
		void UpdateCursorCoordinates (void);
	
		void UpdateCursorCoordinates (
				LongPoint*							mousePoint);
		
		void UpdateSelectionCoordinates (void);
		
		void ZoomIn (void);
		
		void ZoomOut (void);
	
		void UpdateScrolls (
				double								newMagnification);
	
	
				// Pointer to CDC class
		CDC*									m_pDC;
	
				// Pointer to the display class for multispectral image.
		CMDisplay*							m_displayMultiCPtr;
	
				// Pointer to the histogram class for the image.
		CMHistogram*						m_histogramCPtr;
	
		double 								m_dpiScale,
												m_printerTextScaling;
	
		SInt16								m_xPixelsPerInch;
		SInt16								m_yPixelsPerInch;
	
				// Flag indicating whether the current draw mode is for printing
				// or copying.
		Boolean								m_printCopyModeFlag;
	
				// Flag indicating whether the class members have been
				// initialized successfully.
		Boolean								m_initializedFlag;

		#ifdef _DEBUG
			virtual void AssertValid () const;
			virtual void Dump (
					CDumpContext&						dc) const;
		#endif
		
	protected:
		SInt32 NormalizeScrollOffset (
				double								magnification,
				SInt32								scrollIncrement,
				SInt32								scrollOffset);
		
		virtual void OnActivateView (
				BOOL									bActivate,
				CView*								pActivateView,
				CView*								pDeactiveView);

		LRESULT CMImageView::OnDPIChanged (
			WPARAM 								wParam,
			LPARAM								lParam);
		
		virtual void OnInitialUpdate ();	// first time after construct
	
		virtual BOOL OnPreparePrinting (
				CPrintInfo* 						pInfo);
		
		BOOL OnScrollBy (
				CSize									sizeScroll,
				BOOL									bDoScroll);
	
		void SetScrollSizes (
				int									nMapMode,
				SIZE									sizeTotal,
				const SIZE&							sizePage,
				const SIZE&							sizeLine);
		
		void UpdateOffscreenMapOrigin (void);
		
		void UpdateScrollRange (
				double								magnification);
	
		// Generated message map functions
		//{{AFX_MSG (CMImageView)
		afx_msg LRESULT OnDoRealize (
				WPARAM								wParam,
				LPARAM								lParam);  // user message
	
		afx_msg void OnChar (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnEditCut ();
	
		afx_msg void OnEditPaste ();
	
		afx_msg BOOL OnEraseBkgnd (
				CDC* 									pDC);
	
		afx_msg void OnHScroll (
				UINT 									nSBCode,
				UINT 									nPos,
				CScrollBar* 						pScrollBar);
	
		afx_msg void OnKeyDown (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnKeyUp (
				UINT									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnLButtonDown (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg void OnLButtonUp (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg void OnLButtonDblClk (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg void OnMouseMove (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg BOOL OnMouseWheel (
				UINT 									nFlags,
				short 								zDelta,
				CPoint 								point);
	
		afx_msg void OnNcMouseMove (
				UINT 									nFlags,
				CPoint 								point);
	
		afx_msg void OnPaint ();

		afx_msg void OnRButtonDown (
				UINT 									nFlags,
				CPoint 								point);
	
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
	
		afx_msg void OnSysKeyDown (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnSysKeyUp (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnUpdateEditClear (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditCut (
				CCmdUI*								pCmdUI);
	
		afx_msg void OnUpdateEditPaste (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateEditUndo (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFilePrintSetup (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnVScroll (
				UINT 									nSBCode,
				UINT 									nPos,
				CScrollBar* 						pScrollBar);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
				// This structure contains the rectangle to be updated in the current
				// window.
		static RECT							s_updateRect;
	
		static SInt16						s_currentCursor;
	
				// Pointer to the image window class for the image view.
		CMImageWindow*						m_imageWindowCPtr;
	
				// Saved cursor line and column value
		SInt32								m_cursorColumnValue,
												m_cursorLineValue;
	
				 // Flag indicated whether this view is active
		Boolean								m_activeFlag;
	
				 // Flag indicating whether UpdateScrollRange should be called.
		Boolean								m_callUpdateScrollRangeFlag;
		
				 // Flag indicating whether the control key is down indicating that
				 // zooming should occur at .1 increments.
		Boolean								m_ctlKeyDownFlag;
	
				 // Flag indicating whether the control key is down indicating that
				 // zooming should occur at .1 increments.
		Boolean								m_thumbScrollFlag;
	
				 // Flag indicated whether already within 'OnSize' routine.
		Boolean								m_withinOnSizeFlag;
		
};	// end class CMImageView


#ifndef _DEBUG  // debug version in WImageView.cpp
	inline CMImageDoc* CMImageView::GetDocument ()
		{ 
		return (CMImageDoc*)m_pDocument; 
		
		}	// end "inline CMImageDoc* CMImageView::GetDocument"
			
	inline CMImageWindow* CMImageView::GetImageWindowCPtr ()
		{ 
		if (this == NULL)
			return (NULL);
				
		else
			return (CMImageWindow*)m_imageWindowCPtr;
			
		}	// end "inline CMImageWindow* CMImageView::GetImageWindowCPtr"
#endif	// !_DEBUG
