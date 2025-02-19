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
//	File:						xReformatCompareImagesDialog.h
//	Implementation:		xReformatCompareImagesDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMReformatRectifyDlg class
//
//	Written By:				Larry L. Biehl			Date: 05/14/2022
//	Revised By:				Larry L. Biehl			Date: 08/26/2022
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"
#include "xOneColumnDialog.h"
#include "SMultiSpec.h"
#include "wx/wx.h"


class CMReformatCompareImagesDlg : public CMDialog
{
			// ConstructionTransform
	public:
				// standard constructor
		CMReformatCompareImagesDlg (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = wxID_ANY,
				const wxString& 					title =
														wxT("Set Compare Images Parameters"));

		~CMReformatCompareImagesDlg ();

		Boolean DoDialog (
				FileInfoPtr 						outFileInfoPtr,
				FileInfoPtr 						fileInfoPtr,
				WindowInfoPtr 						imageWindowInfoPtr,
				LayerInfoPtr 						imageLayerInfoPtr,
				ReformatOptionsPtr				reformatOptionsPtr);

				// Dialog Data
		enum { IDD = IDD_ReformatRectify };
	
			// Implementation
	protected:
		void OnBnClickedCreateOutputImageFile (
				wxCommandEvent& 					event);
				
		void OnCbnSelendokCompareAlgorithmMethod (
				wxCommandEvent& 					event);
	
		void OnCbnSelendokTargetCombo (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnSelendokOutChannels (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ();
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();

		void UpdateProcedureItems (
				int   								selectItemNumber,
				Boolean								blankOutsideSelectedAreaFlag);
	
	
		const wxString 					title;
	
		FileInfoPtr							m_fileInfoPtr,
												m_outputFileInfoPtr;
	
		LayerInfoPtr						m_imageLayerInfoPtr;
		RefCompareImagesOptionsPtr    m_compareImagesOptionsPtr;
		ReformatOptionsPtr				m_reformatOptionsPtr;
		WindowInfoPtr						m_imageWindowInfoPtr;

		wxBoxSizer							*bSizer119,
												*bSizer120;
	
		wxCheckBox							*m_checkBox18,
												*m_checkBox20;
	
		wxChoice								*m_algorithmCtrl,
												*m_fileFormatCtrl,
												*m_referenceListCtrl;
	
		wxStaticText						*m_staticText361,
												*m_staticText362,
												*m_staticText363,
												*m_staticText364,
												*m_staticText366,
												*m_staticText378,
												*m_staticText380,
												*m_staticText381,
												*m_staticText382,
												*m_staticText402,
												*m_staticText403,
												*m_staticText404,
												*m_staticText410,
												*m_staticText411,
												*m_staticText413,
												*m_staticText414;
	
		wxTextCtrl							*m_textCtrl144,
												*m_textCtrl145,
												*m_textCtrl146,
												*m_textCtrl147,
												*m_textCtrl149,
												*m_textCtrl154,
												*m_textCtrl156,
												*m_textCtrl157,
												*m_textCtrl158,
												*m_textCtrl159,
												*m_textCtrl160,
												*m_textCtrl161;
												
		Handle								m_compareImageWindowInfoHandle;
	
		int     								m_channelSelection,
												m_compareAlgorithmSelection,
			     								m_fileNamesSelection,
			    								m_headerListSelection,
												m_kArcViewMenuItem,
												m_kERDAS74MenuItem,
												m_kGAIAMenuItem,
												m_kMatlabMenuItem,
												m_kNoneMenuItem,
												m_kTIFFGeoTIFFMenuItem;
	
		long    								m_columnShift,
			    								m_lineShift;

		SInt16								m_headerOptionsSelection,
												m_compareAlgorithmCode;
												
		bool    								m_createOutputImageFileFlag;

		Boolean								m_initializedFlag;
	
};	// end "class CMReformatCompareImagesDlg"
