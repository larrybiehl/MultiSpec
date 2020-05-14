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
//	File:						WReformatChangeDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			07/27/2018
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMChangeFormatDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SImageWindow_class.h"

#include	"WChannelsDialog.h"
#include "WImageView.h"
#include "WReformatChangeDialog.h"
#include	"WReformatTransformDialog.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif   

ReformatOptionsPtr	CMChangeFormatDlg::s_reformatOptionsPtr = NULL;



BEGIN_MESSAGE_MAP (CMChangeFormatDlg, CMDialog)
	//{{AFX_MSG_MAP (CMChangeFormatDlg)
	ON_BN_CLICKED (IDC_OutputInWavelengthOrder, OnOutputInWavelengthOrder)
	ON_BN_CLICKED (IDC_TransformData, OnTransformData)
	ON_BN_CLICKED (IDC_WriteChanDescriptions, OnWriteChanDescriptions)
	ON_BN_CLICKED (IDEntireImage, ToEntireImage)
	ON_BN_CLICKED (IDSelectedImage, ToSelectedImage)

	ON_CBN_SELENDOK (IDC_BandInterleave, OnSelendokBandInterleave)
	ON_CBN_SELENDOK (IDC_ChangeHeader, OnSelendokHeader)
	ON_CBN_SELENDOK (IDC_DataValueType, OnSelendokDataValueType)
	ON_CBN_SELENDOK (IDC_ChannelCombo, OnSelendokOutChannels)

	ON_EN_CHANGE (IDC_ColumnEnd, CheckColumnEnd)
	ON_EN_CHANGE (IDC_ColumnStart, CheckColumnStart)
	ON_EN_CHANGE (IDC_LineEnd, CheckLineEnd)
	ON_EN_CHANGE (IDC_LineStart, CheckLineStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMChangeFormatDlg::CMChangeFormatDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMChangeFormatDlg::IDD, pParent)

{
	//{{AFX_DATA_INIT(CMChangeFormatDlg)
	m_swapBytesFlag = FALSE;
	m_transformDataFlag = FALSE;
	m_channelSelection = -1;
	m_headerListSelection = -1;
	m_headerOptionsSelection = -1;
	m_dataValueListSelection = -1;
	m_outputFileSelection = -1;
	m_bandInterleaveSelection = -1;
	m_invertBottomToTopFlag = FALSE;
	m_writeChanDescriptionFlag = FALSE;
	m_invertLeftToRightFlag = FALSE;
	m_channelThematicDisplayFlag = FALSE;
	m_outputInWavelengthOrderFlag = FALSE;
	m_sessionUserSetDataValueTypeSelectionFlag = FALSE;
	//}}AFX_DATA_INIT                                          
	
	m_dataValueTypeSelectionFlag = FALSE;
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
}	// end "CMChangeFormatDlg"



void CMChangeFormatDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMChangeFormatDlg)
	DDX_Text (pDX, IDC_ColumnEnd, m_ColumnEnd);
	DDV_MinMaxLong (pDX, m_ColumnEnd, 1, m_maxNumberColumns);
	DDX_Text (pDX, IDC_ColumnInterval, m_ColumnInterval);
	DDV_MinMaxLong (pDX, m_ColumnInterval, 1, m_maxNumberColumns);
	DDX_Text (pDX, IDC_ColumnStart, m_ColumnStart);
	DDV_MinMaxLong (pDX, m_ColumnStart, 1, m_maxNumberColumns);
	DDX_Text (pDX, IDC_LineEnd, m_LineEnd);
	DDV_MinMaxLong (pDX, m_LineEnd, 1, m_maxNumberLines);
	DDX_Text (pDX, IDC_LineInterval, m_LineInterval);
	DDV_MinMaxLong (pDX, m_LineInterval, 1, m_maxNumberLines);
	DDX_Text (pDX, IDC_LineStart, m_LineStart);
	DDV_MinMaxLong (pDX, m_LineStart, 1, m_maxNumberLines);
	DDX_Check (pDX, IDC_SwapBytes, m_swapBytesFlag);
	DDX_Check (pDX, IDC_TransformData, m_transformDataFlag);
	DDX_CBIndex (pDX, IDC_ChannelCombo, m_channelSelection);
	DDX_CBIndex (pDX, IDC_ChangeHeader, m_headerListSelection);
	DDX_CBIndex (pDX, IDC_DataValueType, m_dataValueListSelection);
	DDX_CBIndex (pDX, IDC_OutputFileSelection, m_outputFileSelection);
	DDX_CBIndex (pDX, IDC_BandInterleave, m_bandInterleaveSelection);
	DDX_Check (pDX, IDC_InvertBottomToTop, m_invertBottomToTopFlag);
	DDX_Check (pDX, IDC_WriteChanDescriptions, m_writeChanDescriptionFlag);
	DDX_Check (pDX, IDC_OutputInWavelengthOrder, m_outputInWavelengthOrderFlag);
	DDX_Check (pDX, IDC_InvertRightToLeft, m_invertLeftToRightFlag);
	//}}AFX_DATA_MAP

			// Verify that the line and column values make sense

	VerifyLineColumnStartEndValues (pDX);

	if (pDX->m_bSaveAndValidate)
		{
		CComboBox* comboBoxPtr = (CComboBox*)GetDlgItem (IDC_ChangeHeader);
		m_headerOptionsSelection = (int)comboBoxPtr->GetItemData (m_headerListSelection);

		}	// end "if (pDX->m_bSaveAndValidate)"
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the reformat
//							change image format dialog box to the user so that the user can
//							select the options to be used.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 09/20/1995
//	Revised By:			Larry L. Biehl			Date: 09/21/2000	

Boolean CMChangeFormatDlg::DoDialog (
				FileInfoPtr							outFileInfoPtr, 
				ReformatOptionsPtr				reformatOptionsPtr)

{                              
	INT_PTR								returnCode;
	
	Boolean								OKFlag = FALSE;
	

			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
					            
	s_reformatOptionsPtr = reformatOptionsPtr;
	            																			
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{ 
		OKFlag = TRUE;   
				
				// Reformat area													
				
		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;
																
		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;

		OKFlag = ChangeImageFormatDialogOK (this,
														gImageWindowInfoPtr,
														gImageLayerInfoPtr,
														gImageFileInfoPtr,
														outFileInfoPtr,
														s_reformatOptionsPtr,
														&m_dialogSelectArea,
														m_outputFileSelection + 1,
														m_bandInterleaveSelection + 1,
														m_dataValueTypeSelection,
														m_sessionUserSetDataValueTypeSelectionFlag,
														m_channelSelection,
														m_invertBottomToTopFlag,
														m_invertLeftToRightFlag,
														m_swapBytesFlag,
														m_writeChanDescriptionFlag,
														m_outputInWavelengthOrderFlag,
														m_headerOptionsSelection,
														m_channelThematicDisplayFlag);
		 
		}	// end "if (returnCode == IDOK)"
		
	return (OKFlag);
		
}	// end "DoDialog"



BOOL CMChangeFormatDlg::OnInitDialog (void)

{       
	CComboBox*							comboBoxPtr;
	CMDialog*							dialogPtr = this;
	
	SInt16								bandInterleaveSelection,
											outputFileSelection,
											dataValueTypeSelection,
											channelSelection,
											headerOptionsSelection;

	Boolean								invertBottomToTopFlag,
											invertLeftToRightFlag,
											outputInWavelengthOrderFlag,
											swapBytesFlag;
	
	
	CDialog::OnInitDialog ();
	
			// Make sure that we have the bitmaps for the entire image buttons.
		
	VERIFY (toEntireButton.AutoLoad (IDEntireImage, this));
	VERIFY (toSelectedButton.AutoLoad (IDSelectedImage, this));

	ChangeImageFormatDialogInitialize (dialogPtr,
													gImageWindowInfoPtr,
													gImageFileInfoPtr,
													s_reformatOptionsPtr,
													&m_dialogSelectArea,
													m_inputBandInterleaveString,
													m_inputDataValueTypeString,
													m_tiffMenuNameString,
													&outputFileSelection,
													&bandInterleaveSelection,
													&dataValueTypeSelection,
													&m_savedDataValueTypeSelection,
													&channelSelection,
													&invertBottomToTopFlag,
													&invertLeftToRightFlag,
													&swapBytesFlag,
													&m_channelDescriptionAllowedFlag,
													&m_savedChannelDescriptionFlag,
													&m_outputInWavelengthOrderAllowedFlag,
													&outputInWavelengthOrderFlag,
													&headerOptionsSelection,
													&m_GAIAFormatAllowedFlag,
													&m_channelThematicDisplayFlag);

			// Be sure to make 0 based.

	m_outputFileSelection = outputFileSelection - 1;
	m_bandInterleaveSelection = bandInterleaveSelection - 1;
	m_dataValueTypeSelection = dataValueTypeSelection;
	m_channelSelection = channelSelection;
	m_invertBottomToTopFlag = invertBottomToTopFlag;
	m_invertLeftToRightFlag = invertLeftToRightFlag;
	m_swapBytesFlag = swapBytesFlag;
	m_writeChanDescriptionFlag = m_savedChannelDescriptionFlag;
	m_headerOptionsSelection = headerOptionsSelection;
	m_outputInWavelengthOrderFlag = outputInWavelengthOrderFlag;
	m_noTransformDataValueTypeSelection = m_savedDataValueTypeSelection;
	
	if (s_reformatOptionsPtr->transformDataCode != kNoTransform)
		{
		m_transformDataFlag = TRUE;
		DDX_Check (m_dialogToPtr, IDC_TransformData, m_transformDataFlag);

		}	// end "if (s_reformatOptionsPtr->transformDataCode != kNoTransform)"

	LoadDItemString (IDC_ChangeFormatInInterleave, (Str255*)m_inputBandInterleaveString);
	LoadDItemString (IDC_ChangeFormatInBits, (Str255*)m_inputDataValueTypeString);

	comboBoxPtr = (CComboBox*)(GetDlgItem (IDC_DataValueType));
	comboBoxPtr->SetItemData (0, k1ByteSignedIntegerMenuItem);
	comboBoxPtr->SetItemData (1, k1ByteUnsignedIntegerMenuItem);
	comboBoxPtr->SetItemData (2, k2ByteSignedIntegerMenuItem);
	comboBoxPtr->SetItemData (3, k2ByteUnsignedIntegerMenuItem);
	comboBoxPtr->SetItemData (4, k4ByteSignedIntegerMenuItem);
	comboBoxPtr->SetItemData (5, k4ByteUnsignedIntegerMenuItem);
	comboBoxPtr->SetItemData (6, k4ByteRealMenuItem);
	comboBoxPtr->SetItemData (7, k8ByteRealMenuItem);
	
	m_dataValueListSelection = GetComboListSelection (IDC_DataValueType,
																		abs (m_dataValueTypeSelection));
	
	if (m_dataValueTypeSelection < 0)
		MHideDialogItem ((DialogPtr)dialogPtr, IDC_DataValueType);

	else	// m_dataValueTypeSelection >= 0
		MHideDialogItem ((DialogPtr)dialogPtr, IDC_FixedDataType);
	
	m_localActiveFeaturesPtr = (UInt16*)s_reformatOptionsPtr->channelPtr;

	comboBoxPtr = (CComboBox*)(GetDlgItem (IDC_ChangeHeader));
	if (!m_GAIAFormatAllowedFlag)
		comboBoxPtr->DeleteString (kGAIAMenuItem);
	
	m_headerListSelection = GetComboListSelection (IDC_ChangeHeader,
																	m_headerOptionsSelection);
																	
	if (UpdateData (FALSE))
		PositionDialogWindow (); 
		
			// Set default text selection to first edit text item 

	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);  			
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"

  

void CMChangeFormatDlg::OnOutputInWavelengthOrder (void)

{                                                           
	DDX_Check (m_dialogFromPtr,
					IDC_OutputInWavelengthOrder,
					m_outputInWavelengthOrderFlag);
	
}	// end "OnOutputInWavelengthOrder" 



void CMChangeFormatDlg::OnTransformData (void)

{   
	CComboBox*							comboBoxPtr;
	
	DialogPtr							dialogPtr = this;

	SInt16								numChars;
	
	UInt16								recommendNumberOfBits,
											suggestedDataValueTypeSelection;

	Boolean								returnFlag = FALSE;
	                            
	
	SetDLogControlHilite (NULL, IDOK, 255);  
		
	CMReformatTransformDlg*		transformDialogPtr = NULL;
		
	TRY
		{                                
		transformDialogPtr = new CMReformatTransformDlg ();
			                                                                     
		returnFlag = transformDialogPtr->DoDialog (
											&recommendNumberOfBits, m_bandInterleaveSelection);
		   
	   if (transformDialogPtr != NULL)                           
			delete transformDialogPtr;
		}
			
	CATCH_ALL (e)
		{
		MemoryMessage (0, kObjectMessage);
		}
	
	END_CATCH_ALL                                    
				                
	if (returnFlag)
		{
		SetChannelDescriptionFlag ();

		if (s_reformatOptionsPtr->transformDataCode == kNoTransform)
			{
			MHideDialogItem (dialogPtr, IDC_MinimumBitsPrompt);

			m_transformDataFlag = FALSE;
			 
			m_dataValueTypeSelectionFlag  = TRUE;
			m_dataValueTypeSelection = m_noTransformDataValueTypeSelection;
							
			if (m_outputInWavelengthOrderAllowedFlag)
				{
				SetDLogControl (this,
										IDC_OutputInWavelengthOrder,
										m_outputInWavelengthOrderFlag);
				SetDLogControlHilite (this, IDC_OutputInWavelengthOrder, 0);
								
				}	// end "if (outputInWavelengthOrderAllowedFlag)"
							
			}	// end "if (...->transformDataCode == kNoTransform)" 
							
		else	// s_reformatOptionsPtr->transformDataCode != kNoTransform 
			{
			MShowDialogItem (dialogPtr, IDC_MinimumBitsPrompt);
			
			if (recommendNumberOfBits > 16)
				{
				numChars = sprintf ((char*)&gTextString2[1], 
											"Recommended minimum data bits: %d-Real",
											recommendNumberOfBits);
				suggestedDataValueTypeSelection = k4ByteRealMenuItem;

				}	// and "if (recommendNumberOfBits > 16)"

			else	// recommendNumberOfBits <= 16)
				{
				numChars = sprintf ((char*)&gTextString2[1], 
											"Recommended minimum data bits: %d",
											recommendNumberOfBits);
				suggestedDataValueTypeSelection = k2ByteSignedIntegerMenuItem;

				}	// and "else recommendNumberOfBits <= 16)"
							
					// The suggested data value type selection will be force if the
					// user has not set it before and the suggested setting contains
					// more bits than the previous setting.
					
			if (!m_sessionUserSetDataValueTypeSelectionFlag &&
					!gReformatOptionsPtr->userSetDataValueTypeSelectionFlag &&
						m_dataValueTypeSelection == m_savedDataValueTypeSelection &&
								suggestedDataValueTypeSelection > m_dataValueTypeSelection)
				{
				m_dataValueListSelection = suggestedDataValueTypeSelection - 1;
				DDX_CBIndex (m_dialogToPtr, IDC_DataValueType, m_dataValueListSelection);
				comboBoxPtr = (CComboBox*)GetDlgItem (IDC_DataValueType);
				m_dataValueTypeSelection = 
									(SInt16)comboBoxPtr->GetItemData (m_dataValueListSelection);

				}	// end "if (!...->userSetDataValueTypeSelectionFlag && ..."

			gTextString[0] = (UInt8)numChars;
			LoadDItemString (IDC_MinimumBitsPrompt, (char*)&gTextString2[1]);
							
			if (m_outputInWavelengthOrderAllowedFlag)
				{
				SetDLogControl (this,
										IDC_OutputInWavelengthOrder,
										0);
				SetDLogControlHilite (this, IDC_OutputInWavelengthOrder, 255);
								
				}	// end "if (m_outputInWavelengthOrderAllowedFlag)"

			m_transformDataFlag = TRUE; 
			 							
			}	// end "else ...->transformDataCode != kNoTransform" 
							
				// Input channels will be defined by the transformation.

		if (gReformatOptionsPtr->transformDataCode == kNoTransform ||
				gReformatOptionsPtr->transformDataCode == kAdjustChannel ||
					gReformatOptionsPtr->transformDataCode == kAdjustChannelsByChannel ||
						gReformatOptionsPtr->transformDataCode == kFunctionOfChannels)
			{
			MShowDialogItem (dialogPtr, IDC_ChannelsLabel);
			MShowDialogItem (dialogPtr, IDC_ChannelCombo);
			
			}	// end "if (...->transformDataCode == kNoTransform || ...)"

		else	// ...->transformDataCode == kTransformChannels ||
									//			transformDataCode == kCreatePCImage)
			{
			MHideDialogItem (dialogPtr, IDC_ChannelsLabel);
			MHideDialogItem (dialogPtr, IDC_ChannelCombo);
			
			}	// end "else ...->transformDataCode == kTransformChannels || ..."
		                            	                                   							
		}	// end "if (returnFlag)"
		
			// Make sure that the transform check box has the appropriate setting.
			
	DDX_Check (m_dialogToPtr, IDC_TransformData, m_transformDataFlag);
		
	SetDLogControlHilite (NULL, IDOK, 0);
	GetDlgItem (IDOK)->SetFocus ();
	
}	// end "OnTransformData"



void CMChangeFormatDlg::OnSelendokBandInterleave (void)

{    
	SInt16								headerOptionsSelection,
											outputFileSelection;
	
	Boolean 								changedFlag;


	DDX_CBIndex (m_dialogFromPtr, IDC_BandInterleave, m_bandInterleaveSelection);

	headerOptionsSelection = m_headerOptionsSelection;
	outputFileSelection = m_outputFileSelection;

	ChangeImageFormatDialogVerifyHeaderSetting (this,
																gImageFileInfoPtr,
																m_bandInterleaveSelection,
																m_dataValueTypeSelection,
																gPopUpOutputFileMenu,
																gPopUpHeaderOptionsMenu,
																&outputFileSelection,
																&headerOptionsSelection);
	
	changedFlag = (m_headerOptionsSelection != headerOptionsSelection);

	m_headerOptionsSelection = headerOptionsSelection;
	m_outputFileSelection = outputFileSelection;
	
	m_headerListSelection = GetComboListSelection (IDC_ChangeHeader,
																	m_headerOptionsSelection);
	DDX_CBIndex (m_dialogToPtr, IDC_ChangeHeader, m_headerListSelection);
	
	if (changedFlag)
		OnSelendokHeader (); 
	
}	// end "OnSelendokBandInterleave"



void CMChangeFormatDlg::OnSelendokOutChannels (void)

{  
	m_localActiveNumberFeatures = s_reformatOptionsPtr->numberChannels; 
	
	HandleChannelsMenu (IDC_ChannelCombo,
								kNoTransformation,
								(SInt16)gImageWindowInfoPtr->totalNumberChannels,
								kImageChannelType,
								TRUE);
	
	s_reformatOptionsPtr->numberChannels = m_localActiveNumberFeatures; 

}	// end "OnSelendokOutChannels"



void CMChangeFormatDlg::OnSelendokHeader (void)

{
	CComboBox*							comboBoxPtr;

	SInt16								bandInterleaveSelection,
											dataValueTypeSelection,
											outputFileSelection;
	
	Boolean								setChannelDescriptionFlag;
	

	dataValueTypeSelection = m_dataValueTypeSelection;
	
	DDX_CBIndex (m_dialogFromPtr, IDC_ChangeHeader, m_headerListSelection);
	comboBoxPtr = (CComboBox*)GetDlgItem (IDC_ChangeHeader);
	m_headerOptionsSelection = (int)comboBoxPtr->GetItemData (m_headerListSelection);

	outputFileSelection = m_outputFileSelection + 1;
	bandInterleaveSelection = m_bandInterleaveSelection;
					
	ChangeImageFormatDialogUpdateHeaderOptions (this,
																gImageFileInfoPtr,
																m_headerOptionsSelection,
																&outputFileSelection,
																&bandInterleaveSelection,
																&dataValueTypeSelection,
																&setChannelDescriptionFlag,
																gPopUpOutputFileMenu,
																m_channelDescriptionAllowedFlag,
																m_channelThematicDisplayFlag);

	m_outputFileSelection = outputFileSelection - 1;
	m_bandInterleaveSelection = bandInterleaveSelection;

	if (m_dataValueTypeSelection != dataValueTypeSelection)
		{
		m_dataValueTypeSelection = dataValueTypeSelection;
	
		m_dataValueListSelection = GetComboListSelection (IDC_DataValueType,
																			m_dataValueTypeSelection);

		}	// end "if (m_dataValueTypeSelection != dataValueTypeSelection)"

	if (setChannelDescriptionFlag)
		SetChannelDescriptionFlag (); 

	DDX_CBIndex (m_dialogToPtr, IDC_BandInterleave, m_bandInterleaveSelection);
	DDX_CBIndex (m_dialogToPtr, IDC_OutputFileSelection, m_outputFileSelection);
	DDX_CBIndex (m_dialogToPtr, IDC_DataValueType, m_dataValueListSelection);
	
}	// end "OnSelendokHeader" 



void CMChangeFormatDlg::SetChannelDescriptionFlag (void)

{ 	
	DialogPtr							dialogPtr = this;
	 
							                  
	if (s_reformatOptionsPtr->transformDataCode == kNoTransform ||
					s_reformatOptionsPtr->transformDataCode == kAdjustChannel)
		{
		if (m_channelDescriptionAllowedFlag)
			{
			m_writeChanDescriptionFlag = m_savedChannelDescriptionFlag;
			DDX_Check (m_dialogToPtr,
							IDC_WriteChanDescriptions,
							m_writeChanDescriptionFlag);
			SetDLogControlHilite (dialogPtr, IDC_WriteChanDescriptions, 0);
						
			}	// end "if (m_channelDescriptionAllowedFlag)" 

		}	// end "if (...->transformDataCode == kNoTransform || ..."

	else	// ...->transformDataCode == kTransformChannels ||
			//			transformDataCode == kCreatePCImage)
		{
		m_writeChanDescriptionFlag = FALSE;
		DDX_Check (m_dialogToPtr, IDC_WriteChanDescriptions, m_writeChanDescriptionFlag);
		SetDLogControlHilite (dialogPtr, IDC_WriteChanDescriptions, 255);

		}	// end "else ...->transformDataCode == kTransformChannels ..." 
	
}	// end "SetChannelDescriptionFlag"

  

void CMChangeFormatDlg::OnWriteChanDescriptions (void)

{                                                           
	DDX_Check (m_dialogFromPtr, IDC_WriteChanDescriptions, m_writeChanDescriptionFlag);
	m_savedChannelDescriptionFlag = m_writeChanDescriptionFlag;
	
}	// end "OnWriteChanDescriptions"  



void CMChangeFormatDlg::OnSelendokDataValueType ()

{
	CComboBox*							comboBoxPtr;

	SInt16								bandInterleaveSelection,
											dataValueListSelection,
											headerOptionsSelection,
											outputFileSelection;


	if (m_dataValueListSelection >= 0)
		{
		dataValueListSelection = m_dataValueListSelection;
		DDX_CBIndex (m_dialogFromPtr, IDC_DataValueType, m_dataValueListSelection);

		if (dataValueListSelection != m_dataValueListSelection)
			{	
			comboBoxPtr = (CComboBox*)GetDlgItem (IDC_DataValueType);
			m_dataValueTypeSelection =
								(SInt16)comboBoxPtr->GetItemData (m_dataValueListSelection);

			outputFileSelection = m_outputFileSelection;
			bandInterleaveSelection = m_bandInterleaveSelection;
			headerOptionsSelection = m_headerOptionsSelection;

			ChangeImageFormatDialogUpdateHeaderMenu (
															this,
															gImageFileInfoPtr,
															m_tiffMenuNameString,
															&outputFileSelection,
															&bandInterleaveSelection,
															&headerOptionsSelection,
															gPopUpOutputFileMenu,
															gPopUpHeaderOptionsMenu,
															s_reformatOptionsPtr->transformDataCode,
															m_dataValueTypeSelection,
															m_GAIAFormatAllowedFlag);

			m_outputFileSelection = outputFileSelection;
			m_bandInterleaveSelection = bandInterleaveSelection;
			m_headerOptionsSelection = headerOptionsSelection;

			DDX_CBIndex (m_dialogToPtr, IDC_BandInterleave, m_bandInterleaveSelection);
			
			m_headerListSelection = GetComboListSelection (IDC_ChangeHeader,
																			m_headerOptionsSelection);
			DDX_CBIndex (m_dialogToPtr, IDC_ChangeHeader, m_headerListSelection);

			}	// end "if (dataValueTypeSelection != m_dataValueTypeSelection)"

		if (s_reformatOptionsPtr->transformDataCode == kNoTransform)
			m_noTransformDataValueTypeSelection = m_dataValueTypeSelection;

		m_sessionUserSetDataValueTypeSelectionFlag = TRUE;

		}	// end "if (m_dataValueListSelection >= 0)"

}	// end "OnSelendokDataValueType"
