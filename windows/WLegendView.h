// WLegendView.h : header file
//        

#if !defined __WTLEGEND_H__
	#define __WTLEGEND_H__  
	
#include "WLegendList.h"

/////////////////////////////////////////////////////////////////////////////
// CMLegendView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif                
       

class CMButton : public CButton
{          
public:
	CMButton();  
	
	virtual ~CMButton();
	
	void		SetButtonID(
					UInt16 				buttonID); 

// Generated message map functions
protected:
	//{{AFX_MSG(CMButton)
	afx_msg void 		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void 		OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void 		OnLButtonUp(UINT nFlags, CPoint point); 
	afx_msg void 		OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void 		OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void 		OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void 		OnRButtonUp(UINT nFlags, CPoint point);   
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	UInt16	m_buttonID;     
							
};                 
       

class CMComboBox : public CComboBox
{          
public:
	CMComboBox();  
	
	virtual ~CMComboBox();
	
	void					DrawLegendTitle (); 

// Generated message map functions
protected:
	//{{AFX_MSG(CMComboBox)                                    
	afx_msg void 		OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void 		OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);  
	afx_msg void		OnDrawItem(int /*nIDCtl*/, LPDRAWITEMSTRUCT lpDrawItemStruct);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()    
							
};



class CMLegendView : public CFormView
{
	DECLARE_DYNCREATE(CMLegendView)
protected:
	CMLegendView();			// protected constructor used by dynamic creation  
			
	virtual void 		OnInitialUpdate();	// first time after construct 

// Form Data
public:
	//{{AFX_DATA(CMLegendView)
	enum { IDD = IDD_LegendWindow };
	int						m_classGroupCode; 
	int						m_paletteSelection; 

	CMLegendList			m_legendListBox;
	CMButton					m_legendScrollDown;
	CMButton					m_legendScrollUp; 
	CMComboBox				m_legendTitleCombo;
	CMComboBox				m_legendPaletteCombo;
	//}}AFX_DATA    
	
	void						AdjustLegendListLength (void);
		
	CMImageDoc* 			GetDocument(void);
	
	CMImageFrame*			GetImageFrameCPtr (void);

	CMImageView* 			GetImageView(void);
	
	CMLegendList*			GetLegendListCPtr (void);
	
	void 						SetImageView (
									CMImageView*				imageViewCPtr); 
									
	void						SetLegendWidthSetFlag (void);
	
	void 						UpdateClassGroupComboList(
									SInt16						newClassGroupCode);
									
	void						UpdateThematicLegendControls (void);

// Operations
public:

// Implementation
protected:
	virtual ~CMLegendView();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support 
			
	virtual void 		OnActivateView(
								BOOL 					bActivate, 
								CView* 				pActivateView,
								CView* 				pDeactiveView);
								
	void					OnDraw(
								CDC* 					pDC);
										
	virtual void		OnUpdate(
								CView*				pSender,
								LPARAM				lHint = 0L,
								CObject*				pHint = NULL); 
										
	// Generated message map functions
	//{{AFX_MSG(CMLegendView) 
			afx_msg LRESULT 	OnDoRealize(WPARAM wParam, LPARAM lParam);  // user message
	afx_msg void OnSelendokClassGroup();
	afx_msg void OnSelendokPalette();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDropdownClassGroupCombo();
	afx_msg void OnDropdownLegendCombo();
	afx_msg void OnPaint();
	afx_msg void OnScrollDown();
	afx_msg void OnScrollUp();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDblclkList1();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSelendcancelPaletteCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	Boolean				m_initializedFlag;
	
	Boolean				m_legendWidthSetFlag,
	                  m_localPaletteUpToDateFlag,

							m_paletteChangedFlag;


	CDataExchange		*m_dialogFromPtr,
							*m_dialogToPtr; 
							
	CMImageView*		m_imageViewCPtr;
	
	SInt16				m_numberDisplayedListItems;

			// Flag indicating whether we are already in the SetLegendWidth
			// routine flag. This is to stop recursion in the routine from
			// call to OnSize..
									
	static Boolean		s_inSetLegendWidthFlag;
							
};

/////////////////////////////////////////////////////////////////////////////  
	
	inline CMImageDoc* CMLegendView::GetDocument()
	   { return (CMImageDoc*)m_pDocument; }

	inline CMImageView* CMLegendView::GetImageView()
	   { return (CMImageView*)m_imageViewCPtr; }
          
#endif	// !defined __WTLEGEND_H__
