// WColorDialog.cpp : implementation file
//
                    
#include "SMultiSpec.h"
                     
#include "WMultiSpec.h"
#include "WColorDialog.h"

//#include	"SExtGlob.h" 

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMColorDialog dialog


CMColorDialog::CMColorDialog(COLORREF clrInit /* = 0 */,
		DWORD dwFlags /* = 0 */,
		CWnd* pParentWnd /* = NULL */) : CColorDialog(clrInit, dwFlags, pParentWnd)
{
	//{{AFX_DATA_INIT(CMColorDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CMColorDialog::DoDataExchange(CDataExchange* pDX)
{
	CColorDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMColorDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMColorDialog, CColorDialog)
	//{{AFX_MSG_MAP(CMColorDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMColorDialog message handlers

BOOL CMColorDialog::OnInitDialog()
{
	CColorDialog::OnInitDialog();
	                                                      
	PositionDialogWindow (this);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
