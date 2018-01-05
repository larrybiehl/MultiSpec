// WEditClassGroupDialog.h : header file
//          
#if !defined __WCSGPDLG_H__
	#define	__WCSGPDLG_H__  
	
	#include "WDialog.h" 
	#include "WLegendList.h"

/////////////////////////////////////////////////////////////////////////////
// CMEditClassGroupDlg dialog

class CMEditClassGroupDlg : public CMDialog
{
// Construction
public:
	CMEditClassGroupDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMEditClassGroupDlg)
	enum { IDD = IDD_EditClassGroupName };
	CString	m_classGroupName;
	//}}AFX_DATA 
	
	Boolean			DoDialog(
							CMLegendList*		legendListCPtr,
							SInt16				selectedCell,
							SInt16				newEditCode,
							SInt16				classGroupIndex,
							Handle				fileInfoHandle,
							Handle				nameHandle);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMEditClassGroupDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()      
	
	TBYTE*			m_classGroupCStringPtr;
	Handle			m_fileInfoHandle; 
	Handle			m_nameHandle;
	SInt16			m_classGroupIndex;
	SInt16			m_newEditCode;
	SInt16			m_numberClassesGroups;
	Boolean			m_initializedFlag;
	Boolean			m_noChangeFlag;
	
}; 
   
#endif	// !defined __WCSGPDLG_H__
