#if !defined(AFX_WLOOMDLG_H__337E2183_F42A_11D3_8D9B_00105AA88EE3__INCLUDED_)
#define AFX_WLOOMDLG_H__337E2183_F42A_11D3_8D9B_00105AA88EE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WLeaveOneOutMixingDialog.h : header file
//           
	     
#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMLOOMixingDialog dialog

class CMLOOMixingDialog : public CMDialog
{
// Construction
public:
	CMLOOMixingDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMLOOMixingDialog)
	enum { IDD = IDD_LOOMixingParameter };
	int		m_mixingParameterCode;
	double	m_userMixingParameter;
	//}}AFX_DATA

Boolean DoDialog(
				SInt16									statsWindowMode);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMLOOMixingDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMLOOMixingDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnUserMixing();
	afx_msg void OnOptimumMixing();
	afx_msg void OnIdentityMatrix();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	double					m_loocMixingParameter;

	SInt16					m_statsWindowMode;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WLOOMDLG_H__337E2183_F42A_11D3_8D9B_00105AA88EE3__INCLUDED_)
