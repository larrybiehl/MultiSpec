// WClassGroupDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMClassGroupDlg dialog   

#if !defined __WCLGRDLG_H__
	#define	__WCLGRDLG_H__ 
	
	#include "WOneColumnDialog.h"

class CMClassGroupDlg : public CMOneColDlg
{
// Construction
public:
	CMClassGroupDlg(CWnd* pParent = NULL);	// standard constructor
	
	Boolean			DoDialog(
							UInt32*						numberOutputItemsPtr, 
							SInt16* 						listPtr, 
							SInt16						minimumNumberItems,
							SInt16						numberInputItems,
							SInt16						classGroupCode);

// Dialog Data
	//{{AFX_DATA(CMClassGroupDlg)        
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMClassGroupDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 
	
	SInt16			m_classGroupCode;
	
	Boolean			m_initializedFlag; 
	
}; 
   
#endif	// !defined __WCLGRDLG_H__
