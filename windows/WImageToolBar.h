// WImageToolBar.h : interface of the CMCoordinateBar class
//
///////////////////////////////////////////////////////////////////////////// 
               
#if !defined __WImgTBar_H__
	#define __WImgTBar_H__ 
	
class CMImageToolBar : public CDialogBar
	{                                           
	public:                  
								CMImageToolBar();                 
	                 
		virtual 				~CMImageToolBar();

	// Generated message map functions
	//{{AFX_MSG(CMImageFrame)                          
//	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);    
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 
		
	};

/////////////////////////////////////////////////////////////////////////////

#endif		// !defined __WImgTBar_H__