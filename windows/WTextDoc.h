// mtextdoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMTextDoc document

class CMTextDoc : public CDocument
{
		DECLARE_SERIAL(CMTextDoc)
	protected:
							CMTextDoc();			// protected constructor used by dynamic creation

		// Attributes
	public:  

	protected:
		CSize       	m_sizeDoc;
	public:
		CSize 			GetDocSize() { return m_sizeDoc; }
		
		BOOL					SaveModified(void);

		// Operations
	public:

		// Implementation
	protected:
		virtual 				~CMTextDoc(void);
		
		void          		InitDocument(void);
		
		virtual void 		Serialize(CArchive& ar);	// overridden for document i/o 
		
		void 					OnCloseDocument (void);
		
		virtual BOOL		OnSaveDocument(
									LPCTSTR								lpszPathName);
		
		virtual BOOL 		OnNewDocument(void);
                 
		// Generated message map functions
	protected:
	//{{AFX_MSG(CMTextDoc)
	afx_msg void OnViewCoordinatesBar();
	afx_msg void OnUpdateViewCoordinatesBar(CCmdUI* pCmdUI);
	//}}AFX_MSG
		
	DECLARE_MESSAGE_MAP()
};

