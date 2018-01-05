// WStatisticsFrame.h : header file
//        
               
#if !defined __WSTATFRM_H__
	#define __WSTATFRM_H__

/////////////////////////////////////////////////////////////////////////////
// CMStatisticsFrame frame

class CMStatisticsFrame : public CMDIChildWnd
//class CMStatisticsFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(CMStatisticsFrame)
protected:
	CMStatisticsFrame();			// protected constructor used by dynamic creation

public:   
	virtual BOOL			PreCreateWindow(CREATESTRUCT& cs);
                  
protected:
	virtual 					~CMStatisticsFrame();

	// Generated message map functions
	//{{AFX_MSG(CMStatisticsFrame)
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnUpdateEditClearSelectRectangle(CCmdUI* pCmdUI);
	afx_msg void OnEditClearSelectRectangle();
	afx_msg void OnUpdateEditSelectionRectangle(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectionRectangle();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnClose();
	afx_msg void OnUpdateFileProjectClose(CCmdUI* pCmdUI);
	afx_msg void OnFileProjectClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
          
#endif	// !defined __WSTATFRM_H__