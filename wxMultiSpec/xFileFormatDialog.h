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
//	File:						xFileFormatDialog.h
//	Implementation:		xFileFormatDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMFileFormatSpecsDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 02/24/2020
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"

#define	IDC_FillDataValueExists			29
#define	IDC_FillDataValue					30


class CMFileFormatSpecsDlg : public CMDialog
{
			// Construction
	public:
				// standard constructor
		CMFileFormatSpecsDlg (
				wxWindow* 							pParent = NULL,
    			wxWindowID 							id = wxID_ANY,
    			const wxString& 					title =
    										wxT("Set Multispectral File Format Specifications"));
	
		~CMFileFormatSpecsDlg ();
    
		Boolean DoDialog (
				Handle 								fileInfoHandle,
				Handle 								windowInfoHandle,
				Handle* 								mapInformationHandlePtr,
				Handle* 								hfaHandlePtr,
				Handle* 								newChannelToHdfDataSetHandlePtr,
				Boolean* 							parameterChangedFlagPtr);


		enum {IDD = IDD_FileFormatSpecs};
	
		void* 								m_menuClientData;
	

			// Implementation
	protected:
		void OnActivate (
				wxActivateEvent& 					event);
	
		void OnBnClickedHdfdatasethelp (
				wxCommandEvent& 					event);
	
		void OnBnClickedLinebottomtotop (
				wxCommandEvent& 					event);
	
		void OnBnClickedFillDataValueExists (
				wxCommandEvent& 					event);
	
		void OnDetermineNumClasses (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnPaint (
				wxPaintEvent& 						event);
	
		void OnSelendokBandInterleave (
				wxCommandEvent& 					event);
	
		void OnSelendokDataValueType (
				wxCommandEvent& 					event);
	
		void OnSelendokHDFDataSet (
				wxCommandEvent& 					event);
	
		void OnShow (
				wxShowEvent& 						event);
	
		void OnStnClickedHdfdatasetprompt (
				wxCommandEvent& 					event);
	
		void ShowMultipleDataSetMessage (void);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	
		wxString 							m_imageName,
												m_stringCheck;

		FileInfoPtr							m_fileInfoPtr;
		WindowInfoPtr						m_windowInfoPtr;
	
		wxBoxSizer							*bSizer47;

		wxButton								*m_button7,
												*m_button8,
												*m_button9,
												*m_button10;

		wxCheckBox							*m_checkBox2,
												*m_checkBox3,
												*m_checkBox4,
												*m_checkBox5;

		wxChoice								*m_interleaveCtrl,
												*m_valueTypeCtrl,
												*m_collapseCtrl,
												*m_hdfDataSetCtrl;

		wxStaticText						*m_staticText60,
												*m_staticText80,
												*m_staticText81,
												*m_staticText82,
												*m_staticText83,
												*m_staticText89,
												*m_staticText90,
												*m_staticText91,
												*m_staticText92,
												*m_staticText93,
												*m_staticText94,
												*m_staticText95,
												*m_staticText96,
												*m_staticText97,
												*m_staticText98,
												*m_staticText99,
												*m_staticText100,
												*m_staticText101,
												*m_staticText102,
												*m_staticText103;
	
		wxTextCtrl							*m_textCtrl35,
												*m_textCtrl41,
												*m_textCtrl42,
												*m_textCtrl43,
												*m_textCtrl44,
												*m_textCtrl45,
												*m_textCtrl46,
												*m_textCtrl47,
												*m_textCtrl48,
												*m_textCtrl49,
												*m_textCtrl50,
												*m_textCtrl51;
	
		double 								m_fillDataValue;

		Handle								m_fileInfoHandle,
												m_hfaHandle,
												m_mapInformationHandle,
												m_newChannelToHdfDataSetHandle,
												m_windowInfoHandle;
	
		int 									m_bandInterleave,
		 	 									m_collapseClassSelection,
		 	 									m_dataValueType,
		 	 									m_hdfDataSetSelection;
	
		SInt32 								m_startColumnNumber,
		 	 									m_startLineNumber;
	
		UInt32 								m_blockHeight,
		 	 									m_blockWidth,
		 	 									m_headerBytes,
												m_maxNumberChannelsClasses,
												m_numberChannels,
												m_numberColumns,
												m_numberLines,
												m_postChannelBytes,
												m_postLineBytes,
												m_preChannelBytes,
												m_preLineBytes,
												m_trailerBytes;

		SInt16								m_dataSetIndex,
		 	 									m_eightBitsPerDataSelection,
												m_gdalDataTypeCode;

		UInt16								m_dataCompressionCode;

		Boolean								m_callGetHDFLineFlag,
												m_computeNumClasses,
												m_fillDataValueExistsFlag,
												m_forceGroupTableUpdateFlag,
												m_initializedFlag,
												m_linesBottomToTopFlag,
												m_onShowCalledFlag,
												m_swapBytesFlag;
	
};	// end "class CMFileFormatSpecsDlg"
