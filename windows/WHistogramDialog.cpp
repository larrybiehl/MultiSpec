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
//	File:						WHistogramDialog.cpp : implementation file
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
//								CMHistogramSpecsDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SHistogram_class.h"
#include "SImageWindow_class.h" 

#include "WChannelsDialog.h"
#include "WHistogramDialog.h"
#include "WImageView.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMHistogramSpecsDlg, CMDialog)
	//{{AFX_MSG_MAP (CMHistogramSpecsDlg)
	ON_BN_CLICKED (IDC_ComputeOnly, OnComputeOnly)
	ON_BN_CLICKED (IDC_DiskFile, OnDiskFile)
	ON_BN_CLICKED (IDC_EmptyBins, OnEmptyBins)
	ON_BN_CLICKED (IDC_ListHistogram, OnListHistogram)
	ON_BN_CLICKED (IDC_TextWindow, OnTextWindow)
	ON_BN_CLICKED (IDEntireImage, ToEntireImage)
	ON_BN_CLICKED (IDSelectedImage, ToSelectedImage)

	ON_CBN_SELENDOK (IDC_Channels, OnSelendokChannels)
	ON_CBN_SELENDOK (IDC_ColumnLineFormat, OnSelendokColumnLineFormat)
	ON_CBN_SELENDOK (IDC_Method, OnSelendokMethod)

	ON_EN_CHANGE (IDC_ColumnEnd, CheckColumnEnd)
	ON_EN_CHANGE (IDC_ColumnStart, CheckColumnStart)
	ON_EN_CHANGE (IDC_LineEnd, CheckLineEnd)
	ON_EN_CHANGE (IDC_LineStart, CheckLineStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMHistogramSpecsDlg::CMHistogramSpecsDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMHistogramSpecsDlg::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMHistogramSpecsDlg)
	m_computeOnlyMinMaxFlag = TRUE;
	m_includeEmptyBinsFlag = TRUE;
	m_listHistogramValuesFlag = FALSE;
	m_listHistogramSummaryFlag = FALSE;
	m_histogramMethod = 0;
	m_textWindowFlag = TRUE;
	m_diskFileFlag = FALSE;
	m_fileName = "";
	m_columnsLinesFormat = 0;
	m_supportFileName = "";
	//}}AFX_DATA_INIT 
	
	m_histogramMethodCode = kComputeFromImage;
	m_histogramSpecsPtr = NULL;
	m_defaultStatFileChangedFlag = FALSE;
	m_updateListHistogramItemsFlag = FALSE;
	
	m_initializedFlag = CMDialog::m_initializedFlag;  
	
	if (m_initializedFlag)
		{                          
				// Get pointer to memory for temporary storage of channel list. 
			                                                                       
		m_localFeaturesPtr = (UInt16*)MNewPointer (
				(UInt32)gImageWindowInfoPtr->totalNumberChannels * sizeof (UInt16));
	                                                       
		m_initializedFlag = (m_localFeaturesPtr != NULL);
		
		}	// end "if (m_initializedFlag)"
		
}	// end "CMHistogramSpecsDlg" 



CMHistogramSpecsDlg::~CMHistogramSpecsDlg (void)

{  
	m_localFeaturesPtr = CheckAndDisposePtr (m_localFeaturesPtr);         
	
}	// end "~CMHistogramSpecsDlg"      

                               

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CheckWriteResultsToSettings                
//
//	Software purpose:	The purpose of this routine is to verify that an output 
//							destination has been selected. If not, then deselect the 
//							OK button.
//
//	Parameters in:		None	
//
//	Parameters out:	None
//
//	Value Returned:  	None
//
// Called By:			HistogramDialog in SHistogram.cpp
//
//	Coded By:			Larry L. Biehl					Date:	06/28/1995
//	Revised By:			Larry L. Biehl					Date: 06/28/1995

void CMHistogramSpecsDlg::CheckWriteResultsToSettings (void)

{   
	if (m_listHistogramValuesFlag)
		{                                    
				// Verify that an output destination has been selected. If
				// not, then deselect the OK button.
					 
		DDX_Check (m_dialogFromPtr, IDC_TextWindow, m_textWindowFlag);
		DDX_Check (m_dialogFromPtr, IDC_DiskFile, m_diskFileFlag);
		   
	   if (!m_textWindowFlag && !m_diskFileFlag)
			SetDLogControlHilite (NULL, IDOK, 255);
				
		else	// m_textWindowFlag || m_diskFileFlag   
			SetDLogControlHilite (NULL, IDOK, 0);
			
		}	// end "if (m_listHistogramValuesFlag)"
		
	else	// !m_listHistogramValuesFlag
		SetDLogControlHilite (NULL, IDOK, 0);
	
}	// end "CheckWriteResultsToSettings"



void CMHistogramSpecsDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	CComboBox*							comboBoxPtr;


	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMHistogramSpecsDlg)
	DDX_CBIndex (pDX, IDC_Channels, m_channelSelection);
	DDX_Text (pDX, IDC_ColumnEnd, m_ColumnEnd);
	DDV_MinMaxLong (pDX, m_ColumnEnd, 1, m_maxNumberColumns);
	DDX_Text (pDX, IDC_ColumnInterval, m_ColumnInterval);
	DDV_MinMaxLong (pDX, m_ColumnInterval, 1, m_maxNumberColumns);
	DDX_Text (pDX, IDC_ColumnStart, m_ColumnStart);
	DDV_MinMaxLong (pDX, m_ColumnStart, 1, m_maxNumberColumns);
	DDX_Check (pDX, IDC_ComputeOnly, m_computeOnlyMinMaxFlag);
	DDX_Check (pDX, IDC_EmptyBins, m_includeEmptyBinsFlag);
	DDX_Text (pDX, IDC_LineEnd, m_LineEnd);
	DDV_MinMaxLong (pDX, m_LineEnd, 1, m_maxNumberLines);
	DDX_Text (pDX, IDC_LineInterval, m_LineInterval);
	DDV_MinMaxLong (pDX, m_LineInterval, 1, m_maxNumberLines);
	DDX_Text (pDX, IDC_LineStart, m_LineStart);
	DDV_MinMaxLong (pDX, m_LineStart, 1, m_maxNumberLines);
	DDX_Check (pDX, IDC_ListHistogram, m_listHistogramValuesFlag);
	DDX_Check (pDX, IDC_ListSummary, m_listHistogramSummaryFlag);
	DDX_CBIndex (pDX, IDC_Method, m_histogramMethod);
	DDX_Check (pDX, IDC_TextWindow, m_textWindowFlag);
	DDX_Check (pDX, IDC_DiskFile, m_diskFileFlag);
	DDX_Text (pDX, IDC_FileName, m_fileName);
	DDX_CBIndex (pDX, IDC_ColumnLineFormat, m_columnsLinesFormat);
	DDX_Text (pDX, IDC_DefaultFile, m_supportFileName);
	DDV_MaxChars (pDX, m_supportFileName, gFileNameLengthLimit);
	//}}AFX_DATA_MAP
	
	if (pDX->m_bSaveAndValidate)
		{
				// Verify that the line and column values make sense
				
		if (m_LineStart > m_LineEnd)
			{
			DDX_Text (pDX, IDC_LineStart, m_LineStart);
			DDV_MinMaxLong (pDX, m_LineStart, 1, m_LineEnd);
			
			}	// end "m_LineStart > m_LineEnd"
				
		if (m_ColumnStart > m_ColumnEnd)
			{ 
			DDX_Text (pDX, IDC_ColumnStart, m_ColumnStart);
			DDV_MinMaxLong (pDX, m_ColumnStart, 1, m_ColumnEnd);
			
			}	// end "if (m_ColumnStart > m_ColumnEnd)"

		comboBoxPtr = (CComboBox*)GetDlgItem (IDC_Method);
		m_histogramMethodCode = 
							(SInt16)comboBoxPtr->GetItemData (m_histogramMethod);
		
		}	// end "if (pDX->m_bSaveAndValidate)"
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the histogram
//							specification dialog box to the user and copy the
//							revised back to the histogram specification structure if
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
//	Coded By:			Larry L. Biehl			Date: 06/27/1995
//	Revised By:			Larry L. Biehl			Date: 11/23/1999

Boolean CMHistogramSpecsDlg::DoDialog (
				HistogramSpecsPtr					histogramSpecsPtr,
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr)

{  
	INT_PTR								returnCode;

	Boolean								continueFlag = FALSE,
											defaultStatFileChangedFlag = FALSE;
	
	
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
																			
	m_histogramSpecsPtr = histogramSpecsPtr;	
	m_windowInfoPtr = windowInfoPtr;				
	m_fileInfoPtr = fileInfoPtr;			
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{ 	  
				// Histogram area													
				
		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;
																
		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;

		HistogramDialogOK (this, 
									histogramSpecsPtr,
									windowInfoPtr,
									m_histogramMethodCode,
									&m_dialogSelectArea,
									m_localFeaturesPtr, 
									m_localActiveNumberFeatures,
									m_channelSelection,
									m_computeOnlyMinMaxFlag,
									m_listHistogramSummaryFlag,
									m_listHistogramValuesFlag,
									(m_columnsLinesFormat == 1),
									m_includeEmptyBinsFlag,
									m_textWindowFlag,
									m_diskFileFlag,
									m_defaultStatFileChangedFlag,
									m_allChannelsAtOnceFlag);

		continueFlag = TRUE;
		
		}	// end "if (returnCode == IDOK)"
		
	else	// returnCode != IDOK
				// Set computeFlag so that new histogram is not computed 			
		histogramSpecsPtr->computeFlag = FALSE;
		
	return (continueFlag);
		
}	// end "DoDialog"



void CMHistogramSpecsDlg::OnComputeOnly ()

{                                                           
	UpdateListAndEmptyBinsDialogBoxes ();
	
}	// end "OnComputeOnly"



void CMHistogramSpecsDlg::OnDiskFile ()

{                                                                                   
	CheckWriteResultsToSettings ();
	
}	// end "OnDiskFile"



void CMHistogramSpecsDlg::OnEmptyBins ()

{                                       
	DDX_Check (m_dialogFromPtr, IDC_EmptyBins, m_includeEmptyBinsFlag);
	
}	// end "OnEmptyBins"



BOOL CMHistogramSpecsDlg::OnInitDialog ()

{  
	CComboBox* 							comboBoxPtr;
	DialogPtr							dialogPtr = this;
	
	SInt16								channelSelection;
	
	Boolean								histogramInMemoryAvailableFlag,
											lineFormatHistFlag;
	
	
	CMDialog::OnInitDialog ();
	
			// Make sure that we have the bitmaps for the entire image buttons.
		
	VERIFY (toEntireButton.AutoLoad (IDEntireImage, this));
	VERIFY (toSelectedButton.AutoLoad (IDSelectedImage, this));
	
			// Initialize the histogram dialog variables.

	HistogramDialogInitialize (dialogPtr, 
											m_histogramSpecsPtr,
											m_windowInfoPtr,
											m_fileInfoPtr,
											&m_fileName,
											&m_supportFileName,
											&m_histogramMethodCode,
											&histogramInMemoryAvailableFlag,
											&m_dialogSelectArea,
											m_localFeaturesPtr, 
											&m_localNumberFeatures,
											&channelSelection,
											(Boolean*)&m_computeOnlyMinMaxFlag,
											(Boolean*)&m_listHistogramSummaryFlag,
											(Boolean*)&m_listHistogramValuesFlag,
											&lineFormatHistFlag,
											(Boolean*)&m_includeEmptyBinsFlag,
											(Boolean*)&m_textWindowFlag,
											(Boolean*)&m_diskFileFlag,
											&m_defaultStatFileChangedFlag,
											&m_updateListHistogramItemsFlag,
											&m_allChannelsAtOnceFlag);
	
			// Set the method code for the histogram method drop down list items.
											
	m_histogramMethod = m_histogramMethodCode - 1;

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_Method);
	comboBoxPtr->SetItemData (0, kStoredInMemory);
	comboBoxPtr->SetItemData (1, kComputeFromImage);
	comboBoxPtr->SetItemData (2, kReadFromSupportFile);
		
			// Only allow for computation for thematic type images.
		
	if (m_fileInfoPtr->thematicType)
		{ 
		comboBoxPtr->DeleteString (2);
		comboBoxPtr->DeleteString (0);
		
		m_histogramMethod = 0; 
		
		}	// end "if (m_fileInfoPtr->thematicType)"
	
	else	// !fileInfoPtr->thematicType  
		{                         
		if (!histogramInMemoryAvailableFlag)
			{                           
			comboBoxPtr->DeleteString (0);
			m_histogramMethod--; 
			
			}	// end "if (!histogramInMemoryAvailableFlag)"
		
		}	// end "else !fileInfoPtr->thematicType"
				
			// Selected area to histogram.
                  
	m_LineStart = m_dialogSelectArea.lineStart;
	m_LineEnd = m_dialogSelectArea.lineEnd;
	m_LineInterval = m_dialogSelectArea.lineInterval;
	m_ColumnStart = m_dialogSelectArea.columnStart;
	m_ColumnEnd = m_dialogSelectArea.columnEnd;
	m_ColumnInterval = m_dialogSelectArea.columnInterval; 	
	
			// Set feature parameters 
			
	m_localActiveNumberFeatures = m_localNumberFeatures; 
	m_localActiveFeaturesPtr = m_localFeaturesPtr;
	m_channelSelection = channelSelection;
	
	m_columnsLinesFormat = 0;
	if (lineFormatHistFlag)  
		m_columnsLinesFormat = 1;      
	
			// Set default text selection to first edit text item 
		                                       
	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);  			  
	                        
	if (UpdateData (FALSE))
		PositionDialogWindow ();
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog" 



void CMHistogramSpecsDlg::OnListHistogram ()

{                        
	UpdateListAndEmptyBinsDialogBoxes ();
	
}	// end "OnListHistogram"



void CMHistogramSpecsDlg::OnSelendokChannels ()

{  
	Boolean								lineFormatHistFlag;
	
	
	HandleChannelsMenu (IDC_Channels,
								kNoTransformation,
								(SInt16)gImageWindowInfoPtr->totalNumberChannels,
								kImageChannelType,
								TRUE);
	
	m_allChannelsAtOnceFlag = HistogramDialogUpdateAllChannelsAtOnceFlag (
																			this,
																			m_localActiveNumberFeatures, 
																			&lineFormatHistFlag);
	
	m_columnsLinesFormat = 0;
	if (lineFormatHistFlag)
		m_columnsLinesFormat = 1;
	
}	// end "OnSelendokChannels"



void CMHistogramSpecsDlg::OnSelendokColumnLineFormat ()

{                                       
	UpdateListAndEmptyBinsDialogBoxes ();
	
}	// end "OnSelendokColumnLineFormat"



void CMHistogramSpecsDlg::OnSelendokMethod ()

{  
	CComboBox*							comboBoxPtr;

	int									localMethod;
	
	SInt16								previousMethodCode;
	 

	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_Method);
	
	previousMethodCode = (SInt16)comboBoxPtr->GetItemData (m_histogramMethod);
	
	DDX_CBIndex (m_dialogFromPtr, IDC_Method, localMethod);
	m_histogramMethodCode = (SInt16)comboBoxPtr->GetItemData (localMethod);
					
	m_histogramMethodCode = HistogramDialogHandleMethod (
																this,
																m_histogramSpecsPtr,
																m_windowInfoPtr,
																m_fileInfoPtr,
																&m_supportFileName,
																m_histogramMethodCode,
																IDOK,
																previousMethodCode,
																&m_updateListHistogramItemsFlag,
																&m_defaultStatFileChangedFlag);

	if (m_histogramMethodCode == kComputeFromImage)
		SetEntireImageButtons (NULL,
										m_LineStart, 
										m_LineEnd, 
										m_ColumnStart, 
										m_ColumnEnd);

			// Set the proper method item.

	if (comboBoxPtr->GetItemData (0) == kStoredInMemory)
		m_histogramMethod = m_histogramMethodCode - 1;

	else	// !comboBoxPtr->GetItemData (0) == kStoredInMemory
		m_histogramMethod = m_histogramMethodCode - 2;

	DDX_CBIndex (m_dialogToPtr, IDC_Method, m_histogramMethod);
											
	if (m_updateListHistogramItemsFlag)
		{
		HistogramDialogSetListAndEmptyBins (this, 
															m_histogramMethodCode,
															m_computeOnlyMinMaxFlag, 
															m_listHistogramValuesFlag, 
															m_includeEmptyBinsFlag,
															(m_columnsLinesFormat==1));
		
		}	// end "if (m_updateListHistogramItemsFlag)"
		
}	// end "OnSelendokMethod"



void CMHistogramSpecsDlg::OnTextWindow ()

{                                                          
	CheckWriteResultsToSettings ();
	
}	// end "OnTextWindow"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ShowAreaHistogramItems
//
//	Software purpose:	The purpose of this routine is to show area selected
//							dialog items.
//
//	Parameters in:		None	
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/28/1995
//	Revised By:			Larry L. Biehl			Date: 11/23/1999

void CMHistogramSpecsDlg::ShowAreaHistogramItems (void)

{
	HistogramDialogShowAreaItems (this);

	SetEntireImageButtons (NULL,
									m_LineStart, 
									m_LineEnd, 
									m_ColumnStart, 
									m_ColumnEnd);
	
}	// end "ShowAreaHistogramItems" 

                               

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateListAndEmptyBinsDialogBoxes                
//
//	Software purpose:	The purpose of this routine is to update the list and
//							empty bin dialog check boxes in the histogram
//							dialog window. This routine is called after a dialog
//							item has been changed
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:  	
//
// Called By:			HistogramDialog in histogram.c
//
//	Coded By:			Larry L. Biehl					Date:	06/28/1995
//	Revised By:			Larry L. Biehl					Date: 06/28/1995

void CMHistogramSpecsDlg::UpdateListAndEmptyBinsDialogBoxes (void)

{                                        
		  	// Get current settings for:
		  	//		compute only min, max, mean, etc
		  	//		list histogram values
		  	//		include empty bins
		  	//		columns or lines format
		  	                                           
	DDX_Check (m_dialogFromPtr, IDC_ComputeOnly, m_computeOnlyMinMaxFlag);
	
	DDX_Check (m_dialogFromPtr, IDC_ListHistogram, m_listHistogramValuesFlag);
	
	DDX_CBIndex (m_dialogFromPtr, IDC_ColumnLineFormat, m_columnsLinesFormat);
					
	HistogramDialogSetListAndEmptyBins (this,
													m_histogramMethodCode,
													m_computeOnlyMinMaxFlag, 
													m_listHistogramValuesFlag, 
													m_includeEmptyBinsFlag,
													(m_columnsLinesFormat == 1));
	                           										
	CheckWriteResultsToSettings ();
	
}	// end "UpdateListAndEmptyBinsDialogBoxes"
