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
//	File:						WDisplayMultispectralDialog.h
//	Implementation:		WDisplayMultispectralDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMDisplaySpecsDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 11/07/2019
//	
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMDisplaySpecsDlg : public CMDialog
{                         
	// Construction
	public:
		CMDisplaySpecsDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		~CMDisplaySpecsDlg ();		// standard desctructor
	
		Boolean DoDialog (
				DisplaySpecsPtr					displaySpecsPtr);

		// Dialog Data
		//{{AFX_DATA (CMDisplaySpecsDlg)
		enum { IDD = IDD_DisplaySpecs };
	
		CString								m_FileName;
	
		double								m_Magnification,
												m_thematicLegendFactor;
	
		int									m_BitsOfColor,
												m_DisplayType,
												m_Enhancement,
												m_MinMaxPopupCode,
												m_TreatZeroAs;
	
		UINT									m_BlueChannel,
												m_GrayChannel,
												m_GreenChannel,
												m_NumberLevels,
												m_RedChannel;
	
		BOOL									m_blueChannelInvertFlag,
												m_ComputeHistogramFlag,
												m_greenChannelInvertFlag,
												m_redChannelInvertFlag,
												m_vectorOverlaysFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMDisplaySpecsDlg)
		afx_msg void OnBnClickedCheck1 ();
	
		afx_msg void OnChangeBlueChannel ();
	
		afx_msg void OnChangeGrayChannel ();
	
		afx_msg void OnChangeGreenChannel ();
	
		afx_msg void OnChangeRedChannel ();
	
		afx_msg void OnChannelDescriptions ();
	
		afx_msg void OnCloseupEnhancement ();
	
		afx_msg void OnDropdownBitsOfColor ();
	
		afx_msg void OnDropdownEnhancement ();
	
		afx_msg void OnEnChangeLegendfactor ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnSelendokBitsOfColor ();
	
		afx_msg void OnSelendokChannels ();
	
		afx_msg void OnSelendokDisplayType ();
	
		afx_msg void OnSelendokEnhancement ();
	
		afx_msg void OnSelendokMinMaxValues ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		void CheckChannel (
				UInt16								itemNumber,
				UINT*									channelValuePtr);
	
		void CheckDisplayLevels (
				DialogPtr							dialogPtr,
				UInt16								displayType);
	
		Boolean GetComputeHistogramDialogSetting (
				DisplaySpecsPtr					displaySpecsPtr,
				SInt16								lEnhancement,
				SInt16								lMinMaxCode);
	
		UInt16 GetMaximumDisplayLevels (
				SInt16								bitsOfColorIndex,
				SInt16								displayType,
				SInt16								duplicateChannelCode);
	
		void SetComputeHistogramDialogItem (
				Boolean								computeHistogramFlag,
				DialogPtr							dialogPtr,
				SInt16								itemNumber);
	
	
		DisplaySpecsPtr					m_displaySpecsPtr;

		double								mMinMaxValues[6],
												m_gaussianStretch;
	
				// This is used as the index into the number of levels array
				// to allow for the Windows version having only 8 and 24 bit
				// color choices.
		SInt16								mBitsOfColorIndex,
		
												mDuplicateChannelCode,
												mMaxSystemPixelSize,
												mRGBColors,
												m_MinMaxCode,
												m_PercentClip,
												m_SavedBitsOfColor;
	
		UInt16								mColorLevelsMax[3][3],
												mLocalDisplayType,
												mMaxChannelNumber,
												mPixelSizeVector[3];
	
		Boolean								m_initializedFlag,
												m_optionKeyFlag,
												m_thematicDisplayWithUserSettingFlag;
	
};	// end class CMDisplaySpecsDlg
