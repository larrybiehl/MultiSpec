// WGraphFrame.cpp : implementation file
//
// Revised by Larry Biehl on 03/27/2018
                       
#include "SMultiSpec.h"

//new                    
#include <afxext.h>
//end new            
#include "WGraphView.h" 
#include "WMultiSpec.h"
#include "WGraphDoc.h" 
#include "WGraphFrame.h"

//#include "SExtGlob.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMGraphFrame

IMPLEMENT_DYNCREATE(CMGraphFrame, CMDIChildWnd)

CMGraphFrame::CMGraphFrame()
{
	m_graphViewCPtr = NULL;
	
}

CMGraphFrame::~CMGraphFrame()
{
}
	
BOOL 
CMGraphFrame::PreCreateWindow(CREATESTRUCT& cs)

{                   
	SInt16		xSize = 280,
					ySize = 190;
					                
	
	if (cs.cx != 0)
		{			                             
/*				// Set the window style
		
		cs.style = WS_CHILD |    
						WS_OVERLAPPED |
//						WS_THICKFRAME |
						WS_MINIMIZEBOX | 
						WS_SYSMENU |
						WS_CAPTION |
						WS_BORDER |
						0x00008000;  	// Don't know what this value is.
*/
		GetGraphWindowLocation (&xSize, &ySize);
		
				//		This sets the size of the window
					
		cs.cx = xSize;
		cs.cy = ySize; 
		
				//		This sets the location of the window
					
		cs.x = gNextGraphicsWindowStart.h;
		cs.y = gNextGraphicsWindowStart.v; 
		
		}		// end "if (cs.cx != 0)"
	
	return (CMDIChildWnd::PreCreateWindow(cs) ); 

}		// end "PreCreateWindow"  



void CMGraphFrame::OnClose()
{  
	CMDIChildWnd::OnClose();
}		// end "OnClose" 



BOOL 
CMGraphFrame::OnCreateClient(
				LPCREATESTRUCT 		lpcs, 
				CCreateContext* 		pContext)
				
{  
	Boolean returnFlag = FALSE;
	
	                                                
	if ( CMDIChildWnd::OnCreateClient(lpcs, pContext) )
		{
		((CMGraphDoc*)pContext->m_pCurrentDoc)->SetGraphFrameCPtr(this);
		returnFlag = TRUE;
		
		}		// end "if ( CMDIChildWnd::OnCreateClient(lpcs, pContext) )"
		
	return (returnFlag);
	
}		// end "OnCreateClient" 
 


BEGIN_MESSAGE_MAP(CMGraphFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CMGraphFrame)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileGraphClose)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


afx_msg void 
CMGraphFrame::OnSysCommand (
				UINT 			nID,
				LPARAM		lParam)
                      
{                     
	if (gProcessorCode != kListDataProcessor || 
										(nID != SC_CLOSE && nID != SC_MAXIMIZE))
		CFrameWnd::OnSysCommand (nID, lParam);

	else   // gProcessorCode == kListDataProcessor && ...
		{
		if (nID == SC_CLOSE)
			m_graphViewCPtr->m_closeGraphSelectedFlag = TRUE;

		}	// end "else   // gProcessorCode == kListDataProcessor && ..."

}	// end "OnSysCommand"


									
void 
CMGraphFrame::SetGraphViewCPtr(
				CMGraphView*			graphViewCPtr)  
				
{               
	m_graphViewCPtr = graphViewCPtr;
	
}		// end "SetGraphViewCPtr"

 

void 
CMGraphFrame::OnUpdateFilePrint(CCmdUI* pCmdUI)
{                                                        
	Boolean enableFlag = UpdateFileGraphPrint (pCmdUI);
	                                                                                     
	pCmdUI->Enable( enableFlag ); 
	
}		// end "OnUpdateFilePrint" 



void CMGraphFrame::OnFilePrint()
{             
	((CMultiSpecApp*)AfxGetApp())->SetPrintOrientation (GetActiveWindowInfoHandle());

	m_graphViewCPtr->DoFilePrint();
	
}		// end "OnFilePrint" 



void CMGraphFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{                                                        
	                                                                                     
	pCmdUI->Enable( TRUE ); 
	
}		// end "OnUpdateFilePrintPreview"



void CMGraphFrame::OnFilePrintPreview()
{                      
	((CMultiSpecApp*)AfxGetApp())->SetPrintOrientation (GetActiveWindowInfoHandle());

	m_graphViewCPtr->DoFilePrintPreview();
	
}		// end "OnFilePrintPreview"

void CMGraphFrame::OnUpdateFileGraphClose(CCmdUI* pCmdUI) 
{
	UpdateFileGraphClose(pCmdUI);

	pCmdUI->Enable (TRUE);
	
}
