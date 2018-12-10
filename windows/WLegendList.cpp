// WLegendList.cpp : implementation file
//
// Revised by Larry Biehl on 12/21/2017
//                    
                    
#include "SMultiSpec.h"       

#include "WImageView.h"
#include "WImageDoc.h"  
#include "WImageFrame.h" 
#include "WLegendList.h" 
#include "CPalette.h" 

//#include	"SExtGlob.h" 

extern Boolean 	DoBlinkCursor1 (
							ListHandle				legendListHandle, 
							Point						lCell, 
							SInt16					listType,
							SInt16					code);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif 
          
									
CPoint				CMLegendList::s_lastMouseDnPoint;
RECT					CMLegendList::s_dragRect;
Handle				CMLegendList::s_legendBitMapInfo = NULL; 
int					CMLegendList::s_lastVerticalPoint = 0;  
int					CMLegendList::s_listBottom = 0;
Boolean				CMLegendList::s_draggingFlag = FALSE; 
Boolean				CMLegendList::s_grayRectDisplayedFlag = FALSE; 
Boolean				CMLegendList::s_isPrintingFlag = FALSE;


/////////////////////////////////////////////////////////////////////////////
// CMLegendList 

BEGIN_MESSAGE_MAP(CMLegendList, CListBox)
	//{{AFX_MSG_MAP(CMLegendList) 
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMListBox message handlers

 

CMLegendList::CMLegendList()

{                    
	//{{AFX_DATA_INIT(CMLegendList)     
	//}}AFX_DATA_INIT 
			// These are initialize in LoadThematicLegendList in SThemWin.cpp.
			
	m_paletteObject = NULL;
	m_backgroundPaletteObject = NULL;
	m_imageFileInfoHandle = NULL;
	m_bitMapInfoHeaderHandle = NULL;
	m_listType = 0;
	m_paletteOffset = 0;           
	m_classPaletteEntries = 0;
	m_activeFlag = FALSE; 
	m_shiftKeyDownFlag = FALSE;
	s_lastMouseDnPoint.x = 0; 
	s_lastMouseDnPoint.y = 0;        
	
	if (s_legendBitMapInfo == NULL)
		{
		UInt32 bytesNeeded = sizeof(BITMAPINFO) + sizeof(UInt8);
		s_legendBitMapInfo = MNewHandle (bytesNeeded);
		
		if (s_legendBitMapInfo != NULL)
			{
			LegendBitMapInfo* legendBitMapInfoPtr = 
									(LegendBitMapInfo*)GetHandlePointer(
													s_legendBitMapInfo, kNoLock, kNoMoveHi);
			legendBitMapInfoPtr->bmiHeader.biSize = 40;
			legendBitMapInfoPtr->bmiHeader.biWidth = 1;
			legendBitMapInfoPtr->bmiHeader.biHeight = 1;
			legendBitMapInfoPtr->bmiHeader.biPlanes = 1; 
			legendBitMapInfoPtr->bmiHeader.biBitCount = 8;
			legendBitMapInfoPtr->bmiHeader.biCompression = 0;
			legendBitMapInfoPtr->bmiHeader.biSizeImage = 0;  
			legendBitMapInfoPtr->bmiHeader.biXPelsPerMeter = 0;
			legendBitMapInfoPtr->bmiHeader.biYPelsPerMeter = 0;
			legendBitMapInfoPtr->bmiHeader.biClrUsed = 1;
			legendBitMapInfoPtr->bmiHeader.biClrImportant = 0;
			
			legendBitMapInfoPtr->bmiColors[0] = 0;
			
			legendBitMapInfoPtr->paletteIndex = 0;
			
			}		// end "if (s_legendBitMapInfo != NULL)"   
		
		}		// end "if (s_legendBitMapInfo != NULL)"		
	
}		// end "CMLegendList" 


CMLegendList::~CMLegendList()
{

}


void CMLegendList::CheckShiftKeyDown (void)

{   
	POINT		position;
	RECT		rectangle;
		
	m_shiftKeyDownFlag = TRUE;
	GetCursorPos (&position);
	ScreenToClient (&position);
	GetClientRect (&rectangle);
	if (m_activeFlag &&
			position.x <= 14 && position.x > 0 && 
			position.y <= rectangle.bottom && position.y >= 0 &&
			gPresentCursor != kBlinkOpenCursor1)
		MSetCursor (kBlinkOpenCursor1);
	
}		// end "CheckShiftKeyDown"


void CMLegendList::CheckShiftKeyUp (void)

{                      
	m_shiftKeyDownFlag = FALSE;
	if (gPresentCursor == kBlinkOpenCursor1)
		MSetCursor (kArrow); 
	
}		// end "CheckShiftKeyUp"


void CMLegendList::DrawItem (
				LPDRAWITEMSTRUCT 			lpDrawItemStruct)
{  
	USES_CONVERSION;

	if (m_classPaletteEntries <= 0 || m_bitMapInfoHeaderHandle == NULL)
																					return;
																					
	GetDrawItemStruct(lpDrawItemStruct);

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);  

	if (lpDrawItemStruct->itemAction & ODA_DRAWENTIRE)
		{
		RECT				colorRect;
		
		char*				namePtr; 
		Handle			nameHandle;
		
		SInt32			paletteIndex;
		
		SInt16			classGroupCode,
							index;
							
		UInt8				bytePaletteIndex;
		
		
		if (lpDrawItemStruct->itemData > 0x0000ffff)
																		return;
		
//		DisplaySpecsPtr				displaySpecsPtr; 
		double 							magnification; 
		

//		Handle displaySpecsHandle = gActiveImageViewCPtr->GetDisplaySpecsHandle ();

//		displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(
//												displaySpecsHandle, kNoLock, kNoMoveHi);
		                     
		magnification = 1.0;                   
		if (s_isPrintingFlag)                              
			magnification = gActiveImageViewCPtr->m_printerTextScaling;

		colorRect.top = lpDrawItemStruct->rcItem.top + 3;
		colorRect.left = lpDrawItemStruct->rcItem.left;
		colorRect.bottom = lpDrawItemStruct->rcItem.bottom - 3;
		colorRect.right = lpDrawItemStruct->rcItem.left + 15;
		
				// Now get the class-group tag (sign bit of 16 bit number)
				//  if == 0 then class index, if == 1 then group index
				// and the class/group index.
		                                              
		index = (SInt16)lpDrawItemStruct->itemData; 
		classGroupCode = (index & 0x8000);
		index &= 0x7fff; 
		
				// Now get the correct handle to the list names to be used.
				
		FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (
												m_imageFileInfoHandle, kNoLock, kNoMoveHi);
												
		if (classGroupCode == 0)
			nameHandle =  fileInfoPtr->classDescriptionH;
			
		else		// classGroupCode != 0    
			nameHandle = fileInfoPtr->groupNameHandle;
			
		if ( m_listType != kGroupClassDisplay || classGroupCode != 0)
			{                
					// Get the index to the correct palette entry.
					
			if (classGroupCode == 0)
						// Class name
				paletteIndex = lpDrawItemStruct->itemID;
				
			else		// classGroupCode != 0
				{
						// Group name
				SInt16* classGroupToPalettePtr = (SInt16*)GetHandlePointer(
								fileInfoPtr->groupTablesHandle, kNoLock, kNoMoveHi);
				paletteIndex = classGroupToPalettePtr[index];
				
				}		// end "else classGroupCode != 0"
				               
			UInt16 paletteOffset = m_paletteOffset;                                      
			if (paletteIndex >= m_classPaletteEntries)
				{                        
				UInt16 paletteEntries = (UInt16)MIN(m_classPaletteEntries, 254);
				paletteIndex = (paletteIndex - (1-paletteOffset)) % paletteEntries;
				paletteOffset = 1;
				
				}		// end "if (paletteIndex >= m_classPaletteEntries)" 

			bytePaletteIndex = (UInt8)(paletteIndex+paletteOffset);
			LPSTR legendChipDIBBitsPtr = (LPSTR)&bytePaletteIndex;
		 
			BITMAPINFOHEADER* legendBitMapInfoPtr  = 
									(BITMAPINFOHEADER*)::GlobalLock(m_bitMapInfoHeaderHandle);
									
					// Save image offscreen bit map and set legend chip bit map to 
					// represent 1 pixel.
					
			SInt32 savedBiWidth = legendBitMapInfoPtr->biWidth;		  
			SInt32 savedBiHeight = legendBitMapInfoPtr->biHeight;
			legendBitMapInfoPtr->biWidth = 1;		  
			legendBitMapInfoPtr->biHeight = 1;
                              
			CPalette* newPalette = m_paletteObject;
			if (!m_activeFlag)
				newPalette = m_backgroundPaletteObject;	   
//			hPal = (HPALETTE)m_paletteObject->m_hObject; 
//			hOldPal = ::SelectPalette(pDC->m_hDC, hPal, TRUE); 

			CPalette* oldPalette = pDC->SelectPalette(newPalette, TRUE);   
			
		  	::StretchDIBits(
		  				pDC->GetSafeHdc(),             		// hDC
					   colorRect.left,           				// DestX
					   colorRect.top,            				// DestY
					   (int)(RECTWIDTH(&colorRect)*magnification),  // nDestWidth
					   (int)(RECTHEIGHT(&colorRect)*magnification), // nDestHeight 
					   0,                						// SrcX
					   0,   											// SrcY
					   1,         									// wSrcWidth
					   1,        									// wSrcHeight
					   legendChipDIBBitsPtr,            	// lpBits
					   (LPBITMAPINFO)legendBitMapInfoPtr,  // lpBitsInfo
					   DIB_PAL_COLORS,                 		// wUsage
					   SRCCOPY);       							// dwROP
			
					// Restore width and height for image offscreen bit map.
										   
			legendBitMapInfoPtr->biWidth = savedBiWidth;		  
			legendBitMapInfoPtr->biHeight = savedBiHeight;
								   
	   	::GlobalUnlock( (HGLOBAL)m_bitMapInfoHeaderHandle ); 
	   	                          
			if (oldPalette != NULL)
				pDC->SelectPalette(oldPalette, TRUE);
			
			}		// end "if ( m_listType != kGroupClassDisplay || ..."
			
				// Now draw the text.
				
		namePtr = (char*)GetHandlePointer (nameHandle, kLock, kNoMoveHi);
		namePtr = &namePtr[index*32];
				
		if (m_listType == kGroupClassDisplay && classGroupCode == 0)
			{
			SInt16		stringLength;
					
					// Add class number of class name in group-class display
					
			sprintf( (char*)&gTextString[1],
							"%3d-",
							index+1);
			pstr( (char*)&gTextString[5],
							namePtr,
							&stringLength);
							
			namePtr = (char*)gTextString;
			namePtr[0] = stringLength + 4;
			
			}		// end "if (m_listType == kGroupClassDisplay && ..."
		                                           
		pDC->SetTextColor (0x00000000);
		pDC->TextOut ((int)(colorRect.right + 15 * magnification), 
							(int)(colorRect.top + magnification), 
							(LPCTSTR)A2T(&namePtr[1]),
							(SInt16)namePtr[0]);
			
		CheckAndUnlockHandle (nameHandle);
		
		}		// end "if (lpDrawItemStruct->itemAction & ODA_DRAWENTIRE)"

}		// end "DrawItem"
 

void CMLegendList::DrawLegendList ()
{	
	DRAWITEMSTRUCT 			drawItemStruct; 
	                             

	OnDrawItem(1, &drawItemStruct);

}		// end "DrawLegendList"


Handle CMLegendList::GetBitMapInfoHeaderHandle()
{                                                                  
	return m_bitMapInfoHeaderHandle;  
	
}		// end "GetBitMapInfoHeaderHandle"
 

void CMLegendList::GetDrawItemStruct (LPDRAWITEMSTRUCT lpDrawItemStruct)

{
	m_drawItem = *lpDrawItemStruct;
	
}		// end "GetDrawItemStruct"


void CMLegendList::GetTextValue (
				SInt16								selectedCell,
				UInt16*								valuePtr)

{
	char*									returnStringPtr;
	SInt32								stringLength;
	UInt32								longClassValue;
	

//#	if defined multispec_win_unicode
		returnStringPtr = (char*)gWideTextString;
//#	else
//		returnStringPtr = (char*)gTextString;
//#	endif

 	stringLength = GetText (selectedCell, (LPTSTR)returnStringPtr);
	
//#	if defined multispec_win_unicode
		wcstombs ((char*)gTextString, (TBYTE*)returnStringPtr, 254);
		returnStringPtr = (char*)gTextString;
//#	endif

	BlockMoveData (&gTextString, &longClassValue, 4);
	*valuePtr = (UInt16)longClassValue;
	
}		// end "GetTextValue"


CPoint CMLegendList::LastClickPoint(void)

{                                                                  
	return (s_lastMouseDnPoint);  
	
}		// end "LastClickPoint"

  

void CMLegendList::MeasureItem(
				LPMEASUREITEMSTRUCT 			lpMeasureItemStruct)
{
			// all items are of fixed size
			// must use LBS_OWNERDRAWVARIABLE for this to work 
			
	lpMeasureItemStruct->itemHeight = 20;
	
}		// end "MeasureItem" 


void CMLegendList::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	double							magnification;
	CRect								legendRect;


   lpDrawItemStruct = &m_drawItem;

   HDC temp = lpDrawItemStruct->hDC;
	                                                            
	lpDrawItemStruct->hDC = gActiveImageViewCPtr->m_pDC->GetSafeHdc();
	lpDrawItemStruct->itemAction = ODA_DRAWENTIRE;

	s_isPrintingFlag = gActiveImageViewCPtr->m_pDC->IsPrinting();
	                               
	magnification = 1.0;                   
	if (s_isPrintingFlag)                              
		magnification = gActiveImageViewCPtr->m_printerTextScaling;
		
	lpDrawItemStruct->rcItem.left = 0;

   for (int count=0;count<GetCount();count++)
		{
		lpDrawItemStruct->itemData = GetItemData(count);
		lpDrawItemStruct->itemID = count; 
		
		lpDrawItemStruct->rcItem.top = 
			(int)(30 * magnification + GetItemHeight(count) * count * magnification);
		lpDrawItemStruct->rcItem.bottom = 
			(int)(30 * magnification + GetItemHeight(count) * (1 + count * magnification));
		DrawItem(lpDrawItemStruct);

		}		// end "for (int count=0;count<GetCount();count++)"

	lpDrawItemStruct->hDC = temp;

	s_isPrintingFlag = FALSE;

}		// end "OnDrawItem"


void CMLegendList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{                                                                                                                                                                       
	if (nChar == 0x11)           
		gActiveImageViewCPtr->SetControlKeyFlag (TRUE);
		
	else if (nChar == 0x10)
		CheckShiftKeyDown ();

	else if (nChar == 0x25 || nChar == 0x26 || nChar == 0x27 || nChar == 0x28)
		DoNextDisplayChannelEvent (gActiveImageViewCPtr, nChar);
	
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
	
}		// end "OnKeyDown"


void CMLegendList::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{                                                                                                                                                                                               
	if (nChar == 0x11)
		gActiveImageViewCPtr->SetControlKeyFlag(FALSE);
		
	else if (nChar == 0x10) 
		CheckShiftKeyUp ();                    
	
	CListBox::OnKeyUp(nChar, nRepCnt, nFlags);
	
}		// end "OnKeyUp"


void CMLegendList::OnLButtonDblClk(
				UINT 							nFlags, 
				CPoint 						point)
				
{                                 
	SInt16						selection;
						
						                                                                                        
	s_lastMouseDnPoint = point; 
	selection = GetCurSel();  
	                           
	if (gPresentCursor == kArrow)
		{               
		CPoint		lastClickPoint; 
						   
						            
		lastClickPoint = LastClickPoint();
	
		if (lastClickPoint.x > 15)
			{                   
			SInt16						cellLine;
	
	
					// Edit class or group name.
				
			cellLine = (SInt16)GetItemData(selection);
			SInt16 classGroupChangeCode = kEditClass;
			if (cellLine & 0x8000) 
				classGroupChangeCode = kEditGroup;
			
			else		// !(cellLine & 0x8000)
				{
				if ( (m_listType == kGroupClassDisplay) &&
//										(m_shiftKeyDownFlag) ) 
									gActiveImageViewCPtr->GetControlKeyFlag() )
					classGroupChangeCode = kNewGroup;
			
				}		// end "	else !(cellLine & 0x8000)"
			
			cellLine &= 0x7fff;
		
			EditGroupClassDialog (this, 
											selection, 
											classGroupChangeCode, 
											cellLine);
				//{
				//if (classGroupChangeCode == kNewGroup)
				//	UpdatePaletteWindow (TRUE, FALSE);
				
				//}		// end "if ( EditGroupClassDialog (..."
		
			}		// end "if (lastClickPoint.x > 15)"
		
		else		// lastClickPoint.x <= 15
			{  
			Handle						displaySpecsHandle;

			displaySpecsHandle = GetActiveDisplaySpecsHandle ();

			EditClassGroupPalette(this,
					 				 displaySpecsHandle,
									 selection,
									 (SInt16)m_listType);
		
			}		// end "else lastClickPoint.x <= 15" 
		
		}		// end "if (gPresentCursor == gPresentCursor == kArrow)"
		
	else if (gPresentCursor == kBlinkOpenCursor1)
		{             
		Point							lCell;
	
	
		lCell.h = 0; 
		lCell.v = selection;
		
		DoBlinkCursor1 (this, lCell, (SInt16)m_listType, 0);
		
		}		// end "else if (gPresentCursor == kBlinkOpenCursor1)"

	CListBox::OnLButtonDblClk(nFlags, point);
	
}		// end "OnLButtonDblClk"


void CMLegendList::OnLButtonDown(
				UINT 							nFlags, 
				CPoint 						point)
				
{  
	Point							lCell;
	RECT							rect; 
	
	UInt32						longCellValue;
	
	UInt16						cellValue;
	
	                                                                
	s_lastMouseDnPoint = point; 
	
	CMImageDoc* imageDocCPtr = 
		gActiveImageViewCPtr->GetDocument();
	CMImageFrame* imageFrameCPtr = 
		imageDocCPtr->GetImageFrameCPtr();
	CMLegendView* legendViewCPtr = 
		imageFrameCPtr->GetLegendViewCPtr();

	legendViewCPtr->GetDlgItem(IDC_List1)->UpdateWindow();

	CListBox::OnLButtonDown(nFlags, point);

	lCell.h = 0;
	lCell.v = GetCurSel();
	
			// Get the bottom of the list box.
			
	s_listBottom = GetCount() * GetItemHeight(0);
	GetClientRect (&rect);
	s_listBottom = MIN(s_listBottom, rect.bottom);

	if (point.x >= 15 && point.y <= s_listBottom && 
			gPresentCursor == kArrow && m_listType == kGroupClassDisplay)
		{
				// Determine if this is a group cell. If so then do not allow to be moved. 
				   
		GetText(lCell.v, (LPTSTR)&gTextString);
		BlockMoveData(&gTextString, &longCellValue, 4);
		cellValue = (UInt16)longCellValue;
		            
		if ( !(cellValue & 0x8000) )
			{
					// This is a class cell. Go ahead and outline it.
					
			gVerticalCellOffset = 0;
			gSelectedCell = lCell.v;   

			GetItemRect( gSelectedCell, &s_dragRect); 
                           
			s_dragRect.left += 15;
                                       
			CClientDC dc(this);
			dc.DrawFocusRect(&s_dragRect);
			s_grayRectDisplayedFlag = TRUE; 
			
					// Save the dragging rectangle size and the offset between the top
					// of the rectangle and the pointer.
					             
			s_lastVerticalPoint = point.y;  

			s_draggingFlag = TRUE;             
			
			}		// end "if ( !(cellValue & 0x8000) )" 
		
		}		// end "if (point.x >= 15 && gPresentCursor == kArrow && m_listType == ..."

	if (gPresentCursor == kBlinkOpenCursor1)
		{
		
		DoBlinkCursor1 (this, lCell, (SInt16)m_listType, 0);
		
		}		// end "if (gPresentCursor == kBlinkOpenCursor1)"
	
}		// end "OnLButtonDown" 


void CMLegendList::OnLButtonUp(UINT nFlags, CPoint point)
{                  
	if (s_draggingFlag)
		{  
		if (s_grayRectDisplayedFlag)
			{
					// Remove the current gray rectangle and edit
					// the group list. 
					
			CClientDC dc(this);
			dc.DrawFocusRect(&s_dragRect);
			s_grayRectDisplayedFlag = FALSE;   
			
			EditGroups (this);  
			
			}		// end "if (s_grayRectDisplayedFlag)"
			
		s_draggingFlag = FALSE;
		s_lastVerticalPoint = 0;
		s_listBottom = 0; 
		
		}		// end "if (s_draggingFlag)" 
	
	UpdateThematicLegendControls(gActiveImageViewCPtr);
	
	CListBox::OnLButtonUp(nFlags, point);
	
}		// end "OnLButtonUp"


void CMLegendList::OnMouseMove(UINT nFlags, CPoint point)

{ 	
	int				bottom;
		                      
	int state = GetKeyState(VK_SHIFT);    // GetAsyncKeyState(VK_SHIFT) 
	
	                                                                 
	if (s_draggingFlag)
		{                             
		CClientDC dc(this); 
	
				// Get the bottom of the list box.
				
		bottom = GetCount() * GetItemHeight(0);
		
				// Determine if the pointer is still within the allowed dragging
				// region of the list.
				
		if (point.x < 0 || point.x > s_dragRect.right || 
				point.y < 0 || point.y > s_listBottom) 
			{
					// The pointer is outside of the allow list region, be
					// sure that the drag rectangle is not displayed.
							
			if (s_grayRectDisplayedFlag)
				{        
				dc.DrawFocusRect (&s_dragRect);
				s_grayRectDisplayedFlag = FALSE;
				
				}		// end "if (s_grayRectDisplayedFlag)"   
			
			}		// end "if (point.x < 0 || point.x > s_dragRect.right || ..."
			
		else		// point is within the drag rectangle for the list.
      	{
      			// Check if the point has moved since the last draw.
      			
      	if (point.y != s_lastVerticalPoint)
      		{ 
	      	if (s_grayRectDisplayedFlag)
	      		{
	      				// Gray rectangle is displayed; remove it 
	      				
					dc.DrawFocusRect (&s_dragRect);
					s_grayRectDisplayedFlag = FALSE;
					
					}		// end "if (s_grayRectDisplayedFlag)"
				                        
						// Now draw the new drag rectangle 
		
				s_dragRect.top += point.y - s_lastVerticalPoint;
				s_dragRect.bottom += point.y - s_lastVerticalPoint; 
		
				dc.DrawFocusRect(&s_dragRect);
				s_grayRectDisplayedFlag = TRUE; 
				
				s_lastVerticalPoint = point.y;
				
				}		// end "if (point.y != s_lastVerticalPoint)"
				
			}		// end "else point is within the drag rectangle for the list."
			
		}		// end "if (s_draggingFlag)"

	if ( point.x <= 14 && (state & 0x8000) && m_activeFlag)
		{
		if (gPresentCursor != kBlinkOpenCursor1) 
			MSetCursor (kBlinkOpenCursor1); 
		
		}		// end "if (point.x <= 14 && ...)"
		
	else		// point.x > 14 || !GetKeyState(VK_SHIFT || !m_activeFlag 
		{	   
		if (gPresentCursor != kArrow)   
			MSetCursor (kArrow);  
		
		}		// end "else point.x > 14 || ..." 
	
	CListBox::OnMouseMove(nFlags, point);
	
}		// end "OnMouseMove"


BOOL CMLegendList::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{                                                                  
			// This is done so that the 'OnSetCursor' routine in CMImageFrame
			// does not get called.
	
	if (gPresentCursor != kBlinkOpenCursor1)
		return CListBox::OnSetCursor(pWnd, nHitTest, message);

	else		// gPresentCursor == kBlinkOpenCursor1                                                   
		return (TRUE);
		                                     
} 		// end "OnSetCursor"


void CMLegendList::SetBitMapInfoHeaderHandle(
				Handle			bitMapInfoHeaderHandle)

{                                                                  
	m_bitMapInfoHeaderHandle = bitMapInfoHeaderHandle;  
	
}		// end "SetBitMapInfoHeader"


void CMLegendList::SetLegendListActiveFlag (
				Boolean				settingFlag)

{  
	if (this != NULL)                 
		m_activeFlag = settingFlag;
	
}		// end "SetLegendListActiveFlag"
