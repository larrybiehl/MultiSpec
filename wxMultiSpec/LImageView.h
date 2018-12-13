//		Revised 04/24/2018 by Larry L. Biehl


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

class CMImageView : public wxView {
public:
	wxDocChildFrame *frame;
    //MyTextWindow *textsw;
  
// Following is copied from shared directive portion of previous Image View
public:
    CMImageView();
    virtual 	~CMImageView();
    CMImageDoc* GetDocument(void);// Its already declared in wxview

    Boolean CheckIfOffscreenImageExists(void);

    void DisposeImageWindowSupportMemory(void);

    Boolean GetControlKeyFlag(void);

    UInt16 GetDisplayPixelSize(void);

    Handle GetDisplaySpecsHandle(void);

    CMImageWindow* GetImageWindowCPtr(void);

    CMImageFrame* GetImageFrameCPtr(void);

    CMLegendView* GetImageLegendViewCPtr(void);

    CMLegendList* GetImageLegendListCPtr(void);

    Boolean GetShiftKeyFlag(void);

    void InvalidateRect(Rect* rect, bool erase);
    void InvalidateRect(wxRect* rectp, bool erase);
    void ClientToDoc(wxPoint& 	point);
    Boolean	GetActiveWindowFlag (void);
    virtual void OnActivateView(bool bActivate, wxView* pActivateView, wxView* pDeactiveView);
    void	UpdateSelectionCoordinates(void);
    void	UpdateCursorCoordinates (void);
    void        UpdateCursorCoordinates(LongPoint* mousePtPtr);
    SInt16	GetClassGroupCode(void);
    SInt16	GetImageType(void);
    SInt16	GetNumberGroups(void);
    SInt16	GetTitleHeight (void);
    SInt16 	GetWindowType(void);

    
    Boolean	DoDeleteKey ();
    void DrawLegend();

    SInt16 GetLegendFullHeight(void);

    SInt16 GetLegendWidth(void);
	 
	 void	SetActiveWindowFlag (
					Boolean				setting);

    void SetControlKeyFlag(Boolean flag);

    void SetLegendBitMapInfoHeaderHandle(Handle bitMapInfoHeaderHandle);

    void SetImageWindowCPtr(CMImageWindow *imageWindowCPtr);

    void SetShiftKeyFlag(Boolean flag);

    void UpdateScrolls(double newMagnification);
    void ZoomIn(void);
    void ZoomOut(void);
    void ZoomSize(void);
    double m_printerTextScaling;
    // Linux specific functions
    bool OnCreate(wxDocument* doc, long flags);
    wxBitmap GetScaledBitmap();
    //wxBitmap* GetScaledBitmapPtr();
    const wxSize & GetViewOffset();
    void SetViewOffset(const wxSize & value);
    virtual void OnDraw(wxDC* dc);
    virtual void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    virtual bool OnClose(bool deleteWindow = true);
    void CreateScaledBitmap();
    void ClearView(bool);
    void	ScrollChanged(void);
	 void OnFocus(wxFocusEvent& event);
         
    // Pointer to CDC class
    CDC* m_pDC;
    // Zooming factor
    double m_Scale;
	
    		// Bitmap object to display
    wxBitmap 	m_ScaledBitmap;

    bool m_hasWaveLength;
    
    wxSize m_ViewOffset;    
    SInt16 m_startchannel_sbs; // save the channel number for side by side display
    CMImageCanvas* m_Canvas;
    CMImageFrame* m_frame;
    CMainFrame* m_mainFrame;

    // This structure contains the rectangle to be updated in the current window.

    static RECT s_updateRect;
    
    // Pointer to the display class for multispectral image.

    CMDisplay* m_displayMultiCPtr;

    // Pointer to the histogram class for the image.

    CMHistogram* m_histogramCPtr;

    // Flag indicating whether the current draw mode is for printing
    // or copying.

    Boolean m_printCopyModeFlag;

    // Flag indicating whether the class members have been
    // initialized successfully.

    Boolean m_initializedFlag;

    SInt16 m_xPixelsPerInch;
    SInt16 m_yPixelsPerInch;    
    //void  OnChar(wxKeyEvent& event);
    //void OnKeyDown(wxKeyEvent& event);
	 void OnZoomInTool(wxCommandEvent& event);

protected:
    SInt32	NormalizeScrollOffset(double	magnification,
                                     SInt32	scrollIncrement,
                                     SInt32	scrollOffset);

    void	UpdateScrollRange(double	magnification);

    void	UpdateOffscreenMapOrigin(void);

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
