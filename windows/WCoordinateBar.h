// WCoordinateBar.h : interface of the CMCoordinateBar class
//
///////////////////////////////////////////////////////////////////////////// 
               
#if !defined __MCOORBAR_H__
	#define __MCOORBAR_H__ 
	
class CMCoordinateBar : public CDialogBar
	{                                           
	public:                  
								CMCoordinateBar();                 
	                 
		virtual 				~CMCoordinateBar();
			
		virtual void 		OnDraw(
									CDC* 					pDC);  // overridden to draw this view
										
		virtual void		OnUpdate(
									CView*				pSender,
									LPARAM				lHint = 0L,
									CObject*				pHint = NULL);
		
		void					UpdateCoordinateView(
									Handle								windowInfoHandle);

	// Implementation
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
			


	// Generated message map functions
	//{{AFX_MSG(CMImageFrame)                          
	afx_msg BOOL 		OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);    
	afx_msg void 		OnPaint();
	afx_msg void 		OnSelendokAreaUnits();
	afx_msg void 		OnSelendokDisplayUnits();
	afx_msg void 		OnDropdownAreaUnitsCombo();
	afx_msg void 		OnCloseupAreaUnitsCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 

	public: 

	CDataExchange		*m_dialogToPtr,
							*m_dialogFromPtr;

	int					m_areaUnitsListSelection;
	int					m_displayUnitsListSelection;

	SInt16				m_areaUnitsCode,
							m_displayUnitsCode;
		
	};

/////////////////////////////////////////////////////////////////////////////

#endif		// !defined __MCOORBAR_H__
