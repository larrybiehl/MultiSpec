// WClassesDialog.h : header file
//             

#if !defined __WCLASDLG_H__
	#define	__WCLASDLG_H__  
	
	#include "WOneColumnDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMClassesDlg dialog

class CMClassesDlg : public CMOneColDlg
{
// Construction
public:
	CMClassesDlg(CWnd* pParent = NULL);	// standard constructor 
	
	Boolean				DoDialog(
								UInt32*						numberOutputClassesPtr, 
								SInt16* 						classListPtr, 
								SInt16						minimumNumberClasses,
								SInt16						numberInputClasses);

// Dialog Data
	//{{AFX_DATA(CMClassesDlg)
	enum { IDD = IDD_Channels };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMClassesDlg)
	virtual BOOL OnInitDialog();                        
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 
	
	Boolean			m_initializedFlag; 
	
};                     
   
#endif	// !defined __WCLASDLG_H__
