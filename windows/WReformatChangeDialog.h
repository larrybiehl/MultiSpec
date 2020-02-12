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
//	File:						WReformatChangeDialog.h
//	Implementation:		WReformatChangeDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMChangeFormatDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 07/03/2018
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDialog.h"
#include "afxwin.h"


class CMChangeFormatDlg : public CMDialog
{
	// Construction
	public:
		CMChangeFormatDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr);

		// Dialog Data
		//{{AFX_DATA (CMChangeFormatDlg)
		enum { IDD = IDD_FileFormatChange };
		int									m_bandInterleaveSelection,
												m_channelSelection,
												m_dataValueListSelection,
												m_headerListSelection,
												m_outputFileSelection;
	
		BOOL									m_invertBottomToTopFlag,
												m_invertLeftToRightFlag,
												m_outputInWavelengthOrderFlag,
												m_swapBytesFlag,
												m_transformDataFlag,
												m_writeChanDescriptionFlag;
		//}}AFX_DATA


	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		void SetChannelDescriptionFlag (void);
	

		// Generated message map functions
		//{{AFX_MSG (CMChangeFormatDlg)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnOutputInWavelengthOrder ();
	
		afx_msg void OnSelendokBandInterleave ();
	
		afx_msg void OnSelendokDataValueType ();
	
		afx_msg void OnSelendokHeader ();
	
		afx_msg void OnSelendokOutChannels ();
	
		afx_msg void OnTransformData ();
	
		afx_msg void OnWriteChanDescriptions ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()

		UInt8									m_inputBandInterleaveString[64],
												m_inputDataValueTypeString[64],
												m_tiffMenuNameString[16];
	
		static ReformatOptionsPtr		s_reformatOptionsPtr;

		int									m_headerOptionsSelection;
	
		SInt16								m_dataValueTypeSelection,
												m_eightBitsPerDataSelection,
												m_noTransformDataValueTypeSelection,
												m_savedDataValueTypeSelection;
	
		Boolean								m_channelDescriptionAllowedFlag,
												m_channelThematicDisplayFlag,
												m_dataValueTypeSelectionFlag,
												m_GAIAFormatAllowedFlag,
												m_initializedFlag,
												m_outputInWavelengthOrderAllowedFlag,
												m_savedChannelDescriptionFlag,
												m_sessionUserSetDataValueTypeSelectionFlag;
	
};	// end class CMChangeFormatDlg
