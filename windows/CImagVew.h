//		Revised 03/14/2015 by Larry L. Biehl


#ifndef __VIEWIMAGEH__
#define __VIEWIMAGEH__

#include "SMulSpec.h"

#if defined multispec_lin
#include "wx/wx.h"
#include "wx/docview.h"
#include "wx/dc.h"
#include "LImageCanvas.h"
#include "CHistgrm.h"
#include "LCoorBar.h"

class CMImageCanvas;

class CMImageView : public wxView {
public:
    wxMDIChildFrame *frame;
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

    void UpdateScrolls(double newMagnification);
    void ZoomIn(void);
    void ZoomOut(void);
    void ZoomSize(void);
    double m_printerTextScaling;
    // Linux specific functions
    bool OnCreate(wxDocument* doc, long flags);
    wxBitmap & GetScaledBitmap();
    const wxSize & GetViewOffset();
    void SetViewOffset(const wxSize & value);
    virtual void OnDraw(wxDC* dc);
    virtual void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    virtual bool OnClose(bool deleteWindow = true);
    void CreateScaledBitmap();
    void ClearView(bool);
   
    // Pointer to CDC class
    CDC* m_pDC;
    // Zooming factor
    double m_Scale;
    // Bitmap object to display
    wxBitmap m_ScaledBitmap;

    wxSize m_ViewOffset;
    CMImageCanvas* m_Canvas;
    CMImageFrame* m_frame;
    
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
    void  OnChar(wxKeyEvent& event);
    void OnKeyDown(wxKeyEvent& event);

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

    // Flag indicating whether the control key is down indicating that
    // zooming should occur at .1 increments.

    Boolean m_ctlKeyDownFlag;

    // Flag indicating whether the control key is down indicating that
    // zooming should occur at .1 increments.

    Boolean m_thumbScrollFlag;

    // Pointer to the image window class for the image view.

    CMImageWindow* m_imageWindowCPtr;

    // Saved cursor line and column value

    SInt32 m_cursorColumnValue;

    SInt32 m_cursorLineValue;

    static SInt16 s_currentCursor;

    // This structure contains the rectangle to be updated in the current window.

    static RECT s_updateRect;

private:
DECLARE_DYNAMIC_CLASS(CMImageView)

    DECLARE_EVENT_TABLE()
};




//#ifndef _DEBUG  // debug version in mimagevw.cpp

inline CMImageDoc* CMImageView::GetDocument() {
    return (CMImageDoc*) wxView::GetDocument();
}

inline CMImageWindow* CMImageView::GetImageWindowCPtr() {
    if (this == NULL)
        return (NULL);

    else
        return (CMImageWindow*) m_imageWindowCPtr;
}

//#endif


#else // !defined multispec_lin




// MImagVew.h : definition for CMImageView class
//
/////////////////////////////////////////////////////////////////////////////
   
	 
	#if defined multispec_mac
		#include <LView.h> 
												
		const ResIDT	Wind_ColorImage 	= 1300;
		const ResIDT	pane_ImagePane	 	= 1302;
		const ResIDT	RidL_ImageButtons = 1300;
		
		const	MessageT	msg_DefaultZoom	= 13031;
		const	MessageT	msg_ZoomIn			= 13041;
		const	MessageT	msg_ZoomOut			= 13051;
	#endif	// defined multispec_mac 
	
	#if defined multispec_win  

//		#include <afxole.h>
		#include "WDrawObj.h"
		#include "WTLegend.h"

	#endif	// defined multispec_win 
	
#if defined multispec_win
	class CMImageView : public CScrollView
		{                    
		DECLARE_DYNCREATE(CMImageView)
		
		protected: // create from serialization only
									CMImageView();

		void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hMenu);
	              
		public:           
			virtual 				~CMImageView();
                                             		
			void 					ClientToDoc(
										CPoint& 					point);
										
			Boolean				DoDeleteKey (
										UINT 						nChar);
										
			void					DoEditCopy(void); 
										
			void					DoFilePrint(); 
										
			void					DoFilePrintPreview();
										
			Boolean				GetActiveWindowFlag (void);
			
			SInt16				GetClassGroupCode(void);
		
			CMImageDoc* 		GetDocument(void);
			
			SInt16 				GetImageType(void);
			
			SInt16				GetNumberGroups(void); 
			
			CMOutlineArea*		GetSelectionAreaCPtr(void);
			
			SInt16				GetTitleHeight (void);
			
			SInt16 				GetWindowType(void);
			
//			void 				Magnification(void); 

			void				DrawDC(CDC* 					pDC); 

			
			virtual void 		OnDraw(
										CDC* 					pDC);  // overridden to draw this view
			
			virtual void 		OnPrint(
										CDC* 					pDC, 
										CPrintInfo* 		pInfo);
										
			virtual void		OnUpdate(
										CView*				pSender,
										LPARAM				lHint = 0L,
										CObject*				pHint = NULL); 
									
			void 					Remove(
										CMDrawObject* 	pObject);
										
			void					SetControlKey (
										Boolean				settingFlag);
										
			void					UpdateSelectionCoordinates(void);
											
			void					UpdateCursorCoordinates(
										LongPoint*					mousePoint);
										
			void					UpdateCursorCoordinates (void);           
			
			void					ZoomIn(void); 
			                   
			void					ZoomOut(void);

			void					ScrollV(UINT nSBCode);
							
			void					ScrollH(UINT nSBCode);

			#ifdef _DEBUG
				virtual void 		AssertValid() const;
				virtual void 		Dump(CDumpContext& dc) const;
			#endif
			
		protected: 
								
			SInt32				NormalizeScrollOffset(
										double				magnification,
										SInt32				scrollIncrement,
										SInt32				scrollOffset); 
			
			virtual void 		OnActivateView(
										BOOL 					bActivate, 
										CView* 				pActivateView,
										CView* 				pDeactiveView); 
			
			virtual void 		OnInitialUpdate();	// first time after construct  
										
			BOOL					OnScrollBy(
										CSize 				sizeScroll, 
										BOOL 					bDoScroll);
			
			void					SetScrollSizes(
										int					nMapMode, 
										SIZE					sizeTotal,
										const SIZE&			sizePage, 
										const SIZE&			sizeLine);  
										
			void					UpdateOffscreenMapOrigin(void);
			
			void					UpdateScrollRange(
										double				magnification);
										                                         
				// Printing support
						
			virtual BOOL 		OnPreparePrinting(CPrintInfo* pInfo);
//			virtual void 		OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
//			virtual void 		OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
		
		// Generated message map functions
		
			//{{AFX_MSG(CMImageView)
			afx_msg LRESULT 	OnDoRealize(WPARAM wParam, LPARAM lParam);  // user message
			afx_msg void 		OnEditCut();
			afx_msg void 		OnUpdateEditCut(CCmdUI* pCmdUI);
//			afx_msg void 		OnEditCopy();
			afx_msg void 		OnUpdateEditPaste(CCmdUI* pCmdUI);
			afx_msg void 		OnEditPaste();
			afx_msg void 		OnUpdateFilePrintSetup(CCmdUI* pCmdUI);
			afx_msg void 		OnUpdateEditClear(CCmdUI* pCmdUI);
			afx_msg void 		OnUpdateEditUndo(CCmdUI* pCmdUI); 
			afx_msg void 		OnSetFocus(CWnd* pOldWnd);
			afx_msg void 		OnMouseMove(UINT nFlags, CPoint point);
			afx_msg void 		OnNcMouseMove(UINT nFlags, CPoint point);
			afx_msg void 		OnLButtonDown(UINT nFlags, CPoint point);
			afx_msg void 		OnLButtonUp(UINT nFlags, CPoint point);
			afx_msg void		OnLButtonDblClk(UINT nFlags, CPoint point);
			afx_msg void 		OnSize(UINT nType, int cx, int cy);
			afx_msg BOOL 		OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
			afx_msg void 		OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void 		OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void 		OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void 		OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void 		OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
			afx_msg void 		OnPaint();
			afx_msg void 		OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
			DECLARE_MESSAGE_MAP() 

#endif	// defined multispec_win 
  
	public: 
		Boolean				CheckIfOffscreenImageExists(void);
											
		void					DisposeImageWindowSupportMemory(void);
                                                              		
		Boolean				GetControlKeyFlag(void); 
		
		UInt16				GetDisplayPixelSize (void);
		
		Handle				GetDisplaySpecsHandle (void);
		
		CMImageWindow* 	GetImageWindowCPtr(void); 

		CMImageFrame* 		GetImageFrameCPtr (void);

		CMLegendView* 		GetImageLegendViewCPtr (void);

		CMLegendList*		GetImageLegendListCPtr (void); 

		void					DrawLegend();

		SInt16				GetLegendFullHeight (void); 

		SInt16				GetLegendWidth (void);
                                                              		
		void					SetControlKeyFlag(
									Boolean					flag); 

		void					SetLegendBitMapInfoHeaderHandle(
									Handle					bitMapInfoHeaderHandle);

		void					SetImageWindowCPtr (
									CMImageWindow					*imageWindowCPtr);
										 
		void					UpdateScrolls(
									double				newMagnification);
									
		double					m_printerTextScaling; 
		
				// Pointer to CDC class 
                                                                      
		CDC*						m_pDC;					
		
				// Pointer to the display class for multispectral image.
				
		CMDisplay*				m_displayMultiCPtr;
			
				// Pointer to the histogram class for the image.
				
		CMHistogram*			m_histogramCPtr; 
		
				// Flag indicating whether the current draw mode is for printing
				// or copying.	
		
		Boolean					m_printCopyModeFlag;
		
				// Flag indicating whether the class members have been
				// initialized successfully.
				
		Boolean					m_initializedFlag;
		
		SInt16					m_xPixelsPerInch;
		SInt16					m_yPixelsPerInch; 
		
protected: 
	
			 // Flag indicated whether this view is active
			 							
	Boolean	 			m_activeFlag;  
	
			 // Flag indicated whether already within 'OnSize' routine.
			 							
	Boolean	 			m_withinOnSizeFlag;  
	
			 // Flag indicating whether UpdateScrollRange should be called.
			 							
	Boolean	 			m_callUpdateScrollRangeFlag;   
	
			 // Flag indicating whether the control key is down indicating that
			 // zooming should occur at .1 increments.
	
	Boolean	 			m_ctlKeyDownFlag;  
	
			 // Flag indicating whether the control key is down indicating that
			 // zooming should occur at .1 increments.
	
	Boolean	 			m_thumbScrollFlag;
	
			// Pointer to the image window class for the image view.
	
	CMImageWindow*		m_imageWindowCPtr;
	
									// Saved cursor line and column value                   
	
	SInt32				m_cursorColumnValue;
	
	SInt32				m_cursorLineValue;
	
	static SInt16		s_currentCursor;
	
			// This structure contains the rectangle to be updated in the current window.
	
	static RECT			s_updateRect;
	
};

#if defined multispec_win

	#ifndef _DEBUG  // debug version in mimagevw.cpp 
	
		inline CMImageDoc* CMImageView::GetDocument()
		   { return (CMImageDoc*)m_pDocument; }
		      
		inline CMImageWindow* CMImageView::GetImageWindowCPtr()
		   { if (this == NULL)
		   		return (NULL);
		   		
		   	else
		   		return (CMImageWindow*)m_imageWindowCPtr; }
	   
	#endif    
	 
#endif	// defined multispec_win                                
          
#endif // !defined multispec_lin
          
#endif // !defined __MIMAGVEW_H__
