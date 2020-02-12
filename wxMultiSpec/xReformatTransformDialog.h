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
//	File:						xReformatTransformDialog.h
//	Implementation:		xReformatTransformDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMReformatTransformDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 02/10/2020
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"
#include "xOneColumnDialog.h"


class CMReformatTransformDlg : public CMDialog
{
		 // Construction
	public:
				// standard constructor
		CMReformatTransformDlg (
				ReformatOptionsPtr				reformatOptionsPtr,
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = wxID_ANY,
				const wxString& 					title =
													wxT("Set Reformat Transform Parameters"));
	
		~CMReformatTransformDlg ();

		Boolean DoDialog (
				UInt16* 								recommendNumberOfBitsPtr,
				SInt16 								bandInterleaveSelection);


		enum { IDD = IDD_ReformatTransform };
	
	
		 // Implementation
	protected:
		void OnAdjustSelectedChannels (
				wxCommandEvent& 					event);
	
		void OnAdjustSelectedChannelsByChannel (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnRTAlgebraicTransformation (
				wxCommandEvent& 					event);
	
		void OnRTEigenvectors (
				wxCommandEvent& 					event);
	
		void OnRTFunctionOfChannels (
				wxCommandEvent& 					event);
	
		void OnRTNoTransformation (
				wxCommandEvent& 					event);
	
		void OnSelendokEVEigenvectors (
				wxCommandEvent& 					event);
	
		void OnSelendokReformatFunctions (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ();

		void CreateControls ();
	
		void ShowHideAdjustChannelItems (
				DialogPtr 							dialogPtr,
				Boolean 								showFlag);

		void ShowHideAdjustSelectedChannelsByChannelItems (
				DialogPtr							dialogPtr,
				Boolean 								showFlag);

		void ShowHideAlgebraicTransformItems (
				DialogPtr 							dialogPtr,
				Boolean 								showFlag);

		void ShowHideFunctionChannelsItems (
				DialogPtr 							dialogPtr,
				Boolean 								showFlag,
				UInt16 								functionChannelCode);

		void ShowHidePCTransformItems (
				DialogPtr 							dialogPtr,
				Boolean 								showFlag,
				SInt16								eigenSource);

		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();


		char 									m_denominatorStringPtr [256],
			 									m_numeratorStringPtr [256];
	
		wxString 							m_denominatorString,
			 									m_numeratorString;

		ReformatOptionsPtr				m_reformatOptionsPtr;

		wxBoxSizer							*bSizer84,
												*bSizer103;
	
		wxRadioButton						*m_radioBtn5,
												*m_radioBtn6,
												*m_radioBtn7,
												*m_radioBtn8,
												*m_radioBtn9,
												*m_radioBtn10;
	
		wxStaticText						*m_staticText127,
												*m_staticText128,
												*m_staticText129,
												*m_staticText130,
												*m_staticText131,
												*m_staticText132,
												*m_staticText133,
												*m_staticText134,
												*m_staticText135,
												*m_staticText136,
												*m_staticText137,
												*m_staticText140,
												*m_staticText141,
												*m_staticText142,
												*m_staticText143,
												*m_staticText144,
												*m_staticText145;
	
		wxTextCtrl							*m_textCtrl48,
												*m_textCtrl49,
												*m_textCtrl50,
												*m_textCtrl51,
												*m_textCtrl52,
												*m_textCtrl53,
												*m_textCtrl54,
												*m_textCtrl56,
												*m_textCtrl57,
												*m_textCtrl58,
												*m_textCtrl59,
												*m_textCtrl60;
	
		double 								m_adjustDivisor,
												m_adjustFactor,
												m_adjustOffset,
												m_adjustSelectedChannelsFactor,
												m_functionFactor,
												m_maxValue,
												m_minValue,
												m_scaleFactor,
												m_transformFactor,
												m_transformOffset;
	
		int 									m_functionCode,
												m_transformCode;
	
		UInt32								m_adjustSelectedChannel,
			 									m_kthSmallestElement,
			 									m_minimumNumberBits,
			 									m_minSelectedNumberBits;

		SInt16 								m_bandInterleaveSelection,
			 									m_eigenSource,
			 									m_maxAdjustOffset;
	
		UInt16 								m_maxAdjustDivisor,
			 									m_maxAdjustFactor,
			 									m_maxChannel,
			 									m_numberEigenvectors;

		Boolean 								m_initializedFlag;
	
};	// end "class CMReformatTransformDlg"
