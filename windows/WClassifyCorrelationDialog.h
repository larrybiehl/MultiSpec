// WClassifyCorrelationDialog.h : header file
//               
#if !defined __WCCORDLG_H__
	#define	__WCCORDLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMCorrelationClassifyDialog dialog

class CMCorrelationClassifyDialog : public CMDialog
{
// Construction
public:
	CMCorrelationClassifyDialog(CWnd* pParent = NULL);	// standard constructor 
	
	Boolean			DoDialog(
							SInt16*								covarianceEstimatePtr);

// Dialog Data
	//{{AFX_DATA(CMCorrelationClassifyDialog)
	enum { IDD = IDD_SAMParameters };
	int		m_covarianceEstimate;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMCorrelationClassifyDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokCovarianceCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()  
	
	Boolean							m_initializedFlag;
	
};		// end "CMCorrelationClassifyDialog dialog" 
  
#endif // !defined __WCLSFDLG_H__
