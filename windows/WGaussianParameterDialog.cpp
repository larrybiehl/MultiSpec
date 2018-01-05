// WGaussianParameterDialog.cpp : implementation file
//
// Revised by Larry Biehl on 12/21/2017
//

#include "SMultiSpec.h"                   
#include "WGaussianParameterDialog.h"
//#include	"SExtGlob.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMGaussianParameterDlg dialog


CMGaussianParameterDlg::CMGaussianParameterDlg(CWnd* pParent /*=NULL*/)
	: CMDialog(CMGaussianParameterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMGaussianParameterDlg)
	m_gaussianStretch = 0.0;
	//}}AFX_DATA_INIT

	m_initializedFlag = CMDialog::m_initializedFlag;
}


void CMGaussianParameterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMGaussianParameterDlg)
	DDX_Text2(pDX, IDC_GaussianStretch, m_gaussianStretch);
	DDV_MinMaxDouble(pDX, m_gaussianStretch, 1.e-004, 1000.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMGaussianParameterDlg, CMDialog)
	//{{AFX_MSG_MAP(CMGaussianParameterDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the display
//							gaussian parameter dialog box to the user and copy the
//							revised parameter to the display specification dialog 
//							variables if the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/07/2003
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean 
CMGaussianParameterDlg::DoDialog(
				double*								gaussianStretchPtr)
{  
	INT_PTR			returnCode;

	Boolean			continueFlag = FALSE;
	 
	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
																			
	m_gaussianStretch = *gaussianStretchPtr;																		 
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{   		
		*gaussianStretchPtr = m_gaussianStretch;																		 
		 
		continueFlag = TRUE;
		
		}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}		// end "DoDialog" 


/////////////////////////////////////////////////////////////////////////////
// CMGaussianParameterDlg message handlers

BOOL CMGaussianParameterDlg::OnInitDialog() 
{
	CMDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
			// Set default text selection to first edit text item	
		                                       
	SelectDialogItemText (this, IDC_GaussianStretch, 0, SInt16_MAX); 
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
