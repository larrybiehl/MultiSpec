// WTextFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMTextFrame frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CMTextFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CMTextFrame)
protected:
	CMTextFrame();			// protected constructor used by dynamic creation

// Attributes
protected:
//	CSplitterWnd	m_wndSplitter;
public:

// Operations
public:

// Implementation
public:
	virtual ~CMTextFrame();
//	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	// Generated message map functions
	//{{AFX_MSG(CMTextFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnClose();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnUpdateFileTextClose(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
