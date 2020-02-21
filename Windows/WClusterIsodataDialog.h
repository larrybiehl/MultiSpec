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
//	File:						WClusterIsodataDialog.h
//	Implementation:		WClusterIsodataDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMISODATAClusterDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/2019
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMISODATAClusterDialog : public CMDialog
{
	// Construction
	public:
		CMISODATAClusterDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		~CMISODATAClusterDialog ();		// standard desctructor
	
		Boolean DoDialog (void);

		// Dialog Data
		//{{AFX_DATA (CMISODATAClusterDialog)
		enum { IDD = IDD_ISODATACluster };
	
		double								m_convergence,
												m_criticalDistance1,
												m_criticalDistance2;
	
		int									m_clustersFrom,
												m_initializationOption;
	
		long									m_minClusterSize,
												m_numberClusters;
	
		BOOL									m_projectClassMeansFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMISODATAClusterDialog)
		afx_msg void OnClusterTrainingAreas ();
	
		afx_msg void OnClusterImageArea ();
	
		afx_msg void OnEigenvectorVolume ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnOnePassCluster ();
	
		afx_msg void OnProjectClassMeans ();
	
		afx_msg void OnSelendokClassCombo ();
	
		afx_msg void On1stCovEigenvector ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		void ClustersFromSetting (
				CDataExchange* 					pDX,
				int 									nIDC,
				int& 									value);
	
		void UpdateOptionSettings (void);


		UInt16								m_distanceDecimalDigits;
	
		Boolean								m_initializedFlag;
	
};	// end class CMISODATAClusterDialog
