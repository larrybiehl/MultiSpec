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
//	File:						xReformatChangeDialog.h
//	Implementation:		xReformatChangeDialog.cpp
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMChangeChannelDescriptionDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2013
//	Revised By:				Larry L. Biehl			Date: 11/21/2019
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"
#include "SMultiSpec.h"
#include "wx/wx.h"


class CMChangeFormatDlg : public CMDialog
{
			// Construction
	public:
				// standard constructor
		CMChangeFormatDlg (
				ReformatOptionsPtr				reformatOptionsPtr,
				wxWindow* 							pParent,
				wxWindowID 							id = wxID_ANY,
				const wxString& 					title =
											wxT("Set Image File Format Change Specifications"));

		Boolean DoDialog (
				FileInfoPtr 						outFileInfoPtr);

		enum { IDD = IDD_FileFormatChange };

			// Implementation
	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnOutputInWavelengthOrder (
				wxCommandEvent& 					event);
	
		void OnSelendokBandInterleave (
				wxCommandEvent& 					event);
	
		void OnSelendokOutChannels (
				wxCommandEvent& 					event);
	
		void OnSelendokHeader (
				wxCommandEvent& 					event);
	
		void OnWriteChanDescriptions (
				wxCommandEvent& 					event);
	
		void OnSelendokDataValueType (
				wxCommandEvent& 					event);
	
		void OnTransformData (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		void SetChannelDescriptionFlag (void);
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	

		UInt8									m_inputBandInterleaveString[256],
												m_inputDataValueTypeString[256],
												m_tiffMenuNameString[256];

		ReformatOptionsPtr 				m_reformatOptionsPtr;
	
		wxBoxSizer							*bSizer124;
	
		wxChoice								*m_bandInterleaveCtrl,
												*m_dataValueTypeCtrl,
												*m_fileFormatCtrl,
												*m_outputFileCtrl;

		wxCheckBox							*m_checkBox41,
												*m_checkBox42,
												*m_checkBox43,
												*m_checkBox44,
												*m_checkBox45,
												*m_checkBox46;

		wxStaticText						*m_staticText136,
												*m_staticText137,
												*m_staticText138,
												*m_staticText139,
												*m_staticText140,
												*m_staticText141,
												*m_staticText142,
												*m_staticText143,
												*m_staticText144,
												*m_staticText145,
												*m_staticText146,
												*m_staticText147,
												*m_staticText148,
												*m_staticText161,
												*m_staticText162,
												*m_staticText163,
												*m_staticText164,
												*m_staticText165,
												*m_staticText166;
	
		int 									m_bandInterleaveSelection,
												m_channelSelection,
												m_dataValueListSelection,
												m_headerListSelection,
												m_outputFileSelection,
												m_headerOptionsSelection;

		SInt16 								m_dataValueTypeSelection,
												m_eightBitsPerDataSelection,
												m_savedDataValueTypeSelection;
	
		bool 									m_invertBottomToTopFlag,
			 									m_invertLeftToRightFlag,
			 									m_swapBytesFlag,
			 									m_transformDataFlag,
			 									m_writeChanDescriptionFlag;

		Boolean								m_channelDescriptionAllowedFlag,
												m_channelThematicDisplayFlag,
												m_dataValueTypeSelectionFlag,
												m_GAIAFormatAllowedFlag,
												m_initializedFlag,
												m_outputInWavelengthOrderFlag,
												m_outputInWavelengthOrderAllowedFlag,
												m_savedChannelDescriptionFlag,
												m_sessionUserSetDataValueTypeSelectionFlag;
	
};	// end "class CMChangeFormatDlg"
