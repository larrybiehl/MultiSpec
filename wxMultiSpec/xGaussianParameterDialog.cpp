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
//	File:						xGaussianParameterDialog.cpp : class implementation file
//	Class Definition:		xGaussianParameterDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/13/2019
//
//	Language:				C++
//
//	System:					Linux & MacOS Operating Systems
//
//	Brief description:	This file contains functions related to the
//								CMGaussianParameterDlg class.
//
// Following is template for debugging
/*
	int numberChars = sprintf ((char*)gTextString3,
									 " xGaussianParameterDialog:: (): %s",
									 gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "xGaussianParameterDialog.h"



BEGIN_EVENT_TABLE (CMGaussianParameterDlg, CMDialog)
	EVT_INIT_DIALOG (CMGaussianParameterDlg::OnInitDialog)
END_EVENT_TABLE ()



CMGaussianParameterDlg::CMGaussianParameterDlg (
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title /*=NULL*/)
		: CMDialog (CMGaussianParameterDlg::IDD, pParent, title)
		
{
	m_gaussianStretch = 0.0;

	m_initializedFlag = CMDialog::m_initializedFlag;
   
   CreateControls ();
	
}	// end "CMGaussianParameterDlg"



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
				wxT("Note: The following percent of normally distributed data will be stretched between 0 and 255.\r   1.0 represents 68.3% (more contrast)\r   2.0 represents 95.5%\r   3.0 represents 99.7% (less contrast)\r\r   The default is 2.3"),
			#else
				wxT("Note: The following percent of normally distributed data will be stretched between 0 and 255.\n   1.0 represents 68.3% (more contrast)\n   2.0 represents 95.5%\n   3.0 represents 99.7% (less contrast)\n\n   The default is 2.3"),
			#endif
			wxDefaultPosition,
			wxSize (600, 120),
			0);
	m_staticText290->Wrap (-1);
	bSizer310->Add (m_staticText290, 0, wxLEFT, 25);
	
	bSizer308->Add (bSizer310, 0, wxLEFT|wxTOP|wxRIGHT, 12);

	CreateStandardButtons (bSizer308);
	
   SetSizerAndFit (bSizer308);
	
	Centre (wxBOTH);
	
}	// end "CreateControls"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//	Called By:			GaussianParameterDialog in SDisplayMultispectral.cpp
//
//	Coded By:			Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:			Larry L. Biehl			Date: 11/07/2018

Boolean CMGaussianParameterDlg::DoDialog(
				double*								gaussianStretchPtr)

{  
	SInt16			returnCode;

	Boolean			continueFlag = FALSE;
	 
	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																							return (FALSE);
																			
	m_gaussianStretch = *gaussianStretchPtr;																		 
																					
	returnCode = ShowModal ();
	
	if (returnCode == wxID_OK)
		{   		
		*gaussianStretchPtr = m_gaussianStretch;																		 
		 
		continueFlag = TRUE;
		
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



void CMGaussianParameterDlg::OnInitDialog (
				wxInitDialogEvent& 				event)

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
   wxTextCtrl* std_stretch = (wxTextCtrl*)FindWindow (IDC_GaussianStretch);
   std_stretch->ChangeValue (wxString::Format (wxT("%.2f"), m_gaussianStretch));
	
   return true;
	
}	// end "TransferDataToWindow"
