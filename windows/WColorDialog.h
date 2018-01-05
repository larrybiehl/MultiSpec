// WColorDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMColorDialog dialog

class CMColorDialog : public CColorDialog
{
// Construction
public:
	CMColorDialog(COLORREF clrInit = 0,
		DWORD dwFlags = 0,
		CWnd* pParentWnd = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMColorDialog)          
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMColorDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
