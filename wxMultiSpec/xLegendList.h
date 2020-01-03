//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xLegendList.h
//	Implementation:		xLegendList.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMLegendList class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/20/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"

#include "wx/wx.h"
#include "wx/listctrl.h"
#include "wx/imaglist.h"

#define LEGEND_LIST 1040


class CMLegendList : public wxListView
{
	DECLARE_DYNAMIC_CLASS (CMLegendList)
	
	public:
		CMLegendList ();
	
		CMLegendList (
				wxWindow*							parent,
				wxWindowID 							id = LEGEND_LIST);
		
		virtual ~CMLegendList ();
		
		void DrawLegendList ();

		Handle GetBitMapInfoHeaderHandle ();
	
		void SetBitMapInfoHeaderHandle (
				Handle								bitMapInfoHeaderHandle);
	
		void SetLegendListActiveFlag (
				Boolean								settingFlag);
	
		void DrawItem (
				int 									itemData,
				int 									itemID);
	
	
		wxImageList             		*m_ilist;
	
		wxPalette 							*m_backgroundPaletteObject,
												*m_paletteObject;
		
		Handle								m_imageFileInfoHandle;
	
		SInt32								m_classPaletteEntries,
												m_listType;
	
		SInt16								m_paletteOffset;
	
		bool                    		m_listReadyFlag;
	
	
	protected:
				// Generated message map functions
	
		void OnBeginDrag (
				wxListEvent& 						event);
	
		void OnContextMenu (
				wxContextMenuEvent& 				event);
	
		void OnKeyDown (
				wxKeyEvent& 						event);
	
		void OnKeyUp (
				wxKeyEvent& 						event);
	
		void OnLButtonDblClk (
				wxListEvent& 						event);
	
		void OnLButtonDown (
				wxMouseEvent& 						event);
	
		void OnLButtonUp (
				wxMouseEvent& 						event);
	
		void OnMouseMove (
				wxMouseEvent& 						event);
		DECLARE_EVENT_TABLE ()
	
		void DrawListItems ();
	
		
		static RECT							s_dragRect;
		
		static Handle						s_legendBitMapInfo;
	
		static wxPoint						s_lastMouseDnPoint;
		
		static int							s_lastVerticalPoint,
												s_listBottom;
		
		static bool							s_draggingFlag,
												s_grayRectDisplayedFlag,
												s_isPrintingFlag;
	
		wxRect 								m_rectFocus;
	
		CMLegendView*        			m_LegendView;
		
		Handle								m_bitMapInfoHeaderHandle;
	
		Boolean								m_activeFlag;
		
};	// end "class CMLegendList"  
