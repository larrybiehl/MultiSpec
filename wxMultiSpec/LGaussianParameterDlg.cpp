// LGaussianParameterDlg.cpp : implementation file
//
// Revised by Larry Biehl on 11/13/2018
//

#include "SMultiSpec.h"                   
#include "LGaussianParameterDlg.h"
#include	"SExternalGlobals.h" 



CMGaussianParameterDlg::CMGaussianParameterDlg(wxWindow* pParent, wxWindowID id, const wxString& title /*=NULL*/ )
: CMDialog(CMGaussianParameterDlg::IDD, pParent, title)
{
	//{{AFX_DATA_INIT(CMGaussianParameterDlg)
	m_gaussianStretch = 0.0;
	//}}AFX_DATA_INIT

	m_initializedFlag = CMDialog::m_initializedFlag;
   
   CreateControls ();
	
}	// end "CMGaussianParameterDlg"



BEGIN_EVENT_TABLE (CMGaussianParameterDlg, CMDialog)
	EVT_INIT_DIALOG (CMGaussianParameterDlg::OnInitDialog)
END_EVENT_TABLE ()



void CMGaussianParameterDlg::CreateControls ()

	{
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	bSizer308 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer309 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText289 = new wxStaticText (this,
													wxID_ANY,
													wxT("Number Standard Deviations:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText289->Wrap (-1);
	bSizer309->Add (m_staticText289, 0, wxALIGN_CENTER|wxALL, 5);
	
	m_textCtrl130 = new wxTextCtrl (this,
												IDC_GaussianStretch,
												wxT("2.0"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	bSizer309->Add (m_textCtrl130, 0, wxALL, 5);
	
	bSizer308->Add (bSizer309, 0, wxALL, 12);
	
	wxBoxSizer* bSizer310 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText290 = new wxStaticText (
			this,
			wxID_ANY,
			#if defined multispec_wxmac
				wxT("Note: The following percent of normally distributed data will be stretched between 0 and 255.\r   1.0 represents 68.3% (more contrast)\r   2.0 represents 95.5%\r   3.0 represents 99.7% (less contrast)"),
			#else
				wxT("Note: The following percent of normally distributed data will be stretched between 0 and 255.\n   1.0 represents 68.3% (more contrast)\n   2.0 represents 95.5%\n   3.0 represents 99.7% (less contrast)"),
			#endif
			wxDefaultPosition,
			wxSize (600, 80),
			0);
	m_staticText290->Wrap (-1);
	bSizer310->Add (m_staticText290, 0, wxLEFT, 25);
	
	bSizer308->Add (bSizer310, 0, wxLEFT|wxTOP|wxRIGHT, 12);
	/*
	wxBoxSizer* bSizer311;
	bSizer311 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button75 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer311->Add( m_button75, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_button76 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer311->Add( m_button76, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	bSizer308->Add( bSizer311, 0, wxALIGN_RIGHT|wxALL, 5 );
	*/
	/*
	wxSizer* standardButtonSizer = CreateButtonSizer (wxOK | wxCANCEL);
	#if defined multispec_wxmac
		bSizer308->Add (standardButtonSizer, wxSizerFlags(0).Right());
	#else
		bSizer308->Add (standardButtonSizer, wxSizerFlags(0).Right().Border(wxTOP|wxBOTTOM, 12));
	#endif
	*/
	CreateStandardButtons (bSizer308);
	
   SetSizerAndFit (bSizer308);
	//this->SetSizer (bSizer308);
	//this->Layout ();
	
	this->Centre (wxBOTH);
	
}	// end "CreateControls"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the display
//							gaussian parameter dialog box to the user and copy the
//							revised parameter to the display specification dialog 
//							variables if the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/07/2003
//	Revised By:			Larry L. Biehl			Date: 11/07/2018

Boolean CMGaussianParameterDlg::DoDialog(
				double*								gaussianStretchPtr)
{  
	SInt16			returnCode;

	Boolean			continueFlag = FALSE;
	 
	                          
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
																			
	m_gaussianStretch = *gaussianStretchPtr;																		 
																					
	returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{   		
		*gaussianStretchPtr = m_gaussianStretch;																		 
		 
		continueFlag = TRUE;
		
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



void CMGaussianParameterDlg::OnInitDialog (wxInitDialogEvent& event)

{
	
   if (TransferDataToWindow ())
        PositionDialogWindow ();
	
			// Set default text selection to first edit text item
	
	SelectDialogItemText (this, IDC_GaussianStretch, 0, SInt16_MAX);
	
}	// end "OnInitDialog"



bool CMGaussianParameterDlg::TransferDataFromWindow ()

{
   wxTextCtrl* std_stretch = (wxTextCtrl*)FindWindow (IDC_GaussianStretch);
   wxString std_stretch_string = std_stretch->GetValue ();
	std_stretch_string.ToDouble (&m_gaussianStretch);
	
   return true;
	
}	// end "TransferDataFromWindow"



bool CMGaussianParameterDlg::TransferDataToWindow ()

{
   wxTextCtrl* std_stretch = (wxTextCtrl*) FindWindow (IDC_GaussianStretch);
   std_stretch->ChangeValue (wxString::Format (wxT("%.2f"), m_gaussianStretch));
	
   return true;
	
}	// end "TransferDataToWindow"
