// WDisplayMinMaxDialog.cpp : implementation file
//   
// Revised by Larry Biehl on 12/21/2017
//
                   
#include "SMultiSpec.h"               
#include "WDisplayMinMaxDialog.h"
//#include "SExtGlob.h" 

extern void 			MinMaxEnhancementDialogInitialize (
								DialogPtr							dialogPtr,
								SInt16*								channelsPtr,
								SInt16								displayType,
								SInt16								numberLevels,
								SInt16								rgbColors,
								SInt16								minMaxSelection,
								double*								minMaxValuesPtr, 
								HistogramSpecsPtr*				histogramSpecsPtrPtr,
								HistogramSummaryPtr*				histogramSummaryPtrPtr,
								double*								minValuePtr,
								double*								maxValuePtr,
								SInt16*								localMinMaxCodePtr,
								SInt16*								percentTailsClippedPtr,
								Boolean*								histogramAvailableFlagPtr );

extern void 			MinMaxEnhancementDialogOK (
								HistogramSummaryPtr				histogramSummaryPtr,
								SInt16*								channelsPtr,
								double*								minMaxValuesPtr,
								SInt16*								minMaxSelectionPtr,
								SInt16*								percentTailsClippedPtr,
								double								redMinValue,
								double								redMaxValue,
								double								greenMinValue,
								double								greenMaxValue,
								double								blueMinValue,
								double								blueMaxValue,
								SInt16								localMinMaxCode,
								SInt16								percentTailsClipped );
													
extern void 			UpdateEnhancementMinMaxes (
								HistogramSpecsPtr 				histogramSpecsPtr,
								DialogPtr 							dialogPtr, 
								SInt16	 							localMinMaxCode, 
								SInt16	 							percentTailsClipped,
								SInt16								numberLevels,
								double*	 							minMaxValuesPtr, 
								SInt16* 								channels,
								Boolean 								updateUserDefinedFlag);  

extern Boolean 		MinMaxEnhancementDialogOnMinMaxCode (
								DialogPtr							dialogPtr,
								SInt16*								currentMinMaxCodePtr,
								SInt16								newMinMaxCode,
								SInt16								percentTailsClipped, 
								HistogramSummaryPtr				histogramSummaryPtr,
								Boolean								histogramAvailableFlag); 

extern void 			MinMaxEnhancementDialogSetSelection (
								DialogPtr							dialogPtr,            
								SInt16								lastMinMaxCode,            
								SInt16								newMinMaxCode);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMDisplayMinMaxDialog dialog


CMDisplayMinMaxDialog::CMDisplayMinMaxDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMDisplayMinMaxDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMDisplayMinMaxDialog)
	m_newBlueChannelMax = 0;
	m_newBlueChannelMin = 0;
	m_newGreenChannelMax = 0;
	m_newGreenChannelMin = 0;
	m_newRedChannelMax = 0;
	m_newRedChannelMin = 0;
	m_percentClipped = 0;
	m_localMinMaxCode = 0;
	//}}AFX_DATA_INIT
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
	m_maxValue = 255;
	m_minValue = 0;
	m_updatingMinMaxFlag = TRUE;
	m_histogramAvailableFlag = TRUE;
	
}		// end "CMDisplayMinMaxDialog"



void CMDisplayMinMaxDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMDisplayMinMaxDialog)
	DDX_Text2 (pDX, IDC_BlueChannelMinE, m_newBlueChannelMin);
	DDV_MinMaxDouble (pDX, m_newBlueChannelMin, m_minValue, m_maxValue);
	DDX_Text2 (pDX, IDC_BlueChannelMaxE, m_newBlueChannelMax);
	DDV_MinMaxDouble (pDX, m_newBlueChannelMax, m_minValue, m_maxValue);
	DDX_Text2 (pDX, IDC_GreenChannelMinE, m_newGreenChannelMin);
	DDV_MinMaxDouble (pDX, m_newGreenChannelMin, m_minValue, m_maxValue);
	DDX_Text2 (pDX, IDC_GreenChannelMaxE, m_newGreenChannelMax);
	DDV_MinMaxDouble (pDX, m_newGreenChannelMax, m_minValue, m_maxValue);
	DDX_Text2 (pDX, IDC_RedChannelMinE, m_newRedChannelMin);	
	DDV_MinMaxDouble (pDX, m_newRedChannelMin, m_minValue, m_maxValue);
	DDX_Text2 (pDX, IDC_RedChannelMaxE, m_newRedChannelMax);
	DDV_MinMaxDouble (pDX, m_newRedChannelMax, m_minValue, m_maxValue);
	DDX_Text (pDX, IDC_PercentClipped, m_percentClipped);
	DDV_MinMaxInt (pDX, m_percentClipped, 0, 100);
	DDX_Radio (pDX, IDC_ChannelMinMax, m_localMinMaxCode);
	//}}AFX_DATA_MAP     
	
}		// end "DoDataExchange"

BEGIN_MESSAGE_MAP(CMDisplayMinMaxDialog, CMDialog)
	//{{AFX_MSG_MAP(CMDisplayMinMaxDialog)
	ON_BN_CLICKED(IDC_ChannelMinMax, OnChannelMinMax)
	ON_EN_CHANGE(IDC_PercentClipped, OnChangePercentClipped)
	ON_BN_CLICKED(IDC_EntireRange, OnChannelMinMax)
	ON_EN_CHANGE(IDC_RedChannelMinE, OnChangeMinMax)
	ON_EN_CHANGE(IDC_RedChannelMaxE, OnChangeMinMax)
	ON_EN_CHANGE(IDC_GreenChannelMinE, OnChangeMinMax)
	ON_EN_CHANGE(IDC_GreenChannelMaxE, OnChangeMinMax)
	ON_EN_CHANGE(IDC_BlueChannelMinE, OnChangeMinMax)
	ON_EN_CHANGE(IDC_BlueChannelMaxE, OnChangeMinMax)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP() 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the display
//							min-max dialog box to the user and copy the
//							revised back to the display specification dialog variables
//							if the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/10/1998
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean 
CMDisplayMinMaxDialog::DoDialog(
				SInt16*								channelsPtr, 
				SInt16								rgbColors, 
				SInt16								displayType,
				SInt16								numberLevels,
				SInt16*								percentTailsClippedPtr, 
				SInt16*								minMaxSelectionPtr, 
				double*								minMaxValuesPtr)

{  
	INT_PTR					returnCode;

	Boolean					continueFlag = FALSE;
	 
	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
																			
	m_channelsPtr = channelsPtr;									
	m_rgbColors = rgbColors;									
	m_displayType = displayType;
	m_numberLevels = numberLevels;
	m_percentClipped = *percentTailsClippedPtr;									
	m_minMaxOptionCode = *minMaxSelectionPtr;										
	m_minMaxValuesPtr = minMaxValuesPtr;																			 
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{ 
		/*
		int numberChars = sprintf ((char*)&gTextString3,
												" WDMinDlg::DoDialog (red, green, blue min) %f, %f, %f %s",
												m_newRedChannelMin,
												m_newGreenChannelMin,
												m_newBlueChannelMin,
												gEndOfLine);
		ListString ((char*)&gTextString3, numberChars, gOutputTextH);
		*/
		MinMaxEnhancementDialogOK (m_histogramSummaryPtr,
											channelsPtr,
											minMaxValuesPtr,
											minMaxSelectionPtr,
											percentTailsClippedPtr,
											m_newRedChannelMin,
											m_newRedChannelMax,
											m_newGreenChannelMin,
											m_newGreenChannelMax,
											m_newBlueChannelMin,
											m_newBlueChannelMax,
											m_localMinMaxCode+2,
											(SInt16)m_percentClipped );                   
		 
		continueFlag = TRUE;
		
		}		// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}		// end "DoDialog" 




/////////////////////////////////////////////////////////////////////////////
// CMDisplayMinMaxDialog message handlers

BOOL CMDisplayMinMaxDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	SInt16 			localMinMaxCode,
						percentTailsClipped; 
						
						
	percentTailsClipped = (SInt16)m_percentClipped;	
	localMinMaxCode = (SInt16)m_localMinMaxCode; 		                

	MinMaxEnhancementDialogInitialize (this,
													m_channelsPtr,
													m_displayType,
													m_numberLevels,
													m_rgbColors,
													m_minMaxOptionCode,
													m_minMaxValuesPtr, 
													&m_histogramSpecsPtr,
													&m_histogramSummaryPtr, 
													&m_minValue,
													&m_maxValue,
													&localMinMaxCode,
													&percentTailsClipped,
													&m_histogramAvailableFlag ); 
													
			// Get the current min-max settings.
													                       
	DDX_Text2(m_dialogFromPtr, IDC_BlueChannelMaxE, m_newBlueChannelMax); 
	DDX_Text2(m_dialogFromPtr, IDC_BlueChannelMinE, m_newBlueChannelMin); 
	DDX_Text2(m_dialogFromPtr, IDC_GreenChannelMaxE, m_newGreenChannelMax); 
	DDX_Text2(m_dialogFromPtr, IDC_GreenChannelMinE, m_newGreenChannelMin);
	DDX_Text2(m_dialogFromPtr, IDC_RedChannelMaxE, m_newRedChannelMax); 
	DDX_Text2(m_dialogFromPtr, IDC_RedChannelMinE, m_newRedChannelMin); 
	
	m_percentClipped = percentTailsClipped;
	m_localMinMaxCode = localMinMaxCode - 2; 
	                 
	if (UpdateData(FALSE) )                   
		PositionDialogWindow (); 
	
			// Set default text selection to first edit text item	
	
	int selectedItem = IDC_RedChannelMinE;
	if (localMinMaxCode == kPerChannelMinMax) 
		selectedItem = IDC_PercentClipped;
		                                       
	SelectDialogItemText (this, selectedItem, 0, SInt16_MAX);
	
	m_updatingMinMaxFlag = FALSE;
	
	return FALSE;  // return TRUE  unless you set the focus to a control 
	
}		// end "OnInitDialog" 



void CMDisplayMinMaxDialog::OnChannelMinMax()

{  
	SInt16			currentMinMaxCode,
						savedMinMaxCode;
	
		                                       
	currentMinMaxCode = m_localMinMaxCode + 2;
	savedMinMaxCode = currentMinMaxCode;   
	
	DDX_Radio(m_dialogFromPtr, IDC_ChannelMinMax, m_localMinMaxCode);

	if (MinMaxEnhancementDialogOnMinMaxCode (this,
															&currentMinMaxCode,
															(SInt16)(m_localMinMaxCode + 2),
															m_percentClipped, 
															m_histogramSummaryPtr,
															m_histogramAvailableFlag))
		{ 
				// Indicate that the min maxes are being updated by the 
				// per-channel or entire range settings.
					
		m_updatingMinMaxFlag = TRUE;
		
		UpdateEnhancementMinMaxes (
								m_histogramSpecsPtr,
								this, 
								(SInt16)(m_localMinMaxCode + 2), 
								m_percentClipped, 
								m_numberLevels,
								m_minMaxValuesPtr, 
								m_channelsPtr,
								FALSE); 
								
				// Now indicate that any change in the min maxes are being done
				// by the user.   
				
		m_updatingMinMaxFlag = FALSE;
		
		}		// end "if ( MinMaxEnhancementDialogOnMinMaxCode (..." 

	MinMaxEnhancementDialogSetSelection (
							this,            
							savedMinMaxCode,            
							(SInt16)(m_localMinMaxCode + 2) ); 
	
}		// end "OnChannelMinMax"



void CMDisplayMinMaxDialog::OnChangePercentClipped()

{  
	int						savedPercentClipped;
	
	Boolean					updateMinMaxFlag = FALSE;
	
	
	savedPercentClipped = m_percentClipped;   
	DDX_Text(m_dialogFromPtr, IDC_PercentClipped, m_percentClipped);
	DDV_MinMaxInt(m_dialogFromPtr, m_percentClipped, 0, 100);
	
	if (m_percentClipped <= 100)
		{
		if (savedPercentClipped != m_percentClipped && m_histogramAvailableFlag)
			updateMinMaxFlag = TRUE;
  
		if (m_percentClipped == 0 && !m_histogramAvailableFlag)
			updateMinMaxFlag = TRUE;
			
		}		// end "if (m_percentClipped <= 100)"                                                    
    
    if (updateMinMaxFlag)
    	{
				// Indicate that the min maxes are being updated by the 
				// per-channel or entire range settings.
					
		m_updatingMinMaxFlag = TRUE; 
		
		UpdateEnhancementMinMaxes (
								m_histogramSpecsPtr,
								this, 
								(SInt16)(m_localMinMaxCode + 2), 
								m_percentClipped, 
								m_numberLevels,
								m_minMaxValuesPtr, 
								m_channelsPtr,
								FALSE);   
								
				// Now indicate that any change in the min maxes are being done
				// by the user.   
				
		m_updatingMinMaxFlag = FALSE;
		
		}		// end "if (updateMinMaxFlag)"
	
}		// end "OnChangePercentClipped" 



void CMDisplayMinMaxDialog::OnChangeMinMax()
{  
	if (!m_updatingMinMaxFlag && (m_localMinMaxCode != (kUserSpecified - 2)))
		{ 
		HideDialogItem (this, IDC_PercentClipped);   
		
				// Hilite the user specified control so that it can
				// be set, then unhilite the control. This control is
				// just for user feedback. If the control is not hilited then
				// it cannot be set.
		                                             	
		SetDLogControlHilite (this, IDC_UserSpecified, 0);
		
		m_localMinMaxCode = kUserSpecified - 2;
		DDX_Radio (m_dialogToPtr, IDC_ChannelMinMax, m_localMinMaxCode);
		
		}		// end "if (!m_updatingMinMaxFlag&& ..."
	
}		// end "OnChangeMinMax"
