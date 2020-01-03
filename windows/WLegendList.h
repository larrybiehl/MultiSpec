//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
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
//	Revised By:				Larry L. Biehl			Date: 03/02/2017
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
	
		afx_msg BOOL OnSetCursor (
				CWnd* 								pWnd,
				UINT 									nHitTest,
				UINT 									message);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		DRAWITEMSTRUCT						m_drawItem;
	
		Handle								m_bitMapInfoHeaderHandle;
	
		Boolean								m_activeFlag,
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
