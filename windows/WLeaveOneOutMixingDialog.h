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
//	File:						WLeaveOneOutMixingDialog.h
//	Implementation:		WLeaveOneOutMixingDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMLOOMixingDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMLOOMixingDialog : public CMDialog
{
	// Construction
	public:
		CMLOOMixingDialog (	// standard constructor
				CWnd* 								pParent = NULL);
				

		// Dialog Data
		//{{AFX_DATA (CMLOOMixingDialog)
		enum { IDD = IDD_LOOMixingParameter };
	
		double	m_userMixingParameter;
	
		int		m_mixingParameterCode;
		//}}AFX_DATA

		Boolean DoDialog (
				SInt16								statsWindowMode);


	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMLOOMixingDialog)
		afx_msg void OnIdentityMatrix ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnOptimumMixing ();
	
		afx_msg void OnUserMixing ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	

		double								m_loocMixingParameter;

		SInt16								m_statsWindowMode;

};	// end class CMLOOMixingDialog
