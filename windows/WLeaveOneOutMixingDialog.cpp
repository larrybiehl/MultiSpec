// WLeaveOneOutMixingDialog.cpp : implementation file
//
// Revised by Larry Biehl on 12/21/2017
//

#include "SMultiSpec.h"
                     
#include "WMultiSpec.h"
#include "WLeaveOneOutMixingDialog.h"

//#include	"SExtGlob.h"



extern void 		LOOCOptionsDialogInitialize (
							DialogPtr							dialogPtr,
							SInt16								statsWindowMode,
							SInt16*								mixingParameterCodePtr,
							double*								loocMixingParameterPtr,
							double*								userMixingParameterPtr);

extern void 		LOOCOptionsDialogOK (
							SInt16								statsWindowMode,
							SInt16								mixingParameterCode,
							double								userMixingParameter);



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMLOOMixingDialog dialog


CMLOOMixingDialog::CMLOOMixingDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMLOOMixingDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMLOOMixingDialog)
	m_mixingParameterCode = -1;
	m_userMixingParameter = 0.0;
	//}}AFX_DATA_INIT

	m_initializedFlag = CMDialog::m_initializedFlag;

	m_loocMixingParameter = 0.0;
}


void CMLOOMixingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMLOOMixingDialog)
	DDX_Radio(pDX, IDC_OptimumMixing, m_mixingParameterCode);
	DDX_Text2(pDX, IDC_UserValue, m_userMixingParameter);
	DDV_MinMaxDouble(pDX, m_userMixingParameter, 0., 3.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMLOOMixingDialog, CMDialog)
	//{{AFX_MSG_MAP(CMLOOMixingDialog)
	ON_BN_CLICKED(IDC_UserMixing, OnUserMixing)
	ON_BN_CLICKED(IDC_OptimumMixing, OnOptimumMixing)
	ON_BN_CLICKED(IDC_IdentityMatrix, OnIdentityMatrix)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()   


//-----------------------------------------------------------------------------
//								 Copyright (1988-2000)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the leave one out
//							mixing parameter specification dialog box to the user and 
//							copy the revised back to the principal component 
//							specification structure if the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/07/2000
//	Revised By:			Larry L. Biehl			Date: 03/07/2000	

Boolean 
CMLOOMixingDialog::DoDialog(
				SInt16									statsWindowMode)

{                                 
	INT_PTR						returnCode;
	
	Boolean						continueFlag = FALSE;
								

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																			return(continueFlag);
																					
	m_statsWindowMode = statsWindowMode;
	
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{		
		LOOCOptionsDialogOK (statsWindowMode,
										m_mixingParameterCode+1,
										m_userMixingParameter);

		continueFlag = TRUE;
									
		}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}		// end "DoDialog" 



/////////////////////////////////////////////////////////////////////////////
// CMLOOMixingDialog message handlers

BOOL CMLOOMixingDialog::OnInitDialog() 
{
	SInt16						mixingParameterCode;
	
	CMDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	LOOCOptionsDialogInitialize (this,
											m_statsWindowMode,
											&mixingParameterCode,
											&m_loocMixingParameter,
											&m_userMixingParameter); 

	m_mixingParameterCode = mixingParameterCode - 1;									
			
	LoadDItemRealValue (this, IDC_OptimumValue, m_loocMixingParameter, 3);
	                 
	if (UpdateData(FALSE) )                   
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item	
		                                                     
	SelectDialogItemText (this, IDC_UserValue, 0, SInt16_MAX);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}		// end "OnInitDialog"



void 
CMLOOMixingDialog::OnUserMixing() 
	{
	ShowDialogItem (this, IDC_UserValue);
	SelectDialogItemText (this, IDC_UserValue, 0, SInt16_MAX);

	}		// end "OnUserMixing"



void 
CMLOOMixingDialog::OnOptimumMixing() 
	{
	HideDialogItem (this, IDC_UserValue);
	
	}



void 
CMLOOMixingDialog::OnIdentityMatrix() 
	{
	HideDialogItem (this, IDC_UserValue);
	
	}
