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
//	File:						xChangeChannelDescriptionDialog.h :
//															CMChangeChannelDescriptionDlg Definition
//	Implementation:		xChangeChannelDescriptionDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMChangeChannelDescriptionDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2013
//	Revised By:				Larry L. Biehl			Date: 01/12/2018
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMChangeChannelDescriptionDlg : public CMDialog
{
		 // Construction
	public:
		 CMChangeChannelDescriptionDlg (
				wxWindow*							pParent = NULL,
				wxWindowID							id = wxID_ANY,
				const wxString&					title = wxT("Channel Descriptions"));
		
		 ~CMChangeChannelDescriptionDlg (); // standard desctructor  
	
		 SInt16 DoDialog (
				FileInfoPtr							fileInfoPtr,
				ChannelDescriptionPtr			channelDescriptionPtr,
				float*								channelValuesPtr);
	
	
		 enum {IDD = IDD_ChangeChannelDescriptions};
	
	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);

		void OnBnClickedAddModifyChannelDescriptionRadio (
				wxCommandEvent& 					event);
	
		void OnBnClickedDeleteChannelDescriptionRadio (
				wxCommandEvent&					event);

		void OnNextChannelButton (
				wxCommandEvent&					event);
				
		void OnPreviousChannelButton (
				wxCommandEvent&					event);

		void OnSetfocusDescription (
				wxFocusEvent& 						event);
	
		void OnSetfocusValue (
				wxFocusEvent& 						event);
		DECLARE_EVENT_TABLE ();

		void CreateControls ();

		void OnOK ();
		
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
		
		void Update ();
		 
		wxString								m_description;
		
		ChannelDescriptionPtr			m_channelDescriptionPtr;
		FileInfoPtr							m_fileInfoPtr;
		float*								m_channelValuesPtr;

		wxButton								*m_button84,
												*m_button85,
												*m_button88,
												*m_button89;
		
		wxRadioButton						*m_radioBtn38,
												*m_radioBtn39;
	
		wxStaticText						*m_staticText306,
												*m_staticText324,
												*m_staticText325,
												*m_staticText326,
												*m_staticText327,
												*m_staticText328,
												*m_staticText329;
	
		wxTextCtrl							*m_textCtrl143,
												*m_textCtrl144;
	
		float									m_value;
	
		SInt16								m_addModifiyDeleteCode,
												m_channelIndex,
												m_lastSelectedTextItem;
													
		Boolean								m_changeFlag,
												m_initializedFlag;
		 
};	// end "class CMChangeChannelDescriptionDlg"
