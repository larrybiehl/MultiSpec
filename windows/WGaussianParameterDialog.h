// CMGaussianParameterDlg.h : header file
//       
#if !defined(AFX_WGAUSSIANPARAMETERDLG_H__12DCBA43_80E8_11D7_8F8E_00105AA88EE3__INCLUDED_)
#define AFX_WGAUSSIANPARAMETERDLG_H__12DCBA43_80E8_11D7_8F8E_00105AA88EE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WGaussianParameterDialog.h : header file
//      
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMGaussianParameterDlg dialog

class CMGaussianParameterDlg : public CMDialog
{
// Construction
public:
	CMGaussianParameterDlg(CWnd* pParent = NULL);   // standard constructor
	
	Boolean			DoDialog(
							double*								gaussianStretchPtr); 

// Dialog Data
	//{{AFX_DATA(CMGaussianParameterDlg)
	enum { IDD = IDD_GaussianParameter };
	double	m_gaussianStretch;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMGaussianParameterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMGaussianParameterDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WGAUSSIANPARAMETERDLG_H__12DCBA43_80E8_11D7_8F8E_00105AA88EE3__INCLUDED_)
