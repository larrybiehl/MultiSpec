// WImageSplitterWnd.cpp : implementation file
//
                    
#include "../SMultiSpec.h"
#include "LImageSplitterWnd.h"
                     
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif	// _DEBUG  

/////////////////////////////////////////////////////////////////////////////
// CMImageSplitterWnd
// Just used to block default mouse click event on image window
// in microsoft windows
// Leaving it for now for linux until need arises for it
IMPLEMENT_DYNAMIC_CLASS(CMImageSplitterWnd, wxSplitterWindow)


CMImageSplitterWnd::CMImageSplitterWnd(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& name = "splitterWindow"):wxSplitterWindow(wxWindow* parent, wxWindowID id, const wxPoint& point = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style=wxSP_3D, const wxString& name = "splitterWindow")
{

}
//BEGIN_MESSAGE_MAP(CMImageSplitterWnd, CSplitterWnd)
//	//{{AFX_MSG_MAP(CMImageSplitterWnd)
//	ON_WM_NCHITTEST()
//	//}}AFX_MSG_MAP
//	// Standard printing commands
//END_MESSAGE_MAP()

// CMImageSplitterWnd message handlers
//afx_msg UINT CMImageSplitterWnd::OnNcHitTest (CPoint point)
//{
//	Handle			windowInfoHandle;
//	SInt16			windowType;
//
//
//	windowInfoHandle = GetActiveImageWindowInfoHandle ();
//	windowType = GetWindowType (windowInfoHandle);
//	if (windowType == kImageWindowType)
//		return HTNOWHERE;
//
//	else		// windowType != kImageWindowType
//		return (CSplitterWnd::OnNcHitTest (point));
//}


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