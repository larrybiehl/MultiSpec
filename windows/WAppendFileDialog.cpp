//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WAppendFileDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/05/2018
//
//	Language:				C
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
#include "WMultiSpec.h" 
#include "WAppendFileDialog.h"
#include	"SExternalGlobals.h" 
							
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



CMAppendFileDialog::CMAppendFileDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMAppendFileDialog::IDD, pParent)

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
			
			m_appendFileName.GetBufferSetLength(256);
				
			}
			
		CATCH_ALL(e)
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
	//{{AFX_DATA_MAP(CMAppendFileDialog)
	DDX_Text (pDX, IDC_AppendFileName, m_appendFileName);
	DDV_MaxChars (pDX, m_appendFileName, 254);
	DDX_Radio (pDX, IDC_LineChannel, m_lineChannel);
	DDX_Text (pDX, IDC_LineChannelNumber, m_lineChannelNumber);
	DDV_MinMaxLong (pDX, m_lineChannelNumber, 0, m_maximumValue);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



BEGIN_MESSAGE_MAP (CMAppendFileDialog, CMDialog)
	//{{AFX_MSG_MAP(CMAppendFileDialog)
	ON_BN_CLICKED (IDC_LineChannel, OnLineChannel)
	ON_BN_CLICKED (IDC_LastLineChannel, OnLastLineChannel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()   


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
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

SInt16 CMAppendFileDialog::DoDialog(
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
							                         
	if ( !m_initializedFlag )
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



/////////////////////////////////////////////////////////////////////////////
// CMAppendFileDialog message handlers

BOOL CMAppendFileDialog::OnInitDialog()
{  
	#if defined _UNICODE
		USES_CONVERSION;
	#endif              

	char				string[256];

	SInt16			numberCharacters;
	
	
	CDialog::OnInitDialog(); 
	
			// Append or modify file name.													
				
	if (m_modifyFlag)
		numberCharacters = sprintf(string, "Modify ");
			
	else		// !modifyFlag                                    
		numberCharacters = sprintf(string, "Append to ");		
		
	FileStringPtr appendFileNamePtr = 
			(FileStringPtr)GetFileNameCPointerFromFileInfo (m_appendFileInfoPtr);	
	sprintf(&string[numberCharacters], "'%s'", appendFileNamePtr);

	TBYTE* bufferPtr = (TBYTE*)m_appendFileName.GetBuffer(256);
	#if defined _UNICODE
		_tcscpy (&bufferPtr[numberCharacters], A2T(string));
	#endif
	#if !defined _UNICODE
		strcpy ((char*)&bufferPtr[numberCharacters], string);
	#endif
	m_appendFileName.ReleaseBuffer();
		
			// After specific line/channel or last line/Channel
			// The last line/channel is the default.								
				                                                   
	m_lineChannel = 1;                                       
		
	if (m_lineFlag)
		{                                                      
		SetDlgItemText (IDC_LineChannel, (LPCTSTR)_T("Line:"));	
		SetDlgItemText (IDC_LastLineChannel, (LPCTSTR)_T("Last line"));
		
		}		// end "if (m_lineFlag)"
			
	else		// !m_lineFlag
		{                                                                      
		SetDlgItemText (IDC_LineChannel, (LPCTSTR)_T("Channel:"));	
		SetDlgItemText (IDC_LastLineChannel, (LPCTSTR)_T("Last channel"));
		
		}		// end "else !m_lineFlag"  
		
			// Line or channel number.											
				                                   
	MHideDialogItem (this, IDC_LineChannelNumber);               
	                 
	if (UpdateData (FALSE) )
		PositionDialogWindow ();
	
	return TRUE;  // return TRUE  unless you set the focus to a control 
	
}	// end "OnInitDialog"



void CMAppendFileDialog::OnLineChannel ()

{                                                          
	MShowDialogItem (this, IDC_LineChannelNumber);
	
			// Set default text selection to first edit text item	
		                                       
//	GetDlgItem(IDC_LineChannelNumber)->SetFocus();
//	SendDlgItemMessage( IDC_LineChannelNumber, EM_SETSEL, 0, MAKELPARAM(0, -1) );
	SelectDialogItemText (this, IDC_LineChannelNumber, 0, SInt16_MAX);     
	
}	// end "OnLineChannel"



void CMAppendFileDialog::OnLastLineChannel ()

{                                                           
	MHideDialogItem (this, IDC_LineChannelNumber);     
	
}	// end "OnLastLineChannel"
