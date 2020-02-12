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
//	File:						xStatusDialog.h
//	Implementation:		xStatusDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CShortStatusDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 01/02/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"

#include "xDialog.h"
#include "xImageView.h"
#include "wx/wx.h"


class CShortStatusDlg : public CMDialog
{
	DECLARE_DYNAMIC_CLASS (CShortStatusDlg)
		// Construction
	public:
		CShortStatusDlg ();
	
		 		// standard constructor
		CShortStatusDlg (
		 		UInt16 								identifier,
		 		wxWindow* 							pParent = NULL);

				// Dialog Data

		enum { IDD = IDD_GraphicsStatus };
	
		Boolean								m_canceledCommandInitiatedFlag,
												m_initializedFlag;
	
			// Implementation
	protected:
		void OnCancel (
				wxCommandEvent& 					event);
	
		void OnCharHook (
				wxKeyEvent& 						event);
	
		void OnClose (
				wxCloseEvent& 						event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnKeyDown (
				wxKeyEvent& 						event);
		DECLARE_EVENT_TABLE ()

		void CreateControl ();
	
		void CreateControl_Graphics ();
	
		void CreateControl_Short ();
	

		wxBoxSizer							*bSizer35,
												*bSizer93,
												*bSizer195;
	
		wxButton								*m_button6;

		wxGauge								*m_gauge3;
	
		wxStaticText						*m_staticText39,
												*m_staticText40,
												*m_staticText41,
												*m_staticText42,
												*m_staticText43,
												*m_staticText44,
												*m_staticText45,
												*m_staticText46,
												*m_staticText47,
												*m_staticText48,
												*m_staticText49,
												*m_staticText50,
												*m_staticText51,
												*m_staticText52,
												*m_staticText53,
												*m_staticText54,
												*m_staticText56,
												*m_staticText57,
												*m_staticText58,
												*m_staticText59,
												*m_staticText105,
												*m_staticText106,
												*m_staticText308,
												*m_staticText309,
												*m_staticText310,
												*m_staticText311,
												*m_staticText312;
	
		UInt16 								m_identifier;

};	// end "class CShortStatusDlg"
