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
//	File:						xClusterSinglePassDialog.h
//	Implementation:		xClusterSinglePassDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMClassifyDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 10/08/2019
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"

class CMSinglePassClusterDialog : public CMDialog
{
	DECLARE_DYNAMIC_CLASS (CMSinglePassClusterDialog)
	
			// Construction
	public:
		CMSinglePassClusterDialog ();
	
				// standard constructor
		CMSinglePassClusterDialog (
				wxWindow*							parent,
				wxWindowID							id = IDD_Cluster,
				const wxString&					title = wxT("Set Cluster Specifications"));
	
				// standard desctructor
		~CMSinglePassClusterDialog ();
	
		Boolean DoDialog (void);

		enum {
			IDD = IDD_SinglePassCluster
		};

			// Implementation
	protected:
		void OnClusterImageArea (
				wxCommandEvent&					event);

		void OnClusterTrainingAreas (
				wxCommandEvent&					event);

		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnClassComboSelendok (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ()

		void ClustersFromSetting (
				bool									from,
				int									nIDC,
				int&									value);
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	

		wxString								m_minClusterSizeString,
												m_distance1String,
												m_distance2String;

		wxBoxSizer							*bSizer150;
		
		wxRadioButton						*m_radioBtn12,
												*m_radioBtn13;
		
		wxStaticText						*m_staticText189,
												*m_staticText190,
												*m_staticText191,
												*m_staticText204;
	
		wxTextCtrl							*m_textCtrl98,
												*m_textCtrl99,
												*m_textCtrl100;

		double								m_criticalDistance1,
												m_criticalDistance2;
	
		int									m_clustersFrom;
	
		SInt32								m_minClusterSize;

		Boolean								m_initializedFlag;

};	// end "class CMSinglePassClusterDialog"
