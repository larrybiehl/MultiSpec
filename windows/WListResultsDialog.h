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
//	File:						WListResultsDialog.h
//	Implementation:		WListResultsDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMListResultsDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMListResultsDialog : public CMDialog
{
	// Construction
	public:
		CMListResultsDialog (	// standard constructor
				CWnd* 								pParent = NULL);
		
		~CMListResultsDialog ();		// standard desctructor
		
		Boolean DoDialog (void);

		// Dialog Data
		//{{AFX_DATA (CMListResultsDialog)
		enum { IDD = IDD_ListResults };
	
		double								m_conversionFactor,
												m_thresholdPercent;
	
		int									m_tableType,
												m_tableUnits;
	
		BOOL									m_classSummaryFlag,
												m_diskFileFlag,
												m_fieldSummaryFlag,
												m_groupSummaryFlag,
												m_imageFlag,
												m_testFlag,
												m_textWindowFlag,
												m_thresholdResultsFlag,
												m_trainingFlag;
		//}}AFX_DATA


	// Implementation
	protected:
		void CheckListAreaItems ();
	
		void CheckSummaryItems ();
	
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
		
		void HideShowUnitsItems (
				Boolean								areaUnitsFlag);
		
		void SetListAreaItems ();
		
		void SetSummarizeByItems ();

		// Generated message map functions
		//{{AFX_MSG (CMListResultsDialog)
		afx_msg void OnAreaUnits ();
	
		afx_msg void OnClassSummary ();
	
		afx_msg void OnFieldSummary ();
	
		afx_msg void OnGroupSummary ();
	
		afx_msg void OnImage ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnPercentUnits ();
	
		afx_msg void OnSampleUnits ();
	
		afx_msg void OnSelendokClassCombo ();
	
		afx_msg void OnTest ();
	
		afx_msg void OnThresholdResults ();
	
		afx_msg void OnTraining ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		double								m_saveAreaConversionFactor,
												m_saveThresholdPercent;
	
		SInt16								m_localAreaCode,
												m_localSummaryCode,
												m_nonClusterFieldTypeCode;
		
		Boolean								m_initializedFlag,
												m_projectWindowFlag;
		
};	// end class CMListResultsDialog
