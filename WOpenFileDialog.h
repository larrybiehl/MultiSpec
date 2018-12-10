// WOpenFileDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMOpenFileDialog dialog

//new
#include <afxdlgs.h>
//end new

UINT_PTR CALLBACK HookProcedure (HWND hdlg, 
											UINT uiMsg, 
											WPARAM wParam, 
											LPARAM lParam);

class CMOpenFileDialog : public CFileDialog
{
// Construction
public:
							CMOpenFileDialog(CWnd* pParent = NULL, TBYTE* filterString = NULL);	// standard constructor 
	
							~CMOpenFileDialog();		// standard desctructor
	
	Boolean				DoDialog(
								SInt16				stringIndex);

// Dialog Data
	//{{AFX_DATA(CMOpenFileDialog)
//	enum { IDD = 1536 };
		// NOTE: the ClassWizard will add data members here
	CString		m_prompt;
	int			m_imageType;
	int			m_linkOption;
	//}}AFX_DATA  
	
	Boolean		m_initializedFlag;

// Implementation
protected:
	virtual void 	DoDataExchange(CDataExchange* pDX);	// DDX/DDV support 

	virtual void	OnFileNameChange (void);
	
	void				SetImageLinkToFalse(void);
	
	void				SetImageLinkToTrue(void);
	
	BOOL				GetItem11String(
							UInt16							stringID,
							CString*							stringPtr);

//	BOOL				PreTranslateMessage (MSG* pMsg);

	// Generated message map functions
	//{{AFX_MSG(CMOpenFileDialog)       
	virtual BOOL OnInitDialog();  
	afx_msg void OnSelendokImageType(); 
	afx_msg void OnSelendokLinkOption();
//	_afxMsgLBSELCHANGE OnLBSelChangedNotify	(
//								UINT					nIDBox, 
//								UINT					iCurSel, 
//								UINT					nCode);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	CString		m_fileName;
	TBYTE*		m_filenamesBuffer;
	
	CDataExchange		*m_dialogFromPtr,
							*m_dialogToPtr;
							
	SInt16            m_stringIndex;

	UInt32				m_linkOptionSelectionDataCode,
							m_selectedFileCount;

	Boolean				m_initialLinkSelectedFilesFlag,
							m_showLinkPopupMenuFlag,
							m_userSetImageType;
							
public:
//	afx_msg void OnCbnDropdown14();
};
