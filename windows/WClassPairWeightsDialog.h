#if !defined(AFX_WCPWTDLG_H__E331A594_B32A_11D3_8D68_00105AA88EE3__INCLUDED_)
#define AFX_WCPWTDLG_H__E331A594_B32A_11D3_8D68_00105AA88EE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WClassPairWeightsDialog.h : header file
//   
	
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMClassPairWeightDlg dialog

class CMClassPairWeightDlg : public CMDialog
{
// Construction
public:
	CMClassPairWeightDlg(CWnd* pParent = NULL);   // standard constructor

	SInt16 DoDialog(
				UInt16								numberOfClassesToUse, 
				SInt16**	 							weightsListPtrPtr,
				SInt16								interClassWeightsSelection,
				SInt16*								defaultClassPairWeightPtr);

// Dialog Data
	//{{AFX_DATA(CMClassPairWeightDlg)
	enum { IDD = IDD_ClassPairWeights };
	long	m_newClassPairWeight;
	long	m_localDefaultClassPairWeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMClassPairWeightDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMClassPairWeightDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHelpButton();
	afx_msg void OnSelchangeClassList();
	afx_msg void OnAddButton();
	afx_msg void OnChangeNewWeight();
	afx_msg void OnRemoveButton();
	afx_msg void OnSelchangeClassPairWeightList();
	afx_msg void OnChangeDefaultWeight();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CFont									m_weightsListFont;

	SInt16**	 							m_weightsListPtrPtr;
	
	SInt16				 				*m_defaultClassPairWeightPtr;

	SInt16								m_selectedClassCell,
											m_classPairWeightsSelection,
											m_selectedWeightGroupCell;

	UInt16								m_numberOfClassesToUse;
	
	Boolean								m_initializedFlag; 

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WCPWTDLG_H__E331A594_B32A_11D3_8D68_00105AA88EE3__INCLUDED_)
