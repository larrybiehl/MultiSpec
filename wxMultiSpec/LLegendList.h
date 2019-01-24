// LLegendList.h : header file
//
// Revised by Larry Biehl on 01/24/2019
//
#if !defined __LLEGLIST_H__
#	define __LLEGLIST_H__

/////////////////////////////////////////////////////////////////////////////
// CMLegendList
#include "SMultiSpec.h"

#include "wx/wx.h"
#include "wx/listctrl.h"
#include "wx/imaglist.h"

#define LEGEND_LIST 1040

typedef wxPoint CPoint;
/*typedef struct LegendBitMapInfo
	{
	BITMAPINFOHEADER 	bmiHeader;
	UInt16				bmiColors[1];
	UInt8					paletteIndex;
	
	}	LegendBitMapInfo;            
  */ 

class CMLegendList : public wxListView
{          
   DECLARE_DYNAMIC_CLASS(CMLegendList)
public:  
	CMLegendList();
   CMLegendList(wxWindow *parent,wxWindowID id = LEGEND_LIST);
	
	virtual ~CMLegendList();
	
	CPoint				LastClickPoint(void);
	
	void					DrawLegendList ();

	Handle				GetBitMapInfoHeaderHandle(); 
	
	void					SetBitMapInfoHeaderHandle(
								Handle				bitMapInfoHeaderHandle);
								
	void					SetLegendListActiveFlag (
								Boolean				settingFlag);  
   void              DrawItem(int itemData, int itemID);
/*							                                  
	void					GetDrawItemStruct(LPDRAWITEMSTRUCT lpDrawItemStruct);                                

// Implementation
	virtual void 		MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void 		DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);*/
//	virtual int 		CompareItem(LPCOMPAREITEMSTRUCT lpCIS);
                                      
	wxPalette* 					m_paletteObject; 
	wxPalette* 					m_backgroundPaletteObject; 
	Handle						m_imageFileInfoHandle;
	SInt32						m_listType;                
	SInt32						m_classPaletteEntries; 
	SInt16						m_paletteOffset;
   wxImageList             *m_ilist;
   bool                    listready;
		    
protected:								
	// Generated message map functions
	void OnLButtonDblClk (wxListEvent& event);
	void OnBeginDrag (wxListEvent& event);
   void OnLButtonUp (wxMouseEvent& event);
	void OnCharHook (wxKeyEvent& event);
	void OnKeyDown (wxKeyEvent& event);
	void OnKeyUp (wxKeyEvent& event);
	void OnMouseMove (wxMouseEvent& event);
        /*Newly added paint functions*/
	void paintEvent(wxPaintEvent & evt);
	//void render(wxDC&  dc);
	//void paintNow();
	void OnDrawItem();

	DECLARE_EVENT_TABLE()
	
	//DRAWITEMSTRUCT				m_drawItem; 
	
	Handle						m_bitMapInfoHeaderHandle;
	Boolean						m_activeFlag,
				 					m_shiftKeyDownFlag;
	
	static CPoint				s_lastMouseDnPoint;
	
	static RECT					s_dragRect;
	
	static Handle				s_legendBitMapInfo;
	
	static int					s_lastVerticalPoint,
									s_listBottom;  
	
	static bool					s_draggingFlag,
									s_grayRectDisplayedFlag,
									s_isPrintingFlag,
									s_controlKeyDownFlag,
									s_altKeyDownFlag,
									s_shiftKeyDownFlag;
   
   wxPanel*        m_LegendView;
   wxRect m_rectFocus;
							
};

///////////////////////////////////////////////////////////////////////////// 
          
#endif	// !defined __LLEGLIST_H__  
