// WGraphFrame.h : header file
//  
               
#if !defined __WGRAFFRM_H__
	#define __WGRAFFRM_H__ 

/////////////////////////////////////////////////////////////////////////////
// CMGraphFrame frame

class CMGraphFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CMGraphFrame)
protected:
	CMGraphFrame();			// protected constructor used by dynamic creation
              
public: 
	virtual BOOL 			OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext); 
	
	virtual BOOL			PreCreateWindow(CREATESTRUCT& cs); 
									
	void						SetGraphViewCPtr(
									CMGraphView*						graphViewCPtr);     
                  
protected:
	virtual 					~CMGraphFrame();

			// Generated message map functions
			
	//{{AFX_MSG(CMGraphFrame)
	afx_msg void OnClose();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnFilePrint();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateFileGraphClose(CCmdUI* pCmdUI);
	//}}AFX_MSG
	
	CMGraphView*			m_graphViewCPtr;
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////  

#endif	// !defined __WGRAFFRM_H__
