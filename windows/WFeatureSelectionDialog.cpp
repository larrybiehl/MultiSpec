// WFeatureSelectionDialog.cpp : implementation file
//   
// Revised by Larry Biehl on 12/21/2017
                   
#include "SMultiSpec.h"
                     
#include "WMultiSpec.h"
#include "WFeatureSelectionDialog.h"
#include "WChannelsDialog.h"

//#include	"SExtGlob.h"  

extern void 			InitializeDialogFeatureParameters (
								Boolean								featureTransformationFlag,
								UInt16								localActiveNumberFeatures, 
								UInt16								numberFeatures, 
								UInt16								numberTransformFeatures,
								UInt16*								localFeaturesPtr,
								UInt16*								localTransformFeaturesPtr,
								UInt16*								localNumberFeaturesPtr,
								UInt16*								localNumberTransformFeaturesPtr,
								UInt16**								localActiveFeaturesPtrPtr);

extern void		 		SeparabilityDialogCheckFeatureTransformation(
								DialogPtr							dialogPtr,
								Boolean*								featureTransformationFlagPtr,
								UInt16*								localActiveNumberFeaturesPtr,
								UInt16**								localActiveFeaturePtrPtr,
								Boolean								featureTransformAllowedFlag,
								SInt16								featureTransformItem, 
								SInt16								channelFeatureItem, 
								SInt16*								channelSelectionPtr,
								SInt16								localNumberFeatures,
								UInt16*								localFeaturesPtr, 
								SInt16								localNumberTransformedFeatures,
								UInt16*								localTransformFeaturesPtr); 
								
extern Boolean 		SeparabilityDialogGetFeatureTransformAllowedFlag (
								SInt16								separabilityDistance,
								UInt16								numberEigenvectors);

extern void 			SeparabilityDialogInitialize (
								DialogPtr							dialogPtr,
								SeparabilitySpecsPtr				separabilitySpecsPtr,
								UInt16*								localFeaturesPtr,
								UInt16*								transformFeaturesPtr,
								UInt16*								localClassPtr,
								unsigned char*						localSymbolPtr,
								UInt16*								localChannelCombinationsPtr,
								SInt16**								localClassPairWeightsListPtr,
								SInt16*								separabilityDistancePtr,
								UInt16*								numberEigenvectorsPtr,
								Boolean*								featureTransformAllowedFlagPtr,
								Boolean*								featureTransformationFlagPtr,
								SInt16*								channelSelectionPtr,
								UInt16*								localNumberFeaturesPtr,
								SInt16*								channelCombinationSelectionPtr,
								UInt16*								numberChannelGroupCombinationsPtr, 
								UInt16*								savedNumberChannelGroupCombinationsPtr,
								SInt32*								savedContiguousChannelsPerGroupPtr,
								Boolean*								savedSearchFlagPtr,
								SInt32*								maxContiguousChannelsPerGroupPtr,
								SInt32*								localCombinationsToListPtr,
								SInt16*								classSelectionPtr,
								UInt32*								localNumberClassesPtr,
								SInt32*								numberClassCombinationsPtr,
								SInt16*								interClassWeightsSelectionPtr,
								SInt16*								defaultClassPairWeightPtr,
								SInt16*								symbolSelectionPtr,
								Boolean*								textWindowFlagPtr,
								Boolean*								diskFileFlagPtr);
								
extern void 			SeparabilityDialogOK (
								SeparabilitySpecsPtr				separabilitySpecsPtr,
								SInt16								separabilityDistance,
								Boolean								featureTransformationFlag,
								SInt16								channelSelection,
								UInt16								localNumberFeatures,
								UInt16*								localFeaturesPtr,
								SInt16								channelCombinationSelection,
								UInt16								localNumberChannelGroupCombinations,
								UInt16*								localChannelCombinationsPtr,
								SInt32								contiguousChannelsPerGroup,
								Boolean								searchFlag,
								SInt32								numberCombinationsToList,
								SInt16								classSelection,
								UInt32								localNumberClasses,
								UInt16*								localClassPtr,
								SInt16								interClassWeightsSelection,
								SInt16*								localClassPairWeightsListPtr,
								SInt16								localDefaultClassPairWeight,
								SInt16								symbolSelection,
								unsigned char*						localSymbolsPtr,
								Boolean								outputTextWindowFlag,
								Boolean								diskFileFlag);
								 
extern void 			SeparabilityDialogUpdateChannelCombinationItems (
								DialogPtr							dialogPtr,
								SeparabilitySpecsPtr				separabilitySpecsPtr,
								Boolean								updateChannelCombinationVectorFlag,
								UInt16								numberFeatures,
								UInt16*								allChanCombinationsPtr,
								SInt32								contiguousChannelsPerGroup,
								Boolean								searchFlag,
								SInt16								channelCombinationSelection,
								UInt16*								channelCombinationsPtr,
								UInt16*								numberChannelGroupCombinationsPtr,
								UInt16*								savedNumberChannelGroupCombinationsPtr,
								SInt32*								combinationsToListPtr,   
								SInt32*								maxContiguousChannelsPerGroupPtr); 
                                                                           
extern void 			SeparabilityDialogUpdateChannelFeatureGroupText (
								DialogPtr							dialogPtr, 
								SInt16								contiguousChannelsPerGroup,
								Boolean								featureTransformationFlag);
								
extern void 			SeparabilityDialogUpdateNumberClassCombintations(
								DialogPtr							dialogPtr,
								SInt16								classSelection,
								UInt32*								numberClassesPtr,
								UInt16*								classPtr);  

extern void				SeparabilityListDialog (
								SInt32*								localCombinationsToListPtr);

extern void 			SeparabilityListDialogInitialize ( 
								DialogPtr							dialogPtr,
								SeparabilitySpecsPtr				separabilitySpecsPtr,
								SInt32								combinationsToList,
								Boolean*								separabilityTableFlagPtr,
								SInt32*								localCombinationsToListPtr,
								double*								localMinDistanceRangeToListPtr,
								double*								localMaxDistanceRangeToListPtr,
								SInt16*								sortChannelCombinationsPtr,
								Boolean*								thresholdedClassPairTableFlagPtr,
								double*								localDistancesLessThanToListPtr,
								Boolean*								thresholdedGroupTableFlagPtr,
								double*								localDistancesLessThanToGroupPtr,
								Boolean*								listClassPairDistancesFlagPtr);
								
extern void 			SeparabilityListDialogOK ( 
								SeparabilitySpecsPtr				separabilitySpecsPtr,
								SInt32*								combinationsToListPtr,
								Boolean								separabilityTableFlag,
								SInt32								localCombinationsToList,
								double								localMinDistanceRangeToList,
								double								localMaxDistanceRangeToList,
								Boolean								sortByAverageDistanceFlag,
								Boolean								sortByMinimumDistanceFlag,
								Boolean								sortByOrderComputedFlag,
								Boolean								thresholdedClassPairTableFlag,
								double								localDistancesLessThanToList,
								Boolean								thresholdedGroupTableFlag,
								double								localDistancesLessThanToGroup,
								Boolean								listClassPairDistancesFlag);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMFeatureSelectionDialog dialog


CMFeatureSelectionDialog::CMFeatureSelectionDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMFeatureSelectionDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMFeatureSelectionDialog) 
	m_separabilityDistance = -1; 
	m_savedContiguousChannelsPerGroup = 1; 
	m_localCombinationsToList = 1;    
	m_textWindowFlag = FALSE; 
	m_diskFileFlag = FALSE;
	m_channelCombinationSelection = -1;          
	m_interClassWeightsSelection = -1;
	m_searchFlag = FALSE;
	//}}AFX_DATA_INIT 
	
	m_separabilitySpecsPtr = NULL; 
	m_localChannelCombinationsPtr = NULL;
	m_maxContiguousChannelsPerGroup = 1;
	m_localClassPairWeightsListPtr = NULL;
	m_localDefaultClassPairWeight = 0;
	
	m_initializedFlag = CMDialog::m_initializedFlag; 
	
	if (m_initializedFlag)
		m_initializedFlag = GetDialogLocalVectors (
														&m_localFeaturesPtr,
														&m_localTransformFeaturesPtr,     
														&m_classListPtr,
														NULL, 
														NULL,
														&m_localSymbolsPtr,
														&m_localChannelCombinationsPtr,
														&m_localClassPairWeightsListPtr);
}     



CMFeatureSelectionDialog::~CMFeatureSelectionDialog(void)
{  
	ReleaseDialogLocalVectors (m_localFeaturesPtr,  
											m_localTransformFeaturesPtr,       
											m_classListPtr,
											NULL,
											NULL,
											m_localSymbolsPtr,
											m_localChannelCombinationsPtr,
											m_localClassPairWeightsListPtr);
	
}		// end "~CMFeatureSelectionDialog"



void CMFeatureSelectionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMFeatureSelectionDialog) 
	DDX_CBIndex(pDX, IDC_DistanceMeasureCombo, m_separabilityListSelection);
	DDX_CBIndex(pDX, IDC_NumberChannelsCombo, m_channelCombinationSelection); 
	DDX_Text(pDX, IDC_ChannelsPerGroup, m_savedContiguousChannelsPerGroup);
	DDV_MinMaxLong(pDX, m_savedContiguousChannelsPerGroup, 1, m_maxContiguousChannelsPerGroup);
	DDX_Text(pDX, IDC_NumberBestToList, m_localCombinationsToList); 
	DDV_MinMaxLong(pDX, m_localCombinationsToList, 1, SInt16_MAX);
	DDX_CBIndex(pDX, IDC_ClassPairWeightsCombo, m_interClassWeightsSelection);
	DDX_Check(pDX, IDC_TextWindow, m_textWindowFlag);
	DDX_Check(pDX, IDC_DiskFile, m_diskFileFlag);
	DDX_Check(pDX, IDC_StepProcedure, m_searchFlag);
	DDX_Check(pDX, IDC_FeatureTransformation, m_featureTransformationFlag);
	DDX_CBIndex(pDX, IDC_ChannelCombo, m_channelSelection);
	DDX_CBIndex(pDX, IDC_ClassCombo, m_classSelection);
	DDX_CBIndex(pDX, IDC_SymbolCombo, m_symbolSelection);       
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMFeatureSelectionDialog, CMDialog)
	//{{AFX_MSG_MAP(CMFeatureSelectionDialog)
	ON_CBN_SELENDOK(IDC_ChannelCombo, OnSelendokChannelCombo)
	ON_CBN_SELENDOK(IDC_DistanceMeasureCombo, OnSelendokDistanceMeasureCombo)
	ON_CBN_SELENDOK(IDC_NumberChannelsCombo, OnSelendokNumberChannelsCombo)
	ON_EN_CHANGE(IDC_ChannelsPerGroup, OnChangeChannelsPerGroup)
	ON_BN_CLICKED(IDC_StepProcedure, OnStepProcedure)
	ON_EN_CHANGE(IDC_NumberBestToList, OnChangeNumberBestToList)
	ON_CBN_SELENDOK(IDC_ClassCombo, OnSelendokClassCombo)
	ON_BN_CLICKED(IDC_FeatureTransformation, OnFeatureTransformation)
	ON_BN_CLICKED(IDC_ListOptions, OnListOptions) 
	ON_CBN_SELENDOK(IDC_ClassPairWeightsCombo, OnSelendokClassPairWeightsCombo)
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
//	Coded By:			Larry L. Biehl			Date: 04/28/1998
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean 
CMFeatureSelectionDialog::DoDialog(
				SeparabilitySpecsPtr				separabilitySpecsPtr,
				UInt16*								allChanCombinationsPtr)

{                                                  
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE; 

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
	
	m_separabilitySpecsPtr = separabilitySpecsPtr;
	                               
	m_allChanCombinationsPtr = allChanCombinationsPtr;
	
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE;
								
		SeparabilityDialogOK (separabilitySpecsPtr,
										m_separabilityDistance,
										m_featureTransformationFlag,
										m_channelSelection,
										m_localActiveNumberFeatures,
										m_localActiveFeaturesPtr,
										m_channelCombinationSelection,
										m_localNumberChannelGroupCombinations,
										m_localChannelCombinationsPtr,
										m_savedContiguousChannelsPerGroup,
										m_searchFlag,
										m_localCombinationsToList,
										m_classSelection,
										m_localNumberClasses,
										m_classListPtr,
										m_interClassWeightsSelection+1,
										m_localClassPairWeightsListPtr,
										m_localDefaultClassPairWeight,
										m_symbolSelection,
										m_localSymbolsPtr,
										m_textWindowFlag,
										m_diskFileFlag ); 
					                                 
		}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}		// end "DoDialog" 



void 
CMFeatureSelectionDialog::HandleChannelCombinationsMenu(
				UInt16								channelCombinationsMenuItemNumber)

{  
	Boolean						returnFlag;
	
	                                                         
	DDX_CBIndex(m_dialogFromPtr, 
						channelCombinationsMenuItemNumber, 
						m_channelCombinationSelection);
	                                           
	if (m_channelCombinationSelection == kSubsetMenuItem)
		{
				// Subset of channels to be used.							
								                
		SetDLogControlHilite (NULL, IDOK, 255);  
		
		CMChannelCombinationsDlg*		channelCombinationsDialogPtr = NULL;
		
		TRY
			{                                
			channelCombinationsDialogPtr = new CMChannelCombinationsDlg();
			                                                                     
			returnFlag = channelCombinationsDialogPtr->DoDialog (
														m_channelCombinationSelection,
														&m_localNumberChannelGroupCombinations,
														m_localChannelCombinationsPtr,
														m_separabilitySpecsPtr->numberAllChanCombinations,
														m_allChanCombinationsPtr,
														m_savedContiguousChannelsPerGroup,
														m_featureTransformationFlag); 
		   
		   if (channelCombinationsDialogPtr != NULL)                           
				delete channelCombinationsDialogPtr;
			}
			
		CATCH_ALL(e)
			{
			MemoryMessage(0, kObjectMessage);
			}
		END_CATCH_ALL 
		                          				
		if (m_localNumberChannelGroupCombinations == 
										m_separabilitySpecsPtr->numberAllChanCombinations)
			m_channelCombinationSelection = kAllMenuItem; 
			
		if (m_channelCombinationSelection == kAllMenuItem)
			DDX_CBIndex(m_dialogToPtr, 
							channelCombinationsMenuItemNumber, 
							m_channelCombinationSelection);
		                  						                                
		SetDLogControlHilite (NULL, IDOK, 0);
						
		}		// end "if (m_channelCombinationsSelection == kSubsetMenuItem)"          
	
}		// end "HandleChannelCombinationsMenu"  


/////////////////////////////////////////////////////////////////////////////
// CMFeatureSelectionDialog message handlers

BOOL CMFeatureSelectionDialog::OnInitDialog()
{
	SInt16								channelCombinationSelection,
											channelSelection,
											classSelection,
											interClassWeightsSelection,
											symbolSelection;
	
	
	CDialog::OnInitDialog(); 
	
			// Initialize dialog variables.

	SeparabilityDialogInitialize ( this,
												m_separabilitySpecsPtr,
												m_localFeaturesPtr,
												m_localTransformFeaturesPtr,
												m_classListPtr,
												m_localSymbolsPtr,
												m_localChannelCombinationsPtr,
												&m_localClassPairWeightsListPtr,
												&m_separabilityDistance,
												&m_numberEigenvectors,
												(Boolean*)&m_featureTransformAllowedFlag,
												(Boolean*)&m_featureTransformationFlag,
												&channelSelection,
												&m_localActiveNumberFeatures,
												&channelCombinationSelection,
												&m_localNumberChannelGroupCombinations, 
												&m_savedNumberChannelGroupCombinations,
												&m_savedContiguousChannelsPerGroup,
												(Boolean*)&m_searchFlag,
												&m_maxContiguousChannelsPerGroup,
												&m_localCombinationsToList,
												&classSelection,
												&m_localNumberClasses,
												&m_numberClassCombinations,
												&interClassWeightsSelection,
												&m_localDefaultClassPairWeight,
												&symbolSelection,
												(Boolean*)&m_textWindowFlag,
												(Boolean*)&m_diskFileFlag); 
	
			// Set feature/transform feature parameters  

	InitializeDialogFeatureParameters (m_featureTransformationFlag,
													m_localActiveNumberFeatures, 
													gProjectInfoPtr->numberStatisticsChannels, 
													gTransformationMatrix.numberFeatures,
													m_localFeaturesPtr,
													m_localTransformFeaturesPtr,
													&m_localNumberFeatures,
													&m_localNumberTransformFeatures,
													&m_localActiveFeaturesPtr);
		
			// Get the feature selection algorithm list selection that matches the input
			// feature selection algorithm.
			                             
	m_separabilityListSelection = GetComboListSelection(IDC_DistanceMeasureCombo, 
																			m_separabilityDistance);
		
	if (m_separabilityListSelection == -1)
		m_separabilityListSelection = 0; 
	                                                                   			
			//	Set the channels/features list item				
			                                             
	m_availableFeaturePtr = gProjectInfoPtr->channelsPtr;
	m_channelSelection = channelSelection;
	
			// Number channel combinations.
			
	m_channelCombinationSelection = channelCombinationSelection;
	
			// Current maximum number of channel combinations 

	LoadDItemRealValue ( this, 
							IDC_MaxSearchCombinations, 
							(double)m_separabilitySpecsPtr->maxNumberFeatureCombinations,
							0);		               
	
			// Classes to use.         
	
	m_classSelection = classSelection;					
	::LoadDItemValue ( this, IDC_NumberClassCombinations, m_numberClassCombinations ); 
	
			// Interclass weights to use.             
			// Adjust for 0 base index.
			// Unequal interclass weights are not available yet.																	
			                                                                  
	m_interClassWeightsSelection = interClassWeightsSelection - 1; 
	HideDialogItem (this, IDC_WeightsEqual);                                            
//	((CComboBox*)GetDlgItem(IDC_ClassPairWeightsCombo))->DeleteString(1); 
	
			// Symbols to use.             
			// Adjust for 0 base index.
			// User defined symbols are not available yet.																	
			                                                                  
	m_symbolSelection = symbolSelection - 1; 
	HideDialogItem (this, IDC_SymbolCombo);                                            
	((CComboBox*)GetDlgItem(IDC_SymbolCombo))->DeleteString(1); 
	                 
	if (UpdateData(FALSE) )                   
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item	
		                                       
//	GetDlgItem(IDC_ChannelsPerGroup)->SetFocus();
//	SendDlgItemMessage( IDC_ChannelsPerGroup, EM_SETSEL, 0, MAKELPARAM(0, -1) );
	SelectDialogItemText (this, IDC_ChannelsPerGroup, 0, SInt16_MAX);  			  
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog" 



void 
CMFeatureSelectionDialog::OnSelendokDistanceMeasureCombo() 

{  
	CComboBox* 			comboBoxPtr;
	
	SInt16				channelSelection,
							separabilityDistance,
							savedSeparabilityListSelection;
	                  
	Boolean				returnFlag = TRUE;  
	
	
	savedSeparabilityListSelection = m_separabilityListSelection;
	                                    
	DDX_CBIndex(m_dialogFromPtr, 
					IDC_DistanceMeasureCombo, 
					m_separabilityListSelection); 
					
			// Get the actual classification procedure code.
			
	comboBoxPtr = (CComboBox*)GetDlgItem(IDC_DistanceMeasureCombo);
	separabilityDistance = 
					(SInt16)comboBoxPtr->GetItemData(m_separabilityListSelection);
		
	if (separabilityDistance != 0)
		{                                                                                                    
		m_separabilityDistance = separabilityDistance; 
					
		m_featureTransformAllowedFlag =
								SeparabilityDialogGetFeatureTransformAllowedFlag (
																		m_separabilityDistance,
																		m_numberEigenvectors);
																		
		SeparabilityDialogCheckFeatureTransformation( this,
																	(Boolean*)&m_featureTransformationFlag,
																	&m_localActiveNumberFeatures,
																	&m_localActiveFeaturesPtr,
																	m_featureTransformAllowedFlag,
																	IDC_FeatureTransformation, 
																	IDC_ChannelPrompt,
																	&channelSelection,
																	m_localNumberFeatures,
																	m_localFeaturesPtr, 
																	m_localNumberTransformFeatures,
																	m_localTransformFeaturesPtr); 
																	
		m_channelSelection = channelSelection;
																	
		DDX_CBIndex(m_dialogToPtr, IDC_ChannelCombo, m_channelSelection);
		
		if (m_featureTransformationFlag && !m_featureTransformAllowedFlag)
			{												            
			SeparabilityDialogUpdateChannelCombinationItems (
														this,
														m_separabilitySpecsPtr,
														TRUE,
														m_localActiveNumberFeatures,
														m_allChanCombinationsPtr,
														m_savedContiguousChannelsPerGroup,
														m_searchFlag,
														m_channelCombinationSelection,
														m_localChannelCombinationsPtr,
														&m_localNumberChannelGroupCombinations,
														&m_savedNumberChannelGroupCombinations,
														&m_localCombinationsToList, 
														&m_maxContiguousChannelsPerGroup);
														      
			SeparabilityDialogUpdateChannelFeatureGroupText (
														this, 
														(SInt16)m_savedContiguousChannelsPerGroup,
														m_featureTransformationFlag);
															
			}		// end "if (m_featureTransformFlag && !m_featureTransformAllowedFlag)"
						
		}		// end "if (separabilityDistance != 0)"
		
	else		// separabilityDistance == 0
		{                      
		m_separabilityListSelection = savedSeparabilityListSelection; 
		
		}		// end "else separabilityDistance == 0" 
	                                    
	DDX_CBIndex(m_dialogToPtr, 
					IDC_DistanceMeasureCombo, 
					m_separabilityListSelection); 
						 
	m_separabilityDistance = 
				(SInt16)comboBoxPtr->GetItemData(m_separabilityListSelection);
	
}		// end "OnSelendokDistanceMeasureCombo" 



void 
CMFeatureSelectionDialog::OnFeatureTransformation()

{  
	SInt16								channelSelection;
	
	      
	DDX_Check(m_dialogFromPtr, 
					IDC_FeatureTransformation, 
					m_featureTransformationFlag);
					                                                    
	SeparabilityDialogCheckFeatureTransformation( this,
																	(Boolean*)&m_featureTransformationFlag,
																	&m_localActiveNumberFeatures,
																	&m_localActiveFeaturesPtr,
																	m_featureTransformAllowedFlag,
																	IDC_FeatureTransformation, 
																	IDC_ChannelPrompt,
																	&channelSelection,
																	m_localNumberFeatures,
																	m_localFeaturesPtr, 
																	m_localNumberTransformFeatures,
																	m_localTransformFeaturesPtr);
																	
	m_channelSelection = channelSelection;
																	
	DDX_CBIndex(m_dialogToPtr, IDC_ChannelCombo, m_channelSelection); 
															            
	SeparabilityDialogUpdateChannelCombinationItems (
														this,
														m_separabilitySpecsPtr,
														TRUE,
														m_localActiveNumberFeatures,
														m_allChanCombinationsPtr,
														m_savedContiguousChannelsPerGroup,
														m_searchFlag,
														m_channelCombinationSelection,
														m_localChannelCombinationsPtr,
														&m_localNumberChannelGroupCombinations,
														&m_savedNumberChannelGroupCombinations,
														&m_localCombinationsToList, 
														&m_maxContiguousChannelsPerGroup);
														      
	SeparabilityDialogUpdateChannelFeatureGroupText (
														this, 
														(SInt16)m_savedContiguousChannelsPerGroup,
														m_featureTransformationFlag);
	
}		// end "OnFeatureTransformation"



void 
CMFeatureSelectionDialog::OnSelendokChannelCombo()

{                                                                                  
	HandleChannelsMenu(IDC_ChannelCombo, 
								m_featureTransformationFlag,
								(SInt16)gProjectInfoPtr->numberStatisticsChannels,
								2,
								TRUE);   
															            
	SeparabilityDialogUpdateChannelCombinationItems (
														this,
														m_separabilitySpecsPtr,
														TRUE,
														m_localActiveNumberFeatures,
														m_allChanCombinationsPtr,
														m_savedContiguousChannelsPerGroup,
														m_searchFlag,
														m_channelCombinationSelection,
														m_localChannelCombinationsPtr,
														&m_localNumberChannelGroupCombinations,
														&m_savedNumberChannelGroupCombinations,
														&m_localCombinationsToList,
														&m_maxContiguousChannelsPerGroup);
	
}		// end "OnSelendokChannelCombo"



void 
CMFeatureSelectionDialog::OnSelendokNumberChannelsCombo()

{                                             
	HandleChannelCombinationsMenu( IDC_NumberChannelsCombo);  
															            
	SeparabilityDialogUpdateChannelCombinationItems (
														this,
														m_separabilitySpecsPtr,
														FALSE,
														m_localActiveNumberFeatures,
														m_allChanCombinationsPtr,
														m_savedContiguousChannelsPerGroup,
														m_searchFlag,
														m_channelCombinationSelection,
														m_localChannelCombinationsPtr,
														&m_localNumberChannelGroupCombinations,
														&m_savedNumberChannelGroupCombinations,
														&m_localCombinationsToList, 
														&m_maxContiguousChannelsPerGroup);
	
}		// end "OnSelendokNumberChannelsCombo"



void 
CMFeatureSelectionDialog::OnChangeChannelsPerGroup()

{  
	SInt32					newContiguousChannelsPerGroup;
	
	
	DDX_Text(m_dialogFromPtr, 
					IDC_ChannelsPerGroup, 
					newContiguousChannelsPerGroup);
					
	if (newContiguousChannelsPerGroup < 1 || 
									newContiguousChannelsPerGroup > m_maxContiguousChannelsPerGroup)
		{					                  
		DDX_Text(m_dialogToPtr, 
					IDC_ChannelsPerGroup, 
					m_savedContiguousChannelsPerGroup);
					
		DDV_MinMaxLong(m_dialogFromPtr, 
							newContiguousChannelsPerGroup, 
							1, 
							m_maxContiguousChannelsPerGroup); 
							
		}
		
	else		// newContiguousChannelsPerGroup in proper range
		{ 
		if (m_savedContiguousChannelsPerGroup != newContiguousChannelsPerGroup)
			{
			m_savedContiguousChannelsPerGroup = newContiguousChannelsPerGroup;
															            
			SeparabilityDialogUpdateChannelCombinationItems (
														this,
														m_separabilitySpecsPtr,
														TRUE,
														m_localActiveNumberFeatures,
														m_allChanCombinationsPtr,
														m_savedContiguousChannelsPerGroup,
														m_searchFlag,
														m_channelCombinationSelection,
														m_localChannelCombinationsPtr,
														&m_localNumberChannelGroupCombinations,
														&m_savedNumberChannelGroupCombinations,
														&m_localCombinationsToList, 
														&m_maxContiguousChannelsPerGroup);
														      
			SeparabilityDialogUpdateChannelFeatureGroupText (
														this, 
														(SInt16)m_savedContiguousChannelsPerGroup,
														m_featureTransformationFlag);
														
			}		// end "if (m_savedContiguousChannelsPerGroup != ..."
														
		}		// end "else newContiguousChannelsPerGroup in proper range"                                                          
	
}
 		// end "OnChangeChannelsPerGroup"
 		
 		
void 
CMFeatureSelectionDialog::OnStepProcedure()

{  
   DDX_Check(m_dialogFromPtr, IDC_StepProcedure, m_searchFlag);                                                          
															            
	SeparabilityDialogUpdateChannelCombinationItems (
														this,
														m_separabilitySpecsPtr,
														TRUE,
														m_localActiveNumberFeatures,
														m_allChanCombinationsPtr,
														m_savedContiguousChannelsPerGroup,
														m_searchFlag,
														m_channelCombinationSelection,
														m_localChannelCombinationsPtr,
														&m_localNumberChannelGroupCombinations,
														&m_savedNumberChannelGroupCombinations,
														&m_localCombinationsToList,
														&m_maxContiguousChannelsPerGroup);
	
}		// end "OnStepProcedure"



void 
CMFeatureSelectionDialog::OnChangeNumberBestToList()

{                                                           
	SInt32					newCombinationsToList;
	
	
	DDX_Text(m_dialogFromPtr, 
					IDC_NumberBestToList, 
					newCombinationsToList);
					
	if (newCombinationsToList < 1 || newCombinationsToList > SInt16_MAX)
		{					                  
		DDX_Text(m_dialogToPtr, 
					IDC_NumberBestToList, 
					m_localCombinationsToList);
					
		DDV_MinMaxLong(m_dialogFromPtr, 
							newCombinationsToList, 
							1, 
							SInt16_MAX); 
							
		}		// end "if (newCombinationsToList < 1 || ..."
		
	else		// newCombinationsToList in proper range 
		m_localCombinationsToList = newCombinationsToList;
	
}		// end "OnChangeNumberBestToList"



void 
CMFeatureSelectionDialog::OnSelendokClassCombo() 

{                                                                                
	HandleClassesMenu( &m_localNumberClasses,
								(SInt16*)m_classListPtr,
								2,      
								(SInt16)gProjectInfoPtr->numberStatisticsClasses, 
								IDC_ClassCombo,
								&m_classSelection); 
								
	SeparabilityDialogUpdateNumberClassCombintations(
								this,
								m_classSelection,
								&m_localNumberClasses,
								m_classListPtr);
	
}		// end "OnSelendokClassCombo"

 

void CMFeatureSelectionDialog::OnOK()
{  
	SInt16								index;
	
	                                    
	if (m_separabilitySpecsPtr->maxNumberFeatureCombinations <= 0)
		{
		index = IDS_FeatureSelection11;
				
		if (m_featureTransformationFlag)
			index = IDS_FeatureSelection12;
				
		if (m_savedContiguousChannelsPerGroup > 1)
			index = IDS_FeatureSelection13;
					                                    
		SetDLogControlHilite (NULL, IDOK, 255); 
				
		DisplayAlert (
						kErrorAlertID, 1, kFeatureSelectionStrID, index, 0, NULL);
				                                          
		SetDLogControlHilite (NULL, IDOK, 0); 
				
		}		// end "if (m_separabilitySpecsPtr->maxNumberFeatureCombinations <= 0)" 
	
	else		// everything is 'okay'
		CMDialog::OnOK();
	
}		// end "OnOK" 



void 
CMFeatureSelectionDialog::OnListOptions() 

{  
	 
	SetDLogControlHilite (NULL, IDOK, 255);  
		
	SeparabilityListDialog (&m_localCombinationsToList); 
	
	SetDLogControlHilite (NULL, IDOK, 0);
	  
	DDX_Text(m_dialogToPtr, 
				IDC_NumberBestToList, 
				m_localCombinationsToList); 
	
}		// end "OnListOptions"

  

void CMFeatureSelectionDialog::OnSelendokClassPairWeightsCombo()
{                                                           
	HandleClassPairWeightsMenu (
						&m_localClassPairWeightsListPtr,
						IDC_ClassPairWeightsCombo,
						&m_interClassWeightsSelection,
						&m_localDefaultClassPairWeight);
	
}		// end "OnSelendokClassPairWeightsCombo"




/////////////////////////////////////////////////////////////////////////////
// CMFeatureSelectionListDialog dialog


CMFeatureSelectionListDialog::CMFeatureSelectionListDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMFeatureSelectionListDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMFeatureSelectionListDialog)
	m_sortChannelCombinations = -1;
//new
	m_sortChannelCombinations = 0;
//end new
	m_thresholdedGroupTableFlag = FALSE;
	m_thresholdedClassPairTableFlag = FALSE;
	m_localDistancesLessThanToGroup = 0;
	m_localDistancesLessThanToList = 0;
	m_listClassPairDistancesFlag = FALSE;
	m_localMinDistanceRangeToList = 0;
	m_localMaxDistanceRangeToList = 0;
	m_localCombinationsToList = 1;
	m_separabilityTableFlag = FALSE;
	//}}AFX_DATA_INIT

	m_initializedFlag = CMDialog::m_initializedFlag;
}

void CMFeatureSelectionListDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMFeatureSelectionListDialog)
	DDX_Radio(pDX, IDC_AverageClassDistance, m_sortChannelCombinations);
	DDX_Check(pDX, IDC_ClassGroupThresholdTable, m_thresholdedGroupTableFlag);
	DDX_Check(pDX, IDC_ClassPairThresholdTable, m_thresholdedClassPairTableFlag);
	DDX_Text2(pDX, IDC_ClassGroupThreshold, m_localDistancesLessThanToGroup);
	DDV_MinMaxDouble(pDX, m_localDistancesLessThanToGroup, 0, 32767);
	DDX_Text2(pDX, IDC_ClassPairThreshold, m_localDistancesLessThanToList);
	DDV_MinMaxDouble(pDX, m_localDistancesLessThanToList, 0, 32767);
	DDX_Check(pDX, IDC_ListClassPairDistances, m_listClassPairDistancesFlag);
	DDX_Text2(pDX, IDC_MinDistanceRange, m_localMinDistanceRangeToList);
	DDV_MinMaxDouble(pDX, m_localMinDistanceRangeToList, 0, 32767);
	DDX_Text2(pDX, IDC_MaxDistanceRange, m_localMaxDistanceRangeToList);
	DDV_MinMaxDouble(pDX, m_localMaxDistanceRangeToList, 0, 32767);
	DDX_Text(pDX, IDC_NumberBestCombinations, m_localCombinationsToList);
	DDV_MinMaxLong(pDX, m_localCombinationsToList, 1, 32767);
	DDX_Check(pDX, IDC_SeparabilityTable, m_separabilityTableFlag);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMFeatureSelectionListDialog, CMDialog)
	//{{AFX_MSG_MAP(CMFeatureSelectionListDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP() 


//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the feature
//							selection list dialog box to the user and copy the
//							revised back to the feature selection specification structure 
//							if the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			 
//
//	Coded By:			Larry L. Biehl			Date: 05/06/1998
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean 
CMFeatureSelectionListDialog::DoDialog(
				SeparabilitySpecsPtr				separabilitySpecsPtr,
				SInt32*								combinationsToListPtr)

{                                                  
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE; 

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																			return(FALSE);
	
	m_separabilitySpecsPtr = separabilitySpecsPtr;
	m_combinationsToListPtr = combinationsToListPtr;
	
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE;  
		
			// Adjust sort channel combinations variable to be 1 based.  
			
		m_sortChannelCombinations++;
				
		SeparabilityListDialogOK (m_separabilitySpecsPtr,
											m_combinationsToListPtr,
											m_separabilityTableFlag,
											m_localCombinationsToList,
											(float)m_localMinDistanceRangeToList,
											(float)m_localMaxDistanceRangeToList,
											(m_sortChannelCombinations == 1),
											(m_sortChannelCombinations == 2),
											(m_sortChannelCombinations == 0),
											m_thresholdedClassPairTableFlag,
											m_localDistancesLessThanToList,
											m_thresholdedGroupTableFlag,
											m_localDistancesLessThanToGroup,
											m_listClassPairDistancesFlag );
					                                 
		}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}		// end "DoDialog" 


/////////////////////////////////////////////////////////////////////////////
// CMFeatureSelectionListDialog message handlers

BOOL CMFeatureSelectionListDialog::OnInitDialog()
{
	SInt16									sortChannelCombinations;
	
	
	CDialog::OnInitDialog();
	
			// Initialize dialog variables.

	SeparabilityListDialogInitialize (this,
													m_separabilitySpecsPtr,
													*m_combinationsToListPtr,
													(Boolean*)&m_separabilityTableFlag,
													&m_localCombinationsToList,
													&m_localMinDistanceRangeToList,
													&m_localMaxDistanceRangeToList,
													&sortChannelCombinations,
													(Boolean*)&m_thresholdedClassPairTableFlag,
													&m_localDistancesLessThanToList,
													(Boolean*)&m_thresholdedGroupTableFlag,
													&m_localDistancesLessThanToGroup,
													(Boolean*)&m_listClassPairDistancesFlag );
		
			// Adjust sort channel combinations variable to be 0 based.
			
	m_sortChannelCombinations = sortChannelCombinations - 1;					
	                 
	if (UpdateData(FALSE) )                   
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item	
		                                       
//	GetDlgItem(IDC_NumberBestCombinations)->SetFocus();
//	SendDlgItemMessage( IDC_NumberBestCombinations, EM_SETSEL, 0, MAKELPARAM(0, -1) );
	SelectDialogItemText (this, IDC_NumberBestCombinations, 0, SInt16_MAX);  			   
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"
