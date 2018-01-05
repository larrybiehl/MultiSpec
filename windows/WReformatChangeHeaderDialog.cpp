// WReformatChangeHeaderDialog.cpp : implementation file
//

//#include "stdafx.h"
#include "SMultiSpec.h"

#include "CWindowInfo.h" 

#include "WReformatChangeHeaderDialog.h"
#include "WImageView.h" 
#include "WFileFormatDialog.h"   

//#include	"SExtGlob.h" 

extern Boolean 	FileSpecificationDialog (
							Handle								fileInfoHandle, 
							Handle								windowInfoHandle,
							Boolean*								parameterChangeFlagPtr);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMChangeHeaderDlg dialog


CMChangeHeaderDlg::CMChangeHeaderDlg(CWnd* pParent /*=NULL*/)
	: CMDialog(CMChangeHeaderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMChangeHeaderDlg)
	m_headerOptionsSelection = -1;
	//}}AFX_DATA_INIT     
	
	m_headerOptionsSelection = 0;
//	if (gImageFileInfoPtr->format == kErdas73Type)
//		m_headerOptionsSelection = 0; 
	
	m_initializedFlag = CMDialog::m_initializedFlag;
		
}		// end "CMChangeHeaderDlg"



void CMChangeHeaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMChangeHeaderDlg)
	DDX_CBIndex(pDX, IDC_COMBO1, m_headerOptionsSelection);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMChangeHeaderDlg, CMDialog)
	//{{AFX_MSG_MAP(CMChangeHeaderDlg)
	ON_BN_CLICKED(IDC_ChangeParameters, OnChangeParameters)
	ON_BN_CLICKED(IDC_ChangeMapParameters, OnChangeMapParameters)
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
//	Coded By:			Larry L. Biehl			Date: 09/19/95
//	Revised By:			Larry L. Biehl			Date: 09/19/95	

SInt16 
CMChangeHeaderDlg::DoDialog(
				SInt16*					requestedFormatPtr)

{                                       
	SInt16				requestedFormat = 0;

	// oul: changed from SInt16 to SInt64
	INT_PTR				returnCode; 
	                   
	
	*requestedFormatPtr = 0;
	
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																		return(FALSE); 
																					
	returnCode = DoModal (); 
	
	if (returnCode == IDOK)
		{       
		m_headerOptionsSelection += 2;
		
//		if (m_headerOptionsSelection == 2)
//			*requestedFormatPtr = kErdas73Type;	 
		
		if (m_headerOptionsSelection == 2)
			*requestedFormatPtr = kErdas74Type;	                                
		
		}		// end "if (returnCode == IDOK)" 
		
	return (returnCode == IDOK);
		
}		// end "DoDialog"



/////////////////////////////////////////////////////////////////////////////
// CMChangeHeaderDlg message handlers

BOOL 
CMChangeHeaderDlg::OnInitDialog(void)

{                                  
	CDataExchange 	dxTo(this, FALSE);
	
	SInt16			stringNumber; 
	
	
	CDialog::OnInitDialog();
	
	stringNumber = IDS_DialogNumberChannels;
	if (gImageFileInfoPtr->thematicType)
		stringNumber = IDS_DialogNumberClasses;
		
	LoadDItemStringNumber(
							NULL,
							stringNumber,
							this,
							IDC_HeadDlgChannelsText,
							(Str255*)&gTextString);
	
	LoadCurrentHeaderParametersInDialog (&dxTo);   
	
	PositionDialogWindow (); 
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"



void 
CMChangeHeaderDlg::OnChangeParameters(void)

{     
	CDataExchange 	dxTo(this, FALSE);
	
	             
	SetDLogControlHilite (NULL, IDOK, 255);
	
	FileSpecificationDialog (gActiveImageFileInfoH, 
											gActiveImageWindowInfoH,
											NULL);
										
	LoadCurrentHeaderParametersInDialog (&dxTo);
	
	if (gImageFileInfoPtr->bandInterleave == kBIL)
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	
	else		// gImageFileInfoPtr->bandInterleave != kBIL
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	             
	SetDLogControlHilite (NULL, IDOK, 0);
	
}		// end "OnChangeParameters"  


                      
//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//							  c Purdue Research Foundation
//									All rights reserved.
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

void 
CMChangeHeaderDlg::LoadCurrentHeaderParametersInDialog (
				CDataExchange* 	dxToPtr)

{                                            
	SInt32			numberBits, 
						numberChannelsClasses;
		
		
			// Number of image lines															
				                                                                  
	DDX_Text(dxToPtr, IDC_HeadDlgLines, gImageWindowInfoPtr->maxNumberLines);
		
			// Number of image columns															
				                                                                     
	DDX_Text(dxToPtr, IDC_HeadDlgColumns, gImageWindowInfoPtr->maxNumberColumns);
		
			// Number of image channels														
		
	if (!gImageFileInfoPtr->thematicType)
		numberChannelsClasses = gImageWindowInfoPtr->totalNumberChannels;
		
			// Number of image classes.														
				
	else		// gImageFileInfoPtr->thematicType 
		numberChannelsClasses = gImageFileInfoPtr->numberClasses;
						                                     
	DDX_Text(dxToPtr, IDC_HeadDlgChannels, numberChannelsClasses);
		
			//	Set one byte data value radio button										
				
	numberBits = 0;
	if (gImageFileInfoPtr->numberBits <= 16)  
		numberBits = 16;
	if (gImageFileInfoPtr->numberBits <= 8)  
		numberBits = 8;
	if (gImageFileInfoPtr->numberBits <= 4) 
		numberBits = 4;                                 
	DDX_Text(dxToPtr, IDC_HeadDlgBits, numberBits);
		
			// Start line of image																
				                                                      
	DDX_Text(dxToPtr, IDC_HeadDlgStartLine, gImageFileInfoPtr->startLine);
		
			// Start column of image															
				                                                        
	DDX_Text(dxToPtr, IDC_HeadDlgStartColumn, gImageFileInfoPtr->startColumn);
	
}		// end "LoadCurrentHeaderParametersInDialog"



void CMChangeHeaderDlg::OnChangeMapParameters() 
{             
	SetDLogControlHilite (NULL, IDOK, 255);

	if ( CoordinateDialog () )
		gUpdateWindowsMenuItemsFlag = TRUE;  
	
	SetDLogControlHilite (NULL, IDOK, 0);
	
}		// end "OnChangeMapParameters"
