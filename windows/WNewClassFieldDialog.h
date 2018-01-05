// WNewClassFieldDialog.h : header file
// 
// Revised by Larry Biehl on 03/16/017
//
#if !defined __WNEWCLS_H__
	#define	__WNEWCLS_H__  
	
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// WEditClassFieldDlg dialog

class CMNewClassFieldDlg : public CMDialog
{
// Construction
public:
	CMNewClassFieldDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMNewClassFieldDlg)
	enum { IDD = IDD_ClassField };
	int		m_classList;
	CString	m_className;
	CString	m_fieldName;
	int		m_fieldType;
	//}}AFX_DATA
	
	Boolean			DoDialog(
							Boolean					newClassOnlyFlag, 
							UCharPtr					classNamePtr,
							UCharPtr					fieldNamePtr, 
							SInt16*		 			fieldTypePtr);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMNewClassFieldDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelendokClassList();
	afx_msg void OnFieldType(); 
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()      
	
	TBYTE*			m_classNameCStringPtr;
	TBYTE*			m_fieldNameCStringPtr;
	
	Boolean			m_initializedFlag;
	Boolean			m_newClassOnlyFlag; 
	UCharPtr			m_classNamePtr;
	UCharPtr			m_fieldNamePtr;
	SInt16*			m_fieldTypePtr;
	
	SInt64			m_numberSelectionPixels; 
	
};  
   
#endif	// !defined __WNEWCLS_H__
