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
//	File:						WLegendList.h
//	Implementation:		WLegendList.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMLegendList class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 04/17/2020
//
//------------------------------------------------------------------------------------

#pragma once

#ifndef __AFXEXT_H__
	#include <afxext.h>
#endif   

typedef struct LegendBitMapInfo
	{
	BITMAPINFOHEADER 	bmiHeader;
	UInt16				bmiColors[1];
	UInt8					paletteIndex;
	
	}	LegendBitMapInfo;            
   

class CMLegendList : public CListBox
{          
	public:
		CMLegendList ();	// standard constructor
	
		virtual ~CMLegendList ();
	
		void CheckShiftKeyDown (void);
	
		void CheckShiftKeyUp (void);
	
		void DrawLegendList ();

		Handle GetBitMapInfoHeaderHandle ();
	
		void GetDrawItemStruct (
				LPDRAWITEMSTRUCT 					lpDrawItemStruct);
	
		void GetTextValue (
				SInt16								selectedCell,
				UInt16*								valuePtr);
	
		CPoint LastClickPoint (void);
	
		void SetBitMapInfoHeaderHandle (
				Handle								bitMapInfoHeaderHandle);
	
		void SetLegendListActiveFlag (
				Boolean								settingFlag);

		// Implementation
		virtual void MeasureItem (
				LPMEASUREITEMSTRUCT 				lpMeasureItemStruct);

		virtual void DrawItem (
				LPDRAWITEMSTRUCT 					lpDrawItemStruct);
	
		CPalette 							*m_paletteObject,
			 									*m_backgroundPaletteObject;
	
		Handle								m_imageFileInfoHandle;
	
		SInt32								m_classPaletteEntries,
												m_listType;
	
		SInt16								m_paletteOffset;
	
	protected:
		// Generated message map functions
		//{{AFX_MSG (CMLegendList)
		//afx_msg void OnContextMenu (
		//		CWnd*									pWnd,
		//		CPoint 								point);

		afx_msg void OnDrawItem (
				int 									nIDCtl,
				LPDRAWITEMSTRUCT 					lpDrawItemStruct);
	
		afx_msg void OnKeyDown (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnKeyUp (
				UINT 									nChar,
				UINT 									nRepCnt,
				UINT 									nFlags);
	
		afx_msg void OnLButtonDblClk (
				UINT 									nFlags,
				CPoint 								point);
	
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
	
		afx_msg BOOL OnSetCursor (
				CWnd* 								pWnd,
				UINT 									nHitTest,
				UINT 									message);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()

		//BOOL PreTranslateMessage (
		//		MSG*									pMsg);
	
		DRAWITEMSTRUCT						m_drawItem;
	
		Handle								m_bitMapInfoHeaderHandle;
	
		Boolean								m_activeFlag,
												m_controlKeyDownFlag,
												m_shiftKeyDownFlag;
	
		static CPoint						s_lastMouseDnPoint;
	
		static RECT							s_dragRect;
	
		static Handle						s_legendBitMapInfo;
	
		static int							s_lastVerticalPoint,
												s_listBottom;
	
		static Boolean						s_draggingFlag,
												s_grayRectDisplayedFlag,
												s_isPrintingFlag;
							
};	// end class CMLegendList
