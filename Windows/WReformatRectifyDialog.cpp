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
//	File:						WReformatRectifyDialog.cpp : implementation file
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
//								CMReformatRectifyDlg class.
//
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h"

#include "WReformatRectifyDialog.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMReformatRectifyDlg, CMDialog)
	//{{AFX_MSG_MAP (CMReformatRectifyDlg)
	ON_BN_CLICKED (IDC_ReprojectToRadio, OnBnClickedReprojectToRadio)
	ON_BN_CLICKED (IDC_TranslateScaleRotateRadio, OnBnClickedTranslateScaleRotateRadio)
	ON_BN_CLICKED (IDC_UseMapOrientationAngle, OnBnClickedUsemaporientationangle)
	ON_BN_CLICKED (IDEntireImage, ToEntireImage)
	ON_BN_CLICKED (IDSelectedImage, ToSelectedImage)

	ON_CBN_SELENDOK (IDC_ReferenceFileList, OnCbnSelendokTargetcombo)
	ON_CBN_SELENDOK (IDC_ResampleMethod, &CMReformatRectifyDlg::OnCbnSelendokResamplemethod)

	ON_EN_CHANGE (IDC_RotationClockwise, OnEnChangeRotationclockwise)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMReformatRectifyDlg::CMReformatRectifyDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMReformatRectifyDlg::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMReformatRectifyDlg)
	m_backgroundValue = 0;
	m_lineShift = 0;
	m_columnShift = 0;
	m_columnScaleFactor = 0.0;
	m_lineScaleFactor = 0.0;
	m_rotationAngle = 0.0;
	m_blankOutsideSelectedAreaFlag = FALSE;
	m_headerListSelection = -1;
	m_channelSelection = -1;
	m_useMapOrientationAngleFlag = FALSE;
	m_procedureCode = 0;
	m_fileNamesSelection = -1;
	m_resampleSelection = 0;
	//}}AFX_DATA_INIT
	
	m_referenceWindowInfoHandle = NULL;
	m_initializedFlag = CMDialog::m_initializedFlag;
	
}	// end "CMReformatRectifyDlg"



void CMReformatRectifyDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMReformatRectifyDlg)
	DDX_Text (pDX, IDC_ColumnEnd, m_ColumnEnd);
	DDV_MinMaxLong (pDX, m_ColumnEnd, 1, m_maxNumberColumns);
	DDX_Text (pDX, IDC_ColumnStart, m_ColumnStart);
	DDV_MinMaxLong (pDX, m_ColumnStart, 1, m_maxNumberColumns);
	DDX_Text (pDX, IDC_LineEnd, m_LineEnd);
	DDV_MinMaxLong (pDX, m_LineEnd, 1, m_maxNumberLines);
	DDX_Text (pDX, IDC_LineStart, m_LineStart);
	DDV_MinMaxLong (pDX, m_LineStart, 1, m_maxNumberLines);
	DDX_Text2 (pDX, IDC_BackgroundValue, m_backgroundValue);
	DDV_MinMaxDouble (pDX, m_backgroundValue, m_minBackgroundValue, m_maxBackgroundValue);
	DDX_Text (pDX, IDC_LineOffset, m_lineShift);
	DDV_MinMaxLong (pDX, m_lineShift, -100, 100);
	DDX_Text (pDX, IDC_ColumnOffset, m_columnShift);
	DDV_MinMaxLong (pDX, m_columnShift, -100, 100);
	DDX_Text2 (pDX, IDC_ColumnScale, m_columnScaleFactor);
	DDX_Text2 (pDX, IDC_LineScale, m_lineScaleFactor);
	DDX_Text2 (pDX, IDC_RotationClockwise, m_rotationAngle);
	DDV_MinMaxDouble (pDX, m_rotationAngle, -180., 180.);
	DDX_Check (pDX, IDC_NonSelectedPixels, m_blankOutsideSelectedAreaFlag);
	DDX_CBIndex (pDX, IDC_Header, m_headerListSelection);
	DDX_CBIndex (pDX, IDC_OutChannels, m_channelSelection);
	DDX_Check (pDX, IDC_UseMapOrientationAngle, m_useMapOrientationAngleFlag);
	DDX_Radio (pDX, IDC_TranslateScaleRotateRadio, m_procedureCode);
	DDX_CBIndex (pDX, IDC_ReferenceFileList, m_fileNamesSelection);
	DDX_CBIndex (pDX, IDC_ResampleMethod, m_resampleSelection);
	//}}AFX_DATA_MAP

			// Verify that the line and column values make sense

	VerifyLineColumnStartEndValues (pDX);

	if (pDX->m_bSaveAndValidate)
		{
		CComboBox*			comboBoxPtr;

		comboBoxPtr = (CComboBox*)GetDlgItem (IDC_Header);
		m_headerOptionsSelection =
							(SInt16)(comboBoxPtr->GetItemData (m_headerListSelection));

		comboBoxPtr = (CComboBox*)GetDlgItem (IDC_ResampleMethod);
		m_resampleMethodCode =  (SInt16)comboBoxPtr->GetItemData (m_resampleSelection);

		}	// end "if (pDX->m_bSaveAndValidate)"
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the reformat rectify
//							image specification dialog box to the user and copy the
//							revised information back to the reformat rectify specification 
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
//	Coded By:			Larry L. Biehl			Date: 04/29/2005
//	Revised By:			Larry L. Biehl			Date: 03/26/2006	

Boolean CMReformatRectifyDlg::DoDialog (
				FileInfoPtr							outFileInfoPtr,
				FileInfoPtr							fileInfoPtr,
				WindowInfoPtr						imageWindowInfoPtr,
				LayerInfoPtr						imageLayerInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr,
				double								minBackgroundValue,
				double								maxBackgroundValue)

{  
	SInt64								returnCode;
	Boolean								continueFlag = FALSE;

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																							return (FALSE);
																																								
	m_outputFileInfoPtr = outFileInfoPtr;
	m_fileInfoPtr = fileInfoPtr;
	m_imageWindowInfoPtr = imageWindowInfoPtr;
	m_imageLayerInfoPtr = imageLayerInfoPtr;
	m_reformatOptionsPtr = reformatOptionsPtr;
	m_minBackgroundValue = minBackgroundValue;
	m_maxBackgroundValue = maxBackgroundValue;

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

		RectifyImageDialogOK (this,
										m_outputFileInfoPtr,
										m_fileInfoPtr,
										m_imageWindowInfoPtr,
										m_imageLayerInfoPtr,
										&m_dialogSelectArea,
										m_reformatOptionsPtr,
										(SInt16)m_headerOptionsSelection,
										(Boolean)m_blankOutsideSelectedAreaFlag,
										(SInt16)m_channelSelection,
										m_backgroundValue,
										(SInt16)(m_procedureCode+1),
										(SInt16)m_resampleMethodCode,
										m_referenceWindowInfoHandle,
										m_lineShift,
										m_columnShift,
										m_lineScaleFactor,
										m_columnScaleFactor,
										m_rotationAngle);

		}	// end "if (returnCode == IDOK)"
			
	return (continueFlag);
		
}	// end "DoDialog"



void CMReformatRectifyDlg::OnBnClickedReprojectToRadio ()

{
	m_procedureCode = kReprojectToReferenceImage - 1;
	UpdateProcedureItems (IDC_LineStart, TRUE);
	
}	// end "OnBnClickedReprojectToRadio"



void CMReformatRectifyDlg::OnBnClickedTranslateScaleRotateRadio ()

{
	m_procedureCode = kTranslateScaleRotate - 1;
	UpdateProcedureItems (IDC_LineOffset, m_blankOutsideSelectedAreaFlag);

}	// end "OnBnClickedTranslateScaleRotateRadio"



void CMReformatRectifyDlg::OnBnClickedUsemaporientationangle ()

{
			// Add your control notification handler code here

	DDX_Check (m_dialogFromPtr,
					IDC_UseMapOrientationAngle,
					m_useMapOrientationAngleFlag);

	m_rotationAngle = 0.;
	if (m_useMapOrientationAngleFlag)
		m_rotationAngle = m_mapOrientationAngle;

	DDX_Text2 (m_dialogToPtr, IDC_RotationClockwise, m_rotationAngle);

}	// end "OnBnClickedUsemaporientationangle"



void CMReformatRectifyDlg::OnCbnSelendokResamplemethod ()

{
	SInt16								savedResampleSelection;


			// Select resampling method popup box

	if (m_resampleSelection >= 0)
		{
		savedResampleSelection = m_resampleSelection;
		DDX_CBIndex (m_dialogFromPtr, IDC_ResampleMethod, m_resampleSelection);
		
		}	// end "if (m_resampleSelection >= 0)"

}	// end "OnCbnSelendokResamplemethod"



void CMReformatRectifyDlg::OnCbnSelendokTargetcombo ()

{
	SInt16								savedFileNamesSelection;


			// Reference image to register against popup box

	if (m_fileNamesSelection >= 0)
		{
		savedFileNamesSelection = m_fileNamesSelection;
		DDX_CBIndex (m_dialogFromPtr, IDC_ReferenceFileList, m_fileNamesSelection);
		
		if (savedFileNamesSelection != m_fileNamesSelection)
			RectifyImageDialogOnReferenceFile (this,
															m_procedureCode+1,
															m_fileNamesSelection+1,
															&m_referenceWindowInfoHandle,
															&m_dialogSelectArea);

		}	// end "if (m_fileNamesSelection >= 0)"

}	// end "OnCbnSelendokTargetcombo"



void CMReformatRectifyDlg::OnEnChangeRotationclockwise ()

{
			// If this is a RICHEDIT control, the control will not
			// send this notification unless you override the CMDialog::OnInitDialog ()
			// function and call CRichEditCtrl ().SetEventMask ()
			// with the ENM_CHANGE flag ORed into the mask.

			// Add your control notification handler code here

	if (m_mapOrientationAngle != 0)
		{
		DDX_Text2 (m_dialogFromPtr, IDC_RotationClockwise, m_rotationAngle);

		if (m_rotationAngle == m_mapOrientationAngle)
			m_useMapOrientationAngleFlag = TRUE;

		else	// m_rotationAngle != m_mapOrientationAngle
			m_useMapOrientationAngleFlag = FALSE;

		DDX_Check (m_dialogToPtr,
						IDC_UseMapOrientationAngle,
						m_useMapOrientationAngleFlag);

		}	// end "if (m_mapOrientationAngle != 0)"

}	// end "OnEnChangeRotationclockwise"



BOOL CMReformatRectifyDlg::OnInitDialog ()

{
	CComboBox*							comboBoxPtr;

	SInt16								channelSelection,
											fileNamesSelection,
											procedureCode,
											resampleMethodCode;

	Boolean								blankOutsideSelectedAreaFlag,
											mapInfoExistsFlag;


	CDialog::OnInitDialog ();
	
			// Add extra initialization here
	
			// Make sure that we have the bitmaps for the entire image buttons.
		
	VERIFY (toEntireButton.AutoLoad (IDEntireImage, this));
	VERIFY (toSelectedButton.AutoLoad (IDSelectedImage, this));
	
	RectifyImageDialogInitialize (this,
											m_fileInfoPtr,
											&m_dialogSelectArea,
											m_reformatOptionsPtr,
											&m_headerOptionsSelection,
											&channelSelection,
											&blankOutsideSelectedAreaFlag,
											&m_backgroundValue,
											&procedureCode,
											&resampleMethodCode,
											&fileNamesSelection,
											&m_referenceWindowInfoHandle,
											&m_lineShift,
											&m_columnShift,
											&m_lineScaleFactor,
											&m_columnScaleFactor,
											&m_rotationAngle,
											&m_mapOrientationAngle);
                  
	m_LineStart = m_reformatOptionsPtr->lineStart;
	m_LineEnd = m_reformatOptionsPtr->lineEnd;
	m_ColumnStart = m_reformatOptionsPtr->columnStart;
	m_ColumnEnd = m_reformatOptionsPtr->columnEnd;

	m_blankOutsideSelectedAreaFlag = blankOutsideSelectedAreaFlag;
	m_procedureCode = procedureCode - 1;
	m_resampleMethodCode = resampleMethodCode;
		
			// Get the resample method list selection that matches the input
			// resample method code.
			                             
	m_resampleSelection = GetComboListSelection (IDC_ResampleMethod,
																	m_resampleMethodCode);
		
	if (m_resampleSelection == -1)
		m_resampleSelection = 0;

	m_fileNamesSelection = fileNamesSelection - 1;

			// Set text indicating whether the output file format could be
			// GeoTIFF or TIFF.

	mapInfoExistsFlag = FindIfMapInformationExists (gImageWindowInfoPtr);
	
	comboBoxPtr = (CComboBox*)(GetDlgItem (IDC_Header));
	comboBoxPtr->DeleteString (kTIFFGeoTIFFMenuItem);
	if (mapInfoExistsFlag)
		comboBoxPtr->InsertString (kTIFFGeoTIFFMenuItem, (LPCTSTR)_T("GeoTIFF format"));

	else	// !mapInfoExistsFlag
		comboBoxPtr->InsertString (kTIFFGeoTIFFMenuItem, (LPCTSTR)_T("TIFF format"));

	comboBoxPtr->SetItemData (0, kNoneMenuItem);
	comboBoxPtr->SetItemData (1, kArcViewMenuItem);
	comboBoxPtr->SetItemData (2, kERDAS74MenuItem);
	comboBoxPtr->SetItemData (3, kGAIAMenuItem);
	comboBoxPtr->SetItemData (4, kTIFFGeoTIFFMenuItem);
	comboBoxPtr->SetItemData (5, kMatlabMenuItem);

			// Remove Matlab and ArcView options.
	
	comboBoxPtr->DeleteString (kMatlabMenuItem);
	comboBoxPtr->DeleteString (kGAIAMenuItem);

	m_headerListSelection = GetComboListSelection (IDC_Header,
																	m_headerOptionsSelection);

	m_channelSelection = channelSelection;

	if (UpdateData (FALSE))
		PositionDialogWindow (); 
	
			// Set default text selection to first edit text item	
		                                                     
	SelectDialogItemText (this, IDC_LineOffset, 0, SInt16_MAX);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}	// end "OnInitDialog"



void CMReformatRectifyDlg::UpdateProcedureItems (
				int									selectItemNumber,
				Boolean								blankOutsideSelectedAreaFlag)

{
	DDX_Radio (m_dialogToPtr, IDC_TranslateScaleRotateRadio, m_procedureCode);

	RectifyImageDialogOnRectifyCode (this, 
												m_procedureCode+1,
												blankOutsideSelectedAreaFlag,
												m_mapOrientationAngle);
	
	RectifyImageDialogOnReferenceFile (this,
													m_procedureCode+1,
													m_fileNamesSelection+1,
													&m_referenceWindowInfoHandle,
													&m_dialogSelectArea);

			// Set default text selection to first edit text item						
			
	SelectDialogItemText (this, selectItemNumber, 0, SInt16_MAX);

}	// end "OnCbnSelendokTargetcombo"
