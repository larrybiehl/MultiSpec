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
//	File:               	xClassifyParallelPipedDialog.h
//	Implementation:      xClassifyParallelPipedDialog.cpp
//
//	Authors:					Tsung Tai Yeh
//								Larry L. Biehl
//
//	Language:            C++
//
//	System:              Linux Operating System
//
//	Brief description:   Header file for the CMClassifyParallelPipedDialog class
//
//	Written By:         	Tsung Tai Yeh         Date: 02/10/2025
// Revised By:          Larry L. Biehl        Date: 02/17/2025
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMClassifyParallelPipedDialog : public CMDialog
	{
	public:
		CMClassifyParallelPipedDialog (
				wxWindow*							parent = NULL,
				wxWindowID           			id = IDD_ParallelPipedParameters,
				const wxString&					title =
														wxT("Specify PP Classifier Parameter"));
		
		Boolean DoDialog (
				SInt16*								parallelPipedCodePtr,
				double*								stdDeviationValuePtr);
		
			// Implementation
	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	
		wxBoxSizer							*bSizer161;
		
		wxRadioButton						*m_radioBtn11,
												*m_radioBtn12;
		
		wxStaticText						*m_staticText175;
		
		wxString								m_StdDeviationString;
		
		wxTextCtrl							*m_kValueControl;
	
		UInt16								m_parallelPipedLimitCode;
		
		double								m_stdDeviationValue;
	
		bool									m_initializedFlag;
		
};	// end "class CMClassifyParallelPipedDialog"
