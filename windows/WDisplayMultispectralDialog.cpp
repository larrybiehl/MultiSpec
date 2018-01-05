// WDisplayMultispectralDialog.cpp : implementation file
//
//	Revised by Larry Biehl on 12/21/2017
//
                   
#include "SMultiSpec.h"

#include "WDisplayMultispectralDialog.h"

#include "CImageWindow.h"
#include "CDisplay.h"
#include "CProcessor.h"

#include "WImageView.h"
#include "WChannelsDialog.h"  

//#include	"SExtGlob.h" 
//#include "wdisplaymultispectraldialog.h"
//#include ".\wdisplaymultispectraldialog.h"

//#define k1_ChannelDisplayType				2
//#define k2_ChannelDisplayType				4
//#define k3_ChannelDisplayType				3
//#define k3_2_ChannelDisplayType			5
//#define kSideSideChannelDisplayType		7	

extern void DisplayMultispectralDialogInitialize (
								DialogPtr							dialogPtr,
								DisplaySpecsPtr					displaySpecsPtr,
								WindowInfoPtr						windowInfoPtr,
								DialogSelectArea*					dialogSelectAreaPtr,
								UInt16*								localFeaturePtr,
								SInt16*								localNumberChannelsPtr,
								SInt16*								localPercentClipPtr,
								double*								localMinMaxValuesPtr,
								SInt16*								duplicateChannelCodePtr,
								SInt16*								localGrayChannelNumberPtr,
								SInt16*								localRedChannelNumberPtr,
								SInt16*								localGreenChannelNumberPtr,
								SInt16*								localBlueChannelNumberPtr,
								SInt16*								localrgbColorsPtr,
								UInt16*								localDisplayTypePtr,
								SInt16*								maxSystemPixelSizePtr,
								SInt16*								displayTypeMenuSelectionPtr,
								SInt16*								bitsOfColorSelectionPtr,
								SInt16*								savedBitsOfColorPtr,
								double*								saveMagnificationPtr,
								SInt16*								pixelSizeVectorPtr,
								double*								localGaussianStretchPtr,
								SInt16*								enhanceStretchSelectionPtr,
								SInt16*								enhanceMinMaxOptionCodePtr,
								SInt16*								enhanceMinMaxMenuSelectionPtr,
								Boolean*								thematicDisplayWithUserSettingFlagPtr,
								SInt16*								enhanceZeroAsSelectionPtr,
								SInt16*								localColorLevelsMaxPtr,
								SInt16*								numberDisplayLevelsPtr,
								Boolean*								userComputeFlagPtr,
								SInt16*								channelSelectionPtr,
								Boolean*								redChannelInvertFlagPtr,
								Boolean*								greenChannelInvertFlagPtr,
								Boolean*								blueChannelInvertFlagPtr,
								double*								localThematicValueFactorPtr,
								Boolean*								vectorOverlaysFlagPtr);

extern void 			DisplayMultispectralDialogOK (
								DialogPtr							dialogPtr,
								DisplaySpecsPtr					displaySpecsPtr,
								WindowInfoPtr						windowInfoPtr,
								FileInfoPtr							fileInfoPtr,
								DialogSelectArea*					dialogSelectAreaPtr,
								UInt16*								localFeaturesPtr,
								SInt16								localNumberChannels,
								SInt16								localPercentClip,
								double*								localMinMaxValuesPtr,
								SInt16								duplicateChannelCode,
								SInt16								localGrayChannelNumber,
								SInt16								localRedChannelNumber,
								SInt16								localGreenChannelNumber,
								SInt16								localBlueChannelNumber,
								SInt16								localrgbColors,
								UInt16								localDisplayType,
								SInt16								displayTypeMenuSelection,
								SInt16								bitsOfColorSelection,
								double								saveMagnification,
								SInt16*								pixelSizeVectorPtr,
								double								localGaussianStretch,
								SInt16								enhanceStretchSelection,
								SInt16								enhanceMinMaxOptionCode,
								SInt16								enhanceZeroAsSelection,
								SInt16								numberDisplayLevels,
								SInt16*								localColorLevelsMaxPtr,
								Boolean								userComputeFlag,
								SInt16								channelSelection,
								Boolean								redChannelInvertFlag,
								Boolean								greenChannelInvertFlag,
								Boolean								blueChannelInvertFlag,
								double								localThematicValueFactor,
								Boolean								vectorOverlaysFlag);
								
extern void 			DisplayMultispectralDialogCheckDisplayLevels (
								DisplaySpecsPtr					displaySpecsPtr,
								DialogPtr							dialogPtr,
								SInt16								displayType,
								SInt16								redChannel,
								SInt16								greenChannel,
								SInt16								blueChannel,
								SInt16*								localColorLevelsMaxPtr,
								SInt16*								duplicateChannelCodePtr,
								SInt16								bitsOfColorSelection,
								SInt16*								numberDisplayLevelsPtr);

extern void 			DisplayMultispectralDialogCheckMinMaxSettings ( 
								SInt16								displayTypeSelection,
								SInt16								inputEnhanceMinMaxMenuSelection,
								SInt16*								enhanceMinMaxMenuSelectionPtr,
								SInt16*								enhanceMinMaxOptionPtr,
								Boolean*								thematicDisplayWithUserSettingFlagPtr);
												
extern void 			DisplayMultispectralDialogUpdateDisplayLevels (	
								DialogPtr 							dialogPtr, 
								DisplaySpecsPtr					displaySpecsPtr, 
								SInt16*		 						localColorLevelsMaxPtr, 
								SInt16 								displayType, 
								SInt16		 						duplicateChannelCode,
								SInt16*								numberDisplayLevelsPtr);

extern SInt16 			DisplayMultispectralDialogUpdateDisplayType (
								DisplaySpecsPtr					displaySpecsPtr,
								DialogPtr							dialogPtr,
								SInt16								currentDisplayTypeMenuSelection,
								SInt16								newDisplayTypeMenuSelection,
								SInt16								savedBitsOfColor,
								UInt16*								localDisplayTypePtr,
								SInt16*								localRGBColorsPtr,
								SInt16*								bitsOfColorSelectionPtr,
								SInt16								maxSystemPixelSize,
								SInt16*								numberDisplayLevelsPtr,
								SInt32								redChannel,
								SInt32								greenChannel,
								SInt32								blueChannel,
								SInt16*								localColorLevelsMaxPtr,
								SInt16*								duplicateChannelCodePtr,
								Boolean*								checkMinMaxSettingFlagPtr);

extern Boolean		 	DisplayMultispectralDialogUpdateComputeHistogram (
								DisplaySpecsPtr					displaySpecsPtr, 
								Boolean								userComputeFlag,
								SInt16								displayType,
								SInt16								enhanceStretchSelection, 
								SInt16								lMinMaxCode, 
								DialogPtr							dialogPtr, 
								SInt16								itemNumber);
								
extern void 			UpdateDialogChannelItems (
								DialogPtr 							dialogPtr, 
//								SInt16								displayTypeSelection,
								SInt16 								rgbColors, 
								SInt16 								displayType);

extern Boolean 		GaussianParameterDialog (
								double*								gaussianStretchPtr); 

extern Boolean 		MinMaxEnhancementDialog (
								SInt16*	 							channelsPtr,
								SInt16 								rgbColors,
								SInt16 								displayType,
								SInt16								numberLevels,
								SInt16* 								percentTailsClippedPtr, 
								SInt16* 								minMaxSelectionPtr, 
								double*	 							minMaxValuesPtr); 	

extern SInt16 			GetHistogramComputeCode (
								DisplaySpecsPtr					displaySpecsPtr);
								
extern SInt16 			GetMinMaxPopupCode(
								SInt16								minMaxCode,
								SInt16								percentClip);


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMDisplaySpecsDlg dialog


CMDisplaySpecsDlg::CMDisplaySpecsDlg(CWnd* pParent /*=NULL*/)
	: CMDialog(CMDisplaySpecsDlg::IDD, pParent)
{               
	WideFileStringPtr				fileNamePtr;
	
	
	//{{AFX_DATA_INIT(CMDisplaySpecsDlg)
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
				(UInt32)gImageWindowInfoPtr->totalNumberChannels * sizeof(UInt16) ); 
	                                                       
		m_initializedFlag = (m_localFeaturesPtr != NULL);
		
		}		// end "if (m_initializedFlag)"
		
	if (m_initializedFlag)                                                              
		{
		TRY
			{																			
					//	Set the image file name.										 
			
			fileNamePtr = (WideFileStringPtr)(gActiveImageViewCPtr->
						GetImageWindowCPtr())->GetImageFileNamePointer (kReturnUnicode);
			if (fileNamePtr != NULL)
				m_FileName = CString(fileNamePtr);
				
			}
			
		CATCH_ALL(e)
			{
			m_initializedFlag = FALSE;
			
			}
		END_CATCH_ALL
		
		}		// end "if (m_initializedFlag)" 
		
	if (!m_initializedFlag)
																										return;  
																										
}		// end "CMDisplaySpecsDlg"



CMDisplaySpecsDlg::~CMDisplaySpecsDlg(void)
{  
	m_localFeaturesPtr = CheckAndDisposePtr (m_localFeaturesPtr);         
	
}		// end "~CMDisplaySpecsDlg"


/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-2002)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CheckDisplayLevels
//
//	Software purpose:	The purpose of this routine is to check the display
//							levels dialog item in the multispectral display dialog.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/01/95
//	Revised By:			Larry L. Biehl			Date: 06/02/95	

void 
CMDisplaySpecsDlg::CheckDisplayLevels (
				DialogPtr					dialogPtr,
				UInt16						displayType)

{  
	mDuplicateChannelCode = 0;
	                                
	if (displayType == 3)
		{ 
		if (m_GreenChannel == m_BlueChannel)
			mDuplicateChannelCode = kRGColor;
		
		if (m_RedChannel == m_BlueChannel)
			mDuplicateChannelCode = kGBColor; 
		
		if (m_RedChannel == m_GreenChannel)
			mDuplicateChannelCode = kRBColor;
		
		}		// end "if (displayType == 3)"
		
	UpdateDisplayLevels (
						dialogPtr,             
						displayType,
						mDuplicateChannelCode);                                                  
	
}		// end "CheckDisplayLevels"
*/


void CMDisplaySpecsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMDisplaySpecsDlg)
	DDX_Text(pDX, ID3C_BlueChannel, m_BlueChannel);
	DDV_MinMaxUInt(pDX, m_BlueChannel, 1, mMaxChannelNumber);
	DDX_Text(pDX, ID3C_GreenChannel, m_GreenChannel);
	DDV_MinMaxUInt(pDX, m_GreenChannel, 1, mMaxChannelNumber);
	DDX_Text2(pDX, ID3C_Magnification, m_Magnification);
	DDV_MinMaxDouble(pDX, m_Magnification, 1.e-002, 99.);
	DDX_Text2(pDX, IDC_LegendFactor, m_thematicLegendFactor);
	DDX_Check(pDX, ID3C_NewHistogram, m_ComputeHistogramFlag);
	DDX_Text(pDX, ID3C_NumberOfLevels, m_NumberLevels);
	DDX_Text(pDX, ID3C_RedChannel, m_RedChannel);
	DDV_MinMaxUInt(pDX, m_RedChannel, 1, mMaxChannelNumber);
	DDX_Text(pDX, ID3C_FileName, m_FileName);
	DDV_MaxChars(pDX, m_FileName, gFileNameLengthLimit);
	DDX_CBIndex(pDX, ID3C_BitsOfColor, m_BitsOfColor);
	DDX_CBIndex(pDX, ID3C_DisplayType, m_DisplayType);
	DDX_CBIndex(pDX, ID3C_Enhancement, m_Enhancement);       
	DDX_Text(pDX, ID3C_GrayChannel, m_GrayChannel);
	DDV_MinMaxUInt(pDX, m_GrayChannel, 1, mMaxChannelNumber);
	DDX_CBIndex(pDX, ID3C_TreatZeroAs, m_TreatZeroAs);
	DDX_CBIndex(pDX, ID3C_MinMaxValues, m_MinMaxPopupCode);
	DDX_Text(pDX, ID3C_ColumnEnd, m_ColumnEnd);
	DDV_MinMaxLong(pDX, m_ColumnEnd, 1, m_maxNumberColumns);
	DDX_Text(pDX, ID3C_ColumnInterval, m_ColumnInterval);
	DDV_MinMaxLong(pDX, m_ColumnInterval, 1, m_maxNumberColumns);
	DDX_Text(pDX, ID3C_ColumnStart, m_ColumnStart);
	DDV_MinMaxLong(pDX, m_ColumnStart, 1, m_maxNumberColumns);
	DDX_Text(pDX, ID3C_LineEnd, m_LineEnd);
	DDV_MinMaxLong(pDX, m_LineEnd, 1, m_maxNumberLines);
	DDX_Text(pDX, ID3C_LineInterval, m_LineInterval);
	DDV_MinMaxLong(pDX, m_LineInterval, 1, m_maxNumberLines);
	DDX_Text(pDX, ID3C_LineStart, m_LineStart);                              
	DDV_MinMaxLong(pDX, m_LineStart, 1, m_maxNumberLines);
	DDX_CBIndex(pDX, ID3C_Channels, m_channelSelection);
	DDX_Check(pDX, IDC_RedChannelInvert, m_redChannelInvertFlag);
	DDX_Check(pDX, IDC_GreenChannelInvert, m_greenChannelInvertFlag);
	DDX_Check(pDX, IDC_BlueChannelInvert, m_blueChannelInvertFlag);
	DDX_Check(pDX, IDC_VectorOverlays, m_vectorOverlaysFlag);
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
				      
		UInt16 maxValue = GetMaximumDisplayLevels (
											mBitsOfColorIndex,
											mLocalDisplayType,
											mDuplicateChannelCode);
							                        
		DDX_Text(pDX, ID3C_NumberOfLevels, m_NumberLevels);
		DDV_MinMaxUInt(pDX, m_NumberLevels, 1, maxValue); 
		
		}		// end "if (pDX->m_bSaveAndValidate)"
		
}		// end "DoDataExchange"



BEGIN_MESSAGE_MAP(CMDisplaySpecsDlg, CMDialog)
	//{{AFX_MSG_MAP(CMDisplaySpecsDlg)
	ON_CBN_SELENDOK(ID3C_DisplayType, OnSelendokDisplayType)
	ON_CBN_SELENDOK(ID3C_BitsOfColor, OnSelendokBitsOfColor)
	ON_EN_CHANGE(ID3C_BlueChannel, OnChangeBlueChannel)
	ON_EN_CHANGE(ID3C_GreenChannel, OnChangeGreenChannel)
	ON_EN_CHANGE(ID3C_RedChannel, OnChangeRedChannel)
	ON_CBN_SELENDOK(ID3C_Channels, OnSelendokChannels)
	ON_CBN_DROPDOWN(ID3C_BitsOfColor, OnDropdownBitsOfColor)
	ON_CBN_SELENDOK(ID3C_MinMaxValues, OnSelendokMinMaxValues)
	ON_EN_CHANGE(ID3C_GrayChannel, OnChangeGrayChannel)
	ON_BN_CLICKED(ID3C_ChannelDescriptions, OnChannelDescriptions)
	ON_CBN_SELENDOK(ID3C_Enhancement, OnSelendokEnhancement)
	ON_CBN_DROPDOWN(ID3C_Enhancement, OnDropdownEnhancement)
	ON_EN_CHANGE(ID3C_ColumnEnd, CheckColumnEnd)
	ON_EN_CHANGE(ID3C_ColumnStart, CheckColumnStart)
	ON_EN_CHANGE(ID3C_LineEnd, CheckLineEnd)
	ON_EN_CHANGE(ID3C_LineStart, CheckLineStart)
	ON_BN_CLICKED(IDEntireImage, ToEntireImage)
	ON_BN_CLICKED(IDSelectedImage, ToSelectedImage)
	ON_CBN_CLOSEUP(ID3C_Enhancement, OnCloseupEnhancement)
	//}}AFX_MSG_MAP
//	ON_BN_CLICKED(IDC_RedChannelInvert, OnBnClickedRedchannelinvert)
// ON_EN_CHANGE(IDC_LegendFactor, OnEnChangeLegendfactor)
END_MESSAGE_MAP()




void 
CMDisplaySpecsDlg::CheckChannel(
				UInt16			itemNumber,
				UINT*				channelValuePtr)
				
{  
	UINT					newValue;

	SInt16				numberLevels;
	
	
	if ( CheckLocalValue(itemNumber, &newValue) )
		{
		if (newValue > 0 && 
						newValue <= gImageWindowInfoPtr->totalNumberChannels)
			{
			*channelValuePtr = newValue;
		
					// Verify number of levels setting
	
//			CheckDisplayLevels (NULL, mLocalDisplayType);
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
		
			}		// end "if (newValue > 0 &&"
			
		else		// newValue <= 0 || ...
			{
			DDX_Text(m_dialogToPtr, itemNumber, *channelValuePtr);
			
			}		// end "else newValue <= 0 || ..." 
		
		}		// end "if ( CheckLocalValue(..."
	
}		// end "CheckChannel"


//-----------------------------------------------------------------------------
//								 Copyright (1988-2002)
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
//	Coded By:			Larry L. Biehl			Date: 06/01/1995
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean 
CMDisplaySpecsDlg::DoDialog(
				DisplaySpecsPtr					displaySpecsPtr)

{  
	Boolean								continueFlag = FALSE; 
	INT_PTR								returnCode;

	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
	
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
		
		}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}		// end "DoDialog"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2002)
//								� Purdue Research Foundation
//									All rights reserved.
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

Boolean 
CMDisplaySpecsDlg::GetComputeHistogramDialogSetting (
			DisplaySpecsPtr			displaySpecsPtr,
			SInt16						lEnhancement,
			SInt16						lMinMaxCode)

{
	Boolean				setComputeItemFlag;
	
	
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
			
		}		// end "if ( !displaySpecsPtr->defaultHistogramFileFlag )"
		
	return (setComputeItemFlag); 
	
}		// end "GetComputeHistogramDialogSetting"  



//-----------------------------------------------------------------------------
//								 Copyright (1988-2002)
//								� Purdue Research Foundation
//									All rights reserved.
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
//	Coded By:			Larry L. Biehl			Date: 06/06/95
//	Revised By:			Larry L. Biehl			Date: 06/06/95	

UInt16 
CMDisplaySpecsDlg::GetMaximumDisplayLevels (
				SInt16						bitsOfColorIndex,          
				SInt16						displayType, 
				SInt16						duplicateChannelCode)

{                                         
	UInt16			numberChannelsIndex = 0;
	

	switch (displayType)
		{
		case 1:
		case 2:
		case 7:
			numberChannelsIndex = 0;                        
								
			break;
			
		case 3:
			
			if (duplicateChannelCode)                          
				numberChannelsIndex = 1;
				
			else                                                                      
				numberChannelsIndex = 2;
								
			break;
			
		case 4:                                                                   
			numberChannelsIndex = 1;
								
			break;
			
		}		// end "switch (displayType)"
	
	return (mColorLevelsMax[bitsOfColorIndex][numberChannelsIndex]);
	
}		// end "GetMaximumDisplayLevels" 



/////////////////////////////////////////////////////////////////////////////
// CMDisplaySpecsDlg message handlers

  

void 
CMDisplaySpecsDlg::OnChangeBlueChannel()
{  
	                                     
		  	// Get current blue channel
		  	
//	DDX_Text(m_dialogFromPtr, ID3C_BlueChannel, m_BlueChannel);
	CheckChannel(ID3C_BlueChannel, &m_BlueChannel);
	
}		// end "OnChangeBlueChannel"



void CMDisplaySpecsDlg::OnChangeGrayChannel()
{                                                           
	CheckChannel(ID3C_GrayChannel, &m_GrayChannel);
	
}		// end "OnChangeGrayChannel"
             

void CMDisplaySpecsDlg::OnChangeGreenChannel()
{                                        
		  	// Get current green channel
		  	
//	DDX_Text(m_dialogFromPtr, ID3C_GreenChannel, m_GreenChannel);
	CheckChannel(ID3C_GreenChannel, &m_GreenChannel);
	
}		// end "OnChangeGreenChannel"



void CMDisplaySpecsDlg::OnChangeRedChannel()
{                                         
		  	// Get current red channel
		  	
//	DDX_Text(m_dialogFromPtr, ID3C_RedChannel, m_RedChannel);
	CheckChannel(ID3C_RedChannel, &m_RedChannel);
	
}		// end "OnChangeRedChannel"



void 
CMDisplaySpecsDlg::OnChannelDescriptions()
 
{
			// Show channel list.							
								                         
	SetDLogControlHilite (NULL, IDOK, 255);  
	
	CMChannelsDlg*		channelsDialogPtr = NULL;

	SInt16				tempInt;
	
	TRY
		{                                
		channelsDialogPtr = new CMChannelsDlg();
			                                                                  
		channelsDialogPtr->DoDialog (
										&tempInt,
										NULL,
										gImageLayerInfoPtr,
										gImageFileInfoPtr,
										2,
										kNoTransformation,
										NULL,
										(SInt16)gImageWindowInfoPtr->totalNumberChannels,
										kSubsetMenuItem ); 
		
		if (channelsDialogPtr != NULL)                           
			delete channelsDialogPtr;
		}
		
	CATCH_ALL(e)
		{
		MemoryMessage(0, kObjectMessage);
		}
	END_CATCH_ALL 
		
	SetDLogControlHilite (NULL, IDOK, 0);
	
}		// end "OnChannelDescriptions"



void CMDisplaySpecsDlg::OnCloseupEnhancement() 
{
	if (m_optionKeyFlag)
		{     
		SetComboItemText (ID3C_Enhancement, 
									2, 
									(UCharPtr)"Gaussian");

		DDX_CBIndex(m_dialogToPtr, ID3C_Enhancement, m_Enhancement);

		m_optionKeyFlag = FALSE;

		}		// end "if (m_optionKeyFlag)"
	
}		// end "OnCloseupEnhancement"



void 
CMDisplaySpecsDlg::OnDropdownBitsOfColor(void)

{                                
	CComboBox* comboBoxPtr = (CComboBox*)GetDlgItem(ID3C_BitsOfColor);
	                                                          
	if (mLocalDisplayType == 1 || mLocalDisplayType == 2 || mLocalDisplayType == 7)
		{
		if (comboBoxPtr->GetCount() == 2) 
   			// Remove the '24' item from the bits of color list.
   		                                                    
			comboBoxPtr->DeleteString(1);
			
		}		// end "if (mLocalDisplayType == 1 || mLocalDisplayType == 2 ..."
		
	else		// mLocalDisplayType != 1 && mLocalDisplayType != 2 && ...
		{
		if (comboBoxPtr->GetCount() == 1)                      
			comboBoxPtr->AddString((LPCTSTR)_T("24"));
			
		}		// end "else mLocalDisplayType != 1 && ..."
	
}		// end "OnDropdownBitsOfColor"  



void CMDisplaySpecsDlg::OnDropdownEnhancement() 
{
	m_optionKeyFlag = FALSE;
	if (GetKeyState (VK_RBUTTON) < 0 || GetKeyState(VK_SHIFT) & 0x8000)
		{                                                                             
		SetComboItemText (ID3C_Enhancement, 
									2, 
									(UCharPtr)"Gaussian...");
								
		m_optionKeyFlag = TRUE;

		DDX_CBIndex(m_dialogToPtr, ID3C_Enhancement, m_Enhancement);
		
		}		// end "if (GetKeyState (VK_RBUTTON) < 0)"
	
}		// end "OnDropdownEnhancement"



BOOL 
CMDisplaySpecsDlg::OnInitDialog()
{                                
//	Boolean							continueFlag, 
//										userComputeFlag; 
										
//	DialogSelectArea				dialogSelectArea;
						
//	FileInfoPtr						fileInfoPtr;
	
//	SInt32							interval,
//										theNum;  
	                                            
//	WindowInfoPtr					windowInfoPtr;

	SInt16							bitsOfColor,
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
	
	
	CDialog::OnInitDialog(); 
	
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
	m_Magnification = atof((char*)&gTextString);
	
			// Make sure that we have the bitmaps for the entire image buttons.
		
	VERIFY(toEntireButton.AutoLoad(IDEntireImage, this));
	VERIFY(toSelectedButton.AutoLoad(IDSelectedImage, this));
	
	SetEntireImageButtons (
						NULL, 
						m_LineStart, 
						m_LineEnd, 
						m_ColumnStart, 
						m_ColumnEnd);
			
			// Set feature parameters 
			
	m_localActiveNumberFeatures =	m_localNumberFeatures; 
	m_localActiveFeaturesPtr = m_localFeaturesPtr;
														
	if (UpdateData(FALSE))
		PositionDialogWindow (); 
	
			// Set up edit text fields for channels to be used for diplay	
			
	UpdateDialogChannelItems (this, mRGBColors, mLocalDisplayType);
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"




void 
CMDisplaySpecsDlg::OnSelendokBitsOfColor()
{           
	SInt16							numberLevels;


			// Get current bits of color setting.
			
	DDX_CBIndex(m_dialogFromPtr, ID3C_BitsOfColor, m_BitsOfColor);
	
   		// Get index into 8, 16, 24 bits of color vector.
   		
//   mBitsOfColorIndex = Update8_16_24BitsOfColorIndex(m_BitsOfColor);
	
			// Save the bits of color index.

	m_SavedBitsOfColor = m_BitsOfColor;
			
			// Verify number of levels setting
	
//	CheckDisplayLevels (NULL, mLocalDisplayType);
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
	
}		// end "OnSelendokBitsOfColor"



void 
CMDisplaySpecsDlg::OnSelendokChannels(void)

{  
	HandleChannelsMenu(ID3C_Channels, 
								kNoTransformation,
								(SInt16)gImageWindowInfoPtr->totalNumberChannels,
								kImageChannelType,
								TRUE);
	
}		// end "OnSelendokChannels"
                           


void 
CMDisplaySpecsDlg::OnSelendokDisplayType(void)

{  
	SInt16*							colorLevelsMaxPtr;
	
	SInt32							blueChannel,
										greenChannel,
										redChannel;
										
	SInt16							bitsOfColor,
										currentDisplayTypeMenuSelection,
										minMaxPopupCode,
										numberDisplayLevels;
	
	Boolean							checkMinMaxSettingFlag;
	
	
	currentDisplayTypeMenuSelection = m_DisplayType + 1;
	                                 
	DDX_CBIndex(m_dialogFromPtr, ID3C_DisplayType, m_DisplayType);
	
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

	DDX_CBIndex(m_dialogToPtr, ID3C_BitsOfColor, m_BitsOfColor);
	
   		// Get index into 8, 16, 24 bits of color vector.
   
   mBitsOfColorIndex = Update8_16_24BitsOfColorIndex(m_BitsOfColor); 

			// Check min-max menu settings.

	minMaxPopupCode = m_MinMaxPopupCode + 1;
	DisplayMultispectralDialogCheckMinMaxSettings (m_DisplayType+1,
																	m_MinMaxPopupCode+1,
																	&minMaxPopupCode,
																	&m_MinMaxCode,
																	&m_thematicDisplayWithUserSettingFlag);

	m_MinMaxPopupCode = minMaxPopupCode - 1;                     
	DDX_CBIndex(m_dialogToPtr, ID3C_MinMaxValues, m_MinMaxPopupCode);

	DisplayMultispectralDialogUpdateComputeHistogram (
											m_displaySpecsPtr, 
											m_ComputeHistogramFlag,
											m_DisplayType+1,
											m_Enhancement+1,
											m_MinMaxPopupCode+1, 
											this, 
											ID3C_NewHistogram);
	
}		// end "OnSelendokDisplayType"



void CMDisplaySpecsDlg::OnSelendokEnhancement() 
{
	DDX_CBIndex(m_dialogFromPtr, ID3C_Enhancement, m_Enhancement);
	
	if (m_Enhancement == 2 && m_optionKeyFlag)
		{     
		SetDLogControlHilite (NULL, IDOK, 255);
	                        
		GaussianParameterDialog (&m_gaussianStretch);
															                                 
		SetDLogControlHilite (NULL, IDOK, 0);
		
		}		// end "if (m_localMinMaxCode == 2)" 

	DisplayMultispectralDialogUpdateComputeHistogram (
											m_displaySpecsPtr, 
											m_ComputeHistogramFlag,
											m_DisplayType+1,
											m_Enhancement+1,
											m_MinMaxPopupCode+1, 
											this, 
											ID3C_NewHistogram);
	
}		// end "OnSelendokEnhancement"



void CMDisplaySpecsDlg::OnSelendokMinMaxValues()
{         
	SInt16								channels[3];
											
	                                                 						                       
	DDX_CBIndex(m_dialogFromPtr, ID3C_MinMaxValues, m_MinMaxPopupCode);
	
	if (m_MinMaxPopupCode == 0)
		{
		m_MinMaxCode = kPerChannelMinMax;
		m_PercentClip = 0;
		
		}		// end "if (m_MinMaxPopupCode == 0)"
	
	else if (m_MinMaxPopupCode == 1)
		{
		m_MinMaxCode = kPerChannelMinMax;
		m_PercentClip = 2;
		
		}		// end "if (m_MinMaxPopupCode == 1)"
	
	else if (m_MinMaxPopupCode == 2) 
		m_MinMaxCode = kEntireDataRange;        
	
	else if (m_MinMaxPopupCode == 3)
		{                                                   
		SetDLogControlHilite (NULL, IDOK, 255);								
	                           
		channels[0] = (SInt16)m_RedChannel; 
		channels[1] = (SInt16)m_GreenChannel;
		channels[2] = (SInt16)m_BlueChannel; 
		
		if (mLocalDisplayType <= 2 )
			channels[0] = (SInt16)m_GrayChannel;
	                        
		MinMaxEnhancementDialog ( channels,
											mRGBColors,
											mLocalDisplayType,
											m_NumberLevels,
											&m_PercentClip, 
											&m_MinMaxCode, 
											mMinMaxValues );
											
		m_MinMaxPopupCode = GetMinMaxPopupCode(m_MinMaxCode, m_PercentClip);
		DDX_CBIndex(m_dialogToPtr, ID3C_MinMaxValues, m_MinMaxPopupCode);
															                                 
		SetDLogControlHilite (NULL, IDOK, 0);
		
		}		// end "if (m_localMinMaxCode == 3)" 
	
	else if (m_MinMaxPopupCode == 4) 
		m_MinMaxCode = kThematicDefault;  

	DisplayMultispectralDialogUpdateComputeHistogram (
											m_displaySpecsPtr, 
											m_ComputeHistogramFlag,
											m_DisplayType+1,
											m_Enhancement+1,
											m_MinMaxCode, 
											this, 
											ID3C_NewHistogram);      
	
}		// end "OnSelendokMinMaxValues"



/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-2002)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SetComputeHistogramDialogItem
//
//	Software purpose:	The purpose of this routine is to draw the enhancement
//							prompt and enhancement selection for the enhancement
//							pop up menu.
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

void 
CMDisplaySpecsDlg::SetComputeHistogramDialogItem ( 
			Boolean						computeItemFlag,
			DialogPtr					dialogPtr, 
			SInt16						itemNumber)

{                                         
		
	if (computeItemFlag)                      
		SetDLogControlHilite (dialogPtr, itemNumber, 255);
			
	else		// !computeItemFlag
		SetDLogControlHilite (dialogPtr, itemNumber, 0);
	
}		// end "SetComputeHistogramDialogItem" 
*/


/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-2002)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateDialogChannelItems
//
//	Software purpose:	The purpose of this routine is to update the dialog 
//							channel items in the multispectral display dialog.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/24/93
//	Revised By:			Larry L. Biehl			Date: 06/09/95	

void 
CMDisplaySpecsDlg::UpdateDialogChannelItems (
				DialogPtr				dialogPtr, 
				SInt16					rgbColors, 
				SInt16					displayType)

{
	SInt16				hiliteItem;
	
	
			// text fields for channels to be displayed								
			
			// Side by side channel display.												
								
	ShowHideDialogItem ( NULL, ID3C_Channels, (displayType == 7) );
//	GetDlgItem(ID3C_Channels)->EnableWindow(FALSE);
			
			// 1-channel display channel number											
	
	ShowHideDialogItem ( NULL, ID3C_GrayChannel, (rgbColors & 8) );
	ShowHideDialogItem ( NULL, ID3C_STATIC35, (rgbColors & 8) );
	if (displayType == 7)
		MHideDialogItem (NULL, ID3C_GrayChannel);
		
			// red channel number															
				
	ShowHideDialogItem ( NULL, ID3C_STATIC28, (rgbColors & 4) );
	ShowHideDialogItem ( NULL, ID3C_RedChannel, (rgbColors & 4) );
		
			// green channel number															
				
	ShowHideDialogItem ( NULL, ID3C_STATIC29, (rgbColors & 2) );
	ShowHideDialogItem ( NULL, ID3C_GreenChannel, (rgbColors & 2) );
		
			// blue channel number															
				
	ShowHideDialogItem ( NULL, ID3C_STATIC30, (rgbColors & 1) );
	ShowHideDialogItem ( NULL, ID3C_BlueChannel, (rgbColors & 1) );
	
			// Set intial focus (hilite) to the appropriate edit text item
	
	if (m_DisplayType+2 == 7)
		hiliteItem = IDC_LineStart;
		
	else		// m_DisplayType+2 != 7
		{
		hiliteItem = ID3C_GrayChannel;
		
		if (m_DisplayType+2 > 2)
			hiliteItem = ID3C_RedChannel; 
		
		if (m_DisplayType+2 == 4)
			hiliteItem = ID3C_GreenChannel;
		
		}		// end "else m_DisplayType+2 != 7"
		                                       
//	GetDlgItem(hiliteItem)->SetFocus();
//	SendDlgItemMessage( hiliteItem, EM_SETSEL, 0, MAKELPARAM(0, -1) );
	SelectDialogItemText (this, hiliteItem, 0, SInt16_MAX);
	
}		// end "UpdateDialogChannelItems"
*/

/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-2002)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateDisplayLevels
//
//	Software purpose:	The purpose of this routine is to update the display
//							levels dialog item in the multispectral display dialog.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/24/93
//	Revised By:			Larry L. Biehl			Date: 06/01/95	

void 
CMDisplaySpecsDlg::UpdateDisplayLevels (
				DialogPtr					dialogPtr,          
				SInt16						displayType, 
				SInt16						duplicateChannelCode)

{                                         
	m_NumberLevels = GetMaximumDisplayLevels (
							mBitsOfColorIndex,          
							displayType, 
							duplicateChannelCode);
							
	DDX_Text(m_dialogToPtr, ID3C_NumberOfLevels, m_NumberLevels);
	
}		// end "UpdateDisplayLevels"
*/


SInt16 
CMDisplaySpecsDlg::Update8_16_24BitsOfColorIndex (
				SInt16			menuBitsOfColorIndex)

{	
   		// Get index into 8, 16, 24 bits of color vector.
   		
   SInt16 bitsOfColorIndex = menuBitsOfColorIndex;
   if (bitsOfColorIndex == 1)
   	bitsOfColorIndex = 2;
   	
   return (bitsOfColorIndex); 
	
}		// end "Update8_16_24BitsOfColorIndex"
