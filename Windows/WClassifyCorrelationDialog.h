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
//	File:						WClassifyCorrelationDialog.h
//	Implementation:		WClassifyCorrelationDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMCorrelationClassifyDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1990's
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMCorrelationClassifyDialog : public CMDialog
{
	// Construction
	public:
		CMCorrelationClassifyDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				SInt16*								covarianceEstimatePtr);

		// Dialog Data
		//{{AFX_DATA (CMCorrelationClassifyDialog)
		enum { IDD = IDD_SAMParameters };
	
		int									m_covarianceEstimate;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMCorrelationClassifyDialog)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnSelendokCovarianceCombo ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		Boolean								m_initializedFlag;
	
}; // end class CMCorrelationClassifyDialog
