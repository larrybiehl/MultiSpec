// WCoordinateBar.cpp : implementation file
//
// Revised by Larry Biehl on 08/30/2018
                    
#include "SMultiSpec.h"
                     
#include "WImageView.h" 
#include "WCoordinateBar.h" 
 
//#include	"SExtGlob.h"

/////////////////////////////////////////////////////////////////////////////
// CMCoordinateBar
                                                

CMCoordinateBar::CMCoordinateBar()
{
	m_dialogFromPtr = NULL;
	m_dialogToPtr = NULL;
	m_areaUnitsCode = kNumberPixelsUnitsMenuItem;
	m_displayUnitsCode = kLineColumnUnitsMenuItem;
	m_displayUnitsListSelection = 0;
	m_areaUnitsListSelection = 0;

	TRY
		{
		m_dialogFromPtr = new CDataExchange(this, TRUE);
		m_dialogToPtr = new CDataExchange(this, FALSE);

		}

	CATCH_ALL(e)
		{
		MemoryMessage(0, kObjectMessage);

		}

	END_CATCH_ALL

}



CMCoordinateBar::~CMCoordinateBar()
{
	if (m_dialogToPtr != NULL)
		delete m_dialogToPtr;

	if (m_dialogFromPtr != NULL)
		delete m_dialogFromPtr;

}		// end "~CMCoordinateBar()" 



void CMCoordinateBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMEchoClassifyDialog)
	DDX_CBIndex(pDX, IDC_AreaUnitsCombo, m_areaUnitsListSelection);
	DDX_CBIndex(pDX, IDC_DisplayUnitsCombo, m_displayUnitsListSelection);
	//}}AFX_DATA_MAP
}



BOOL CMCoordinateBar::OnSetCursor(
				CWnd* 		pWnd, 
				UINT 			nHitTest, 
				UINT 			message)
			
{                                    
	if (gPresentCursor == kWait || gPresentCursor == kSpin)
		{
				// Make sure the wait cursor is on. The cursor may have just
				// move over the legend from being outside the window.
		
		AfxGetApp ()->DoWaitCursor (0);
																		return (TRUE);

		}	// end "if (gPresentCursor != kArrow && ..."

	if (gPresentCursor != kArrow)
		{
		if (gActiveImageViewCPtr != NULL)                
			gActiveImageViewCPtr->UpdateCursorCoordinates();
		                                                                
		gPresentCursor = kArrow; 		// Non image window cursor
		
		}		// end "if (gPresentCursor != kArrow)"  
		
	return CDialogBar::OnSetCursor(pWnd, nHitTest, message); 
	
}		// end "OnSetCursor" 



BEGIN_MESSAGE_MAP(CMCoordinateBar, CDialogBar)
	//{{AFX_MSG_MAP(CMCoordinateBar) 
	ON_WM_SETCURSOR()
	ON_CBN_SELENDOK(IDC_AreaUnitsCombo, OnSelendokAreaUnits)
	ON_CBN_SELENDOK(IDC_DisplayUnitsCombo, OnSelendokDisplayUnits)
	ON_CBN_DROPDOWN(IDC_AreaUnitsCombo, OnDropdownAreaUnitsCombo)
	ON_CBN_CLOSEUP(IDC_AreaUnitsCombo, OnCloseupAreaUnitsCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void 
CMCoordinateBar::OnDraw(
				CDC* 				pDC)
	
{
	SInt16		test;


	test = 1;

}		// end "OnDraw"



void CMCoordinateBar::OnPaint()
{                   
//	Boolean			returnFlag;
	RECT				updateRect; 
	
	
	if ( GetUpdateRect(&updateRect) )
		{ 
			
		CPaintDC dc(this); // device context for painting 
//		OnPrepareDC(&dc);
		OnDraw(&dc); 
		
		}		// end "	if ( GetUpdateRect(&updateRect) )"    

	//already
	// Do not call CScrollView::OnPaint() for painting messages
}                 


							
void 
CMCoordinateBar::OnUpdate(
				CView*		pSender,
				LPARAM		lHint,
				CObject*		pHint)
				
{  
			// The following is what is in "CView::OnUpdate" except that the
			// argument passed is TRUE.
/*					             
	sprintf(	(char*)&gTextString,
				"Image Invalidate%s",
				gEndOfLine);
	OutputString ( NULL, 
					(char*)&gTextString, 
					0, 
					1, 
					TRUE);
*/			
	Invalidate (FALSE);

}		// end "OnUpdate"  



void CMCoordinateBar::OnSelendokAreaUnits()

{  
	CComboBox* 			comboBoxPtr;
	
	
	DDX_CBIndex(m_dialogFromPtr, 
					IDC_AreaUnitsCombo, 
					m_areaUnitsListSelection); 
					
			// Get the actual display units code.
			
	comboBoxPtr = (CComboBox*)GetDlgItem(IDC_AreaUnitsCombo);
						 
	m_areaUnitsCode = 
				(SInt16)comboBoxPtr->GetItemData(m_areaUnitsListSelection);

	if (m_areaUnitsCode != GetCoordinateViewAreaUnits(gActiveImageWindowInfoH))
		{
		SetCoordinateViewAreaUnits (gActiveImageWindowInfoH, m_areaUnitsCode);

		UpdateCoordinateView (gActiveImageWindowInfoH);

		}		// end "if (m_displayUnitsCode != GetCoordinateViewUnits())"
	
}		// end "OnSelendokAreaUnits"  



void CMCoordinateBar::OnSelendokDisplayUnits()

{  
	CComboBox* 			comboBoxPtr;
	
	
	DDX_CBIndex(m_dialogFromPtr, 
					IDC_DisplayUnitsCombo, 
					m_displayUnitsListSelection); 
					
			// Get the actual display units code.
			
	comboBoxPtr = (CComboBox*)GetDlgItem(IDC_DisplayUnitsCombo);
						 
	m_displayUnitsCode = 
				(SInt16)comboBoxPtr->GetItemData(m_displayUnitsListSelection);

	if (m_displayUnitsCode != GetCoordinateViewUnits(gActiveImageWindowInfoH))
		{
		SetCoordinateViewUnits (gActiveImageWindowInfoH, m_displayUnitsCode);

		UpdateCoordinateView (gActiveImageWindowInfoH);

		}		// end "if (m_displayUnitsCode != GetCoordinateViewUnits())"
	
}		// end "OnSelendokDisplayUnits"  



void 
CMCoordinateBar::UpdateCoordinateView(
				Handle								windowInfoHandle)
				
{
	RECT					itemRect,
							rect;

	double				maxScale;

	CComboBox* 			comboBoxPtr;

	UInt32				width;

	SInt16				numberChars;


		SetCoordinateViewLocationParameters (windowInfoHandle);
/*
				// Adjust location of map unit list.

		comboBoxPtr = (CComboBox*)GetDlgItem(IDC_DisplayUnitsCombo);
		comboBoxPtr->GetClientRect (&itemRect);
		itemRect.top = 6;
		itemRect.bottom = 28;
		comboBoxPtr->MoveWindow(&itemRect, FALSE);
*/
				// Adjust location of line symbol.

		comboBoxPtr = (CComboBox*)GetDlgItem(IDC_LineSymbol);
		comboBoxPtr->GetClientRect (&itemRect);
		itemRect.top = 4;
		itemRect.bottom = 16;
		itemRect.left = GetCoordinateViewCursorStart(windowInfoHandle) - 15;
		itemRect.right = itemRect.left + 12;
		comboBoxPtr->MoveWindow(&itemRect, FALSE);

				// Adjust location of column symbol.

		comboBoxPtr = (CComboBox*)GetDlgItem(IDC_ColumnSymbol);
		itemRect.top = 18;
		itemRect.bottom = 30;
		comboBoxPtr->MoveWindow(&itemRect, FALSE);

				// Adjust size of cursor line box.

		comboBoxPtr = (CComboBox*)GetDlgItem(IDC_CursorLine);
		width = GetCoordinateViewSelectionStart(windowInfoHandle) -
						GetCoordinateViewCursorStart(windowInfoHandle) -
						10;
		itemRect.top = 4;
		itemRect.bottom = 16;
		itemRect.left = GetCoordinateViewCursorStart(windowInfoHandle);
		itemRect.right = itemRect.left + width;
		comboBoxPtr->MoveWindow(&itemRect, FALSE);

				// Adjust size of cursor column box.

		comboBoxPtr = (CComboBox*)GetDlgItem(IDC_CursorColumn);
		itemRect.top = 18;
		itemRect.bottom = 30;
		comboBoxPtr->MoveWindow(&itemRect, FALSE);

				// Move separator box 1.

		comboBoxPtr = (CComboBox*)GetDlgItem(IDC_Separator1);
		itemRect.top = 5;
		itemRect.bottom = 30;
		itemRect.left = itemRect.right + 4;
		itemRect.right = itemRect.left + 3;
		comboBoxPtr->MoveWindow(&itemRect, FALSE);

				// Adjust size of selection line box.

		comboBoxPtr = (CComboBox*)GetDlgItem(IDC_SelectionLine);
		width = GetCoordinateViewNumberPixelsStart(windowInfoHandle) -
						GetCoordinateViewSelectionStart(windowInfoHandle) -
						10;
		itemRect.top = 4;
		itemRect.bottom = 16;
		itemRect.left = GetCoordinateViewSelectionStart(windowInfoHandle);
		itemRect.right = itemRect.left + width;
		comboBoxPtr->MoveWindow(&itemRect, FALSE);

				// Adjust size of selection column box.

		comboBoxPtr = (CComboBox*)GetDlgItem(IDC_SelectionColumn);
		itemRect.top = 18;
		itemRect.bottom = 30;
		comboBoxPtr->MoveWindow(&itemRect, FALSE);

				// Adjust size and location of "number pixels box" box.

		comboBoxPtr = (CComboBox*)GetDlgItem(IDC_NumberPixelsPrompt);
		width = GetCoordinateViewAreaPopupStart(windowInfoHandle) -
						GetCoordinateViewNumberPixelsStart(windowInfoHandle) -
						4;
		itemRect.top = 4;
		itemRect.bottom = 16;
		itemRect.left = GetCoordinateViewNumberPixelsStart(windowInfoHandle);
		itemRect.right = itemRect.left + width;
		comboBoxPtr->MoveWindow(&itemRect, FALSE);

				// Adjust size and location of area box.

		comboBoxPtr = (CComboBox*)GetDlgItem(IDC_NumberPixels);
		itemRect.top = 18;
		itemRect.bottom = 30;
		comboBoxPtr->MoveWindow(&itemRect, FALSE);

				// Move area units popup box.

		comboBoxPtr = (CComboBox*)GetDlgItem(IDC_AreaUnitsCombo);
		comboBoxPtr->GetClientRect (&rect);
		itemRect.top = 5;
		itemRect.bottom = 200;
		itemRect.left = GetCoordinateViewScaleStart(windowInfoHandle) - rect.right - 8;
		itemRect.right = itemRect.left + rect.right;
		comboBoxPtr->MoveWindow(&itemRect, FALSE);
		comboBoxPtr->SetDroppedWidth (100);

				// Move separator box 2.

		comboBoxPtr = (CComboBox*)GetDlgItem(IDC_Separator2);
		itemRect.top = 5;
		itemRect.bottom = 30;
		itemRect.left = GetCoordinateViewScaleStart(windowInfoHandle) - 5;
		itemRect.right = itemRect.left + 3;
		comboBoxPtr->MoveWindow(&itemRect, FALSE);

				// Adjust size and location of "scale" box.
				//		First, get width of maximum possible scale value.

		maxScale = GetScale (windowInfoHandle,
										GetMaxNumberColumns (windowInfoHandle),
										gMinMagnification);
			
		if (maxScale >= 1)
			{						
			numberChars = sprintf( (char*)&gTextString2, 
											"%.0f",  
											maxScale);

			numberChars = InsertCommasInNumberString (
										(char*)&gTextString2, 
										numberChars, 
										-1,
										numberChars);
										
			}		// end "if (scale >= 1)"
			
		else		// maxScale < 1
			{						
			numberChars = sprintf( (char*)&gTextString2, 
											"%f",  
											maxScale);
										
			}		// end "else scale < 1"
			
		numberChars = sprintf( (char*)&gTextString, 
										"1:%*s",  
										-17,
										(char*)gTextString2);
										
		width = TextWidth(gTextString, 0, numberChars) + 10;
		width = MAX (50, width);

		comboBoxPtr = (CComboBox*)GetDlgItem(IDC_ScalePrompt);
		itemRect.top = 4;
		itemRect.bottom = 16;
		itemRect.left = GetCoordinateViewScaleStart(windowInfoHandle);
		itemRect.right = itemRect.left + width;
		comboBoxPtr->MoveWindow(&itemRect, FALSE);

				// Adjust size and location of scale value box.

		comboBoxPtr = (CComboBox*)GetDlgItem(IDC_Scale);
		itemRect.top = 18;
		itemRect.bottom = 30;
		comboBoxPtr->MoveWindow(&itemRect, FALSE);

		Invalidate();

		UpdateSelectionCoordinates (windowInfoHandle);
		ShowSelection (windowInfoHandle); 
	
}		// end "UpdateCoordinateView"  



void 
CMCoordinateBar::OnCloseupAreaUnitsCombo()

{  
 /*
 RECT				itemRect,
						rect;

	CComboBox*		comboBoxPtr;

			// Change size of area units popup box.

	comboBoxPtr = (CComboBox*)GetDlgItem(IDC_AreaUnitsCombo);
	comboBoxPtr->GetClientRect (&rect);
	itemRect.top = 8;
	itemRect.bottom = 27;
	itemRect.left = GetCoordinateViewScaleStart(gActiveImageWindowInfoH) - 
											20 - 8;
	itemRect.right = itemRect.left + 20;
	comboBoxPtr->MoveWindow(&itemRect, FALSE);
*/		
//	m_displayUnitsCode = GetCoordinateViewUnits (gActiveImageWindowInfoH);					                             
//	DDX_CBIndex(m_dialogToPtr, IDC_DisplayUnitsCombo, m_displayUnitsCode);
	
}		// end "OnCloseupAreaUnitsCombo"     



void 
CMCoordinateBar::OnDropdownAreaUnitsCombo()

{  
//   RECT				itemRect,
//						rect;
/*
	CComboBox*		comboBoxPtr;


			// Change size of area units popup box.

	comboBoxPtr = (CComboBox*)GetDlgItem(IDC_AreaUnitsCombo);
	comboBoxPtr->SetDroppedWidth (100);
/*
	comboBoxPtr->GetClientRect (&rect);
	itemRect.top = 8;
	itemRect.bottom = 27;
	itemRect.left = GetCoordinateViewScaleStart(gActiveImageWindowInfoH) - 
											80 - 8;
	itemRect.right = itemRect.left + 80;
	comboBoxPtr->MoveWindow(&itemRect, FALSE);
*/		
//	m_displayUnitsCode = GetCoordinateViewUnits (gActiveImageWindowInfoH);					                             
//	DDX_CBIndex(m_dialogToPtr, IDC_DisplayUnitsCombo, m_displayUnitsCode);
	
}		// end "OnDropdownAreaUnitsCombo"                


