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
//	File:						xEditSelectionDialog.h
//	Implementation:		xEditSelectionDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMEditCoordinatesDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 06/20/2017
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "xDialog.h"


class CMEditCoordinatesDlg : public CMDialog 
{
		 // Construction
	public:
				// standard constructor
		CMEditCoordinatesDlg (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id=wxID_ANY,
				const wxString& 					title= wxT("Edit Selection Rectangle"));

		Boolean DoDialog (
				WindowPtr 							windowPtr,
				WindowInfoPtr 						windowInfoPtr,
				LongRect* 							inputSelectionRectanglePtr,
				LongRect* 							selectionRectanglePtr,
				DoubleRect* 						coordinateRectanglePtr,
				SInt16 								pointType,
				SInt16* 								unitsDisplayCodePtr,
				Boolean* 							changedFlagPtr,
				Boolean* 							applyToAllWindowsPtr,
				Boolean* 							useStartLineColumnFlagPtr,
				Boolean* 							previewWasUsedFlagPtr,
				SInt16 								stringID);

				// Dialog Data

		enum {
			IDD = IDD_FieldCoordinates
			};

		// Implementation
	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnChangeNewColumnEnd (
				wxCommandEvent&					event);

		void OnChangeNewColumnStart (
				wxCommandEvent&					event);

		void OnChangeNewLineEnd (
				wxCommandEvent&					event);

		void OnChangeNewLineStart (
				wxCommandEvent&					event);

		void OnPreview (
				wxCommandEvent&					event);

		void OnSelendokCoordinateUnits (
				wxCommandEvent&					event);

		void OnApplyToAllCheckbox (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ()

		Boolean CheckLineColumnValues (
				UInt16* 								controlIDPtr);

		void CheckOKFlag (void);
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();

	
	
		DoubleRect							m_inputCoordinateRectangle,
												m_minMaxCoordinateRectangle;

		LongRect								m_minMaxSelectionRectangle;

		DoubleRect*							m_coordinateRectanglePtr;
	
		LongRect								*m_inputSelectionRectanglePtr,
												*m_selectionRectanglePtr;
	
		WindowInfoPtr						m_windowInfoPtr;
		WindowPtr							m_windowPtr;

		wxBoxSizer							*bSizer200;

		wxButton 							*m_button29,
												*m_button30,
												*m_button31;

		wxCheckBox 							*m_checkBox15,
												*m_checkBox16;

		wxChoice								*m_choice1;
	
		wxStaticText 						*m_staticText313,
												*m_staticText314,
												*m_staticText315,
												*m_staticText316,
												*m_staticText317,
												*m_staticText318,
												*m_staticText319,
												*m_staticText3141,
												*m_staticText3151,
												*m_staticText3161,
												*m_staticText3171,
												*m_staticText3181,
												*m_staticText3191;

		wxTextCtrl 							*m_textCtrl182,
												*m_textCtrl183,
												*m_textCtrl1821,
												*m_textCtrl1831;
	
		double								m_doubleValueCheck,
												m_newColumnEnd,
												m_newColumnStart,
												m_newLineEnd,
												m_newLineStart,
												m_realValue;
	
		int									m_listSelectionUnits;
	
		unsigned long						m_uInt32ValueCheck;


		SInt16								m_pointType,
												m_selectionUnits,
												m_unitsDisplayCode;
	
		bool									m_applyToAllWindowsFlag,
												m_useStartLineColumnFlag;

		Boolean								m_coordinateUnitsChangingFlag,
												m_initializedFlag,
												m_previewWasUsedFlag,
												m_stringID,
												m_valueChangedFlag;

};	// end "class CMEditCoordinatesDlg"
