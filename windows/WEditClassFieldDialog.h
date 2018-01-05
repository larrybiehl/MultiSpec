// WEditClassFieldDialog.h : header file
//      
#if !defined __WEDITCLASSFIELD_H__
	#define	__WEDITCLASSFIELD_H__  
	
	#include "WDialog.h"   

/////////////////////////////////////////////////////////////////////////////
// WEditClassFieldDlg dialog

class CMEditClassFieldDlg : public CMDialog
{
// Construction
public:
	CMEditClassFieldDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMEditClassFieldDlg)
	enum { IDD = IDD_ClassField };
	CString	m_className;
	CString	m_fieldName;
	int		m_fieldType;
	//}}AFX_DATA
	
	Boolean			DoDialog(
							SInt16					classFieldCode, 
							SInt16					currentClass,
							SInt16					currentField,
							char*						titleStringPtr);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMEditClassFieldDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();               
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()      
	
	TBYTE*			m_classNameCStringPtr;
	TBYTE*			m_fieldNameCStringPtr;
	char*				m_titleStringPtr;
	
	Boolean			m_initializedFlag;
	SInt16			m_classFieldCode; 
	SInt16			m_currentField;
	SInt16			m_classStorage;
	SInt16*			m_fieldTypePtr; 
	
};            

#endif // !defined __WEDITCLASSFIELD_H__ 
