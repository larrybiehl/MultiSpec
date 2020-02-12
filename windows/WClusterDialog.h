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
//	File:						WClusterDialog.h
//	Implementation:		WClusterDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMClusterDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/2019
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMClusterDialog : public CMDialog
{
	// Construction
	public:
		CMClusterDialog (	// standard constructor
				CWnd* 								pParent = NULL);
		
		~CMClusterDialog ();		// standard desctructor
							
		Boolean DoDialog (
				Boolean								newProjectFlag);

		// Dialog Data
		//{{AFX_DATA (CMClusterDialog)
		enum { IDD = IDD_Cluster };
	
		double								m_classifyThreshold,
												m_probabilityThreshold;
	
		int									m_classificationArea,
												m_clusterProcedure,
												m_maskFileFormatCode,
												m_saveStatisticsSelection,
												m_selectImageOverlaySelection,
												m_symbolSelection;
	
		BOOL									m_createClusterMaskFlag,
												m_createImageOverlayFlag,
												m_diskFileFlag,
												m_textWindowFlag,
												m_thresholdFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		// Generated message map functions
		//{{AFX_MSG (CMClusterDialog)
		afx_msg void OnClassifySelectedArea ();
	
		afx_msg void OnCreateMaskFile ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnImageOverlay ();
	
		afx_msg void OnISODATAAlgorithm ();
	
		virtual void OnOK ();
	
		afx_msg void OnSelendokChannelCombo ();
	
		afx_msg void OnSelendokImageOverlayCombo ();
	
		afx_msg void OnSelendokMaskFileDiskCombo ();
	
		afx_msg void OnSinglePassAlgorithm ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		void ClassifyAreaSetting (
				CDataExchange* 					pDX,
				int 									nIDC,
				int& 									value);
	
		void OnAlgorithm (
				Boolean								returnFlag);
	
		void UpdateClassificationItems (
				UInt16								classificationArea);
	
	
		double								mMaxClassifyThreshold;

		SInt32								m_transparencyValue;
		
		SInt16								m_channelListType;
		
		Boolean								m_initializedFlag,
												m_characterLimitDialogDisplayedFlag,
												m_clusterProcedureSetFlag,
												m_newProjectFlag;
	
};	// end class CMClusterDialog
