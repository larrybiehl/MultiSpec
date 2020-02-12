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
//	File:						WListResultsDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/04/2018
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMListResultsDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WListResultsDialog.h"
#include "WMultiSpec.h"

extern ListResultsSpecsPtr						gListResultsSpecsPtr;

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMListResultsDialog, CMDialog)
	//{{AFX_MSG_MAP (CMListResultsDialog)
	ON_BN_CLICKED (IDC_Area, OnAreaUnits)
	ON_BN_CLICKED (IDC_Class, OnClassSummary)
	ON_BN_CLICKED (IDC_Field, OnFieldSummary)
	ON_BN_CLICKED (IDC_Group, OnGroupSummary)
	ON_BN_CLICKED (IDC_Image, OnImage)
	ON_BN_CLICKED (IDC_Percent, OnPercentUnits)
	ON_BN_CLICKED (IDC_Samples, OnSampleUnits)
	ON_BN_CLICKED (IDC_Test, OnTest)
	ON_BN_CLICKED (IDC_ThresholdResults, OnThresholdResults)
	ON_BN_CLICKED (IDC_Training, OnTraining)
	ON_BN_CLICKED (IDEntireImage, ToEntireImage)
	ON_BN_CLICKED (IDSelectedImage, ToSelectedImage)

	ON_CBN_SELENDOK (IDC_ClassCombo, OnSelendokClassCombo)

	ON_EN_CHANGE (IDC_ColumnEnd, CheckColumnEnd)
	ON_EN_CHANGE (IDC_ColumnStart, CheckColumnStart)
	ON_EN_CHANGE (IDC_LineEnd, CheckLineEnd)
	ON_EN_CHANGE (IDC_LineStart, CheckLineStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMListResultsDialog::CMListResultsDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog	(CMListResultsDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMListResultsDialog)
	m_trainingFlag = FALSE;
	m_testFlag = FALSE;
	m_imageFlag = FALSE;
	m_fieldSummaryFlag = FALSE;
	m_classSummaryFlag = FALSE;
	m_groupSummaryFlag = FALSE;
	m_conversionFactor = 0;
	m_thresholdResultsFlag = FALSE;
	m_thresholdPercent = 0;
	m_textWindowFlag = FALSE;
	m_diskFileFlag = FALSE;
	m_tableType = -1;
	m_tableUnits = -1;
	//}}AFX_DATA_INIT
			
	m_projectWindowFlag = (gProjectInfoPtr != NULL);
	
	m_initializedFlag = CMDialog::m_initializedFlag;  
	
	if (m_initializedFlag)
		{
				// Get memory for the local class list vector.	
		
		m_classListPtr = NULL;
		if (m_projectWindowFlag)
			{		
			m_classListPtr = (UInt16*)MNewPointer (
						(UInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));
	                                                       
			m_initializedFlag = (m_classListPtr != NULL);
			
			}	// end "if (m_projectWindowFlag)"
		
		}	// end "if (m_initializedFlag)"
	
}	// end "CMListResultsDialog"  



CMListResultsDialog::~CMListResultsDialog (void)

{                                                              
	m_classListPtr = CheckAndDisposePtr (m_classListPtr);
	
}	// end "~CMListResultsDialog"



void CMListResultsDialog::CheckListAreaItems ()

{                                          
	m_localAreaCode = 0;
	
	if (m_trainingFlag)
		m_localAreaCode += 1;
	
	if (m_testFlag)
		m_localAreaCode += 2;
	
	if (m_imageFlag)
		m_localAreaCode += 4;
	
	SetSummarizeByItems ();
	
}	// end "CheckListAreaItems" 



void CMListResultsDialog::CheckSummaryItems ()

{                                          
	m_localSummaryCode = 0;
	
	if (m_fieldSummaryFlag)
		m_localSummaryCode += kFieldSummary;
	
	if (m_classSummaryFlag)
		m_localSummaryCode += kClassSummary;
	
	if (m_groupSummaryFlag)
		m_localSummaryCode += kGroupSummary;
	
	SetSummarizeByItems ();
	
}	// end "CheckSummaryItems"



void CMListResultsDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMListResultsDialog)
	DDX_Check (pDX, IDC_Training, m_trainingFlag);
	DDX_Check (pDX, IDC_Test, m_testFlag);
	DDX_Check (pDX, IDC_Image, m_imageFlag);
	DDX_Check (pDX, IDC_Field, m_fieldSummaryFlag);
	DDX_Check (pDX, IDC_Class, m_classSummaryFlag);
	DDX_Check (pDX, IDC_Group, m_groupSummaryFlag);
	DDX_Text2 (pDX, IDC_ConversionFactor, m_conversionFactor);
	DDV_MinMaxDouble (pDX, m_conversionFactor, 0., 1000000000000000.);
	DDX_Check (pDX, IDC_ThresholdResults, m_thresholdResultsFlag);
	DDX_Text2 (pDX, IDC_Probability, m_thresholdPercent);
	DDV_MinMaxDouble (pDX, m_thresholdPercent, 0., kMaxThreshold);
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
	DDX_CBIndex (pDX, IDC_ClassCombo, m_classSelection);
	DDX_Radio (pDX, IDC_ClassTable, m_tableType);
	DDX_Radio (pDX, IDC_Samples, m_tableUnits);
	//}}AFX_DATA_MAP

			// Verify that the line and column values make sense
	
	if (m_imageFlag)
		VerifyLineColumnStartEndValues (pDX);

}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean DoDialog
//
//	Software purpose:	The purpose of this routine is to present the List
//							Results specification dialog box to the user and copy the
//							revised info to the list results specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/20/1997
//	Revised By:			Larry L. Biehl			Date: 05/20/1997

Boolean CMListResultsDialog::DoDialog (void)

{                                 
	Boolean								continueFlag = FALSE;
	
	INT_PTR								returnCode;
								
	
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{ 						                    
		continueFlag = TRUE; 
					
				// Class areas																
                          
      if (m_groupSummaryFlag)   
			m_classSelection = 0;
			   
		gListResultsSpecsPtr->classAreaSet = m_classSelection;
		
		if (m_projectWindowFlag)
			{ 	                               
			SInt16* classAreaPtr = (SInt16*)GetHandlePointer (
															gListResultsSpecsPtr->classAreaHandle);
														
			if (m_classSelection == kAllMenuItem)
				LoadClassAreaVector (&gListResultsSpecsPtr->numberClassAreas,
											classAreaPtr);
											
			else	// m_classSelection == kSubsetMenuItem
				{
				UInt32		index;
				                                              
				gListResultsSpecsPtr->numberClassAreas = m_localNumberClasses;
				for (index=0; index<m_localNumberClasses; index++)
					classAreaPtr[index] = m_classListPtr[index]; 
			
				}	// end "else m_classSelection == kSubsetMenuItem"
				
			}	// end "if (m_projectWindowFlag)"
					
				// Areas to use to list results.										
				
		gListResultsSpecsPtr->areaCode = 0;
		if (m_trainingFlag)
			gListResultsSpecsPtr->areaCode += kTrainingType;
					
		if (m_testFlag)
			gListResultsSpecsPtr->areaCode += kTestingType;
					
		if (m_imageFlag)
			gListResultsSpecsPtr->areaCode += kAreaType;
				
				// Selected list results area.										
				
		gListResultsSpecsPtr->lineStart = m_LineStart;
		gListResultsSpecsPtr->lineEnd = m_LineEnd;
		gListResultsSpecsPtr->lineInterval = m_LineInterval;
		gListResultsSpecsPtr->columnStart = m_ColumnStart;
		gListResultsSpecsPtr->columnEnd = m_ColumnEnd;
		gListResultsSpecsPtr->columnInterval = m_ColumnInterval;
					
				// Summarize by.															
				
		if (m_nonClusterFieldTypeCode)
			{
			gListResultsSpecsPtr->summaryCode = 0;
			if (m_fieldSummaryFlag)
				gListResultsSpecsPtr->summaryCode += kFieldSummary;
						
			if (m_classSummaryFlag)
				gListResultsSpecsPtr->summaryCode += kClassSummary;
						
			if (m_groupSummaryFlag)
				gListResultsSpecsPtr->summaryCode += kGroupSummary;
						
			}	// end "if (nonClusterFieldTypeCode)" 
					
				// Table type.																
					
		gListResultsSpecsPtr->tableType = 0;
		if (m_tableType == 0)
			gListResultsSpecsPtr->tableType = 1;
					
		else if (m_tableType == 1)
			gListResultsSpecsPtr->tableType = 2;
					
				// Table units.															
				
		gListResultsSpecsPtr->unitsCode = 0;
		if (m_tableUnits == 0)
			gListResultsSpecsPtr->unitsCode += kSampleUnits;
					
		if (m_tableUnits == 1)
			gListResultsSpecsPtr->unitsCode += kPercentUnits;
					
		if (m_tableUnits == 2)
			gListResultsSpecsPtr->unitsCode += kAreaUnits;

		if (gListResultsSpecsPtr->unitsCode == kAreaUnits)
			gListResultsSpecsPtr->summaryUnitsCode = kAreaUnits;

		else	// ...->unitsCode != kAreaUnits
			{
			if (GetDefaultAreaConversionFactor (
												gImageFileInfoPtr->mapProjectionHandle,
												gImageWindowInfoPtr->windowInfoHandle) == 0)
				gListResultsSpecsPtr->summaryUnitsCode = kPercentUnits;
													
			}	// end "else ...->unitsCode != kAreaUnits"
	
				// Area conversion factor.												
		
		if (m_conversionFactor > 0)
			gListResultsSpecsPtr->areaConversionFactor = m_conversionFactor;
						
				// Threshold results flag.												
						
		gListResultsSpecsPtr->thresholdFlag = m_thresholdResultsFlag;
				
		if (gListResultsSpecsPtr->thresholdFlag)
			{
					// Get probability threshold.										
						
			gListResultsSpecsPtr->probabilityThreshold = m_thresholdPercent;
					
					// Get the image data value which corresponds to the given
					// threshold value.
					
			Handle fileInfoHandle = GetFileInfoHandle (
									gListResultsSpecsPtr->probabilityWindowInfoHandle);

			gListResultsSpecsPtr->probabilityThresholdCode =
						GetThresholdCode (m_thresholdPercent,
												fileInfoHandle,
												gListResultsSpecsPtr->thresholdTypeCode);
															
			}	// end "if (gListResultsSpecsPtr->thresholdFlag)" 
					
				// Write list results to output window.							
				// Write list results to disk file.									
						
		gListResultsSpecsPtr->outputStorageType = 0;
									
		if (m_textWindowFlag)				
			gListResultsSpecsPtr->outputStorageType += 1;
												
		if (m_diskFileFlag)	
			gListResultsSpecsPtr->outputStorageType += 2;
					
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"


/*
void CMListResultsDialog::HideShowProbabilityItems (
				Boolean								thresholdResultsFlag)
				
{  
	if (thresholdResultsFlag)
		{                                          
		MShowDialogItem (this, IDC_ProbabilityPrompt);
		MShowDialogItem (this, IDC_Probability);
	
		}	// end "if (thresholdResultsFlag)"
	
	else	// !thresholdResultsFlag
		{	                                        
		MHideDialogItem (this, IDC_ProbabilityPrompt);
		MHideDialogItem (this, IDC_Probability);
		
		}	// end "else !thresholdResultsFlag"
	
}	// end "HideShowProbabilityItems"
*/


void CMListResultsDialog::HideShowUnitsItems (
				Boolean								areaUnitsFlag)
				
{  
	if (areaUnitsFlag)
		{                                        
		MShowDialogItem (this, IDC_ConversionText);
		MShowDialogItem (this, IDC_ConversionFactor);
	
		}	// end "if (areaUnitsFlag)"
	
	else	// !areaUnitsFlag
		{	                                      
		MHideDialogItem (this, IDC_ConversionText);
		MHideDialogItem (this, IDC_ConversionFactor);
		
		}	// end "else !areaUnitsFlag"
	
}	// end "HideShowUnitsItems"



void CMListResultsDialog::OnAreaUnits ()

{                                                                                      
	HideShowUnitsItems (TRUE);
	
}	// end "OnAreaUnits"



void CMListResultsDialog::OnClassSummary ()

{                                                                                    
	DDX_Check (m_dialogFromPtr, IDC_Class, m_classSummaryFlag);
	
	CheckSummaryItems ();
	
}	// end "OnClassSummary"

 

void CMListResultsDialog::OnFieldSummary ()

{                                                                               
	DDX_Check (m_dialogFromPtr, IDC_Field, m_fieldSummaryFlag);
	
	CheckSummaryItems ();
	
}	// end "OnFieldSummary"



void CMListResultsDialog::OnGroupSummary ()

{                                                                                    
	DDX_Check (m_dialogFromPtr, IDC_Group, m_groupSummaryFlag);
	
	CheckSummaryItems ();
	
}	// end "OnGroupSummary"



void CMListResultsDialog::OnImage ()

{                                                                         
	DDX_Check (m_dialogFromPtr, IDC_Image, m_imageFlag);
	
	CheckListAreaItems ();
	
	HideShowAreaItems (m_imageFlag);
	
	if (m_imageFlag)
		{                 
		SetEntireImageButtons (NULL,
										m_LineStart, 
										m_LineEnd, 
										m_ColumnStart, 
										m_ColumnEnd); 
			
				// Set default text selection to first edit text item	
		                                       
		SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);
		
		}	// end "if (m_imageFlag)"
	
}	// end "OnImage"
 


BOOL CMListResultsDialog::OnInitDialog ()

{
	CDialog::OnInitDialog ();
																	
	m_nonClusterFieldTypeCode = 0;
	
	if (m_projectWindowFlag)
		m_nonClusterFieldTypeCode = DetermineFieldTypes ();
		
	m_nonClusterFieldTypeCode &= 0x0003; 
	
			// Make sure that we have the bitmaps for the entire image buttons.
		
	VERIFY (toEntireButton.AutoLoad (IDEntireImage, this));
	VERIFY (toSelectedButton.AutoLoad (IDSelectedImage, this));
	
			// List areas to be used - training, test, and/or image selection.
			
	m_localAreaCode = gListResultsSpecsPtr->areaCode;
	 
	if (m_projectWindowFlag)
		{		
			// List results for training areas. Unhilite this control if there 	
			// are no training areas for the project. 									
			
		if (!(m_nonClusterFieldTypeCode & kTrainingType))
			SetDLogControlHilite (this, IDC_Training, 255);
		
		else	// m_nonClusterFieldTypeCode & kTrainingType 
			m_trainingFlag = (m_localAreaCode & kTrainingType);
	
				// List results for test areas. Unhilite this control if there 	
				// are no test areas for the project.										
			
		if (!(m_nonClusterFieldTypeCode & kTestingType))
			SetDLogControlHilite (this, IDC_Test, 255);
			
		else	// m_nonClusterFieldTypeCode & kTestingType
			m_testFlag = (m_localAreaCode & kTestingType);
		
		if (!m_trainingFlag && !m_testFlag)
			MHideDialogItem (this, IDC_ClassCombo);
					
		}	// end "if (m_projectWindowFlag)" 
	
	else	// !m_projectWindowFlag
		{                                  
		MHideDialogItem (this, IDC_Training);
		MHideDialogItem (this, IDC_Test);
		MHideDialogItem (this, IDC_ClassCombo);
		
		}	// end "else !m_projectWindowFlag" 
	
			// Class areas to use.		
	                               
	m_classSelection = gListResultsSpecsPtr->classAreaSet;
	m_localNumberClasses = gListResultsSpecsPtr->numberClassAreas;
	
	if (m_projectWindowFlag)
		{
		UInt32		index;
										 
		UInt16* classPtr = (UInt16*)GetHandlePointer (
															gListResultsSpecsPtr->classAreaHandle);
									
		for (index=0; index<m_localNumberClasses; index++)
			m_classListPtr[index] = classPtr[index]; 
			
		}	// end "if (m_projectWindowFlag)"
			
			// If the train/test areas are to be summarized by groups then all	
			// classes will need to be used.													
									
	if (gListResultsSpecsPtr->summaryCode & kGroupSummary)
		MHideDialogItem (this, IDC_ClassCombo);
	
			// List results for selected area.												
			                                
	m_imageFlag = (m_localAreaCode & kAreaType) || (m_nonClusterFieldTypeCode == 0);
                                 															
			//	Selected area for list results
			// 	Initialize selected area structure.		
	
	InitializeDialogSelectArea (&m_dialogSelectArea,
											gImageWindowInfoPtr,
											gActiveImageWindow,
											gListResultsSpecsPtr->columnStart,
											gListResultsSpecsPtr->columnEnd,
											gListResultsSpecsPtr->columnInterval,
											gListResultsSpecsPtr->lineStart,
											gListResultsSpecsPtr->lineEnd,
											gListResultsSpecsPtr->lineInterval,
											12,
											10,
											kAdjustToBaseImage);
                  
	m_LineStart = gListResultsSpecsPtr->lineStart;
	m_LineEnd = gListResultsSpecsPtr->lineEnd;
	m_LineInterval = gListResultsSpecsPtr->lineInterval;
	m_ColumnStart = gListResultsSpecsPtr->columnStart;
	m_ColumnEnd = gListResultsSpecsPtr->columnEnd;
	m_ColumnInterval = gListResultsSpecsPtr->columnInterval;
	
	HideShowAreaItems (m_imageFlag);	
	
	if (m_imageFlag)                     
		SetEntireImageButtons (NULL,
										m_LineStart, 
										m_LineEnd, 
										m_ColumnStart, 
										m_ColumnEnd);
	
			// Summarize by field.
			// Summarize by class. 																
			// Summarize by group.
			
	m_localSummaryCode = gListResultsSpecsPtr->summaryCode; 																
	
	if (m_projectWindowFlag)
		{      
				// Verify that the group summary option is not available if there
				// are not groups defined.
				
		if (gImageFileInfoPtr->numberGroups == 0)
			m_localSummaryCode &= (kFieldSummary + kClassSummary);
			
		if (m_nonClusterFieldTypeCode && 
										(m_localAreaCode & (kTrainingType+kTestingType)))
			{
			m_fieldSummaryFlag = ((m_localSummaryCode & kFieldSummary) != 0);
			m_classSummaryFlag = ((m_localSummaryCode & kClassSummary) != 0);
			m_groupSummaryFlag = ((m_localSummaryCode & kGroupSummary) != 0);
							
			}	// end "if (nonClusterFieldTypeCode && ...)" 
		
		if (gImageFileInfoPtr->numberGroups == 0)
			SetDLogControlHilite (this, IDC_Group, 255);
			
		if (m_localAreaCode == kAreaType || m_nonClusterFieldTypeCode == 0)
			{                                         
			SetDLogControlHilite (this, IDC_Field, 255);
			SetDLogControlHilite (this, IDC_Class, 255);
			SetDLogControlHilite (this, IDC_Group, 255);
			
			}	// end "if (m_localAreaCode == ...)" 
		
		}	// end "if (projectWindowFlag)" 
		
	else	// !projectWindowFlag 
		{                                    
		MHideDialogItem (this, IDC_SummarizeOutline);
		MHideDialogItem (this, IDC_Field);
		MHideDialogItem (this, IDC_Class);
		MHideDialogItem (this, IDC_Group);
		
		}	// end "else !projectWindowFlag"  
	
			// Class type table. 																
			// Group type table. 																
	
	if (gListResultsSpecsPtr->tableType == kClassTable)
		m_tableType = 0;
		
	else if (gListResultsSpecsPtr->tableType == kGroupTable)	
		m_tableType = 1;
				
	if (gImageFileInfoPtr->numberGroups == 0)     
		SetDLogControlHilite (this, IDC_GroupTable, 255); 
	
			// Sample units. 																		
			// Percent units. 																	
			// Area units. 																		
			            
	if (gListResultsSpecsPtr->unitsCode & kSampleUnits)
		m_tableUnits = 0;
		
	else if (gListResultsSpecsPtr->unitsCode & kPercentUnits)	
		m_tableUnits = 1; 
		
	else if (gListResultsSpecsPtr->unitsCode & kAreaUnits)	
		m_tableUnits = 2;
				
	HideShowUnitsItems (m_tableUnits == 2); 
	
			// Area conversion factor.															
		
	m_saveAreaConversionFactor = gListResultsSpecsPtr->areaConversionFactor;
	m_conversionFactor = m_saveAreaConversionFactor;
	
			// Threshold the results.															
			// Threshold percent.																
		
	m_thresholdResultsFlag = gListResultsSpecsPtr->thresholdFlag;
	if (gListResultsSpecsPtr->probabilityWindowInfoHandle == NULL)
		SetDLogControlHilite (this, IDC_ThresholdResults, 255);
		
	m_saveThresholdPercent = gListResultsSpecsPtr->probabilityThreshold;
	m_thresholdPercent = m_saveThresholdPercent;

	ListResultsDialogSetThresholdItems (this, 
													m_thresholdResultsFlag,
													gListResultsSpecsPtr->thresholdTypeCode);

			// List results to output text window.											
			// List results to disk file.														
			
	m_textWindowFlag = (gListResultsSpecsPtr->outputStorageType & 0x0001);
	m_diskFileFlag = (gListResultsSpecsPtr->outputStorageType & 0x0002);               
	                 
	if (UpdateData (FALSE))
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item	
		                                       
	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);
	
	return FALSE;  // return TRUE  unless you set the focus to a control 
	
}	// end "OnInitDialog"



void CMListResultsDialog::OnPercentUnits ()

{                                                                                      
	HideShowUnitsItems (FALSE);
	
}	// end "OnPercentUnits"



void CMListResultsDialog::OnSampleUnits ()

{                                                          
	HideShowUnitsItems (FALSE);
	
}	// end "OnSampleUnits"



void CMListResultsDialog::OnSelendokClassCombo ()

{                                                                                                                     
	HandleClassesMenu (&m_localNumberClasses,
								(SInt16*)m_classListPtr,
								1,      
								(SInt16)gProjectInfoPtr->numberStatisticsClasses, 
								IDC_ClassCombo,
								&m_classSelection);
	
}	// end "OnSelendokClassCombo"



void CMListResultsDialog::OnTest ()

{                                                                               
	DDX_Check (m_dialogFromPtr, IDC_Test, m_testFlag);
	
	CheckListAreaItems ();
	
}	// end "OnTest"

 

void CMListResultsDialog::OnThresholdResults ()

{                                                           
	DDX_Check (m_dialogFromPtr, IDC_ThresholdResults, m_thresholdResultsFlag);

	ListResultsDialogSetThresholdItems (this, 
													m_thresholdResultsFlag,
													gListResultsSpecsPtr->thresholdTypeCode);
	                       
	//HideShowProbabilityItems (m_thresholdResultsFlag);
	
}	// end "OnThresholdResults" 



void CMListResultsDialog::OnTraining ()

{                                                                               
	DDX_Check (m_dialogFromPtr, IDC_Training, m_trainingFlag);
	
	CheckListAreaItems ();
	
}	// end "OnTraining"



void CMListResultsDialog::SetSummarizeByItems ()

{  
	Boolean								tempFlag = TRUE;
	
	   		
	if (m_projectWindowFlag)
		{		
		tempFlag = (m_localSummaryCode != 0);
					
		if (m_localAreaCode == kAreaType || m_localAreaCode == 0)
			{                                   
			m_fieldSummaryFlag = FALSE;
			m_classSummaryFlag = FALSE;
			m_groupSummaryFlag = FALSE;
			                               
			DDX_Check (m_dialogToPtr, IDC_Field, m_fieldSummaryFlag);
			DDX_Check (m_dialogToPtr, IDC_Class, m_classSummaryFlag);
			DDX_Check (m_dialogToPtr, IDC_Group, m_groupSummaryFlag);
			                                          
			SetDLogControlHilite (this, IDC_Field, 255); 
			SetDLogControlHilite (this, IDC_Class, 255);
			SetDLogControlHilite (this, IDC_Group, 255);
						                    
			MHideDialogItem (this, IDC_ClassCombo);
						
			}	// end "if (m_localAreaCode == kAreaType || ...)" 
						
		else	// localAreaCode != kAreaType && ... 
			{                     
			m_fieldSummaryFlag = ((m_localSummaryCode & kFieldSummary) != 0);
			m_classSummaryFlag = ((m_localSummaryCode & kClassSummary) != 0);
			m_groupSummaryFlag = ((m_localSummaryCode & kGroupSummary) != 0);
			                               
			DDX_Check (m_dialogToPtr, IDC_Field, m_fieldSummaryFlag);
			DDX_Check (m_dialogToPtr, IDC_Class, m_classSummaryFlag);
			DDX_Check (m_dialogToPtr, IDC_Group, m_groupSummaryFlag);
			                                         
			SetDLogControlHilite (this, IDC_Field, 0); 
			SetDLogControlHilite (this, IDC_Class, 0);
		
			if (gImageFileInfoPtr->numberGroups > 0)   
				SetDLogControlHilite (this, IDC_Group, 0);
						
			if (m_groupSummaryFlag)                    
				MHideDialogItem (this, IDC_ClassCombo);
				
			else	// !m_groupSummaryFlag
				MShowDialogItem (this, IDC_ClassCombo);
						
			}	// end "else m_localAreaCode != kAreaType && ..." 
						
		}	// end "if (m_projectWindowFlag)" 
														       
	GetDlgItem (IDOK)->EnableWindow ((m_localAreaCode != 0) && tempFlag);
	
}	// end "SetSummarizeByItems"
