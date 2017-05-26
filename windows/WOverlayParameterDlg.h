#if !defined(AFX_WOVERLAYPARAMETERDLG_H__EB172C63_5127_11D5_8E6D_00105AA88EE3__INCLUDED_)
#define AFX_WOVERLAYPARAMETERDLG_H__EB172C63_5127_11D5_8E6D_00105AA88EE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WOverlayParameterDlg.h : header file
//     
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMOverlayParameterDlg dialog

class CMOverlayParameterDlg : public CMDialog
{
// Construction
public:
	CMOverlayParameterDlg(CWnd* pParent = NULL);   // standard constructor
  
	
	Boolean			DoDialog(
							WindowInfoPtr			windowInfoPtr,
							SInt16					overlayCode,
							SInt16					selection); 

// Dialog Data
	//{{AFX_DATA(CMOverlayParameterDlg)
	enum { IDD = IDD_OverlayParameters };
	int		m_overlayValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMOverlayParameterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMOverlayParameterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnOverlayColor();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	WindowInfoPtr		m_windowInfoPtr;
	
	Boolean				m_initializedFlag;

	SInt16				m_overlayCode,
							m_overlayIndex;

	int					m_lineThickness,
							m_maxValue,
							m_minValue,
							m_transparency;
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WOVERLAYPARAMETERDLG_H__EB172C63_5127_11D5_8E6D_00105AA88EE3__INCLUDED_)
