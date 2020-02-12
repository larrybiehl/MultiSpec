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
//	File:						WFalseColorDialog.cpp : implementation file
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
//								CMFalseColorDlg class.
//
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h"
#include "SDisplay_class.h"

#include "WFalseColorDialog.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMFalseColorDlg, CMDialog)
	//{{AFX_MSG_MAP (CMFalseColorDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMFalseColorDlg::CMFalseColorDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog	(CMFalseColorDlg::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMFalseColorDlg)
	m_redChannel = 1;
	m_greenChannel = 1;
	m_blueChannel = 1;
	//}}AFX_DATA_INIT  
	
	m_layerInfoPtr = NULL;
	m_fileInfoPtr = NULL;
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
	m_maxChannelFeatureNum = 1; 
	
}	// end "CMFalseColorDlg"



void CMFalseColorDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMFalseColorDlg)
	DDX_Text (pDX, IDC_EDITRed, m_redChannel);
	DDV_MinMaxLong (pDX, m_redChannel, 1, m_maxChannelFeatureNum);
	DDX_Text (pDX, IDC_EDITGreen, m_greenChannel);
	DDV_MinMaxLong (pDX, m_greenChannel, 1, m_maxChannelFeatureNum);
	DDX_Text (pDX, IDC_EDITBlue, m_blueChannel);
	DDV_MinMaxLong (pDX, m_blueChannel, 1, m_maxChannelFeatureNum);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the false color
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
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/17/1997
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean CMFalseColorDlg::DoDialog (void)

{
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE;

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{	
		continueFlag = TRUE; 
	
				// Cell Width.	
						
		gProjectInfoPtr->falseColorPaletteRed = m_redChannelIndex;
		gProjectInfoPtr->falseColorPaletteGreen = m_greenChannelIndex;
		gProjectInfoPtr->falseColorPaletteBlue = m_blueChannelIndex; 
		
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



BOOL CMFalseColorDlg::OnInitDialog (void)

{  
	CListBox*							listBoxPtr;
	WindowInfoPtr						projectWindowInfoPtr;
	
	SInt16								projectHandleStatus;
	
	
	CMDialog::OnInitDialog ();
																				
			// Get Project window information structure pointer.
			
	GetProjectImageFileInfo (kDoNotPrompt,
										kSetupGlobalInfoPointersIfCan,
										&projectWindowInfoPtr,
										&m_layerInfoPtr,
										&m_fileInfoPtr,
										&projectHandleStatus);
	
			// Load default project values if needed.
			
	LoadDefaultProjectFalseColorChannels ();
	
			// Get default values.  			
			
	m_redChannel = 
			gProjectInfoPtr->channelsPtr[ gProjectInfoPtr->falseColorPaletteRed ] + 1;
	m_greenChannel = 
			gProjectInfoPtr->channelsPtr[ gProjectInfoPtr->falseColorPaletteGreen ] + 1;
	m_blueChannel = 
			gProjectInfoPtr->channelsPtr[ gProjectInfoPtr->falseColorPaletteBlue ] + 1;
		
	m_maxChannelFeatureNum = projectWindowInfoPtr->totalNumberChannels; 
	                     
	if (UpdateData (FALSE))
		{
		PositionDialogWindow (); 
		
		listBoxPtr = (CListBox*)GetDlgItem (IDC_List1);
		
		}	// end "if (UpdateData (FALSE))"

	if (listBoxPtr != NULL)
		{                   
		AddChannelsToDialogList (listBoxPtr,
											NULL, 
											NULL, 
											m_layerInfoPtr,
											m_fileInfoPtr,
											kItemsListOnly, 
											(SInt16*)gProjectInfoPtr->channelsPtr, 
											gProjectInfoPtr->numberStatisticsChannels,
											kSubsetMenuItem);
			
		}	// end "if (listBoxPtr != NULL)"
		                                       
	SelectDialogItemText (this, IDC_EDITRed, 0, SInt16_MAX);
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"



void CMFalseColorDlg::OnOK ()

{  
	SInt16								badChannel;
	
	            
	DDX_Text (m_dialogFromPtr, IDC_EDITRed, m_redChannel);
	SInt16 itemHit = FalseColorCheckColorChannel (
													this, 4, m_redChannel, &m_redChannelIndex);
	
	if (itemHit == 1)
		{                               
		DDX_Text (m_dialogFromPtr, IDC_EDITGreen, m_greenChannel);
		itemHit = FalseColorCheckColorChannel (
											this, 6, m_greenChannel, &m_greenChannelIndex);
		
		if (itemHit == 1)
			{                                 
			DDX_Text (m_dialogFromPtr, IDC_EDITBlue, m_blueChannel);
			itemHit = FalseColorCheckColorChannel (
											this, 8, m_blueChannel, &m_blueChannelIndex);
											
			if (itemHit != 1)  
				badChannel = IDC_EDITBlue;
		
			}	// end "if (itemHit == 1)" 
		
		else    // itemHit != 1
			badChannel = IDC_EDITGreen;
			
		}	// end "if (itemHit == 1)"
		
	else    // itemHit != 1
		badChannel = IDC_EDITRed;
	
	if (itemHit == 1)
		CMDialog::OnOK ();
		
	else    // itemHit != 1 
		SelectDialogItemText (this, badChannel, 0, SInt16_MAX);
	
}	// end "OnOK"
