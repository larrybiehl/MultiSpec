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
//	File:						WReformatChangeHeaderDialog.cpp : implementation file
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
//								CMChangeHeaderDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WFileFormatDialog.h"
#include "WImageView.h"
#include "WReformatChangeHeaderDialog.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMChangeHeaderDlg, CMDialog)
	//{{AFX_MSG_MAP (CMChangeHeaderDlg)
	ON_BN_CLICKED (IDC_ChangeMapParameters, OnChangeMapParameters)
	ON_BN_CLICKED (IDC_ChangeParameters, OnChangeParameters)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMChangeHeaderDlg::CMChangeHeaderDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMChangeHeaderDlg::IDD, pParent)
		
{
	//{{AFX_DATA_INIT(CMChangeHeaderDlg)
	m_headerOptionsSelection = -1;
	//}}AFX_DATA_INIT     
	
	m_headerOptionsSelection = 0;
	
	m_initializedFlag = CMDialog::m_initializedFlag;
		
}	// end "CMChangeHeaderDlg"



void CMChangeHeaderDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMChangeHeaderDlg)
	DDX_CBIndex (pDX, IDC_COMBO1, m_headerOptionsSelection);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the reformat
//							change header dialog box to the user so that the user can
//							make changes.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/19/1995
//	Revised By:			Larry L. Biehl			Date: 09/19/1995

SInt16 CMChangeHeaderDlg::DoDialog (
				SInt16*								requestedFormatPtr)

{                                       
	INT_PTR								returnCode;
	
	SInt16								requestedFormat = 0;

	
	*requestedFormatPtr = 0;
	
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
																					
	returnCode = DoModal (); 
	
	if (returnCode == IDOK)
		{       
		m_headerOptionsSelection += 2;
		
		if (m_headerOptionsSelection == 2)
			*requestedFormatPtr = kErdas74Type;	                                
		
		}	// end "if (returnCode == IDOK)" 
		
	return (returnCode == IDOK);
		
}	// end "DoDialog"



void CMChangeHeaderDlg::OnChangeMapParameters ()

{
	SetDLogControlHilite (NULL, IDOK, 255);

	if (CoordinateDialog ())
		gUpdateWindowsMenuItemsFlag = TRUE;
	
	SetDLogControlHilite (NULL, IDOK, 0);
	
}	// end "OnChangeMapParameters"



void CMChangeHeaderDlg::OnChangeParameters (void)

{
	CDataExchange 						dxTo (this, FALSE);
	
	
	SetDLogControlHilite (NULL, IDOK, 255);
	
	FileSpecificationDialog (gActiveImageFileInfoH,
										gActiveImageWindowInfoH,
										NULL);
	
	LoadCurrentHeaderParametersInDialog (&dxTo);
	
	if (gImageFileInfoPtr->bandInterleave == kBIL)
		GetDlgItem (IDOK)->EnableWindow (TRUE);
	
	else    // gImageFileInfoPtr->bandInterleave != kBIL
		GetDlgItem (IDOK)->EnableWindow (FALSE);
	
	SetDLogControlHilite (NULL, IDOK, 0);
	
}	// end "OnChangeParameters"



BOOL CMChangeHeaderDlg::OnInitDialog (void)

{                                  
	CDataExchange 						dxTo (this, FALSE);
	
	SInt16								stringNumber;
	
	
	CDialog::OnInitDialog ();
	
	stringNumber = IDS_DialogNumberChannels;
	if (gImageFileInfoPtr->thematicType)
		stringNumber = IDS_DialogNumberClasses;
		
	LoadDItemStringNumber (NULL,
									stringNumber,
									this,
									IDC_HeadDlgChannelsText,
									(Str255*)&gTextString);
	
	LoadCurrentHeaderParametersInDialog (&dxTo);   
	
	PositionDialogWindow (); 
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"


                      
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadCurrentHeaderParametersInDialog
//
//	Software purpose:	This loads the current header parameters in
//							the change header dialog box.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			ChangeErdasHeaderDialog		
//
//	Coded By:			Larry L. Biehl			Date: 11/20/91
//	Revised By:			Larry L. Biehl			Date: 09/19/95

void CMChangeHeaderDlg::LoadCurrentHeaderParametersInDialog (
				CDataExchange* 					dxToPtr)

{                                            
	SInt32								numberBits,
											numberChannelsClasses;
		
		
			// Number of image lines															
				                                                                  
	DDX_Text (dxToPtr, IDC_HeadDlgLines, gImageWindowInfoPtr->maxNumberLines);
		
			// Number of image columns															
				                                                                     
	DDX_Text (dxToPtr, IDC_HeadDlgColumns, gImageWindowInfoPtr->maxNumberColumns);
		
			// Number of image channels														
		
	if (!gImageFileInfoPtr->thematicType)
		numberChannelsClasses = gImageWindowInfoPtr->totalNumberChannels;
		
			// Number of image classes.														
				
	else    // gImageFileInfoPtr->thematicType 
		numberChannelsClasses = gImageFileInfoPtr->numberClasses;
						                                     
	DDX_Text (dxToPtr, IDC_HeadDlgChannels, numberChannelsClasses);
		
			//	Set one byte data value radio button										
				
	numberBits = 0;
	if (gImageFileInfoPtr->numberBits <= 16)  
		numberBits = 16;
	if (gImageFileInfoPtr->numberBits <= 8)  
		numberBits = 8;
	if (gImageFileInfoPtr->numberBits <= 4) 
		numberBits = 4;                                 
	DDX_Text (dxToPtr, IDC_HeadDlgBits, numberBits);
		
			// Start line of image																
				                                                      
	DDX_Text (dxToPtr, IDC_HeadDlgStartLine, gImageFileInfoPtr->startLine);
		
			// Start column of image															
				                                                        
	DDX_Text (dxToPtr, IDC_HeadDlgStartColumn, gImageFileInfoPtr->startColumn);
	
}	// end "LoadCurrentHeaderParametersInDialog"
