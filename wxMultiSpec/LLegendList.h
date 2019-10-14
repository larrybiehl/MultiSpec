//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LLegendList.h
//	Implementation:		LLegendList.cpp
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
//	Revised By:				Larry L. Biehl			Date: 09/30/2019
//
//------------------------------------------------------------------------------------
//
#if !defined __LLEGLIST_H__
	#define __LLEGLIST_H__

#include "SMultiSpec.h"

#include "wx/wx.h"
#include "wx/listctrl.h"
#include "wx/imaglist.h"

#define LEGEND_LIST 1040

typedef wxPoint CPoint;

class CMLegendList : public wxListView
	{
	DECLARE_DYNAMIC_CLASS (CMLegendList)
	
	public:
		CMLegendList ();
		CMLegendList (
				wxWindow*		parent,
				wxWindowID 		id = LEGEND_LIST);
		
		virtual ~CMLegendList();
		
		//CPoint				LastClickPoint (void);
		
		void					DrawLegendList ();

		Handle				GetBitMapInfoHeaderHandle();
	
		void					SetBitMapInfoHeaderHandle(
									Handle				bitMapInfoHeaderHandle);
	
		void					SetLegendListActiveFlag (
									Boolean				settingFlag);
	
		void              DrawItem (int itemData, int itemID);
	
		wxPalette* 					m_paletteObject;
		wxPalette* 					m_backgroundPaletteObject;
		Handle						m_imageFileInfoHandle;
		SInt32						m_listType;
		SInt32						m_classPaletteEntries;
		SInt16						m_paletteOffset;
		wxImageList             *m_ilist;
		bool                    listready;
	
	protected:
		void DrawListItems ();
		
				// Generated message map functions
	
		void OnBeginDrag (wxListEvent& event);
		//void OnCharHook (wxKeyEvent& event);
		void OnKeyDown (wxKeyEvent& event);
		void OnKeyUp (wxKeyEvent& event);
		//void OnKillFocusEvent (wxFocusEvent& event);
		void OnLButtonDblClk (wxListEvent& event);
		void OnLButtonDown (wxMouseEvent& event);
		void OnLButtonUp (wxMouseEvent& event);
		void OnMouseMove (wxMouseEvent& event);

		DECLARE_EVENT_TABLE()
	
		Handle						m_bitMapInfoHeaderHandle;
		Boolean						m_activeFlag;
		
		static CPoint				s_lastMouseDnPoint;
		
		static RECT					s_dragRect;
		
		static Handle				s_legendBitMapInfo;
		
		static int					s_lastVerticalPoint,
										s_listBottom;
		
		static bool					s_draggingFlag,
										s_grayRectDisplayedFlag,
										s_isPrintingFlag;
		
		//wxPanel*        		m_LegendView;
		CMLegendView*        	m_LegendView;
		wxRect 						m_rectFocus;
		
	};
#endif	// !defined __LLEGLIST_H__  
