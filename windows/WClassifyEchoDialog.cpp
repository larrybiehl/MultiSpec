// WClassifyEchoDialog.cpp : implementation file
//           
// Revised by Larry Biehl on 12/21/2017
//
                   
#include "SMultiSpec.h"
                      
#include "WClassifyEchoDialog.h"

//#include	"SExtGlob.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMEchoClassifyDialog dialog


CMEchoClassifyDialog::CMEchoClassifyDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMEchoClassifyDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMEchoClassifyDialog)
	m_cellWidth = 2;
	m_annexationThreshold = 0;
	m_homogeneityThreshold = 0;
	m_combineLikeFieldsFlag = FALSE;
	m_createHomogeneousFilesFlag = FALSE;
	m_mixCellsFlag = FALSE;
	m_homogeneousThresholdType = 0;
	m_echoAlgorithmProcedure = -1;
	//}}AFX_DATA_INIT
	
	m_minPhase1 = 0.;
	m_maxPhase1 = 100.;
	
	m_initializedFlag = CMDialog::m_initializedFlag; 
	
}		// end "CMEchoClassifyDialog"



CMEchoClassifyDialog::~CMEchoClassifyDialog(void)
{              
	
}		// end "~CMEchoClassifyDialog"



void CMEchoClassifyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMEchoClassifyDialog)
	DDX_Text(pDX, IDC_CellWidth, m_cellWidth);
	DDV_MinMaxLong(pDX, m_cellWidth, 1, 100);
	DDX_Text2(pDX, IDC_AnnexationThreshold, m_annexationThreshold);
	DDV_MinMaxDouble(pDX, m_annexationThreshold, 0., 100.);
	DDX_Text2(pDX, IDC_HomogeneityThreshold, m_homogeneityThreshold);
	DDV_MinMaxDouble(pDX, m_homogeneityThreshold, m_minPhase1, m_maxPhase1);
	DDX_Check(pDX, IDC_CombineLikeFields, m_combineLikeFieldsFlag);
	DDX_Check(pDX, IDC_CreateHomogeneousFiles, m_createHomogeneousFilesFlag);
	DDX_Check(pDX, IDC_MixCells, m_mixCellsFlag);
	DDX_Radio(pDX, IDC_Percent, m_homogeneousThresholdType);
	DDX_CBIndex(pDX, IDC_EchoAlgorithm, m_echoAlgorithmProcedure);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMEchoClassifyDialog, CMDialog)
	//{{AFX_MSG_MAP(CMEchoClassifyDialog)
	ON_BN_CLICKED(IDC_Percent, OnPercent)
	ON_BN_CLICKED(IDC_LogLike, OnLogLike)
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
//	Coded By:			Larry L. Biehl			Date: 04/18/1996
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

SInt16 
CMEchoClassifyDialog::DoDialog(
				EchoClassifierVarPtr				echoClassifierVarPtr)

{  
	Boolean						continueFlag = FALSE; 
	
	INT_PTR						returnCode;

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
																			
	m_echoClassifierVarPtr = echoClassifierVarPtr;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE; 
	
				// Cell Width.													
						
		m_echoClassifierVarPtr->cell_width = (SInt16)m_cellWidth;
												
		m_echoClassifierVarPtr->cell_size = 
			m_echoClassifierVarPtr->cell_width * m_echoClassifierVarPtr->cell_width;
					
				// Homogeneity (phase 1) threshold.						
						
		m_echoClassifierVarPtr->homogeneityThreshold = m_homogeneityThreshold;
				
				// Homogeniety (phase 1) Threshold specified in 		
				// percent probability correct.										
				
		m_echoClassifierVarPtr->fixed_homogeneityThreshold_option = 
																	m_homogeneousThresholdType;
					
				// Annexation threshold.									
						
		m_echoClassifierVarPtr->annexationThreshold = m_annexationThreshold;
	
				// Flag indicating whether to merge like fields.	
				
		m_echoClassifierVarPtr->combineLikeFieldsFlag = 
																	m_combineLikeFieldsFlag; 
	
				// Flag indicating whether to create image files with the	
				// homogeneous fields and classes.									
																
		m_echoClassifierVarPtr->createHomogeneousFilesFlag =  
														m_createHomogeneousFilesFlag; 
	
				// Flag indicating whether to allow merging of 					
				// cells and fields which are of different classes.			
				
		m_echoClassifierVarPtr->mixCellsFlag = m_mixCellsFlag;
		
				// Set classification algorithm to use for echo. 
		
		m_echoClassifierVarPtr->algorithmCode = m_echoAlgorithmProcedure + 1;
		
		}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}		// end "DoDialog"


/////////////////////////////////////////////////////////////////////////////
// CMEchoClassifyDialog message handlers

BOOL CMEchoClassifyDialog::OnInitDialog()
{
	CMDialog::OnInitDialog();
	
			// Cell Width.							
	
	m_cellWidth = m_echoClassifierVarPtr->cell_width;
	
			// Homogeniety Threshold (phase 1).
			// Threshold specified in percent		
			// probability correct. (> 0.0 && < 100.0)
			// or in -Log Likelihood.	 (> 0.0 && < 1000000.0)													
                                 
	m_homogeneityThreshold = m_echoClassifierVarPtr->homogeneityThreshold;
	                        
	m_homogeneousThresholdType = 0;
   if (m_echoClassifierVarPtr->fixed_homogeneityThreshold_option)
		m_homogeneousThresholdType = 1;
						 
	m_minPhase1 = 0; 
	m_savedPhase1PercentThreshold = 2.;
	m_savedPhase1LogLikelihoodThreshold = 95.;
	
	if (m_homogeneousThresholdType == 1)
		{
		m_maxPhase1 = 1000000.;
		m_savedPhase1LogLikelihoodThreshold = m_homogeneityThreshold;
											
		}		// end "if (m_homogeneousThresholdType == 1)" 
		
	else		// m_homogeneousThresholdType == 0
		{
		m_maxPhase1 = 100.;
		m_savedPhase1PercentThreshold = m_homogeneityThreshold;
											
		}		// end "else m_homogeneousThresholdType == 0"
	
			// Annexation Threshold. (>= 0.0 && < 10.0)						
		
	m_annexationThreshold = m_echoClassifierVarPtr->annexationThreshold;
	
			// Flag indicating whether to merge like fields.				
	
	m_combineLikeFieldsFlag = m_echoClassifierVarPtr->combineLikeFieldsFlag;
	
			// Flag indicating whether to create image files with the				
			// homogeneous fields and classes.												
	
	m_createHomogeneousFilesFlag = 
								m_echoClassifierVarPtr->createHomogeneousFilesFlag;
	
			// Flag indicating whether to allow merging of cells and 				
			// fields which are of different classes.										
	                               
	m_mixCellsFlag = m_echoClassifierVarPtr->mixCellsFlag;
	MHideDialogItem (this, IDC_MixCells);
	
			// Classification procedure
			
	m_echoAlgorithmProcedure = m_echoClassifierVarPtr->algorithmCode - 1;
	                  
	if (UpdateData(FALSE) )                   
		PositionDialogWindow ();
		                                       
//	GetDlgItem(IDC_CellWidth)->SetFocus();
//	SendDlgItemMessage( IDC_CellWidth, EM_SETSEL, 0, MAKELPARAM(0, -1) );
	SelectDialogItemText (this, IDC_CellWidth, 0, SInt16_MAX);  			 
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"



void CMEchoClassifyDialog::OnPercent()

{ 			
	if (m_homogeneousThresholdType == 1)
		{                
		DDX_Radio(m_dialogFromPtr, IDC_Percent, m_homogeneousThresholdType); 
		                                                          
		DDX_Text2(m_dialogFromPtr, IDC_HomogeneityThreshold, m_homogeneityThreshold);
		                                
		m_maxPhase1 = 100.;
		m_savedPhase1LogLikelihoodThreshold = m_homogeneityThreshold;
		m_homogeneityThreshold = m_savedPhase1PercentThreshold;  
		                                                          
		DDX_Text2(m_dialogToPtr, IDC_HomogeneityThreshold, m_homogeneityThreshold);
										
		}		// end "if (m_homogeneousThresholdType == 1)"                                                         
	
}		// end "OnPercent"



void CMEchoClassifyDialog::OnLogLike()

{  			
	if (m_homogeneousThresholdType == 0)
		{                                             
		DDX_Radio(m_dialogFromPtr, IDC_Percent, m_homogeneousThresholdType); 
		                                                          
		DDX_Text2(m_dialogFromPtr, IDC_HomogeneityThreshold, m_homogeneityThreshold);
		                                
		m_maxPhase1 = 1000000.;
		m_savedPhase1PercentThreshold = m_homogeneityThreshold;
		m_homogeneityThreshold = m_savedPhase1LogLikelihoodThreshold;
		                                                          
		DDX_Text2(m_dialogToPtr, IDC_HomogeneityThreshold, m_homogeneityThreshold);
									
		}		// end "if (m_homogeneousThresholdType == 0)"                                                         
	
}		// end "OnLogLike"
