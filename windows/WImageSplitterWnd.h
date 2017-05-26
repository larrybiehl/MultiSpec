// WImageSplitterWnd.h : header file
//     

#if !defined __WIMASPLI_H__
	#define __WIMASPLI_H__  

/////////////////////////////////////////////////////////////////////////////
// CMImageFrame frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif         
                     
class CMImageSplitterWnd : public CSplitterWnd
{
	DECLARE_DYNAMIC(CMImageSplitterWnd)
public:
	CMImageSplitterWnd() { }
//TTY change
//#if defined _AMD64_
	void set_cxSplitter(int cxSplitter) { m_cxSplitter = cxSplitter; }
	void set_cxSplitterGap(int cxSplitterGap) { m_cxSplitterGap = cxSplitterGap; }
	void set_cxBorder(int cxBorder) { m_cxBorder = cxBorder; }
//#endif
//	void OnDrawSplitter(
//							CDC*						pDC, 
//							ESplitType					nType,
//							const CRect&				rectArg);

public:
	// Generated message map functions
	//{{AFX_MSG(CMImageFrame)
	afx_msg LRESULT 		OnNcHitTest(CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif	// !defined __WIMASPLI_H__

