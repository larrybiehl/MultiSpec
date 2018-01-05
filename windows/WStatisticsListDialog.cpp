// WStatisticsListDialog.cpp : implementation file
//
                   
#include "SMultiSpec.h"
#include "WStatisticsListDialog.h"

extern void 		ListStatsDialogInitialize (
							DialogPtr							dialogPtr,
							SInt16								statsWindowMode,
							Boolean*								listFieldFlagPtr,
							Boolean*								listClassFlagPtr,
							Boolean*								listMeansStdDevFlagPtr,
							Boolean*								listCovarianceFlagPtr,
							Boolean*								listCorrelationFlagPtr,
							Boolean*								featureTransformationFlagPtr,
							SInt16*								listMeanStdPrecisionPtr,
							SInt16*								listCovCorPrecisionPtr);
							
extern void 		ListStatsDialogOK (
							Boolean								listFieldFlag,
							Boolean								listClassFlag,
							Boolean								listMeansStdDevFlag,
							Boolean								listCovarianceFlag,
							Boolean								listCorrelationFlag,
							Boolean								featureTransformationFlag,
							SInt16								listMeanStdPrecision,
							SInt16								listCovCorPrecision);

/////////////////////////////////////////////////////////////////////////////
// CMListStatsDialog dialog


CMListStatsDialog::CMListStatsDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMListStatsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMListStatsDialog)
	m_listClassFlag = FALSE;
	m_listFieldFlag = FALSE;
	m_listMeansStdDevFlag = FALSE;
	m_listCovarianceFlag = FALSE;
	m_listCorrelationFlag = FALSE;
	m_featureTransformationFlag = FALSE;
	m_listMeanStdPrecision = 1;
	m_listCovCorPrecision = 2;
	//}}AFX_DATA_INIT

	m_initializedFlag = CMDialog::m_initializedFlag;
}

void CMListStatsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMListStatsDialog)
	DDX_Check(pDX, IDC_Classes, m_listClassFlag);
	DDX_Check(pDX, IDC_Fields, m_listFieldFlag);
	DDX_Check(pDX, IDC_MeansStdDev, m_listMeansStdDevFlag);
	DDX_Check(pDX, IDC_CovarianceMatrix, m_listCovarianceFlag);
	DDX_Check(pDX, IDC_CorrelationMatrix, m_listCorrelationFlag);
	DDX_Check(pDX, IDC_UseTransformation, m_featureTransformationFlag);
	DDX_Text(pDX, IDC_MeanPrecision, m_listMeanStdPrecision);
	DDV_MinMaxUInt(pDX, m_listMeanStdPrecision, 1, 10);
	DDX_Text(pDX, IDC_CovariancePrecision, m_listCovCorPrecision);
	DDV_MinMaxUInt(pDX, m_listCovCorPrecision, 1, 10);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMListStatsDialog, CMDialog)
	//{{AFX_MSG_MAP(CMListStatsDialog)
	ON_BN_CLICKED(IDC_Classes, OnClasses)
	ON_BN_CLICKED(IDC_Fields, OnFields)
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
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/10/98
//	Revised By:			Larry L. Biehl			Date: 04/16/98	

Boolean 
CMListStatsDialog::DoDialog(
			SInt16									statsWindowMode)

{  
	// oul: changed from SInt16 to SInt64
	SInt64								returnCode;
	
	Boolean								continueFlag = FALSE;

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE); 
	
	m_statsWindowMode = statsWindowMode;
																			
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE; 
				
		ListStatsDialogOK ( m_listFieldFlag,
									m_listClassFlag,
									m_listMeansStdDevFlag,
									m_listCovarianceFlag,
									m_listCorrelationFlag,
									m_featureTransformationFlag,
									m_listMeanStdPrecision,
									m_listCovCorPrecision);
		
		}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}		// end "DoDialog"


/////////////////////////////////////////////////////////////////////////////
// CMListStatsDialog message handlers

BOOL CMListStatsDialog::OnInitDialog()
{
	SInt16								listCovCorPrecision,
											listMeanStdPrecision;
											
											
	CDialog::OnInitDialog();
	

	ListStatsDialogInitialize (this,
											m_statsWindowMode,
											(Boolean*)&m_listFieldFlag,
											(Boolean*)&m_listClassFlag,
											(Boolean*)&m_listMeansStdDevFlag,
											(Boolean*)&m_listCovarianceFlag,
											(Boolean*)&m_listCorrelationFlag,
											(Boolean*)&m_featureTransformationFlag,
											&listMeanStdPrecision,
											&listCovCorPrecision);
											
	m_listMeanStdPrecision = listMeanStdPrecision; 
	m_listCovCorPrecision = listCovCorPrecision;
	                 
	if (UpdateData(FALSE) )                   
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item	
		                                       
	SelectDialogItemText (this, IDC_MeanPrecision, 0, SInt16_MAX);  			
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"



void 
CMListStatsDialog::CheckListFieldClassSettings(void)

{  
	Boolean		enableFlag = FALSE;
	
	                                                                                     
	if (m_listFieldFlag || 
				m_listClassFlag)
		enableFlag = TRUE;
	
	GetDlgItem(IDOK)->EnableWindow(enableFlag); 
	
}		// end "CheckListFieldClassSettings" 

  

void CMListStatsDialog::OnClasses()
{                                                                   
	DDX_Check(m_dialogFromPtr, 
					IDC_Classes, 
					m_listClassFlag); 
					
	CheckListFieldClassSettings ();
	
}		// end "OnClasses" 



void CMListStatsDialog::OnFields()
{                                                               
	DDX_Check(m_dialogFromPtr, 
					IDC_Fields, 
					m_listFieldFlag); 
					                               
	CheckListFieldClassSettings ();
	
}		// end "OnFields"
