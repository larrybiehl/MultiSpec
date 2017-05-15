// WLegList.h : header file
//        

#if !defined __WLEGLIST_H__
	#define __WLEGLIST_H__

/////////////////////////////////////////////////////////////////////////////
// CMLegendList

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif   

typedef struct LegendBitMapInfo
	{
	BITMAPINFOHEADER 	bmiHeader;
	UInt16				bmiColors[1];
	UInt8					paletteIndex;
	
	}	LegendBitMapInfo;            
   

class CMLegendList : public CListBox
{          
public:  
	CMLegendList(); 
	
	virtual ~CMLegendList();
	
	CPoint				LastClickPoint(void);
	
	void					CheckShiftKeyDown(void);
	
	void					CheckShiftKeyUp(void);
	
	void					DrawLegendList ();

	Handle				GetBitMapInfoHeaderHandle(); 
	
	void					SetBitMapInfoHeaderHandle(
								Handle				bitMapInfoHeaderHandle);
								
	void					SetLegendListActiveFlag (
								Boolean				settingFlag);  
								                                  
	void					GetDrawItemStruct(LPDRAWITEMSTRUCT lpDrawItemStruct);                                

// Implementation
	virtual void 		MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void 		DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
//	virtual int 		CompareItem(LPCOMPAREITEMSTRUCT lpCIS);
                                      
	CPalette* 					m_paletteObject; 
	CPalette* 					m_backgroundPaletteObject; 
	Handle						m_imageFileInfoHandle;
	SInt32						m_listType;                
	SInt32						m_classPaletteEntries; 
	SInt16						m_paletteOffset;
		    
protected:								
	// Generated message map functions
	//{{AFX_MSG(CMLegendList)  
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()  
	
	DRAWITEMSTRUCT				m_drawItem; 
	
	Handle						m_bitMapInfoHeaderHandle;
	Boolean						m_activeFlag,
				 					m_shiftKeyDownFlag;
	
	static CPoint				s_lastMouseDnPoint;
	
	static RECT					s_dragRect;
	
	static Handle				s_legendBitMapInfo;
	
	static int					s_lastVerticalPoint,
									s_listBottom;  
	
	static Boolean				s_draggingFlag,
									s_grayRectDisplayedFlag,
									s_isPrintingFlag; 
							
};

///////////////////////////////////////////////////////////////////////////// 
          
#endif	// !defined __WLEGLIST_H__  