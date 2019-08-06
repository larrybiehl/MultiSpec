/* 
 * File:   LImageCanvas.h
 * Author: multispec
 *
 * Created on June 22, 2011, 5:00 PM
 * Revised by Larry Biehl		04/02/2019
 */

#ifndef LIMAGECANVAS_H
#define	LIMAGECANVAS_H
#include "SMultiSpec.h"
#include "LImageView.h"
//#include "LImageFrame.h"
#include "wx/wx.h"


//#define DocViewTestCanvasStyle wxNO_BORDER|wxHSCROLL|wxVSCROLL|wxTAB_TRAVERSAL
#define LImageCanvasName wxT("ImageCanvas")

//class CMImageView;

class CMImageCanvas : public wxScrolledWindow
	{
	DECLARE_DYNAMIC_CLASS(CMImageCanvas)

	void Init ();
	
	wxRect 			m_Selection;
	
	CMImageView* 	m_View;
	
	wxPoint			m_TR;
	wxPoint 			m_BL;
	wxPoint 			unscrolledpt;
	wxPoint 			m_LastSelectionPoint;
	
	int 				m_size_h,
						m_size_w;
	
	static wxSize	ScrollingIncrement;
	
	bool 				m_dataListShowFlag,
		 				m_displayImageFlag,
						m_featureListShowFlag,
		 				m_scrolledFlag;
    
	public:
		CMImageCanvas();

		CMImageCanvas(wxWindow * parent,
				wxWindowID id = wxID_ANY,
				const wxPoint & pos = wxDefaultPosition,
				const wxSize & size = wxDefaultSize,
				long style = wxSUNKEN_BORDER,
				const wxString & name = LImageCanvasName);

		bool Create(wxWindow * parent,
				wxWindowID id = wxID_ANY,
				const wxPoint & pos = wxDefaultPosition,
				const wxSize & size = wxDefaultSize,
				long style = wxSUNKEN_BORDER,
				const wxString & name = LImageCanvasName);

		CMImageView* GetView ();
		void SetView (CMImageView * value);

		const wxRect& GetSelection ();
		wxPoint GetLastSelectionPoint ();

		wxSize GetCurrentSize();
		wxPoint GetScrollPosition();

		//void Scroll(wxPoint pt);
		void CanvasScroll (int x, int y);

		void FixViewOffset();
		void AdjustScrollBars(bool callScrollFlag);

		wxRect GetImageDisplayRect(const wxPoint & scrollPos);
		wxPoint ClientToImage(const wxPoint & pos);

		bool AutoScroll(wxPoint currentPos, wxPoint scrollPos);
		void DoMouseWheel (
					wxMouseWheelAxis 					axis,
					int 									rotation,
					bool 									ctrlDown);

		void EraseBackground(bool);

	private:
		void OnCharHook (wxKeyEvent& event);
		void OnCursorChange (wxSetCursorEvent& event);
		void OnDlbClk (wxMouseEvent& event);
		void OnEraseBackground (wxEraseEvent& event);
		void OnIdle (wxIdleEvent& event);
		void OnLeaveImageWindow (wxMouseEvent& event);
		void OnLeftDown (wxMouseEvent& event);
		void OnLeftUp (wxMouseEvent& event);
		void OnMotion (wxMouseEvent& event);
		void OnMouseWheel (wxMouseEvent& event);
		void OnPaint (wxPaintEvent& event);
		void OnScrollChanged (wxScrollWinEvent& event);
		DECLARE_EVENT_TABLE ()
		
	};

#endif	/* LIMAGECANVAS_H */

