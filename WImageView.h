//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2017)
//						(c) Purdue Research Foundation
//								All rights reserved.
//
//	File:						WImageView.h
//	Implementation:		WImageView.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.5
//
//	Revision date:			12/21/2017
//
//	Language:				C++
//
//	System:					Windows Operating Systems
//
//	Brief description:	This file is the definition for the CMImageView class
//
//------------------------------------------------------------------------------------

#ifndef __VIEWIMAGEH__
	#define __VIEWIMAGEH__

	#include "SMultiSpec.h"

	#include "WDrawObjects.h"
	#include "WLegendView.h"

	class CMImageView : public CScrollView
		{                    
		DECLARE_DYNCREATE(CMImageView)
		
		protected: // create from serialization only
			CMImageView ();

			void OnMenuSelect (
							UINT									nItemID, 
							UINT									nFlags, 
							HMENU									hMenu);
	              
		public:           
			virtual ~CMImageView ();
			
			void ClientToDoc (
							CPoint&								point);
										
			Boolean DoDeleteKey (
							UINT									nChar);
										
			void DoEditCopy (void); 
										
			void DoFilePrint (); 
										
			void DoFilePrintPreview ();
										
			Boolean GetActiveWindowFlag (void);
			
			SInt16 GetClassGroupCode (void);
		
			CMImageDoc* GetDocument (void);
			
			SInt16 GetImageType (void);
			
			SInt16 GetNumberGroups (void); 
			
			CMOutlineArea* GetSelectionAreaCPtr (void);
			
			SInt16 GetTitleHeight (void);
			
			SInt16 GetWindowType (void); 

			void DrawDC (
							CDC*									pDC); 

			
			virtual void OnDraw (
									// overridden to draw this view
							CDC*									pDC); 
			
			virtual void OnPrint (
							CDC*									pDC, 
							CPrintInfo*							pInfo);
										
			virtual void OnUpdate (
							CView*								pSender,
							LPARAM								lHint = 0L,
							CObject*								pHint = NULL); 
									
			void Remove (
							CMDrawObject*						pObject);
										
			void SetControlKey (
							Boolean								settingFlag);
										
			void UpdateSelectionCoordinates (void);
											
			void UpdateCursorCoordinates (
							LongPoint*							mousePoint);
										
			void UpdateCursorCoordinates (void);           
			
			void ZoomIn (void); 
			                   
			void ZoomOut (void);

			void ScrollV (
							UINT									nSBCode);
							
			void ScrollH (
							UINT									nSBCode);

			#ifdef _DEBUG
				virtual void AssertValid () const;
				virtual void Dump (
								CDumpContext&						dc) const;
			#endif
			
		protected: 
			SInt32 NormalizeScrollOffset (
							double								magnification,
							SInt32								scrollIncrement,
							SInt32								scrollOffset); 
			
			virtual void OnActivateView (
							BOOL									bActivate, 
							CView*								pActivateView,
							CView*								pDeactiveView); 
			
			virtual void OnInitialUpdate ();	// first time after construct  
										
			BOOL OnScrollBy (
							CSize									sizeScroll, 
							BOOL									bDoScroll);
			
			void SetScrollSizes (
							int									nMapMode, 
							SIZE									sizeTotal,
							const SIZE&							sizePage, 
							const SIZE&							sizeLine);  
										
			void UpdateOffscreenMapOrigin (void);
			
			void UpdateScrollRange (
							double								magnification);
										                                         
				// Printing support
						
			virtual BOOL OnPreparePrinting (CPrintInfo* pInfo);
		
					// Generated message map functions
		
			//{{AFX_MSG(CMImageView)
			afx_msg LRESULT OnDoRealize (
							WPARAM								wParam, 
							LPARAM								lParam);  // user message
			afx_msg void OnEditCut ();
			afx_msg void OnUpdateEditCut (
							CCmdUI*								pCmdUI);
			afx_msg void OnUpdateEditPaste (CCmdUI* pCmdUI);
			afx_msg void OnEditPaste ();
			afx_msg void OnUpdateFilePrintSetup (CCmdUI* pCmdUI);
			afx_msg void OnUpdateEditClear (CCmdUI* pCmdUI);
			afx_msg void OnUpdateEditUndo (CCmdUI* pCmdUI); 
			afx_msg void OnSetFocus (CWnd* pOldWnd);
			afx_msg void OnMouseMove (UINT nFlags, CPoint point);
			//afx_msg void OnNcMouseMove (UINT nFlags, CPoint point);
			afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
			afx_msg void OnLButtonUp (UINT nFlags, CPoint point);
			afx_msg void OnLButtonDblClk (UINT nFlags, CPoint point);
			afx_msg void OnSize (UINT nType, int cx, int cy);
			afx_msg BOOL OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message);
			afx_msg void OnChar (UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void OnSysKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
			afx_msg void OnPaint ();
			afx_msg void OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
			afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
			afx_msg BOOL OnEraseBkgnd (CDC* pDC);
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP() 
  
		public: 
			Boolean CheckIfOffscreenImageExists (void);
												
			void DisposeImageWindowSupportMemory (void);
																							
			void DrawLegend ();

			Boolean GetControlKeyFlag (void); 
			
			UInt16 GetDisplayPixelSize (void);
			
			Handle GetDisplaySpecsHandle (void);
			
			CMImageFrame* GetImageFrameCPtr (void);

			CMImageWindow* GetImageWindowCPtr (void); 

			CMLegendView* GetImageLegendViewCPtr (void);

			CMLegendList* GetImageLegendListCPtr (void); 

			SInt16 GetLegendFullHeight (void); 

			SInt16 GetLegendWidth (void);
																							
			void SetControlKeyFlag (
							Boolean								flag); 

			void SetLegendBitMapInfoHeaderHandle(
							Handle								bitMapInfoHeaderHandle);

			void SetImageWindowCPtr (
							CMImageWindow						*imageWindowCPtr);
											 
			void UpdateScrolls(
							double								newMagnification);
										
			double m_printerTextScaling; 
			
					// Pointer to CDC class 
																								 
			CDC*									m_pDC;					
			
					// Pointer to the display class for multispectral image.
					
			CMDisplay*							m_displayMultiCPtr;
				
					// Pointer to the histogram class for the image.
					
			CMHistogram*						m_histogramCPtr; 
			
					// Flag indicating whether the current draw mode is for printing
					// or copying.	
			
			Boolean								m_printCopyModeFlag;
			
					// Flag indicating whether the class members have been
					// initialized successfully.
					
			Boolean								m_initializedFlag;
			
			SInt16								m_xPixelsPerInch;
			SInt16								m_yPixelsPerInch; 
			
		protected: 
					 // Flag indicated whether this view is active
												
			Boolean								m_activeFlag;  
			
					 // Flag indicated whether already within 'OnSize' routine.
												
			Boolean								m_withinOnSizeFlag;  
			
					 // Flag indicating whether UpdateScrollRange should be called.
												
			Boolean								m_callUpdateScrollRangeFlag;   
			
					 // Flag indicating whether the control key is down indicating that
					 // zooming should occur at .1 increments.
			
			Boolean								m_ctlKeyDownFlag;  
			
					 // Flag indicating whether the control key is down indicating that
					 // zooming should occur at .1 increments.
			
			Boolean								m_thumbScrollFlag;
			
					// Pointer to the image window class for the image view.
			
			CMImageWindow*						m_imageWindowCPtr;
			
					// Saved cursor line and column value                   
			
			SInt32								m_cursorColumnValue;
			
			SInt32								m_cursorLineValue;
			
			static SInt16						s_currentCursor;
			
					// This structure contains the rectangle to be updated in the current 
					// window.
			
			static RECT							s_updateRect;
			
		};

	#ifndef _DEBUG  // debug version in cimageview.cpp 	
		inline CMImageDoc* CMImageView::GetDocument()
			{ 
			return (CMImageDoc*)m_pDocument; 
			
			}
				
		inline CMImageWindow* CMImageView::GetImageWindowCPtr()
			{ 
			if (this == NULL)
				return (NULL);
					
			else
				return (CMImageWindow*)m_imageWindowCPtr;
				
			}
	#endif	// _DEBUG
#endif // !defined __VIEWIMAGEH__
