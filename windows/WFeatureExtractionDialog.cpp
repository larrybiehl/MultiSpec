// WFeatureExtractionDialog.cpp : implementation file
//  
// Revised by Larry Biehl on 12/21/2017
//
                   
#include "SMultiSpec.h"

#include "WChannelsDialog.h" 
#include "WFeatureExtractionDialog.h"
#include "WProjectionPursuitDialog.h"

//#include	"SExtGlob.h"  

extern void 		FeatureExtractionDialogAlgorithm (
							DialogPtr							dialogPtr, 
							SInt16								algorithmCode);

extern void 		FeatureExtractionDialogInitialize (
							DialogPtr							dialogPtr,
							FeatureExtractionSpecsPtr		featureExtractionSpecsPtr,
							UInt16*								localFeaturesPtr,
							UInt16*								localClassPtr,
							SInt16**								localClassPairWeightsListPtr,
							SInt16*								algorithmCodePtr,
							SInt16*								weightsSelectionPtr,
							SInt16*								interClassWeightsSelectionPtr,
							SInt16*								defaultClassPairWeightPtr,
							Boolean*								specialOptionsFlagPtr,
							double*								outlierThreshold1Ptr,
							double*								outlierThreshold2Ptr,
							UInt32*								minimumThresholdNumberPtr,
							Boolean*								optimizeClassFlagPtr,
							double*								optimizeThresholdPtr,
							UInt32*								maxPixelsPerClassPtr,
							SInt16*								channelSelectionPtr,
							UInt16*								localNumberChannelsPtr,
							SInt16*								channelListTypePtr,
							SInt16*								classSelectionPtr,
							UInt32*								localNumberClassesPtr,
							SInt16*								preprocessSelectionPtr,
							Boolean*								listTransformationFlagPtr );
	
extern void 		FeatureExtractionDialogOK (
							FeatureExtractionSpecsPtr		featureExtractionSpecsPtr,
							SInt16								algorithmCode,
							SInt16								weightsSelection,
							float*								localClassWeightsPtr,
							SInt16								interClassWeightsSelection,
							SInt16*								localClassPairWeightsListPtr,
							SInt16								localDefaultClassPairWeight,
							Boolean								specialOptionsFlag,
							double								outlierThreshold1,
							double								outlierThreshold2,
							UInt32								minThresholdNumber,
							double								optimizeThreshold,
							Boolean								optimizeClassFlag,
							UInt32								maxPixelsPerClass,
							SInt16								channelSelection,
							UInt16								localNumberFeatures,
							UInt16*								localFeaturesPtr,
							SInt16								classSelection,
							UInt32								localNumberClasses,
							UInt16*								localClassPtr,
							Boolean								listTransformationFlag,
							SInt16								preprocessSelection );
	
extern void 		FeatureExtractionDialogOptimizeClass (
							DialogPtr							dialogPtr,
							Boolean								optimizeClassFlag);
	
extern void 		FeatureExtractionDialogUpdateSpecialOptions (
							DialogPtr							dialogPtr,
							SInt16								algorithmCode,
							Boolean								specialOptionsFlag,
							Boolean								optimizeClassFlag );

extern Boolean 	ProjectionPursuitDialog (void);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMFeatureExtractionDialog dialog


CMFeatureExtractionDialog::CMFeatureExtractionDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMFeatureExtractionDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMFeatureExtractionDialog)
	m_interclassThrehold = (float)0.;
	m_withinClassThreshold = (float)0.;
	m_optimizeClassesFlag = FALSE;
	m_minThresholdNumber = 0;
	m_maxPixelsPerClass = 0;
	m_specialOptionsFlag = FALSE;
	m_listTransformationFlag = FALSE;
	m_interClassWeightsSelection = -1;
	m_preprocessSelection = -1;
	m_algorithmCode = -1;
	m_optimizeThreshold = (float)0.;
	//}}AFX_DATA_INIT
	
	m_localClassPairWeightsListPtr = NULL;
	m_localDefaultClassPairWeight = 0;
	
	m_initializedFlag = CMDialog::m_initializedFlag; 
	
	if (m_initializedFlag)
		m_initializedFlag = GetDialogLocalVectors (
														&m_localActiveFeaturesPtr,
														NULL,     
														&m_classListPtr,
														NULL, 
														&m_classWeightsPtr,
														NULL,
														NULL,
														&m_localClassPairWeightsListPtr);
														
}		// end "CMFeatureExtractionDialog"    



CMFeatureExtractionDialog::~CMFeatureExtractionDialog(void)

{  
	ReleaseDialogLocalVectors (m_localActiveFeaturesPtr,  
											NULL,       
											m_classListPtr,
											NULL,
											m_classWeightsPtr,
											NULL,
											NULL,
											m_localClassPairWeightsListPtr);
	
}		// end "CMFeatureExtractionDialog"



void CMFeatureExtractionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMFeatureExtractionDialog)
	DDX_Text2(pDX, IDC_InterclassThreshold, m_interclassThrehold);
	//DDX_Text(pDX, IDC_MinThresholdNumber, m_minThresholdNumber);	// This added to work around Microsoft bug (8/18/2017)
	DDX_Text2(pDX, IDC_WithinClassThreshold, m_withinClassThreshold);
	DDX_Check(pDX, IDC_OptimizeClasses, m_optimizeClassesFlag);
	DDX_Text(pDX, IDC_MinThresholdNumber, m_minThresholdNumber);
	DDX_Text(pDX, IDC_MaxPixels, m_maxPixelsPerClass);
	DDV_MinMaxLong(pDX, m_maxPixelsPerClass, 0, 32000);
	DDX_Check(pDX, IDC_SpecialOptions, m_specialOptionsFlag);
	DDX_Check(pDX, IDC_ListTransformationMatrix, m_listTransformationFlag);
	DDX_CBIndex(pDX, IDC_ClassPairWeightsCombo, m_interClassWeightsSelection);
	DDX_CBIndex(pDX, IDC_PreprocessingCombo, m_preprocessSelection);
	DDX_CBIndex(pDX, IDC_FEAlgorithmCombo, m_algorithmCode);
	DDX_CBIndex(pDX, IDC_ChannelCombo, m_channelSelection);
	DDX_CBIndex(pDX, IDC_ClassCombo, m_classSelection);      
	DDX_CBIndex(pDX, IDC_WeightsCombo, m_weightsSelection);
	DDX_Text2(pDX, IDC_PercentAccuracy, m_optimizeThreshold);
	DDV_MinMaxDouble(pDX, m_optimizeThreshold, 0., 100.);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMFeatureExtractionDialog, CMDialog)
	//{{AFX_MSG_MAP(CMFeatureExtractionDialog)           
	ON_BN_CLICKED(IDC_OptimizeClasses, OnOptimizeClasses)
	ON_BN_CLICKED(IDC_SpecialOptions, OnSpecialOptions)
	ON_CBN_SELENDOK(IDC_ChannelCombo, OnSelendokChannelCombo)
	ON_CBN_SELENDOK(IDC_ClassCombo, OnSelendokClassCombo)      
	ON_CBN_SELENDOK(IDC_WeightsCombo, OnSelendokClassWeightsCombo)
	ON_CBN_SELENDOK(IDC_ClassPairWeightsCombo, OnSelendokClassPairWeightsCombo)
	ON_CBN_SELENDOK(IDC_FEAlgorithmCombo, OnDecisionBoundary)
	//}}AFX_MSG_MAP
	ON_CBN_SELENDOK(IDC_PreprocessingCombo, OnCbnSelendokPreprocessingcombo)
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
//	Coded By:			Larry L. Biehl			Date: 03/02/1999
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean 
CMFeatureExtractionDialog::DoDialog(
				FeatureExtractionSpecsPtr		featureExtractionSpecsPtr)

{                                                  
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE; 

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
	
	m_featureExtractionSpecsPtr = featureExtractionSpecsPtr;  
	
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE;
		
		FeatureExtractionDialogOK (featureExtractionSpecsPtr,
												m_algorithmCode+1,
												m_weightsSelection+1,
												m_classWeightsPtr,
												m_interClassWeightsSelection+1,
												m_localClassPairWeightsListPtr,
												m_localDefaultClassPairWeight,
												m_specialOptionsFlag,
												(double)m_withinClassThreshold,
												(double)m_interclassThrehold,
												m_minThresholdNumber,
												(double)m_optimizeThreshold,
												m_optimizeClassesFlag,
												m_maxPixelsPerClass,
												m_channelSelection,
												m_localActiveNumberFeatures,
												m_localActiveFeaturesPtr,
												m_classSelection,
												m_localNumberClasses,
												m_classListPtr,
												m_listTransformationFlag,
												m_preprocessSelection+1 );
														
		}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}		// end "DoDialog" 


/////////////////////////////////////////////////////////////////////////////
// CMFeatureExtractionDialog message handlers

BOOL CMFeatureExtractionDialog::OnInitDialog()
{  
	double								interclassThrehold,
											optimizeThreshold,
											withinClassThreshold;

	ProjectionPursuitSpecsPtr		projectionPursuitSpecsPtr;
														
	SInt16								algorithmCode,
											channelSelection,
											classSelection,
											interClassWeightsSelection,
											preprocessSelection,
											weightsSelection;
	
	
	CDialog::OnInitDialog();
	
			// Initialize dialog variables.

	FeatureExtractionDialogInitialize (this,
														m_featureExtractionSpecsPtr,
														m_localActiveFeaturesPtr,
														m_classListPtr,
														&m_localClassPairWeightsListPtr,
														&algorithmCode,
														&weightsSelection,
														&interClassWeightsSelection,
														&m_localDefaultClassPairWeight,
														(Boolean*)&m_specialOptionsFlag, 
														&withinClassThreshold,
														&interclassThrehold,
														(UInt32*)&m_minThresholdNumber,
														(Boolean*)&m_optimizeClassesFlag,
														&optimizeThreshold,
														(UInt32*)&m_maxPixelsPerClass,
														&channelSelection,
														&m_localNumberFeatures,
														&m_channelListType,
														&classSelection,
														&m_localNumberClasses,
														&preprocessSelection,
														(Boolean*)&m_listTransformationFlag ); 
	                                                                   			
			//	Set the channels/features list item				
			                                             
	m_availableFeaturePtr = gProjectInfoPtr->channelsPtr;
	m_channelSelection = channelSelection;
	m_localActiveNumberFeatures = m_localNumberFeatures;
	
			// Algorithm.
			// Adjust for 0 base index.
			
	m_algorithmCode = algorithmCode - 1;  
                      
	FeatureExtractionDialogAlgorithm (this, m_algorithmCode+1);							
	HideDialogItem (this, IDC_PreprocessingOnly);
	
			// Class selection.
			
	m_classSelection = classSelection;
	
			// Preprocessing selection.  
			// Adjust for 0 base index.
	
	m_preprocessSelection = preprocessSelection - 1;                            													
//	HideDialogItem (this, IDC_PreprocessingCombo);	
	projectionPursuitSpecsPtr = 
							&m_featureExtractionSpecsPtr->projectionPursuitParams;
	if (projectionPursuitSpecsPtr->finalNumberFeatures <= 0)                           
		((CComboBox*)GetDlgItem(IDC_PreprocessingCombo))->DeleteString(2);
		
	
			// Class weights.
			// Adjust for 0 base index.
	
	m_weightsSelection = weightsSelection - 1; 
//	HideDialogItem (this, IDC_WeightsCombo);   
	HideDialogItem (this, IDC_WeightsEqual);   
	
			// Inter class weights.
			// Adjust for 0 base index.
	
	m_interClassWeightsSelection = interClassWeightsSelection - 1;  
//	HideDialogItem (this, IDC_ClassPairWeightsCombo);
	HideDialogItem (this, IDC_InterClassWeightsEqual);
	
	
	
	m_interclassThrehold = (float)interclassThrehold;
	m_optimizeThreshold = (float)optimizeThreshold;
	m_withinClassThreshold = (float)withinClassThreshold;
	                 
	if (UpdateData(FALSE) )                   
		PositionDialogWindow ();
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"



void CMFeatureExtractionDialog::OnOptimizeClasses()
{         
	DDX_Check(m_dialogFromPtr, 
					IDC_OptimizeClasses, 
					m_optimizeClassesFlag);  
					
	FeatureExtractionDialogOptimizeClass (this, m_optimizeClassesFlag);                                                     
	
}		// end "OnOptimizeClasses" 



void CMFeatureExtractionDialog::OnSpecialOptions() 

{                                       
	DDX_Check(m_dialogFromPtr, 
					IDC_SpecialOptions, 
					m_specialOptionsFlag);                                                       
	
	FeatureExtractionDialogUpdateSpecialOptions (
														this,
														m_algorithmCode+1,
														m_specialOptionsFlag,
														m_optimizeClassesFlag);
	
}		// end "OnSpecialOptions"

void CMFeatureExtractionDialog::OnDecisionBoundary()
{                                                 
	DDX_CBIndex(m_dialogFromPtr, 
					IDC_FEAlgorithmCombo, 
					m_algorithmCode); 
					                     
	FeatureExtractionDialogAlgorithm (this, m_algorithmCode+1);                                                        
	
	FeatureExtractionDialogUpdateSpecialOptions (this,
																m_algorithmCode+1,
																m_specialOptionsFlag,
																m_optimizeClassesFlag);
														 
}		// end "OnDecisionBoundary"


void CMFeatureExtractionDialog::OnSelendokChannelCombo()
{                                                                                                       
	HandleChannelsMenu(IDC_ChannelCombo, 
								kNoTransformation,
								(SInt16)gProjectInfoPtr->numberStatisticsChannels,
								m_channelListType,
								TRUE);   
	
}		// end "OnSelendokChannelCombo"

  

void CMFeatureExtractionDialog::OnSelendokClassWeightsCombo()
{                                                           
	HandleClassWeightsMenu ((SInt16)m_localNumberClasses, 
									(SInt16*)m_classListPtr,
									m_classWeightsPtr,
									gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
									IDC_WeightsCombo,
									&m_weightsSelection);
	
}		// end "OnSelendokClassWeightsCombo"

  

void CMFeatureExtractionDialog::OnSelendokClassPairWeightsCombo()
{                                                           
	HandleClassPairWeightsMenu (
						&m_localClassPairWeightsListPtr,
						IDC_ClassPairWeightsCombo,
						&m_interClassWeightsSelection,
						&m_localDefaultClassPairWeight);
	
}		// end "OnSelendokClassPairWeightsCombo"

void CMFeatureExtractionDialog::OnCbnSelendokPreprocessingcombo()
{   
	Boolean					okFlag;


	DDX_CBIndex(m_dialogFromPtr, 
					IDC_PreprocessingCombo, 
					m_preprocessSelection);

	if (m_preprocessSelection == 1)
		okFlag = ProjectionPursuitDialog();

	if (okFlag)
	{

	}		// end "if (okFlag)"

}		// end "OnCbnSelendokPreprocessingcombo"
