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
//	File:						xMosaicTwoImagesDialog.h
//	Implementation:		xMosaicTwoImagesDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMMosaicTwoImagesDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/07/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"

#include "xDialog.h"
#include "xOneColumnDialog.h"
#include "wx/wx.h"


class CMMosaicTwoImagesDialog : public CMDialog
{
	public:
				// standard constructor
		CMMosaicTwoImagesDialog (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = wxID_ANY,
				const wxString& 					title =
													wxT("Set Mosaic Images Specifications"));

		~CMMosaicTwoImagesDialog ();

		Boolean DoDialog (
				FileInfoPtr     					fileInfoPtr,
				FileInfoPtr							outFileInfoPtr,
				ReformatOptionsPtr 				reformatOptionsPtr);

		enum { IDD = IDD_Mosaic };
	
	protected:
		void OnBnClickedIgnoreBackgroundValue (
				wxCommandEvent& 					event);
	
		void OnCbnSelChangeHeaderFormatList (
				wxCommandEvent& 					event);
	
		void OnCbnSelChangeMosaicDirection (
				wxCommandEvent& 					event);
	
		void OnCbnSelChangeRightImageFileList (
				wxCommandEvent& 					event);
	
		void OnCbnSelChangeBottomImageFileList (
				wxCommandEvent& 					event);
	
		void OnEnChangeLeftTopLineEnd (
				wxCommandEvent& 					event);
	
		void OnEnChangeLeftTopLineStart (
				wxCommandEvent& 					event);
	
		void OnEnChangeLeftTopColumnEnd (
				wxCommandEvent& 					event);
	
		void OnEnChangeLeftTopColumnStart (
				wxCommandEvent& 					event);
	
		void OnEnChangeRightLineStart2 (
				wxCommandEvent& 					event);
	
		void OnEnChangeBottomColumnStart3 (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void ToEntireImage (
				wxCommandEvent& 					event);
	
		void ToSelectedImage (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ();
	
		void CheckBottomImageColumns (
				UInt16  								itemSelected);
	
		void CheckRightImageLines (
				UInt16  								itemSelected);
	
		void CreateControls ();
	
		bool TransferDataToWindow ();
	
		bool TransferDataFromWindow ();
	
	
		DialogSelectArea					m_rightBottomDialogSelectArea;
	
		FileInfoPtr         				m_fileInfoPtr,
			         						m_outputFileInfoPtr;
	
		ReformatOptionsPtr				m_reformatOptionsPtr;
	
		wxBitmapButton						*m_bpButton51,
												*m_bpButton52;

		wxBoxSizer							*bSizer119;
	
		wxCheckBox							*m_checkBox21,
												*m_checkBox22;
	
	
		wxChoice								*m_bottomListCtrl,
												*m_directionCtrl,
												*m_fileFormatCtrl,
												*m_rightListCtrl;
	
		wxStaticText						*m_staticText140,
												*m_staticText148,
												*m_staticText248,
												*m_staticText249,
												*m_staticText250,
												*m_staticText251,
												*m_staticText290,
												*m_staticText291,
												*m_staticText303,
												*m_staticText304,
												*m_staticText305,
												*m_staticText306,
												*m_staticText307,
												*m_staticText308,
												*m_staticText309,
												*m_staticText310,
												*m_staticText311,
												*m_staticText312,
												*m_staticText313,
												*m_staticText314;
	
		wxTextCtrl							*m_textCtrl136,
												*m_textCtrl137,
												*m_textCtrl138,
												*m_textCtrl139,
												*m_textCtrl141,
												*m_textCtrl142,
												*m_textCtrl143,
												*m_textCtrl144,
												*m_textCtrl145,
												*m_textCtrl146,
												*m_textCtrl147,
												*m_textCtrl149,
												*m_textCtrl150;
	
		Handle              				m_rightBottomWindowInfoHandle;

		int                 				m_bottomFileNameSelection,
												m_headerListSelection,
												m_mosaicDirectionCode,
			                				m_rightFileNameSelection;
	
		SInt32              				m_backgroundValue,
			              					m_maxDataValue,
			 									m_maxNumberColumns,
			 									m_maxNumberLines;
	
		SInt16								m_headerOptionsSelection;

		Boolean								m_initializedFlag;
	
		bool									m_ignoreBackgroundValueFlag,
												m_writeChannelDescriptionsFlag;

};	// end "class CMMosaicTwoImagesDialog"
