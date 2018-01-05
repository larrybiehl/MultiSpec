// WStatisticsDialog.cpp : implementation file
//      
// Revised by Larry Biehl on 01/05/2018
//
                   
#include "SMultiSpec.h"
 
#include "WStatisticsDialog.h"
#include	"WChannelsDialog.h"       

//#include	"SExtGlob.h" 		

extern void 		StatisticsDialogOK (
							SInt16								channelSelection,
							SInt16						      totalNumberChannels,
							UInt16*								featurePtr,
							UInt32								localNumberChannels,
							Boolean								showTrainingFieldsFlag,
							Boolean								showTestFieldsFlag,
							Boolean								showClassNamesFlag,
							Boolean								showFieldNamesFlag,
							Boolean								showTrainTestTextFlag,
							SInt16								localStatCode,
							SInt16								outlineColorSelection,
							Boolean								keepClassStatsFlag,
							double								variance,
							double								minLogDeterminant,
							Boolean								useCommonCovarianceInLOOCFlag );

extern void 		StatisticsDialogInitialize (
							DialogPtr								dialogPtr,
							SInt16									totalNumberChannels,
							SInt16*									localStatCodePtr,
							Boolean*									keepClassStatsFlagPtr,
							Boolean*									useCommonCovarianceInLOOCFlagPtr,
							double*									variancePtr,
							double*									minLogDeterminantPtr,
							SInt16*									channelSelectionPtr,
							SInt16*									channelListTypePtr,
							UInt16*									featurePtr,
							UInt16*									localNumberChannelsPtr,
							SInt16*									localOutlineFieldTypePtr,
							SInt16*									localLabelFieldCodePtr,
							SInt16*									outlineColorSelectionPtr,
							SInt16*									maskTrainImageSelectionPtr,
							SInt16*									maskTestImageSelectionPtr,
							UInt16*									maxNumberTrainLayersPtr,
							UInt16*									maxNumberTestLayersPtr);
							
extern SInt16 		StatisticsDialogSelectMaskItem (
							Handle*								maskFileInfoHandlePtr,
							DialogPtr							dialogPtr,
							MenuHandle							popUpSelectMaskImageMenu,
							SInt16								itemHit,
							SInt16								maskImageSelection,
							SInt16								selectStringNumber,
							SInt16								maskPopupItemNumber,
							SInt16								layerItemNumber,
							UInt16*								maxNumberLayersPtr);  

extern SInt16 		StatisticsDialogMaskCheck (
							Handle								trainMaskFileInfoHandle,
							Handle								testMaskFileInfoHandle,
							SInt16								maskTrainImageSelection,
							SInt16								maskTestImageSelection,
							UInt16								trainLayerNumber,
							UInt16								testLayerNumber);

extern Boolean	 	StatisticsOptionsDialog (
							SInt16*								statCodePtr, 
							Boolean*								keepClassStatsFlagPtr, 
							double*								variancePtr,
							double*								minLogDeterminantPtr, 
							Boolean*								useCommonCovarianceInLOOCFlagPtr);


/////////////////////////////////////////////////////////////////////////////
// CMStatisticsDialog dialog


CMStatisticsDialog::CMStatisticsDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMStatisticsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMStatisticsDialog)
	m_showFieldNames = FALSE;
	m_showTestFields = FALSE;
	m_showTrainingFields = FALSE;
	m_projectCommands = 0;
	m_showClassNames = FALSE;
	m_showTrainTestText = FALSE;
	m_outlineColorSelection = -1;
	m_testMaskCombo = -1;
	m_trainMaskCombo = -1;
	//}}AFX_DATA_INIT
	
	m_totalNumberChannels = 0; 
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
}



void CMStatisticsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMStatisticsDialog)                      
	DDX_Check(pDX, IDC_ShowFieldNames, m_showFieldNames);
	DDX_Check(pDX, IDC_TestFields, m_showTestFields);
	DDX_Check(pDX, IDC_TrainingFields, m_showTrainingFields);
	DDX_CBIndex(pDX, IDC_ProjectChanges, m_projectCommands);
	DDX_Check(pDX, IDC_ShowClassNames, m_showClassNames);
	DDX_Check(pDX, IDC_ShowTrainTestLabel, m_showTrainTestText);
	DDX_CBIndex(pDX, IDC_ColorCombo, m_outlineColorSelection);
	DDX_CBIndex(pDX, IDC_Channels, m_channelSelection);
	DDX_CBIndex(pDX, IDC_TestMaskPopUp, m_testMaskCombo);
	DDX_CBIndex(pDX, IDC_TrainMaskPopUp, m_trainMaskCombo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMStatisticsDialog, CMDialog)
	//{{AFX_MSG_MAP(CMStatisticsDialog)
	ON_CBN_SELENDOK(IDC_Channels, OnSelendokChannels)
	ON_CBN_SELENDOK(IDC_ProjectChanges, OnSelendokProjectChanges)
	ON_CBN_DROPDOWN(IDC_ProjectChanges, OnDropdownProjectChanges)
	ON_CBN_DROPDOWN(IDC_Channels, OnDropdownChannels)
	ON_BN_CLICKED(IDC_StatisticsOptions, OnStatisticsOptions)
	ON_BN_CLICKED(IDC_ShowClassNames, OnShowClassNames)
	ON_BN_CLICKED(IDC_ShowFieldNames, OnShowFieldNames)
	ON_BN_CLICKED(IDC_TestFields, OnTestFields)
	ON_BN_CLICKED(IDC_TrainingFields, OnTrainingFields)
	ON_BN_CLICKED(IDC_ShowTrainTestLabel, OnShowTrainTestLabel)
	ON_CBN_DROPDOWN(IDC_TestMaskPopUp, OnDropdownTestMaskCOMBO)
	ON_CBN_SELENDOK(IDC_TestMaskPopUp, OnSelendokTestMaskCOMBO)
	ON_CBN_DROPDOWN(IDC_TrainMaskPopUp, OnDropdownTrainMaskCOMBO)
	ON_CBN_SELENDOK(IDC_TrainMaskPopUp, OnSelendokTrainMaskCOMBO)
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
//	Revised By:			Larry L. Biehl			Date: 02/26/99	

SInt16 
CMStatisticsDialog::DoDialog(
				SInt16*								featurePtr,
				SInt16								totalNumberChannels,
				Handle*								trainMaskFileInfoHandlePtr,
				Handle*								testMaskFileInfoHandlePtr)

{  
	Boolean								continueFlag = FALSE;
						
	INT_PTR								returnCode;
	SInt16								statisticsRequest;

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
																			
	m_localFeaturesPtr = (UInt16*)featurePtr;  
	m_totalNumberChannels = totalNumberChannels;
	m_trainMaskFileInfoHandle = *trainMaskFileInfoHandlePtr;
	m_testMaskFileInfoHandle = *testMaskFileInfoHandlePtr;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{                 
		statisticsRequest = 1; 
				
		StatisticsDialogOK ( m_channelSelection,
									m_totalNumberChannels,
									m_localActiveFeaturesPtr, 
									m_localActiveNumberFeatures, 
									m_showTrainingFields,
									m_showTestFields,
									m_showClassNames, 
									m_showFieldNames,
									m_showTrainTestText,  
									m_outlineColorSelection + 1,
									m_localStatCode,       
									m_keepClassStatsFlag,
									m_variance,
									m_minLogDeterminant,
									m_useCommonCovarianceInLOOCFlag ); 
									
		continueFlag = TRUE;
		
		}		// end "if (returnCode == IDOK)"
		
	CheckAndUnlockHandle ( gProjectInfoPtr->windowInfoHandle ); 
	
	*trainMaskFileInfoHandlePtr = m_trainMaskFileInfoHandle;
	*testMaskFileInfoHandlePtr = m_testMaskFileInfoHandle;
		
	return (continueFlag);
		
}		// end "DoDialog"


/////////////////////////////////////////////////////////////////////////////
// CMStatisticsDialog message handlers

BOOL 
CMStatisticsDialog::OnInitDialog()

{  
	SInt16								localChannelSelection,
											localLabelFieldCode,
											localOutlineColorSelection,
											localOutlineFieldType,
											localTestMaskCombo,
											localTrainMaskCombo;
	
	
	CDialog::OnInitDialog();
	
			// Initialize dialog variables.
			
	StatisticsDialogInitialize ( this,
												m_totalNumberChannels,
												&m_localStatCode,
												&m_keepClassStatsFlag,
												&m_useCommonCovarianceInLOOCFlag,
												&m_variance,
												&m_minLogDeterminant,
												&localChannelSelection,
												&m_channelListType,
												m_localFeaturesPtr,
												&m_localNumberFeatures,
												&localOutlineFieldType,
												&localLabelFieldCode,
												&localOutlineColorSelection,
												&localTrainMaskCombo,
												&localTestMaskCombo,
												&m_maxNumberTrainLayers,
												&m_maxNumberTestLayers);
	
			// Set feature parameters 
			
	m_localActiveNumberFeatures = m_localNumberFeatures; 
	m_localActiveFeaturesPtr = m_localFeaturesPtr;
	m_channelSelection = localChannelSelection;
	
			// Outline training areas.															
	
	m_showTrainingFields = (localOutlineFieldType & 0x0001);
	
			// Outline test areas.																
			
	m_showTestFields = (localOutlineFieldType & 0x0002);
	
			// Put labels with the area outlines.              
	
	m_showClassNames = (localLabelFieldCode & 0x0001);

	m_showFieldNames = FALSE;
	if (localLabelFieldCode & 0x0002)
		m_showFieldNames = TRUE;

	m_showTrainTestText = FALSE;
	if (localLabelFieldCode & 0x0004)
		m_showTrainTestText = TRUE;
	
	m_outlineColorSelection = localOutlineColorSelection;
	
			// Make the train and test mask combo variables to be 0 based.
	
	m_trainMaskCombo = localTrainMaskCombo - 1;	
	m_testMaskCombo = localTestMaskCombo - 1; 
	            
			// Make the outline color combo variable to be 0 based.
			
	m_outlineColorSelection--;
		
			// Update the dialog box parameters and then center the dialog.
	                 
	if (UpdateData(FALSE) )
		PositionDialogWindow (); 
	
	SetOutlineAreaOptions ();

	if (localTrainMaskCombo == 3)
		SelectDialogItemText (this, IDC_TrainMaskLayer, 0, SInt16_MAX);
	else if (localTestMaskCombo == 3)
		SelectDialogItemText (this, IDC_TestMaskLayer, 0, SInt16_MAX);
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog" 



void CMStatisticsDialog::OnSelendokChannels()
{                                                                
	HandleChannelsMenu(IDC_Channels, 
								kNoTransformation,
								m_totalNumberChannels,
								kImageChannelType,
								TRUE);
	
}		// end "OnSelendokChannels"



void CMStatisticsDialog::OnSelendokProjectChanges()
{                                                           
	DDX_CBIndex(m_dialogFromPtr, IDC_ProjectChanges, m_projectCommands);
	if (m_projectCommands == 1)
		{
		SInt16 returnCode = m_channelListType;
		if ( ProjectMenuClearStatistics() )
			returnCode = 1; 
							
				// If project stats were cleared, then reload the local	
				// feature vector to include all channels.					
							
		if (m_channelListType == kSelectedItemsListOnly && 
																	returnCode == 1)
			{
			SInt16		index;
			
			m_channelListType = 1;
			m_channelSelection = kAllMenuItem;
			for (index=0;
					index<gProjectInfoPtr->numberStatisticsChannels;
					index++)
				m_localFeaturesPtr[index] = index;
			m_localActiveNumberFeatures = 
									gProjectInfoPtr->numberStatisticsChannels; 
			
			UpdateAllSubsetList (IDC_Channels);
				
			DDX_CBIndex(m_dialogToPtr, IDC_Channels, m_channelSelection);
						
			}		// end "if (m_channelListType == ..."
		
		m_projectCommands = 0;	
		DDX_CBIndex(m_dialogToPtr, IDC_ProjectChanges, m_projectCommands);
		
		}		// end "if (m_projectCommands == 1)"
	
}		// end "OnSelendokProjectChanges"



void CMStatisticsDialog::OnDropdownProjectChanges()
{                                                          
	if (gProjectInfoPtr->statsLoaded)
		{
				// Make sure that the clear statistics command is in the list.
				
		if ( ((CComboBox*)GetDlgItem(IDC_ProjectChanges))->GetCount() == 1 )
			 ((CComboBox*)GetDlgItem(IDC_ProjectChanges))->
			 											AddString((LPCTSTR)_T("Clear Statistics..."));
		
		}		// end "if (gProjectInfoPtr->statsLoaded)" 
		
	else		// !gProjectInfoPtr->statsLoaded
		{
				// Make sure that the clear statistics command is not in the list.
				
		if ( ((CComboBox*)GetDlgItem(IDC_ProjectChanges))->GetCount() == 2 )
			 ((CComboBox*)GetDlgItem(IDC_ProjectChanges))->DeleteString(1);
		
		}		// end "else !gProjectInfoPtr->statsLoaded"
	
}		// end "OnDropdownProjectChanges"



void CMStatisticsDialog::OnDropdownChannels()
{  
	UpdateAllSubsetList (IDC_Channels);
	
}		// end "OnDropdownChannels"

 

void CMStatisticsDialog::OnStatisticsOptions()
{                          
	SetDLogControlHilite (NULL, IDOK, 255); 
	
	StatisticsOptionsDialog (
									&m_localStatCode, 
									&m_keepClassStatsFlag, 
									&m_variance,
									&m_minLogDeterminant,
									&m_useCommonCovarianceInLOOCFlag );
									
	SetDLogControlHilite (NULL, IDOK, 0); 
	
} 		// end "OnStatisticsOptions" 



void CMStatisticsDialog::SetOutlineAreaOptions (void)					

{    
	if (m_showTrainingFields || m_showTestFields)
		{                                         
		DDX_Check(m_dialogToPtr, IDC_ShowClassNames, m_showClassNames);
		DDX_Check(m_dialogToPtr, IDC_ShowFieldNames, m_showFieldNames);
		DDX_Check(m_dialogToPtr, IDC_ShowTrainTestLabel, m_showTrainTestText); 
		                                          
		SetDLogControlHilite (this, IDC_ShowClassNames, 0);
		SetDLogControlHilite (this, IDC_ShowFieldNames, 0);
		SetDLogControlHilite (this, IDC_ShowTrainTestLabel, 0); 
			                                
		ShowDialogItem (this, IDC_ColorPrompt);
		ShowDialogItem (this, IDC_ColorCombo);
		
		}		// end "if (m_showTrainingFields || m_showTestFields)"
		
	else		// !m_showTrainingFields && !m_showTestFields 
		{   
		BOOL	falseSetting = FALSE;
	                                
		DDX_Check(m_dialogToPtr, IDC_ShowClassNames, falseSetting);
		DDX_Check(m_dialogToPtr, IDC_ShowFieldNames, falseSetting);
		DDX_Check(m_dialogToPtr, IDC_ShowTrainTestLabel, falseSetting);
		                                          
		SetDLogControlHilite (this, IDC_ShowClassNames, 255);
		SetDLogControlHilite (this, IDC_ShowFieldNames, 255);
		SetDLogControlHilite (this, IDC_ShowTrainTestLabel, 255); 
			                                
		HideDialogItem (this, IDC_ColorPrompt);
		HideDialogItem (this, IDC_ColorCombo);
		
		}		// end "else !m_showTrainingFields && !m_showTestFields"
	
}		// end "SetOutlineAreaOptions"   



void CMStatisticsDialog::OnTrainingFields()
{                                                                            
	DDX_Check(m_dialogFromPtr, IDC_TrainingFields, m_showTrainingFields);
	
	SetOutlineAreaOptions ();
	
}		// end "OnTrainingFields"  

 

void CMStatisticsDialog::OnTestFields()
{                                                           
	DDX_Check(m_dialogFromPtr, IDC_TestFields, m_showTestFields);
	
	SetOutlineAreaOptions ();
	
}		// end "OnTestFields"   

 

void CMStatisticsDialog::OnShowClassNames()
{                                                        
	DDX_Check(m_dialogFromPtr, IDC_ShowClassNames, m_showClassNames);
	
}		// end "OnShowClassNames"



void CMStatisticsDialog::OnShowFieldNames()
{                                                           
	DDX_Check(m_dialogFromPtr, IDC_ShowFieldNames, m_showFieldNames);
	
}		// end "OnShowFieldNames" 



void CMStatisticsDialog::OnShowTrainTestLabel()
{                                                           
	DDX_Check(m_dialogFromPtr, IDC_ShowTrainTestLabel, m_showTrainTestText);
	
}		// end "OnShowTrainTestLabel"   

void CMStatisticsDialog::OnDropdownTestMaskCOMBO()
{
	// TODO: Add your control notification handler code here
	
}

void CMStatisticsDialog::OnSelendokTestMaskCOMBO()
{
	SInt16							itemHit,
										savedTestMaskCombo;
	
	
	savedTestMaskCombo = m_testMaskCombo;
	                                    
	DDX_CBIndex(m_dialogFromPtr, 
					IDC_TestMaskPopUp, 
					m_testMaskCombo); 
													
	if (m_testMaskCombo >= 0)
		{						
		itemHit = StatisticsDialogSelectMaskItem (
									&m_testMaskFileInfoHandle,
									this,
									NULL,
									m_testMaskCombo+1,
									savedTestMaskCombo+1,
									IDS_FileIO96,
									IDC_TestMaskPopUp,
									IDC_TestMaskLayer,
									&m_maxNumberTestLayers);
									
		m_testMaskCombo = itemHit - 1;
	                                    
		DDX_CBIndex(m_dialogToPtr, 
						IDC_TestMaskPopUp, 
						m_testMaskCombo); 
		
		}		// end "if (m_testMaskCombo >= 0)"
	
}		// end "OnSelendokTestMaskCOMBO"



void CMStatisticsDialog::OnDropdownTrainMaskCOMBO()
{
	// TODO: Add your control notification handler code here
	
}		// end "OnDropdownTrainMaskCOMBO"



void CMStatisticsDialog::OnSelendokTrainMaskCOMBO()
{
	SInt16							itemHit,
										savedTrainMaskCombo;
	
	
	savedTrainMaskCombo = m_trainMaskCombo;
	                                    
	DDX_CBIndex(m_dialogFromPtr, 
					IDC_TrainMaskPopUp, 
					m_trainMaskCombo); 
													
	if (m_trainMaskCombo >= 0)
		{						
		itemHit = StatisticsDialogSelectMaskItem (
									&m_trainMaskFileInfoHandle,
									this,
									NULL,
									m_trainMaskCombo+1,
									savedTrainMaskCombo+1,
									IDS_FileIO95,
									IDC_TrainMaskPopUp,
									IDC_TrainMaskLayer,
									&m_maxNumberTrainLayers);	
									
		m_trainMaskCombo = itemHit - 1;
	                                    
		DDX_CBIndex(m_dialogToPtr, 
						IDC_TrainMaskPopUp, 
						m_trainMaskCombo); 
		
		}		// end "if (m_trainMaskCombo >= 0)"
	
}		// end "OnSelendokTrainMaskCOMBO" 

             

void CMStatisticsDialog::OnOK()
{  
	SInt16					itemHit = 1;

	UInt16					testLayerNumber,
								trainLayerNumber;
	
	
	if (UpdateData(TRUE))
		{                                   
				// User selected OK for information. 

				// Check channels used for train and test mask
			
		trainLayerNumber = 1;
		if (itemHit == 1 && m_trainMaskCombo == 2 && m_maxNumberTrainLayers > 1)
			{
			itemHit = CheckMaxValue (this,
												IDC_TrainMaskLayer,
												1,
												m_maxNumberTrainLayers,
												kDisplayRangeAlert);

			trainLayerNumber = (UInt16)GetDItemValue(this, IDC_TrainMaskLayer);

			}	// end "if (itemHit == 1 && gMaskTrainImageSelection == 3 && ..."


		testLayerNumber = 1;
		if (itemHit == 1 && m_testMaskCombo == 2 && m_maxNumberTestLayers > 1)
			{
			itemHit = CheckMaxValue (this,
												IDC_TestMaskLayer,
												1,
												m_maxNumberTestLayers,
												kDisplayRangeAlert);

			testLayerNumber = (UInt16)GetDItemValue (this, IDC_TestMaskLayer);

			}	// end "if (itemHit == 1 && gMaskTestImageSelection == 3 && ..."
		
				// Load the mask data into memory if a mask file was selected.
		
		if (itemHit == 1)
			itemHit = StatisticsDialogMaskCheck (
													m_trainMaskFileInfoHandle,
													m_testMaskFileInfoHandle,
													m_trainMaskCombo + 1,
													m_testMaskCombo + 1,
													trainLayerNumber,
													testLayerNumber);
		
		if (itemHit == 1)
			CMDialog::OnOK(); 
			
		}		// end "if ( UpdateData(TRUE) )"
		
}		// end "OnOK"
