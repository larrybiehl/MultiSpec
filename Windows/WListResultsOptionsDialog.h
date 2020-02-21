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
//	File:						WListResultsOptionsDialog.h
//	Implementation:		WListResultsOptionsDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMListResultsOptionsDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMListResultsOptionsDlg : public CMDialog
{
	// Construction
	public:
		CMListResultsOptionsDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		void DoDialog (
				SInt16*								listResultsTrainingCodePtr,
				SInt16*								listResultsTestCodePtr);

		// Dialog Data
		//{{AFX_DATA (CMListResultsOptionsDlg)
		enum { IDD = IDD_ResultsListOptions };
	
		BOOL									m_testAreaPerformanceFlag,
												m_testAreaSummariesFlag,
												m_testAreasUsedFlag,
												m_testClassPerformanceFlag,
												m_trainAreaPerformanceFlag,
												m_trainAreaSummariesFlag,
												m_trainAreasUsedFlag,
												m_trainClassPerformanceFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMListResultsOptionsDlg)
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		SInt16								m_listResultsTestCode,
												m_listResultsTrainingCode;
	
};	// end class CMListResultsOptionsDlg
