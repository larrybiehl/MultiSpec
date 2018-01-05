// WClusterIsodataDialog.cpp : implementation file
//
// Revised by Larry Biehl on 12/21/2017
//
                    
#include "SMultiSpec.h"       
#include "WClusterIsodataDialog.h"
//#include	"SExtGlob.h" 

extern void 		ISODATAClusterDialogInitialize (
							DialogPtr							dialogPtr,
							UInt16*								localClassAreaPtr,
							SInt16*								initializationOptionPtr,
							Boolean*								useCorrelationMatrixFlagPtr,
							SInt16*								projectClassMeansCodePtr,
							SInt16*								clustersFromPtr,
							SInt16*								classAreaSelectionPtr,
							UInt32*								numberClassAreasPtr,
							DialogSelectArea*					dialogSelectAreaPtr,
							SInt32*								maxNumberClustersPtr,
							double*								convergencePtr,
							SInt32*								minClusterSizePtr,
							double*								criticalDistance1Ptr,
							double*								criticalDistance2Ptr,
							UInt16*								distanceDecimalDigitsPtr);

extern void 		ISODATAClusterDialogOK (
							SInt16								initializationOption,
							Boolean								useCorrelationMatrixFlag,
							Boolean								projectClassMeansFlag,
							SInt16								clustersFrom,
							SInt16								classAreaSelection,
							UInt16*								localClassAreaPtr,
							UInt32								localNumberClassAreas,
							DialogSelectArea*					dialogSelectAreaPtr,
							SInt16								maxNumberClusters,
							double								convergence,
							SInt16								minClusterSize,
							double								criticalDistance1,
							double								criticalDistance2);

extern void 		ISODATAClusterDialogOnImageArea (
							DialogPtr							dialogPtr);

extern void 		ISODATAClusterDialogOnTrainingAreas (
							DialogPtr							dialogPtr);

extern SInt16		ISODATAClusterDialogCheckNumberPixels (
							SInt16								itemHit,
							SInt16								clustersFrom,
							DialogSelectArea*					dialogSelectAreaPtr);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMISODATAClusterDialog dialog


CMISODATAClusterDialog::CMISODATAClusterDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMISODATAClusterDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMISODATAClusterDialog)
	m_initializationOption = -1;
	m_criticalDistance1 = 1;
	m_criticalDistance2 = 1;
	m_numberClusters = 1;
	m_minClusterSize = 1;
	m_convergence = 0;
	m_classSelection = -1;
	m_projectClassMeansFlag = FALSE;
	//}}AFX_DATA_INIT
	
	m_initializedFlag = CMDialog::m_initializedFlag; 
	
	if (m_initializedFlag)
		{
				// Get memory for the local class list vector if it may be needed.	
		
		if (gProjectInfoPtr->numberStatisticsClasses > 0)
			{		
			m_classListPtr = (UInt16*)MNewPointer ( 
				(UInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof(UInt16) );
	                                                       
			m_initializedFlag = (m_classListPtr != NULL);
			
			}		// end "if (...->numberStatisticsClasses > 0)"
		
		}		// end "if (m_initializedFlag)"  
		
}		// end "CMISODATAClusterDialog" 



CMISODATAClusterDialog::~CMISODATAClusterDialog(void)
{        
	m_classListPtr = CheckAndDisposePtr (m_classListPtr);                                         
	
}		// end "~CMISODATAClusterDialog" 



void CMISODATAClusterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMISODATAClusterDialog)
	DDX_Radio(pDX, IDC_1stCovEigenvector, m_initializationOption);
	DDX_Text2(pDX, IDC_Distance1, m_criticalDistance1);
	// DDX_Text(pDX, IDC_NumberClusters, m_numberClusters);	// This added to work around Microsoft bug in previous call for doubles (8/18/2017)
	DDX_Text2(pDX, IDC_Distance2, m_criticalDistance2);
	DDX_Text(pDX, IDC_NumberClusters, m_numberClusters);
	DDV_MinMaxLong(pDX, m_numberClusters, 1, 254);
	DDX_Text(pDX, IDC_MinClusterSize, m_minClusterSize);
	DDV_MinMaxLong(pDX, m_minClusterSize, 1, 65535);
	DDX_Text2(pDX, IDC_Convergence, m_convergence);
	DDV_MinMaxDouble(pDX, m_convergence, 0., 100.);
	DDX_CBIndex(pDX, IDC_ClassCombo, m_classSelection);
	DDX_Text(pDX, IDC_LineEnd, m_LineEnd);
	DDV_MinMaxLong(pDX, m_LineEnd, 1, m_maxNumberLines);
	DDX_Text(pDX, IDC_LineInterval, m_LineInterval);
	DDV_MinMaxLong(pDX, m_LineInterval, 1, m_maxNumberLines);
	DDX_Text(pDX, IDC_LineStart, m_LineStart);                              
	DDV_MinMaxLong(pDX, m_LineStart, 1, m_maxNumberLines); 
	DDX_Text(pDX, IDC_ColumnEnd, m_ColumnEnd);
	DDV_MinMaxLong(pDX, m_ColumnEnd, 1, m_maxNumberColumns);
	DDX_Text(pDX, IDC_ColumnInterval, m_ColumnInterval);
	DDV_MinMaxLong(pDX, m_ColumnInterval, 1, m_maxNumberColumns);
	DDX_Text(pDX, IDC_ColumnStart, m_ColumnStart);
	DDV_MinMaxLong(pDX, m_ColumnStart, 1, m_maxNumberColumns);
	DDX_Check(pDX, IDC_ProjectClassMeans, m_projectClassMeansFlag);
	//}}AFX_DATA_MAP

			// Verify that the line and column values make sense
		
	VerifyLineColumnStartEndValues (pDX);
	
	ClustersFromSetting(pDX, IDC_ClusterTrainingAreas, m_clustersFrom);

	if (pDX->m_bSaveAndValidate)
		{
		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;
																
		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;

		if (!ISODATAClusterDialogCheckNumberPixels (1,
																	m_clustersFrom,
																	&m_dialogSelectArea))
			{
			pDX->PrepareEditCtrl(IDC_LineInterval);
			pDX->Fail();

			}		// end "if (!ISODATAClusterDialogCheckNumberPixels( ..."
			
		}		// end "if (pDX->m_bSaveAndValidate)"
	
}		// end "DoDataExchange"



BEGIN_MESSAGE_MAP(CMISODATAClusterDialog, CDialog)
	//{{AFX_MSG_MAP(CMISODATAClusterDialog)
	ON_CBN_SELENDOK(IDC_ClassCombo, OnSelendokClassCombo)
	ON_BN_CLICKED(IDC_ClusterTrainingAreas, OnClusterTrainingAreas)
	ON_BN_CLICKED(IDC_ClusterImageArea, OnClusterImageArea)
	ON_BN_CLICKED(IDC_1stCovEigenvector, On1stCovEigenvector)
	ON_BN_CLICKED(IDC_EigenvectorVolume, OnEigenvectorVolume)
	ON_BN_CLICKED(IDC_OnePassCluster, OnOnePassCluster)
	ON_BN_CLICKED(IDEntireImage, ToEntireImage)
	ON_BN_CLICKED(IDSelectedImage, ToSelectedImage)
	ON_BN_CLICKED(IDC_ProjectClassMeans, OnProjectClassMeans)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP() 


//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the cluster
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
//	Coded By:			Larry L. Biehl			Date: 08/02/1996
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

SInt16 
CMISODATAClusterDialog::DoDialog(void)

{  
	Boolean						continueFlag = FALSE;  
	
	INT_PTR						returnCode;
								

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE;
	
//		clustersFrom = kTrainingType;
//		if (m_clustersFrom == 1)	
//			clustersFrom = kAreaType;   
				
				// Cluster area													
				
//		m_dialogSelectArea.lineStart = m_LineStart;
//		m_dialogSelectArea.lineEnd = m_LineEnd;
//		m_dialogSelectArea.lineInterval = m_LineInterval;
																
//		m_dialogSelectArea.columnStart = m_ColumnStart;
//		m_dialogSelectArea.columnEnd = m_ColumnEnd;
//		m_dialogSelectArea.columnInterval = m_ColumnInterval;

		ISODATAClusterDialogOK ( (SInt16)(m_initializationOption+1),
											FALSE,
											m_projectClassMeansFlag,
											m_clustersFrom,
											(SInt16)m_classSelection,
											m_classListPtr,
											m_localNumberClasses,
											&m_dialogSelectArea,
											(SInt16)m_numberClusters,
											m_convergence,
											(SInt16)m_minClusterSize,
											m_criticalDistance1,
											m_criticalDistance2 );

	}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}		// end "DoDialog"  



void CMISODATAClusterDialog::ClustersFromSetting(
				CDataExchange* 	pDX, 
				int 					nIDC, 
				int& 					value)

{  
	BOOL				settingFlag;
	
	         
	if (pDX->m_bSaveAndValidate)
		{
				// Reading from radio group. Get index ignoring the disabled
				// items.  
					
		value = -1;
					 
		DDX_Check(m_dialogFromPtr, IDC_ClusterTrainingAreas, settingFlag);
		if (settingFlag)
			value = kTrainingType;
				
		DDX_Check(m_dialogFromPtr, IDC_ClusterImageArea, settingFlag);
		if (settingFlag)
			value = kAreaType; 
			
		}		// end "if (pDX->m_bSaveAndValidate)"
		
	else		// !pDX->m_bSaveAndValidate
		{                                         
		int			localValue = value;
		
		Boolean		savedTrainingAreaEnabledFlag;
		
		
		savedTrainingAreaEnabledFlag = 
								GetDlgItem(IDC_ClusterTrainingAreas)->IsWindowEnabled();
								
				// Setting specific radio group item. Take into account any
				// disabled items.  
		     		
		if( !savedTrainingAreaEnabledFlag )
			GetDlgItem(IDC_ClusterTrainingAreas)->EnableWindow(TRUE); 
		                                                                                    
		DDX_Radio(pDX, 
					IDC_ClusterTrainingAreas, 
					localValue);       
		     		
		if( !savedTrainingAreaEnabledFlag )
			GetDlgItem(IDC_ClusterTrainingAreas)->EnableWindow(FALSE); 
			
		}		// end "else !pDX->m_bSaveAndValidate"
	
}		// end "ClustersFromSetting"          


 
void CMISODATAClusterDialog::UpdateOptionSettings(void)

{  
	BOOL			settingFlag = FALSE;
	
	
	DDX_Radio(m_dialogFromPtr, IDC_1stCovEigenvector, m_initializationOption);
	 
	if (m_initializationOption != 2)
		{                                    
		MShowDialogItem (this, IDC_NumberClustersPrompt);
		MShowDialogItem (this, IDC_NumberClusters);
		
		MHideDialogItem (this, IDC_Distance1Prompt);
		MHideDialogItem (this, IDC_Distance1);     
		MHideDialogItem (this, IDC_Distance2Prompt);
		MHideDialogItem (this, IDC_Distance2);  
		                                              
		DDX_Check(m_dialogToPtr, IDC_ProjectClassMeans, m_projectClassMeansFlag);
		SetDLogControlHilite (this, IDC_ProjectClassMeans, 0); 
		
		}		// end "if (m_initializationOption != 2)" 
	
	else		// m_initializationOption == 2)
		{                                   
		MHideDialogItem (this, IDC_NumberClustersPrompt);
		MHideDialogItem (this, IDC_NumberClusters);
		
		MShowDialogItem (this, IDC_Distance1Prompt);
		MShowDialogItem (this, IDC_Distance1);     
		MShowDialogItem (this, IDC_Distance2Prompt);
		MShowDialogItem (this, IDC_Distance2); 
		                                              
		DDX_Check(m_dialogToPtr, IDC_ProjectClassMeans, settingFlag);
		SetDLogControlHilite (this, IDC_ProjectClassMeans, 255); 
		
		}		// end "else m_initializationOption == 2" 
	
}		// end "UpdateOptionSettings"      



/////////////////////////////////////////////////////////////////////////////
// CMISODATAClusterDialog message handlers

BOOL CMISODATAClusterDialog::OnInitDialog()

{                     
	SInt16							classSelection,
										clustersFrom,
										initializationOption,
										projectClassMeansCode;
	
	Boolean							savedUseCorrelationMatrixFlag;
	
	
	CMDialog::OnInitDialog();              
	
			// Make sure that we have the bitmaps for the entire image buttons.
		
	VERIFY(toEntireButton.AutoLoad(IDEntireImage, this));
	VERIFY(toSelectedButton.AutoLoad(IDSelectedImage, this));

	ISODATAClusterDialogInitialize ( this,
													m_classListPtr,
													&initializationOption,
													&savedUseCorrelationMatrixFlag,
													&projectClassMeansCode,
													&clustersFrom,
													&classSelection,
													&m_localNumberClasses,
													&m_dialogSelectArea,
													&m_numberClusters,
													&m_convergence,
													&m_minClusterSize,
													&m_criticalDistance1,
													&m_criticalDistance2,
													&m_distanceDecimalDigits);
													
			// Adjust those options that need to be.
			// 	Initialization option.
													
	m_initializationOption = initializationOption - 1;
		
			// 	User project class means flag.
	
	m_projectClassMeansFlag = FALSE;
	if (projectClassMeansCode == 1)		
		m_projectClassMeansFlag = TRUE;
		
			// 	Cluster areas from training areas or a selected area.
	
	m_clustersFrom = 0;
	if (clustersFrom == kAreaType)	
		m_clustersFrom = 1;
				
			// 	All class training areas or a subset.
		
	m_classSelection = classSelection;
				
			// 	Selected area to cluster.
                  
	m_LineStart = m_dialogSelectArea.lineStart;
	m_LineEnd = m_dialogSelectArea.lineEnd;
	m_LineInterval = m_dialogSelectArea.lineInterval;
	m_ColumnStart = m_dialogSelectArea.columnStart;
	m_ColumnEnd = m_dialogSelectArea.columnEnd;
	m_ColumnInterval = m_dialogSelectArea.columnInterval;

			// Limit critical distance measures to computed decimal place.
			// This may need to be change if we have data sets with very
			// small values. 

	LoadDItemRealValue (this, IDC_Distance1, m_criticalDistance1, m_distanceDecimalDigits);
	DDX_Text2(m_dialogFromPtr, IDC_Distance1, m_criticalDistance1);

	LoadDItemRealValue (this, IDC_Distance2, m_criticalDistance2, m_distanceDecimalDigits);
	DDX_Text2(m_dialogFromPtr, IDC_Distance2, m_criticalDistance2);

	if (UpdateData(FALSE) )                   
		PositionDialogWindow ();
	
			// Update the option boxes to be shown based on the 
			// intialization options.
				
//	UpdateOptionSettings ();
	
			// Set default text selection to first edit text item						
	
	UInt16  defaultTextIndex;		
	if (gClusterSpecsPtr->initializationOption != 4)
		defaultTextIndex = IDC_NumberClusters;
		
	else		// gClusterSpecsPtr->initializationOption == 4 
		defaultTextIndex = IDC_Convergence;
		                                       
	SelectDialogItemText (this, defaultTextIndex, 0, SInt16_MAX);       
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}          



void CMISODATAClusterDialog::OnSelendokClassCombo(void)

{                                                                                                              
	HandleClassesMenu( &m_localNumberClasses,
								(SInt16*)m_classListPtr,
								1,      
								(SInt16)gProjectInfoPtr->numberStatisticsClasses, 
								IDC_ClassCombo,
								&m_classSelection);
	
}		// end "OnSelendokClassCombo" 



void CMISODATAClusterDialog::OnClusterTrainingAreas()
{                                                                                
	ISODATAClusterDialogOnTrainingAreas (this);
/*	                                                            
	HideShowAreaItems(kHide);                          
		                                                     
	MShowDialogItem (this, IDC_StartEndInterval);
	MShowDialogItem (this, IDC_LinePrompt);
	MShowDialogItem (this, IDC_ColumnPrompt);  
	MShowDialogItem (this, IDC_LineInterval);  
	MShowDialogItem (this, IDC_ColumnInterval); 
	
	MShowDialogItem (this, IDC_ClassPrompt); 
	MShowDialogItem (this, IDC_ClassCombo);
*/	
}		// end "OnClusterTrainingAreas" 



void CMISODATAClusterDialog::OnClusterImageArea()
{                                                                                                                          
//	HideShowAreaItems(kShow);  
	                                                                               
	ISODATAClusterDialogOnImageArea (this);
	
	SetEntireImageButtons (
					NULL, 
					m_LineStart, 
					m_LineEnd, 
					m_ColumnStart, 
					m_ColumnEnd);  
	
//	MHideDialogItem (this, IDC_ClassPrompt); 
//	MHideDialogItem (this, IDC_ClassCombo);
	
}		// end "OnClusterImageArea" 



void CMISODATAClusterDialog::On1stCovEigenvector()
{
	UpdateOptionSettings ();
	
}		// end "On1stCovEigenvector"



void CMISODATAClusterDialog::OnEigenvectorVolume()
{                                                          
	UpdateOptionSettings ();
	
}		// end "OnEigenvectorVolume"



void CMISODATAClusterDialog::OnOnePassCluster()
{                                                          
	UpdateOptionSettings ();
	
}		// end "OnOnePassCluster"



void CMISODATAClusterDialog::OnProjectClassMeans()
{                                                           
		                                              
	DDX_Check(m_dialogFromPtr, IDC_ProjectClassMeans, m_projectClassMeansFlag);
	
}		// end "OnProjectClassMeans"
