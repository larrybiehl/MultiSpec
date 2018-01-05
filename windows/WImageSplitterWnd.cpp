// WImageSplitterWnd.cpp : implementation file
//
                    
#include "SMultiSpec.h"

#include "WImageSplitterWnd.h"
                     
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif	// _DEBUG  

/////////////////////////////////////////////////////////////////////////////
// CMImageSplitterWnd

IMPLEMENT_DYNAMIC(CMImageSplitterWnd, CSplitterWnd) 

BEGIN_MESSAGE_MAP(CMImageSplitterWnd, CSplitterWnd)
	//{{AFX_MSG_MAP(CMImageSplitterWnd) 
	ON_WM_NCHITTEST()
	//}}AFX_MSG_MAP
	// Standard printing commands 
END_MESSAGE_MAP() 

// CMImageSplitterWnd message handlers
afx_msg LRESULT CMImageSplitterWnd::OnNcHitTest (CPoint point)
{
	Handle			windowInfoHandle;
	SInt16			windowType;


	windowInfoHandle = GetActiveImageWindowInfoHandle ();
	windowType = GetWindowType (windowInfoHandle);
	if (windowType == kImageWindowType)
		return HTNOWHERE;

	else		// windowType != kImageWindowType
		return (CSplitterWnd::OnNcHitTest (point));
}


//void CMImageSplitterWnd::OnDrawSplitter(
//							CDC*					pDC, 
//							ESplitType			nType,
//							const CRect&		rectArg)
//	{
//	Handle			windowInfoHandle;
//	SInt16			windowType;


//	windowInfoHandle = GetActiveImageWindowInfoHandle ();
//	windowType = GetWindowType (windowInfoHandle);
//	if (windowType != kImageWindowType)
//		CSplitterWnd::OnDrawSplitter (pDC, nType, rectArg);

//	}		// end "OnDrawSplitter"