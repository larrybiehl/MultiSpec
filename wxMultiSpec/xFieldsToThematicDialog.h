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
//	File:						xFieldsToThematicDialog.h
//	Implementation:		xFieldsToThematicDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMFieldsToThematicDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 02/24/2020
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "xDialog.h"


class CMFieldsToThematicDialog : public CMDialog
{
			// Construction
	public:
				// standard constructor
		CMFieldsToThematicDialog (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = IDD_FeatureExtraction,
				const wxString& 					title =
															wxT("Project Areas to Thematic Image"));
		
		Boolean DoDialog (
				ReformatOptionsPtr				reformatOptionsPtr);

	
		enum {IDD = IDD_AreasToThematicImage};

			// Implementation
	protected:
	
				// Generated message map functions

		void OnInitDialog (
				wxInitDialogEvent& 				event);    
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
		ReformatOptionsPtr				m_reformatOptionsPtr;
			  
		wxChoice								*m_fileFormatCtrl;
	
		wxCheckBox							*m_checkBox66,
												*m_checkBox67;
	
		wxStaticText						*m_staticText282,
												*m_staticText283,
												*m_staticText284,
												*m_staticText285;
	
		int									m_outputFormatCode;

		Boolean								m_initializedFlag,
												m_testingAreasFlag,
												m_trainingAreasFlag;
	
};	// end "class CMFieldsToThematicDialog"
