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
//	File:						xFalseColorDialog.h
//	Implementation:		xFalseColorDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMFalseColorDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 10/03/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "xDialog.h"


class CMFalseColorDlg : public CMDialog
{
			// Construction
	public:
				// standard constructor
		CMFalseColorDlg (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = IDS_ProcessorBiPlot,
				const wxString& 					title =
												wxT("Assign Channels for False Color Palette"));
	
		Boolean DoDialog (void);

				// Dialog Data
	
		enum { IDD = IDD_FalseColor };

			// Implementation
	protected:
				// Generated message map functions
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
		
		bool TransferDataToWindow ();


		FileInfoPtr 						m_fileInfoPtr;
		LayerInfoPtr 						m_layerInfoPtr;

		wxBoxSizer							*bVSizerMain;
	
		wxButton								*m_button78,
												*m_button79;
	
		wxListBox							*m_listBox5;
	
		wxStaticText						*m_staticText293,
												*m_staticText294,
												*m_staticText295,
												*m_staticText296;
	
		wxTextCtrl							*m_textCtrl132,
												*m_textCtrl133,
												*m_textCtrl134;
	
		long									m_blueChannel,
												m_greenChannel,
												m_redChannel;

		SInt32 								m_maxChannelFeatureNum;

		SInt16 								m_blueChannelIndex,
		 										m_greenChannelIndex,
				 								m_redChannelIndex;

		Boolean 								m_initializedFlag;
	
};	// end "class CMFalseColorDlg"
