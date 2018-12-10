// WLegendView.cpp : implementation file
//       
// Revised by Larry Biehl on 08/30/2018
//
                    
#include "SMultiSpec.h"
                    
#include "WMultiSpec.h"
#include "WImageView.h"
#include "WImageDoc.h"  
#include "WImageFrame.h"
#include "WMainFrame.h"    
#include "WLegendView.h"
#include "CPalette.h"

//#include	"SExtGlob.h" 

extern SInt16 			GetComboListSelection(                     
								CWnd*									cWindowPtr,
								UInt16								dialogItemNumber,
								SInt32								selectedListItemData);	

extern void				DoNextDisplayChannelEvent (
								WindowPtr							window,
								char									theChar);	

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif 
									
Boolean		CMLegendView::s_inSetLegendWidthFlag = FALSE;

Boolean 					DetermineIfFalseColorAvailable (       
								SInt16								fileFormat,
								UInt32								numberClasses,
								Handle								descriptionH);											


/////////////////////////////////////////////////////////////////////////////
// CMLegendView

IMPLEMENT_DYNCREATE(CMLegendView, CFormView)

CMLegendView::CMLegendView()
	: CFormView(CMLegendView::IDD)
{
	//{{AFX_DATA_INIT(CMLegendView)
	m_classGroupCode = kClassDisplay-1;
	m_paletteSelection = 0;
	//}}AFX_DATA_INIT 
	
	m_initializedFlag = FALSE;
	m_legendWidthSetFlag = FALSE;
	m_dialogFromPtr = NULL; 
	m_dialogToPtr = NULL;
	m_imageViewCPtr = NULL;
	m_numberDisplayedListItems = 0;
			
	TRY
		{                          
		m_dialogFromPtr = new CDataExchange(this, TRUE);
	
		m_dialogToPtr = new CDataExchange(this, FALSE);
		
		m_initializedFlag = TRUE;
		
		}
		
	CATCH_ALL(e)
		{                         
		MemoryMessage(0, kObjectMessage);
		
		}
		
	END_CATCH_ALL
	   
}		// end "CMLegendView"



CMLegendView::~CMLegendView()
{                             
	if (m_dialogFromPtr != NULL)
		delete m_dialogFromPtr; 
			
	if (m_dialogToPtr != NULL)
		delete m_dialogToPtr;
		
} 		// end "~CMLegendView"



void CMLegendView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMLegendView)
	DDX_CBIndex(pDX, IDC_COMBO1, m_classGroupCode);
	DDX_CBIndex(pDX, IDC_PaletteCombo, m_paletteSelection);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CMLegendView, CFormView)
	//{{AFX_MSG_MAP(CMLegendView) 
	ON_MESSAGE(WM_DOREALIZE, OnDoRealize)
	ON_CBN_SELENDOK(IDC_COMBO1, OnSelendokClassGroup)
	ON_CBN_SELENDOK(IDC_PaletteCombo, OnSelendokPalette)
	ON_WM_SIZE()
	ON_CBN_DROPDOWN(IDC_COMBO1, OnDropdownClassGroupCombo)
	ON_CBN_DROPDOWN(IDC_PaletteCombo, OnDropdownLegendCombo)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_ScrollDown, OnScrollDown)
	ON_BN_CLICKED(IDC_ScrollUp, OnScrollUp)
	ON_WM_LBUTTONUP()   
	ON_WM_MOUSEMOVE()
	ON_LBN_DBLCLK(IDC_List1, OnDblclkList1)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SETCURSOR()
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	ON_CBN_SELENDCANCEL(IDC_PaletteCombo, OnSelendcancelPaletteCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMLegendView message handlers   
                            

		                      
void 
CMLegendView::AdjustLegendListLength (void) 

{                                                                           
	CRect						legendRect;

	Handle					windowInfoHandle;
				
	SInt16					height,
								windowType; 
					
	
	windowInfoHandle = GetWindowInfoHandle (m_imageViewCPtr);
	windowType = GetWindowType (windowInfoHandle);
	if (windowType == kThematicWindowType)
		{
				// Adjust the list to fit as much of the legend as is possible.
		
		CRect		listRect;			
		GetWindowRect (legendRect); 
		m_legendListBox.GetWindowRect (listRect);
		CSize listSize = listRect.Size(); 
		
		listSize.cx = legendRect.right - listRect.left - 5;
		
		height = m_legendListBox.GetItemHeight(0);
		height = MAX(height, 20);
		m_numberDisplayedListItems = (SInt16)((legendRect.bottom - 17 - listRect.top)/height);
		listSize.cy = m_numberDisplayedListItems * height;
		
		m_legendListBox.SetWindowPos (NULL, 
									0, 
									0,
									listSize.cx,
									listSize.cy,
									SWP_NOMOVE+SWP_NOZORDER+SWP_NOACTIVATE); 
			
				// Adjust the legend title to the proper position.    
		
		CComboBox*			legendTitle;
		CRect					comboRect;
		
//		#ifdef _WIN32
			SInt32			left,
								top;			                                                                        
//		#else
//			SInt16			left,
//								top;
//		#endif
				                                                                        
		legendTitle = (CComboBox*)GetDlgItem(IDC_COMBO1);
		legendTitle->GetWindowRect (comboRect);
		CSize comboSize = comboRect.Size();
		left = (legendRect.right - legendRect.left - comboSize.cx)/2;  
		legendTitle->SetWindowPos (NULL, 
									left, 
									comboRect.top - legendRect.top - 1,
									comboSize.cx,
									comboSize.cy,
									SWP_NOZORDER+SWP_NOACTIVATE+SWP_NOMOVE); 
			
				// Adjust the the scroll up botton to the proper position.
		
		CButton*				scrollButton;		                                                                        
		scrollButton = (CButton*)GetDlgItem(IDC_ScrollUp);
		
		#ifdef _WIN32
			left = legendRect.right - legendRect.left - 16;  
			top = legendRect.bottom - legendRect.top - 16;
		#else
			left = legendRect.right - legendRect.left - 18;  
			top = legendRect.bottom - legendRect.top - 18;
		#endif
		
		scrollButton->SetWindowPos (NULL, 
									left, 
									top,
									17,
									17,
									SWP_NOZORDER+SWP_NOACTIVATE);       
			
				// Adjust the the scroll down botton to the proper position.
		                                  		                                                                        
		scrollButton = (CButton*)GetDlgItem(IDC_ScrollDown);
		left -= 17;
		scrollButton->SetWindowPos (NULL, 
									left, 
									top,
									17,
									17,
									SWP_NOZORDER+SWP_NOACTIVATE);     
			
				// Adjust the palette combo box to the proper position
									
		CComboBox*			paletteCombo;

		paletteCombo = (CComboBox*)GetDlgItem(IDC_PaletteCombo);
		paletteCombo->GetWindowRect (comboRect);
		comboSize = comboRect.Size();
		left -= comboSize.cx;  
		paletteCombo->SetWindowPos (NULL, 
									left, 
									top,
									comboSize.cx,
									comboSize.cy,
									SWP_NOZORDER+SWP_NOACTIVATE);   
		
		UpdateThematicLegendControls();

		}		// end "if (windowType == kThematicWindowType)"
	
}		// end "AdjustLegendListLength"



CMImageFrame* 
CMLegendView::GetImageFrameCPtr (void)

{
	CMImageFrame* 	imageFrameCPtr = NULL;
	
	if (this != NULL)
		{
		imageFrameCPtr = GetDocument()->GetImageFrameCPtr();
		
		}		// end "if (this != NULL)"
		
	return (imageFrameCPtr);
	
}		// end "GetImageFrameCPtr"

 

void 
CMLegendView::OnActivateView(
				BOOL 			bActivate, 
				CView* 		pActivateView,
				CView* 		pDeactiveView)
				
{
	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
/*					             
	sprintf(	(char*)&gTextString,
				"OnActivateLegendView %d %s",
				bActivate,
				gEndOfLine);
	OutputString ( NULL, 
					(char*)&gTextString, 
					0, 
					1, 
					TRUE);  
*/	         
	if (bActivate)
		ASSERT(pActivateView == this); 
		
	Boolean changeWindowFlag = 
				(pActivateView == this && pDeactiveView != m_imageViewCPtr );
		
	GetImageFrameCPtr()->ActivateImageWindowItems (bActivate, changeWindowFlag);
	
}		// end "OnActivateView"
  


LRESULT 
CMLegendView::OnDoRealize(
				WPARAM 			wParam, 
				LPARAM 			lParam)

{ 
	LRESULT			returnCode;
	                                                  						
			// If wParam == NULL, then assume that wParam is to equal the
			// m_hWnd parameter for this view.
			// wParam is not used as of 2/27/97.
			
//	if (wParam == NULL)
//		wParam = (WPARAM)m_hWnd; 
/*					             
	sprintf(	(char*)&gTextString,
				"Legend OnDoRealize %s",
				gEndOfLine);
	OutputString ( NULL, 
					(char*)&gTextString, 
					0, 
					1, 
					TRUE);  
*/	
	returnCode = GetImageFrameCPtr()->DoRealize(lParam != 1, this);
/*	
	sprintf(	(char*)&gTextString,
				"%s",
				gEndOfLine);
	OutputString ( NULL, 
					(char*)&gTextString, 
					0, 
					1, 
					TRUE); 
*/
	return returnCode;
	
}		// end "OnDoRealize"
 


void 
CMLegendView::OnInitialUpdate(void)

{   
	if (m_legendListBox.SubclassDlgItem(IDC_List1, this))
		{
		Handle windowInfoHandle = GetWindowInfoHandle (m_imageViewCPtr);
	
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
												windowInfoHandle,
												kNoLock,
												kNoMoveHi);
	
		if (windowInfoPtr != NULL) 
			windowInfoPtr->legendListHandle = &m_legendListBox;
		
		}		// end "if (m_legendListBox.SubclassDlgItem(IDC_List1, this))"
	
			// Subclass the combo box.
				
	m_legendTitleCombo.SubclassDlgItem(IDC_COMBO1, this);
	m_legendPaletteCombo.SubclassDlgItem(IDC_PaletteCombo, this);
	
			// Subclass the scroll down botton.
				
	if ( m_legendScrollDown.SubclassDlgItem(IDC_ScrollDown, this) )
		m_legendScrollDown.SetButtonID(IDC_ScrollDown); 
	
			// Subclass the scroll up botton.
				
	if ( m_legendScrollUp.SubclassDlgItem(IDC_ScrollUp, this) )
		m_legendScrollUp.SetButtonID(IDC_ScrollUp); 
		
			// If the number of groups is 0, then do not allow the Groups
			// option.
						    
	if (m_imageViewCPtr->GetNumberGroups() <= 0)                                  
		((CComboBox*)GetDlgItem(IDC_COMBO1))->DeleteString(kGroupDisplay-1); 
		
	DDX_CBIndex(m_dialogToPtr, IDC_COMBO1, m_classGroupCode);
	                           
	m_legendPaletteCombo.AddString (_T("Palette"));
	m_paletteSelection = m_legendPaletteCombo.GetCount() - 1;                                   
	DDX_CBIndex(m_dialogToPtr, IDC_PaletteCombo, m_paletteSelection);
	
}		// end "OnInitialUpdate" 

  

void 
CMLegendView::OnDropdownClassGroupCombo() 

{     
	if( ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCount() < 3 )  
		{         									
				// Check if group string needs to be added.
				    
		if (m_imageViewCPtr->GetNumberGroups() > 0)
			{                                  
			((CComboBox*)GetDlgItem(IDC_COMBO1))->
												InsertString(kGroupDisplay-1, _T("Groups"));
			if (m_classGroupCode > 0)
				{
				m_classGroupCode++;          
				DDX_CBIndex(m_dialogToPtr, IDC_COMBO1, m_classGroupCode);
				
				}		// end "if (m_classGroupCode > 0)"
				
			}		// end "if (m_imageViewCPtr->GetNumberGroups() > 0)"
														
		}		// end "if( ((CComboBox*)GetDlgItem(IDC_COMBO1))->Count() < 3 )"
		
}		// end "OnDropdownClassGroupCombo"


void CMLegendView::OnDropdownLegendCombo ()

{                                           
	DisplaySpecsPtr 					displaySpecsPtr;
	FileInfoPtr							fileInfoPtr;
		
	Handle 								nameHandle; 
		   
	UInt32 								colorTableOffset,
											numberClasses;
	
	SInt16								ancillaryInfoFormat,
											format,         
											paletteCode;
 	
	
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (
											GetLegendListCPtr ()->m_imageFileInfoHandle);

	if (fileInfoPtr != NULL)
		{								
		nameHandle = fileInfoPtr->classDescriptionH;
		format = fileInfoPtr->format; 
		ancillaryInfoFormat = fileInfoPtr->ancillaryInfoformat;          
		numberClasses = fileInfoPtr->numberClasses;
		colorTableOffset = fileInfoPtr->colorTableOffset;
                                                                        
		displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
															m_imageViewCPtr->GetDisplaySpecsHandle (), 
															kLock);

		if (displaySpecsPtr->classGroupCode == kClassDisplay)
			paletteCode = displaySpecsPtr->thematicClassPaletteType;
			
		else		// ...classGroupCode != kClassDisplay 
			paletteCode = displaySpecsPtr->thematicGroupPaletteType;

		SetUpPalettePopUpMenu ((CMDialog*)this,
										NULL,
										format,
										ancillaryInfoFormat,
										numberClasses,
										nameHandle,
										colorTableOffset,
										displaySpecsPtr,
										displaySpecsPtr->classGroupCode,
										paletteCode); 
			
		CheckAndUnlockHandle (m_imageViewCPtr->GetDisplaySpecsHandle ());                                   
														
		m_paletteSelection = ::GetComboListSelection (this,
																		IDC_PaletteCombo, 
																		paletteCode);
			                                       		
		DDX_CBIndex (m_dialogToPtr, IDC_PaletteCombo, m_paletteSelection);

		}		// end "if (fileInfoPtr != NULL)"

}		// end "OnDropdownLegendCombo" 



void CMLegendView::OnSelendokClassGroup()

{  
	CMImageFrame* imageFrameCPtr = m_imageViewCPtr->GetImageFrameCPtr();
	
	if (imageFrameCPtr != NULL)
		{                                                                    
		DDX_CBIndex(m_dialogFromPtr, IDC_COMBO1, m_classGroupCode);
		
		SInt16 classGroupCode = m_classGroupCode + 1;
		if ( m_imageViewCPtr->GetNumberGroups() <= 0 && m_classGroupCode == 1)
			classGroupCode = kGroupClassDisplay;
	
		imageFrameCPtr-> ChangeClassGroupDisplay (classGroupCode, FALSE);
		
		}		// end "if (imageFrameCPtr != NULL)" 
	
}		// end "OnSelendokClassGroup" 



void CMLegendView::OnSelendokPalette()

{  
   SInt16				lastComboPaletteSelection,
   						lastPaletteSelection,
   						paletteSelection;  
                                                                          
	                                                              
	lastComboPaletteSelection = m_paletteSelection;                                                        
	DDX_CBIndex(m_dialogFromPtr, IDC_PaletteCombo, m_paletteSelection);
	                                         
	paletteSelection = (SInt16)m_legendPaletteCombo.GetItemData(m_paletteSelection);
	
	if (paletteSelection == kFalseColors)
		{
		if ( FalseColorPaletteDialog() )
			m_localPaletteUpToDateFlag  = FALSE;
			
		else		// !FalseColorPaletteDialog()
			{                 
			if (lastComboPaletteSelection != m_paletteSelection)
				{
				m_paletteSelection = lastComboPaletteSelection;                                            
				DDX_CBIndex(m_dialogToPtr, IDC_PaletteCombo, m_paletteSelection);
				
				}		// end "if (lastPaletteSelection != m_paletteSelection)"
					                                          
			paletteSelection = (SInt16)m_legendPaletteCombo.GetItemData(m_paletteSelection);
			
			}		// end "else !FalseColorPaletteDialog()"
			
		}		// end "if (paletteSelection == kFalseColors)" 
	                    
	lastPaletteSelection = (SInt16)m_legendPaletteCombo.GetItemData(lastComboPaletteSelection);
	if (paletteSelection != lastPaletteSelection)
		{
		Handle displaySpecsHandle = m_imageViewCPtr->GetDisplaySpecsHandle();
		DisplaySpecsPtr displaySpecsPtr = 
			(DisplaySpecsPtr)GetHandlePointer(displaySpecsHandle, kNoLock, kNoMoveHi);

		SetPaletteSpecification (		// in SPalette.cpp
					displaySpecsPtr,
					displaySpecsPtr->classGroupCode,
					paletteSelection,
					m_localPaletteUpToDateFlag);

		UpdateActiveImageLegend (displaySpecsPtr->classGroupCode, kCallCreatePalette);

		}		// end "if (paletteSelection != lastPaletteSelection)"
		
			// Add 'Palette' to the end of the combo list and set the combo selection to
			// that item so that Palette will always appear as the 'title' of the combo
			// list.
			
	m_legendPaletteCombo.AddString (_T("Palette"));
	m_paletteSelection = m_legendPaletteCombo.GetCount() - 1;                                   
	DDX_CBIndex(m_dialogToPtr, IDC_PaletteCombo, m_paletteSelection);

}		// end "OnSelendokPalette"

 

void CMLegendView::UpdateClassGroupComboList(
				SInt16		newClassGroupCode)
				
{  
			// Make sure that the combo list is correct.
			
	OnDropdownClassGroupCombo ();
	                                                     
	m_classGroupCode = newClassGroupCode - 1;
	DDX_CBIndex(m_dialogToPtr, IDC_COMBO1, m_classGroupCode);
	
}		// end "UpdateClassGroupComboList"



void CMLegendView::OnSize(UINT nType, int cx, int cy)
{
	Handle						windowInfoHandle;

	SInt16						windowType;


//	CFormView::OnSize(nType, cx, cy);

	windowInfoHandle = GetWindowInfoHandle (m_imageViewCPtr);
	windowType = GetWindowType (windowInfoHandle);
	if (windowType == kImageWindowType)
		{
		cx = 0;
		m_legendWidthSetFlag = TRUE;

		}		// end "if (windowType == kImageWindowType)"

	if (!s_inSetLegendWidthFlag && m_legendWidthSetFlag)
		{                                 
			// Get the image frame pointer
					
		CMImageDoc* imageDocCPtr = (CMImageDoc*)GetDocument();
		CMImageFrame* imageFrameCPtr = imageDocCPtr->GetImageFrameCPtr();
		 
		s_inSetLegendWidthFlag = TRUE;						
		imageFrameCPtr->SetLegendWidth (cx); 
		s_inSetLegendWidthFlag = FALSE;
		
				// Adjust the list to fit as much of the legend as is possible.
				
		AdjustLegendListLength ();
		
		}		// end "if (!s_inSetLegendWidthFlag && m_legendWidthSetFlag)"
	
}		// end "OnSize"  



void CMLegendView::SetLegendWidthSetFlag (void)
{
	m_legendWidthSetFlag = TRUE;
	
}		// end "SetLegendWidthSetFlag" 



void CMLegendView::SetImageView (
				CMImageView*				imageViewCPtr)
{
	m_imageViewCPtr = imageViewCPtr;
	
}		// end "SetImageView"  

 

void 
CMLegendView::OnDraw(
				CDC* 				pDC)
				
{                                                      
	CRect			legendRect;
	SInt16		top;
	
	
	GetClientRect (legendRect); 
	
	top = (SInt16)(legendRect.bottom - legendRect.top - 16);
	
//	#ifdef _WIN32
		pDC->FillSolidRect(legendRect,0x00ffffff);
//	#endif
	
	pDC->MoveTo( legendRect.left, top );
	pDC->LineTo( legendRect.right, top );                                              
	
}		// end "OnDraw" 



void 
CMLegendView::OnLButtonUp(UINT nFlags, CPoint point)

{                           
		           
	CFormView::OnLButtonUp(nFlags, point);
	
}

 

void CMLegendView::OnMouseMove (UINT nFlags, CPoint point)

{                    
//	if (gPresentCursor != kArrow &&
//				gPresentCursor != kWait &&
//						gPresentCursor != kSpin)
//		{
//		::SetCursor(AfxGetApp()->LoadCursor(IDC_ARROW));
//		gPresentCursor = kArrow;
			
//		}		// end "if (gPresentCursor != kArrow && ..."
			                             
	CFormView::OnMouseMove (nFlags, point);
	
}	// end "OnMouseMove" 

 

void CMLegendView::OnPaint (void)

{                                                                 
	CPaintDC dc(this); // device context for painting);                                 
	OnPrepareDC (&dc);                                   
	OnDraw (&dc); 
	
	// Do not call CFormView::OnPaint() for painting messages
	
}		// end "OnPaint" 



void 
CMLegendView::OnScrollDown()

{                     
	if ( ((CMultiSpecApp*)AfxGetApp())->GetZoomCode() != 0 )
		{
		SInt16		itemsInList,
						topIndex;
		
		topIndex = m_legendListBox.GetTopIndex(); 
		itemsInList = m_legendListBox.GetCount(); 
		                                    
		if (topIndex < itemsInList - m_numberDisplayedListItems)
			m_legendListBox.SetTopIndex (topIndex+1); 
			
		}		// end "if ( ...->GetZoomCode() != 0 )"
	
}		// end "OnScrollDown" 



void 
CMLegendView::OnScrollUp()

{                                                  
	if ( ((CMultiSpecApp*)AfxGetApp())->GetZoomCode() != 0 )
		{
		SInt16		topIndex;
		
		topIndex = m_legendListBox.GetTopIndex();
		                   
		if (topIndex > 0)
			m_legendListBox.SetTopIndex (topIndex-1); 
			
		}		// end "if ( ...->GetZoomCode() != 0 )"
	
}		// end "OnScrollUp"


							
void 
CMLegendView::OnUpdate(
				CView*		pSender,
				LPARAM		lHint,
				CObject*		pHint)
				
{  
			// The following is what is in "CView::OnUpdate" except that the
			// argument passed is TRUE.
			
	Invalidate (FALSE); 

}		// end "OnUpdate"
    

                            
void 
CMLegendView::UpdateThematicLegendControls()

{  
	SInt16		itemsInList,
					topIndex;
	
			// Get the number of items in the list.
			
	itemsInList = m_legendListBox.GetCount(); 
	
			// Get the top index 
			
	topIndex = m_legendListBox.GetTopIndex ();
	
			// Scroll Up
			
	if (topIndex > 0)  
		GetDlgItem(IDC_ScrollUp)->EnableWindow(TRUE); 
		
	else		// topIndex == 0                                 
		GetDlgItem(IDC_ScrollUp)->EnableWindow(FALSE);
		
			// Scroll Down    
			
	if (topIndex < itemsInList - m_numberDisplayedListItems)  
		GetDlgItem(IDC_ScrollDown)->EnableWindow(TRUE); 
		
	else		// topIndex >= itemsInList - m_numberDisplayedListItems 
		GetDlgItem(IDC_ScrollDown)->EnableWindow(FALSE);
		
			// Legend Control    
	
	DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
														m_imageViewCPtr->GetDisplaySpecsHandle (),
														kNoLock,
														kNoMoveHi);

	if (displaySpecsPtr->imageDimensions[kVertical] > 0)  
		GetDlgItem(IDC_PaletteCombo)->EnableWindow(TRUE); 
		
	else		// displaySpecsPtr->imageDimensions[kVertical] <= 0 
		GetDlgItem(IDC_PaletteCombo)->EnableWindow(FALSE);
	
}		// end "UpdateThematicLegendButtons" 

 

void CMLegendView::OnDblclkList1()
{
}		// end "OnDblclkList1" 
  


CMLegendList* 
CMLegendView::GetLegendListCPtr (void)

{  
	CMLegendList*		legendListCPtr = NULL;
	
	if (this != NULL)
		legendListCPtr = &m_legendListBox;
		                                     
	return (legendListCPtr);
	
}		// end "GetLegendListCPtr" 

                                 

void CMLegendView::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)

{       
	if (nChar == 0x10)
		m_legendListBox.CheckShiftKeyDown ();
	                                                                                                                                                                                        
	else if (nChar == 0x11)           
		gActiveImageViewCPtr->SetControlKeyFlag (TRUE);
	
	CFormView::OnKeyDown (nChar, nRepCnt, nFlags);
} 



void 
CMLegendView::OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags)

{           
	if (nChar == 0x10)
		m_legendListBox.CheckShiftKeyUp ();
		                                                                                                                                                                            
	else if (nChar == 0x11)           
		gActiveImageViewCPtr->SetControlKeyFlag(FALSE);
	
	CFormView::OnKeyUp(nChar, nRepCnt, nFlags);
	
}		// end "OnKeyUp"



BOOL CMLegendView::OnSetCursor (
				CWnd* pWnd,
				UINT nHitTest,
				UINT message)

{
	if (gPresentCursor == kWait || gPresentCursor == kSpin)
		{
				// Make sure the wait cursor is on. The cursor may have just
				// move over the legend from being outside the window.
		
		//if (nHitTest != HTNOWHERE)
		AfxGetApp ()->DoWaitCursor (0);
																		return (TRUE);

		}	// end "if (gPresentCursor != kArrow && ..."
	
	gPresentCursor = kArrow;
	
	return CFormView::OnSetCursor(pWnd, nHitTest, message); 
	
}		// end "OnSetCursor" 



/////////////////////////////////////////////////////////////////////////////
// CMButton                                                        

BEGIN_MESSAGE_MAP(CMButton, CButton)
	//{{AFX_MSG_MAP(CMButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()   
	//}}AFX_MSG_MAP
END_MESSAGE_MAP() 
                 
                 
/////////////////////////////////////////////////////////////////////////////
// CMButton construction/destruction 

CMButton::CMButton()

{
	m_buttonID = 0;                     

}		// end "CMButton" 



CMButton::~CMButton()
{                                   
	
}		// end "~CMButton"



void 
CMButton::SetButtonID(
				UInt16 			buttonID)

{                                                      
	m_buttonID = buttonID;

}		// end "SetButtonID"

 

/////////////////////////////////////////////////////////////////////////////
// CMButton message handlers 

void 
CMButton::OnLButtonDown(UINT nFlags, CPoint point)

{                                                      
	
	CButton::OnLButtonDown(nFlags, point);
	
	if ( GetState() & 0x0004 )
		{
		((CMultiSpecApp*)AfxGetApp())->SetZoomCode (m_buttonID);                
	          
		SetCapture();     
		
		}		// end "if ( GetState() & 0x0004 )"

}		// end "OnLButtonDown"



void 
CMButton::OnMouseMove(UINT nFlags, CPoint point)

{                                                                  
	CButton::OnMouseMove(nFlags, point);
	                                                     
	UINT	 			itemID; 
	
	
	if ( GetCapture() != this ) 
		itemID = 0; 
		
	else		// GetCapture() == this
		{
		if (GetState() & 0x0004)
			itemID = m_buttonID; 
			
		else		// !(state & 0x0008) 
			itemID = 0;
			
		}		// end "else GetCapture() == this"
			                                            
	((CMultiSpecApp*)AfxGetApp())->SetZoomCode (itemID);                       
	
}		// end "OnMouseMove" 



void CMButton::OnLButtonUp(UINT nFlags, CPoint point)
{  
	if (GetCapture() == this)
		{
		ReleaseCapture ();                                                                
		((CMultiSpecApp*)AfxGetApp())->SetZoomCode (0);      
	((CMultiSpecApp*)AfxGetApp())->SetControlDelayFlag (TRUE);
		
		}		// end "if (GetCapture() == this)"
		
	CButton::OnLButtonUp(nFlags, point); 
		                 
	CMLegendView* legendViewCPtr = gActiveImageViewCPtr->GetImageLegendViewCPtr ();
	legendViewCPtr->UpdateThematicLegendControls ();
	
}		// end "OnLButtonUp" 

                                 

void CMButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{     
	if (nChar == 0x10)
		{                
		CMLegendView* legendViewCPtr = (CMLegendView*)GetParent(); 
		legendViewCPtr->GetLegendListCPtr()->CheckShiftKeyDown ();
		
		}		// end "if (nChar == 0x10)"
	                                                                                                                                                                                        
	else if (nChar == 0x11)           
		gActiveImageViewCPtr->SetControlKeyFlag(TRUE);
	
	CButton::OnKeyDown(nChar, nRepCnt, nFlags);
	
}		// end "OnKeyDown"



void CMButton::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{    
	if (nChar == 0x10)
		{                
		CMLegendView* legendViewCPtr = (CMLegendView*)GetParent(); 
		legendViewCPtr->GetLegendListCPtr()->CheckShiftKeyUp ();
		
		}		// end "if (nChar == 0x10)"  
	                                                                                                                                                                                        
	else if (nChar == 0x11)           
		gActiveImageViewCPtr->SetControlKeyFlag(FALSE);
	
	CButton::OnKeyUp(nChar, nRepCnt, nFlags);
	
}		// end "OnKeyUp" 

void 
CMButton::OnRButtonDown(UINT nFlags, CPoint point)

{                                                      
	
	CButton::OnRButtonDown(nFlags, point);
	                           
	if (GetCapture() == this)
		((CMultiSpecApp*)AfxGetApp())->SetControlDelayFlag (FALSE); 

}		// end "OnRButtonDown" 



void CMButton::OnRButtonUp(UINT nFlags, CPoint point)
{  
	if (GetCapture() == this)                                                            
		((CMultiSpecApp*)AfxGetApp())->SetControlDelayFlag (TRUE); 
		
	CButton::OnRButtonUp(nFlags, point);
	
}		// end "OnRButtonUp" 



/////////////////////////////////////////////////////////////////////////////
// CMComboBox                                                        

BEGIN_MESSAGE_MAP(CMComboBox, CComboBox)
	//{{AFX_MSG_MAP(CMComboBox)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()   
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP() 
                 
                 
/////////////////////////////////////////////////////////////////////////////
// CMButton construction/destruction 

CMComboBox::CMComboBox()

{                                       

}		// end "CMComboBox" 



CMComboBox::~CMComboBox()
{                                   
	
}		// end "~CMButton"  

 

void CMComboBox::DrawLegendTitle ()
{	
	DRAWITEMSTRUCT 			drawItemStruct; 
	                              
	
	OnDrawItem(1, &drawItemStruct);

}		// end "DrawLegendTitle"

 

/////////////////////////////////////////////////////////////////////////////
// CMComboBox message handlers   

void 
CMComboBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)

{    
	CMImageView*				imageViewCPtr;
	CMLegendView*				legendViewCPtr;
	Boolean						callComboOnKeyDownFlag = true;


	legendViewCPtr = (CMLegendView*)GetParent();
	imageViewCPtr = legendViewCPtr->GetImageView();

	if (nChar == 0x10)       
		{                 
		legendViewCPtr->GetLegendListCPtr()->CheckShiftKeyDown ();
		
		}		// end "if (nChar == 0x10)"    
	                                                                                                                                                                                        
	else if (nChar == 0x11)           
		gActiveImageViewCPtr->SetControlKeyFlag(TRUE);

	else if (nChar == 0x25 || nChar == 0x26 || nChar == 0x27 || nChar == 0x28)
		{
		DoNextDisplayChannelEvent (imageViewCPtr, nChar);
		callComboOnKeyDownFlag = false;

		}		// end "else if (nChar == 0x25 || nChar == 0x26 || ..."
	
	if (callComboOnKeyDownFlag)
		CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
	
}		// end "OnKeyDown"



void 
CMComboBox::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)

{     
	if (nChar == 0x10)
		{                
		CMLegendView* legendViewCPtr = (CMLegendView*)GetParent(); 
		legendViewCPtr->GetLegendListCPtr()->CheckShiftKeyUp ();
		
		}		// end "if (nChar == 0x10)"   
	                                                                                                                                                                                        
	else if (nChar == 0x11)           
		gActiveImageViewCPtr->SetControlKeyFlag(FALSE);
	
	CComboBox::OnKeyUp(nChar, nRepCnt, nFlags);
	
}		// end "OnKeyUp"

 

void CMComboBox::OnDrawItem(int /*nIDCtl*/, LPDRAWITEMSTRUCT lpDrawItemStruct)
{  
	SInt16			legendWidth;
	

	CMLegendView* legendViewCPtr = gActiveImageViewCPtr->GetImageLegendViewCPtr();

	CString	nameStr; 

	legendViewCPtr->m_legendTitleCombo.GetLBText (this->GetCurSel(), nameStr);  

	CDC* pDC = gActiveImageViewCPtr->m_pDC;

	CSize size = pDC->GetTextExtent (nameStr, nameStr.GetLength());
   
   legendWidth = gActiveImageViewCPtr->GetLegendWidth ();
	int left = (int)(legendWidth * gActiveImageViewCPtr->m_printerTextScaling - size.cx) /2;
	left = MAX(0,left);

	pDC->SetTextColor (0x00000000);
	pDC->TextOut (left, 10, nameStr);

}		// end "OnDrawItem"


void CMLegendView::OnSetFocus(CWnd* pOldWnd)
{                            
			// These settings will change only when
			// there is no active processing funtion running.
	                                		
	if (gProcessorCode == 0)
		GetImageFrameCPtr()->UpdateActiveImageWindowInfo();
		
	CFormView::OnSetFocus(pOldWnd);
	
}



BOOL CMLegendView::OnEraseBkgnd(CDC* pDC) 
{                             
	CRect			legendRect;


	GetClientRect (legendRect); 
	
	CBrush brush( GetSysColor( COLOR_WINDOW ) );

	pDC->FillRect(legendRect, &brush); 
	                                          
//   FillOutsideRect( pDC, &brush );
   return TRUE;                   // Erased
	
//	return CFormView::OnEraseBkgnd(pDC);
}



void CMLegendView::OnSelendcancelPaletteCombo() 
{

			// Add 'Palette' to the end of the combo list and set the combo selection to
			// that item so that Palette will always appear as the 'title' of the combo
			// list.
	
	m_paletteSelection = 
				m_legendPaletteCombo.FindStringExact (-1, _T("Palette"));

	if (m_paletteSelection == CB_ERR)
		{
		m_legendPaletteCombo.AddString (_T("Palette"));
		m_paletteSelection = m_legendPaletteCombo.GetCount() - 1;                                   
		DDX_CBIndex(m_dialogToPtr, IDC_PaletteCombo, m_paletteSelection);

		}		// end "if (m_paletteSelection == CB_ERR)"

	else		// m_paletteSelection != CB_ERR
		DDX_CBIndex(m_dialogToPtr, IDC_PaletteCombo, m_paletteSelection);
	
}		// end "OnSelendcancelPaletteCombo"
