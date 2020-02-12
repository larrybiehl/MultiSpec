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
//	File:						xOverlayParametersDialog.cpp : class implementation file
//	Class Definition:		xOverlayParametersDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			01/28/2019
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMOverlayParameterDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xOverlayParametersDialog.h"
#include "wx/valgen.h"
#include "wx/valnum.h"



BEGIN_EVENT_TABLE (CMOverlayParameterDlg, CMDialog)
	EVT_BUTTON (IDC_LineColorPrompt, CMOverlayParameterDlg::OnOverlayColor)
	EVT_INIT_DIALOG (CMOverlayParameterDlg::OnInitDialog)
END_EVENT_TABLE ()



CMOverlayParameterDlg::CMOverlayParameterDlg (
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title)
		: CMDialog (CMOverlayParameterDlg::IDD, pParent, title)

{
	m_overlayCode = 0;
	
	m_overlayValue = 1;

	m_windowInfoPtr = NULL;
	m_overlayIndex = 0;
	m_minValue = 0;
	m_maxValue = 20;

	m_initializedFlag = CMDialog::m_initializedFlag;
	CreateControls ();
	
}	// end "CMOverlayParameterDlg"



void CMOverlayParameterDlg::CreateControls ()

{
    SetSizeHints (wxDefaultSize, wxDefaultSize);

    bSizer107 = new wxBoxSizer (wxVERTICAL);

    wxFlexGridSizer* fgSizer7;
    fgSizer7 = new wxFlexGridSizer (2, 2, 0, 0);
    fgSizer7->SetFlexibleDirection (wxBOTH);
    fgSizer7->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);

    m_button23 = new wxButton (this,
    										IDC_LineColorPrompt,
    										wxT("Line Color..."),
    										wxDefaultPosition,
    										wxDefaultSize,
    										0);
    fgSizer7->Add (m_button23, 0, wxALL, 5);

    m_bitmap3 = new wxStaticBitmap (this,
    											IDC_LineColor,
    											wxNullBitmap,
    											wxDefaultPosition,
    											wxDefaultSize,
    											0);
    fgSizer7->Add (m_bitmap3, 0, wxALL, 5);

	m_staticText139 = new wxStaticText (this,
													IDC_ValuePrompt,
													wxT("Line Thickness:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText139->Wrap (-1);
	fgSizer7->Add (m_staticText139, 0, wxALIGN_CENTER | wxALL, 5);

	m_textCtrl55 = new wxTextCtrl (this,
    											IDC_LineThickness,
    											wxEmptyString,
    											wxDefaultPosition,
    											wxDefaultSize,
    											0);
	fgSizer7->Add(m_textCtrl55, 0, wxALL, 5);
	wxIntegerValidator<int> _val (&m_lineThickness);
	_val.SetRange (1, 20);
	m_textCtrl55->SetValidator (_val);

	bSizer107->Add (fgSizer7, 0, wxALL | wxEXPAND, 5);

	CreateStandardButtons (bSizer107);

	SetSizer (bSizer107);
	Layout ();

	Centre (wxBOTH);
	
}	// end "CreateControls"



//------------------------------------------------------------------------------------
//                   Copyright 2009-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the overlay
//							specification dialog box to the user and copy the
//							revised info to the overlay specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			OverlayControlDialog in SArcView.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/25/2009
//	Revised By:			Larry L. Biehl			Date: 07/09/2015	

Boolean CMOverlayParameterDlg::DoDialog (
				WindowInfoPtr 						windowInfoPtr,
				SInt16 								overlayCode,
				SInt16 								overlayIndex)

{
	SInt16 								returnCode;
	Boolean 								continueFlag = FALSE;


			// Make sure intialization has been completed.

	if (!m_initializedFlag)
																						return (FALSE);

	m_windowInfoPtr = windowInfoPtr;
	m_overlayCode = overlayCode;
	m_overlayIndex = overlayIndex;

	returnCode = ShowModal();

	if (returnCode == wxID_OK)
		{
		OverlayDialogOK (windowInfoPtr,
								 m_overlayCode,
								 m_overlayIndex,
								 &m_currentSelectedColor,
								 m_overlayValue);

		continueFlag = TRUE;

		}	// end "if (returnCode == IDOK)"

	return (continueFlag);

}	// end "DoDialog"



void CMOverlayParameterDlg::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	UInt16 								lineThickness,
											transparency;


	CMDialog::OnInitDialog (event);

	OverlayDialogInitialize (this,
										m_windowInfoPtr,
										m_overlayCode,
										m_overlayIndex,
										&m_currentSelectedColor,
										&lineThickness,
										&transparency);

			// Overlay color

	m_lineThickness = lineThickness;
	m_transparency = transparency;

	if (m_overlayCode == kImageOverlay)
		{
		m_overlayValue = m_transparency;
		m_minValue = 0;
		m_maxValue = 100;
		
		wxIntegerValidator<int> _val (&m_transparency);
		_val.SetRange (0, 100);
		m_textCtrl55->SetValidator (_val);

		}	// end "if (m_overlayCode == kImageOverlay)"

	else	// m_overlayCode != kImageOverlay
		{
		m_overlayValue = m_lineThickness;
		m_minValue = 1;
		m_maxValue = 20;

		}	// end "else m_overlayCode != kImageOverlay"

	if (TransferDataToWindow ())
		PositionDialogWindow ();

	SelectDialogItemText (this, IDC_LineThickness, 0, SInt16_MAX);
	SetSizerAndFit (bSizer107);

}	// end "OnInitDialog"



void CMOverlayParameterDlg::OnOverlayColor (
				wxCommandEvent& 					event)

{
	RGBColor 							newRGB;

    
	if (SelectColor (3,
            			&m_currentSelectedColor,
							&newRGB))
		{
		m_currentSelectedColor = newRGB;

		Update();

    	}	// end "if (SelectColor (3, ..."

	SelectDialogItemText(this, IDC_LineThickness, 0, SInt16_MAX);

}	// end "OnOverlayColor"



bool CMOverlayParameterDlg::TransferDataFromWindow ()

{
    wxTextCtrl* overlayValue = (wxTextCtrl*)FindWindow (IDC_LineThickness);
    m_overlayValue = wxAtoi (overlayValue->GetValue ());
	
    return true;
	
}	// end "TransferDataFromWindow"



bool CMOverlayParameterDlg::TransferDataToWindow ()

{
    wxTextCtrl* overlayValue = (wxTextCtrl*)FindWindow (IDC_LineThickness);
	
    overlayValue->ChangeValue (wxString::Format (wxT("%i"), m_overlayValue));
	
    return true;
	
}	// end "TransferDataToWindow"
