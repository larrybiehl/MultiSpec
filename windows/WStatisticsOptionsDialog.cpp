// WStatisticsDialog.cpp : implementation file
//      
// Revised by Larry Biehl on 01/05/2018
//
                   
#include "SMultiSpec.h"
 
#include "WStatisticsOptionsDialog.h"
#include	"WChannelsDialog.h"       

//#include	"SExtGlob.h" 	

extern Boolean	 	StatisticsOptionsDialog (
							SInt16*								statCodePtr, 
							Boolean*								keepClassStatsFlagPtr, 
							double*								variancePtr,
							double*								minLogDeterminantPtr, 
							Boolean*								useCommonCovarianceInLOOCFlagPtr);
						
extern void			StatisticsOptionsDialogOK (
							SInt16								localStatCode,
							Boolean								localKeepStatsFlag,
							Boolean								localZeroVarianceFlag,
							double								localVariance,
							double								localMinLogDeterminant,
							Boolean								localCommonCovarianceInLOOCFlag,
							SInt16*								statCodePtr,
							Boolean*								keepClassStatsFlagPtr,
							double*								variancePtr,
							double*								minLogDeterminantPtr,
							Boolean*								useCommonCovarianceInLOOCFlagPtr);


/////////////////////////////////////////////////////////////////////////////
// CMStatOptionsDlg dialog


CMStatOptionsDlg::CMStatOptionsDlg(CWnd* pParent /*=NULL*/)
	: CMDialog(CMStatOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMStatOptionsDlg)
	m_classStatsOnlyFlag = FALSE;
	m_setZeroVarianceFlag = FALSE;
	m_statCode = -1;
	m_minLogDetValue = 0;
	m_varianceValue = 0;
	m_useCommonCovarianceInLOOCFlag = FALSE;
	//}}AFX_DATA_INIT
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
}



void CMStatOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMStatOptionsDlg)
	DDX_Check(pDX, IDC_ClassStatsOnly, m_classStatsOnlyFlag);
	DDX_Check(pDX, IDC_SetZeroVariance, m_setZeroVarianceFlag);
	DDX_Radio(pDX, IDC_MeanStd, m_statCode);
	DDX_Text2(pDX, IDC_minLogDetValue, m_minLogDetValue);
	DDX_Text2(pDX, IDC_Variance, m_varianceValue);
	DDV_MinMaxDouble(pDX, m_varianceValue, 0., 10000.);
	DDX_Check(pDX, IDC_UseCommonCov, m_useCommonCovarianceInLOOCFlag);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CMStatOptionsDlg, CMDialog)
	//{{AFX_MSG_MAP(CMStatOptionsDlg)
	ON_BN_CLICKED(IDC_MeanStd, OnMeanStd)
	ON_BN_CLICKED(IDC_SetZeroVariance, OnSetZeroVariance)
	ON_BN_CLICKED(IDC_meanStdCov, OnMeanStdCov)
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
//	Coded By:			Larry L. Biehl			Date: 02/13/96
//	Revised By:			Larry L. Biehl			Date: 02/02/98	

SInt16 
CMStatOptionsDlg::DoDialog(
				SInt16*			statCodePtr, 
				Boolean*			keepClassStatsFlagPtr, 
				double*			variancePtr,
				double*			minLogDeterminantPtr,
				Boolean*			useCommonCovarianceInLOOCFlagPtr)

{  
	Boolean			continueFlag = FALSE;
						
	INT_PTR			returnCode; 

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);             
	
			// Statistics to be computed.														
	
	m_statCode = *statCodePtr - 1;
	
			// Keep only class statistics in memory.										
					
	m_classStatsOnlyFlag = *keepClassStatsFlagPtr;
	
	m_setZeroVarianceFlag = (*variancePtr > 0);
		
	m_varianceValue = fabs(*variancePtr);               

			// Minimum log determinant allowed for valid matrix inversion.

	m_minLogDetValue = *minLogDeterminantPtr;
	
	m_useCommonCovarianceInLOOCFlag = *useCommonCovarianceInLOOCFlagPtr; 
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{              
		StatisticsOptionsDialogOK (m_statCode + 1,
												m_classStatsOnlyFlag,
												m_setZeroVarianceFlag,
												m_varianceValue,
												m_minLogDetValue,
												m_useCommonCovarianceInLOOCFlag,
												statCodePtr,
												keepClassStatsFlagPtr,
												variancePtr,
												minLogDeterminantPtr,
												useCommonCovarianceInLOOCFlagPtr);
												
		continueFlag = TRUE;
		
		}		// end "if (returnCode == IDOK)" 
		
	return (continueFlag);
		
}		// end "DoDialog"


/////////////////////////////////////////////////////////////////////////////
// CMStatOptionsDlg message handlers

BOOL CMStatOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	if (m_statCode+1 == kMeanCovariance)
		{					
		SetDLogControl (this, IDC_SetZeroVariance, m_setZeroVarianceFlag );
		                          
		ShowHideDialogItem (this, IDC_Variance, m_setZeroVarianceFlag);
		     
		ShowDialogItem (this, IDC_MinLogPrompt);
		ShowDialogItem (this, IDC_minLogDetValue);
		
		}		// end "if (m_statCode == kMeanCovariance)" 
		
	else		// m_statCode+1 == kMeanStdOnly 
		{
		SetDLogControl (this, IDC_SetZeroVariance, 0 );
		SetDLogControlHilite (this, IDC_SetZeroVariance, 255);
		HideDialogItem (this, IDC_Variance);         
		HideDialogItem (this, IDC_MinLogPrompt);
		HideDialogItem (this, IDC_minLogDetValue);
		                                          
		SetDLogControl (this, IDC_UseCommonCov, 0 );
		SetDLogControlHilite (this, IDC_UseCommonCov, 255);
		
		}		// end "else m_statCode+1 == kMeanStdOnly"
		
			// Center the dialog and then show it.	
		
			// Update the dialog box parameters and then center the dialog.
	                 
	if (UpdateData(FALSE) )
		PositionDialogWindow (); 
	
	SInt16 selectedItem = IDC_minLogDetValue;
	if (m_statCode+1 == kMeanCovariance && m_setZeroVarianceFlag)
		selectedItem = IDC_Variance; 
		
			// Set default text selection to first edit text item 
			                                       
//	GetDlgItem(selectedItem)->SetFocus();
//	SendDlgItemMessage( selectedItem, EM_SETSEL, 0, MAKELPARAM(0, -1) );
	SelectDialogItemText (this, selectedItem, 0, SInt16_MAX);  			
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"



void 
CMStatOptionsDlg::OnMeanStd(void)

{                       
	if ( ProjectMenuClearStatistics() )
		{                                            
		if (m_statCode+1 == kMeanCovariance)                  
			{                    
			SetDLogControl (this, IDC_SetZeroVariance, 0);    
			SetDLogControlHilite (this, IDC_SetZeroVariance, 255);
			HideDialogItem (this, IDC_Variance);     
			HideDialogItem (this, IDC_MinLogPrompt);
			HideDialogItem (this, IDC_minLogDetValue); 
			
			m_statCode = kMeanStdDevOnly - 1;    
							
			}		// end "if ( m_statCode+1 == kMeanCovariance )"
			
		else		// m_statCode+1 == kMeanStdDevOnly
			{  
			SetDLogControl (this, IDC_SetZeroVariance, m_setZeroVarianceFlag);
			SetDLogControlHilite (this, IDC_SetZeroVariance, 0);             
	                           
			SetZeroVariance();
				 
			ShowDialogItem (this, IDC_MinLogPrompt);
			ShowDialogItem (this, IDC_minLogDetValue);
			
			m_statCode = kMeanCovariance - 1;
			
			}		// end "else m_statCode+1 == kMeanStdDevOnly"
							
		}		// end "if ( ProjectMenuClearStatistics () )"
		
	else		// !ProjectMenuClearStatistics()
				// User cancelled the operation. Set back to previous value.	 
		DDX_Radio(m_dialogToPtr, IDC_MeanStd, m_statCode);   
	
}		// end "OnMeanStd" 



void 
CMStatOptionsDlg::OnSetZeroVariance(void)

{                                                          
	DDX_Check(m_dialogFromPtr, IDC_SetZeroVariance, m_setZeroVarianceFlag);
	                           
	SetZeroVariance();
	
}		// end "OnSetZeroVariance"

  

void 
CMStatOptionsDlg::OnMeanStdCov(void)

{
	OnMeanStd ();
	
}		// end "OnMeanStdCov" 



void CMStatOptionsDlg::SetZeroVariance(void)

{                                                                          
	ShowHideDialogItem (this, IDC_Variance, m_setZeroVarianceFlag);
	if (m_setZeroVarianceFlag)
		{                                                                                               
//		GetDlgItem(IDC_Variance)->SetFocus();
//		SendDlgItemMessage( IDC_Variance, EM_SETSEL, 0, MAKELPARAM(0, -1) );
		SelectDialogItemText (this, IDC_Variance, 0, SInt16_MAX);  			
		
		}		// end "if (m_setZeroVarianceFlag)"
	
}		// end "SetZeroVariance"