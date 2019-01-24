// LTextView.cpp
//
// Revised by Larry Biehl on 01/24/2019
// Revised by Wei-Kang Hsu on 07/25/2016
//
#include "LMultiSpec.h"
#include "LTextFrame.h"
#include "LTextView.h"

#include "wx/fontenum.h"

IMPLEMENT_DYNAMIC_CLASS(CMTextView, wxView)

int s_textWindowXPosition, s_textWindowYPosition, s_clientWidth, s_clientHeight;

//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetWindowType
//
//	Software purpose:	The purpose of this routine is to return the window type
//							variable for this window object.
//
//	Parameters in:		None			
//
//	Parameters out:	None			
//
//	Value Returned:	0 if now window structure exists
//							window type for output text window
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 06/06/2015
//	Revised By:			Larry L. Biehl			Date: 06/06/2015	

SInt16 CMTextView::GetWindowType(void)

{  
	SInt16		windowType = 0;
	
	         
			// Get the window type which will be output window
	                              
	if (this != NULL)
		windowType = kOutputWindowType;
	
	return (windowType);
	
}	// end "GetWindowType"


void CMTextView::InitialUpdate (void)

{   
   m_textsw->SetTabWidth (1);
   m_textsw->SetMarginLeft (-12);
   m_textsw->SetScrollWidthTracking (true);
   m_textsw->SetFoldMarginColour (true, wxColor (0,0,0));
   m_textsw->SetFoldMarginHiColour (true, wxColor (0,0,0));
	//m_textsw->MarginSetStyleOffset (0);
	//m_textsw->SetWrapMode (1);
	//m_textsw->SetUseHorizontalScrollBar (false);

}	// end "InitialUpdate"



bool CMTextView::OnCreate (wxDocument *doc, long WXUNUSED(flags))

{
	int	clientWidth,
			clientHeight,
			width = 650,
			height = 350,
			textWindowXPosition,
			textWindowYPosition;
			
	
	GetMainFrame()->GetClientSize(&clientWidth, &clientHeight);
	width = MIN(width, clientWidth);
	height = MIN(height, clientHeight);


   textWindowXPosition = clientWidth - width - 12;
	
	#ifdef NetBeansProject
		textWindowYPosition = 110;
	#else	// mygeohub
		textWindowYPosition = 80;
	#endif
	
	s_textWindowXPosition = textWindowXPosition;
	s_textWindowYPosition = textWindowYPosition;
	s_clientWidth = clientWidth;
	s_clientHeight = clientHeight;

	//wxSize upperLeftPosition (textWindowXPosition, textWindowYPosition);	// instead of wxDefautlPostition
	
	m_frame = new CMTextFrame (doc,
										this,
										GetMainFrame (),
										wxID_ANY,
										wxT("Text Output"),
										wxPoint (textWindowXPosition, textWindowYPosition),
										wxSize (width, height),
										#if defined multispec_wxmac
											wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT);
										#else
											wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP);
										#endif

	doc->SetFilename (wxT("MultiSpec_Text_Output"), false);
	wxFont font (gFontSize, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL); 
   m_frame->SetFont (font);
	m_frame->GetClientSize (&width, &height);
   m_frame->SetTitle (_T("Text Output"));
   
	m_textsw = new TextWindow2 (this, m_frame, wxPoint(0, 0), wxSize (width, height), wxTE_MULTILINE); //wxHSCROLL|wxTE_READONLY|
	wxFont textwf (gFontSize, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	m_textsw->StyleSetFont (wxSTC_STYLE_DEFAULT, textwf);
   m_textsw->StyleClearAll ();
	
	gOutputViewCPtr = this;
	gTheActiveWindow = (WindowPtr)this;
	#ifdef __X__
			// X seems to require a forced resize
		int x, y;
		m_frame->GetSize (&x, &y);
		m_frame->SetSize (wxDefaultCoord, wxDefaultCoord, x, y);
	#endif

	m_frame->Show (true);
	Activate (true);
	
	InitialUpdate ();

	return true;
	
}	// end "OnCreate"


/*
void CMTextView::UpdateCaretPosition(wxStyledTextEvent& event)
{
//   m_textsw->MoveCaretInsideView();
//   m_textsw->VerticalCentreCaret();
//   OnTextChange();
   printf("Update Caret Position\n");
   if(m_textsw->m_eventSkip != true){
      m_textsw->LineScrollDown();   
      printf("LineScrollDown\n");
   }
   if(enableFlag == true) // make sure m_textsw->m_eventSkip is changed only at the second time in
      m_textsw->m_eventSkip = false;
   
   enableFlag = true;
   
//   event.Skip();
//   event.StopPropagation();
   
}

void CMTextView::SetVariableSkip(wxKeyEvent& event)
{
   m_textsw->m_eventSkip = true;
   enableFlag = false;
//   OnTextChange();
   printf("entering Keyboard\n");
   event.Skip();   
}
*/
//void CMTextView::MoveBackCaretPosition(wxStyledTextEvent& event)
//{
//   m_textsw->LineScrollUp();
//
//}

class MyFontEnumerator : public wxFontEnumerator
{
	public:
		bool GotAny() const
			{ return !m_facenames.IsEmpty(); }
	
		const wxArrayString& GetFacenames() const
				{ return m_facenames; }
	
	protected:
		virtual bool OnFacename(const wxString& facename)
		{
				m_facenames.Add(facename);
				return true;
		}
	
	private:
		wxArrayString m_facenames;

} fontEnumerator;
	
	
bool CMTextView::DoEnumerateFamilies(wxFont textwf, bool fixedWidthOnly,
												wxFontEncoding encoding)
{
	MyFontEnumerator	fontEnumerator;
	int					numberChars;
   
	fontEnumerator.EnumerateFacenames(encoding, fixedWidthOnly);
   
	if (fontEnumerator.GotAny())
		{
		int nFacenames = fontEnumerator.GetFacenames().GetCount();
		numberChars = sprintf ((char*)&gTextString3,
								" LTextView::DoEnumerateFamilies: (count): %d%s", 
								nFacenames,
								gEndOfLine);
		::ListString ((char*)&gTextString3, numberChars, gOutputTextH);
   
		wxString facename;
   
		int n;
		for (n = 0; n < nFacenames; n++)
			{
			facename = fontEnumerator.GetFacenames().Item(n);
			//textwf.SetFaceName(facename);
			//m_textsw->SetFont(textwf);
			numberChars = sprintf ((char*)&gTextString3,
									" LTextView::DoEnumerateFamilies: (fontname): %d, %s%s", 
									n,
									(const char*)facename.mb_str(wxConvUTF8),
									gEndOfLine);
			::ListString ((char*)&gTextString3, numberChars, gOutputTextH);
			}
  
		return true;
		}
	  
	else 
		{
		numberChars = sprintf ((char*)&gTextString3,
								" LTextView::DoEnumerateFamilies: (No fonts found.)%s", 
								gEndOfLine);
		::ListString ((char*)&gTextString3, numberChars, gOutputTextH);
		}
 
	return false;
}



//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListString
//
//	Software purpose:	The purpose of this routine is to list the input
//							information to the output text window.
//
//	Parameters in:		Pointer to 'c' string.
//
//	Parameters out:	None
//
//	Value Returned:	True if operation is okay
//							False if the text limit was reached
//
// Called By:			main
//							DisplayCImage  in displayc.c
//							DisplayPImage  in displayp.c
//
//	Coded By:			Larry L. Biehl			Date: 03/30/1988
//	Revised By:			Larry L. Biehl			Date: 06/19/2017

bool CMTextView::ListString (
				HPtr									textPtr,
				UInt32								textLength,
				SInt16								charFormatCode)

{
			// Make certain that there is a 'c' terminator at the end of the string.

	textPtr[textLength] = 0;
   wxString strtext(textPtr,wxConvUTF8);
	//GetEditCtrl().ReplaceSel(textPtr);
	if (m_textsw != NULL)
		m_textsw->AppendText (strtext);
	//GetEditCtrl().SetModify(TRUE);
	if (m_frame != NULL) // It could be null if the application is being closed.
		m_frame->SetTitle(wxT("Text Output"));
	return (true);

}			// end "ListString"// This function is called when we call wxView::Activate()


// Inherited from wxView
void CMTextView::OnActivateView(
        bool bActivate,
        wxView* pActivateView,
        wxView* pDeactiveView)
{
	if (m_frame != NULL)
		{
		if (bActivate)
			wxASSERT(pActivateView == (wxView*)this);
		  
				// If the window is being activated, outside of a processing operating, make sure the global
				// active image information is up to date.
		if (gProcessorCode == 0 && bActivate) {
			gActiveWindowType = kOutputWindowType;
			gTheActiveWindow = (WindowPtr)gOutputViewCPtr;
			
			}		// end "if (gProcessorCode == 0 && bActivate)"
		
		}		// end "if (m_frame != NULL)"

} // end "OnActivateView"



// Handled by wxTextWindow
void CMTextView::OnDraw(wxDC *WXUNUSED(dc) )
{
}

void CMTextView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint) )
{
}

bool CMTextView::OnClose(bool deleteWindow)
{
	if (!GetDocument()->Close())
		return false;

	Activate(false);

	if (deleteWindow)
		{
		delete m_frame;
		m_frame = NULL;
		return true;
		}
		
	return true;
}

// currently not used 
void CMTextView::OnTextChange(wxCommandEvent& event)
{
	((CMTextDoc *)GetDocument())->Modify(true);

	event.Skip();
}


TextWindow::TextWindow(wxView *v, wxDocChildFrame *frame, const wxPoint& pos, const wxSize& size, long style):
		wxTextCtrl(frame, wxID_ANY, _T(""), pos, size, style)

{
  m_view = v;
}


TextWindow2::TextWindow2(wxView *v, wxDocChildFrame *frame, const wxPoint& pos, const wxSize& size, long style):
		wxStyledTextCtrl(frame, ID_TextWindowStyled, pos, size, style)
{
  m_view = v;  
 }
