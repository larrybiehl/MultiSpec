// WToolBar.h : interface of the CMToolBar class
//
// Revised by Larry Biehl on 03/27/2017
//
/////////////////////////////////////////////////////////////////////////////
                
#if !defined __WTOOLBAR_H__
	#define __WTOOLBAR_H__ 
	
class CMToolBar : public CToolBar
{                              
public:
	CMToolBar();

// Attributes
public:

// Operations
public:
//new
	void IsZoomPressed(CPoint point);
	UINT ConvertToID(CPoint point);
//end new

// Implementation
public:
	virtual ~CMToolBar();

protected:  // control bar embedded members  
	
	void OnShowOverlay (
				UINT							menuID);

// Generated message map functions
protected:
	//{{AFX_MSG(CToolBarCtrl)
	afx_msg void 		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void 		OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void 		OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk( UINT, CPoint );
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	  
//#ifdef _WIN32             
	SInt16								m_iButtonCapture;
	
	CMenu*								m_overlayMenu;
	
	Boolean								m_rightButtonFlag,
											m_optionOverlayFlag;
//#endif       

};

/////////////////////////////////////////////////////////////////////////////
                     
#endif	// !defined __WTOOLBAR_H__