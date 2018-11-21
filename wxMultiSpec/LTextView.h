//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2018)
//						(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LTextView.h
//	Implementation:		LTextView.cpp
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

#ifndef __VIEWTEXTH__
#define __VIEWTEXTH__

#include "wx/docview.h"
#include "wx/stc/stc.h"
#include "wx/textctrl.h"
#include "wx/frame.h"
#include "SMultiSpec.h"
class TextWindow: public wxTextCtrl
{
public:
	wxView *m_view;
    
	TextWindow(wxView *v, wxDocChildFrame *frame, const wxPoint& pos, const wxSize& size, long style);
};

class TextWindow2: public wxStyledTextCtrl
{
public:
	wxView *m_view;
    
	TextWindow2(wxView *v, wxDocChildFrame *frame, const wxPoint& pos, const wxSize& size, long style);
};

class CMTextView: public wxView
{
public:
	TextWindow2 *m_textsw;
 
	wxDocChildFrame *m_frame;
		
	CMTextView(): wxView() {m_frame = (wxDocChildFrame *) NULL; m_textsw = (TextWindow2 *) NULL; }
	~CMTextView() {m_textsw = NULL; m_frame = NULL; gOutputViewCPtr = NULL; gActiveWindowType = 0; gTheActiveWindow = NULL;}

	bool ListString (
				HPtr									textPtr,
				UInt32								textLength ,
				SInt16								charFormatCode);
				
	bool OnCreate(wxDocument *doc, long flags);
	void OnDraw(wxDC *dc);
	void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
	bool OnClose(bool deleteWindow = true);
	void OnTextChange(wxCommandEvent& event);
//        void OnTextChange();
//        void UpdateCaretPosition(wxStyledTextEvent& event);
//        void MoveBackCaretPosition(wxStyledTextEvent& event);
//        void SetVariableSkip(wxKeyEvent& event);
	virtual void OnActivateView (bool bActivate, wxView* pActivateView, wxView* pDeactiveView);
	SInt16 GetWindowType(void);
    
protected:
	void        InitialUpdate(void);
	bool			DoEnumerateFamilies(wxFont textwf, bool fixedWidthOnly, wxFontEncoding encoding);

private:
  DECLARE_DYNAMIC_CLASS(CMTextView)
//  DECLARE_EVENT_TABLE()
};
#endif
