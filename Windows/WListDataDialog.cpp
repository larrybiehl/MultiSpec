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
//	File:						WListDataDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			08/08/2019
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMListDataDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WMultiSpec.h"
#include "WListDataDialog.h"

extern ListDataSpecsPtr						gListDataSpecsPtr;

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMListDataDialog, CMDialog)
	//{{AFX_MSG_MAP (CMListDataDialog)
	ON_BN_CLICKED (IDC_Area, OnArea)
	ON_BN_CLICKED (IDC_Classes, OnClasses)
	ON_BN_CLICKED (IDC_GraphData, OnGraphData)
	ON_BN_CLICKED (IDEntireImage, ToEntireImage)
	ON_BN_CLICKED (IDSelectedImage, ToSelectedImage)

	ON_CBN_SELENDOK (IDC_ChannelCombo, OnSelendokChannelCombo)
	ON_CBN_SELENDOK (IDC_ClassCombo, OnSelendokClassCombo)
	ON_CBN_SELENDOK (IDC_ListChannelsFormatCombo, OnCbnSelendokListchannelsformatcombo)

	ON_EN_CHANGE(IDC_ColumnEnd, CheckColumnEnd)
	ON_EN_CHANGE(IDC_ColumnStart, CheckColumnStart)
	ON_EN_CHANGE(IDC_LineEnd, CheckLineEnd)
	ON_EN_CHANGE(IDC_LineStart, CheckLineStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMListDataDialog::CMListDataDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMListDataDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT(CMListDataDialog)
	m_areaFlag = FALSE;
	m_classFlag = FALSE;
	m_diskFileFlag = FALSE;
	m_graphDataFlag = FALSE;
	m_includeClassFieldFlag = FALSE;
	m_includeLineColumnFlag = FALSE;
	m_includeLatLongFlag = FALSE;
	m_textWindowFlag = FALSE;
	m_trainingFlag = FALSE;
	m_testFlag = FALSE;
	m_listDataFormatCode = -1;
	m_numberDecimalPlaces = 2;
	//}}AFX_DATA_INIT
	
	m_initializedFlag = CMDialog::m_initializedFlag;  
	
	if (m_initializedFlag)
		{                          
				// Get pointer to memory for temporary storage of channel list. 
			                                                                       
		m_localFeaturesPtr = (UInt16*)MNewPointer (
					(UInt32)gImageWindowInfoPtr->totalNumberChannels * sizeof (UInt16));
	                                                       
		m_initializedFlag = (m_localFeaturesPtr != NULL);
		
		}	// end "if (m_initializedFlag)" 
	
	if (m_initializedFlag && gListDataSpecsPtr->projectFlag)
		{
				// Get memory for the local class list vector.	
				
		m_classListPtr = (UInt16*)MNewPointer (
				(UInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));
	                                                       
		m_initializedFlag = (m_classListPtr != NULL);
		
		}	// end "if (m_initializedFlag && ...->projectFlag)" 

	m_latLongPossibleFlag = FALSE;
	
}	// end "CMListDataDialog"  



CMListDataDialog::~CMListDataDialog (void)

{                                                              
	m_classListPtr = CheckAndDisposePtr (m_classListPtr);
	m_localFeaturesPtr = CheckAndDisposePtr (m_localFeaturesPtr); 
	
}	// end "~CMListDataDialog"



void CMListDataDialog::CheckClassItems (
				Boolean								listClassDataFlag)

{
	if (listClassDataFlag)
		{
		MShowDialogItem (this, IDC_ClassCombo);
		MShowDialogItem (this, IDC_IncludeClassField);
		MShowDialogItem (this, IDC_Training);
		MShowDialogItem (this, IDC_Test);
		
		}	// end "if (listClassDataFlag)"
	
	else    // !listClassDataFlag
		{
		MHideDialogItem (this, IDC_ClassCombo);
		MHideDialogItem (this, IDC_IncludeClassField);
		MHideDialogItem (this, IDC_Training);
		MHideDialogItem (this, IDC_Test);
		
		}	// end "else !listClassDataFlag"
	
}	// end "CheckClassItems"



void CMListDataDialog::CheckOKButton (void)

{
	if (m_areaFlag || m_classFlag)
		SetDLogControlHilite (this, IDOK, 0);
	
	else    // !m_areaFlag && !m_classFlag
		SetDLogControlHilite (this, IDOK, 255);
	
}	// end "CheckOKButton"



void CMListDataDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMListDataDialog)
	DDX_Check (pDX, IDC_Area, m_areaFlag);
	DDX_Check (pDX, IDC_Classes, m_classFlag);
	DDX_Check (pDX, IDC_DiskFile, m_diskFileFlag);
	DDX_Check (pDX, IDC_GraphData, m_graphDataFlag);
	DDX_Check (pDX, IDC_IncludeClassField, m_includeClassFieldFlag);
	DDX_Check (pDX, IDC_IncludeLineColumn, m_includeLineColumnFlag);
	DDX_Check (pDX, IDC_IncludeLatitudeLongitude, m_includeLatLongFlag);
	DDX_Check (pDX, IDC_TextWindow, m_textWindowFlag);
	DDX_Check (pDX, IDC_Training, m_trainingFlag);
	DDX_Check (pDX, IDC_Test, m_testFlag);
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
	DDX_CBIndex (pDX, IDC_ListChannelsFormatCombo, m_listDataFormatCode);
	DDX_Text (pDX, IDC_NumberDecimalPlaces, m_numberDecimalPlaces);
	DDV_MinMaxLong (pDX, m_numberDecimalPlaces, 0, 9);
	//}}AFX_DATA_MAP

			// Verify that the line and column values make sense
		
	VerifyLineColumnStartEndValues (pDX);

}	// end "DoDataExchange"



//-----------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the list
//							data specification dialog box to the user and copy the
//							revised back to the list data specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			ListDataDialog in SLstData.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/28/97
//	Revised By:			Larry L. Biehl			Date: 05/28/97	

SInt16 CMListDataDialog::DoDialog (void)

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

		ListDataDialogOK (gListDataSpecsPtr,
								m_classFlag,
								m_classSelection,
								m_localNumberClasses,
								m_classListPtr,
								m_areaFlag,
								&dialogSelectArea,
								m_channelSelection,
								m_localActiveNumberFeatures,
								m_localFeaturesPtr,
								m_includeLineColumnFlag,
								m_includeLatLongFlag,
								m_includeClassFieldFlag,
								m_textWindowFlag,
								m_diskFileFlag,
								m_trainingFlag,
								m_testFlag,
								m_graphDataFlag,
								(UInt16)m_numberDecimalPlaces,
								m_listDataFormatCode + 1);
									
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



void CMListDataDialog::OnArea ()

{
	UInt16								selectItem;
	
	
	DDX_Check (m_dialogFromPtr, IDC_Area, m_areaFlag);
	
	if (m_areaFlag)
		{
		ShowSomeAreaSelectionItems ();
		
		selectItem = IDC_LineStart;
		
		}	// end "if (m_areaFlag)"
	
	else    // !m_areaFlag
		{
		HideSomeAreaSelectionItems ();
		
		selectItem = IDC_LineInterval;
		
		}	// end "else !m_areaFlag"
	
	SelectDialogItemText (this, selectItem, 0, SInt16_MAX);
	
	CheckOKButton ();
	
}	// end "OnArea"



void CMListDataDialog::OnCbnSelendokListchannelsformatcombo ()

{
	DDX_CBIndex (m_dialogFromPtr, IDC_ListChannelsFormatCombo, m_listDataFormatCode);
	
	if (m_listDataFormatCode == 0 && m_latLongPossibleFlag)
		MShowDialogItem (this, IDC_IncludeLatitudeLongitude);
	
	else    // m_listDataFormatCode == 1 || ...
		MHideDialogItem (this, IDC_IncludeLatitudeLongitude);
	
}	// end "OnCbnSelendokListchannelsformatcombo"



void CMListDataDialog::OnClasses ()

{
	DDX_Check (m_dialogFromPtr, IDC_Classes, m_classFlag);
	
	CheckClassItems (m_classFlag);
	
	CheckOKButton ();
	
}	// end "OnClasses"



void CMListDataDialog::OnGraphData ()

{
	DDX_Check (m_dialogFromPtr, IDC_GraphData, m_localGraphDataFlag);
	
}	// end "OnGraphData"



BOOL CMListDataDialog::OnInitDialog ()

{   
	UInt32								index;
	
	UInt16								selectItem;
	
	
	CDialog::OnInitDialog ();
	
			// Make sure that we have the bitmaps for the entire image buttons.
		
	VERIFY (toEntireButton.AutoLoad (IDEntireImage, this));
	VERIFY (toSelectedButton.AutoLoad (IDSelectedImage, this)); 
		
			// Update the modal dialog with the default settings						
			// Set check box for "Classes".												
	
	m_classFlag = gListDataSpecsPtr->listClassesDataFlag;
				
	if (!gListDataSpecsPtr->projectFlag)
		MHideDialogItem (this, IDC_Classes);    
	
	m_classSelection = gListDataSpecsPtr->classSet;
	m_localNumberClasses = gListDataSpecsPtr->numberClasses;         
	
	UInt16* classPtr = (UInt16*)GetHandlePointer (gListDataSpecsPtr->classHandle);
	
	if (gListDataSpecsPtr->projectFlag)
		{								 
		for (index=0; index<m_localNumberClasses; index++)
			m_classListPtr[index] = classPtr[index]; 
			
		}	// end "if (gListDataSpecsPtr->projectFlag)" 
	                                                    	
	CheckClassItems (gListDataSpecsPtr->listClassesDataFlag); 
                                 															
			//	Selected area for list data
			// 	Initialize selected area structure.		
	
	InitializeDialogSelectArea (&m_dialogSelectArea,
											gImageWindowInfoPtr,
											gActiveImageWindow,
											gListDataSpecsPtr->columnStart,
											gListDataSpecsPtr->columnEnd,
											gListDataSpecsPtr->columnInterval,
											gListDataSpecsPtr->lineStart,
											gListDataSpecsPtr->lineEnd,
											gListDataSpecsPtr->lineInterval,
											13,
											11,
											kDontAdjustToBaseImage);	
                  
	m_LineStart = gListDataSpecsPtr->lineStart;
	m_LineEnd = gListDataSpecsPtr->lineEnd;
	m_LineInterval = gListDataSpecsPtr->lineInterval;
	m_ColumnStart = gListDataSpecsPtr->columnStart;
	m_ColumnEnd = gListDataSpecsPtr->columnEnd;
	m_ColumnInterval = gListDataSpecsPtr->columnInterval;  

			// Set check box for area.															
			                              
	m_areaFlag = gListDataSpecsPtr->listSelectedAreaDataFlag;
	if (gListDataSpecsPtr->listSelectedAreaDataFlag)
		{                                
		ShowSomeAreaSelectionItems ();
		
		selectItem = IDC_LineStart; 
		
		}	// end "if (gListDataSpecsPtr->listSelectedAreaDataFlag)" 
					
	else	// !gListDataSpecsPtr->listSelectedAreaDataFlag 
		{                                   
		HideSomeAreaSelectionItems ();
		
		selectItem = IDC_LineInterval;  
		
		}	// end "else !gListDataSpecsPtr->listSelectedAreaDataFlag" 
	                                                                   			
			//	Set the All/Subset channels list item													
			                                                              
	m_channelSelection = gListDataSpecsPtr->channelSet;
	m_localNumberFeatures = gListDataSpecsPtr->numberChannels;
	
	SInt16* channelsPtr = (SInt16*)GetHandlePointer (gListDataSpecsPtr->featureHandle);
	for (index=0; index<m_localNumberFeatures; index++)
		m_localFeaturesPtr[index] = channelsPtr[index];
	
			// Set feature parameters 
			
	m_localActiveNumberFeatures =	m_localNumberFeatures; 
	m_localActiveFeaturesPtr = m_localFeaturesPtr; 
	
	if (gImageFileInfoPtr->thematicType)
		{                                   
		MHideDialogItem (this, IDC_ChannelPrompt);
		MHideDialogItem (this, IDC_ChannelCombo);
		
		}	// end "if (gImageFileInfoPtr->thematicType)"
		
	else	// !gImageFileInfoPtr->thematicType
		{                                     
		MShowDialogItem (this, IDC_ChannelPrompt);
		MShowDialogItem (this, IDC_ChannelCombo);
		
		}	// end "else !gImageFileInfoPtr->thematicType"
	
			// Set check box for "Include line and column values".		
	
	m_includeLineColumnFlag = gListDataSpecsPtr->includeLineColumnValuesFlag;

			// Set check box for "Include latitude and longitude values".	
			
	if (DetermineIfLatLongPossible (gListDataSpecsPtr->windowInfoHandle))
		{
		m_latLongPossibleFlag = TRUE;
		m_includeLatLongFlag = gListDataSpecsPtr->includeLatLongValuesFlag;
		ShowDialogItem (this, IDC_IncludeLatitudeLongitude);
		
		}	// end "if (DetermineIfLatLongPossible (...->windowInfoHandle))"
		
	else    // !DetermineIfLatLongPossible (...->windowInfoHandle)
		{
		m_latLongPossibleFlag = FALSE;
		HideDialogItem (this, IDC_IncludeLatitudeLongitude);
		
		}	// end "else !DetermineIfLatLongPossible (...->windowInfoHandle))"
	
			// Set check box for "Inlude class and field codes".			
			                                                          
	m_includeClassFieldFlag = gListDataSpecsPtr->includeClassFieldFlag;      
	
			// Set check box for "text output window".						
	
	if (gListDataSpecsPtr->outputStorageType & 0x0001)
		m_textWindowFlag = TRUE;
	
			// Set check box for "disk file".									
	
	if (gListDataSpecsPtr->outputStorageType & 0x0002)
		m_diskFileFlag = TRUE; 
	
			// Set control for including training fields.								
			
	if (gListDataSpecsPtr->fieldTypeCode & kTrainingType)
		m_trainingFlag = TRUE;
	
			// Set control for including test fields.										
			
	if (gListDataSpecsPtr->fieldTypeCode & kTestingType)
		m_testFlag = TRUE;
		
	if (gProjectInfoPtr != NULL && gProjectInfoPtr->numberStatTestFields <= 0)
		{                                         
		SetDLogControlHilite (this, IDC_Test, 255);
		m_testFlag = FALSE;
		
		}	// end "if (gProjectInfoPtr != NULL && ..."
	
			// Set check box for "Graph data values".	
			
	m_localGraphDataFlag = gListDataSpecsPtr->graphDataFlag;
	if (gNumberOfGWindows < kMaxNumberGWindows &&
									m_channelSelection == kAllMenuItem)
		{
		m_graphDataFlag = gListDataSpecsPtr->graphDataFlag;    

		}	// if (gNumberOfGWindows < kMaxNumberGWindows)
		
	else	// gNumberOfGWindows >= kMaxNumberGWindows                                          
		SetDLogControlHilite (this, IDC_GraphData, 255); 
		
	m_listDataFormatCode = gListDataSpecsPtr->outputFormatCode - 1;	 

	m_numberDecimalPlaces = gListDataSpecsPtr->numberFDecimalDigits;
	if (gImageFileInfoPtr->dataTypeCode == kIntegerType)
		{
		HideDialogItem (this, IDC_NumberDecimalPlacesPrompt);
		HideDialogItem (this, IDC_NumberDecimalPlaces);

		}	// end "if (gImageFileInfoPtr->dataTypeCode == kIntegerType)"

	else    // gImageFileInfoPtr->dataTypeCode == kRealType
		{
		ShowDialogItem (this, IDC_NumberDecimalPlacesPrompt);
		ShowDialogItem (this, IDC_NumberDecimalPlaces);

		}	// end "if (gImageFileInfoPtr->dataTypeCode == kIntegerType)"
	                 
	if (UpdateData (FALSE))                   
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item	
		                                       
	SelectDialogItemText (this, selectItem, 0, SInt16_MAX);
	
	return FALSE;  // return TRUE  unless you set the focus to a control  
	
}	// end "OnInitDialog"



void CMListDataDialog::OnSelendokChannelCombo ()

{                                                                                                                      
	HandleChannelsMenu (IDC_ChannelCombo, 
								kNoTransformation,
								(SInt16)gImageWindowInfoPtr->totalNumberChannels,
								1,
								TRUE);

	DDX_CBIndex (m_dialogFromPtr, IDC_ChannelCombo, m_channelSelection); 
	
	if (gNumberOfGWindows < kMaxNumberGWindows &&
													m_channelSelection == kAllMenuItem)
		{
		DDX_Check (m_dialogToPtr, IDC_GraphData, m_localGraphDataFlag);
		SetDLogControlHilite (this, IDC_GraphData, 0);
						
		}	// end "if (m_channelSelection == kAllMenuItem)"
					
	else	// m_channelSelection == kSubsetMenuItem
		{
		BOOL localGraphDataFlag = FALSE;
		DDX_Check (m_dialogToPtr, IDC_GraphData, localGraphDataFlag);
		SetDLogControlHilite (this, IDC_GraphData, 255);
						
		}	// end "else m_channelSelection == kSubsetMenuItem"

}	// end "OnSelendokChannelCombo" 



void CMListDataDialog::OnSelendokClassCombo ()

{                                                                                                                    
	HandleClassesMenu (&m_localNumberClasses,
								(SInt16*)m_classListPtr,
								1,      
								(SInt16)gProjectInfoPtr->numberStatisticsClasses, 
								IDC_ClassCombo,
								&m_classSelection);
	
}	// end "OnSelendokClassCombo"
