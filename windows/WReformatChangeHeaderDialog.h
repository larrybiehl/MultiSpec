// WReformatChangeHeaderDialog.h : header file
//              
#if !defined __WRHEDDLG_H__
	#define	__WRHEDDLG_H__  
	
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMChangeHeaderDlg dialog

class CMChangeHeaderDlg : public CMDialog
{
// Construction
public:
	CMChangeHeaderDlg(CWnd* pParent = NULL);	// standard constructor
	
	SInt16				DoDialog(
								SInt16*					requestedFormatPtr);

// Dialog Data
	//{{AFX_DATA(CMChangeHeaderDlg)
	enum { IDD = IDD_ChangeHeader };
	int		m_headerOptionsSelection;
	//}}AFX_DATA

// Implementation
protected:
	virtual void 		DoDataExchange(CDataExchange* pDX);	// DDX/DDV support 
								
//	Boolean				Initialize(void);
	
	void					LoadCurrentHeaderParametersInDialog (
								CDataExchange* 		dxToPtr);

	// Generated message map functions
	//{{AFX_MSG(CMChangeHeaderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeParameters();
	afx_msg void OnChangeMapParameters();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	Boolean				m_initializedFlag;
};      
  
#endif // !defined __WRHEDDLG_H__
