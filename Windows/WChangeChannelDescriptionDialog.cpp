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
//	File:						WChangeChannelDescriptionDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			08/29/2020
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMChangeChannelDescriptionDlg class.
//
//------------------------------------------------------------------------------------
                   
#include "SMultiSpec.h"

#include "WChangeChannelDescriptionDialog.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMChangeChannelDescriptionDlg, CMDialog)
	//{{AFX_MSG_MAP (CMChangeChannelDescriptionDlg)
	ON_BN_CLICKED (IDC_NextChannelButton, OnNextChannelButton)
	ON_BN_CLICKED (IDC_PreviousChannelButton, OnPreviousChannelButton)
	ON_EN_SETFOCUS (IDC_Description, OnSetfocusDescription)
	ON_EN_SETFOCUS (IDC_Value, OnSetfocusValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMChangeChannelDescriptionDlg::CMChangeChannelDescriptionDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMChangeChannelDescriptionDlg::IDD, pParent)

{
	//{{AFX_DATA_INIT(CMChangeChannelDescriptionDlg)
	m_description = _T("");
	m_value = 0.0f;
	//}}AFX_DATA_INIT
	
	m_channelDescriptionPtr = NULL; 
	m_fileInfoPtr = NULL;
	m_channelValuesPtr = NULL;
	m_channelIndex = 0;
	m_lastSelectedTextItem = 0;
	m_changeFlag = FALSE;
	
	m_initializedFlag = CMDialog::m_initializedFlag; 

}	// end "CMChangeChannelDescriptionDlg"



void CMChangeChannelDescriptionDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMChangeChannelDescriptionDlg)
	DDX_Text (pDX, IDC_Description, m_description);
	DDV_MaxChars (pDX, m_description, kChannelDescriptionLength);
	DDX_Text2 (pDX, IDC_Value, m_value);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



SInt16 CMChangeChannelDescriptionDlg::DoDialog (
				FileInfoPtr							fileInfoPtr, 
				ChannelDescriptionPtr			channelDescriptionPtr, 
				float*								channelValuesPtr)

{  
	INT_PTR								returnCode;
	SInt16								returnValue;


			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
													
	m_fileInfoPtr = fileInfoPtr;						
	m_channelDescriptionPtr = channelDescriptionPtr; 
	m_channelValuesPtr = channelValuesPtr;

	returnValue = 1;
																					
	returnCode = DoModal ();
	
	if (returnCode != IDOK)
		{
		m_changeFlag = FALSE;
		returnValue = 0;
		
		}	// end "if (returnCode != IDOK)"
		
	return (returnValue);
		
}	// end "DoDialog" 



BOOL CMChangeChannelDescriptionDlg::OnInitDialog ()

{
	CDialog::OnInitDialog ();
	
	m_channelIndex = 0;
	::LoadDItemValue (this, IDC_ChannelNumber, m_channelIndex+1);

	m_value = *m_channelValuesPtr;
	m_lastSelectedTextItem = IDC_Description;
		
	if (UpdateData (FALSE))
		{ 
		PositionDialogWindow (); 
		
				// Set default text selection to first edit text item 
	
		LoadDescriptionIntoDItem (
						this, IDC_Description, &m_channelDescriptionPtr[m_channelIndex]);
			                                       
		SelectDialogItemText (this, IDC_Description, 0, SInt16_MAX);      
		
		return FALSE;  // return TRUE  unless you set the focus to a control
		
		}	// end "if (UpdateData (FALSE))"
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	
}	// end "OnInitDialog"



void CMChangeChannelDescriptionDlg::OnNextChannelButton ()
 
{		
	Update ();
	
	m_channelIndex++;
	if (m_channelIndex >= m_fileInfoPtr->numberChannels)
		m_channelIndex = 0;
							
	ModifyChannelDescriptionsChangeChannel (this, 
															m_channelDescriptionPtr, 
															m_channelValuesPtr,
															m_channelIndex,
															m_lastSelectedTextItem);
	
}	// end "OnNextChannelButton"



void CMChangeChannelDescriptionDlg::OnOK ()

{
	Update ();
	
	CMDialog::OnOK ();

}	// end "OnOK"



void CMChangeChannelDescriptionDlg::OnPreviousChannelButton ()
 
{	
	Update ();
	
	m_channelIndex--;
	if (m_channelIndex < 0)
		m_channelIndex = m_fileInfoPtr->numberChannels - 1;
							
	ModifyChannelDescriptionsChangeChannel (this, 
															m_channelDescriptionPtr, 
															m_channelValuesPtr,
															m_channelIndex,
															m_lastSelectedTextItem);
	
}	// end "OnPreviousChannelButton"



void CMChangeChannelDescriptionDlg::OnSetfocusDescription ()

{
	m_lastSelectedTextItem = IDC_Description;
	
}	// end "OnSetfocusDescription"



void CMChangeChannelDescriptionDlg::OnSetfocusValue ()

{
	m_lastSelectedTextItem = IDC_Value;
	
}	// end "OnSetfocusValue"



void CMChangeChannelDescriptionDlg::Update (void)

{
	USES_CONVERSION;

	char							inputUTF8DescriptionSring[256];

	Str255						descriptionString;

	SInt16						stringLength;
	
			// Get the latest description for the current channel

	DDX_Text (m_dialogFromPtr, IDC_Description, m_description);
	
	MGetString (descriptionString, kFileIOStrID, IDS_BlankString16);
	stringLength = m_description.GetLength ();
	strcpy ((char*)inputUTF8DescriptionSring, T2A(m_description));
	BlockMoveData (inputUTF8DescriptionSring, &descriptionString[1], stringLength);
	
			// Get the new value

	DDX_Text2 (m_dialogFromPtr, IDC_Value, m_value);

	m_changeFlag = ModifyChannelDescriptionsUpdate (this,
																	m_channelDescriptionPtr,
																	m_channelValuesPtr,
																	m_channelIndex,
																	(char*)&descriptionString,
																	m_value,
																	m_changeFlag);

}	// end "Update"
