// WTextFrame.cpp : implementation file
//
// Revised by Larry Biehl on 08/30/2018
                    
#include "SMultiSpec.h"
                     
#include "WImageView.h"
#include "WTextFrame.h"

//#include	"SExtGlob.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMTextFrame

IMPLEMENT_DYNCREATE(CMTextFrame, CMDIChildWnd)

CMTextFrame::CMTextFrame()
{
}

CMTextFrame::~CMTextFrame()
{
}

/*
BOOL 
CMTextFrame::OnCreateClient(
				LPCREATESTRUCT 		/*lpcs, 
				CCreateContext* 		pContext)
				
{  
//	ShowWindow(SW_SHOWMINIMIZED);
	
//	return m_wndSplitter.Create(this,
//		2, 1,		// TODO: adjust the number of rows, columns
//		CSize(20, 20),	// TODO: adjust the minimum pane size
//		pContext);
		
	return (TRUE);
		
}		// end "OnCreateClient"
*/


BEGIN_MESSAGE_MAP(CMTextFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CMTextFrame)
	ON_WM_CREATE()
	ON_WM_INITMENUPOPUP()
	ON_WM_CLOSE()
	ON_WM_SETCURSOR()
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileTextClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMTextFrame message handlers


int CMTextFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}



void 
CMTextFrame::OnInitMenuPopup(
				CMenu* 			pPopupMenu, 
				UINT 				nIndex, 
				BOOL 				bSysMenu)

{
	CMDIChildWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu); 
   
   if (bSysMenu)
   	{                                        
		if (pPopupMenu != NULL)
			pPopupMenu->EnableMenuItem(6, MF_BYPOSITION | MF_GRAYED);
			
		}		// end "if (bSysMenu)"
	
}		// end "OnInitMenuPopup"

 

void 
CMTextFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default

			// I do not want the text output window to be closed.
				
//	CMDIChildWnd::OnClose();

}		// end "OnClose"



BOOL CMTextFrame::OnSetCursor (
			CWnd* 		pWnd, 
			UINT 			nHitTest, 
			UINT 			message)
			
{                                                                                              
	if (gPresentCursor == kWait || gPresentCursor == kSpin)
		{
				// Wait cursor in affect. Processing underway.
				// Restart the wait cursor in case in was changed to pointer
				// before entering the image frame.
		
		AfxGetApp ()->DoWaitCursor (0);
																			return (TRUE);
		
		}	// end "if (gPresentCursor == kWait || gPresentCursor == kSpin)"

	if (gPresentCursor != kArrow)
		{                  
		if (gActiveImageViewCPtr != NULL)
			gActiveImageViewCPtr->UpdateCursorCoordinates();
		                                                                
		gPresentCursor = kArrow; 		// Non image window cursor
		
		}		// end "if (gPresentCursor != kArrow)"
	
	return CMDIChildWnd::OnSetCursor(pWnd, nHitTest, message);
	
}	// end "OnSetCursor"



void CMTextFrame::OnUpdateFileTextClose(CCmdUI* pCmdUI) 
{
	UpdateFileTextClose(pCmdUI);

	pCmdUI->Enable (FALSE);
	
}
