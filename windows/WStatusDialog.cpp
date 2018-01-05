// WStatusDialog.cpp : implementation file
//
                   
#include "SMultiSpec.h"
#include "WStatusDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatDlg dialog


CShortStatusDlg::CShortStatusDlg(UInt16 identifier, CWnd* pParent /*=NULL*/)
	: CMDialog(identifier, pParent)
{
	//{{AFX_DATA_INIT(CShortStatusDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
			// Add this here so that the window is automatically
			// created (i.e., displayed) when the C++ object
			// is constructed. 

	m_initializedFlag = CMDialog::m_initializedFlag; 
	
	if (m_initializedFlag)		
		m_initializedFlag = Create(identifier, pParent);
} 



void CShortStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CShortStatusDlg, CMDialog)
	//{{AFX_MSG_MAP(CShortStatusDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP() 


/////////////////////////////////////////////////////////////////////////////
// CShortStatusDlg message handlers


// Over-ride of the virtual function PostNCDestroy() so that 
// the 'CDlgModeless' C++ object can be deleted.
// This ensures that when the window is destroyed, the C++ object
// is also deleted.
// The addition of the 'delete this' effectively adds auto-cleanup
// to the 'CShortStatusDlg' class.

void 
CShortStatusDlg::PostNcDestroy()
{
	// Delete the 'CDlgModeless' object.
	delete this ;
	
	// Perform the other necessary destroy functions.
	CWnd::PostNcDestroy() ;
}  



BOOL CShortStatusDlg::OnInitDialog()
{
	CDialog::OnInitDialog(); 
	
	PositionDialogWindow (); 
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

                       

void CShortStatusDlg::OnCancel()
{
	// TODO: Add extra cleanup here
	
	CMDialog::OnCancel();
}
