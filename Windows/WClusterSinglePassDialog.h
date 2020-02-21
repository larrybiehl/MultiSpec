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
//	File:						WClusterSinglePassDialog.h
//	Implementation:		WClusterSinglePassDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMSinglePassClusterDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/2019
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMSinglePassClusterDialog : public CMDialog
{
	// Construction
	public:
		CMSinglePassClusterDialog (	// standard constructor
				CWnd* 								pParent = NULL);
		
		~CMSinglePassClusterDialog ();		// standard desctructor
							
		Boolean DoDialog (void);

		// Dialog Data
		//{{AFX_DATA (CMSinglePassClusterDialog)
		enum { IDD = IDD_SinglePassCluster };
	
		double								m_criticalDistance1,
												m_criticalDistance2;
	
		int									m_clustersFrom;
	
		long									m_minClusterSize;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMSinglePassClusterDialog)
		afx_msg void OnClusterImageArea ();
	
		afx_msg void OnClusterTrainingAreas ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnSelendokClassCombo ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP () 
		
		void ClustersFromSetting (
				CDataExchange* 					pDX,
				int 									nIDC,
				int& 									value);

		UInt16								m_distanceDecimalDigits;
	
		Boolean								m_initializedFlag;
	
};	// end class CMSinglePassClusterDialog
