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
//	File:						WAppendFileDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/05/2018
//
//	Language:				C++
//
//	System:					Windows Operating Systems
//
//	Brief description:	This file contains functions that relate to the CAboutDlg
//								class.
//
//	Functions in file:	
//
//	Include files:			"SMultiSpec.h"
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include	"SExternalGlobals.h"

#include "WAppendFileDialog.h"
#include "WMultiSpec.h" 
							
extern void AppendFileDialogOK (
				FileInfoPtr							appendFileInfoPtr,
				FileInfoPtr							newFileInfoPtr,
				SInt16								lineColumnCode,
				Boolean								lineFlag,
				Boolean								modifyFlag,
				SInt32								newAfterLineChannel,
				SInt64*								bytesToSkipPtr);

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMAppendFileDialog, CMDialog)
	//{{AFX_MSG_MAP (CMAppendFileDialog)
	ON_BN_CLICKED (IDC_LastLineChannel, OnLastLineChannel)
	ON_BN_CLICKED (IDC_LineChannel, OnLineChannel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()   



CMAppendFileDialog::CMAppendFileDialog (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMAppendFileDialog::IDD, pParent)

{
	//{{AFX_DATA_INIT(CMAppendFileDialog)
	m_appendFileName = "";
	m_lineChannel = 0;
	m_lineChannelNumber = 0;
	//}}AFX_DATA_INIT
	
	m_initializedFlag = CMDialog::m_initializedFlag;
		
	if (m_initializedFlag)                                                              
		{
		TRY
			{																			
					//	Get space for the c string										 
			
			m_appendFileName.GetBufferSetLength (256);
				
			}
			
		CATCH_ALL (e)
			{
			m_initializedFlag = FALSE;
			
			}
		END_CATCH_ALL
		
		}	// end "if (m_initializedFlag)"
	
}	// end "CMAppendFileDialog"



void CMAppendFileDialog::DoDataExchange (
				CDataExchange*						pDX)

{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMAppendFileDialog)
	DDX_Text (pDX, IDC_AppendFileName, m_appendFileName);
	DDV_MaxChars (pDX, m_appendFileName, 254);
	DDX_Radio (pDX, IDC_LineChannel, m_lineChannel);
	DDX_Text (pDX, IDC_LineChannelNumber, m_lineChannelNumber);
	DDV_MinMaxLong (pDX, m_lineChannelNumber, 0, m_maximumValue);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 DoDialog
//
//	Software purpose:	The purpose of this routine is to present the Append File
//							specification dialog box to the user and copy the
//							revised info to the append file specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in AppendFileDialog.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/30/1997
//	Revised By:			Larry L. Biehl			Date: 11/21/2006	

SInt16 CMAppendFileDialog::DoDialog (
				FileInfoPtr							appendFileInfoPtr,
				FileInfoPtr							newFileInfoPtr,
				Boolean								modifyFlag,
				SInt64*								bytesToSkipPtr,
				Boolean								lineFlag,
				SInt32								newAfterLineChannel)

{                                               
	INT_PTR								returnCode;
	SInt16								returnValue = -1;
								

			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																							return (-1);
																			
	m_modifyFlag = modifyFlag;
	m_lineFlag = lineFlag;
	
	m_lineChannelNumber = newAfterLineChannel;
	m_appendFileInfoPtr = appendFileInfoPtr; 
	m_bytesToSkipPtr = bytesToSkipPtr;
	m_maximumValue = newAfterLineChannel;
										
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{ 						                    
		returnValue = 0;
		
		AppendFileDialogOK (appendFileInfoPtr,
									newFileInfoPtr,
									m_lineChannel,
									lineFlag,
									modifyFlag,
									m_lineChannelNumber,
									bytesToSkipPtr);
			
		}	// end "if (returnCode == IDOK)"
		
	return (returnValue);
		
}	//	end "DoDialog"



BOOL CMAppendFileDialog::OnInitDialog ()

{  
	USES_CONVERSION;

	char									string[256];

	SInt16								numberCharacters;
	
	
	CDialog::OnInitDialog (); 
	
			// Append or modify file name.													
				
	if (m_modifyFlag)
		numberCharacters = sprintf (string, "Modify ");
			
	else	// !modifyFlag                                    
		numberCharacters = sprintf (string, "Append to ");		
		
	FileStringPtr appendFileNamePtr = 
			(FileStringPtr)GetFileNameCPointerFromFileInfo (m_appendFileInfoPtr);	
	sprintf (&string[numberCharacters], "'%s'", appendFileNamePtr);

	TBYTE* bufferPtr = (TBYTE*)m_appendFileName.GetBuffer (256);
	_tcscpy (&bufferPtr[numberCharacters], A2T(string));
	m_appendFileName.ReleaseBuffer ();
		
			// After specific line/channel or last line/Channel
			// The last line/channel is the default.								
				                                                   
	m_lineChannel = 1;                                       
		
	if (m_lineFlag)
		{                                                      
		SetDlgItemText (IDC_LineChannel, (LPCTSTR)_T("Line:"));	
		SetDlgItemText (IDC_LastLineChannel, (LPCTSTR)_T("Last line"));
		
		}	// end "if (m_lineFlag)"
			
	else	// !m_lineFlag
		{                                                                      
		SetDlgItemText (IDC_LineChannel, (LPCTSTR)_T("Channel:"));	
		SetDlgItemText (IDC_LastLineChannel, (LPCTSTR)_T("Last channel"));
		
		}	// end "else !m_lineFlag"
		
			// Line or channel number.											
				                                   
	MHideDialogItem (this, IDC_LineChannelNumber);               
	                 
	if (UpdateData (FALSE))
		PositionDialogWindow ();
	
	return TRUE;  // return TRUE  unless you set the focus to a control 
	
}	// end "OnInitDialog"



void CMAppendFileDialog::OnLineChannel ()

{                                                          
	MShowDialogItem (this, IDC_LineChannelNumber);
	
			// Set default text selection to first edit text item	
		                                       
	SelectDialogItemText (this, IDC_LineChannelNumber, 0, SInt16_MAX);     
	
}	// end "OnLineChannel"



void CMAppendFileDialog::OnLastLineChannel ()

{                                                           
	MHideDialogItem (this, IDC_LineChannelNumber);     
	
}	// end "OnLastLineChannel"
