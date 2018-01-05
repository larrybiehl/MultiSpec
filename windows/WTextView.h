// WTextView.h : interface of the CMTextView class
//
// Revised by Larry Biehl on 03/27/2017

/////////////////////////////////////////////////////////////////////////////

#if !defined __MTEXTVEW_H__
	#define __MTEXTVEW_H__ 
	
// oul: added the condition of _AMD64_ to be compatible with x64
// _AMD64_ is the predefined macro for x64 machines according to MSDN
//	#if defined _X86_ || defined _AMD64_
	
class CMTextView : public CEditView
{
	DECLARE_DYNCREATE(CMTextView)
	
protected:
	CMTextView();		// protected constructor used by dynamic creation

		// Attributes
public:  
	CMTextDoc* 		GetDocument();

		// Operations
public:        
	SInt32			GetTextBufferLength(void);

	Boolean			ListString(
							HPtr									textPtr, 
							UInt32								textLength,
							SInt16								charFormatCode=kASCIICharString);
	
		// Implementation
protected:
	virtual 				~CMTextView();
//	virtual	void 		OnDraw(CDC* pDC);	// overridden to draw this view
	virtual	void 		OnInitialUpdate();	// first time after construct
	
#ifdef _DEBUG
	virtual void 		AssertValid() const;
	virtual void 		Dump(CDumpContext& dc) const;
#endif

protected:
	
	virtual void 		OnActivateView(
								BOOL 					bActivate, 
								CView* 				pActivateView,
								CView* 				pDeactiveView);
	
//	#ifdef _WIN32
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
//	#endif	// _WIN32
    	                                
protected: 

	static LOGFONT NEAR 		m_lfDefFont;
	CFont 						m_font;   
	
		// Generated message map functions
	
	//{{AFX_MSG(CMTextView)
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintSetup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileClose(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
	
			 // Flag indicated whether this view is active
			 							
	Boolean	 			m_activeFlag; 
	
};

#ifndef _DEBUG  // debug version in mspecvw.cpp
inline CMTextDoc* CMTextView::GetDocument()
   { return (CMTextDoc*)m_pDocument; }
#endif     

//	#endif // defined _X86_ || defined _AMD64_
          
#endif // !defined __MIMAGVEW_H__

/////////////////////////////////////////////////////////////////////////////
