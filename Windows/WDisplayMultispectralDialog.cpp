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
//	File:						WDisplayMultispectralDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/17/2019
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMDisplaySpecsDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SDisplay_class.h"
#include "SImageWindow_class.h"

#include "WChannelsDialog.h"
#include "WDisplayMultispectralDialog.h"
#include "WImageView.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMDisplaySpecsDlg, CMDialog)
	//{{AFX_MSG_MAP (CMDisplaySpecsDlg)
	ON_BN_CLICKED (IDEntireImage, ToEntireImage)
	ON_BN_CLICKED (IDSelectedImage, ToSelectedImage)
	ON_BN_CLICKED (ID3C_ChannelDescriptions, OnChannelDescriptions)

	ON_CBN_CLOSEUP (ID3C_Enhancement, OnCloseupEnhancement)

	ON_CBN_SELENDOK (ID3C_BitsOfColor, OnSelendokBitsOfColor)
	ON_CBN_SELENDOK (ID3C_Channels, OnSelendokChannels)
	ON_CBN_SELENDOK (ID3C_DisplayType, OnSelendokDisplayType)
	ON_CBN_SELENDOK (ID3C_Enhancement, OnSelendokEnhancement)
	ON_CBN_SELENDOK (ID3C_MinMaxValues, OnSelendokMinMaxValues)

	ON_CBN_DROPDOWN (ID3C_BitsOfColor, OnDropdownBitsOfColor)
	ON_CBN_DROPDOWN (ID3C_Enhancement, OnDropdownEnhancement)

	ON_EN_CHANGE (ID3C_BlueChannel, OnChangeBlueChannel)
	ON_EN_CHANGE (ID3C_GrayChannel, OnChangeGrayChannel)
	ON_EN_CHANGE (ID3C_GreenChannel, OnChangeGreenChannel)
	ON_EN_CHANGE (ID3C_RedChannel, OnChangeRedChannel)
	ON_EN_CHANGE (ID3C_ColumnEnd, CheckColumnEnd)
	ON_EN_CHANGE (ID3C_ColumnStart, CheckColumnStart)
	ON_EN_CHANGE (ID3C_LineEnd, CheckLineEnd)
	ON_EN_CHANGE (ID3C_LineStart, CheckLineStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMDisplaySpecsDlg::CMDisplaySpecsDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMDisplaySpecsDlg::IDD, pParent)

{               
	WideFileStringPtr					fileNamePtr;
	
	
	//{{AFX_DATA_INIT (CMDisplaySpecsDlg)
	m_BlueChannel = 1;
	m_ColumnEnd = 1;
	m_ColumnInterval = 1;
	m_ColumnStart = 1;
	m_GreenChannel = 1;
	m_LineEnd = 1;
	m_LineInterval = 1;
	m_LineStart = 1;
	m_Magnification = 1;
	m_ComputeHistogramFlag = FALSE;
	m_NumberLevels = 254;
	m_RedChannel = 1;
	m_FileName = "";
	m_BitsOfColor = 0;
	m_DisplayType = 0;
	m_Enhancement = 0;
	m_GrayChannel = 1;
	m_TreatZeroAs = 0;
	m_MinMaxPopupCode = 0;
	m_redChannelInvertFlag = FALSE;
	m_greenChannelInvertFlag = FALSE;
	m_blueChannelInvertFlag = FALSE;
	m_thematicLegendFactor = 1.0;
	m_vectorOverlaysFlag = FALSE;
	//}}AFX_DATA_INIT  

	m_gaussianStretch = 2;
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
	if (m_initializedFlag)
		{                          
				// Get pointer to memory for temporary storage of channel list. 
			                                                                       
		m_localFeaturesPtr = (UInt16*)MNewPointer (
					(UInt32)gImageWindowInfoPtr->totalNumberChannels * sizeof (UInt16));
	                                                       
		m_initializedFlag = (m_localFeaturesPtr != NULL);
		
		}	// end "if (m_initializedFlag)"
		
	if (m_initializedFlag)                                                              
		{
		TRY
			{																			
					//	Set the image file name.										 
			
			fileNamePtr = (WideFileStringPtr)(gActiveImageViewCPtr->
						GetImageWindowCPtr())->GetImageFileNamePointer (kReturnUnicode);
			if (fileNamePtr != NULL)
				m_FileName = CString (fileNamePtr);
				
			}
			
		CATCH_ALL (e)
			{
			m_initializedFlag = FALSE;
			
			}
		
		END_CATCH_ALL
		
		}	// end "if (m_initializedFlag)"
																										
}	// end "CMDisplaySpecsDlg"



CMDisplaySpecsDlg::~CMDisplaySpecsDlg (void)

{  
	m_localFeaturesPtr = CheckAndDisposePtr (m_localFeaturesPtr);         
	
}	// end "~CMDisplaySpecsDlg"



void CMDisplaySpecsDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMDisplaySpecsDlg)
	DDX_Text (pDX, ID3C_BlueChannel, m_BlueChannel);
	DDV_MinMaxUInt (pDX, m_BlueChannel, 1, mMaxChannelNumber);
	DDX_Text (pDX, ID3C_GreenChannel, m_GreenChannel);
	DDV_MinMaxUInt (pDX, m_GreenChannel, 1, mMaxChannelNumber);
	DDX_Text2 (pDX, ID3C_Magnification, m_Magnification);
	DDV_MinMaxDouble (pDX, m_Magnification, 1.e-002, 99.);
	DDX_Text2 (pDX, IDC_LegendFactor, m_thematicLegendFactor);
	DDX_Check (pDX, ID3C_NewHistogram, m_ComputeHistogramFlag);
	DDX_Text (pDX, ID3C_NumberOfLevels, m_NumberLevels);
	DDX_Text (pDX, ID3C_RedChannel, m_RedChannel);
	DDV_MinMaxUInt (pDX, m_RedChannel, 1, mMaxChannelNumber);
	DDX_Text (pDX, ID3C_FileName, m_FileName);
	DDV_MaxChars (pDX, m_FileName, gFileNameLengthLimit);
	DDX_CBIndex (pDX, ID3C_BitsOfColor, m_BitsOfColor);
	DDX_CBIndex (pDX, ID3C_DisplayType, m_DisplayType);
	DDX_CBIndex (pDX, ID3C_Enhancement, m_Enhancement);       
	DDX_Text (pDX, ID3C_GrayChannel, m_GrayChannel);
	DDV_MinMaxUInt (pDX, m_GrayChannel, 1, mMaxChannelNumber);
	DDX_CBIndex (pDX, ID3C_TreatZeroAs, m_TreatZeroAs);
	DDX_CBIndex (pDX, ID3C_MinMaxValues, m_MinMaxPopupCode);
	DDX_Text (pDX, ID3C_ColumnEnd, m_ColumnEnd);
	DDV_MinMaxLong (pDX, m_ColumnEnd, 1, m_maxNumberColumns);
	DDX_Text (pDX, ID3C_ColumnInterval, m_ColumnInterval);
	DDV_MinMaxLong (pDX, m_ColumnInterval, 1, m_maxNumberColumns);
	DDX_Text (pDX, ID3C_ColumnStart, m_ColumnStart);
	DDV_MinMaxLong (pDX, m_ColumnStart, 1, m_maxNumberColumns);
	DDX_Text (pDX, ID3C_LineEnd, m_LineEnd);
	DDV_MinMaxLong (pDX, m_LineEnd, 1, m_maxNumberLines);
	DDX_Text (pDX, ID3C_LineInterval, m_LineInterval);
	DDV_MinMaxLong (pDX, m_LineInterval, 1, m_maxNumberLines);
	DDX_Text (pDX, ID3C_LineStart, m_LineStart);                              
	DDV_MinMaxLong (pDX, m_LineStart, 1, m_maxNumberLines);
	DDX_CBIndex (pDX, ID3C_Channels, m_channelSelection);
	DDX_Check (pDX, IDC_RedChannelInvert, m_redChannelInvertFlag);
	DDX_Check (pDX, IDC_GreenChannelInvert, m_greenChannelInvertFlag);
	DDX_Check (pDX, IDC_BlueChannelInvert, m_blueChannelInvertFlag);
	DDX_Check (pDX, IDC_VectorOverlays, m_vectorOverlaysFlag);
	//}}AFX_DATA_MAP
	
	if (pDX->m_bSaveAndValidate)
		{              
				// Get bits of color index for 8, 16,24 bit vector.
				 
		mBitsOfColorIndex = m_BitsOfColor;
		if (mBitsOfColorIndex == 1)
			mBitsOfColorIndex = 2;	 
			  
		VerifyLineColumnValues (pDX, 
										8,
										11,
										mPixelSizeVector[mBitsOfColorIndex],
										mLocalDisplayType,
										m_localActiveNumberFeatures);
																
				// Verify that the number of display levels makes sense.
				      
		UInt16 maxValue = GetMaximumDisplayLevels (mBitsOfColorIndex,
																	mLocalDisplayType,
																	mDuplicateChannelCode);
							                        
		DDX_Text (pDX, ID3C_NumberOfLevels, m_NumberLevels);
		DDV_MinMaxUInt (pDX, m_NumberLevels, 1, maxValue); 
		
		}	// end "if (pDX->m_bSaveAndValidate)"
		
}	// end "DoDataExchange"



void CMDisplaySpecsDlg::CheckChannel (
				UInt16								itemNumber,
				UINT*									channelValuePtr)
				
{
	UINT									newValue;

	SInt16								numberLevels;
	
	
	if (CheckLocalValue (itemNumber, &newValue))
		{
		if (newValue > 0 &&  newValue <= gImageWindowInfoPtr->totalNumberChannels)
			{
			*channelValuePtr = newValue;
		
					// Verify number of levels setting
	
			DisplayMultispectralDialogCheckDisplayLevels (
																m_displaySpecsPtr,
																this,
																mLocalDisplayType,
																m_RedChannel,
																m_GreenChannel,
																m_BlueChannel,
																(SInt16*)&mColorLevelsMax[0][0],
																&mDuplicateChannelCode,
																(SInt16)m_BitsOfColor + 1,
																&numberLevels);

			m_NumberLevels = numberLevels;
		
			}	// end "if (newValue > 0 &&"
			
		else	// newValue <= 0 || ...
			{
			DDX_Text (m_dialogToPtr, itemNumber, *channelValuePtr);
			
			}	// end "else newValue <= 0 || ..." 
		
		}	// end "if (CheckLocalValue (..."
	
}	// end "CheckChannel"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//	Called By:			Dialog in SDisplayMultispectral.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/01/1995
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean CMDisplaySpecsDlg::DoDialog (
				DisplaySpecsPtr					displaySpecsPtr)

{
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE;

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
	
	m_displaySpecsPtr = displaySpecsPtr;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{      
				// Display area													
				
		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;
																
		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;
		
		DisplayMultispectralDialogOK (this,
												displaySpecsPtr,
												gImageWindowInfoPtr,
												gImageFileInfoPtr,
												&m_dialogSelectArea,
												m_localFeaturesPtr,
												m_localActiveNumberFeatures,
												m_PercentClip,
												&mMinMaxValues[0],
												mDuplicateChannelCode,
												m_GrayChannel,
												m_RedChannel,
												m_GreenChannel,
												m_BlueChannel,
												mRGBColors,
												mLocalDisplayType,
												m_DisplayType+1,
												mBitsOfColorIndex+1,
												m_Magnification,
												(SInt16*)&mPixelSizeVector[0],
												m_gaussianStretch,
												m_Enhancement+1,
												m_MinMaxCode,
												m_TreatZeroAs+1,
												m_NumberLevels,
												(SInt16*)&mColorLevelsMax[0][0],
												m_ComputeHistogramFlag,
												m_channelSelection,
												m_redChannelInvertFlag,
												m_greenChannelInvertFlag,
												m_blueChannelInvertFlag,
												m_thematicLegendFactor,
												m_vectorOverlaysFlag);

		continueFlag = TRUE;
		
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetComputeHistogramDialogSetting
//
//	Software purpose:	The purpose of this routine is to determine the setting to
//							use for the compute histogram dialog item.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/05/91
//	Revised By:			Larry L. Biehl			Date: 06/06/95	

Boolean CMDisplaySpecsDlg::GetComputeHistogramDialogSetting (
				DisplaySpecsPtr					displaySpecsPtr,
				SInt16								lEnhancement,
				SInt16								lMinMaxCode)

{
	Boolean								setComputeItemFlag;
	
	
			// Initialize local variables.												
			
	setComputeItemFlag = FALSE;

	if (!displaySpecsPtr->defaultHistogramFileFlag)
		{
		if (lEnhancement == kEqualAreaStretch)
			setComputeItemFlag = TRUE;
			
		if (lEnhancement == kLinearStretch && 
				displaySpecsPtr->histogramCompute == 1 &&
				lMinMaxCode != kUserSpecified)
			setComputeItemFlag = TRUE;
			
		}	// end "if (!displaySpecsPtr->defaultHistogramFileFlag)"
		
	return (setComputeItemFlag); 
	
}	// end "GetComputeHistogramDialogSetting"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetMaximumDisplayLevels
//
//	Software purpose:	The purpose of this routine is to determine the maximum
//							number of display levels for the input specifications.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/06/1995
//	Revised By:			Larry L. Biehl			Date: 12/17/2019

UInt16 CMDisplaySpecsDlg::GetMaximumDisplayLevels (
				SInt16								bitsOfColorIndex,
				SInt16								displayType,
				SInt16								duplicateChannelCode)

{                                         
	UInt16								numberChannelsIndex = 0;
	

	switch (displayType)
		{
		case 1:
		case 2:
		case 4:
			numberChannelsIndex = 0;
			break;
			
		case 3:
			if (duplicateChannelCode)                          
				numberChannelsIndex = 1;
				
			else                                                                      
				numberChannelsIndex = 2;
								
			break;
			
		}	// end "switch (displayType)"
	
	return (mColorLevelsMax[bitsOfColorIndex][numberChannelsIndex]);
	
}	// end "GetMaximumDisplayLevels" 



void CMDisplaySpecsDlg::OnChangeBlueChannel ()

{  
	                                     
		  	// Get current blue channel
		  	
	CheckChannel (ID3C_BlueChannel, &m_BlueChannel);
	
}	// end "OnChangeBlueChannel"



void CMDisplaySpecsDlg::OnChangeGrayChannel ()

{
	CheckChannel (ID3C_GrayChannel, &m_GrayChannel);
	
}	// end "OnChangeGrayChannel"

				       

void CMDisplaySpecsDlg::OnChangeGreenChannel ()

{                                        
		  	// Get current green channel
		  	
	CheckChannel (ID3C_GreenChannel, &m_GreenChannel);
	
}	// end "OnChangeGreenChannel"



void CMDisplaySpecsDlg::OnChangeRedChannel ()

{                                         
		  	// Get current red channel
		  	
	CheckChannel (ID3C_RedChannel, &m_RedChannel);
	
}	// end "OnChangeRedChannel"



void CMDisplaySpecsDlg::OnChannelDescriptions ()
 
{
	
	CMChannelsDlg*						channelsDialogPtr = NULL;

	SInt16								tempInt;
	
			// Show channel list.							
								                         
	SetDLogControlHilite (NULL, IDOK, 255);
	
	TRY
		{                                
		channelsDialogPtr = new CMChannelsDlg ();
			                                                                  
		channelsDialogPtr->DoDialog (&tempInt,
												NULL,
												gImageLayerInfoPtr,
												gImageFileInfoPtr,
												2,
												kNoTransformation,
												NULL,
												(SInt16)gImageWindowInfoPtr->totalNumberChannels,
												kSubsetMenuItem); 
		
		if (channelsDialogPtr != NULL)                           
			delete channelsDialogPtr;
			
		}	// end "TRY"
		
	CATCH_ALL (e)
		{
		MemoryMessage (0, kObjectMessage);
		}
	
	END_CATCH_ALL 
		
	SetDLogControlHilite (NULL, IDOK, 0);
	
}	// end "OnChannelDescriptions"



void CMDisplaySpecsDlg::OnCloseupEnhancement ()

{
	if (m_optionKeyFlag)
		{     
		SetComboItemText (ID3C_Enhancement, 
								2,
								(UCharPtr)"Gaussian");

		DDX_CBIndex (m_dialogToPtr, ID3C_Enhancement, m_Enhancement);

		m_optionKeyFlag = FALSE;

		}	// end "if (m_optionKeyFlag)"
	
}	// end "OnCloseupEnhancement"



void CMDisplaySpecsDlg::OnDropdownBitsOfColor (void)

{                                
	CComboBox* comboBoxPtr = (CComboBox*)GetDlgItem (ID3C_BitsOfColor);
	                                                          
	if (mLocalDisplayType == 1 || mLocalDisplayType == 2 || mLocalDisplayType == 7)
		{
		if (comboBoxPtr->GetCount () == 2)
   			// Remove the '24' item from the bits of color list.
   		                                                    
			comboBoxPtr->DeleteString (1);
			
		}	// end "if (mLocalDisplayType == 1 || mLocalDisplayType == 2 ..."
		
	else	// mLocalDisplayType != 1 && mLocalDisplayType != 2 && ...
		{
		if (comboBoxPtr->GetCount () == 1)
			comboBoxPtr->AddString ((LPCTSTR)_T("24"));
			
		}	// end "else mLocalDisplayType != 1 && ..."
	
}	// end "OnDropdownBitsOfColor"  



void CMDisplaySpecsDlg::OnDropdownEnhancement ()

{
	m_optionKeyFlag = FALSE;
	if (GetKeyState (VK_RBUTTON) < 0 || GetKeyState (VK_SHIFT) & 0x8000)
		{                                                                             
		SetComboItemText (ID3C_Enhancement, 
								2,
								(UCharPtr)"Gaussian...");
								
		m_optionKeyFlag = TRUE;

		DDX_CBIndex (m_dialogToPtr, ID3C_Enhancement, m_Enhancement);
		
		}	// end "if (GetKeyState (VK_RBUTTON) < 0)"
	
}	// end "OnDropdownEnhancement"



BOOL CMDisplaySpecsDlg::OnInitDialog ()

{                                
	SInt16								bitsOfColor,
											blueChannel,
											channelSelection,
											displayType,
											enhancement,
											grayChannel,
											greenChannel,
											localNumberFeatures,
											minMaxPopupCode,
											numberLevels,
											redChannel,
											treatZeroAs;
	
	
	CDialog::OnInitDialog ();
	
	DisplayMultispectralDialogInitialize (this,
														m_displaySpecsPtr,
														gImageWindowInfoPtr,
														&m_dialogSelectArea,
														m_localFeaturesPtr,
														&localNumberFeatures,
														&m_PercentClip,
														&mMinMaxValues[0],
														&mDuplicateChannelCode,
														&grayChannel,
														&redChannel,
														&greenChannel,
														&blueChannel,
														&mRGBColors,
														&mLocalDisplayType,
														&mMaxSystemPixelSize,
														&displayType,
														&bitsOfColor,
														&m_SavedBitsOfColor,
														&m_Magnification,
														(SInt16*)&mPixelSizeVector[0],
														&m_gaussianStretch,
														&enhancement,
														&m_MinMaxCode,
														&minMaxPopupCode,
														&m_thematicDisplayWithUserSettingFlag,
														&treatZeroAs,
														(SInt16*)&mColorLevelsMax[0][0],
														&numberLevels,
														(Boolean*)&m_ComputeHistogramFlag,
														&channelSelection,
														(Boolean*)&m_redChannelInvertFlag,
														(Boolean*)&m_greenChannelInvertFlag,
														(Boolean*)&m_blueChannelInvertFlag,
														&m_thematicLegendFactor,
														(Boolean*)&m_vectorOverlaysFlag);    

	m_localNumberFeatures = localNumberFeatures;
	m_GrayChannel = grayChannel;
	m_RedChannel = redChannel;
	m_GreenChannel = greenChannel;
	m_BlueChannel = blueChannel;
	m_DisplayType = displayType - 1;
	m_BitsOfColor = bitsOfColor - 1;
	m_Enhancement = enhancement - 1;
	m_MinMaxPopupCode = minMaxPopupCode - 1;
	m_TreatZeroAs = treatZeroAs - 1;
	m_channelSelection = channelSelection;
	m_NumberLevels = numberLevels;
	
	m_LineStart = m_displaySpecsPtr->lineStart;
	m_LineEnd = m_displaySpecsPtr->lineEnd;
	m_LineInterval = m_displaySpecsPtr->lineInterval;
	m_ColumnStart = m_displaySpecsPtr->columnStart;
	m_ColumnEnd = m_displaySpecsPtr->columnEnd;
	m_ColumnInterval = m_displaySpecsPtr->columnInterval;
	
			// Set limit values 
			                                                    
	mMaxChannelNumber = gImageWindowInfoPtr->totalNumberChannels;

			// Force magnification that is displayed to be limited to 3 decimal digits.
	
	sprintf ((char*)&gTextString, "%.3f", m_Magnification);
	m_Magnification = atof ((char*)&gTextString);
	
			// Make sure that we have the bitmaps for the entire image buttons.
		
	VERIFY (toEntireButton.AutoLoad (IDEntireImage, this));
	VERIFY (toSelectedButton.AutoLoad (IDSelectedImage, this));
	
	SetEntireImageButtons (NULL,
									m_LineStart, 
									m_LineEnd, 
									m_ColumnStart, 
									m_ColumnEnd);
	
			// Set feature parameters 
			
	m_localActiveNumberFeatures =	m_localNumberFeatures; 
	m_localActiveFeaturesPtr = m_localFeaturesPtr;
														
	if (UpdateData (FALSE))
		PositionDialogWindow (); 
	
			// Set up edit text fields for channels to be used for diplay	
			
	UpdateDialogChannelItems (this, mRGBColors, mLocalDisplayType);
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"




void CMDisplaySpecsDlg::OnSelendokBitsOfColor ()

{           
	SInt16								numberLevels;


			// Get current bits of color setting.
			
	DDX_CBIndex (m_dialogFromPtr, ID3C_BitsOfColor, m_BitsOfColor);
	
			// Save the bits of color index.

	m_SavedBitsOfColor = m_BitsOfColor;
			
			// Verify number of levels setting
	
	DisplayMultispectralDialogCheckDisplayLevels (m_displaySpecsPtr,
																	this,
																	mLocalDisplayType,
																	m_RedChannel,
																	m_GreenChannel,
																	m_BlueChannel,
																	(SInt16*)&mColorLevelsMax[0][0],
																	&mDuplicateChannelCode,
																	(SInt16)m_BitsOfColor + 1,
																	&numberLevels);

	m_NumberLevels = numberLevels;
	
}	// end "OnSelendokBitsOfColor"



void CMDisplaySpecsDlg::OnSelendokChannels (void)

{  
	HandleChannelsMenu (ID3C_Channels,
								kNoTransformation,
								(SInt16)gImageWindowInfoPtr->totalNumberChannels,
								kImageChannelType,
								TRUE);
	
}	// end "OnSelendokChannels"
                           


void CMDisplaySpecsDlg::OnSelendokDisplayType (void)

{  
	SInt16*								colorLevelsMaxPtr;
	
	SInt32								blueChannel,
											greenChannel,
											redChannel;
										
	SInt16								bitsOfColor,
											currentDisplayTypeMenuSelection,
											minMaxPopupCode,
											numberDisplayLevels;
	
	Boolean								checkMinMaxSettingFlag;
	WindowInfoPtr						windowInfoPtr;
	
	
	currentDisplayTypeMenuSelection = m_DisplayType + 1;
	                                 
	DDX_CBIndex (m_dialogFromPtr, ID3C_DisplayType, m_DisplayType);
	
	blueChannel = m_BlueChannel;
	greenChannel = m_GreenChannel;
	redChannel = m_RedChannel;
	bitsOfColor = m_BitsOfColor + 1;		// Make index from 1 not 0
	
	colorLevelsMaxPtr = (SInt16*)&mColorLevelsMax[0][0];
	
	DisplayMultispectralDialogUpdateDisplayType (m_displaySpecsPtr,
																this,
																currentDisplayTypeMenuSelection,
																m_DisplayType+1,
																m_SavedBitsOfColor,
																&mLocalDisplayType,
																&mRGBColors,
																&bitsOfColor,
																mMaxSystemPixelSize,
																&numberDisplayLevels,
																redChannel,
																greenChannel,
																blueChannel,
																colorLevelsMaxPtr,
																&mDuplicateChannelCode,
																&checkMinMaxSettingFlag);
																
	m_BitsOfColor = bitsOfColor - 1;

	m_NumberLevels = numberDisplayLevels;

	DDX_CBIndex (m_dialogToPtr, ID3C_BitsOfColor, m_BitsOfColor);
	
   		// Get index into 8, 16, 24 bits of color vector.
   
   mBitsOfColorIndex = Update8_16_24BitsOfColorIndex (m_BitsOfColor);

			// Check min-max menu settings.

	minMaxPopupCode = m_MinMaxPopupCode + 1;
	DisplayMultispectralDialogCheckMinMaxSettings (
															m_DisplayType+1,
															m_MinMaxPopupCode+1,
															&minMaxPopupCode,
															&m_MinMaxCode,
															&m_thematicDisplayWithUserSettingFlag);

	m_MinMaxPopupCode = minMaxPopupCode - 1;                     
	DDX_CBIndex (m_dialogToPtr, ID3C_MinMaxValues, m_MinMaxPopupCode);

	DisplayMultispectralDialogUpdateComputeHistogram (m_displaySpecsPtr,
																		m_ComputeHistogramFlag,
																		m_DisplayType+1,
																		m_Enhancement+1,
																		m_MinMaxPopupCode+1, 
																		this, 
																		ID3C_NewHistogram);
	
}	// end "OnSelendokDisplayType"



void CMDisplaySpecsDlg::OnSelendokEnhancement ()

{
	DDX_CBIndex (m_dialogFromPtr, ID3C_Enhancement, m_Enhancement);
	
	if (m_Enhancement == 2 && m_optionKeyFlag)
		{     
		SetDLogControlHilite (NULL, IDOK, 255);
	                        
		GaussianParameterDialog (&m_gaussianStretch);
															                                 
		SetDLogControlHilite (NULL, IDOK, 0);
		
		}	// end "if (m_localMinMaxCode == 2)" 

	DisplayMultispectralDialogUpdateComputeHistogram (m_displaySpecsPtr,
																		m_ComputeHistogramFlag,
																		m_DisplayType+1,
																		m_Enhancement+1,
																		m_MinMaxPopupCode+1, 
																		this, 
																		ID3C_NewHistogram);
	
}	// end "OnSelendokEnhancement"



void CMDisplaySpecsDlg::OnSelendokMinMaxValues ()

{         
	SInt16								channels[3];
											
	                                                 						                       
	DDX_CBIndex (m_dialogFromPtr, ID3C_MinMaxValues, m_MinMaxPopupCode);
	
	if (m_MinMaxPopupCode == 0)
		{
		m_MinMaxCode = kPerChannelMinMax;
		m_PercentClip = 0;
		
		}	// end "if (m_MinMaxPopupCode == 0)"
	
	else if (m_MinMaxPopupCode == 1)
		{
		m_MinMaxCode = kPerChannelMinMax;
		m_PercentClip = 2;
		
		}	// end "if (m_MinMaxPopupCode == 1)"
	
	else if (m_MinMaxPopupCode == 2) 
		m_MinMaxCode = kEntireDataRange;        
	
	else if (m_MinMaxPopupCode == 3)
		{                                                   
		SetDLogControlHilite (NULL, IDOK, 255);								
	                           
		channels[0] = (SInt16)m_RedChannel; 
		channels[1] = (SInt16)m_GreenChannel;
		channels[2] = (SInt16)m_BlueChannel; 
		
		if (mLocalDisplayType <= 2)
			channels[0] = (SInt16)m_GrayChannel;
	                        
		MinMaxEnhancementDialog (channels,
											mRGBColors,
											mLocalDisplayType,
											m_NumberLevels,
											&m_PercentClip, 
											&m_MinMaxCode, 
											mMinMaxValues);
											
		m_MinMaxPopupCode = GetMinMaxPopupCode (m_MinMaxCode, m_PercentClip);
		DDX_CBIndex (m_dialogToPtr, ID3C_MinMaxValues, m_MinMaxPopupCode);
															                                 
		SetDLogControlHilite (NULL, IDOK, 0);
		
		}	// end "if (m_localMinMaxCode == 3)" 
	
	else if (m_MinMaxPopupCode == 4) 
		m_MinMaxCode = kThematicDefault;  

	DisplayMultispectralDialogUpdateComputeHistogram (m_displaySpecsPtr,
																		m_ComputeHistogramFlag,
																		m_DisplayType+1,
																		m_Enhancement+1,
																		m_MinMaxCode, 
																		this, 
																		ID3C_NewHistogram);      
	
}	// end "OnSelendokMinMaxValues"
