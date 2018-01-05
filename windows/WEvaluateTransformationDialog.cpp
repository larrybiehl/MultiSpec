// WEvaluateTransformationDialog.cpp : implementation file
//                  
                   
#include "SMultiSpec.h"
                      
#include "WEvaluateTransformationDialog.h"

//#include	"SExtGlob.h"  

extern void 		EvaluateTransformationInitialize (
							DialogPtr							dialogPtr,
							EvaluateTransformSpecsPtr		evaluateTransformSpecsPtr,
							Boolean*								listEigenvaluesFlagPtr,
							Boolean*								listOriginalMatrixFlagPtr,
							Boolean*								checkTransformationFlagPtr,
							Boolean*								listOriginalXInvertedMatrixFlagPtr);
							
extern void 		EvaluateTransformationOK (
							EvaluateTransformSpecsPtr		evaluateTransformSpecsPtr,
							Boolean								listEigenvaluesFlag,
							Boolean								listOriginalMatrixFlag,
							Boolean								checkTransformationFlag,
							Boolean								listOriginalXInvertedMatrixFlag);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMEvalTransformDialog dialog


CMEvalTransformDialog::CMEvalTransformDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMEvalTransformDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMEvalTransformDialog)
	m_checkTransformationFlag = FALSE;
	m_listCheckMatrixFlag = FALSE;
	m_listEigenvaluesFlag = FALSE;
	m_listOriginalMatrixFlag = FALSE;
	//}}AFX_DATA_INIT
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
}		// end "CMEvalTransformDialog"



void CMEvalTransformDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMEvalTransformDialog)
	DDX_Check(pDX, IDC_CheckTransform, m_checkTransformationFlag);
	DDX_Check(pDX, IDC_ListCheckMatrix, m_listCheckMatrixFlag);
	DDX_Check(pDX, IDC_ListEigenvalues, m_listEigenvaluesFlag);
	DDX_Check(pDX, IDC_ListTransform, m_listOriginalMatrixFlag);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CMEvalTransformDialog, CMDialog)
	//{{AFX_MSG_MAP(CMEvalTransformDialog)
	ON_BN_CLICKED(IDC_CheckTransform, OnCheckTransform)
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
//							specification dialog box to the user and copy the
//							revised back to the display specification structure if
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
//	Coded By:			Larry L. Biehl			Date: 05/20/97
//	Revised By:			Larry L. Biehl			Date: 05/13/98	

Boolean 
CMEvalTransformDialog::DoDialog(
				EvaluateTransformSpecsPtr		evaluateTransformSpecsPtr)

{  
	Boolean						continueFlag = FALSE; 
	
	// oul: changed from SInt16 to SInt64
	SInt64						returnCode;					

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
																			
	m_evaluateTransformSpecsPtr = evaluateTransformSpecsPtr;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE;
					
				// List options.	
				
		EvaluateTransformationOK (evaluateTransformSpecsPtr,
											m_listEigenvaluesFlag,
											m_listOriginalMatrixFlag,
											m_checkTransformationFlag,
											m_listCheckMatrixFlag );														

		}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}		// end "DoDialog"



/////////////////////////////////////////////////////////////////////////////
// CMEvalTransformDialog message handlers

BOOL 
CMEvalTransformDialog::OnInitDialog()

{
	CMDialog::OnInitDialog();  
	
			// Initialize dialog variables.

	EvaluateTransformationInitialize (this,
													m_evaluateTransformSpecsPtr,
													(Boolean*)&m_listEigenvaluesFlag,
													(Boolean*)&m_listOriginalMatrixFlag,
													(Boolean*)&m_checkTransformationFlag,
													(Boolean*)&m_listCheckMatrixFlag);
	                                                                       
	SetListCheckTransform (m_checkTransformationFlag);
	                  
	if (UpdateData(FALSE) )                   
		PositionDialogWindow ();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"



void 
CMEvalTransformDialog::OnCheckTransform()

{                                                                                          
	DDX_Check(m_dialogFromPtr, IDC_CheckTransform, m_checkTransformationFlag);
	
	SetListCheckTransform (m_checkTransformationFlag);
	
}		// end "OnCheckTransform"     



void 
CMEvalTransformDialog::SetListCheckTransform(
				Boolean				checkTransformFlag)

{                                                                            
	if (checkTransformFlag)
		{ 
		MShowDialogItem (this, IDC_ListCheckMatrix);
		MShowDialogItem (this, IDC_ListCheckMatrix2);
		
		}		// end "if (checkTransformFlag)"
		
	else		// !checkTransformFlag
		{   
		MHideDialogItem (this, IDC_ListCheckMatrix); 
		MHideDialogItem (this, IDC_ListCheckMatrix2); 
		
		}		// end "else !checkTransformFlag"
	
}		// end "SetListCheckTransform"
