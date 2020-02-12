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
//	File:						xRecodeThematicImageDialog.h
//	Implementation:		xRecodeThematicImageDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMRecodeThematicDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/08/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMRecodeThematicDialog : public CMDialog
{
			// Construction
	public:
		CMRecodeThematicDialog (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = wxID_ANY,
				const wxString& 					title =
															wxT("Set Thematic Recode Parameters"));

		~CMRecodeThematicDialog (); // standard desctructor

		Boolean DoDialog (
				RecodeThematicImagePtr  		recodeThematicImagePtr);
	
				// Dialog Data
		enum { IDD = IDD_ThematicRecode };


			// Implementation
	protected:
		virtual void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnSelendokThresholdFileCombo (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ();

		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();


		RecodeThematicImagePtr  		m_recodeThematicImagePtr;

		wxChoice								*m_operationCtrl,
												*m_thresholdFileCtrl;

		wxStaticText						*m_staticText339,
												*m_staticText340,
												*m_staticText341,
												*m_staticText342,
												*m_staticText343,
												*m_staticText344,
												*m_staticText345;

		wxTextCtrl							*m_textCtrl173,
												*m_textCtrl175;
	
		Handle                  		m_thresholdFileInfoHandle;
	
		int									m_compareParameterCode,
												m_thresholdImageSelection;
	
		SInt32								m_recodedValue,
												m_thresholdValue;
	
		Boolean                			m_initializedFlag;
	
};	// end "class CMRecodeThematicDialog"
