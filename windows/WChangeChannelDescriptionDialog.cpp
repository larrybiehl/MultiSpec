// WChangeChannelDescriptionDialog.cpp : implementation file
//
// Revised by Larry Biehl on 01/04/2018
//
                   
#include "SMultiSpec.h"
#include "WChangeChannelDescriptionDialog.h"
//#include	"SExtGlob.h"

extern void 		LoadDescriptionIntoDItem (
							DialogPtr 							dialogPtr, 
							SInt16	 							itemNumber, 
							ChannelDescriptionPtr 			channelDescriptionPtr);

extern void 		ModifyChannelDescriptionsChangeChannel (
							DialogPtr							dialogPtr, 
							ChannelDescriptionPtr			channelDescriptionPtr, 
							float*								channelValuesPtr,
							SInt16								channelIndex,
							SInt16								lastSelectedTextItem);

extern Boolean 	ModifyChannelDescriptionsUpdate (
							DialogPtr							dialogPtr, 
							ChannelDescriptionPtr			channelDescriptionPtr, 
							float*								channelValuesPtr,
							SInt16								channelIndex,
							char*									stringPtr,
							float									realValue,
							Boolean								changeFlag);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMChangeChannelDescriptionDlg dialog


CMChangeChannelDescriptionDlg::CMChangeChannelDescriptionDlg(CWnd* pParent /*=NULL*/)
	: CMDialog(CMChangeChannelDescriptionDlg::IDD, pParent)
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

}		// end "CMChangeChannelDescriptionDlg"


void CMChangeChannelDescriptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMChangeChannelDescriptionDlg)
	DDX_Text(pDX, IDC_Description, m_description);
	DDV_MaxChars(pDX, m_description, 16);
	DDX_Text2(pDX, IDC_Value, m_value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMChangeChannelDescriptionDlg, CMDialog)
	//{{AFX_MSG_MAP(CMChangeChannelDescriptionDlg)
	ON_BN_CLICKED(IDC_NextChannelButton, OnNextChannelButton)
	ON_BN_CLICKED(IDC_PreviousChannelButton, OnPreviousChannelButton)
	ON_EN_SETFOCUS(IDC_Description, OnSetfocusDescription)
	ON_EN_SETFOCUS(IDC_Value, OnSetfocusValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()   


//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the CEM
//							specification dialog box to the user and copy the
//							revised back to the classify specification structure if
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
//	Coded By:			Larry L. Biehl			Date: 12/29/99
//	Revised By:			Larry L. Biehl			Date: 12/29/99	

SInt16 
CMChangeChannelDescriptionDlg::DoDialog(
				FileInfoPtr							fileInfoPtr, 
				ChannelDescriptionPtr			channelDescriptionPtr, 
				float*								channelValuesPtr)

{  
	INT_PTR							returnCode;
	SInt16							returnValue;


			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
													
	m_fileInfoPtr = fileInfoPtr;						
	m_channelDescriptionPtr = channelDescriptionPtr; 
	m_channelValuesPtr = channelValuesPtr;

	returnValue = 1;
																					
	returnCode = DoModal ();
	
	if (returnCode != IDOK)
		{
		m_changeFlag = FALSE;
		returnValue = 0;
		
		}		// end "if (returnCode != IDOK)"
		
	return (returnValue);
		
}		// end "DoDialog" 



/////////////////////////////////////////////////////////////////////////////
// CMChangeChannelDescriptionDlg message handlers

BOOL CMChangeChannelDescriptionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_channelIndex = 0;
	::LoadDItemValue (this, IDC_ChannelNumber, m_channelIndex+1);

//	m_description = ;

	m_value = *m_channelValuesPtr;
	m_lastSelectedTextItem = IDC_Description;
		
	if ( UpdateData(FALSE) )
		{ 
		PositionDialogWindow (); 
		
				// Set default text selection to first edit text item 
	
		LoadDescriptionIntoDItem (
					this, IDC_Description, &m_channelDescriptionPtr[m_channelIndex]);
			                                       
		SelectDialogItemText (this, IDC_Description, 0, SInt16_MAX);      
		
		return FALSE;  // return TRUE  unless you set the focus to a control
		
		}		// end "if (UpdateData(FALSE) )"
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void 
CMChangeChannelDescriptionDlg::OnNextChannelButton()
 
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
	
}		// end "OnNextChannelButton"



void 
CMChangeChannelDescriptionDlg::OnPreviousChannelButton()
 
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
	
}		// end "OnPreviousChannelButton"



void CMChangeChannelDescriptionDlg::OnOK()
 
{
	Update ();
	
	CMDialog::OnOK();

}		// end "OnOK"



void CMChangeChannelDescriptionDlg::Update(void)
 
{
	Str255						descriptionString;

	SInt16						stringLength;
	
			// Get the latest description for the current channel

	DDX_Text(m_dialogFromPtr, IDC_Description, m_description);
	
	MGetString (descriptionString, kFileIOStrID, IDS_BlankString16);
	stringLength = m_description.GetLength();		
	BlockMoveData (m_description, &descriptionString[1], stringLength);
	
			// Get the new value

	DDX_Text2(m_dialogFromPtr, IDC_Value, m_value);

	m_changeFlag = ModifyChannelDescriptionsUpdate (this, 
																	m_channelDescriptionPtr, 
																	m_channelValuesPtr,
																	m_channelIndex,
																	(char*)&descriptionString,
																	m_value,
																	m_changeFlag);

}		// end "OnOK"



void CMChangeChannelDescriptionDlg::OnSetfocusDescription() 
{
	m_lastSelectedTextItem = IDC_Description;
	
}



void CMChangeChannelDescriptionDlg::OnSetfocusValue() 
{
	m_lastSelectedTextItem = IDC_Value;
	
}
