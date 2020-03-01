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
//	Brief description:	Header file for the CMGraphViewButton, CMGraphViewCombo,
//		 						and CMGraphView classes.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 02/29/2020
//
//------------------------------------------------------------------------------------

#pragma once


class CMGraphViewButton : public CButton
{
	public:
		CMGraphViewButton ();
		
		virtual ~CMGraphViewButton ();
	
		void SetButtonID (
				UInt16 								buttonID);

		void SetGraphViewCPtr (
				CMGraphView*						graphViewCPtr);

	protected:
		// Generated message map functions
		//{{AFX_MSG (CMGraphViewButton)
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
	
		void OnBinWidth (
				UINT									menuID);

		void OnLines2 (
				UINT									menuID);

		void OnSelectVector (
				UINT									menuID);
	
	
		CMGraphView*						m_graphViewCPtr;
	
		UInt16								m_buttonID;
	
};	// end class CMGraphViewButton



class CMGraphViewCombo : public CComboBox
{
	public:
		CMGraphViewCombo ();
	
		virtual ~CMGraphViewCombo ();

		void SetGraphViewCPtr (
				CMGraphView*						graphViewCPtr);

	protected:
		// Generated message map functions
		//{{AFX_MSG (CMGraphViewCombo)
		afx_msg void OnKeyDown (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		CMGraphView*						m_graphViewCPtr;
	
};	// end class CMGraphViewCombo



class CMGraphView : public CFormView
{
	DECLARE_DYNCREATE (CMGraphView)
	
	protected:
		CMGraphView ();		// protected constructor used by dynamic creation

		virtual ~CMGraphView ();
	
		virtual void DoDataExchange (
				CDataExchange* 					pDX);
	
		void IntializeGraphRecord (void);
	
		void MoveGraphControls (void);
	
		virtual void OnDraw (
				CDC* 									pDC);		// overridden to draw this view
	
		void OnInitialUpdate (void);

		void OnLines2 (
				UINT									menuID);
	
		virtual BOOL OnPreparePrinting (
				CPrintInfo* 						pInfo);

				// Generated message map functions
	
		//{{AFX_MSG (CMGraphView)
		afx_msg int OnCreate (
				LPCREATESTRUCT 					lpCreateStruct);
	
		afx_msg void OnDestroy ();
	
		afx_msg void OnDropdownXAxisDropList ();
	
		afx_msg void OnEditCopy ();
	
		afx_msg void OnNextChannel ();
	
		afx_msg void OnPreviousChannel ();
	
		afx_msg void OnSelendokXAxisDropList ();
	
		afx_msg void OnSize (
				UINT 									nType,
				int 									cx,
				int 									cy);
	
		afx_msg void OnUpdateEditCopy (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFileSaveAs (
				CCmdUI* 								pCmdUI);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()

		CDataExchange						*m_dialogFromPtr,
												*m_dialogToPtr;
	
		CMenu									*m_binWidthMenu,
												*m_graphOverlayMenu,
												*m_selectVectorMenu;
	
		WindowPtr 							m_window;

		Handle 								m_graphRecordHandle;
	
		UInt16 								m_graphicsWindowNumber;
	
		static CFont 						s_font;

			// This structure contains the rectangle to be updated in the current window.
		static RECT 						s_updateRect;

		static Handle 						s_selectionIOInfoHandle;
	
		static UInt16 						s_graphicsWindowCount,
			 									s_numberOfGWindows;

			// Form Data
	public:
		void DoFilePrint (void);

		void DoFilePrintPreview (void);

		void DrawGraphGrowIcon	(
				GraphPtr								graphRecordPtr);

		Boolean FinishGraphRecordSetUp (
				SInt16*								channelListPtr,
				SInt32								channelListLength,
				SInt32								vectorLength,
				SInt32								numberYVectors,
				SInt32								numberSets,
				UInt16								xValueType,
				UInt16								yValueType);
	
		CMenu* GetBinWidthPopupMenu ();
	
		CMGraphDoc* GetDocument ();
	
		CMenu* GetGraphOverlayPopupMenu ();
	
		Handle GetGraphRecordHandle (void);
	
		UInt16 GetGraphWindowNumber (void);

		Handle GetSelectionIOHandle (void);
	
		CMenu* GetSelectVectorPopupMenu ();

		WindowPtr GetWindowPtr (void);
	
		CMenu* GetXAxisPopupMenu ();
	
		virtual void OnPrint (
				CDC* 									pDC,
				CPrintInfo* 						pInfo);
	
		void SetCheckIOMemoryFlag (
				Boolean								flagSetting);

		void SetWindowPtr (
				WindowPtr							window);

		Boolean UpdateGraphControls (void);
		
	
		//{{AFX_DATA (CMGraphView)
		enum { IDD = IDD_GraphWindow };
	
		CMGraphViewButton					m_binWidth,
												m_nextChannel,
												m_overlayControl,
												m_previousChannel,
												m_selectVectors,
												m_xAxisCombo;
	
		int									m_xAxisSelection;
		//}}AFX_DATA
	
		static CBrush						s_grayBrush;
	
		static CPen 						s_bluePen,
			 									s_grayPen,
			 									s_greenPen,
			 									s_ltGrayPen,
			 									s_redPen,
			 									s_whitePen;
	
		double								m_printerTextScaling;

		SInt16								m_xPixelsPerInch,
												m_yPixelsPerInch;

		Boolean								m_closeGraphSelectedFlag,

				// Flag indicating whether the current draw mode is for printing
				// or copying.
												m_printCopyModeFlag;
	
};	// end class CMGraphView

inline CMenu* CMGraphView::GetBinWidthPopupMenu ()
		{ return m_binWidthMenu; }

inline CMenu* CMGraphView::GetGraphOverlayPopupMenu ()
		{ return m_graphOverlayMenu; }

inline Handle CMGraphView::GetGraphRecordHandle ()
		{ return (Handle)m_graphRecordHandle; }  
			
inline UInt16 CMGraphView::GetGraphWindowNumber ()
		{ return (UInt16)m_graphicsWindowNumber; }
			
inline Handle CMGraphView::GetSelectionIOHandle ()
		{ return (Handle)s_selectionIOInfoHandle; }

inline CMenu* CMGraphView::GetSelectVectorPopupMenu ()
		{ return m_selectVectorMenu; }
			
inline WindowPtr CMGraphView::GetWindowPtr ()
		{ return (WindowPtr)m_window; }
			
inline void CMGraphView::SetWindowPtr (WindowPtr	window)
		{ m_window = window; }
