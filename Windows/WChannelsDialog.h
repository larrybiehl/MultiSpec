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
//	File:						WChannelsDialog.h
//	Implementation:		WChannelsDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMChannelsDlg and CMChannelCombinationsDlg
//								classes
//
//	Written By:				Larry L. Biehl			Date: ??/??/1990's
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WOneColumnDialog.h"


//------------------------------------------------------------------------------------
// CMChannelsDlg dialog

class CMChannelsDlg : public CMOneColDlg
{
	// Construction
	public:
		CMChannelsDlg ( 	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				SInt16*								numberOutputFeaturesPtr,
				SInt16* 								selectedFeaturePtr,
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16								listType,
				Boolean								useTransformFlag,
				SInt16* 								availableFeaturePtr,
				SInt16								numberInputChannels,
				SInt16								currentSelection);

		// Dialog Data
		//{{AFX_DATA (CMChannelsDlg)
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMChannelsDlg)
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		FileInfoPtr							m_fileInfoPtr;
		LayerInfoPtr						m_layerInfoPtr;
		SInt16* 								m_availableFeaturePtr;
	
		Boolean								m_initializedFlag,
												m_useTransformFlag;
	
};	// end "CMChannelsDlg" 


                     
//------------------------------------------------------------------------------------
// CMChannelCombinationsDlg dialog

class CMChannelCombinationsDlg : public CMOneColDlg
{
	// Construction
	public:
		CMChannelCombinationsDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				SInt16								channelCombinationSelection,
				UInt16*								numberOutputChannelCombinationsPtr,
				UInt16* 								channelCombinationsPtr,
				UInt32								numberInputChannelCombinations,
				UInt16*								allChanCombinationsPtr,
				UInt32								contiguousChannelsPerGroup,
				Boolean								useTransformFlag);

		// Dialog Data
		//{{AFX_DATA (CMChannelCombinationsDlg)
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMChannelCombinationsDlg)
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		SInt16* 								m_availableFeaturePtr;
	
		SInt16								m_channelCombinationSelection,
												m_contiguousChannelsPerGroup;
	
		Boolean								m_initializedFlag,
												m_useTransformFlag;
	
};	// end "CMChannelCombinationsDlg"                     
