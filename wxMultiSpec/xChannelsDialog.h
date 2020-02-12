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
//	File:						xChannelsDialog.h : CMChannelsDlg Definition
//	Implementation:		xChannelsDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMChannelsDlg and CMChannelCombinationsDlg
//								classes
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2013
//	Revised By:				Larry L. Biehl			Date: 01/12/2018
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xOneColumnDialog.h"


//------------------------------------------------------------------------------------
// CMChannelsDlg dialog   

class CMChannelsDlg : public CMOneColDlg
{
		 // Construction
	public:
		CMChannelsDlg (
				wxWindow* 							pParent = NULL); // standard constructor

		bool DoDialog (
				SInt16*								numberOutputFeaturesPtr,
				SInt16*								selectedFeaturePtr,
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16								listType,
				Boolean								useTransformFlag,
				SInt16*								availableFeaturePtr,
				SInt16								numberInputChannels,
				SInt16								currentSelection);
	
		 // Implementation
	protected:
		
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ();

		FileInfoPtr							m_fileInfoPtr;
		LayerInfoPtr						m_layerInfoPtr;
		SInt16*								m_availableFeaturePtr;

		bool									m_initializedFlag,
												m_useTransformFlag;

}; // end "class CMChannelsDlg"



//------------------------------------------------------------------------------------
// CMChannelCombinationsDlg dialog   

class CMChannelCombinationsDlg : public CMOneColDlg
{
		 // Construction
	public:
		CMChannelCombinationsDlg (
				wxWindow* 							pParent = NULL); // standard constructor

		bool DoDialog (
				SInt16								channelCombinationSelection,
				UInt16*								numberOutputChannelCombinationsPtr,
				UInt16*								channelCombinationsPtr,
				UInt32								numberInputChannelCombinations,
				UInt16*								allChanCombinationsPtr,
				UInt32								contiguousChannelsPerGroup,
				Boolean								useTransformFlag);

			// Implementation
	protected:

				// Generated message map functions
		void OnInitDialog (
				wxInitDialogEvent& 				event);

		DECLARE_EVENT_TABLE ();
		 
		SInt16*								m_availableFeaturePtr;

		SInt16								m_channelCombinationSelection,
												m_contiguousChannelsPerGroup;

		bool									m_initializedFlag,
												m_useTransformFlag;

};	// end "class CMChannelCombinationsDlg"
