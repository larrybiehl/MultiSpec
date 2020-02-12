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
//	File:						WChangeChannelDesciptionDialog.h
//	Implementation:		WChangeChannelDesciptionDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMChangeChannelDescriptionDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1990's
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMChangeChannelDescriptionDlg : public CMDialog
{
	// Construction
	public:
		CMChangeChannelDescriptionDlg (// standard constructor
				CWnd* 								pParent = NULL);

		SInt16 DoDialog (
				FileInfoPtr							fileInfoPtr,
				ChannelDescriptionPtr			channelDescriptionPtr,
				float*								channelValuesPtr);

		// Dialog Data
		//{{AFX_DATA (CMChangeChannelDescriptionDlg)
		enum { IDD = IDD_ChangeChannelDescriptions };
	
		CString								m_description;
	
		float									m_value;
		//}}AFX_DATA

	protected:
		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL (CMChangeChannelDescriptionDlg)
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

		void	Update (void);

		// Generated message map functions
		//{{AFX_MSG (CMChangeChannelDescriptionDlg)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnNextChannelButton ();
	
		virtual void OnOK ();
	
		afx_msg void OnPreviousChannelButton ();
	
		afx_msg void OnSetfocusDescription ();
	
		afx_msg void OnSetfocusValue ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		ChannelDescriptionPtr			m_channelDescriptionPtr;
		FileInfoPtr							m_fileInfoPtr;
		float*								m_channelValuesPtr;

		SInt16								m_channelIndex,
												m_lastSelectedTextItem;
	
		Boolean								m_changeFlag,
												m_initializedFlag;

};	// end class CMChangeChannelDescriptionDlg
