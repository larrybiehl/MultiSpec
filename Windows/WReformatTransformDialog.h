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
//	File:						WReformatTransformDialog.h
//	Implementation:		WReformatTransformDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMReformatRectifyDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 05/06/2020
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDialog.h"


class CMReformatTransformDlg : public CMDialog
{
	// Construction
	public:
 		CMReformatTransformDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		~CMReformatTransformDlg ();
	
		Boolean DoDialog (
				UInt16*								recommendNumberOfBitsPtr,
				SInt16								bandInterleaveSelection);

	// Dialog Data
		//{{AFX_DATA (CMReformatTransformDlg)
		enum { IDD = IDD_ReformatTransform };
	
		CString								m_denominatorString,
												m_numeratorString;
	
		double								m_adjustDivisor,
												m_adjustFactor,
												m_adjustOffset,
												m_adjustSelectedChannelsFactor,
												m_functionFactor,
												m_radianceMult,
												m_radianceOffset,
												m_scaleFactor,
												m_thermalK1,
												m_thermalK2,
												m_transformFactor,
												m_transformOffset;
	
		int									m_defaultThermalChannel,
												m_functionCode,
												m_transformCode,
												m_algebraicTransformOption;
	
		UINT									m_adjustSelectedChannel,
												m_kthSmallestElement,
												m_minimumNumberBits,
												m_minSelectedNumberBits;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		void ShowHideAdjustChannelItems (
				DialogPtr         				dialogPtr,
				Boolean								showFlag);

		void ShowHideAdjustSelectedChannelsByChannelItems (
				DialogPtr							dialogPtr,
				Boolean								showFlag);
	
		void ShowHideAlgebraicTransformItems (
				DialogPtr         				dialogPtr,
				Boolean								showFlag);

		void ShowHideAlgebraicTransformThermItems (
				DialogPtr 							dialogPtr,
				Boolean 								showFlag);
	
		void ShowHideFunctionChannelsItems (
				DialogPtr							dialogPtr,
				Boolean								showFlag,
				UInt16								functionChannelCode);

		void ShowHidePCTransformItems (
				DialogPtr         				dialogPtr,
				Boolean								showFlag,
				Boolean								pcButtonFlag);
	
		// Generated message map functions
		//{{AFX_MSG (CMReformatTransformDlg)
		afx_msg void OnAdjustSelectedChannels ();
	
		afx_msg void OnAdjustSelectedChannelsByChannel ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnRTAlgebraicTransformation ();
	
		afx_msg void OnRTEigenvectors ();
	
		afx_msg void OnRTFunctionOfChannels ();
	
		afx_msg void OnRTNoTransformation ();

		afx_msg void OnSelendokAlgebraicTransformOptions ();
	
		afx_msg void OnSelendokEVEigenvectors ();
	
		afx_msg void OnSelendokReformatFunctions ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		TBYTE									*m_denominatorStringPtr,
												*m_numeratorStringPtr;
	
		double								m_maxValue,
												m_minValue;
	
		SInt16								m_bandInterleaveSelection,
												m_eigenSource,
												m_maxAdjustOffset;

		UInt16								m_maxAdjustDivisor,
												m_maxAdjustFactor,
												m_maxChannel,
												m_numberEigenvectors;
	
		Boolean								m_initializedFlag;
		
};	// end class CMReformatTransformDlg
