// LFalseColorDialog.cpp : implementation file
// Revised by Larry Biehl on 11/16/2018

#include	"SMultiSpec.h" 
                        
#include "CDisplay.h"
#include "LFalseColorDialog.h" 



/////////////////////////////////////////////////////////////////////////////
// CMFalseColorDlg dialog


CMFalseColorDlg::CMFalseColorDlg(wxWindow* pParent, wxWindowID id, const wxString& title)
: CMDialog(CMFalseColorDlg::IDD, pParent, title)
{
	//{{AFX_DATA_INIT(CMFalseColorDlg)
	m_redChannel = 1;
	m_greenChannel = 1;
	m_blueChannel = 1;
	//}}AFX_DATA_INIT  
	
	m_layerInfoPtr = NULL;
	m_fileInfoPtr = NULL;
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
	m_maxChannelFeatureNum = 1; 
	
   CreateControls();
   SetSizerAndFit(bSizer322);
}		// end "CMFalseColorDlg"



bool CMFalseColorDlg::TransferDataFromWindow()
{
   wxTextCtrl* red = (wxTextCtrl*)FindWindow(IDC_EDITRed);
   wxTextCtrl* green = (wxTextCtrl*)FindWindow(IDC_EDITGreen);
   wxTextCtrl* blue = (wxTextCtrl*)FindWindow(IDC_EDITBlue);
   
   SInt16		badChannel;
	m_redChannel = wxAtoi(red->GetValue());
	SInt16 itemHit = FalseColorCheckColorChannel (this, 4, m_redChannel, &m_redChannelIndex);
	
	if (itemHit == 1)
		{                      
      m_greenChannel =  wxAtoi(green->GetValue());
		itemHit = FalseColorCheckColorChannel (this, 6, m_greenChannel, &m_greenChannelIndex);
		if (itemHit == 1)
			{          
         m_blueChannel = wxAtoi(blue->GetValue());
			itemHit = FalseColorCheckColorChannel (this, 8, m_blueChannel, &m_blueChannelIndex);
			if (itemHit != 1)  
				badChannel = IDC_EDITBlue;		
			}		// end "if (itemHit == 1)" 		
		else		// itemHit != 1
			badChannel = IDC_EDITGreen;
			
		}		// end "if (itemHit == 1)"		
	else		// itemHit != 1
		badChannel = IDC_EDITRed;
   
   return true;
}

bool CMFalseColorDlg::TransferDataToWindow()
{
   wxTextCtrl* red = (wxTextCtrl*)FindWindow(IDC_EDITRed);
   wxTextCtrl* green = (wxTextCtrl*)FindWindow(IDC_EDITGreen);
   wxTextCtrl* blue = (wxTextCtrl*)FindWindow(IDC_EDITBlue);
   
   red->ChangeValue(wxString::Format(wxT("%i"), (int) m_redChannel));   
   green->ChangeValue(wxString::Format(wxT("%i"), (int) m_greenChannel));
   blue->ChangeValue(wxString::Format(wxT("%i"), (int) m_blueChannel));
   
   return true;
}

BEGIN_EVENT_TABLE(CMFalseColorDlg, CMDialog)
EVT_INIT_DIALOG(CMFalseColorDlg::OnInitDialog)
END_EVENT_TABLE()



//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
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
//	Coded By:			Larry L. Biehl			Date: 01/17/97
//	Revised By:			Larry L. Biehl			Date: 01/17/97	

Boolean CMFalseColorDlg::DoDialog(void)

{  
	Boolean			continueFlag = FALSE;
	
	SInt16			returnCode;

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag || gProjectInfoPtr == NULL)
																			return(FALSE); 
																					
	returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{	
		continueFlag = TRUE; 
	
				// Cell Width.	
						
		gProjectInfoPtr->falseColorPaletteRed = m_redChannelIndex;
		gProjectInfoPtr->falseColorPaletteGreen = m_greenChannelIndex;
		gProjectInfoPtr->falseColorPaletteBlue = m_blueChannelIndex; 
		
		}		// end "if (returnCode == IDOK)"
   else
		{
      // Original in OnOk else...
      SInt16		badChannel = IDC_EDITRed;
      SelectDialogItemText (this, badChannel, 0, SInt16_MAX);  			
   }
		
	return (continueFlag);
		
}	// end "DoDialog"



/////////////////////////////////////////////////////////////////////////////
// CMFalseColorDlg message handlers

void CMFalseColorDlg::OnInitDialog(wxInitDialogEvent&	event)

{  
	wxListBox*					listBoxPtr = NULL;
	WindowInfoPtr				projectWindowInfoPtr;  
	
	SInt16						projectHandleStatus;
	
	
//	CMDialog::OnInitDialog();
																				
			// Get Project window information structure pointer.
			
	GetProjectImageFileInfo (
								kDoNotPrompt, 
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
	                     
	if (TransferDataToWindow ())
		{
		PositionDialogWindow (); 
		
		listBoxPtr = (wxListBox*)FindWindow(IDC_List1);
		
		}		// end "if (UpdateData(FALSE) )"

	if (listBoxPtr != NULL)
		{                   
		AddChannelsToDialogList (
									listBoxPtr,
									NULL, 
									NULL, 
									m_layerInfoPtr,
									m_fileInfoPtr,
									kItemsListOnly, 
									(SInt16*)gProjectInfoPtr->channelsPtr, 
									gProjectInfoPtr->numberStatisticsChannels,
									kSubsetMenuItem);
				
		}		// end "if (listBoxPtr != NULL)"
		                                       
//	GetDlgItem(IDC_EDITRed)->SetFocus();
//	SendDlgItemMessage( IDC_EDITRed, EM_SETSEL, 0, MAKELPARAM(0, -1) );
	SelectDialogItemText (this, IDC_EDITRed, 0, SInt16_MAX);  			 
	
	return;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"



/*
void CMFalseColorDlg::OnOK()
{  
	SInt16		badChannel;
	
	            
	DDX_Text(m_dialogFromPtr, IDC_EDITRed, m_redChannel);                       
	SInt16 itemHit = FalseColorCheckColorChannel (
											this, 4, m_redChannel, &m_redChannelIndex);
	
	if (itemHit == 1)
		{                               
		DDX_Text(m_dialogFromPtr, IDC_EDITGreen, m_greenChannel);
		itemHit = FalseColorCheckColorChannel (
											this, 6, m_greenChannel, &m_greenChannelIndex);
		
		if (itemHit == 1)
			{                                 
			DDX_Text(m_dialogFromPtr, IDC_EDITBlue, m_blueChannel);
			itemHit = FalseColorCheckColorChannel (
											this, 8, m_blueChannel, &m_blueChannelIndex);
											
			if (itemHit != 1)  
				badChannel = IDC_EDITBlue;
		
			}		// end "if (itemHit == 1)" 
		
		else		// itemHit != 1
			badChannel = IDC_EDITGreen;
			
		}		// end "if (itemHit == 1)"
		
	else		// itemHit != 1
		badChannel = IDC_EDITRed;
	
	if (itemHit == 1)
		CMDialog::OnOK();
		
	else		// itemHit != 1 
		{	                                       
//		GetDlgItem(badChannel)->SetFocus();
//		SendDlgItemMessage( badChannel, EM_SETSEL, 0, MAKELPARAM(0, -1) );
		SelectDialogItemText (this, badChannel, 0, SInt16_MAX);  			 
		
		}		// end "else itemHit != 1"
	
}		// end "OnOK"
*/

void CMFalseColorDlg::CreateControls ()

{
   this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	
	wxBoxSizer* bVSizerMain = new wxBoxSizer (wxVERTICAL);
	
//	wxBoxSizer* bSizer322;
	bSizer322 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer323;
	bSizer323 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer325;
	bSizer325 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText293 = new wxStaticText( this, wxID_ANY, wxT("Red:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText293->Wrap( -1 );
	bSizer325->Add( m_staticText293, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl132 = new wxTextCtrl( this, IDC_EDITRed, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), 0 );
	bSizer325->Add( m_textCtrl132, 0, wxALL, 5 );
	
	
	//bSizer323->Add( bSizer325, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
   bSizer323->Add( bSizer325, 0, wxALIGN_CENTER|wxALL, 5 );
	
	wxBoxSizer* bSizer326;
	bSizer326 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText294 = new wxStaticText( this, wxID_ANY, wxT("Green:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText294->Wrap( -1 );
	bSizer326->Add( m_staticText294, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl133 = new wxTextCtrl( this, IDC_EDITGreen, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), 0 );
	bSizer326->Add( m_textCtrl133, 0, wxALL, 5 );
	
	
	//bSizer323->Add( bSizer326, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
   bSizer323->Add( bSizer326, 0, wxALIGN_CENTER|wxALL, 5 );
	
	wxBoxSizer* bSizer327;
	bSizer327 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText295 = new wxStaticText( this, wxID_ANY, wxT("Blue:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText295->Wrap( -1 );
	bSizer327->Add( m_staticText295, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl134 = new wxTextCtrl( this, IDC_EDITBlue, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), 0 );
	bSizer327->Add( m_textCtrl134, 0, wxALL, 5 );
	
	
	//bSizer323->Add( bSizer327, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
	bSizer323->Add( bSizer327, 0, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer322->Add( bSizer323, 1, wxALIGN_CENTER, 5 );
	
	wxBoxSizer* bSizer324;
	bSizer324 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer328;
	bSizer328 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText296 = new wxStaticText( this, wxID_ANY, wxT("Available Channels"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText296->Wrap( -1 );
	bSizer328->Add( m_staticText296, 0, wxLEFT, 5 );
	
	m_listBox5 = new wxListBox( this, IDC_List1, wxDefaultPosition, wxSize( 180,160 ), 0, NULL, 0 ); 
	bSizer328->Add( m_listBox5, 0, wxALL, 5 );
	
	
	//bSizer324->Add( bSizer328, 0, wxALIGN_CENTER|wxEXPAND, 5 );
   bSizer324->Add( bSizer328, 0, wxEXPAND, 5 );
	/*
	wxBoxSizer* bSizer329;
	bSizer329 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button78 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer329->Add( m_button78, 0, wxALL, 5 );
	
	m_button79 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer329->Add( m_button79, 0, wxALL, 5 );
	
	
	//bSizer324->Add( bSizer329, 0, wxALIGN_BOTTOM, 5 );
	bSizer324->Add( bSizer329, 0, wxALIGN_CENTER, 5 );
	*/
	//bSizer322->Add( bSizer324, 0, wxALIGN_BOTTOM|wxALIGN_CENTER|wxALL, 5 );
	bSizer322->Add (bSizer324, 0, wxALIGN_CENTER|wxALL, 5);
	
	bVSizerMain->Add (bSizer322, 0);
	
	CreateStandardButtons (bVSizerMain);
	
	this->SetSizer (bVSizerMain);
	this->Layout ();
	this->Fit ();
	this->Centre (wxBOTH);
   
}
