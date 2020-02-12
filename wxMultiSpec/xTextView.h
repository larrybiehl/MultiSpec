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
//	File:						xTextView.h
//	Implementation:		xTextView.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			10/19/2018
//
//	Language:				C++
//
//	System:					Linux Operating Systems
//
//	Brief description:	This file is the definition for the TextWindow, TextWindow2
//								and CMTextView classes
//
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"
#include "wx/docview.h"
#include "wx/stc/stc.h"
#include "wx/textctrl.h"
#include "wx/frame.h"


class TextWindow: public wxTextCtrl
{
	public:
		TextWindow (
				wxView*								v,
				wxDocChildFrame*					frame,
				const wxPoint& 					pos,
				const wxSize& 						size,
				long 									style);
	
		wxView*								m_view;
	
};	// end "class TextWindow"


class TextWindow2: public wxStyledTextCtrl
{
	public:
		TextWindow2 (
				wxView*								v,
				wxDocChildFrame*					frame,
				const wxPoint& 					pos,
				const wxSize& 						size,
				long 									style);

		wxView*								m_view;
    
};	// end "class TextWindow2"


class CMTextView: public wxView
{
	DECLARE_DYNAMIC_CLASS (CMTextView)
	
	public:
		CMTextView ();
	
		~CMTextView ();

		bool ListString (
				HPtr									textPtr,
				UInt32								textLength ,
				SInt16								charFormatCode);

		virtual void OnActivateView (
				bool 									bActivate,
				wxView* 								pActivateView,
				wxView* 								pDeactiveView);
	
		bool OnClose (
				bool 									deleteWindow = true);
					
		bool OnCreate (
				wxDocument*							doc,
				long 									flags);
	
		void OnDraw (
				wxDC*									dc);
	
		void OnTextChange (
				wxCommandEvent& 					event);
	
		void OnUpdate (
				wxView*								sender,
				wxObject*							hint = (wxObject*)NULL);
	
		SInt16 GetWindowType (void);
	
	
		TextWindow2*								m_textsw;
	 
		wxDocChildFrame*							m_frame;
	
		 
	protected:
		void InitialUpdate (void);
	
		bool DoEnumerateFamilies (
				wxFont 								textwf,
				bool 									fixedWidthOnly,
				wxFontEncoding 					encoding);

};	// end "class CMTextView"
