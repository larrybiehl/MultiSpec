//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WGraphView.h
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Language:				C
//
//	System:					Macintosh and Windows Operating Systems
//
//	Brief description:	definition for CMGraphView class
//
//	Revised By:				Larry L. Biehl			Date: 07/03/2018
//
// **Note:					This file needs to be split into Mac and Windows files. It
//								is too confusing to try to make all work in one file.
//	
//------------------------------------------------------------------------------------
               
#if !defined __WGRAPHVIEW_H__
	#define __WGRAPHVIEW_H__

	class CMGraphViewButton : public CButton
		{          
		public:
			CMGraphViewButton ();  
			
			virtual ~CMGraphViewButton ();
			
			void		SetButtonID (
							UInt16 				buttonID); 

			void 		SetGraphViewCPtr (
							CMGraphView*		graphViewCPtr);

		// Generated message map functions
		protected:
			//{{AFX_MSG(CMGraphViewButton)
			afx_msg void 		OnLButtonDown (UINT nFlags, CPoint point);
			afx_msg void 		OnMouseMove (UINT nFlags, CPoint point);
			afx_msg void 		OnLButtonUp (UINT nFlags, CPoint point); 
			afx_msg void 		OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void 		OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void 		OnRButtonDown (UINT nFlags, CPoint point);
			afx_msg void 		OnRButtonUp (UINT nFlags, CPoint point); 
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

			void					OnLines2 (
										UINT					menuID);

			void					OnSelectVector (
										UINT					menuID);
			
			void					OnBinWidth (
										UINT							menuID);
			
			UInt16					m_buttonID; 
			
			CMGraphView*			m_graphViewCPtr;
									
		};


	class CMGraphViewCombo : public CComboBox
		{          
		public:
			CMGraphViewCombo ();  
			
			virtual ~CMGraphViewCombo ();

			void 		SetGraphViewCPtr (
							CMGraphView*		graphViewCPtr);

				// Generated message map functions
		protected:
			//{{AFX_MSG(CMGraphViewCombo)
			afx_msg void OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
			
			CMGraphView*			m_graphViewCPtr;
									
		};

	
	class CMGraphView : public CFormView
		{
		DECLARE_DYNCREATE(CMGraphView)
			
		protected:
									CMGraphView ();		// protected constructor used by dynamic creation

			virtual 				~CMGraphView ();
					
			virtual void 		OnDraw (CDC* pDC);		// overridden to draw this view
		
			void					OnInitialUpdate (void);
																								  
				// Printing support
						
			virtual BOOL 		OnPreparePrinting (CPrintInfo* pInfo);
			
			void					MoveGraphControls (void);

			//void					OnNextChannel (void);

			//void					OnPreviousChannel (void);

				// Generated message map functions 
			
		//{{AFX_MSG(CMGraphView)
		afx_msg int			OnCreate (LPCREATESTRUCT lpCreateStruct);
		afx_msg void 		OnUpdateFileSaveAs (CCmdUI* pCmdUI);
		afx_msg void		OnDestroy ();
		afx_msg void		OnUpdateEditCopy (CCmdUI* pCmdUI);
		afx_msg void		OnEditCopy ();
		afx_msg void		OnNextChannel ();
		afx_msg void		OnPreviousChannel ();
		afx_msg void		OnLines ();
		afx_msg void		OnSize (UINT nType, int cx, int cy);
		afx_msg void		OnDropdownXAxisDropList ();
		afx_msg void		OnSelendokXAxisDropList ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP() 

		CDataExchange		*m_dialogFromPtr,
								*m_dialogToPtr; 

	// Form Data
	public: 
		//{{AFX_DATA(CMGraphView)
		enum { IDD = IDD_GraphWindow };
		CMGraphViewButton				m_nextChannel;
		CMGraphViewButton				m_previousChannel;
		CMGraphViewButton				m_selectVectors;
		CMGraphViewButton				m_overlayControl;
		CMGraphViewButton				m_binWidth;
		CMGraphViewCombo				m_xAxisCombo;
		int								m_xAxisSelection;
		//}}AFX_DATA
				
		public: 
			void 					DoFilePrint (void); 

			void					DoFilePrintPreview (void);
				
			virtual void 		OnPrint (
										CDC* 									pDC, 
										CPrintInfo* 						pInfo);
			
			CMGraphDoc*			GetDocument ();
			
			CMenu*				GetGraphOverlayPopupMenu ();
			
			CMenu*				GetSelectVectorPopupMenu ();
			
			CMenu*				GetBinWidthPopupMenu ();
			
			CMenu*				GetXAxisPopupMenu ();

			Boolean				UpdateGraphControls (void);

		protected:
			virtual void DoDataExchange(CDataExchange* pDX);

		void					OnLines2(
									UINT					menuID);
		
	public:
		void					DrawGraphGrowIcon	( 
									GraphPtr								graphRecordPtr);

		Boolean 				FinishGraphRecordSetUp (
									SInt16*								channelListPtr,
									SInt32								channelListLength,
									SInt32								vectorLength,
									SInt32								numberYVectors,
									SInt32								numberSets,
									UInt16								xValueType,
									UInt16								yValueType);
										
		Handle 				GetGraphRecordHandle (void);
		
		UInt16 				GetGraphWindowNumber (void);
	
		Handle 				GetSelectionIOHandle (void);
	
		WindowPtr 			GetWindowPtr (void);
		
		void 					SetCheckIOMemoryFlag (
									Boolean					flagSetting);

		void		 			SetWindowPtr (
									WindowPtr				window);
													  
		static CBrush				s_grayBrush;
					
		static CPen 				s_bluePen; 
		
		static CPen 				s_grayPen; 
		
		static CPen 				s_greenPen;   
		
		static CPen 				s_ltGrayPen; 
		
		static CPen 				s_redPen;   
		
		static CPen 				s_whitePen; 
								
		double						m_printerTextScaling;
	
				// Flag indicating whether the current draw mode is for printing
				// or copying.	
	
		Boolean						m_printCopyModeFlag; 
	
		SInt16						m_xPixelsPerInch;
		SInt16						m_yPixelsPerInch;   

		Boolean						m_closeGraphSelectedFlag;                      
		
	protected: 

		void							IntializeGraphRecord (void);
	
		Handle						m_graphRecordHandle;
	
		static Handle				s_selectionIOInfoHandle;
		
		WindowPtr					m_window;
		
		static UInt16				s_graphicsWindowCount; 
		
		UInt16						m_graphicsWindowNumber;  

		static UInt16				s_numberOfGWindows;
									
		static CFont 				s_font;

			// This structure contains the rectangle to be updated in the current window.

		static RECT					s_updateRect;
		
		CMenu							*m_binWidthMenu,
										*m_graphOverlayMenu,
										*m_selectVectorMenu;
	};

	inline Handle CMGraphView::GetGraphRecordHandle ()
		{ return (Handle)m_graphRecordHandle; }  
			
	inline UInt16 CMGraphView::GetGraphWindowNumber ()
		{ return (UInt16)m_graphicsWindowNumber; }
			
	inline Handle CMGraphView::GetSelectionIOHandle ()
		{ return (Handle)s_selectionIOInfoHandle; }
			
	inline WindowPtr CMGraphView::GetWindowPtr ()
		{ return (WindowPtr)m_window; }
			
	inline void CMGraphView::SetWindowPtr (WindowPtr	window)
		{ m_window = window; }
				 
	inline CMenu* CMGraphView::GetGraphOverlayPopupMenu ()
		{ return m_graphOverlayMenu; }
	inline CMenu* CMGraphView::GetSelectVectorPopupMenu ()
		{ return m_selectVectorMenu; }
	inline CMenu* CMGraphView::GetBinWidthPopupMenu ()
		{ return m_binWidthMenu; }

#endif	// !defined __SGRAPHVIEW_H__
