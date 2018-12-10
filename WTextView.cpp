// WTextView.cpp : implementation file
//
//	Revised by Larry Biehl on 08/30/2018
                   
#include "SMultiSpec.h"
                   
#include "WImageView.h"
#include "WTextDoc.h"
#include "WTextView.h"

//#include	"SExternalGlobals.h"  
								
extern void 					UpdateEditTextClear (
										CCmdUI*					pCmdUI);
								
extern void 					UpdateEditTextCopy (
										CCmdUI*					pCmdUI); 
								
extern void 					UpdateEditTextCut (
										CCmdUI*					pCmdUI); 
								
extern void 					UpdateEditTextPaste (
										CCmdUI*					pCmdUI);
								
extern void 					UpdateEditTextSelectAll (
										CCmdUI*					pCmdUI);

extern Boolean 				UpdateFileOutputTextSaveAs (
										CCmdUI*					pCmdUI);
							
extern Boolean 				UpdateFileOutputTextPrint (
										CCmdUI*					pCmdUI);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMTextView

IMPLEMENT_DYNCREATE(CMTextView, CEditView);


BEGIN_MESSAGE_MAP(CMTextView, CEditView)
	//{{AFX_MSG_MAP(CMTextView)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_SETUP, OnUpdateFilePrintSetup)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()  


LOGFONT NEAR CMTextView::m_lfDefFont;


CMTextView::CMTextView()
{                 
	gOutputViewCPtr = this;

}		// end "CMTextView"



CMTextView::~CMTextView()
{              
	gOutputViewCPtr = NULL;
    
	m_font.DeleteObject();
	
}		// end "~CMTextView"          

									


//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListString
//
//	Software purpose:	The purpose of this routine is to list the input
//							information to the output text window.
//
//	Parameters in:		Pointer to 'c' string.
//
//	Parameters out:	None
//
//	Value Returned:	True if operation is okay
//							False if the text limit was reached	
// 
// Called By:			main
//							DisplayCImage  in displayc.c
//							DisplayPImage  in displayp.c
//
//	Coded By:			Larry L. Biehl			Date: 03/30/1988
//	Revised By:			Larry L. Biehl			Date: 03/03/2017			

Boolean CMTextView::ListString ( 
				HPtr									textPtr, 
				UInt32								textLength,
				SInt16								charFormatCode)

{      
	TBYTE								tempWideCharacterString[1000];


	USES_CONVERSION;

			// Make certain that there is a 'c' terminator at the end of the string.
	
	if (textPtr[textLength] != 0)
		textPtr[textLength] = 0;
   
			//
	if (charFormatCode == kUnicodeCharString)
		GetEditCtrl().ReplaceSel((LPCTSTR)textPtr);
		
	else if (charFormatCode == kUTF8CharString)
		{
		//std::string utf8 = UTF8FromUTF16 (filePathCPtr);  requires visualc++ 2010
		int sizeNeeded = MultiByteToWideChar (
									CP_UTF8, 0, textPtr, -1, NULL, 0);

		sizeNeeded = MIN(sizeNeeded, 1000);
		MultiByteToWideChar (CP_UTF8, 0, textPtr, -1, (LPWSTR)tempWideCharacterString, sizeNeeded);

		GetEditCtrl().ReplaceSel((LPCTSTR)tempWideCharacterString);
		}

	else	// is kASCIICharString
		{
		GetEditCtrl().ReplaceSel((LPCTSTR)A2T(textPtr));
		}
	                                 
	GetEditCtrl().SetModify(TRUE);
	
	return (TRUE);    
	
}			// end "ListString" 



/////////////////////////////////////////////////////////////////////////////
// CMTextView drawing
/*
void CMTextView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
		// Add draw code here
}
*/


void 
CMTextView::OnInitialUpdate()

{
//	CScrollView::OnInitialUpdate();
//
//	CSize sizeTotal;
	// TODO: calculate the total size of this view
//	sizeTotal.cx = sizeTotal.cy = 100;
//	SetScrollSizes(MM_TEXT, sizeTotal);
	
//	SetScrollSizes(MM_TEXT, GetDocument()->GetDocSize());
	
			// Move the text window to the right portion of the client area
			// of the main frame rect.
	                            
	CRect		mainFrameRect,
				textWindowRect;
	SInt16	xLocation,
				xSize,
				yLocation,
				ySize;		
	
	CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	pMainFrame->GetClientRect (&mainFrameRect);
	GetParentFrame()->GetWindowRect (&textWindowRect);
	
	xSize = (SInt16)(textWindowRect.right - textWindowRect.left);
	ySize = (SInt16)(textWindowRect.bottom - textWindowRect.top);
	xLocation = (SInt16)(mainFrameRect.right - xSize - 7);
	xLocation = MAX (0, xLocation);
	yLocation = (SInt16)(mainFrameRect.top + 3);
	yLocation = MAX (0,yLocation);
	GetParentFrame()->MoveWindow (xLocation, yLocation, xSize, ySize, TRUE); 
	
	CEditView::OnInitialUpdate();

			// This should set the limit to 0x0fffffff for when running
			// under Windows NT and 0xffff when running Windows 95/98.

	CEdit& editControlPtr = GetEditCtrl ();
	editControlPtr.SetLimitText (0x0FFFFFFF);

	SInt32 limit = editControlPtr.GetLimitText();
}     

 

void 
CMTextView::OnActivateView(
				BOOL 			bActivate, 
				CView* 		pActivateView,
				CView* 		pDeactiveView)
				
{
	CEditView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	
	m_activeFlag = bActivate;
	
}		// end "OnActivateView"


/////////////////////////////////////////////////////////////////////////////
// CMTextView printing 

 

BOOL CMTextView::PreCreateWindow(
				CREATESTRUCT&			cs)

{      
	return CEditView::PreCreateWindow(cs);
                                  
//	cs.style |= WS_MINIMIZE; 
	
//	CEditView::PreCreateWindow(cs); 
	
//	return (TRUE);

}	// end "PreCreateWindow"

 
 
/////////////////////////////////////////////////////////////////////////////
// CMTextView diagnostics

#ifdef _DEBUG
void CMTextView::AssertValid() const
{
	CEditView::AssertValid();
}

void CMTextView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CMTextDoc* CMTextView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMTextDoc)));
	return (CMTextDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMTextView message handlers 

void CMTextView::OnUpdateFilePrint(CCmdUI* pCmdUI)
{                                     
	Boolean enableFlag = UpdateFileOutputTextPrint (pCmdUI);
	
	enableFlag = (enableFlag && GetTextBufferLength() != 0); 
	                                                                               
	pCmdUI->Enable(enableFlag);        
	
}



void CMTextView::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{                                                                                                                                                  
	pCmdUI->Enable(GetTextBufferLength() != 0); 
	                      
}



void CMTextView::OnUpdateFilePrintSetup(CCmdUI* pCmdUI)
{                                                                                                                     
	pCmdUI->Enable(TRUE);
	
}



void CMTextView::OnUpdateFileSave(CCmdUI* pCmdUI)
{                                                                                                                     
	pCmdUI->Enable( UpdateFileSave(pCmdUI) );
	
}



void CMTextView::OnUpdateFileSaveAs(CCmdUI* pCmdUI)
{                       
	Boolean enableFlag = UpdateFileOutputTextSaveAs (pCmdUI);
	
	enableFlag = (enableFlag && GetTextBufferLength() != 0); 
	                                                                                             
	pCmdUI->Enable(enableFlag);            
	
}		// end "OnUpdateFileSaveAs"



void CMTextView::OnUpdateFileClose(CCmdUI* pCmdUI)
{                                                                                                                                                   
	pCmdUI->Enable(FALSE);
	
}



int CMTextView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{              
	LOGFONT		lf;
	
	
	if (CEditView::OnCreate(lpCreateStruct) == -1)
		return -1; 

			// Set font to be used. 
   
	::GetObject(GetStockObject(SYSTEM_FIXED_FONT), sizeof(LOGFONT), &lf);
	
			// Change to Courier font. 
                           
/*	lf.lfHeight = 0xfff3;   
	lf.lfWidth = 0x0000;    
	lf.lfQuality = 0x01;
*/
//new
	lf.lfHeight = 10;   
	lf.lfWidth = 0;    
	lf.lfQuality = DEFAULT_QUALITY;
//

	lf.lfFaceName[0] = 'C';
	lf.lfFaceName[1] = 'o'; 
	lf.lfFaceName[2] = 'u';
	lf.lfFaceName[3] = 'r';
	lf.lfFaceName[4] = 'i';
	lf.lfFaceName[5] = 'e';
	lf.lfFaceName[6] = 'r';
	lf.lfFaceName[7] = 0x00;
                                                               
	if (m_font.CreateFontIndirect(&lf))
		{                   
		SetFont(&m_font);
		m_lfDefFont = lf;
		
		}		// end "if (m_font.CreateFontIndirect(&lf))"
			
   SetTabStops (4);
	
	return 0;
	
}		// end "OnCreate"



void CMTextView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
		// Only process the delete key for now
	
 	if (nChar == 8 && nRepCnt == 0)
		CWnd::OnChar(nChar, nRepCnt, nFlags);
		
} 



void CMTextView::OnMouseMove(
				UINT 			nFlags, 
				CPoint 		point)
				
{                                    
	
	CEditView::OnMouseMove(nFlags, point);
	
} 	// end "OnMouseMove"



BOOL CMTextView::OnSetCursor (
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

	if (!m_activeFlag)
		{
		if (gPresentCursor != kArrow && gActiveImageViewCPtr != NULL)
			gActiveImageViewCPtr->UpdateCursorCoordinates();

		SetCursor (AfxGetApp()->LoadStandardCursor (IDC_ARROW));
		gPresentCursor = kArrow;		// Non-image window cursors.
		
																					return TRUE;
		
		}	// end "if (!m_activeFlag)"
		
	gPresentCursor = kIBeam;
	
	return CEditView::OnSetCursor (pWnd, nHitTest, message);
	
}	// end "OnSetCursor"



void CMTextView::OnFilePrintPreview ()

{                                                       
	CEditView::OnFilePrintPreview ();
	
}



void CMTextView::OnUpdateEditClear (CCmdUI* pCmdUI)

{  
	int		startSel, endSel;
	
	
	UpdateEditTextClear (pCmdUI);
	
	CEdit& editControlPtr = GetEditCtrl ();
	editControlPtr.GetSel (startSel, endSel);                                                                 
	pCmdUI->Enable(endSel > startSel);            
	
} 



void CMTextView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{                          
	int		startSel, endSel;
	
	                              
	UpdateEditTextCopy (pCmdUI);
	
	CEdit& editControlPtr = GetEditCtrl ();
	editControlPtr.GetSel (startSel, endSel);                                                                 
	pCmdUI->Enable(endSel > startSel);
	
} 



void CMTextView::OnUpdateEditCut(CCmdUI* pCmdUI)
{                    
	int		startSel, endSel;
	
	                                    
	UpdateEditTextCut (pCmdUI);
	
	CEdit& editControlPtr = GetEditCtrl ();
	editControlPtr.GetSel (startSel, endSel);                                                                
	pCmdUI->Enable(endSel != startSel);
	
}



void CMTextView::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{                                                       
	UpdateEditTextSelectAll (pCmdUI);
	                                                           
	pCmdUI->Enable(GetTextBufferLength() != 0);
	
}  



SInt32 
CMTextView::GetTextBufferLength(void)

{  
	Boolean modifiedFlag = GetEditCtrl().GetModify();
	SInt32 bufferLength = GetBufferLength();

			// Reset the modified flag. Under Windows 95, it
			// get set to false during the call to GetBufferLength.
	
	GetEditCtrl().SetModify(modifiedFlag);

	return (bufferLength);
	
}		// end "GetTextBufferLength" 


/*
void CMTextView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{                                                       
	UpdateEditTextPaste (pCmdUI);
	
}
*/
