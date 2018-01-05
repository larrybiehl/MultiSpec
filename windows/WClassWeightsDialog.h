#if !defined(AFX_WCWTDLG_H__E331A593_B32A_11D3_8D68_00105AA88EE3__INCLUDED_)
#define AFX_WCWTDLG_H__E331A593_B32A_11D3_8D68_00105AA88EE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WClassWeightsDialog.h : header file
//   
	
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMClassWeightsDlg dialog

class CMClassWeightsDlg : public CMDialog
{
// Construction
public:
	CMClassWeightsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMClassWeightsDlg)
	enum { IDD = IDD_ClassWeights };
	double	m_saveNewWeight;
	int		m_weightUnits;
	//}}AFX_DATA

	SInt16 DoDialog(                     
					UInt16								numberOutputClassesToUse,
					SInt16*								classListPtr,
					float*								weightsListPtr, 
					SInt16								weightsSelection,
					Boolean								useEnhancedStatFlag);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMClassWeightsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMClassWeightsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnEnterNewWeightButton();
	afx_msg void OnEqualWeightButton();
	afx_msg void OnHelpButton();
	afx_msg void OnChangeWeight();
	afx_msg void OnSelchangeList();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CFont									m_weightsListFont;
		
	double								m_weightSum;
	
	float*								m_weightsListPtr;
	SInt16*								m_classListPtr;

	SInt16								m_selectedCell,
											m_weightsSelection;

	UInt16								m_numberOutputClassesToUse;

	Boolean								m_initializedFlag,
											m_useEnhancedStatFlag;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WCWTDLG_H__E331A593_B32A_11D3_8D68_00105AA88EE3__INCLUDED_)
