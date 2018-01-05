// WReformatTransformDialog.cpp : implementation file
//      
// Revised by Larry Biehl on 08/21/2017
//

//#include <afxpriv.h>
#include "SMultiSpec.h" 

#include "WReformatTransformDialog.h"
#include	"WChannelsDialog.h"

//#include	"SExtGlob.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern Boolean		GetDefaultBandRatio (
							WindowInfoPtr						windowInfoPtr,
							FileInfoPtr							fileInfoPtr,
							ReformatOptionsPtr				reformatOptionsPtr);

/////////////////////////////////////////////////////////////////////////////
// CMReformatTransform dialog


CMReformatTransformDlg::CMReformatTransformDlg(CWnd* pParent /*=NULL*/)
	: CMDialog(CMReformatTransformDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMReformatTransform)
	m_adjustDivisor = 0;
	m_adjustFactor = 0;
	m_adjustOffset = 0;
	m_transformFactor = 0;
	m_transformOffset = 0;
	m_denominatorString = "";
	m_numeratorString = "";
	m_scaleFactor = 0;
	m_channelSelection = -1;
	m_minimumNumberBits = 1;
	m_minSelectedNumberBits = 8;
	m_transformCode = -1;
	m_functionFactor = 1;
	m_kthSmallestElement = 1;
	m_functionCode = -1;
	m_adjustSelectedChannelsFactor = -1.;
	m_adjustSelectedChannel = 1;
	//}}AFX_DATA_INIT 	
	
	m_maxAdjustOffset = SInt16_MAX;
	m_maxAdjustFactor = SInt16_MAX;
	m_maxAdjustDivisor = SInt16_MAX;
	
	m_maxValue = SInt16_MAX;
	m_minValue = SInt16_MAX;
	
	m_denominatorStringPtr = NULL;
	m_numeratorStringPtr = NULL;
	
	m_initializedFlag = CMDialog::m_initializedFlag;
		
	if (m_initializedFlag)                                                              
		{
		TRY
			{																			
					//	Set the algebraic transform descriptions.	
			
			m_denominatorStringPtr = (TBYTE*)m_denominatorString.GetBufferSetLength(256);
			m_numeratorStringPtr = (TBYTE*)m_numeratorString.GetBufferSetLength(256);
				
			}
			
		CATCH_ALL(e)
			{
			m_initializedFlag = FALSE;
			
			}
		END_CATCH_ALL
		
		}		// end "if (m_initializedFlag)"
	
}		// end "CMReformatTransformDlg"



CMReformatTransformDlg::~CMReformatTransformDlg()

{                   
	if (m_denominatorStringPtr != NULL)
		m_denominatorString.ReleaseBuffer();
	                      
	if (m_numeratorStringPtr != NULL)     
		m_numeratorString.ReleaseBuffer();

}		// end "~CMReformatTransformDlg"



void CMReformatTransformDlg::DoDataExchange(CDataExchange* pDX)
{
	#if defined _UNICODE
		USES_CONVERSION; 
	#endif	

	UInt8								string[256];

	SInt16							item,
										numberTerms;

	UInt16							stringID;

	Boolean							algebraicItemFlag,
										eigenvectorItemFlag;
    

	CDialog::DoDataExchange(pDX);

	// If we are reading values from the dialog make sure that all
	// of the radio buttons are showing.

	if (pDX->m_bSaveAndValidate)
		{        
		eigenvectorItemFlag = 
			GetDlgItem(IDC_RT_Eigenvectors)->IsWindowVisible ();

		algebraicItemFlag = 
			GetDlgItem(IDC_RT_AlgebraicTransformation)->IsWindowVisible ();

		MShowDialogItem (this, IDC_RT_Eigenvectors); 
		MShowDialogItem (this, IDC_RT_AlgebraicTransformation);
		MShowDialogItem (this, IDC_RT_FunctionOfChannels);

		}		// end "if (pDX->m_bSaveAndValidate)"

	//{{AFX_DATA_MAP(CMReformatTransformDlg)
	DDX_Text2(pDX, IDC_RT_AC_Divisor, m_adjustDivisor);
	//	DDV_MinMaxDouble(pDX, m_adjustDivisor, 0, 65535);
	DDX_Text2(pDX, IDC_RT_AC_Factor, m_adjustFactor);
	//	DDV_MinMaxDouble(pDX, m_adjustFactor, 0, 65535);
	DDX_Text2(pDX, IDC_RT_AC_Offset, m_adjustOffset);
	//	DDV_MinMaxDouble(pDX, m_adjustOffset, -32767, 32767);
	DDX_Text2(pDX, IDC_RT_AT_Factor, m_transformFactor);
	DDV_MinMaxDouble(pDX, m_transformFactor, -FLT_MAX, FLT_MAX);
	DDX_Text2(pDX, IDC_RT_AT_Offset, m_transformOffset);
	DDV_MinMaxDouble(pDX, m_transformOffset, -FLT_MAX, FLT_MAX);
	DDX_Text(pDX, IDC_RT_AT_Denominator, m_denominatorString);
	DDV_MaxChars(pDX, m_denominatorString, 255);
	DDX_Text(pDX, IDC_RT_AT_Numerator, m_numeratorString);
	DDV_MaxChars(pDX, m_numeratorString, 255);
	DDX_Text2(pDX, IDC_EV_ScaleFactor, m_scaleFactor);
	DDV_MinMaxDouble(pDX, m_scaleFactor, 0, FLT_MAX);
	DDX_CBIndex(pDX, IDC_EV_Eigenvectors, m_channelSelection);
	DDX_Text(pDX, IDC_RT_EV_AllComponents, m_minimumNumberBits);
	DDX_Text(pDX, IDC_RT_EV_SelComponents, m_minSelectedNumberBits);
//	DDX_Radio(pDX, IDC_AdjustSelectedChannels, m_transformCode);
	DDX_Radio(pDX, IDC_RT_NoTransformation, m_transformCode);
	DDX_CBIndex(pDX, IDC_ReformatFunctions, m_functionCode);
	DDX_Text2(pDX, IDC_FunctionFactor, m_functionFactor);
	DDV_MinMaxDouble(pDX, m_functionFactor, -FLT_MAX, FLT_MAX);
	DDX_Text(pDX, IDC_kthSmallestElement, m_kthSmallestElement);
	DDV_MinMaxUInt(pDX, m_kthSmallestElement, 1, 1000);
	DDX_Text2(pDX, IDC_RT_ACbyC_Factor, m_adjustSelectedChannelsFactor);
	DDV_MinMaxDouble(pDX, m_adjustSelectedChannelsFactor, -FLT_MAX, FLT_MAX);
	DDX_Text(pDX, IDC_RT_ACbyC_ChannelNumber, m_adjustSelectedChannel);
	DDV_MinMaxUInt(pDX, m_adjustSelectedChannel, 1, m_maxChannel);

	if (pDX->m_bSaveAndValidate && m_transformCode == kTransformChannels)
		{
				// Verify that the parameter string does not include channels
				// that do not exist in the image file.
				// Decode the denominator.		

		item = 0;
		
		#if defined _UNICODE
			strcpy ((char*)string, T2A(m_denominatorStringPtr)); 
		#endif                                 
		#if !defined _UNICODE
			strcpy ((char*)string, (const char*)m_denominatorStringPtr); 
		#endif   
		CtoPstring (string, 
							gReformatOptionsPtr->denominatorString);

		numberTerms = DecodeAlgebraicFormula (
			(unsigned char*)gReformatOptionsPtr->denominatorString, 
			gReformatOptionsPtr->coefficientsPtr, 
			gReformatOptionsPtr->transformChannelPtr, 
			gReformatOptionsPtr->transformOperatorPtr);

		if (numberTerms >= 0)
			gReformatOptionsPtr->numberDenominatorTerms = numberTerms;
		else		// numberTerms < 0)
			item = IDC_RT_AT_Denominator;

		// Decode the numerator.								

		if (item == 0)
			{
			#if defined _UNICODE
				strcpy ((char*)string, T2A(m_numeratorStringPtr)); 
			#endif                                 
			#if !defined _UNICODE
				strcpy ((char*)string, (const char*)m_numeratorStringPtr); 
			#endif   	             
			CtoPstring (string, 
								gReformatOptionsPtr->numeratorString);

			numberTerms = DecodeAlgebraicFormula (
				(unsigned char*)gReformatOptionsPtr->numeratorString, 
				&gReformatOptionsPtr->coefficientsPtr[
					gReformatOptionsPtr->numberDenominatorTerms], 
						&gReformatOptionsPtr->transformChannelPtr[
							gReformatOptionsPtr->numberDenominatorTerms], 
								&gReformatOptionsPtr->transformOperatorPtr[
									gReformatOptionsPtr->numberDenominatorTerms]);

									if (numberTerms >= 0)
										gReformatOptionsPtr->numberNumeratorTerms = numberTerms;
									else		// numberTerms < 0)
										item = IDC_RT_AT_Numerator;

			}		// end "if (item == 0)" 

		if (item > 0)
			{
					// One of the parameters is wrong. Select the field that is
					// wrong and allow the user to correct it.

			if (!eigenvectorItemFlag)
				MHideDialogItem (this, IDC_RT_Eigenvectors); 

			if (!algebraicItemFlag)
				MHideDialogItem (this, IDC_RT_AlgebraicTransformation);

			stringID = IDS_Alert118 + abs(numberTerms) - 1;

			DisplayAlert (kErrorAlert2ID, 
				kStopAlert,
				kAlertStrID,
				stringID,
				0,
				NIL);

			pDX->PrepareEditCtrl (item);
			pDX->Fail();

			}		// end "if (item > 0)" 

		}		// end "if (pDX->m_bSaveAndValidate && m_transformCode == kTransformChannels)"

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMReformatTransformDlg, CMDialog)
	//{{AFX_MSG_MAP(CMReformatTransformDlg)
	ON_BN_CLICKED(IDC_AdjustSelectedChannels, OnAdjustSelectedChannels)
	ON_BN_CLICKED(IDC_RT_AlgebraicTransformation, OnRTAlgebraicTransformation)
	ON_BN_CLICKED(IDC_RT_Eigenvectors, OnRTEigenvectors)
	ON_BN_CLICKED(IDC_RT_FunctionOfChannels, OnRTFunctionOfChannels)
	ON_BN_CLICKED(IDC_RT_NoTransformation, OnRTNoTransformation)
	ON_CBN_SELENDOK(IDC_EV_Eigenvectors, OnSelendokEVEigenvectors)
	ON_CBN_SELENDOK(IDC_ReformatFunctions, OnSelendokReformatFunctions)
	ON_BN_CLICKED(IDC_AdjustSelectedChannelsbyChannel, OnAdjustSelectedChannelsByChannel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP() 


//-----------------------------------------------------------------------------
//								 Copyright (1988-2013)
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
//	Coded By:			Larry L. Biehl			Date: 10/19/1995
//	Revised By:			Larry L. Biehl			Date: 03/09/2013	

Boolean 
CMReformatTransformDlg::DoDialog(
				UInt16*			recommendNumberOfBitsPtr,
				SInt16			bandInterleaveSelection)

{  
	Boolean			OKFlag = FALSE; 
	
	// oul: changed from SInt16 to SInt64
	SInt64			returnCode;
	
	UInt16			item;
	  
	
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																			return(FALSE);
																			
	m_bandInterleaveSelection = bandInterleaveSelection;             
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{            
				// Get transform data code.									
 
		gReformatOptionsPtr->transformDataCode = m_transformCode;
			
				// Number to offset data values by.						
						
		gReformatOptionsPtr->adjustOffset = m_adjustOffset;
			
				// Number to multiply data values by.					
						
		gReformatOptionsPtr->adjustFactor = m_adjustFactor;
			
				// Number to divide data values by.						
						
		gReformatOptionsPtr->adjustDivisor = m_adjustDivisor;
				
				// Output data can be signed flag.									
				// Only valid for transform code of 1.								
				
//		if (gReformatOptionsPtr->transformDataCode == kAdjustChannel)		
//			gReformatOptionsPtr->signedOutputDataFlag = m_signedOutputDataFlag;
																
//		else		// gReformatOptionsPtr->transformDataCode != kAdjustChannel 	
//			gReformatOptionsPtr->signedOutputDataFlag = FALSE;
				
				// Get the list of principal component channels to create	
				// if transform code 'kCreatePCImage' has been selected.							
						
		if (gReformatOptionsPtr->transformDataCode == kCreatePCImage)
			{
			gReformatOptionsPtr->pcScaleFactor = m_scaleFactor;
			gReformatOptionsPtr->numberPCChannels = m_localActiveNumberFeatures;
					
			if (m_channelSelection == kAllMenuItem)
				{
				gReformatOptionsPtr->numberPCChannels = m_numberEigenvectors;
				for ( item=0; item<m_numberEigenvectors; item++)
					gReformatOptionsPtr->pcChannelPtr[item] = item;
								
				}		// end "if (m_channelSelection == kAllMenuItem)" 
						
			}		// end "if (...->transformDataCode == kCreatePCImage)" 
				
				// Decode the numerator and denominator if transform code	
				// 2 has been selected.													
						
		if (gReformatOptionsPtr->transformDataCode == kTransformChannels)
			{
			item = 0;
					
					// Transform offset.										
						
			gReformatOptionsPtr->transformOffset = m_transformOffset;
				
					// Decode the denominator.
					// Decode the numerator.
					//		These have already been decoded in "DoDataExchange".
							
					// Estimate recommendatioin for minimum number bits needed.
					
			if (gReformatOptionsPtr->numberDenominatorTerms > 0)
				{
				if (m_transformFactor <= 10)
							// This implies 32-bit real
					m_minSelectedNumberBits = 32;
				
				else if (m_transformFactor <= 100)
					m_minSelectedNumberBits = 8;
				
				else
					m_minSelectedNumberBits = 16;
					
				}		// end "if (...->numberDenominatorTerms > 0)"

					// Transform factor.										
						
			gReformatOptionsPtr->transformFactor = m_transformFactor;

			gReformatOptionsPtr->transformChannelPtrSetFlag = FALSE;
					
			}		// end "if (...->transformDataCode == kTransformChannels)"													
						
		if (gReformatOptionsPtr->transformDataCode == kFunctionOfChannels)
			{
					// Get info for function of channels.										
						
			gReformatOptionsPtr->functionCode = m_functionCode + 1;
			gReformatOptionsPtr->functionFactor = m_functionFactor;
			gReformatOptionsPtr->kthSmallestValue = m_kthSmallestElement;
					
			}		// end "if (...->transformDataCode == kFunctionOfChannels)" 												
						
		if (gReformatOptionsPtr->transformDataCode == kAdjustChannelsByChannel)
			{
					// Get info for adjusting selected channels by a channel										
						
			gReformatOptionsPtr->transformAdjustSelectedChannelsFactor = m_adjustSelectedChannelsFactor;
			gReformatOptionsPtr->transformAdjustSelectedChannel = m_adjustSelectedChannel;
					
			}		// end "if (...->transformDataCode == kFunctionOfChannels)" 
					
		gReformatOptionsPtr->eigenSource = m_eigenSource;
				
		*recommendNumberOfBitsPtr = m_minSelectedNumberBits;                          
			
		OKFlag = TRUE; 
		
		}		// end "if (returnCode == IDOK)"
		
	return (OKFlag);
		
}		// end "DoDialog"



/////////////////////////////////////////////////////////////////////////////
// CMReformatTransform message handlers

BOOL CMReformatTransformDlg::OnInitDialog()
				
{     
	#if defined _UNICODE
		USES_CONVERSION; 
	#endif

	char				string[256];

	CMDialog*		dialogPtr = this; 
	
	UInt16			selectedItem;
	
	
	CMDialog::OnInitDialog(); 

	m_maxChannel = gImageWindowInfoPtr->totalNumberChannels;
	
			// Adjust data radio button.											
			// Hide related items until the box is checked.
	
	selectedItem = 0;

	m_transformCode = gReformatOptionsPtr->transformDataCode; 
 
			// Adjust Selected Channels

	if (m_transformCode != kAdjustChannel)
		ShowHideAdjustChannelItems(dialogPtr, kHide); 
		
	else		// m_transformCode == kAdjustChannel 
		selectedItem = IDC_RT_AC_Offset;
	                                                        
	m_adjustOffset = gReformatOptionsPtr->adjustOffset; 	                                                        
	m_adjustFactor = gReformatOptionsPtr->adjustFactor; 	                                                          
	m_adjustDivisor = gReformatOptionsPtr->adjustDivisor;
 
			// Adjust Selected Channels by a Selected Channel

	if (m_transformCode != kAdjustChannelsByChannel)
		ShowHideAdjustSelectedChannelsByChannelItems (dialogPtr, kHide); 
		
	else		// m_transformCode == kAdjustChannel 
		selectedItem = IDC_RT_ACbyC_Factor;									
						
	m_adjustSelectedChannelsFactor = gReformatOptionsPtr->transformAdjustSelectedChannelsFactor;
	m_adjustSelectedChannel = gReformatOptionsPtr->transformAdjustSelectedChannel;
	
			// Use transformation radio button.		
	
	EigenvectorInfoExists (kNoProject, &m_eigenSource, &m_numberEigenvectors);
	
	m_scaleFactor = gReformatOptionsPtr->pcScaleFactor;
	if (m_numberEigenvectors > 0 && m_bandInterleaveSelection != kBSQ)
		{                        
		if (m_eigenSource == 1)
			MHideDialogItem (dialogPtr, IDC_RT_EV_FEEigenvectors);	
			
		if (m_eigenSource == 2)
			MHideDialogItem (dialogPtr, IDC_RT_EV_PCEigenvectors);
				
		if (m_transformCode != kCreatePCImage) 
			ShowHidePCTransformItems(dialogPtr, kHide, FALSE); 
			
		else		// m_transformCode == kCreatePCImage 
			selectedItem = IDC_EV_ScaleFactor;
			
//		if (m_eigenSource == 2)
//			m_scaleFactor = 10;
		
		m_minimumNumberBits = DeterminePCMinimumNumberBits (
														m_scaleFactor, m_eigenSource, 0);
		
		m_minSelectedNumberBits = DeterminePCMinimumNumberBits ( 
				m_scaleFactor, m_eigenSource, gReformatOptionsPtr->pcChannelPtr[0] );
			
		}		// end "if ( m_numberEigenvectors > 0 && ...)" 
		
	else		// m_numberEigenvectors <= 0 || m_bandInterleaveSelection == kBSQ
		ShowHidePCTransformItems(dialogPtr, kHide, TRUE);
		                     
	m_channelSelection = kAllMenuItem;
	m_localNumberFeatures = gReformatOptionsPtr->numberPCChannels;
	
	if (m_localNumberFeatures < m_numberEigenvectors) 
		m_channelSelection = kSubsetMenuItem;
		                                     
	m_localFeaturesPtr = (UInt16*)gReformatOptionsPtr->pcChannelPtr;  
	
			// Set feature parameters 
			
	m_localActiveNumberFeatures =	m_localNumberFeatures; 
	m_localActiveFeaturesPtr = m_localFeaturesPtr;
	
			// Use general algebraic transformation radio button.					
			
	if (gImageFileInfoPtr->thematicType)
		MHideDialogItem (dialogPtr, IDC_RT_AlgebraicTransformation);
	                          
	if (m_transformCode != kTransformChannels)
		ShowHideAlgebraicTransformItems(dialogPtr, kHide);
		
	else		// m_transformCode == kTransformChannels 
		selectedItem = IDC_RT_AT_Offset;
		
	if (!GetDefaultBandRatio (gImageWindowInfoPtr, gImageFileInfoPtr, gReformatOptionsPtr))
		{
		gReformatOptionsPtr->transformFactor = 1.;
		gReformatOptionsPtr->transformOffset = 0.;
		
		}		// end "if (!GetDefaultBandRatio (gImageWindowInfoPtr, ..."
		                                                               
	m_transformOffset = gReformatOptionsPtr->transformOffset;

	strncpy(string, 
				(CharPtr)&gReformatOptionsPtr->numeratorString[1],
				gReformatOptionsPtr->numeratorString[0]);  
	string[gReformatOptionsPtr->numeratorString[0]] = 0;
	#if defined _UNICODE
		_tcscpy (m_numeratorStringPtr, A2T(string)); 
	#endif	    
	#if !defined _UNICODE
		strcpy ((char*)m_numeratorStringPtr, string); 
	#endif	
	                                                                                 
	strncpy(string, 
				(CharPtr)&gReformatOptionsPtr->denominatorString[1],
				gReformatOptionsPtr->denominatorString[0]);  
	string[gReformatOptionsPtr->denominatorString[0]] = 0;
	#if defined _UNICODE
		_tcscpy (m_denominatorStringPtr, A2T(string)); 
	#endif	    
	#if !defined _UNICODE
		strcpy ((char*)m_denominatorStringPtr, string); 
	#endif	
	                                                                   
	m_transformFactor = gReformatOptionsPtr->transformFactor;

			// Function of channels items

	m_functionCode = gReformatOptionsPtr->functionCode - 1;
	m_functionFactor = gReformatOptionsPtr->functionFactor;
	m_kthSmallestElement = gReformatOptionsPtr->kthSmallestValue;

	ShowHideFunctionChannelsItems (dialogPtr, 
												m_transformCode==kFunctionOfChannels, 
												gReformatOptionsPtr->functionCode);

	if (m_transformCode == kFunctionOfChannels)
		{
		selectedItem = IDC_FunctionFactor;
		if (m_functionCode == kFunctionKthSmallestElement - 1)
			selectedItem = IDC_kthSmallestElement;

		}		// end "if (m_transformCode == kFunctionOfChannels)"

			// Make certain that all radio buttons are showing for now.
			 
	MShowDialogItem (dialogPtr, IDC_AdjustSelectedChannelsbyChannel);
	MShowDialogItem (dialogPtr, IDC_RT_Eigenvectors);   
	MShowDialogItem (dialogPtr, IDC_RT_AlgebraicTransformation);  
	MShowDialogItem (dialogPtr, IDC_RT_FunctionOfChannels);
	
	if (UpdateData(FALSE) ) 
		PositionDialogWindow ();
		
			// Update the radio buttons.
			
	if ( m_numberEigenvectors <= 0 || m_bandInterleaveSelection == kBSQ) 
		MHideDialogItem (dialogPtr, IDC_RT_Eigenvectors); 
	
	if (gImageFileInfoPtr->thematicType)
		MHideDialogItem (dialogPtr, IDC_RT_AlgebraicTransformation); 
		
			// Set default text selection to first edit text item 
	
	if (selectedItem > 0)
		SelectDialogItemText (this, selectedItem, 0, SInt16_MAX); 
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"
                          
               
               
void 
CMReformatTransformDlg::ShowHideFunctionChannelsItems(
				DialogPtr					dialogPtr,
				Boolean						showFlag,
				UInt16						functionChannelCode)
							
{           	 
	ShowHideDialogItem (dialogPtr, IDC_ReformatFunctions, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_FunctionFactorPrompt, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_FunctionFactor, showFlag); 

	if (functionChannelCode != kFunctionKthSmallestElement)
		showFlag = FALSE;

	ShowHideDialogItem (dialogPtr, IDC_kthSmallestElementPrompt, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_kthSmallestElement, showFlag); 
			
}		// end "ShowHideFunctionChannelsItems"  
                          
               
               
void 
CMReformatTransformDlg::ShowHidePCTransformItems(
				DialogPtr      dialogPtr,
				Boolean			showFlag,
				Boolean			pcButtonFlag)
							
{       
   if (pcButtonFlag)
   	{
		ShowHideDialogItem (dialogPtr, IDC_RT_Eigenvectors, showFlag);
		ShowHideDialogItem (dialogPtr, IDC_RT_EV_FEEigenvectors, showFlag);
		ShowHideDialogItem (dialogPtr, IDC_RT_EV_PCEigenvectors, showFlag);
   	
   	}		// end "if (pcButtonFlag)"
   	
	ShowHideDialogItem (dialogPtr, IDC_EV_Eigenvectors, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_EV_NumberBits, showFlag); 
	ShowHideDialogItem (dialogPtr, IDC_RT_EV_AllComponents, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_EV_SelComponents, showFlag);   
	ShowHideDialogItem (dialogPtr, IDC_ScaleFactorPrompt, showFlag);          
	ShowHideDialogItem (dialogPtr, IDC_EV_ScaleFactor, showFlag); 
			
}		// end "ShowHidePCTransformItems"  
                          
               
               
void 
CMReformatTransformDlg::ShowHideAdjustChannelItems(
				DialogPtr      dialogPtr,
				Boolean			showFlag)
							
{       
	ShowHideDialogItem (dialogPtr, IDC_RT_AC_Equal, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AC_Offset, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AC_Plus1, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AC_Factor, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AC_OldValue, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AC_Divisor, showFlag);
			
}		// end "ShowHideAdjustChannelItems"  
                          
               
               
void 
CMReformatTransformDlg::ShowHideAdjustSelectedChannelsByChannelItems(
				DialogPtr      dialogPtr,
				Boolean			showFlag)
							
{       
	ShowHideDialogItem (dialogPtr, IDC_RT_ACbyC_OldValue, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_ACbyC_Factor, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_ACbyC_ChannelPrompt, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_ACbyC_ChannelNumber, showFlag);
			
}		// end "ShowHideAdjustSelectedChannelsByChannelItems"     
                          
               
               
void 
CMReformatTransformDlg::ShowHideAlgebraicTransformItems(
				DialogPtr      dialogPtr,
				Boolean			showFlag)
							
{                                         
	ShowHideDialogItem (dialogPtr, IDC_RT_AT_Equal, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AT_Offset, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AT_Plus, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AT_Numerator, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AT_Line, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AT_Denominator, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AT_Multiply, showFlag);
	ShowHideDialogItem (dialogPtr, IDC_RT_AT_Factor, showFlag);  
			
}		// end "ShowHideAlgebraicTransformItems"

   

void CMReformatTransformDlg::OnAdjustSelectedChannels()
{  
	DialogPtr			dialogPtr = this;
	                                                        
   
   ShowHideAdjustChannelItems (dialogPtr, kShow);
	ShowHideAdjustSelectedChannelsByChannelItems (dialogPtr, kHide);
   ShowHidePCTransformItems (dialogPtr, kHide, FALSE);
	ShowHideAlgebraicTransformItems (dialogPtr, kHide);
	ShowHideFunctionChannelsItems (dialogPtr, kHide, 0);
		
		// Set default text selection to first edit text item 
	
	SelectDialogItemText (this, IDC_RT_AC_Offset, 0, SInt16_MAX); 
	
}		// end "OnAdjustSelectedChannels"



void CMReformatTransformDlg::OnRTAlgebraicTransformation()
{                                                          
	DialogPtr			dialogPtr = this;
	                                                        
   
   ShowHideAdjustChannelItems (dialogPtr, kHide);
	ShowHideAdjustSelectedChannelsByChannelItems (dialogPtr, kHide);
   ShowHidePCTransformItems (dialogPtr, kHide, FALSE);
	ShowHideAlgebraicTransformItems(dialogPtr, kShow);
	ShowHideFunctionChannelsItems (dialogPtr, kHide, 0);
		
		// Set default text selection to first edit text item 
	
	SelectDialogItemText (this, IDC_RT_AT_Offset, 0, SInt16_MAX); 
	
}		// end "OnRTAlgebraicTransformation"



void CMReformatTransformDlg::OnRTEigenvectors()
{                                                
	DialogPtr			dialogPtr = this;                              
	                                                        
   
   ShowHideAdjustChannelItems (dialogPtr, kHide);
	ShowHideAdjustSelectedChannelsByChannelItems (dialogPtr, kHide);
   ShowHidePCTransformItems (dialogPtr, kShow, TRUE);
	ShowHideAlgebraicTransformItems(dialogPtr, kHide);
	ShowHideFunctionChannelsItems (dialogPtr, kHide, 0);
		
		// Set default text selection to first edit text item 
	
	SelectDialogItemText (this, IDC_EV_ScaleFactor, 0, SInt16_MAX); 
	
}		// end "OnRTEigenvectors" 



void CMReformatTransformDlg::OnRTNoTransformation()
{                                                                     
	DialogPtr			dialogPtr = this;                              
	                                                        
   
   ShowHideAdjustChannelItems (dialogPtr, kHide);
	ShowHideAdjustSelectedChannelsByChannelItems (dialogPtr, kHide);
   ShowHidePCTransformItems (dialogPtr, kHide, FALSE);
	ShowHideAlgebraicTransformItems(dialogPtr, kHide);
	ShowHideFunctionChannelsItems (dialogPtr, kHide, 0);
	
}		// end "OnRTNoTransformation"



void CMReformatTransformDlg::OnSelendokEVEigenvectors()
{                                                                                                                        
	HandleChannelsMenu(IDC_EV_Eigenvectors, 
								kUseTransformation,
								(SInt16)gReformatOptionsPtr->numberPCChannels,
								2,
								TRUE);
	
}		// end "OnSelendokEVEigenvectors"



void CMReformatTransformDlg::OnRTFunctionOfChannels()
{      
	DialogPtr			dialogPtr = this;                              
	                                                        
   
   ShowHideAdjustChannelItems (dialogPtr, kHide);
	ShowHideAdjustSelectedChannelsByChannelItems (dialogPtr, kHide);
   ShowHidePCTransformItems (dialogPtr, kHide, TRUE);
	ShowHideAlgebraicTransformItems(dialogPtr, kHide);
	ShowHideFunctionChannelsItems (dialogPtr, kShow, m_functionCode+1);
		
		// Set default text selection to first edit text item 
	
	SelectDialogItemText (this, IDC_FunctionFactor, 0, SInt16_MAX);

}		// end "OnRTFunctionOfChannels"



void CMReformatTransformDlg::OnSelendokReformatFunctions()
{
	Boolean					showFlag = FALSE;


	DDX_CBIndex (m_dialogFromPtr, IDC_ReformatFunctions, m_functionCode);
	
	if (m_functionCode == kFunctionKthSmallestElement-1)
		showFlag = TRUE;

	ShowHideDialogItem (this, IDC_kthSmallestElementPrompt, showFlag);
	ShowHideDialogItem (this, IDC_kthSmallestElement, showFlag); 

}		// end "OnSelendokReformatFunctions"

void CMReformatTransformDlg::OnAdjustSelectedChannelsByChannel()
{
	DialogPtr			dialogPtr = this;                              
	                                                        
   
   ShowHideAdjustChannelItems (dialogPtr, kHide);
	ShowHideAdjustSelectedChannelsByChannelItems (dialogPtr, kShow);
   ShowHidePCTransformItems (dialogPtr, kHide, TRUE);
	ShowHideAlgebraicTransformItems(dialogPtr, kHide);
	ShowHideFunctionChannelsItems (dialogPtr, kHide, 0);
		
		// Set default text selection to first edit text item 
	
	SelectDialogItemText (this, IDC_RT_ACbyC_Factor, 0, SInt16_MAX);

}		// end "OnBnClickedAdjustSelectedChannelsByChannel"
