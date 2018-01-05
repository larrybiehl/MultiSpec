// WClassifyCorrelationDialog.cpp : implementation file
//    
// Revised by Larry Biehl on 01/04/2018
//
                   
#include "SMultiSpec.h"
                     
#include "WMultiSpec.h"  
#include "WClassifyCorrelationDialog.h"

//#include	"SExtGlob.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMCorrelationClassifyDialog dialog

CMCorrelationClassifyDialog::CMCorrelationClassifyDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMCorrelationClassifyDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMCorrelationClassifyDialog)
	m_covarianceEstimate = -1;
	//}}AFX_DATA_INIT
	
	m_initializedFlag = CMDialog::m_initializedFlag;
}



void CMCorrelationClassifyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMCorrelationClassifyDialog)
	DDX_CBIndex(pDX, IDC_CovarianceCombo, m_covarianceEstimate);
	//}}AFX_DATA_MAP
}
  
  
  
BEGIN_MESSAGE_MAP(CMCorrelationClassifyDialog, CMDialog)
	//{{AFX_MSG_MAP(CMCorrelationClassifyDialog)
	ON_CBN_SELENDOK(IDC_CovarianceCombo, OnSelendokCovarianceCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()  


//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the CEM
//							specification dialog box to the user and copy the
//							revised back to the classify specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/09/1998
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean 
CMCorrelationClassifyDialog::DoDialog(
				SInt16*								covarianceEstimatePtr)

{  
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE;
								

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
																			
	m_covarianceEstimate = *covarianceEstimatePtr - 1;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE;
		
		*covarianceEstimatePtr = m_covarianceEstimate + 1; 
		
		}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}		// end "DoDialog"


/////////////////////////////////////////////////////////////////////////////
// CMCorrelationClassifyDialog message handlers 

BOOL CMCorrelationClassifyDialog::OnInitDialog()
{
	CMDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMCorrelationClassifyDialog::OnSelendokCovarianceCombo()
{
	// TODO: Add your control notification handler code here
	
}  
