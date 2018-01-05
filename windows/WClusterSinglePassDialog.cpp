// WClusterSinglePassDialog.cpp : implementation file
//  
// Revised by Larry Biehl on 12/21/2017
//
    
#include "SMultiSpec.h" 
#include "WClusterSinglePassDialog.h" 
//#include	"SExtGlob.h"  

extern void 			OnePassClusterDialogInitialize (
								DialogPtr							dialogPtr,
								UInt16*								localClassAreaPtr,
								SInt32*								minClusterSizePtr,
								double*								criticalDistance1Ptr,
								double*								criticalDistance2Ptr,
								UInt16*								distanceDecimalDigitsPtr,
								SInt16*								clustersFromPtr,
								SInt16*								classAreaSelectionPtr,
								UInt32*								numberClassAreasPtr,
								DialogSelectArea*					dialogSelectAreaPtr );

extern void 			OnePassClusterDialogOK (
								SInt16								minClusterSize,
								double								criticalDistance1,
								double								criticalDistance2,
								SInt16								clustersFrom,
								SInt16								classAreaSelection,
								UInt16*								localClassAreaPtr,
								UInt32								localNumberClassAreas,
								DialogSelectArea*					dialogSelectAreaPtr);

extern void 			OnePassClusterDialogOnImageArea (
								DialogPtr							dialogPtr);

extern void 			OnePassClusterDialogOnTrainingAreas (
								DialogPtr							dialogPtr);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMSinglePassClusterDialog dialog


CMSinglePassClusterDialog::CMSinglePassClusterDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMSinglePassClusterDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMSinglePassClusterDialog)
	m_criticalDistance1 = 1;
	m_criticalDistance2 = 1;
	m_minClusterSize = 1;
	m_clustersFrom = -1;
	m_classSelection = -1;
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
	
}		// end "CMSinglePassClusterDialog"  



CMSinglePassClusterDialog::~CMSinglePassClusterDialog(void)
{        
	m_classListPtr = CheckAndDisposePtr (m_classListPtr);                                         
	
}		// end "~CMSinglePassClusterDialog"



void CMSinglePassClusterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMSinglePassClusterDialog)
	DDX_Text2(pDX, IDC_CriticalDistance1, m_criticalDistance1);
	DDX_Text2(pDX, IDC_CriticalDistance2, m_criticalDistance2);
	DDX_Text(pDX, IDC_MinClusterSize, m_minClusterSize);
	DDV_MinMaxLong(pDX, m_minClusterSize, 1, 65535);
//	DDX_Radio(pDX, IDC_ClusterTrainingAreas, m_clustersFrom);
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
	//}}AFX_DATA_MAP 

			// Verify that the line and column values make sense
		
	VerifyLineColumnStartEndValues (pDX);
	
	ClustersFromSetting(pDX, IDC_ClusterTrainingAreas, m_clustersFrom);
}

BEGIN_MESSAGE_MAP(CMSinglePassClusterDialog, CMDialog)
	//{{AFX_MSG_MAP(CMSinglePassClusterDialog)                     
	ON_CBN_SELENDOK(IDC_ClassCombo, OnSelendokClassCombo)
	ON_BN_CLICKED(IDC_ClusterTrainingAreas, OnClusterTrainingAreas)
	ON_BN_CLICKED(IDC_ClusterImageArea, OnClusterImageArea)
	ON_BN_CLICKED(IDEntireImage, ToEntireImage)
	ON_BN_CLICKED(IDSelectedImage, ToSelectedImage)
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
CMSinglePassClusterDialog::DoDialog(void)

{  
	Boolean						continueFlag = FALSE; 
	
	SInt16						clustersFrom;
	INT_PTR						returnCode;
								

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE; 
	
		clustersFrom = kTrainingType;
		if (m_clustersFrom == 1)	
			clustersFrom = kAreaType;   
				
				// Cluster area													
				
		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;
																
		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;

		OnePassClusterDialogOK ( (SInt16)m_minClusterSize,
											m_criticalDistance1,
											m_criticalDistance2,
											clustersFrom,
											(SInt16)m_classSelection,
											m_classListPtr,
											m_localNumberClasses,
											&m_dialogSelectArea );
	
		}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}		// end "DoDialog"


/////////////////////////////////////////////////////////////////////////////
// CMSinglePassClusterDialog message handlers

BOOL CMSinglePassClusterDialog::OnInitDialog(void)

{                         
	SInt16							classSelection,
										clustersFrom;
	
	
	CMDialog::OnInitDialog();              
	
			// Make sure that we have the bitmaps for the entire image buttons.
		
	VERIFY(toEntireButton.AutoLoad(IDEntireImage, this));
	VERIFY(toSelectedButton.AutoLoad(IDSelectedImage, this)); 
	
	OnePassClusterDialogInitialize ( this,
													m_classListPtr,
													&m_minClusterSize,
													&m_criticalDistance1,
													&m_criticalDistance2,
													&m_distanceDecimalDigits,
													&clustersFrom,
													&classSelection,
													&m_localNumberClasses,
													&m_dialogSelectArea );
													
			// Adjust those options that need to be.
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

	LoadDItemRealValue (this, IDC_CriticalDistance1, m_criticalDistance1, m_distanceDecimalDigits);
	DDX_Text2(m_dialogFromPtr, IDC_CriticalDistance1, m_criticalDistance1);

	LoadDItemRealValue (this, IDC_CriticalDistance2, m_criticalDistance2, m_distanceDecimalDigits);
	DDX_Text2(m_dialogFromPtr, IDC_CriticalDistance2, m_criticalDistance2);
		                   
	if (UpdateData(FALSE) )                   
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item	
		                                       
	SelectDialogItemText (this, IDC_MinClusterSize, 0, SInt16_MAX);         
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog" 



void CMSinglePassClusterDialog::OnSelendokClassCombo()
{                                                                                               
	HandleClassesMenu( &m_localNumberClasses,
								(SInt16*)m_classListPtr,
								1,      
								(SInt16)gProjectInfoPtr->numberStatisticsClasses, 
								IDC_ClassCombo,
								&m_classSelection);
	
}		// end "OnSelendokClassCombo"   



void CMSinglePassClusterDialog::OnClusterTrainingAreas()
{                                                               
	OnePassClusterDialogOnTrainingAreas (this);
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



void CMSinglePassClusterDialog::OnClusterImageArea()
{                                                                                           
//	HideShowAreaItems(kShow); 
	                  
	OnePassClusterDialogOnImageArea (this);
	
	SetEntireImageButtons (
					NULL, 
					m_LineStart, 
					m_LineEnd, 
					m_ColumnStart, 
					m_ColumnEnd);  
	
//	MHideDialogItem (this, IDC_ClassPrompt); 
//	MHideDialogItem (this, IDC_ClassCombo);
	
} 		// end "OnClusterTrainingAreas"  



void CMSinglePassClusterDialog::ClustersFromSetting(
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
			value = 0;
				
		DDX_Check(m_dialogFromPtr, IDC_ClusterImageArea, settingFlag);
		if (settingFlag)
			value = 1; 
			
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
