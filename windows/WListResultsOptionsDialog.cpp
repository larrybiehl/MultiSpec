// WListResultsOptionsDialog.cpp : implementation file
//                    
                   
#include "SMultiSpec.h"
                     
#include "WMultiSpec.h"  
#include "WListResultsOptionsDialog.h"

//#include	"SExtGlob.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMListResultsOptionsDlg dialog


CMListResultsOptionsDlg::CMListResultsOptionsDlg(CWnd* pParent /*=NULL*/)
	: CMDialog(CMListResultsOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMListResultsOptionsDlg)
	m_trainAreasUsedFlag = FALSE;
	m_trainAreaSummariesFlag = FALSE;
	m_trainAreaPerformanceFlag = FALSE;
	m_trainClassPerformanceFlag = FALSE;
	m_testAreasUsedFlag = FALSE;
	m_testAreaSummariesFlag = FALSE;
	m_testAreaPerformanceFlag = FALSE;
	m_testClassPerformanceFlag = FALSE;
	//}}AFX_DATA_INIT

	m_initializedFlag = CMDialog::m_initializedFlag;
}

void CMListResultsOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMListResultsOptionsDlg)
	DDX_Check(pDX, IDC_CHECK1, m_trainAreasUsedFlag);
	DDX_Check(pDX, IDC_CHECK2, m_trainAreaSummariesFlag);
	DDX_Check(pDX, IDC_CHECK3, m_trainAreaPerformanceFlag);
	DDX_Check(pDX, IDC_CHECK4, m_trainClassPerformanceFlag);
	DDX_Check(pDX, IDC_CHECK5, m_testAreasUsedFlag);
	DDX_Check(pDX, IDC_CHECK6, m_testAreaSummariesFlag);
	DDX_Check(pDX, IDC_CHECK7, m_testAreaPerformanceFlag);
	DDX_Check(pDX, IDC_CHECK8, m_testClassPerformanceFlag);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMListResultsOptionsDlg, CMDialog)
	//{{AFX_MSG_MAP(CMListResultsOptionsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()   


//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the classification
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
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/09/97
//	Revised By:			Larry L. Biehl			Date: 04/09/97	

void 
CMListResultsOptionsDlg::DoDialog(
				SInt16*			listResultsTrainingCodePtr, 
				SInt16*			listResultsTestCodePtr)

{  
	Boolean						continueFlag = FALSE;
	
	INT_PTR						returnCode;
								

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																					return;
																			
	m_listResultsTestCode = *listResultsTestCodePtr;		
	m_listResultsTrainingCode = *listResultsTrainingCodePtr;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{              
				// Codes for training and test listings.
						 
		*listResultsTestCodePtr = m_listResultsTestCode;
		*listResultsTrainingCodePtr = m_listResultsTrainingCode;
				
				// List Results options for training areas.	
				
		*listResultsTrainingCodePtr = 0;		
		if ( m_trainAreasUsedFlag )
			*listResultsTrainingCodePtr += kAreasUsed;
							
		if ( m_trainAreaSummariesFlag )
			*listResultsTrainingCodePtr += kAreasSummary;	
						
		if ( m_trainAreaPerformanceFlag )
			*listResultsTrainingCodePtr += kFieldSummary;	
						
		if ( m_trainClassPerformanceFlag )
			*listResultsTrainingCodePtr += kClassSummary;
				
				// List Results options for test areas.	
				
		*listResultsTestCodePtr = 0;		
		if ( m_testAreasUsedFlag )
			*listResultsTestCodePtr += kAreasUsed;
							
		if ( m_testAreaSummariesFlag )
			*listResultsTestCodePtr += kAreasSummary;	
						
		if ( m_testAreaPerformanceFlag )
			*listResultsTestCodePtr += kFieldSummary;	
						
		if ( m_testClassPerformanceFlag )
			*listResultsTestCodePtr += kClassSummary;
				
		continueFlag = TRUE; 
		
		}		// end "if (returnCode == IDOK)"
		
	return;
		
}		// end "DoDialog"


/////////////////////////////////////////////////////////////////////////////
// CMListResultsOptionsDlg message handlers

BOOL CMListResultsOptionsDlg::OnInitDialog()
{
	CMDialog::OnInitDialog(); 
	
			// Set current settings	for training set 
			
	m_trainAreasUsedFlag = ( (m_listResultsTrainingCode & kAreasUsed) > 0);
	m_trainAreaSummariesFlag = ( (m_listResultsTrainingCode & kAreasSummary) > 0);
	m_trainAreaPerformanceFlag = ( (m_listResultsTrainingCode & kFieldSummary) > 0);
	m_trainClassPerformanceFlag = ( (m_listResultsTrainingCode & kClassSummary) > 0);
	
			// Set current settings	for test set	
	
	m_testAreasUsedFlag = ( (m_listResultsTestCode & kAreasUsed) > 0);
	m_testAreaSummariesFlag = ( (m_listResultsTestCode & kAreasSummary) > 0);
	m_testAreaPerformanceFlag = ( (m_listResultsTestCode & kFieldSummary) > 0);
	m_testClassPerformanceFlag = ( (m_listResultsTestCode & kClassSummary) > 0);   
	                 
	if (UpdateData(FALSE) )                   
		PositionDialogWindow ();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"
