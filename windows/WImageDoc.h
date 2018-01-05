// WImageDoc.h : interface of the CMImagDoc class
//
///////////////////////////////////////////////////////////////////////////// 
               
#if !defined __MIMAGDOC_H__
	#define __MIMAGDOC_H__ 
	
#include "WDrawObjects.h"  
	
class CMImageDoc : public CDocument
	{
	protected: // create from serialization only
						CMImageDoc();
						
		DECLARE_DYNCREATE(CMImageDoc)
	              
	public:                                   
	                 
		virtual 				~CMImageDoc();  
		           			
		void 					Add(
									CMDrawObject* 	pObject);

		void 					Draw(
									CDC* 				pDC, 
									CMImageView* 	pImageView); 
		 
		Boolean			 	GetDisplayCoordinatesFlag() { return m_displayCoordinatesFlag; }   
		 
		CMImageWindow*	 	GetDocImageWindowCPtr() { return m_imageWindowCPtr; } 
		     
		CSize 				GetDocSize() { return m_sizeDoc; } 
		 
		CMImageFrame*	 	GetImageFrameCPtr() { return m_imageFrameCPtr; } 
		
		CMOutlineArea*	 	GetSelectionAreaCPtr() { return m_selectionAreaCPtr; }
		
		void 					OnCloseDocument (void);
								
		void 					Remove(
									CMDrawObject* 	pObject);   
		 
		void	 				SetDocSize( 
									SInt16			height,
									SInt16			width);
									
		void					SetImageFrameCPtr(
									CMImageFrame*			imageFrameCPtr); 
									
		void					SetImageWindowCPtr(
									CMImageWindow*			imageWindowCPtr);

		void					ShowCoordinateView (
									Boolean					displayCoordinatesFlag);
									
	//	virtual void 		Serialize(CArchive& ar);   // overridden for document i/o  
	
		#ifdef _DEBUG
			virtual void 		AssertValid() const;
			virtual void 		Dump(CDumpContext& dc) const;
		#endif                                     
	
	protected:   
		void          		InitDocument(); 
		
		BOOL 					OnNewDocument(); 
		
		BOOL 					OnOpenDocument(
									LPCTSTR pszPathName); 
	
				// Generated message map functions
				 
		//{{AFX_MSG(CMImageDoc)
	afx_msg void OnUpdateViewCoordinatesBar(CCmdUI* pCmdUI);
	afx_msg void OnViewCoordinatesBar();
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWindowShowCoordinateView(CCmdUI* pCmdUI);
	afx_msg void OnWindowShowCoordinateView();
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
		
		Boolean				m_initializedFlag;  
		
		Boolean				m_displayCoordinatesFlag;
	
		CObList 				m_objects; 
	           
		CSize     			m_sizeDoc; 
		
		CMImageWindow*		m_imageWindowCPtr;  
		
		CMOutlineArea*		m_selectionAreaCPtr;
		
		CMImageFrame*		m_imageFrameCPtr;
		
	};

/////////////////////////////////////////////////////////////////////////////

#endif		// !defined __MIMAGDOC_H__
