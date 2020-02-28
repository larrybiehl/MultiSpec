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
//	File:						xFeatureSelectionDialog.h
//	Implementation:		xFeatureSelectionDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMFeatureSelectionDialog and
//								CMFeatureSelectionListDialog classes
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 02/24/2020
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMFeatureSelectionDialog : public CMDialog
{
		 // Construction
	public:
				// standard constructor
		CMFeatureSelectionDialog (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = IDD_FeatureSelection,
				const wxString& 					title =
													wxT("Set Feature Selection Specifications"));

		~CMFeatureSelectionDialog (); // standard destructor

		Boolean DoDialog (
				SeparabilitySpecsPtr 			separabilitySpecsPtr,
				UInt16* 								allChanCombinationsPtr);


				// Dialog Data

		enum { IDD = IDD_FeatureSelection };
			
		int 									m_channelCombinationSelection,
			 									m_classPairWeightsSelection,
			 									m_localCombinationsToList,
			 									m_savedContiguousChannelsPerGroup,
			 									m_separabilityListSelection;
	
		Boolean								m_diskFileFlag,
			 									m_searchFlag,
			 									m_textWindowFlag;

			// Implementation
	protected:
				// Generated message map functions
		void OnChangeChannelsPerGroup (
				wxCommandEvent&					event);

		void OnChangeNumberBestToList (
				wxCommandEvent&					event);

		void OnChannelComboSelendok (
				wxCommandEvent&					event);

		void OnClassComboSelendok (
				wxCommandEvent&					event);

		void OnClassPairWeightsComboSelendok (
				wxCommandEvent&					event);
	
		void OnDistanceMeasureComboSelendok (
				wxCommandEvent&					event);

		void OnFeatureTransformation (
				wxCommandEvent&					event);

		void OnHelpButton (
				wxCommandEvent&					event);

		void OnInitDialog (
				wxInitDialogEvent& 				event);

		void OnListOptions (
				wxCommandEvent&					event);

		void OnNumberChannelsComboCloseUp (
				wxCommandEvent&					event);

		void OnNumberChannelsComboDropDown (
				wxCommandEvent&					event);

		void OnNumberChannelsComboSelendok (
				wxCommandEvent&					event);

		void OnStepProcedure (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ()
	
		SInt16 CheckValues (
				int 									controlID,
				SInt16 								minValue,
				SInt16 								maxValue,
				SInt32* 								newValue);
	
		void CreateControls ();
		
		void HandleChannelCombinationsMenu ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
					  
		SeparabilitySpecsPtr				m_separabilitySpecsPtr;
	
		SInt16								*m_localClassPairWeightsListPtr;
	
		UInt16								*m_allChanCombinationsPtr,
												*m_localChannelCombinationsPtr;

		wxString 							m_channelgroupString;
		wxString 							m_numBestString;

		wxBoxSizer							*bSizer211;
	
		wxButton								*m_button46,
												*m_button47,
												*m_button48;
	
		wxCheckBox							*m_checkBox47,
												*m_checkBox48,
												*m_checkBox49,
												*m_checkBox50;
	
		wxChoice								*m_distanceMeasureCtrl;
	
		wxComboBox							*m_comboBox40;
	
		wxStaticText						*m_staticText213,
												*m_staticText214,
												*m_staticText215,
												*m_staticText216,
												*m_staticText217,
												*m_staticText218,
												*m_staticText219,
												*m_staticText221,
												*m_staticText222,
												*m_staticText223,
												*m_staticText224,
												*m_staticText225,
												*m_staticText226,
												*m_staticText228,
												*m_staticText229;
	
		wxTextCtrl							*m_textCtrl120,
												*m_textCtrl121;

		SInt32								m_maxContiguousChannelsPerGroup,
												m_numberClassCombinations;

		SInt16								m_localDefaultClassPairWeight,
												m_separabilityDistance;

		UInt16								m_localNumberChannelGroupCombinations,
												m_savedNumberChannelGroupCombinations;

};	// end "class CMFeatureSelectionDialog"


//------------------------------------------------------------------------------------

class CMFeatureSelectionListDialog : public CMDialog
{
		 // Construction
	public:
				// standard constructor
		CMFeatureSelectionListDialog (
		 		wxWindow* 							pParent = NULL,
		 		wxWindowID 							id = IDD_ClassifyDialog,
		 		const wxString& 					title =
													wxT("Set Feature Selection List Options"));

		Boolean DoDialog (
				SeparabilitySpecsPtr 			separabilitySpecsPtr,
				SInt32* 								combinationsToListPtr);


		enum { IDD = IDD_FeatureSelectionListOptions };

			// Implementation
	protected:
		void OnHelpButton (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();


		wxString 							m_maxdistrangeString,
					 							m_mindistrangeString,
					 							m_numberbestString;
	
		SeparabilitySpecsPtr 			m_separabilitySpecsPtr;

		SInt32* 								m_combinationsToListPtr;
	
		wxBoxSizer							*bSizer226;

		wxCheckBox							*m_checkBox52,
												*m_checkBox51,
												*m_checkBox53,
												*m_checkBox54;

		wxRadioButton						*m_radioBtn24,
												*m_radioBtn25,
												*m_radioBtn26;
	
		wxStaticText						*m_staticText230,
												*m_staticText231,
												*m_staticText232,
												*m_staticText233,
												*m_staticText234,
												*m_staticText235,
												*m_staticText236,
												*m_staticText237,
												*m_staticText238;

		wxTextCtrl							*m_textCtrl122,
												*m_textCtrl123,
												*m_textCtrl124,
												*m_textCtrl125,
												*m_textCtrl126;
	
		double 								m_localDistancesLessThanToGroup,
												m_localDistancesLessThanToList,
												m_localMaxDistanceRangeToList,
												m_localMinDistanceRangeToList;
	
		int 									m_localCombinationsToList,
			 									m_sortChannelCombinations;
	
		Boolean								m_listClassPairDistancesFlag,
			 									m_separabilityTableFlag,
			 									m_thresholdedGroupTableFlag,
			 									m_thresholdedClassPairTableFlag;
	
};	// end "class CMFeatureSelectionListDialog"
