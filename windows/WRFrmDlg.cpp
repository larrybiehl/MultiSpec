// wrfrmdlg.cpp : implementation file
//

#include "SMulSpec.h" 

#include "WRfrmDlg.h"
#include "CWinInfo.h"  

#include	"SExtGlob.h"

extern void 		ReformatDialogInitialize (
							DialogPtr							dialogPtr,
							WindowInfoPtr						windowInfoPtr,
							FileInfoPtr							fileInfoPtr,
							SInt16*								reformatRequestPtr,
							SInt16*								actionCodeVector,
							SInt16*								fileNamesSelectionPtr,
							SInt16*								chanDescriptionSelectionPtr);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMReformatDlg dialog


CMReformatDlg::CMReformatDlg(CWnd* pParent /*=NULL*/)
	: CMDialog(CMReformatDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMReformatDlg)
	m_chanDescriptionSelection = -1;
	m_reformatRequest = -1;
	//}}AFX_DATA_INIT 
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
	SInt16		index;
	for(index=0; index<9; index++)
		m_actionCodeVector[index] = 0;
		
}		// end "CMReformatDlg"



void CMReformatDlg::DoDataExchange(CDataExchange* pDX)
{  
	Boolean					adjustReformatRequestFlag = FALSE;
	
	
	CDialog::DoDataExchange(pDX);
	
			// If we are reading values from the dialog make sure that the
			// first radio button in the group is enabled.

	#ifndef _WIN32			
		if (pDX->m_bSaveAndValidate)
			{  
			if ( !GetDlgItem(IDC_Header)->IsWindowEnabled() )
				{
				GetDlgItem(IDC_Header)->EnableWindow(TRUE);
				adjustReformatRequestFlag = TRUE;
				
				}		// end "if ( !GetDlgItem(IDC_Header)->IsWindowEnabled() )"
				
			}		// end "if (pDX->m_bSaveAndValidate)"
	#endif
		
	//{{AFX_DATA_MAP(CMReformatDlg)
	DDX_CBIndex(pDX, IDC_ChanDescriptionChoice, m_chanDescriptionSelection);
	DDX_Radio(pDX, IDC_Header, m_reformatRequest);
	//}}AFX_DATA_MAP
	
	if (adjustReformatRequestFlag)
		m_reformatRequest--;
		
}		// end "DoDataExchange"



BEGIN_MESSAGE_MAP(CMReformatDlg, CMDialog)
	//{{AFX_MSG_MAP(CMReformatDlg)
	ON_BN_CLICKED(IDC_ModifyChanDescriptions, OnModifyChanDescriptions)
	ON_BN_CLICKED(IDC_ChangeImageFileFormat, OnChangeImageFileFormat)
	ON_BN_CLICKED(IDC_ConvertFieldsToThematic, OnConvertFieldsToThematic)
	ON_BN_CLICKED(IDC_ENVIROIToThematic, OnENVIROIToThematic)
	ON_BN_CLICKED(IDC_Header, OnHeader)
	ON_BN_CLICKED(IDC_MosaicImage, OnMosaicImage)
	ON_BN_CLICKED(IDC_MultispectralToThematic, OnMultispectralToThematic)
	ON_BN_CLICKED(IDC_RecodeThematic, OnRecodeThematic)
	ON_BN_CLICKED(IDC_RectifyImage, OnRectifyImage)
	ON_CBN_SELENDOK(IDC_ChanDescriptionChoice, OnSelendokChanDescriptionChoice)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP() 



//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the reformat
//							options dialog box to the user so that the user can
//							selection which reformat function is to be run.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/14/95
//	Revised By:			Larry L. Biehl			Date: 07/30/98	

SInt16 
CMReformatDlg::DoDialog(
				SInt16*					channelDescriptionRequestPtr)

{                                       
	SInt16			reformatRequest = 0,
						returnCode;
	               
	
			// First make sure all intialization has been completed.
			
	if ( !m_initializedFlag )
																		return (reformatRequest);				                                                     
																					
	returnCode = DoModal ();
	
	*channelDescriptionRequestPtr = 0;
	
	if (returnCode == IDOK)
		{
//		#ifdef _WIN32 
//			reformatRequest = m_reformatRequest + 1;      
//		#else
			reformatRequest = m_actionCodeVector[m_reformatRequest];
//		#endif

		*channelDescriptionRequestPtr = m_chanDescriptionSelection + 1;                     
		
		}		// end "if (returnCode == IDOK)"
		
	return (reformatRequest);
		
}		// end "DoDialog"



/////////////////////////////////////////////////////////////////////////////
// CMReformatDlg message handlers

BOOL 
CMReformatDlg::OnInitDialog(void)

{  
	SInt16							chanDescriptionSelection,
										fileNamesSelection,
										reformatRequest;
										
										    
	CMDialog::OnInitDialog();
	
			// Update the reformat specification dialog with the information  	
			// given in the image file specification		
			
	ReformatDialogInitialize (this,
										gImageWindowInfoPtr,
										gImageFileInfoPtr,
										&reformatRequest,
										m_actionCodeVector,
										&fileNamesSelection,
										&chanDescriptionSelection);
	
	m_reformatRequest = reformatRequest - 1;									
	m_fileNamesSelection = fileNamesSelection - 1;
	m_chanDescriptionSelection = chanDescriptionSelection - 1;
	
	if (UpdateData(FALSE))
		PositionDialogWindow (); 
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}



void CMReformatDlg::OnModifyChanDescriptions() 
{						
	ShowDialogItem (this, IDC_ChanDescriptionChoice);
	
}		// end "OnModifyChanDescriptions"

void CMReformatDlg::OnChangeImageFileFormat() 
{
	HideDialogItem (this, IDC_ChanDescriptionChoice);
	
}

void CMReformatDlg::OnConvertFieldsToThematic() 
{
	HideDialogItem (this, IDC_ChanDescriptionChoice);
	
}

void CMReformatDlg::OnENVIROIToThematic() 
{
	HideDialogItem (this, IDC_ChanDescriptionChoice);
	
}

void CMReformatDlg::OnHeader() 
{
	HideDialogItem (this, IDC_ChanDescriptionChoice);
	
}

void CMReformatDlg::OnMosaicImage() 
{
	HideDialogItem (this, IDC_ChanDescriptionChoice);
	
}

void CMReformatDlg::OnMultispectralToThematic() 
{
	HideDialogItem (this, IDC_ChanDescriptionChoice);
	
}

void CMReformatDlg::OnRecodeThematic() 
{
	HideDialogItem (this, IDC_ChanDescriptionChoice);
	
}

void CMReformatDlg::OnRectifyImage() 
{
	HideDialogItem (this, IDC_ChanDescriptionChoice);
	
}

void CMReformatDlg::OnSelendokChanDescriptionChoice() 
{
	// TODO: Add your control notification handler code here
	
}
