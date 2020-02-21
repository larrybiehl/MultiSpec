//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
//
//	File:						WPrincipalComponentsDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/03/2018
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMPrincipalCompDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
                     
#include "WMultiSpec.h"
#include "WPrincipalComponentsDialog.h"

		// Pointer to the Principal Components analysis information structure.	
		
extern  PrincipalCompSpecsPtr		gPrincipalCompSpecsPtr;

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMPrincipalCompDialog, CMDialog)
	//{{AFX_MSG_MAP (CMPrincipalCompDialog)
	ON_BN_CLICKED (IDC_Classes, OnClasses)
	ON_BN_CLICKED (IDC_ImageArea, OnImageArea)
	ON_BN_CLICKED (IDEntireImage, ToEntireImage)
	ON_BN_CLICKED (IDSelectedImage, ToSelectedImage)

	ON_CBN_SELENDOK (IDC_ChannelCombo, OnSelendokChannelCombo)
	ON_CBN_SELENDOK (IDC_ClassCombo, OnSelendokClassCombo)

	ON_EN_CHANGE (IDC_ColumnEnd, CheckColumnEnd)
	ON_EN_CHANGE (IDC_ColumnStart, CheckColumnStart)
	ON_EN_CHANGE (IDC_LineEnd, CheckLineEnd)
	ON_EN_CHANGE (IDC_LineStart, CheckLineStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMPrincipalCompDialog::CMPrincipalCompDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMPrincipalCompDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMPrincipalCompDialog)
	m_dataCode = -1;
	m_trainingFlag = FALSE;
	m_testFlag = FALSE;
	m_listEigenvectorFlag = FALSE;
	m_equalVariancesFlag = FALSE;
	m_saveEigenvectorFlag = FALSE;
	m_textWindowFlag = FALSE;
	m_diskFileFlag = FALSE;
	//}}AFX_DATA_INIT
	
	m_initializedFlag = CMDialog::m_initializedFlag; 
	
	if (m_initializedFlag)
		{                          
				// Get pointer to memory for temporary storage of channel list. 
			                                                                       
		m_localFeaturesPtr = (UInt16*)MNewPointer (
					(UInt32)gImageWindowInfoPtr->totalNumberChannels * sizeof (UInt16));
	                                                       
		m_initializedFlag = (m_localFeaturesPtr != NULL);
		
		}	// end "if (m_initializedFlag)"
	
	if (m_initializedFlag && gPrincipalCompSpecsPtr->projectFlag)
		{
				// Get memory for the local class list vector.	
				
		m_classListPtr = (UInt16*)MNewPointer (
					(UInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));
	                                                       
		m_initializedFlag = (m_classListPtr != NULL);
		
		}	// end "if (m_initializedFlag && ...->projectFlag)"
		 
}	// end "CMPrincipalCompDialog"



CMPrincipalCompDialog::~CMPrincipalCompDialog (void)

{                                                              
	m_classListPtr = CheckAndDisposePtr (m_classListPtr);
	m_localFeaturesPtr = CheckAndDisposePtr (m_localFeaturesPtr); 
	
}	// end "~CMPrincipalCompDialog"



void CMPrincipalCompDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
			// Make sure that the first radio button in the group is shown and enabled.
			
	Boolean showingFlag = GetDlgItem (IDC_Classes)->IsWindowVisible ();
	GetDlgItem (IDC_Classes)->ShowWindow (SW_SHOW);
	Boolean enabledFlag = GetDlgItem (IDC_Classes)->IsWindowEnabled ();
	GetDlgItem (IDC_Classes)->EnableWindow (TRUE);
		
	//{{AFX_DATA_MAP (CMPrincipalCompDialog)
	DDX_Radio (pDX, IDC_Classes, m_dataCode);
	DDX_Check (pDX, IDC_Training, m_trainingFlag);
	DDX_Check (pDX, IDC_Test, m_testFlag);
	DDX_Check (pDX, IDC_ListEigenvector, m_listEigenvectorFlag);
	DDX_Check (pDX, IDC_EqualizeVariances, m_equalVariancesFlag);
	DDX_Check (pDX, IDC_SaveEigenvectors, m_saveEigenvectorFlag);
	DDX_Check (pDX, IDC_TextWindow, m_textWindowFlag);
	DDX_Check (pDX, IDC_DiskFile, m_diskFileFlag);
	DDX_Text (pDX, IDC_LineEnd, m_LineEnd);
	DDV_MinMaxLong (pDX, m_LineEnd, 1, m_maxNumberLines);
	DDX_Text (pDX, IDC_LineInterval, m_LineInterval);
	DDV_MinMaxLong (pDX, m_LineInterval, 1, m_maxNumberLines);
	DDX_Text (pDX, IDC_LineStart, m_LineStart);
	DDV_MinMaxLong (pDX, m_LineStart, 1, m_maxNumberLines);
	DDX_Text (pDX, IDC_ColumnEnd, m_ColumnEnd);
	DDV_MinMaxLong (pDX, m_ColumnEnd, 1, m_maxNumberColumns);
	DDX_Text (pDX, IDC_ColumnInterval, m_ColumnInterval);
	DDV_MinMaxLong (pDX, m_ColumnInterval, 1, m_maxNumberColumns);
	DDX_Text (pDX, IDC_ColumnStart, m_ColumnStart);
	DDV_MinMaxLong (pDX, m_ColumnStart, 1, m_maxNumberColumns);
	DDX_CBIndex (pDX, IDC_ChannelCombo, m_channelSelection);
	DDX_CBIndex (pDX, IDC_ClassCombo, m_classSelection);
	//}}AFX_DATA_MAP
	
			// Set first radio button in the group back as it was.
					                      
	GetDlgItem (IDC_Classes)->EnableWindow (enabledFlag);
	GetDlgItem (IDC_Classes)->ShowWindow (gPrincipalCompSpecsPtr->projectFlag);

			// Verify that the line and column values make sense
		
	VerifyLineColumnStartEndValues (pDX);
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the principal
//							component specification dialog box to the user and copy the
//							revised back to the principal component specification structure
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
//	Coded By:			Larry L. Biehl			Date: 04/16/1997
//	Revised By:			Larry L. Biehl			Date: 05/19/1997

SInt16 CMPrincipalCompDialog::DoDialog (void)

{                                 
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE;
	

			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{ 						
		DialogSelectArea		dialogSelectArea;
	
	
		continueFlag = TRUE;						
				
		dialogSelectArea.lineStart = m_LineStart;
		dialogSelectArea.lineEnd = m_LineEnd;
		dialogSelectArea.lineInterval = m_LineInterval;
		dialogSelectArea.columnStart = m_ColumnStart;
		dialogSelectArea.columnEnd = m_ColumnEnd;
		dialogSelectArea.columnInterval = m_ColumnInterval; 	

		PrincipalComponentDialogOK (gPrincipalCompSpecsPtr,
												m_dataCode,
												m_classSelection,
												m_localNumberClasses,
												m_classListPtr,
												m_trainingFlag,
												m_testFlag,
												&dialogSelectArea,
												m_channelSelection,
												m_localActiveNumberFeatures,
												m_localFeaturesPtr,
												m_listEigenvectorFlag,
												m_equalVariancesFlag,
												m_saveEigenvectorFlag,
												m_textWindowFlag,
												m_diskFileFlag);
									
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



void CMPrincipalCompDialog::HideFieldTypeItems ()

{
	MHideDialogItem (this, IDC_ClassCombo);
	MHideDialogItem (this, IDC_Training);
	MHideDialogItem (this, IDC_Test);
	
}	// end "HideFieldTypeItems"



void CMPrincipalCompDialog::OnClasses ()

{
	ShowFieldTypeItems ();
	HideSomeAreaSelectionItems ();
		                                       
	SelectDialogItemText (this, IDC_LineInterval, 0, SInt16_MAX);
	
}	// end "OnClasses"



void CMPrincipalCompDialog::OnImageArea ()

{                                                          
	HideFieldTypeItems ();
	ShowSomeAreaSelectionItems ();
		                                       
	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);
	
}	// end "OnImageArea"



BOOL CMPrincipalCompDialog::OnInitDialog ()

{
	UInt32								index;
	
	UInt16								selectItem;
	
	
	CDialog::OnInitDialog ();
	
			// Make sure that we have the bitmaps for the entire image buttons.
	
	VERIFY (toEntireButton.AutoLoad (IDEntireImage, this));
	VERIFY (toSelectedButton.AutoLoad (IDSelectedImage, this));
	
			// Update the modal dialog with the default settings
			// Set radio button for "Classes".
	
	if (gPrincipalCompSpecsPtr->dataCode == kTrainingType)
		m_dataCode = 0;
	
	else if (gPrincipalCompSpecsPtr->dataCode == kAreaType)
		m_dataCode = 1;
	
	if (!gPrincipalCompSpecsPtr->projectFlag)
		MHideDialogItem (this, IDC_Classes);
	
			// Classes to use.
	
	m_classSelection = gPrincipalCompSpecsPtr->classSet;
	m_localNumberClasses = gPrincipalCompSpecsPtr->numberClasses;
	
	UInt16* classPtr = (UInt16*)GetHandlePointer (gPrincipalCompSpecsPtr->classHandle);
	
	if (gPrincipalCompSpecsPtr->projectFlag)
		{
		for (index=0; index<m_localNumberClasses; index++)
			m_classListPtr[index] = classPtr[index];
		
		}	// end "if (gPrincipalCompSpecsPtr->projectFlag)"
	
			// Training or Test fields.

	if (gPrincipalCompSpecsPtr->dataCode == kTrainingType)
		ShowFieldTypeItems ();
	
	else	// gPrincipalCompSpecsPtr->dataCode != kTrainingType"
		HideFieldTypeItems ();
	
			// Set control for including training fields.
	
	m_trainingFlag = (gPrincipalCompSpecsPtr->fieldTypeCode == kTrainingType);
	
			// Set control for including test fields.
	
	m_testFlag = (gPrincipalCompSpecsPtr->fieldTypeCode == kTestingType);
	if (gProjectInfoPtr && gProjectInfoPtr->numberStatTestFields <= 0)
		SetDLogControlHilite (this, IDC_Test, 255);
	
	if (gImageFileInfoPtr == NULL)
		{
		SetDLogControlHilite (this, IDC_ImageArea, 255);
		MHideDialogItem (this, IDC_LinePrompt);
		MHideDialogItem (this, IDC_ColumnPrompt);
		MHideDialogItem (this, IDC_LineInterval);
		MHideDialogItem (this, IDC_ColumnInterval);
		
		}	// end "if (gImageFileInfoPtr == NULL)"
	
			//	Selected area for principal components
			// 	Initialize selected area structure.
	
	InitializeDialogSelectArea (&m_dialogSelectArea,
											gImageWindowInfoPtr,
											gActiveImageWindow,
											gPrincipalCompSpecsPtr->columnStart,
											gPrincipalCompSpecsPtr->columnEnd,
											gPrincipalCompSpecsPtr->columnInterval,
											gPrincipalCompSpecsPtr->lineStart,
											gPrincipalCompSpecsPtr->lineEnd,
											gPrincipalCompSpecsPtr->lineInterval,
											14,
											10,
											kAdjustToBaseImage);
	
	m_LineStart = gPrincipalCompSpecsPtr->lineStart;
	m_LineEnd = gPrincipalCompSpecsPtr->lineEnd;
	m_LineInterval = gPrincipalCompSpecsPtr->lineInterval;
	m_ColumnStart = gPrincipalCompSpecsPtr->columnStart;
	m_ColumnEnd = gPrincipalCompSpecsPtr->columnEnd;
	m_ColumnInterval = gPrincipalCompSpecsPtr->columnInterval;

			// Set radio button for area.
	
	if (gPrincipalCompSpecsPtr->dataCode == kAreaType)
		{
		ShowSomeAreaSelectionItems ();
		
		selectItem = IDC_LineStart;
		
		}	// end "if (gPrincipalCompSpecsPtr->dataCode == kAreaType)"
	
	else	// gPrincipalCompSpecsPtr->dataCode != kAreaType
		{
		HideSomeAreaSelectionItems ();
		
		selectItem = IDC_LineInterval;
		
		}	// end "else gPrincipalCompSpecsPtr->dataCode != kAreaType"
	
			//	Set the All/Subset channels/features list item
	
	m_channelSelection = gPrincipalCompSpecsPtr->channelSet;
	m_localNumberFeatures = gPrincipalCompSpecsPtr->numberChannels;
	
	SInt16* channelsPtr = (SInt16*)GetHandlePointer (
															gPrincipalCompSpecsPtr->featureHandle);
	for (index=0; index<m_localNumberFeatures; index++)
		m_localFeaturesPtr[index] = channelsPtr[index];
	
			// Set feature parameters
	
	m_localActiveNumberFeatures =	m_localNumberFeatures;
	m_localActiveFeaturesPtr = m_localFeaturesPtr;
	
			// Set check box for "List eigenvector vectors".
	
	m_listEigenvectorFlag = gPrincipalCompSpecsPtr->listEigenvectorFlag;
	
			// Set check box for "Equalize variances".
	
	m_equalVariancesFlag = gPrincipalCompSpecsPtr->equalVariancesFlag;
	
			// Set check box for "Save eigenvector flag".
	
	m_saveEigenvectorFlag = gPrincipalCompSpecsPtr->saveEigenvectorFlag;
	
			// Set check box for "text output window".
	
	if (gPrincipalCompSpecsPtr->outputStorageType & 0x0001)
		m_textWindowFlag = TRUE;
	
			// Set check box for "disk file".
	
	if (gPrincipalCompSpecsPtr->outputStorageType & 0x0002)
		m_diskFileFlag = TRUE;
	
	if (UpdateData (FALSE))
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item
	
	SelectDialogItemText (this, selectItem, 0, SInt16_MAX);
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"



void CMPrincipalCompDialog::OnSelendokChannelCombo ()

{
	HandleChannelsMenu (IDC_ChannelCombo,
								kNoTransformation,
								(SInt16)gImageWindowInfoPtr->totalNumberChannels,
								1,
								TRUE);
	
}	// end "OnSelendokChannelCombo"



void CMPrincipalCompDialog::OnSelendokClassCombo ()

{                                                                                                      
	HandleClassesMenu (&m_localNumberClasses,
								(SInt16*)m_classListPtr,
								1,      
								(SInt16)gProjectInfoPtr->numberStatisticsClasses, 
								IDC_ClassCombo,
								&m_classSelection);
	
}	// end "OnSelendokClassCombo"

 

void CMPrincipalCompDialog::ShowFieldTypeItems ()

{                                                        
	MShowDialogItem (this, IDC_ClassCombo);
	MShowDialogItem (this, IDC_Training);
	MShowDialogItem (this, IDC_Test);
	
}	// end "ShowFieldTypeItems"

