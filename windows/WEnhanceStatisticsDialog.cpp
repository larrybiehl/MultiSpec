// WEnhanceStatisticsDialog.cpp : implementation file
//
// Revised by Larry Biehl on 12/21/2017
//
                    
#include "SMultiSpec.h"
#include "WEnhanceStatisticsDialog.h"
//#include	"SExtGlob.h"

		// Enhance Statistics constants												
	
#define	kUseNoThreshold					1
#define	kUseChiChiThreshold				2
#define	kUsePercentThreshold				3
#define	kUseFarthestPixelThreshold		4


	
extern void 		StatisticsEnhanceDialogInitialize (
							DialogPtr							dialogPtr,
							StatEnhanceSpecsPtr				statEnhanceSpecsPtr,
							DialogSelectAreaPtr				dialogSelectAreaPtr,
							UInt16*								localClassPtr,
							SInt16*								classSelectionPtr,
							UInt32*								localNumberClassesPtr,
							Boolean*								useEnhancedStatisticsFlagPtr,
							SInt16*								classWeightSetPtr,
							SInt16*								weightsSelectionPtr,
							SInt16*								softThresholdSelectionPtr,
							double*								softChiChiThresholdPtr,
							double*								softPercentThresholdPtr,
							SInt16*								hardThresholdSelectionPtr,
							double*								hardChiChiThresholdPtr,
							double*								hardPercentThresholdPtr,
							Boolean*								weightLabeledFlagPtr,
							double*								labelWeightPtr,
							SInt32*								iterationMaxPtr,
							SInt32*								iterationStopLengthPtr,
							double*								logLikeStopPercentPtr);
	
extern void 		StatisticsEnhanceDialogOK (
							StatEnhanceSpecsPtr				statEnhanceSpecsPtr,
							DialogSelectAreaPtr				dialogSelectAreaPtr,
							UInt16*								localClassPtr,
							SInt16								classSelection,
							UInt32								localNumberClasses,
							Boolean								useEnhancedStatisticsFlag,
							SInt16								weightsSelection,
							float*								classWeightsPtr,
							SInt16								softThresholdSelection,
							double								softChiChiThreshold,
							double								softPercentThreshold,
							SInt16								hardThresholdSelection,
							double								hardChiChiThreshold,
							double								hardPercentThreshold,
							Boolean								weightLabeledFlag,
							double								labelWeight,
							SInt32								iterationMax,
							SInt32								iterationStopLength,
							double								logLikeStopPercent);
	
extern void			StatisticsEnhanceDialogUpdateChiSquaredValue (
							DialogPtr							dialogPtr,
							SInt16								thresholdCode,
							SInt16								chiChiThresholdItemNumber,
							double								thresholdPercent,
							double*								chiChiThresholdPtr);
	
extern void 		StatisticsEnhanceDialogUpdateThresholdItems (
							DialogPtr							dialogPtr,
							SInt16								thresholdCode,
							SInt16								chiChiThresholdItemNumber,
							SInt16								percentThresholdPromptItemNumber,
							SInt16								percentThresholdItemNumber,
							double								thresholdPercent,
							double*								chiChiThresholdPtr);
		
extern void 		UpdateNumberLabeledSamples (
							DialogPtr							dialogPtr,
							SInt16								classSelection,
							SInt16*								classPtr,
							UInt32								numberClasses);
									
extern void			UpdateNumberUnlabeledSamples (
							DialogSelectAreaPtr				dialogSelectAreaPtr,
							DialogPtr							dialogPtr);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMEnhanceStatisticsDialog dialog


CMEnhanceStatisticsDialog::CMEnhanceStatisticsDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMEnhanceStatisticsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMEnhanceStatisticsDialog)
	m_logLikeStopPercent = 1;
	m_iterationStopLength = 1;
	m_iterationMax = 1;
	m_labelWeight = 1;
	m_useEnhancedStatisticsFlag = FALSE;
	m_weightLabeledFlag = FALSE;
	m_weightsSelection = 0;
	m_softChiChiThreshold = 0.0;
	m_hardChiChiThreshold = 0.0;
	m_softPercentThreshold = 0.0;
	m_hardPercentThreshold = 0.0;
	m_softThresholdCode = -1;
	m_hardThresholdCode = -1;
	//}}AFX_DATA_INIT
	
	m_classWeightSet = 0;

	m_updatingThresholdItemsFlag = FALSE;
	
	m_initializedFlag = CMDialog::m_initializedFlag; 
	
	if (m_initializedFlag)
		m_initializedFlag = GetDialogLocalVectors (
														NULL,
														NULL,     
														&m_classListPtr,
														NULL, 
														&m_classWeightsPtr,
														NULL,
														NULL,
														NULL);
	
}		// end "CMEnhanceStatisticsDialog"



CMEnhanceStatisticsDialog::~CMEnhanceStatisticsDialog(void)
{  
	ReleaseDialogLocalVectors (NULL,
											NULL,        
											m_classListPtr,
											NULL,
											m_classWeightsPtr,
											NULL,
											NULL,
											NULL);
	
}		// end "~CMEnhanceStatisticsDialog"


void CMEnhanceStatisticsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMEnhanceStatisticsDialog)
	DDX_Text2(pDX, IDC_LogLikeChange, m_logLikeStopPercent);
	DDV_MinMaxDouble(pDX, m_logLikeStopPercent, 1.e-006, 10.);
	DDX_Text(pDX, IDC_IterationLength, m_iterationStopLength);
	DDV_MinMaxLong(pDX, m_iterationStopLength, 1, 200);
	DDX_Text(pDX, IDC_MaximumNumber, m_iterationMax);
	DDV_MinMaxLong(pDX, m_iterationMax, 1, 200);
	DDX_Text2(pDX, IDC_Weighting, m_labelWeight);
	DDV_MinMaxDouble(pDX, m_labelWeight, 1.e-005, 1000.);
	DDX_Check(pDX, IDC_UseEnhancedStats, m_useEnhancedStatisticsFlag);
	DDX_Check(pDX, IDC_WeightLabeledSamples, m_weightLabeledFlag);
	DDX_CBIndex(pDX, IDC_WeightCombo, m_weightsSelection);
	DDX_Text2(pDX, IDC_SoftChiChiThreshold, m_softChiChiThreshold);
	DDX_Text2(pDX, IDC_HardChiChiThreshold, m_hardChiChiThreshold);
	DDX_Text2(pDX, IDC_SoftPercentThreshold, m_softPercentThreshold);
	DDV_MinMaxDouble(pDX, m_softPercentThreshold, 0., 100.);
	DDX_Text2(pDX, IDC_HardPercentThreshold, m_hardPercentThreshold);
	DDV_MinMaxDouble(pDX, m_hardPercentThreshold, 0., 100.);
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
	DDX_CBIndex(pDX, IDC_SoftThresholdCombo, m_softThresholdCode);
	DDX_CBIndex(pDX, IDC_HardThresholdCombo, m_hardThresholdCode);
	//}}AFX_DATA_MAP
	
	if (pDX->m_bSaveAndValidate)
		{
				// Verify that the line and column values make sense
		
		VerifyLineColumnStartEndValues (pDX);
/*
		if (m_LineStart > m_LineEnd)
			{
			DDX_Text(pDX, IDC_LineStart, m_LineStart);              
			DDV_MinMaxLong(pDX, m_LineStart, 1, m_LineEnd);
			
			}		// end "m_LineStart > m_LineEnd"
				
		if (m_ColumnStart > m_ColumnEnd)
			{ 
			DDX_Text(pDX, IDC_ColumnStart, m_ColumnStart);
			DDV_MinMaxLong(pDX, m_ColumnStart, 1, m_ColumnEnd);
			
			}		// end "if (m_ColumnStart > m_ColumnEnd)" 
*/			
		}		// end "if (pDX->m_bSaveAndValidate)"

}		// end "DoDataExchange"


BEGIN_MESSAGE_MAP(CMEnhanceStatisticsDialog, CMDialog)
	//{{AFX_MSG_MAP(CMEnhanceStatisticsDialog)
	ON_BN_CLICKED(IDC_WeightLabeledSamples, OnWeightLabeledSamples)
	ON_BN_CLICKED(IDC_UseEnhancedStats, OnUseEnhancedStats)
	ON_CBN_SELCHANGE(IDC_HardThresholdCombo, OnSelchangeHardThresholdCombo)
	ON_CBN_SELCHANGE(IDC_SoftThresholdCombo, OnSelchangeSoftThresholdCombo)
	ON_EN_CHANGE(IDC_HardChiChiThreshold, OnChangeHardChiChiThreshold)
	ON_CBN_SELENDOK(IDC_ClassCombo, OnSelendokClassCombo)
	ON_EN_CHANGE(IDC_ColumnEnd, CheckColumnEnd)
	ON_EN_CHANGE(IDC_ColumnStart, CheckColumnStart)
	ON_EN_CHANGE(IDC_LineEnd, CheckLineEnd)
	ON_EN_CHANGE(IDC_LineStart, CheckLineStart)
	ON_BN_CLICKED(IDEntireImage, ToEntireImage)
	ON_BN_CLICKED(IDSelectedImage, ToSelectedImage)
	ON_EN_CHANGE(IDC_LineInterval, CheckLineInterval)
	ON_EN_CHANGE(IDC_ColumnInterval, CheckColumnInterval)
	ON_EN_CHANGE(IDC_HardPercentThreshold, OnChangeHardPercentThreshold)
	ON_EN_CHANGE(IDC_SoftPercentThreshold, OnChangeSoftPercentThreshold)
	ON_EN_CHANGE(IDC_SoftChiChiThreshold, OnChangeSoftChiChiThreshold)
	ON_CBN_SELENDOK(IDC_WeightCombo, OnSelendokClassWeightsCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()  


//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the statistics
//							enhancement dialog box to the user and copy the
//							revised back to the statistics enhancement specification 
//							structure if the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/04/1999
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

SInt16 
CMEnhanceStatisticsDialog::DoDialog(
				StatEnhanceSpecsPtr				statEnhanceSpecsPtr)

{  
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE;
								

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
	
	m_statEnhanceSpecsPtr = statEnhanceSpecsPtr;
	
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE;  
				
				// Classification area													
				
		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;
																
		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;
				
		StatisticsEnhanceDialogOK (m_statEnhanceSpecsPtr,
												&m_dialogSelectArea,
												m_classListPtr,
												m_classSelection,
												(SInt16)m_localNumberClasses,
												m_useEnhancedStatisticsFlag,
												m_weightsSelection+1,
												m_classWeightsPtr,
												m_softThresholdCode+1,
												m_softChiChiThreshold,
												m_softPercentThreshold,
												m_hardThresholdCode+1,
												m_hardChiChiThreshold,
												m_hardPercentThreshold,
												m_weightLabeledFlag,
												m_labelWeight,
												m_iterationMax,
												m_iterationStopLength,
												m_logLikeStopPercent);
					                                 
		}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}		// end "DoDialog" 

/////////////////////////////////////////////////////////////////////////////
// CMEnhanceStatisticsDialog message handlers

BOOL 
CMEnhanceStatisticsDialog::OnInitDialog()
 
{
	
	SInt16									classSelection,
												hardThresholdSelection,
												softThresholdSelection,
												weightsSelection;
												
												
	CDialog::OnInitDialog();
	
			// Make sure that we have the bitmaps for the entire image buttons.
		
	VERIFY(toEntireButton.AutoLoad(IDEntireImage, this));
	VERIFY(toSelectedButton.AutoLoad(IDSelectedImage, this));  
	
			// Initialize dialog variables.

	m_updatingThresholdItemsFlag = TRUE;
	
	StatisticsEnhanceDialogInitialize (this,
													m_statEnhanceSpecsPtr,
													&m_dialogSelectArea,
													m_classListPtr,
													&classSelection,
													&m_localNumberClasses,
													(Boolean*)&m_useEnhancedStatisticsFlag,
													&m_classWeightSet,
													&weightsSelection,
													&softThresholdSelection,
													&m_softChiChiThreshold,
													&m_softPercentThreshold,
													&hardThresholdSelection,
													&m_hardChiChiThreshold,
													&m_hardPercentThreshold,
													(Boolean*)&m_weightLabeledFlag,
													&m_labelWeight,
													&m_iterationMax,
													&m_iterationStopLength,
													&m_logLikeStopPercent);

	m_updatingThresholdItemsFlag = FALSE;
				
			// Selected area to use for non-labeled samples.
                  
	m_LineStart = m_dialogSelectArea.lineStart;
	m_LineEnd = m_dialogSelectArea.lineEnd;
	m_LineInterval = m_dialogSelectArea.lineInterval;
	m_ColumnStart = m_dialogSelectArea.columnStart;
	m_ColumnEnd = m_dialogSelectArea.columnEnd;
	m_ColumnInterval = m_dialogSelectArea.columnInterval; 	
						
			// Classes to use.
			
	m_classSelection = classSelection;
	
			// Class weights. Allow for 0 base, not 1 base as for Mac version				 
	
	m_weightsSelection = weightsSelection - 1;

			// Threshold codes.

	m_softThresholdCode = softThresholdSelection - 1;
	m_hardThresholdCode = hardThresholdSelection - 1;
	                 
	if (UpdateData(FALSE) )                   
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item	
		                                                     
	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);     
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	              
}		// end "OnInitDialog"



void CMEnhanceStatisticsDialog::OnWeightLabeledSamples() 
{
	DDX_Check(m_dialogFromPtr, 
					IDC_WeightLabeledSamples, 
					m_weightLabeledFlag);

	ShowHideDialogItem (this, IDC_WeightingPrompt, m_weightLabeledFlag);
	ShowHideDialogItem (this, IDC_Weighting, m_weightLabeledFlag);
	
}		// end "OnWeightLabeledSamples"



void CMEnhanceStatisticsDialog::OnUseEnhancedStats() 

{
	DDX_Check(m_dialogFromPtr, 
					IDC_UseEnhancedStats, 
					m_useEnhancedStatisticsFlag);
										
	m_weightsSelection = UpdateDialogClassWeightsInfo (
												(SInt16)m_weightsSelection,
												m_useEnhancedStatisticsFlag,
												&m_classWeightSet,
												FALSE);

	DDX_CBIndex(m_dialogToPtr, IDC_WeightCombo, m_weightsSelection);

//	GetDialogItem (dialogPtr, 18,  &theType, &theHandle, &theBox);
//	InvalRect (&theBox);
	
}		// end "OnUseEnhancedStats"  
 
  

void 
CMEnhanceStatisticsDialog::CheckColumnEnd(void)

{  
	if (!m_settingSelectedEntireButton)
		{
		CMDialog::CheckColumnEnd();
		
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
		
		}		// end "if (!m_settingSelectedEntireButton)" 
	
}		// end "CheckColumnEnd" 
  

void 
CMEnhanceStatisticsDialog::CheckColumnStart()

{        
	if (!m_settingSelectedEntireButton)
		{
		CMDialog::CheckColumnStart();
		
		m_dialogSelectArea.columnStart = m_ColumnStart;
		UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
		
		}		// end "if (!m_settingSelectedEntireButton)" 
	
}		// end "CheckColumnStart"
 
  

void CMEnhanceStatisticsDialog::CheckLineEnd()
{  
	if (!m_settingSelectedEntireButton)
		{
		CMDialog::CheckLineEnd(); 
		
		m_dialogSelectArea.lineEnd = m_LineEnd;
		UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
		
		}		// end "if (!m_settingSelectedEntireButton)" 
	
}		// end "CheckLineEnd" 
  

void CMEnhanceStatisticsDialog::CheckLineStart()
{  
	if (!m_settingSelectedEntireButton)
		{
		CMDialog::CheckLineStart();
		
		m_dialogSelectArea.lineStart = m_LineStart;
		UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
		
		}		// end "if (!m_settingSelectedEntireButton)" 
	
}		// end "CheckLineStart" 



void 
CMEnhanceStatisticsDialog::ToEntireImage(void)

{                                   
	CMDialog::ToEntireImage(); 

	m_dialogSelectArea.lineStart = m_LineStart;
	m_dialogSelectArea.lineEnd = m_LineEnd;
	m_dialogSelectArea.columnStart = m_ColumnStart;
	m_dialogSelectArea.columnEnd = m_ColumnEnd;
	UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
	
}		// end "ToEntireImage"



void 
CMEnhanceStatisticsDialog::ToSelectedImage(void)

{                               
	CMDialog::ToSelectedImage(); 

	m_dialogSelectArea.lineStart = m_LineStart;
	m_dialogSelectArea.lineEnd = m_LineEnd;
	m_dialogSelectArea.columnStart = m_ColumnStart;
	m_dialogSelectArea.columnEnd = m_ColumnEnd;
	UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
	
}		// end "ToSelectedImage" 



void CMEnhanceStatisticsDialog::CheckLineInterval() 
{
	CMDialog::CheckLineInterval();
		
	m_dialogSelectArea.lineInterval = m_LineInterval;
	UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
	
}		// end "CheckLineInterval"



void CMEnhanceStatisticsDialog::CheckColumnInterval() 
{
	CMDialog::CheckColumnInterval();
		
	m_dialogSelectArea.columnInterval = m_ColumnInterval;
	UpdateNumberUnlabeledSamples (&m_dialogSelectArea, this);
	
}		// end "CheckColumnInterval"



void CMEnhanceStatisticsDialog::OnSelendokClassCombo()
{                                                                                
	CMDialog::OnSelendokClassCombo();

	UpdateNumberLabeledSamples (this,
											m_classSelection,
											(SInt16*)m_classListPtr,
											m_localNumberClasses);
	
}		// end "OnSelendokClassCombo"



void 
CMEnhanceStatisticsDialog::OnSelchangeHardThresholdCombo()
 
{
	DDX_CBIndex(m_dialogFromPtr, 
						IDC_HardThresholdCombo, 
						m_hardThresholdCode);

	m_updatingThresholdItemsFlag = TRUE;

	StatisticsEnhanceDialogUpdateThresholdItems (
													this,
													(SInt16)(m_hardThresholdCode+1),
													IDC_HardChiChiThreshold,
													IDC_HardPercentThresholdPrompt,
													IDC_HardPercentThreshold,
													m_hardPercentThreshold,
													&m_hardChiChiThreshold);
	
	DDX_Text2(m_dialogToPtr, 
					IDC_HardChiChiThreshold, 
					m_hardChiChiThreshold);

	m_updatingThresholdItemsFlag = FALSE;
	
}		// end "OnSelchangeHardThresholdCombo"



void 
CMEnhanceStatisticsDialog::OnSelchangeSoftThresholdCombo()
 
{
	DDX_CBIndex(m_dialogFromPtr, 
						IDC_SoftThresholdCombo, 
						m_softThresholdCode);

	m_updatingThresholdItemsFlag = TRUE;

	StatisticsEnhanceDialogUpdateThresholdItems (
													this,
													(SInt16)(m_softThresholdCode+1),
													IDC_SoftChiChiThreshold,
													IDC_SoftPercentThresholdPrompt,
													IDC_SoftPercentThreshold,
													m_softPercentThreshold,
													&m_softChiChiThreshold);
	
	DDX_Text2(m_dialogToPtr, 
					IDC_SoftChiChiThreshold, 
					m_softChiChiThreshold);

	m_updatingThresholdItemsFlag = FALSE;

}		// end "OnSelchangeSoftThresholdCombo"



void 
CMEnhanceStatisticsDialog::OnChangeHardPercentThreshold()
 
{
	if (!m_updatingThresholdItemsFlag)
		{
		m_updatingThresholdItemsFlag = TRUE;

		DDX_Text2(m_dialogFromPtr, 
						IDC_HardPercentThreshold, 
						m_hardPercentThreshold);
		
		StatisticsEnhanceDialogUpdateChiSquaredValue (
												this,
												(SInt16)(m_hardThresholdCode+1),
												IDC_HardChiChiThreshold,
												m_hardPercentThreshold,
												&m_hardChiChiThreshold);

		m_updatingThresholdItemsFlag = FALSE;

		}		// end "if (!m_updatingThresholdItemsFlag)"
	
}		// end "OnChangeHardPercentThreshold"



void 
CMEnhanceStatisticsDialog::OnChangeSoftPercentThreshold()
 
{
	if (!m_updatingThresholdItemsFlag)
		{
		m_updatingThresholdItemsFlag = TRUE;

		DDX_Text2(m_dialogFromPtr, 
						IDC_SoftPercentThreshold, 
						m_softPercentThreshold);
		
		StatisticsEnhanceDialogUpdateChiSquaredValue (
												this,
												m_softThresholdCode+1,
												IDC_SoftChiChiThreshold,
												m_softPercentThreshold,
												&m_softChiChiThreshold); 

		m_updatingThresholdItemsFlag = FALSE;

		}		// end "if (!m_updatingThresholdItemsFlag)"
	
}		// end "OnChangeSoftPercentThreshold"



void 
CMEnhanceStatisticsDialog::OnChangeHardChiChiThreshold()
 
{			
	if (!m_updatingThresholdItemsFlag)
		{
		m_updatingThresholdItemsFlag = TRUE;

		if (m_hardThresholdCode+1 != kUseChiChiThreshold)
			{
			m_hardThresholdCode = kUseChiChiThreshold - 1;
								
			StatisticsEnhanceDialogUpdateThresholdItems (
										this,
										m_hardThresholdCode + 1,
										IDC_HardChiChiThreshold,
										IDC_HardPercentThresholdPrompt,
										IDC_HardPercentThreshold,
										m_hardPercentThreshold,
										&m_hardChiChiThreshold);

			DDX_Check(m_dialogToPtr, 
							IDC_HardThresholdCombo, 
							m_hardThresholdCode);
			
			}		// end "if (m_usePercentThresholdFlag && ..." 

		m_updatingThresholdItemsFlag = FALSE;

		}		// end "if (!m_updatingThresholdItemsFlag)"
	
}		// end "OnChangeHardChiChiThreshold"



void 
CMEnhanceStatisticsDialog::OnChangeSoftChiChiThreshold()
 
{
	if (!m_updatingThresholdItemsFlag)
		{
		m_updatingThresholdItemsFlag = TRUE;

		if (m_softThresholdCode+1 != kUseChiChiThreshold)
			{
			m_softThresholdCode = kUseChiChiThreshold - 1;
								
			StatisticsEnhanceDialogUpdateThresholdItems (
										this,
										m_softThresholdCode + 1,
										IDC_SoftChiChiThreshold,
										IDC_SoftPercentThresholdPrompt,
										IDC_SoftPercentThreshold,
										m_softPercentThreshold,
										&m_softChiChiThreshold);

			DDX_Check(m_dialogToPtr, 
							IDC_SoftThresholdCombo, 
							m_softThresholdCode);
			
			}		// end "if (m_usePercentThresholdFlag && ..."

		m_updatingThresholdItemsFlag = FALSE;

		}		// end "if (!m_updatingThresholdItemsFlag)"
	
}		// end "OnChangeSoftChiChiThreshold"

  

void CMEnhanceStatisticsDialog::OnSelendokClassWeightsCombo()
{                                                           
	HandleClassWeightsMenu ((SInt16)m_localNumberClasses, 
									(SInt16*)m_classListPtr,
									m_classWeightsPtr,
									gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
									IDC_WeightCombo,
									&m_weightsSelection);
	
}		// end "OnSelendokClassWeightsCombo"
