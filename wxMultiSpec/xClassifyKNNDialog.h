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
//	File:               	xClassifyKNNDialog.h
//	Implementation:      xClassifyKNNDialog.cpp
//
//	Authors:					Tsung Tai Yeh
//								Larry L. Biehl
//
//	Language:            C++
//
//	System:              Linux Operating System
//
//	Brief description:   Header file for the CMKNNClassifyDialog class
//
//	Written By:         	Tsung Tai Yeh         Date: 04/??/2019
// Revised By:          Larry L. Biehl        Date: 08/09/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMKNNClassifyDialog : public CMDialog
	{
	public:
		CMKNNClassifyDialog (
				wxWindow*							parent = NULL,
				wxWindowID           			id = IDD_KNNParameters,
				const wxString&					title =
														wxT("Specify KNN Classifier Parameters"));
		
		Boolean DoDialog (
				SInt16*								nearestNeighborKValuePtr);
		
			// Implementation
	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	
		wxString 							m_kValueString;
		
		wxBoxSizer							*bSizer166;
		
		wxStaticText						*m_staticText183,
												*m_staticText184;
		
		wxTextCtrl							*m_kValueControl;
	
		SInt16								m_nearestNeighborKValue;
	
		bool									m_initializedFlag;
		
};	// end "class CMKNNClassifyDialog"
