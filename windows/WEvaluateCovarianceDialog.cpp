// WEvaluateCovarianceDialog.cpp : implementation file
//   
// Revised by Larry Biehl on 12/21/2017
//
                   
#include "SMultiSpec.h"                     
#include "WEvaluateCovarianceDialog.h"
//#include	"SExtGlob.h"   	

extern void 		EvaluateCovariancesDialogOK (
							EvaluateCovarianceSpecsPtr		evaluateCovarianceSpecsPtr,
							Boolean								listOriginalMatrixFlag,
							Boolean								listInvertedMatrixFlag,
							Boolean								listOriginalXInvertedMatrixFlag,
							Boolean								listInvertedInvertedMatrixFlag,
							Boolean								featureTransformationFlag);


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMEvalCovarianceDialog dialog


CMEvalCovarianceDialog::CMEvalCovarianceDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMEvalCovarianceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMEvalCovarianceDialog)
	m_listOriginalMatrixFlag = FALSE;
	m_listInvertedMatrixFlag = FALSE;
	m_listInvertedInvertedMatrixFlag = FALSE;
	m_listOriginalXInvertedMatrixFlag = FALSE;
	m_featureTransformationFlag = FALSE;
	//}}AFX_DATA_INIT 
	
	m_evaluateCovarianceSpecsPtr = NULL;
	m_featureTransformAllowedFlag = FALSE;
	m_numberEigenvectors = 0;
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
	if (m_initializedFlag)
		{                          
				// Get pointer to memory for temporary storage of channel list.
				
		WindowInfoPtr projectWindowInfoPtr = (WindowInfoPtr)GetHandlePointer(
								gProjectInfoPtr->windowInfoHandle, kNoLock, kNoMoveHi);
		SInt16 totalNumberChannels = projectWindowInfoPtr->totalNumberChannels; 
			                                                                       
		m_localFeaturesPtr = (UInt16*)MNewPointer ( 
											(UInt32)totalNumberChannels * sizeof(UInt16) ); 
	                                                       
		m_initializedFlag = (m_localFeaturesPtr != NULL);
		
		}		// end "if (m_initializedFlag)" 
	
	if (m_initializedFlag)
		{
				// Get memory for the local class list vector.	
				
		m_classListPtr = (UInt16*)MNewPointer ( 
				(UInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof(UInt16) );
	                                                       
		m_initializedFlag = (m_classListPtr != NULL);
		
		}		// end "if (m_initializedFlag)"  
	
}		// end "CMEvalCovarianceDialog"



CMEvalCovarianceDialog::~CMEvalCovarianceDialog(void)

{                                                               
	m_classListPtr = CheckAndDisposePtr (m_classListPtr);
	m_localFeaturesPtr = CheckAndDisposePtr (m_localFeaturesPtr); 
	
}		// end "~CMEvalCovarianceDialog"



void CMEvalCovarianceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMEvalCovarianceDialog)
	DDX_Check(pDX, IDC_OrigMatrix, m_listOriginalMatrixFlag);
	DDX_Check(pDX, IDC_InvertedMatrix, m_listInvertedMatrixFlag);
	DDX_Check(pDX, IDC_InvertedInverted, m_listInvertedInvertedMatrixFlag);
	DDX_Check(pDX, IDC_Identity, m_listOriginalXInvertedMatrixFlag);
	DDX_Check(pDX, IDC_Transformation, m_featureTransformationFlag); 
	DDX_CBIndex(pDX, IDC_ChannelCombo, m_channelSelection);
	DDX_CBIndex(pDX, IDC_ClassCombo, m_classSelection);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CMEvalCovarianceDialog, CMDialog)
	//{{AFX_MSG_MAP(CMEvalCovarianceDialog)
	ON_CBN_SELENDOK(IDC_ChannelCombo, OnSelendokChannelCombo) 
	ON_CBN_SELENDOK(IDC_ClassCombo, OnSelendokClassCombo)
	ON_BN_CLICKED(IDC_Transformation, OnTransformation)
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
//	Coded By:			Larry L. Biehl			Date: 04/26/1996
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean 
CMEvalCovarianceDialog::DoDialog(
				EvaluateCovarianceSpecsPtr		evaluateCovarianceSpecsPtr)

{  
	Boolean						continueFlag = FALSE; 
	
	INT_PTR						returnCode;
	
	UInt16						index;						

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
																			
	m_evaluateCovarianceSpecsPtr = evaluateCovarianceSpecsPtr;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE;
		
		gChannelSelection = m_channelSelection; 
		if (m_channelSelection == kSubsetMenuItem)	// Subset of channels or features 
			{				    		                                      
			evaluateCovarianceSpecsPtr->numberFeatures = m_localActiveNumberFeatures; 
			for ( index=0; index<(UInt16)m_localNumberFeatures; index++)
				evaluateCovarianceSpecsPtr->featurePtr[index] = m_localFeaturesPtr[index];
												
			}		// end "else m_channelSelection == kSubsetMenuItem"  
		
		gClassSelection = m_classSelection;
		if (m_classSelection == kSubsetMenuItem) 
			{                                          
			evaluateCovarianceSpecsPtr->numberClasses = m_localNumberClasses;               
			for (index=0; index<(UInt16)m_localNumberClasses; index++)
				evaluateCovarianceSpecsPtr->classPtr[index] = m_classListPtr[index]; 
			
			}		// end "else m_classSelection == kSubsetMenuItem"	

		EvaluateCovariancesDialogOK (
							evaluateCovarianceSpecsPtr,
							m_listOriginalMatrixFlag,
							m_listInvertedMatrixFlag,
							m_listOriginalXInvertedMatrixFlag,
							m_listInvertedInvertedMatrixFlag,
							m_featureTransformationFlag);
		
		}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}		// end "DoDialog" 



/////////////////////////////////////////////////////////////////////////////
// CMEvalCovarianceDialog message handlers

BOOL CMEvalCovarianceDialog::OnInitDialog()

{
	Boolean					tempFlag; 
	
	UInt16					index;
	
	
	CMDialog::OnInitDialog(); 
	
			// Set flag for listing the original covariance matrix.		
			
	m_listOriginalMatrixFlag = m_evaluateCovarianceSpecsPtr->listOriginalMatrixFlag;
	
			// Set flag for listing the inverted matrix.					
			
	m_listInvertedMatrixFlag = m_evaluateCovarianceSpecsPtr->listInvertedMatrixFlag;
	
			// Set flag for listing the original covariance matrix		
			//          multiplied by the inverted matrix, i.e. identity matrix.	
			
	m_listOriginalXInvertedMatrixFlag = 
									m_evaluateCovarianceSpecsPtr->listOriginalXInvertedMatrixFlag;
	
			// Set flag for listing the inverted-inverted covariance matrix.																	
			
	m_listInvertedInvertedMatrixFlag = 
									m_evaluateCovarianceSpecsPtr->listInvertedInvertedMatrixFlag;
		
			// Set the check box for the feature transformation option.				
			
	EigenvectorInfoExists (kProject, NULL, &m_numberEigenvectors);
	m_featureTransformAllowedFlag = (m_numberEigenvectors > 0);
	if (m_numberEigenvectors <= 0)
		{		
		m_featureTransformationFlag = FALSE;
		MHideDialogItem (this, IDC_Transformation);
		
		}		// end "if (numberEigenvectors <= 0)" 
		
	else		// numberEigenvectors > 0 
		m_featureTransformationFlag = m_evaluateCovarianceSpecsPtr->featureTransformationFlag;
									
	tempFlag = CheckFeatureTransformationDialog (
							this, 
							m_featureTransformAllowedFlag,
							IDC_Transformation, 
							IDC_Channels, 
							(SInt16*)&m_featureTransformationFlag);
	                                                        							
			//	Channels to use													
			                                                                   
	m_channelSelection = m_evaluateCovarianceSpecsPtr->channelSet;
	m_localNumberFeatures = m_evaluateCovarianceSpecsPtr->numberChannels; 
	for (index=0; index<m_localNumberFeatures; index++)
		m_localFeaturesPtr[index] = m_evaluateCovarianceSpecsPtr->featurePtr[index];
	
			// Set feature parameters 
			
	m_localActiveNumberFeatures =	m_localNumberFeatures; 
	m_localActiveFeaturesPtr = m_localFeaturesPtr; 
	                                                                   			
			//	Set the channels/features list item				
			                                             
	m_availableFeaturePtr = gProjectInfoPtr->channelsPtr;
	                                                      								
			//	Classes to use													
			                                                                      
	m_classSelection = m_evaluateCovarianceSpecsPtr->classSet;
	m_localNumberClasses = m_evaluateCovarianceSpecsPtr->numberClasses;
	for (index=0; index<m_localNumberClasses; index++)
		m_classListPtr[index] = m_evaluateCovarianceSpecsPtr->classPtr[index];  
	                  
	if (UpdateData(FALSE) )                   
		PositionDialogWindow ();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	
} 



void CMEvalCovarianceDialog::OnTransformation()
{
	// TODO: Add your control notification handler code here
	
}  

  

void CMEvalCovarianceDialog::OnSelendokChannelCombo()
{                                                           
	HandleChannelsMenu(IDC_ChannelCombo, 
								m_featureTransformationFlag,
								(SInt16)gProjectInfoPtr->numberStatisticsChannels,
								2,
								TRUE);
	
}		// end "OnSelendokChannelCombo"  



void CMEvalCovarianceDialog::OnSelendokClassCombo()
{                                                                                
	HandleClassesMenu( &m_localNumberClasses,
								(SInt16*)m_classListPtr,
								1,      
								(SInt16)gProjectInfoPtr->numberStatisticsClasses, 
								IDC_ClassCombo,
								&m_classSelection);
	
}		// end "OnSelendokClassCombo" 
