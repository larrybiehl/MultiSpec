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
//	File:						WOverlayParameterDlg.h
//	Implementation:		WOverlayParameterDlg.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMOverlayParameterDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMOverlayParameterDlg : public CMDialog
{
	// Construction
	public:
		CMOverlayParameterDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
	
		Boolean DoDialog (
				WindowInfoPtr						windowInfoPtr,
				SInt16								overlayCode,
				SInt16								selection);

		// Dialog Data
		//{{AFX_DATA (CMOverlayParameterDlg)
		enum { IDD = IDD_OverlayParameters };
	
		int									m_overlayValue;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMOverlayParameterDlg)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnOverlayColor ();
	
		afx_msg void OnPaint ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()

		WindowInfoPtr						m_windowInfoPtr;

		int									m_lineThickness,
												m_maxValue,
												m_minValue,
												m_transparency;

		SInt16								m_overlayCode,
												m_overlayIndex;
	
		Boolean								m_initializedFlag;
	
};	// end class CMOverlayParameterDlg
