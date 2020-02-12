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
//	File:						WStatisticsDialog.h
//	Implementation:		WStatisticsDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMStatisticsDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/13/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDialog.h"


class CMStatisticsDialog : public CMDialog
{
	// Construction
	public:
		CMStatisticsDialog (	// standard constructor
				CWnd* 								pParent = NULL);
		
		SInt16 DoDialog (
				SInt16*								featurePtr,
				SInt16								totalNumberChannels,
				Handle*								trainMaskFileInfoHandle,
				Handle*								testMaskFileInfoHandle);
				
		// Dialog Data
		//{{AFX_DATA (CMStatisticsDialog)
		enum { IDD = IDD_StatisticsDialog };
	
		int									m_outlineColorSelection,
												m_projectCommands,
												m_testMaskCombo,
												m_trainMaskCombo;
	
		BOOL									m_showClassNames,
												m_showFieldNames,
												m_showTestFields,
												m_showTrainingFields,
												m_showTrainTestText;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
		
		void SetOutlineAreaOptions (void);

		// Generated message map functions
		//{{AFX_MSG (CMStatisticsDialog)
		afx_msg void OnDropdownChannels ();
	
		afx_msg void OnDropdownProjectChanges ();
	
		afx_msg void OnDropdownTestMaskCOMBO ();
	
		afx_msg void OnDropdownTrainMaskCOMBO ();
	
		virtual BOOL OnInitDialog ();
	
		virtual void OnOK ();
	
		afx_msg void OnSelendokChannels ();
	
		afx_msg void OnSelendokProjectChanges ();
	
		afx_msg void OnSelendokTestMaskCOMBO ();
	
		afx_msg void OnSelendokTrainMaskCOMBO ();
	
		afx_msg void OnShowClassNames ();
	
		afx_msg void OnShowFieldNames ();
	
		afx_msg void OnShowTrainTestLabel ();
	
		afx_msg void OnStatisticsOptions ();
	
		afx_msg void OnTestFields ();
	
		afx_msg void OnTrainingFields ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		SInt16*								m_featurePtr;
	
		double								m_minLogDeterminant,
												m_variance;
								
		Handle								m_testMaskFileInfoHandle,
												m_trainMaskFileInfoHandle;
		
		SInt16								m_localStatCode,
												m_totalNumberChannels;

		UInt16								m_maxNumberTestLayers,
												m_maxNumberTrainLayers;
		
		Boolean								m_initializedFlag,
												m_keepClassStatsFlag,
												m_useCommonCovarianceInLOOCFlag;
	
};	// end class CMStatisticsDialog
