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
//	File:						xImageCanvas.h
//	Implementation:		xImageCanvas.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMImageCanvas class
//
//	Written By:				Abdur Rahman Maud		Date: 06/22/2011
//	Revised By:				Larry L. Biehl			Date: 10/25/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"
#include "xImageView.h"
#include "wx/wx.h"

#define LImageCanvasName wxT("ImageCanvas")


class CMImageCanvas : public wxScrolledWindow
{
	DECLARE_DYNAMIC_CLASS (CMImageCanvas)
    
	public:
		static wxSize								s_scrollingIncrement;
	
		CMImageCanvas ();

		CMImageCanvas (
				wxWindow* 							parent,
				wxWindowID 							id = wxID_ANY,
				const wxPoint& 					pos = wxDefaultPosition,
				const wxSize& 						size = wxDefaultSize,
				long 									style = wxSUNKEN_BORDER,
				const wxString& 					name = LImageCanvasName);
	
		~CMImageCanvas ();
	
		void AdjustScrollBars (
				bool 									callScrollFlag);
	
		bool AutoScroll (
				wxPoint 								currentPos,
				wxPoint 								scrollPos);

		void CanvasScroll (
				int 									x,
				int 									y);
	
		wxPoint ClientToImage (
				const wxPoint& 					pos);

		bool Create (
				wxWindow* 							parent,
				wxWindowID 							id = wxID_ANY,
				const wxPoint& 					pos = wxDefaultPosition,
				const wxSize& 						size = wxDefaultSize,
				long 									style = wxSUNKEN_BORDER,
				const wxString& 					name = LImageCanvasName);

		void DoMouseWheel (
				wxMouseWheelAxis 					axis,
				int 									rotation,
				bool 									ctrlDown);

		void EraseBackground (
				bool									flag);

		void FixViewOffset ();

		wxSize GetCurrentSize ();

		wxRect GetImageDisplayRect (
				const wxPoint& 					scrollPos);
	
		wxPoint GetLastSelectionPoint ();
	
		wxPoint GetScrollPosition ();

		const wxRect& GetSelection ();

		CMImageView* GetView ();

		void Init ();
	
		void SetView (
				CMImageView* 						value);
	
	
		wxRect 								m_Selection;
	
		CMImageView* 						m_View;
	
		wxPoint 								m_BL,
			 									m_LastSelectionPoint,
												m_TR,
		 										unscrolledpt;
	
		int 									m_size_h,
												m_size_w;
	
		bool 									m_dataListShowFlag,
												m_displayImageFlag,
												m_featureListShowFlag,
												m_scrolledFlag;

	private:
		void OnCharHook (
				wxKeyEvent& 						event);
	
		void OnContextMenu (
				wxContextMenuEvent& 				event);
	
		void OnCursorChange (
				wxSetCursorEvent& 				event);
	
		void OnDlbClk (
				wxMouseEvent& 						event);
	
		void OnEraseBackground (
				wxEraseEvent& 						event);
	
		void OnIdle (
				wxIdleEvent& 						event);
	
		void OnKeyDown (
				wxKeyEvent& 						event);
	
		void OnKeyUp (
				wxKeyEvent& 						event);
	
		void OnLeaveImageWindow (
				wxMouseEvent& 						event);
	
		void OnLeftDown (
				wxMouseEvent& 						event);
	
		void OnLeftUp (
				wxMouseEvent& 						event);
	
		void OnMouseMove (
				wxMouseEvent& 						event);
	
		void OnMouseWheel (
				wxMouseEvent& 						event);
	
		void OnPaint (
				wxPaintEvent& 						event);
	
		void OnScrollChanged (
				wxScrollWinEvent& 				event);
		DECLARE_EVENT_TABLE ()
			
};	// end "class CMImageCanvas"

