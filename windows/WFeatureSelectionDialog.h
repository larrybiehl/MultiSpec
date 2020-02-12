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
//	File:						WFeatureSelectionDialog.h
//	Implementation:		WFeatureSelectionDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMFeatureSelectionDialog and
//								CMFeatureSelectionListDialog classes.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMFeatureSelectionDialog : public CMDialog
{
	// Construction
	public:
		CMFeatureSelectionDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		~CMFeatureSelectionDialog ();		// standard desctructor
	
		Boolean DoDialog (
				SeparabilitySpecsPtr				separabilitySpecsPtr,
				UInt16*								allChanCombinationsPtr);


		// Dialog Data
		//{{AFX_DATA (CMFeatureSelectionDialog)
		enum { IDD = IDD_FeatureSelection };
	
		int									m_channelCombinationSelection,
												m_interClassWeightsSelection,
												m_separabilityListSelection;
	
		long									m_localCombinationsToList,
												m_savedContiguousChannelsPerGroup;
	
		BOOL									m_diskFileFlag,
												m_searchFlag,
												m_textWindowFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		void HandleChannelCombinationsMenu (
				UInt16								channelCombinationsMenuItemNumber);

		// Generated message map functions
		//{{AFX_MSG (CMFeatureSelectionDialog)
		afx_msg void OnChangeChannelsPerGroup ();
	
		afx_msg void OnChangeNumberBestToList ();
	
		afx_msg void OnFeatureTransformation ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnListOptions ();
	
		virtual void OnOK ();
	
		afx_msg void OnSelendokChannelCombo ();
	
		afx_msg void OnSelendokClassCombo ();
	
		afx_msg void OnSelendokClassPairWeightsCombo ();
	
		afx_msg void OnSelendokDistanceMeasureCombo ();
	
		afx_msg void OnSelendokNumberChannelsCombo ();
	
		afx_msg void OnStepProcedure ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		SeparabilitySpecsPtr				m_separabilitySpecsPtr;

		SInt16								*m_localClassPairWeightsListPtr;
	
		UInt16								*m_allChanCombinationsPtr,
												*m_localChannelCombinationsPtr;
	
		SInt32								m_maxContiguousChannelsPerGroup,
												m_numberClassCombinations;
	
		SInt16								m_localDefaultClassPairWeight,
												m_separabilityDistance;
	
		UInt16								m_localNumberChannelGroupCombinations,
												m_savedNumberChannelGroupCombinations;
	
};	// end class CMFeatureSelectionDialog



//------------------------------------------------------------------------------------
// CMFeatureSelectionListDialog dialog

class CMFeatureSelectionListDialog : public CMDialog
{
	// Construction
	public:
		CMFeatureSelectionListDialog (	// standard constructor
				CWnd* 								pParent = NULL);

		Boolean DoDialog (
				SeparabilitySpecsPtr				separabilitySpecsPtr,
				SInt32*								combinationsToListPtr);

		// Dialog Data
		//{{AFX_DATA (CMFeatureSelectionListDialog)
		enum { IDD = IDD_FeatureSelectionListOptions };
	
		double								m_localDistancesLessThanToGroup,
												m_localDistancesLessThanToList,
												m_localMinDistanceRangeToList,
												m_localMaxDistanceRangeToList;
	
		int									m_sortChannelCombinations;
	
		long									m_localCombinationsToList;
	
		BOOL									m_listClassPairDistancesFlag,
												m_separabilityTableFlag,
												m_thresholdedClassPairTableFlag,
												m_thresholdedGroupTableFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMFeatureSelectionListDialog)
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		SeparabilitySpecsPtr				m_separabilitySpecsPtr;
	
		SInt32*								m_combinationsToListPtr;
	
};	// end class CMFeatureSelectionListDialog
