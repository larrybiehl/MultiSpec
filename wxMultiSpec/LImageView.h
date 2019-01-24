//		Revised 01/03/2019 by Larry L. Biehl


#ifndef __VIEWIMAGEH__
#define __VIEWIMAGEH__

#include "SMultiSpec.h"

#include "CHistogram.h"

#include "LCoordinateBar.h"
#include "LImageCanvas.h"
#include "LMainFrame.h"

#include "wx/wx.h"
#include "wx/docview.h"
#include "wx/dc.h"

class CMImageCanvas;

class CMImageView : public wxView
	{
	public:
		wxDocChildFrame *frame;
	
	public:
		CMImageView();
		virtual 			~CMImageView();

		Boolean 			CheckIfOffscreenImageExists (void);

		void 				ClearView (bool);

		void 				ClientToDoc (wxPoint& 	point);

		void 				CreateScaledBitmap ();

		void 				DisposeImageWindowSupportMemory (void);

		void 				DrawLegend();

		Boolean			GetActiveWindowFlag (void);

		SInt16			GetClassGroupCode (void);

		Boolean 			GetControlKeyFlag (void);

		UInt16 			GetDisplayPixelSize(void);

		Handle 			GetDisplaySpecsHandle(void);

		CMImageDoc* 	GetDocument (void);// Its already declared in wxview

		SInt16			GetImageType (void);

		CMImageWindow* GetImageWindowCPtr (void);

		CMImageFrame* 	GetImageFrameCPtr (void);

		CMLegendList* 	GetImageLegendListCPtr (void);

		CMLegendView* 	GetImageLegendViewCPtr (void);

		SInt16 			GetLegendFullHeight (void);

		SInt16 			GetLegendWidth (void);

		SInt16			GetNumberGroups (void);

		wxBitmap 		GetScaledBitmap ();

		Boolean 			GetShiftKeyFlag (void);

		SInt16			GetTitleHeight (void);

		const wxSize& 	GetViewOffset (void);

		SInt16 			GetWindowType (void);

		void 				InvalidateRect (Rect* rect, bool erase);

		void 				InvalidateRect (wxRect* rectp, bool erase);

		virtual void 	OnActivateView (bool bActivate, wxView* pActivateView, wxView* pDeactiveView);

		virtual bool 	OnClose (bool deleteWindow = true);

		bool 				OnCreate(wxDocument* doc, long flags);

		virtual void 	OnDraw (wxDC* dc);

		void 				OnFocus (wxFocusEvent& event);

		virtual void 	OnUpdate (wxView *sender, wxObject *hint = (wxObject*)NULL);

		void 				OnZoomInTool(wxCommandEvent& event);

		void				UpdateCursorCoordinates (void);

		void        	UpdateCursorCoordinates (LongPoint* mousePtPtr);

		void				UpdateSelectionCoordinates (void);

		void				ScrollChanged (void);

		void				SetActiveWindowFlag (
								Boolean				setting);

		void 				SetControlKeyFlag(Boolean flag);

		void 				SetImageWindowCPtr(CMImageWindow *imageWindowCPtr);

		void 				SetLegendBitMapInfoHeaderHandle(Handle bitMapInfoHeaderHandle);

		void 				SetShiftKeyFlag(Boolean flag);

		void 				SetViewOffset (const wxSize & value);

		void				UpdateOffscreenMapOrigin (void);

		void 				UpdateScrolls(double newMagnification);

		void 				ZoomIn (void);

		void 				ZoomSize (void);

		void 				ZoomOut (void);

		double 				m_printerTextScaling;
	
				// Pointer to CDC class
		CDC* 					m_pDC;
	
				// Zooming factor
		double 				m_Scale;

				// Bitmap object to display
		wxBitmap 			m_ScaledBitmap;

		bool 					m_hasWaveLength;

		wxSize 				m_ViewOffset;
	
		SInt16 				m_startchannel_sbs; // save the channel number for side by side display
	
		CMImageCanvas* 	m_Canvas;
	
		CMImageFrame* 		m_frame;
	
		CMainFrame* 		m_mainFrame;

				// This structure contains the rectangle to be updated in the current window.

		static RECT 		s_updateRect;

				// Pointer to the display class for multispectral image.

		CMDisplay* 			m_displayMultiCPtr;

				// Pointer to the histogram class for the image.

		CMHistogram* 		m_histogramCPtr;

				// Flag indicating whether the current draw mode is for printing
				// or copying.

		Boolean 				m_printCopyModeFlag;

				// Flag indicating whether the class members have been
				// initialized successfully.

		Boolean 				m_initializedFlag;

		SInt16 				m_xPixelsPerInch;
		SInt16 				m_yPixelsPerInch;

	protected:
		SInt32	NormalizeScrollOffset (double	magnification,
                                     SInt32	scrollIncrement,
                                     SInt32	scrollOffset);

    void	UpdateScrollRange (double	magnification);

    void        InitialUpdate(void);
    // Flag indicated whether this view is active

    Boolean m_activeFlag;

    // Flag indicated whether already within 'OnSize' routine.

    Boolean m_withinOnSizeFlag;

    // Flag indicating whether UpdateScrollRange should be called.

    Boolean m_callUpdateScrollRangeFlag;

    // Flag indicating whether the control key is down. The contexts indicates
	 // what it will be used for.
    Boolean m_ctlKeyDownFlag;

    // Flag indicating whether the shift key is down. The context indicates
	 // what it will be used for.
    Boolean m_shiftKeyDownFlag;

    Boolean m_thumbScrollFlag;

    // Pointer to the image window class for the image view.

    CMImageWindow* m_imageWindowCPtr;

    // Saved cursor line and column value

    SInt32 m_cursorColumnValue;

    SInt32 m_cursorLineValue;

    static SInt16 s_currentCursor;

private:
	DECLARE_DYNAMIC_CLASS (CMImageView)
	DECLARE_EVENT_TABLE()
	
};


inline CMImageDoc* CMImageView::GetDocument()
	{
	return (CMImageDoc*) wxView::GetDocument();
	}


inline CMImageWindow* CMImageView::GetImageWindowCPtr() {
	if (this == NULL)
		return (NULL);

	else
		return (CMImageWindow*) m_imageWindowCPtr;
}

#endif // !defined __VIEWIMAGEH__
