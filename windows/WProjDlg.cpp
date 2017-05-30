// wprojdlg.cpp : implementation file
//               
                   
#include "SMulSpec.h"
 
#include "WProjDlg.h"
#include	"WChanDlg.h"       

#include	"SExtGlob.h" 

extern SInt16 		EditLineColumnDialogCheckCoordinates ( 
							DialogPtr							dialogPtr, 
							Handle								inputWindowInfoHandle,
							SInt16								selectionDisplayUnits,
							LongRect*							selectionRectanglePtr,
							LongRect*							minMaxSelectionRectanglePtr,
							DoubleRect*							coordinateRectanglePtr,
							DoubleRect*							minMaxCoordinateRectanglePtr,
							double								newColumnStart,
							double								newColumnEnd,
							double								newLineStart,
							double								newLineEnd);

extern void			ChangeFieldType (
							SInt16								classStorage, 
							SInt16								currentField, 
							SInt16								fieldType); 		
                         
extern void 		ComputeMapCoordinates (
							Handle								windowInfoHandle,
							SInt16								viewUnits,
							LongRect*							lineColumnRectPtr,
							DoubleRect*							coordinateRectPtr);

extern void 		EditClassFieldDialogInitialize (
							DialogPtr							dialogPtr, 
							SInt16								classFieldFlag,
							SInt16								classStorage,
							SInt16								currentField,
							UInt16*								selectedItemPtr);

extern void 		EditLineColumnDialogOK (
							Handle								windowInfoHandle,
							LongRect*							inputSelectionRectanglePtr,
							LongRect*							selectionRectanglePtr,
							DoubleRect*							coordinateRectanglePtr,
							SInt16								unitsDisplayCode,
							Boolean*								changedFlagPtr);	

extern void 		EditLineColumnDialogSetStartLC ( 
							DialogPtr							dialogPtr,  
							SInt16								unitsDisplayCode,
							Boolean								applyToAllWindowsFlag);

extern void 		EditSelectionDialogSetCoordinates (
							DialogPtr							dialogPtr,
							Handle								windowInfoHandle,
							LongRect*							inputSelectionRectanglePtr,
							LongRect*							selectionRectanglePtr,
							DoubleRect*							inputCoordinateRectanglePtr,
							DoubleRect*							coordinateRectanglePtr,
							LongRect*							minMaxSelectionRectanglePtr,
							DoubleRect*							minMaxCoordinateRectanglePtr,
							SInt16								requestedSelectionUnitsCode,
							SInt16								currentSelectionUnitsCode);

extern SInt16 		GetFileGridCoordinateCode (
							Handle								windowInfoHandle);		

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

extern void 		EditLineColumnDialogInitialize (
							DialogPtr							dialogPtr,
							WindowPtr							windowPtr,
							WindowInfoPtr						windowInfoPtr,
							SInt16								pointType,
							SInt16								unitsDisplayCode,
							LongRect*							inputSelectionRectanglePtr,
							LongRect*							selectionRectanglePtr,
							DoubleRect*							coordinateRectanglePtr,
							LongRect*							minMaxSelectionRectanglePtr,
							DoubleRect*							minMaxCoordinateRectanglePtr,
							DoubleRect*							inputCoordinateRectanglePtr,
							Boolean*								applyToAllWindowsFlagPtr,
							Boolean*								useStartLineColumnFlagPtr,
							SInt16*								selectionDisplayUnitsPtr,
							SInt16								stringID);

extern void 		EditSelectionDialogShowSelection (
							WindowPtr							windowPtr,
							Handle								windowInfoHandle,
							LongRect*							selectionRectanglePtr,
							DoubleRect*							coordinateRectanglePtr,
							Boolean								applyToAllWindowsFlag,
							Boolean								useStartLineColumnFlag,
							SInt16								unitsToUseCode,
							double								factor);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif     

/////////////////////////////////////////////////////////////////////////////
// CMEditClassFieldDlg dialog


CMEditClassFieldDlg::CMEditClassFieldDlg(CWnd* pParent /*=NULL*/)
	: CMDialog(CMEditClassFieldDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMEditClassFieldDlg)
	m_className = "Class 1";
	m_fieldName = "Field 1";
	m_fieldType = 0;
	//}}AFX_DATA_INIT 
	
	m_classNameCStringPtr = NULL;
	m_fieldNameCStringPtr = NULL;  
	
	TRY
		{																			
				//	Get pointers to the string buffers.	
			
		m_classNameCStringPtr = (TBYTE*)m_className.GetBufferSetLength(64);
		m_fieldNameCStringPtr = (TBYTE*)m_fieldName.GetBufferSetLength(64);
				
		}
			
	CATCH_ALL(e)
		{
		m_initializedFlag = FALSE;
			
		}
	END_CATCH_ALL
	
	m_initializedFlag = TRUE;
}

void CMEditClassFieldDlg::DoDataExchange(CDataExchange* pDX)
{
	CMDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMEditClassFieldDlg)        
	DDX_Text(pDX, IDC_ClassName, m_className);
	DDV_MaxChars(pDX, m_className, 31);
	DDX_Text(pDX, IDC_FieldName, m_fieldName);
	DDV_MaxChars(pDX, m_fieldName, 31);
	DDX_Radio(pDX, IDC_Training, m_fieldType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMEditClassFieldDlg, CMDialog)
	//{{AFX_MSG_MAP(CMEditClassFieldDlg)               
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the reformat
//							options dialog box to the user so that the user can
//							selection which reformat function is to be run.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/19/96
//	Revised By:			Larry L. Biehl			Date: 02/19/96	

Boolean 
CMEditClassFieldDlg::DoDialog(
				SInt16								classFieldCode, 
				SInt16								currentClass,
				SInt16								currentField,
				char*									titleStringPtr)

{    
	#if defined _UNICODE
		USES_CONVERSION;
	#endif              

	UInt8						string[256];

	Boolean					OKFlag = FALSE;
	
	INT_PTR					returnCode;
	
	
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
																			
	m_classFieldCode = classFieldCode;
	m_currentField = currentField;
	
	m_classStorage = gProjectInfoPtr->storageClass[currentClass];
	
	m_titleStringPtr = titleStringPtr;
	            																			
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{ 
		OKFlag = TRUE;   
		
		if (classFieldCode == 2)
			{ 
					// Class Name
					                  
			#if defined _UNICODE
				strcpy ((char*)string, T2A(m_classNameCStringPtr)); 
			#endif                                 
			#if !defined _UNICODE
				strcpy ((char*)string, (const char*)m_classNameCStringPtr); 
			#endif                                        
			CtoPstring (string, 
							gProjectInfoPtr->classNamesPtr[m_classStorage].name);
						
			}		// end "if (classFieldCode == 2)" 
					             
		if (classFieldCode == 3)
			{                      
			HPFieldIdentifiersPtr fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
			
					// Field Identifier
					  
			#if defined _UNICODE
				strcpy ((char*)string, T2A(m_fieldNameCStringPtr)); 
			#endif                                 
			#if !defined _UNICODE
				strcpy ((char*)string, (const char*)m_fieldNameCStringPtr); 
			#endif               
			CtoPstring (string, 
								gProjectInfoPtr->fieldIdentPtr[m_currentField].name);
					
					// Training field or test field						
					                
			SInt16 fieldType = kTrainingType; 
			if (m_fieldType == 1)
				fieldType = kTestingType;
				
			fieldIdentPtr[currentField].fieldType = fieldType;
				
			}		// end "if (classFieldCode == 3)" 
		
		}		// end "if (returnCode == IDOK)"
		
	return (OKFlag);
		
}		// end "DoDialog"


/////////////////////////////////////////////////////////////////////////////
// CMEditClassFieldDlg message handlers

BOOL 
CMEditClassFieldDlg::OnInitDialog(void)

{                                         
	UInt16					selectedItem;
	
	
	CDialog::OnInitDialog();

	EditClassFieldDialogInitialize (this, 
												m_classFieldCode,
												m_classStorage,
												m_currentField,
												&selectedItem);
												
	SetWindowText ((LPCTSTR)m_titleStringPtr); 
	
	PositionDialogWindow ();  
		
			// Set default text selection 
	                                     
	SelectDialogItemText (this, selectedItem, 0, SInt16_MAX);  			
	
	return FALSE;  // return TRUE  unless you set the focus to a control 
	
}		// end "OnInitDialog"



void 
CMEditClassFieldDlg::OnOK(void)

{  
	#if defined _UNICODE
		USES_CONVERSION;
	#endif              

	UInt8			string[256];
	Boolean 		OKFlag = TRUE;
	SInt16		returnCode; 
	UInt16		selectedItem;


	UpdateData(TRUE); 
	
	if (m_classFieldCode == 2)
		{              
		#if defined _UNICODE
			strcpy ((char*)string, T2A(m_classNameCStringPtr)); 
		#endif                                 
		#if !defined _UNICODE
			strcpy ((char*)string, (const char*)m_classNameCStringPtr); 
		#endif                                 
		CtoPstring (string, gTextString);
						
				// Check if proposed class name is a duplicate.	 If it is, 	
				// allow user to change it again.									
							                                                         
		returnCode = CheckForDuplicateClassName (m_classStorage, (Str255*)gTextString);
		if ( returnCode > 0 )
			{ 			 
			if ( returnCode == 1 )
				DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
						
			else		// returnCode == 2
				DupClassFieldNameAlert (1, (UCharPtr)gTextString);
							                                   
			selectedItem = IDC_ClassName;
			OKFlag = FALSE;
			
			}		// end "if ( returnCode > 0 )"                                     
			
		}		// end "if (m_classFieldCode == 2)" 
	
	if (OKFlag && m_classFieldCode == 3)
		{	    
		#if defined _UNICODE
			strcpy ((char*)string, T2A(m_fieldNameCStringPtr)); 
		#endif                                 
		#if !defined _UNICODE
			strcpy ((char*)string, (const char*)m_fieldNameCStringPtr); 
		#endif   
		CtoPstring (string, gTextString);
								                              
				// Check if proposed field name is a duplicate.	 If it  	
				// is, allow user to change it again.							
								                                
		returnCode = CheckForDuplicateFieldName (m_currentField, (Str255*)gTextString);
		if ( returnCode > 0 )
			{                                        
			if ( returnCode == 1 )
				DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
						
			else		// returnCode == 2
				DupClassFieldNameAlert (2, gTextString);
				     
			selectedItem = IDC_FieldName;
			OKFlag = FALSE;
			
			}		// end "if ( returnCode > 0 )"                                        
			
		}		// end "if (OKFlag && ...)"
	
	if (OKFlag)
		CMDialog::OnOK();
		
	else		// !OKFlag                      
		SelectDialogItemText (this, selectedItem, 0, SInt16_MAX); 
		
}		// end "OnOK"



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
	DDX_Text(pDX, IDC_minLogDetValue, m_minLogDetValue);
	DDX_Text(pDX, IDC_Variance, m_varianceValue);
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



void 
CMStatOptionsDlg::SetZeroVariance(void)

{                                                                          
	ShowHideDialogItem (this, IDC_Variance, m_setZeroVarianceFlag);
	if (m_setZeroVarianceFlag)
		{                                                                                               
//		GetDlgItem(IDC_Variance)->SetFocus();
//		SendDlgItemMessage( IDC_Variance, EM_SETSEL, 0, MAKELPARAM(0, -1) );
		SelectDialogItemText (this, IDC_Variance, 0, SInt16_MAX);  			
		
		}		// end "if (m_setZeroVarianceFlag)"
	
}		// end "SetZeroVariance"



/////////////////////////////////////////////////////////////////////////////
// CMEditCoordinatesDlg dialog


CMEditCoordinatesDlg::CMEditCoordinatesDlg(CWnd* pParent /*=NULL*/)
	: CMDialog(CMEditCoordinatesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMEditCoordinatesDlg)
	m_applyToAllWindowsFlag = FALSE;
	m_listSelectionUnits = -1;
	m_newColumnStart = 0.0;
	m_newColumnEnd = 0.0;
	m_newLineEnd = 0.0;
	m_newLineStart = 0.0;
	m_useStartLineColumnFlag = FALSE;
	//}}AFX_DATA_INIT
	
	m_pointType = 0;                   
	m_windowInfoPtr = NULL;
	m_previewWasUsedFlag = FALSE; 
	m_valueChangedFlag = FALSE;
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
}



void CMEditCoordinatesDlg::DoDataExchange(CDataExchange* pDX)
{
	UInt16					controlID;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMEditCoordinatesDlg)
	DDX_Check(pDX, IDC_ApplyToAllCheckbox, m_applyToAllWindowsFlag);
	DDX_CBIndex(pDX, IDC_CoordinateUnits, m_listSelectionUnits);
	DDX_Text(pDX, IDC_NewColumnStart, m_newColumnStart);
	DDX_Text(pDX, IDC_NewColumnEnd, m_newColumnEnd);
	DDX_Text(pDX, IDC_NewLineEnd, m_newLineEnd);
	DDX_Text(pDX, IDC_NewLineStart, m_newLineStart);
	DDX_Check(pDX, IDC_StartLCCheckBox, m_useStartLineColumnFlag);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
		{
				// Verify that the coordinates make sense.

		if (!CheckLineColumnValues (&controlID))
			{
			pDX->PrepareEditCtrl (controlID);
			pDX->Fail();

			}		// end "if (!CheckLineColumnValues ())"

		}		// end "if (pDX-m_bSaveAndValidate)"

	DDV_MinMaxDouble(pDX, m_newLineEnd, -99999999, 99999999);
}		// end "DoDataExchange"     



BEGIN_MESSAGE_MAP(CMEditCoordinatesDlg, CMDialog)
	//{{AFX_MSG_MAP(CMEditCoordinatesDlg)
	ON_EN_CHANGE(IDC_NewColumnEnd, OnChangeNewColumnEnd)
	ON_EN_CHANGE(IDC_NewColumnStart, OnChangeNewColumnStart)
	ON_EN_CHANGE(IDC_NewLineEnd, OnChangeNewLineEnd)
	ON_EN_CHANGE(IDC_NewLineStart, OnChangeNewLineStart)
	ON_BN_CLICKED(IDC_Preview, OnPreview)
	ON_CBN_SELENDOK(IDC_CoordinateUnits, OnSelendokCoordinateUnits)
	ON_BN_CLICKED(IDC_ApplyToAllCheckbox, OnApplyToAllCheckbox)
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
//	Coded By:			Larry L. Biehl			Date: 02/20/1996
//	Revised By:			Larry L. Biehl			Date: 03/18/2005	

Boolean 
CMEditCoordinatesDlg::DoDialog(
            WindowPtr							windowPtr,
				WindowInfoPtr						windowInfoPtr,
				LongRect*							inputSelectionRectanglePtr,
				LongRect*							selectionRectanglePtr,
				DoubleRect*							coordinateRectanglePtr,
				SInt16								pointType,
				SInt16*								unitsDisplayCodePtr,
				Boolean*								changedFlagPtr,
				Boolean*								applyToAllWindowsFlagPtr,
				Boolean*								useStartLineColumnFlagPtr,
				Boolean*								previewWasUsedFlagPtr,
				SInt16								stringID)

{  
	Boolean								continueFlag = FALSE;
						
	INT_PTR								returnCode; 

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																			return(FALSE);
								       
	m_windowPtr = windowPtr;
	m_windowInfoPtr = windowInfoPtr;
	m_inputSelectionRectanglePtr = inputSelectionRectanglePtr;
	m_selectionRectanglePtr = selectionRectanglePtr;
	m_coordinateRectanglePtr = coordinateRectanglePtr;                     								
	m_pointType = pointType;
	m_selectionUnits = *unitsDisplayCodePtr;
	m_stringID = stringID;
		
			// Initialize new rectangle coordinates. 									
				
	m_newLineStart = inputSelectionRectanglePtr->top;
	m_newColumnStart = inputSelectionRectanglePtr->left;
	m_newLineEnd = inputSelectionRectanglePtr->bottom;
	m_newColumnEnd = inputSelectionRectanglePtr->right;

	m_applyToAllWindowsFlag = *applyToAllWindowsFlagPtr;
	m_useStartLineColumnFlag = *useStartLineColumnFlagPtr;

	m_coordinateUnitsChangingFlag = FALSE;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{                                
		continueFlag = TRUE; 
		*unitsDisplayCodePtr = m_selectionUnits;
		*useStartLineColumnFlagPtr = m_useStartLineColumnFlag;
		
		}		// end "if (returnCode == IDOK)" 

	*previewWasUsedFlagPtr = m_previewWasUsedFlag;
	*applyToAllWindowsFlagPtr = m_applyToAllWindowsFlag;
		
	return (continueFlag);
		
}		// end "DoDialog"



/////////////////////////////////////////////////////////////////////////////
// CMEditCoordinatesDlg message handlers

BOOL CMEditCoordinatesDlg::OnInitDialog()
{  
	Boolean					applyToAllWindowsFlag,
								useStartLineColumnFlag;

	
			// Get maximum number of lines and columns allowed.
			
	CDialog::OnInitDialog();
	
			//	Set menu options.

	applyToAllWindowsFlag = m_applyToAllWindowsFlag;
	useStartLineColumnFlag = TRUE;
	m_coordinateUnitsChangingFlag = TRUE;
	
	EditLineColumnDialogInitialize (this,
												m_windowPtr,
												m_windowInfoPtr,
												m_pointType,
												m_selectionUnits,
												m_inputSelectionRectanglePtr,
												m_selectionRectanglePtr,
												m_coordinateRectanglePtr,
												&m_minMaxSelectionRectangle,
												&m_minMaxCoordinateRectangle,
												&m_inputCoordinateRectangle,
												&applyToAllWindowsFlag,
												&useStartLineColumnFlag,
												&m_selectionUnits,
												m_stringID);

	m_applyToAllWindowsFlag = applyToAllWindowsFlag;
	m_coordinateUnitsChangingFlag = FALSE;
			                             
	m_listSelectionUnits = GetComboListSelection(IDC_CoordinateUnits, 
																	m_selectionUnits);
	if (m_listSelectionUnits < 0)
		m_listSelectionUnits = 0;

			// Update the dialog box parameters and then center the dialog.
	                 
	if (UpdateData(FALSE) )
		PositionDialogWindow ();
		
			// Set default text selection to first edit text item 
			                                       
	SelectDialogItemText (this, IDC_NewLineStart, 0, SInt16_MAX);  			
	
	return FALSE;  // return TRUE  unless you set the focus to a control 
	
}		// end "OnInitDialog"



void CMEditCoordinatesDlg::OnChangeNewColumnStart()
{  
	double							previousValue;

	Boolean							errorAlertFlag = FALSE,
										updateValueFlag = FALSE;
	
	
	previousValue = m_newColumnStart;
	DDX_Text(m_dialogFromPtr, IDC_NewColumnStart, m_value);
								                                             
	if (m_selectionUnits == kLineColumnUnits)
		{                               
		if (m_valuePtr[0] != 0)                      
			DDX_Text(m_dialogFromPtr, IDC_NewColumnStart, m_newColumnStart);
				
		else		// m_valuePtr[0] == 0
			m_newColumnStart = 0; 
		
		if (m_newColumnStart < m_minMaxSelectionRectangle.left)
			{
			errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newColumnStart = m_minMaxSelectionRectangle.left;
			
			}		// end "if (m_newColumnStart < m_minMaxSelectionRectangle.left)"
		
		else if (m_newColumnStart > m_minMaxSelectionRectangle.right)
			{
			errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newColumnStart = m_minMaxSelectionRectangle.right;
			
			}		// end "else if (m_newColumnStart > m_minMaxSelectionRectangle.right)"

		m_selectionRectanglePtr->left = (SInt32)m_newColumnStart;

		}		// end "if (m_selectionUnits == kLineColumnUnits)"

	else		// m_selectionUnits != kLineColumnUnits
		{
		if (m_valuePtr[0] != 0 && !(m_valuePtr[0] == '-' && m_valuePtr[1] == 0))                      
			DDX_Text(m_dialogFromPtr, IDC_NewColumnStart, m_newColumnStart);
				
		else		// m_valuePtr[0] == 0
			m_newColumnStart = 0; 
		
		m_coordinateRectanglePtr->left = m_newColumnStart;

		}		// end "else m_selectionUnits != kLineColumnUnits"

	if (previousValue != m_newColumnStart && !m_coordinateUnitsChangingFlag)
		m_valueChangedFlag = TRUE;
		
	if (updateValueFlag)
		DDX_Text(m_dialogToPtr, IDC_NewColumnStart, m_newColumnStart);

	if (errorAlertFlag)
		NumberErrorAlert ((SInt32)m_newColumnStart, this, IDC_NewColumnStart);
		
//	CheckOKFlag ();
	
}		// end "OnChangeNewColumnStart"



void CMEditCoordinatesDlg::OnChangeNewColumnEnd()
{  
	double						previousValue;

	Boolean						errorAlertFlag = FALSE,
									updateValueFlag = FALSE;
	
	   
	previousValue = m_newColumnEnd;
	DDX_Text(m_dialogFromPtr, IDC_NewColumnEnd, m_value);
															                                             
	if (m_selectionUnits == kLineColumnUnits)
		{
		if (m_valuePtr[0] != 0)                      
			DDX_Text(m_dialogFromPtr, IDC_NewColumnEnd, m_newColumnEnd);
				
		else		// m_valuePtr[0] == 0
			m_newColumnEnd = 0; 
		
		if (m_newColumnEnd < m_minMaxSelectionRectangle.left)
			{
			errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newColumnEnd = m_minMaxSelectionRectangle.left;
			
			}		// end "if (m_newColumnEnd < m_minMaxSelectionRectangle.left)"
		
		else if (m_newColumnEnd > m_minMaxSelectionRectangle.right)
			{
			errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newColumnEnd = m_minMaxSelectionRectangle.right;
			
			}		// end "else if (m_newColumnEnd > m_minMaxSelectionRectangle.right)"

		m_selectionRectanglePtr->right = (SInt32)m_newColumnEnd;

		}		// end "if (m_selectionUnits == kLineColumnUnits)"

	else		// m_selectionUnits != kLineColumnUnits
		{
		if (m_valuePtr[0] != 0 && !(m_valuePtr[0] == '-' && m_valuePtr[1] == 0))                      
			DDX_Text(m_dialogFromPtr, IDC_NewColumnEnd, m_newColumnEnd);
				
		else		// m_valuePtr[0] == 0
			m_newColumnEnd = 0; 
		
		m_coordinateRectanglePtr->right = m_newColumnEnd;

		}		// end "else m_selectionUnits != kLineColumnUnits"

	if (previousValue != m_newColumnEnd && !m_coordinateUnitsChangingFlag)
		m_valueChangedFlag = TRUE;
		
	if (updateValueFlag)
		DDX_Text(m_dialogToPtr, IDC_NewColumnEnd, m_newColumnEnd);

	if (errorAlertFlag)
		NumberErrorAlert ((SInt32)m_newColumnEnd, this, IDC_NewColumnEnd);
		
//	CheckOKFlag ();
	
}		// end "OnChangeNewColumnEnd"


void CMEditCoordinatesDlg::OnChangeNewLineStart()
{                           
	double							previousValue;

	Boolean							errorAlertFlag = FALSE,
										updateValueFlag = FALSE;
	
	               
	previousValue = m_newLineStart;                              
	DDX_Text(m_dialogFromPtr, IDC_NewLineStart, m_value);
															                                             
	if (m_selectionUnits == kLineColumnUnits)
		{     
		if (m_valuePtr[0] != 0)                      
			DDX_Text(m_dialogFromPtr, IDC_NewLineStart, m_newLineStart);
				
		else		// m_valuePtr[0] == 0
			m_newLineStart = 0; 
		
		if (m_newLineStart < m_minMaxSelectionRectangle.top)
			{
			errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newLineStart = m_minMaxSelectionRectangle.top;
			
			}		// end "if (m_newLineStart < m_minMaxSelectionRectangle.top)"
		
		else if (m_newLineStart > m_minMaxSelectionRectangle.bottom)
			{
			errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newLineStart = m_minMaxSelectionRectangle.bottom;
			
			}		// end "else if (m_newLineStart > m_minMaxSelectionRectangle.bottom)"

		m_selectionRectanglePtr->top = (SInt32)m_newLineStart;

		}		// end "if (m_selectionUnits == kLineColumnUnits)"

	else		// m_selectionUnits != kLineColumnUnits
		{
		if (m_valuePtr[0] != 0 && !(m_valuePtr[0] == '-' && m_valuePtr[1] == 0))                      
			DDX_Text(m_dialogFromPtr, IDC_NewLineStart, m_newLineStart);
				
		else		// m_valuePtr[0] == 0
			m_newLineStart = 0; 
		
		m_coordinateRectanglePtr->bottom = m_newLineStart;

		}		// end "else if (m_selectionUnits != kLineColumnUnits)"

	if (previousValue != m_newLineStart && !m_coordinateUnitsChangingFlag)
		m_valueChangedFlag = TRUE;
		
	if (updateValueFlag)
		DDX_Text(m_dialogToPtr, IDC_NewLineStart, m_newLineStart);

	if (errorAlertFlag)
		NumberErrorAlert ((SInt32)m_newLineStart, this, IDC_NewLineStart);
		
//	CheckOKFlag ();
	
}		// end "OnChangeNewLineStart" 



void CMEditCoordinatesDlg::OnChangeNewLineEnd(void)

{                     
	double							previousValue;

	Boolean							errorAlertFlag = FALSE,
										updateValueFlag = FALSE;
	
	                           
	previousValue = m_newLineEnd;                   
	DDX_Text(m_dialogFromPtr, IDC_NewLineEnd, m_value);
															                                             
	if (m_selectionUnits == kLineColumnUnits)
		{  
		if (m_valuePtr[0] != 0)                      
			DDX_Text(m_dialogFromPtr, IDC_NewLineEnd, m_newLineEnd);
				
		else		// m_valuePtr[0] == 0
			m_newLineEnd = 0; 
		
		if (m_newLineEnd < m_minMaxSelectionRectangle.top)
			{
			errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newLineEnd = m_minMaxSelectionRectangle.top;
			
			}		// end "if (m_newLineEnd < m_minMaxSelectionRectangle.top)"
		
		else if (m_newLineEnd > m_minMaxSelectionRectangle.bottom)
			{
			errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newLineEnd = m_minMaxSelectionRectangle.bottom;
			
			}		// end "else if (m_newLineEnd > m_minMaxSelectionRectangle.bottom)"

		m_selectionRectanglePtr->bottom = (SInt32)m_newLineEnd;

		}		// end "if (m_selectionUnits == kLineColumnUnits)"

	else		// m_selectionUnits == kLineColumnUnits
		{
		if (m_valuePtr[0] != 0 && !(m_valuePtr[0] == '-' && m_valuePtr[1] == 0))                      
			DDX_Text(m_dialogFromPtr, IDC_NewLineEnd, m_newLineEnd);
				
		else		// m_valuePtr[0] == 0
			m_newLineEnd = 0; 
		
		m_coordinateRectanglePtr->top = m_newLineEnd;

		}		// end "if (m_selectionUnits == kLineColumnUnits)"

	if (previousValue != m_newLineEnd && !m_coordinateUnitsChangingFlag)
		m_valueChangedFlag = TRUE;
		
	if (updateValueFlag)
		DDX_Text(m_dialogToPtr, IDC_NewLineEnd, m_newLineEnd);

	if (errorAlertFlag)
		NumberErrorAlert ((SInt32)m_newLineStart, this, IDC_NewLineStart);
		
//	CheckOKFlag ();
	
}		// end "OnChangeNewLineEnd" 



void CMEditCoordinatesDlg::CheckOKFlag()
{              
				
			// Make certain the line-column values make sense.					
					
	if ( (m_newLineStart > m_newLineEnd) || 
									(m_newColumnStart > m_newColumnEnd) )
		MHiliteControl ( (CMImageView*)this, (ControlHandle)IDOK, 255);
				
	else		// (m_newLineStart <= m_newLineEnd) &&
				//				(m_newColumnStart <= m_newColumnEnd) 
		MHiliteControl ( (CMImageView*)this, (ControlHandle)IDOK, 0);                                            
	
}		// end "CheckOKFlag" 

void CMEditCoordinatesDlg::OnPreview() 
{

	if (CheckLineColumnValues (NULL))
/*
	SInt16							valueItemHit;


	valueItemHit = 0;
	if (m_valueChangedFlag)
		{
		valueItemHit = EditLineColumnDialogCheckCoordinates (
											this,
											m_selectionUnits,
											m_selectionRectanglePtr,
											m_coordinateRectanglePtr,
											&m_minMaxCoordinateRectangle,
											m_coordinateRectanglePtr->left,
											m_coordinateRectanglePtr->right,
											m_coordinateRectanglePtr->bottom,
											m_coordinateRectanglePtr->top);

		if (valueItemHit != 0)
			{
			SelectDialogItemText (this, valueItemHit, 0, SInt16_MAX);				

			}		// end "if (valueItemHit != 0)"

		else		// valueItemHit == 0
			m_valueChangedFlag = FALSE;

		}		// end "if (m_valueChangedFlag)"
*/

//	if (valueItemHit == 0)
		{
		DDX_Check(m_dialogFromPtr, IDC_ApplyToAllCheckbox, m_applyToAllWindowsFlag);
									
		EditSelectionDialogShowSelection (
										m_windowPtr,
										m_windowInfoPtr->windowInfoHandle,
										m_selectionRectanglePtr,
										m_coordinateRectanglePtr,
										m_applyToAllWindowsFlag,
										m_useStartLineColumnFlag,
										m_selectionUnits,
										1);
										
		m_previewWasUsedFlag = TRUE;

		}		// end "if (valueItemHit == 0)"
	
}		// end "OnPreview"



void CMEditCoordinatesDlg::OnSelendokCoordinateUnits() 
{
	CComboBox* 					comboBoxPtr;

	SInt16						previousSelectionUnits;


//	if (CheckLineColumnValues (NULL))
//		{
		comboBoxPtr = (CComboBox*)GetDlgItem(IDC_CoordinateUnits);

		previousSelectionUnits = m_selectionUnits;

		DDX_CBIndex(m_dialogFromPtr, IDC_CoordinateUnits, m_listSelectionUnits);
		m_selectionUnits = 
						(SInt16)comboBoxPtr->GetItemData(m_listSelectionUnits);
																	
		if (m_selectionUnits >= 0  && previousSelectionUnits != m_selectionUnits)
			{
			DDX_Check(m_dialogFromPtr, IDC_ApplyToAllCheckbox, m_applyToAllWindowsFlag);

			m_coordinateUnitsChangingFlag = TRUE;

			EditLineColumnDialogSetStartLC (this,  
														m_selectionUnits,
														m_applyToAllWindowsFlag);

			EditSelectionDialogSetCoordinates (
										this,
										m_windowInfoPtr->windowInfoHandle,
										m_inputSelectionRectanglePtr,
										m_selectionRectanglePtr,
										&m_inputCoordinateRectangle,
										m_coordinateRectanglePtr,
										&m_minMaxSelectionRectangle,
										&m_minMaxCoordinateRectangle,
										m_selectionUnits,
										previousSelectionUnits);

			m_coordinateUnitsChangingFlag = FALSE;

			}		// end "m_listSelectionUnits >= 0  && ..."

//		}		// end "if (CheckLineColumnValues ())"
	
}		// end "OnSelendokCoordinateUnits"



void CMEditCoordinatesDlg::OnApplyToAllCheckbox() 
{
	// TODO: Add your control notification handler code here

	DDX_Check(m_dialogFromPtr, IDC_ApplyToAllCheckbox, m_applyToAllWindowsFlag);

	EditLineColumnDialogSetStartLC (this,  
												m_selectionUnits,
												m_applyToAllWindowsFlag);
	
}		// end "OnApplyToAllCheckbox"



Boolean CMEditCoordinatesDlg::CheckLineColumnValues(
				UInt16*								controlIDPtr) 
{
	SInt16							valueItemHit;

	Boolean							returnFlag = TRUE;


	valueItemHit = 0;
	if (m_valueChangedFlag)
		{
		valueItemHit = EditLineColumnDialogCheckCoordinates (
											this,
											m_windowInfoPtr->windowInfoHandle,
											m_selectionUnits,
											m_selectionRectanglePtr,
											&m_minMaxSelectionRectangle,
											m_coordinateRectanglePtr,
											&m_minMaxCoordinateRectangle,
											m_coordinateRectanglePtr->left,
											m_coordinateRectanglePtr->right,
											m_coordinateRectanglePtr->bottom,
											m_coordinateRectanglePtr->top);

		if (valueItemHit != 0)
			{
			SelectDialogItemText (this, valueItemHit, 0, SInt16_MAX);
			returnFlag = FALSE;

			}		// end "if (valueItemHit != 0)"

		else		// valueItemHit == 0
			m_valueChangedFlag = FALSE;

		}		// end "if (m_valueChangedFlag)"

	if (controlIDPtr != NULL)
		*controlIDPtr = valueItemHit;

	return (returnFlag);
	
}		// end "CheckLineColumnValues"
