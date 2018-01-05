#if !defined(AFX_WRECTDLG_H__D64668B3_8D4C_11D3_8D48_00105AA88EE3__INCLUDED_)
#define AFX_WRECTDLG_H__D64668B3_8D4C_11D3_8D48_00105AA88EE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WRecodeThematicImageDialog.h : header file
//     
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMRecodeThematicDialog dialog

class CMRecodeThematicDialog : public CMDialog
{
// Construction
public:
	CMRecodeThematicDialog(CWnd* pParent = NULL);   // standard constructor

	Boolean DoDialog( 
				RecodeThematicImagePtr			recodeThematicImagePtr);

// Dialog Data
	//{{AFX_DATA(CMRecodeThematicDialog)
	enum { IDD = IDD_ThematicRecode };
	long		m_thresholdValue;
	long		m_recodedValue;
	int		m_compareParameterCode;
	int		m_thresholdImageSelection;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMRecodeThematicDialog)
	public:
				
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMRecodeThematicDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokThresholdFileCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 
	
	RecodeThematicImagePtr			m_recodeThematicImagePtr;
	
	Handle								m_thresholdFileInfoHandle;
	
	Boolean								m_initializedFlag;
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WRECTDLG_H__D64668B3_8D4C_11D3_8D48_00105AA88EE3__INCLUDED_)
