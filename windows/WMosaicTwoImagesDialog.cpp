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
//	File:						WMosaicTwoImagesDialog.cpp : implementation file
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
//								CMMosaicTwoImagesDialog class.
//
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h"

#include "WMosaicTwoImagesDialog.h"



BEGIN_MESSAGE_MAP (CMMosaicTwoImagesDialog, CDialog)
	ON_BN_CLICKED (IDC_IgnoreBackgroundValue, &CMMosaicTwoImagesDialog::OnBnClickedIgnoreBackgroundValue)
	ON_BN_CLICKED (IDEntireImage, ToEntireImage)
	ON_BN_CLICKED (IDSelectedImage, ToSelectedImage)

	ON_CBN_SELCHANGE (IDC_BottomImageFileList, &CMMosaicTwoImagesDialog::OnCbnSelchangeBottomimagefilelist)
	ON_CBN_SELCHANGE (IDC_HeaderFormatList, &CMMosaicTwoImagesDialog::OnCbnSelchangeHeaderformatlist)
	ON_CBN_SELCHANGE (IDC_MosaicDirection, &CMMosaicTwoImagesDialog::OnCbnSelchangeMosaicdirection)
	ON_CBN_SELCHANGE (IDC_RightImageFileList, &CMMosaicTwoImagesDialog::OnCbnSelchangeRightimagefilelist)

	ON_EN_CHANGE (IDC_ColumnEnd, &CMMosaicTwoImagesDialog::OnEnChangeLeftTopColumnEnd)
	ON_EN_CHANGE (IDC_ColumnStart, &CMMosaicTwoImagesDialog::OnEnChangeLeftTopColumnStart)
	ON_EN_CHANGE (IDC_ColumnStart3, &CMMosaicTwoImagesDialog::OnEnChangeBottomColumnStart3)
	ON_EN_CHANGE (IDC_LineEnd, &CMMosaicTwoImagesDialog::OnEnChangeLeftTopLineEnd)
	ON_EN_CHANGE (IDC_LineStart, &CMMosaicTwoImagesDialog::OnEnChangeLeftTopLineStart)
	ON_EN_CHANGE (IDC_LineStart2, &CMMosaicTwoImagesDialog::OnEnChangeRightLineStart2)
END_MESSAGE_MAP ()



CMMosaicTwoImagesDialog::CMMosaicTwoImagesDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMMosaicTwoImagesDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMMosaicTwoImagesDialog)
	m_mosaicDirectionCode = -1;
	m_rightFileNameSelection = -1;
	m_bottomFileNameSelection = -1;
	m_ignoreBackgroundValueFlag = FALSE;
	m_backgroundValue = 0;
	m_writeChannelDescriptionsFlag = FALSE;
	m_headerListSelection = -1;
	//}}AFX_DATA_INIT

	m_initializedFlag = CMDialog::m_initializedFlag;
	
}	// end "CMMosaicTwoImagesDialog"



CMMosaicTwoImagesDialog::~CMMosaicTwoImagesDialog ()

{

}	// end "~CMMosaicTwoImagesDialog"



void CMMosaicTwoImagesDialog::CheckBottomImageColumns (
				UInt16								itemSelected)

{
	if ((m_dialogSelectArea.columnEnd -
										m_dialogSelectArea.columnStart) !=
		  			(m_rightBottomDialogSelectArea.columnEnd -
												m_rightBottomDialogSelectArea.columnStart))
		MosaicTwoImagesDialogUpdateLineColumnEnds (this,
																	&m_dialogSelectArea,
																	&m_rightBottomDialogSelectArea,
																	m_mosaicDirectionCode+1,
																	m_rightBottomWindowInfoHandle,
																	itemSelected);

}	// end "CheckBottomImageColumns"



void CMMosaicTwoImagesDialog::CheckRightImageLines (
				UInt16								itemSelected)

{
	if ((m_dialogSelectArea.lineEnd - m_dialogSelectArea.lineStart) !=
			  						(m_rightBottomDialogSelectArea.lineEnd -
														m_rightBottomDialogSelectArea.lineStart))
		MosaicTwoImagesDialogUpdateLineColumnEnds (this,
																	&m_dialogSelectArea,
																	&m_rightBottomDialogSelectArea,
																	m_mosaicDirectionCode+1,
																	m_rightBottomWindowInfoHandle,
																	itemSelected);

}	// end "CheckRightImageLines"



void CMMosaicTwoImagesDialog::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	DDX_CBIndex (pDX, IDC_MosaicDirection, m_mosaicDirectionCode);
	DDX_Check (pDX, IDC_IgnoreBackgroundValue, m_ignoreBackgroundValueFlag);
	DDX_Text (pDX, IDC_BackgroundValue, m_backgroundValue);
	DDX_Check (pDX, IDC_WriteChannelDescriptions, m_writeChannelDescriptionsFlag);
	DDX_CBIndex (pDX, IDC_HeaderFormatList, m_headerListSelection);
	DDX_CBIndex (pDX, IDC_RightImageFileList, m_rightFileNameSelection);
	DDX_CBIndex (pDX, IDC_BottomImageFileList, m_bottomFileNameSelection);
	DDX_Text (pDX, IDC_LineEnd, m_LineEnd);
	DDV_MinMaxLong (pDX, m_LineEnd, 1, m_maxNumberLines);
	DDX_Text (pDX, IDC_LineStart, m_LineStart);
	DDV_MinMaxLong (pDX, m_LineStart, 1, m_maxNumberLines);
	DDX_Text (pDX, IDC_ColumnEnd, m_ColumnEnd);
	DDV_MinMaxLong (pDX, m_ColumnEnd, 1, m_maxNumberColumns);
	DDX_Text (pDX, IDC_ColumnStart, m_ColumnStart);
	DDV_MinMaxLong (pDX, m_ColumnStart, 1, m_maxNumberColumns);

	VerifyLineColumnStartEndValues (pDX);

	if (pDX->m_bSaveAndValidate)
		{
		CComboBox*					comboBoxPtr;

		comboBoxPtr = (CComboBox*)GetDlgItem (IDC_HeaderFormatList);
		m_headerOptionsSelection =
								(SInt16)(comboBoxPtr->GetItemData (m_headerListSelection));

		}	// end "if (pDX->m_bSaveAndValidate)"

	if (pDX->m_bSaveAndValidate)
		{
		SInt16				itemHit;


				// Verify that the number of lines for right/left or number
				// of columns for top/bottom are the same. Present an alert
				// if they are not.
				
		itemHit = MosaicTwoImagesDialogVerifyLineColumnSettings (
																		&m_dialogSelectArea,
																		&m_rightBottomDialogSelectArea,
																		m_mosaicDirectionCode+1);

		if (itemHit == 0)
			pDX->Fail ();

		}	// end "if (pDX->m_bSaveAndValidate)"

}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the reformat mosaic
//							two images specification dialog box to the user and copy the
//							revised information back to the reformat mosiac specification 
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
//	Coded By:			Larry L. Biehl			Date: 03/17/2014
//	Revised By:			Larry L. Biehl			Date: 03/19/2014	

Boolean CMMosaicTwoImagesDialog::DoDialog (
				FileInfoPtr							fileInfoPtr,
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr)

{  
	INT_PTR								returnCode;
	Boolean								continueFlag = FALSE;

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																					return (FALSE);
																																								
	m_outputFileInfoPtr = outFileInfoPtr;
	m_fileInfoPtr = fileInfoPtr;
	m_reformatOptionsPtr = reformatOptionsPtr;

			// Selected area for output file.
				
	m_dialogSelectArea.lineStart = m_LineStart;
	m_dialogSelectArea.lineEnd = m_LineEnd;
	m_dialogSelectArea.lineInterval = m_LineInterval;
															
	m_dialogSelectArea.columnStart = m_ColumnStart;
	m_dialogSelectArea.columnEnd = m_ColumnEnd;
	m_dialogSelectArea.columnInterval = m_ColumnInterval;
	
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		continueFlag = TRUE; 						
				
		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = 1;
		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = 1; 

		MosaicTwoImagesDialogOK (m_reformatOptionsPtr,
											m_outputFileInfoPtr,
											m_mosaicDirectionCode+1,
											m_rightBottomWindowInfoHandle,
											&m_dialogSelectArea,
											&m_rightBottomDialogSelectArea,
											m_ignoreBackgroundValueFlag,
											m_backgroundValue,
											m_writeChannelDescriptionsFlag,
											(SInt16)m_headerOptionsSelection);

		}	// end "if (returnCode == IDOK)"
			
	return (continueFlag);
		
}	// end "DoDialog"



void CMMosaicTwoImagesDialog::OnBnClickedIgnoreBackgroundValue ()

{
	DDX_Check (m_dialogFromPtr, IDC_IgnoreBackgroundValue, m_ignoreBackgroundValueFlag);
	MosaicTwoImagesDialogUpdateBackGroundValuesItem (this,
																		m_ignoreBackgroundValueFlag);

}	// end "OnBnClickedIgnoreBackgroundValue"



void CMMosaicTwoImagesDialog::OnCbnSelchangeBottomimagefilelist ()

{
	SInt16								savedFileNameSelection;


	savedFileNameSelection = m_bottomFileNameSelection;
	DDX_CBIndex (m_dialogFromPtr, IDC_BottomImageFileList, m_bottomFileNameSelection);
													
	if (m_bottomFileNameSelection != savedFileNameSelection)
		{
		MosaicTwoImagesDialogOnSelectRightBottomImage (this,
																		&m_dialogSelectArea,
																		&m_rightBottomDialogSelectArea,
																		m_rightFileNameSelection+1,
																		&m_rightBottomWindowInfoHandle,
																		m_mosaicDirectionCode+1);

				// Keep the right file list in sync with the bottom file list.

		m_rightFileNameSelection = m_bottomFileNameSelection;
		DDX_CBIndex (m_dialogToPtr, IDC_RightImageFileList, m_rightFileNameSelection);

		}	// end "if (m_bottomFileNameSelection != savedFileNameSelection)"

}	// end "OnCbnSelchangeBottomimagefilelist"



void CMMosaicTwoImagesDialog::OnCbnSelchangeHeaderformatlist ()

{
	SInt16								savedHeaderListSelection;


	savedHeaderListSelection = m_headerListSelection;
	DDX_CBIndex (m_dialogFromPtr, IDC_HeaderFormatList, m_headerListSelection);
													
	if (m_headerListSelection != savedHeaderListSelection)
		{		
		CComboBox*					comboBoxPtr;

		comboBoxPtr = (CComboBox*)GetDlgItem (IDC_HeaderFormatList);
		m_headerOptionsSelection =
								(SInt16)(comboBoxPtr->GetItemData (m_headerListSelection));

		MosaicTwoImagesDialogUpdateChannelDescriptionItem (
								this,
								GetHeaderFormatFromPopUpSelection (m_headerOptionsSelection),
								m_outputFileInfoPtr->thematicType);
			
		}	// end "if (m_headerListSelection != savedHeaderListSelection)"

}	// end "OnCbnSelchangeHeaderformatlist"



void CMMosaicTwoImagesDialog::OnCbnSelchangeMosaicdirection ()

{
	SInt16								savedMosaicDirectionCode;


			// Add your control notification handler code here

	savedMosaicDirectionCode = m_mosaicDirectionCode;
	DDX_CBIndex (m_dialogFromPtr, IDC_MosaicDirection, m_mosaicDirectionCode);

	if (m_mosaicDirectionCode != savedMosaicDirectionCode)
		{	
		MosaicTwoImagesDialogUpdateDirectionItems (this,
																	&m_dialogSelectArea,
																	&m_rightBottomDialogSelectArea,
																	m_rightBottomWindowInfoHandle,
																	m_mosaicDirectionCode + 1,
																	m_ignoreBackgroundValueFlag);
										
		}	// end "if (m_mosaicDirectionCode != savedMosaicDirectionCode)"

}	// end "OnCbnSelchangeMosaicdirection"



void CMMosaicTwoImagesDialog::OnCbnSelchangeRightimagefilelist ()

{
	SInt16								savedFileNameSelection;


	savedFileNameSelection = m_rightFileNameSelection;
	DDX_CBIndex (m_dialogFromPtr, IDC_RightImageFileList, m_rightFileNameSelection);
																										
	if (m_rightFileNameSelection != savedFileNameSelection)
		{
		MosaicTwoImagesDialogOnSelectRightBottomImage (this,
																		&m_dialogSelectArea,
																		&m_rightBottomDialogSelectArea,
																		m_rightFileNameSelection+1,
																		&m_rightBottomWindowInfoHandle,
																		m_mosaicDirectionCode+1);

				// Keep the bottom file list in sync with the right file list.

		m_bottomFileNameSelection = m_rightFileNameSelection;
		DDX_CBIndex (m_dialogToPtr, IDC_BottomImageFileList, m_bottomFileNameSelection);

		}	// end "if (m_rightFileNameSelection != savedFileNameSelection)"

}	// end "OnCbnSelchangeRightimagefilelist"



void CMMosaicTwoImagesDialog::OnEnChangeBottomColumnStart3 ()

{
	SInt32								columnStart3;
	

	columnStart3 = GetDialogItemValue (IDC_ColumnStart3);

	if (columnStart3 > 0)
		{
		m_rightBottomDialogSelectArea.columnStart = columnStart3;

		MosaicTwoImagesDialogUpdateLineColumnEnds (this,
																	&m_dialogSelectArea,
																	&m_rightBottomDialogSelectArea,
																	m_mosaicDirectionCode+1,
																	m_rightBottomWindowInfoHandle,
																	IDC_ColumnStart3);

		}	// end "if (columnStart3 > 0)"

}	// end "OnEnChangeBottomColumnStart3"



void CMMosaicTwoImagesDialog::OnEnChangeLeftTopColumnEnd ()

{
	CheckColumnEnd ();
	m_dialogSelectArea.columnEnd = m_ColumnEnd;
	
	if (m_mosaicDirectionCode == 1)
		CheckBottomImageColumns (IDC_ColumnEnd);

}	// end "OnEnChangeLeftTopColumnEnd"



void CMMosaicTwoImagesDialog::OnEnChangeLeftTopColumnStart ()

{
	CheckColumnStart ();
	m_dialogSelectArea.columnStart = m_ColumnStart;
	
	if (m_mosaicDirectionCode == 1)
		CheckBottomImageColumns (IDC_ColumnStart);

}	// end "OnEnChangeLeftTopColumnStart"



void CMMosaicTwoImagesDialog::OnEnChangeLeftTopLineEnd ()

{
	CheckLineEnd ();
	m_dialogSelectArea.lineEnd = m_LineEnd;
	
	if (m_mosaicDirectionCode == 0)
		CheckRightImageLines (IDC_LineEnd);

}	// end "OnEnChangeLeftTopLineEnd"



void CMMosaicTwoImagesDialog::OnEnChangeLeftTopLineStart ()

{
	CheckLineStart ();
	m_dialogSelectArea.lineStart = m_LineStart;
	
	if (m_mosaicDirectionCode == 0)
		CheckRightImageLines (IDC_LineStart);

}	// end "OnEnChangeLeftTopLineStart"



void CMMosaicTwoImagesDialog::OnEnChangeRightLineStart2 ()

{	
	SInt32								lineStart2;
	

	lineStart2 = GetDialogItemValue (IDC_LineStart2);

	if (lineStart2 > 0)
		{
		m_rightBottomDialogSelectArea.lineStart = lineStart2;

		MosaicTwoImagesDialogUpdateLineColumnEnds (this,
																	&m_dialogSelectArea,
																	&m_rightBottomDialogSelectArea,
																	m_mosaicDirectionCode+1,
																	m_rightBottomWindowInfoHandle,
																	IDC_LineStart2);

		}	// end "if (lineStart2 > 0)"

}	// end "OnEnChangeRightLineStart2"



BOOL CMMosaicTwoImagesDialog::OnInitDialog ()

{
	SInt16								fileNamesSelection,
											mosaicDirectionCode;

	Boolean								ignoreBackgroundValueFlag;


	CDialog::OnInitDialog ();
	
			// Make sure that we have the bitmaps for the entire image buttons.
	
	VERIFY (toEntireButton.AutoLoad (IDEntireImage, this));
	VERIFY (toSelectedButton.AutoLoad (IDSelectedImage, this));
	
	MosaicTwoImagesDialogInitialize (this,
												m_reformatOptionsPtr,
												m_fileInfoPtr,
												m_outputFileInfoPtr,
												&mosaicDirectionCode,
												&m_dialogSelectArea,
												&m_rightBottomDialogSelectArea,
												&fileNamesSelection,
												&m_rightBottomWindowInfoHandle,
												&ignoreBackgroundValueFlag,
												&m_maxDataValue,
												&m_backgroundValue,
												&m_headerOptionsSelection);

	m_LineStart = m_reformatOptionsPtr->lineStart;
	m_LineEnd = m_reformatOptionsPtr->lineEnd;
	m_ColumnStart = m_reformatOptionsPtr->columnStart;
	m_ColumnEnd = m_reformatOptionsPtr->columnEnd;

	m_ignoreBackgroundValueFlag = ignoreBackgroundValueFlag;
	m_mosaicDirectionCode = mosaicDirectionCode - 1;
	
	m_rightFileNameSelection = fileNamesSelection - 1;
	m_bottomFileNameSelection = fileNamesSelection - 1;

	m_headerListSelection = GetComboListSelection (IDC_HeaderFormatList,
																	m_headerOptionsSelection);

	if (UpdateData (FALSE))
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item
	
	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	
}	// end "OnInitDialog"
